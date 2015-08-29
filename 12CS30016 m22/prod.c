#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <string.h>

#define fk 1001
#define ek 1002
#define msgk 1003
#define mutexk 1004

void up(int semid) {
	struct sembuf sop;
	sop.sem_num=0;
	sop.sem_op=1;
	sop.sem_flg=0;
	semop(semid,&sop,1);
}
void down(int semid) {
	struct sembuf sop;
	sop.sem_num=0;
	sop.sem_op=-1;
	sop.sem_flg=0;
	semop(semid,&sop,1);
}

typedef struct $
{
	long mtype;
	char mtext[256];	
}message;

int main() {
	int i,j;
	
	int flag=IPC_CREAT|0666;
	int mutex=semget(mutexk,1,flag);
	int full =semget(fk,1,flag);
	int empty=semget(ek,1,flag);
	int msg=msgget(msgk,flag);
	semctl(mutex,0,SETVAL,1);
	semctl(full,0,SETVAL,0);
	semctl(empty,0,SETVAL,10);
	while(1) {
		message m1,m2;
		down(empty);
		i=rand()%101;
		m1.mtype=1;
		sprintf(m1.mtext,"%d",i);
		down(mutex);
		if(semctl(full,0,GETVAL,0)==10) {
			up(mutex);
		}
		else {
			msgsnd(msg,&m1,strlen(m1.mtext)+1,0);
			up(full);
			printf("Producer has added %d to buffer\n",i );
			printf("full=%d\nempty=%d\nmutex=%d\n",
			 semctl(full,0,GETVAL,0),
			 semctl(empty,0,GETVAL,0),
			 semctl(mutex,0,GETVAL,0));
			up(mutex);
			sleep(rand()%5+1);
		}
	}
	return 0;


}