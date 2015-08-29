#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <string.h>
#include <utmp.h>
#include <pwd.h>
#include <fcntl.h>

#define MSGQ 1
#define SHM_PID 2
#define SHM_MSG 3
#define SEM_PID 4
#define SEM_MSG 5
#define PID_SIZE 500
#define MSG_SIZE 5000

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

void wait2(int sem_id){
	struct sembuf sops;
	sops.sem_num = 0;
	sops.sem_op = 0;
	sops.sem_flg = 0;
	semop(sem_id,&sops,1);
}

void signal2(int sem_id){
	struct sembuf sops;
	sops.sem_num = 0;
	sops.sem_op = 3;
	sops.sem_flg = 0;
	semop(sem_id,&sops,1);
}

int main(int argc,char **argv){
	int msgid,shm_pid,shm_msg,sem_pid,sem_msg,file;
	char *msg;
	int *pid;
	struct utmp u;
	FILE *fp;
	int login_file;

	fp = fopen("/var/run/utmp","r");

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
	printf("%d\n",shm_msg);
	if((pid=(int*)shmat(shm_pid,NULL,0))==NULL){
		perror("SHMAT PID");
		exit(1);
	}
	pid[0]=0;
	if((msg=(char*)shmat(shm_msg,NULL,0))==NULL){
		perror("SHMAT MSG");
		exit(1);
	}

	semctl(sem_pid,0,SETVAL,1);
	semctl(sem_msg,0,SETVAL,3);

	if((file=open("ser.txt",O_CREAT|O_EXCL))==-1){
		perror("File ser.txt exists");
		exit(1);
	}
	/*writing server pid to ser.txt*/
	char server_pid[10];
	sprintf(server_pid,"%d",getpid());
	write(file,server_pid,10);

	printf("Initialisation complete\n");

	while(fread(&u,sizeof(u),1,fp)){
		if(u.ut_name && getpwnam(u.ut_name)){
			int i;
			if(strcmp(u.ut_line,":0") && u.ut_pid){
				for(i=0;i<argc;i++){
					if(strcmp(u.ut_line,argv[i])==0){
						char name[200];
						printf("Sending commencement message to /dev/%s\n",u.ut_line);
						sprintf(name,"./commence > /dev/%s",u.ut_line);
						system(name);
						break;
					}
				}
			}
		}
	}

	while(1){
		char recv_msg[MSG_SIZE];
		wait2(sem_msg);
		/*read msg from shared msg memory*/
		strcpy(recv_msg,msg);
		signal2(sem_msg);

		int len = strlen(recv_msg);
		char pid_sender[50],login_name[50];
		int k=0,j=0;
		while(recv_msg[k]!='/') k++;
		strncpy(login_name,recv_msg,k);
		k++;
		j=k;
		while(recv_msg[k]!=':') k++;
		strncpy(pid_sender,recv_msg+j,k-j);
		k++;
		if(recv_msg[k]=='.' && k==len-1)
			continue;
		else if(recv_msg[k]=='*')
			break;
		else if(strcmp(recv_msg,"")==0)
			continue;
		wait1(sem_pid);
		struct msgbuf msgp;
		strcpy(msgp.mtext,recv_msg);
		printf("Received message %s\n",recv_msg);
		for(k=1;k<=pid[0];k++){
			if(pid[k]!=atoi(pid_sender) && pid[k]!=-1 && atoi(pid_sender)){
				printf("Sending message to pid %d\n",pid[k]);
				msgp.mtype = pid[k];
				if(msgsnd(msgid,&msgp,MSG_SIZE,0)==-1)
					perror("msgsnd");
			}
		}
		signal1(sem_pid);
	}


	if(msgctl(msgid,IPC_RMID,(struct msqid_ds *)NULL)==-1)
		perror("msgid rm");
	if(shmdt(msg)==-1)
		perror("msgshm rm");
	if(shmdt(pid)==-1)
		perror("pidshm rm");
	if(semctl(sem_pid,0,IPC_RMID)==-1)
		perror("sempid rm");
	if(semctl(sem_msg,0,IPC_RMID)==-1)
		perror("semmsg rm");
	if(remove("ser.txt")==-1)
		perror("remove");

	return 0;
}