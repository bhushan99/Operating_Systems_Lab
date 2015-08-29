#include <stdio.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#define MSGQ 1
#define SHM_PID 2
#define SHM_MSG 3
#define SEM_PID 4
#define SEM_MSG 5
#define PID_SIZE 500
#define MSG_SIZE 5000


char *msg,user_msg[MSG_SIZE];
int sem_msg;
int *pid;
pid_t child;


struct msgbuf{
	long mtype;
	char mtext[MSG_SIZE];
};

void wait1(int sem_id){
	struct sembuf sops;
	sops.sem_num = 0;
	sops.sem_op = -1;
	sops.sem_flg = 0;
	semop(sem_id,&sops,1);
}

void signal1(int sem_id){
	struct sembuf sops;
	sops.sem_num = 0;
	sops.sem_op = 1;
	sops.sem_flg = 0;
	semop(sem_id,&sops,1);
}

void waitz(int sem_id){
	struct sembuf sops;
	sops.sem_num = 0;
	sops.sem_op = -2;
	sops.sem_flg = 0;
	semop(sem_id,&sops,1);
}

void signalz(int sem_id){
	struct sembuf sops;
	sops.sem_num = 0;
	sops.sem_op = -1;
	sops.sem_flg = 0;
	semop(sem_id,&sops,1);
}

void terminate(){
	if(shmdt(msg)==-1)
		perror("msgshm");
	shmdt(pid);
	printf("receiver exit\n");
	exit(1);
}

void user(){
	waitz(sem_msg);
	char process_pid[50],input_msg[MSG_SIZE];
	memset(user_msg,0,MSG_SIZE);
	sprintf(process_pid,"%d",getpid());
	printf("Ctrl+C pressed\n");
	printf("Enter message\n");
	gets(input_msg);
	strcpy(user_msg,getlogin());
	strcat(user_msg,"/");
	strcat(user_msg,process_pid);
	strcat(user_msg,":");

	if(strcmp(input_msg,"bye")==0){
		int k,count=0,index;
		for(k=1;k<=pid[0];k++){
			if(pid[k]!=-1)
				count++;
			if(pid[k]==getpid())
				index=k;
		}
		/*remove pid*/
		pid[index]=-1;
		if(count==1)
			strcat(user_msg,"*");
		else
			strcat(user_msg,".");
		memset(msg,0,MSG_SIZE);
		strcpy(msg,user_msg);
		signalz(sem_msg);
		kill(child,SIGUSR1);
		if(shmdt(msg)==-1)
			perror("msgshm");
		shmdt(pid);
		wait();
		exit(1);
	}
	else
		strcat(user_msg,input_msg);
	
	memset(msg,0,MSG_SIZE);
	strcpy(msg,user_msg);
	signalz(sem_msg);
	memset(user_msg,0,MSG_SIZE);
	strcpy(user_msg,getlogin());
	strcat(user_msg,"/");
	strcat(user_msg,process_pid);
	strcat(user_msg,":.");
}

int main(){
	int msgid,shm_pid,shm_msg,sem_pid,file;

	/*Initialising data structures*/
	if((msgid=msgget((key_t)MSGQ,IPC_CREAT|0666))==-1){
		perror("MSGGET");
		exit(1);
	}
	if((sem_pid=semget((key_t)SEM_PID,1,IPC_CREAT|0666))==-1){
		perror("SEM_PID GET");
		exit(1);
	}
	if((sem_msg=semget((key_t)SEM_MSG,1,IPC_CREAT|0666))==-1){
		perror("SEM_MSG GET");
		exit(1);
	}
	if((shm_pid=shmget((key_t)SHM_PID,500,IPC_CREAT|0666))==-1){
		perror("SHM_PID GET");
		exit(1);
	}
	if((shm_msg=shmget((key_t)SHM_MSG,5000,IPC_CREAT|0666))==-1){
		perror("SHM_MSG GET");
		exit(1);
	}

	if((pid=(int*)shmat(shm_pid,NULL,0))==NULL){
		perror("SHMAT PID");
		exit(1);
	}
	if((msg=(char*)shmat(shm_msg,NULL,0))==NULL){
		perror("SHMAT MSG");
		exit(1);
	}

	if((file=open("ser.txt",O_CREAT|O_EXCL))!=-1){
		printf("File ser.txt does not exists\n");
		exit(1);
	}

	child=fork();
	if(child==0){

		signal(SIGINT,SIG_IGN);
		signal(SIGUSR1,terminate);
		wait1(sem_pid);
		/*add parent pid to shared pid list*/
		pid[pid[0]+1]=getppid();
		pid[0]++;
		signal1(sem_pid);
		while(1){
			struct msgbuf msgp;
			if(msgrcv(msgid,&msgp,(size_t)MSG_SIZE,getppid(),0)==-1)
				perror("msgrcv");
			printf("Message received %s\n",msgp.mtext);
		}
	}
	else{
		char process_pid[10],login[50],user_msg[MSG_SIZE];
		sprintf(process_pid,"%d",getpid());
		memset(user_msg,0,MSG_SIZE);
		memset(msg,0,MSG_SIZE);
		strcpy(user_msg,getlogin());
		strcat(user_msg,"/");
		strcat(user_msg,process_pid);
		strcat(user_msg,":.");

		signal(SIGINT,user);
		while(1){}
	}
	return 0;
}