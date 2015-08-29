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
#include <sys/ipc.h>
#include <string.h>



// PROBLEM: sorting half doing well, but some problem in pipe 
// all fine running for 4 elements


#define MAX_SIZE 256

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

int main() {
	int flag=IPC_CREAT|0666;
	int semid=semget((key_t)10000,1,flag);
	semctl(semid,0,SETVAL,1);
	int n,i,j,fd[2][2],index;
	scanf("%d",&n);
	char buf[MAX_SIZE],buf5[MAX_SIZE];
	buf[0]=buf5[0]='\0';
	int a[n],b[n];
	for(i=0;i<n;i++) {
		scanf("%d",&a[i]);
	}
	int loop;
	for(loop=0;loop<2;loop++){
		index=loop+1;
		pipe(fd[loop]);

		if(fork()==0) {
			
			close(fd[i][0]);
			if(index==1) {
				char buf1[MAX_SIZE];
				buf1[0]='\0';
				down(semid);
				printf("First half: " );
				int temp,mn,mnp;
				for(i=0;i<n/2-1;i++) {
					mn=a[i];mnp=i;
					for(j=i+1;j<n/2;j++) {
						if(a[j]<mn) {mn=a[j];mnp=j;}
					}
					temp=a[i];
					a[i]=a[mnp];
					a[mnp]=temp;					
				}
				for(i=0;i<n/2;i++) {

					printf("%d ", a[i]);
					char c[10];
					sprintf(c,"%d ",a[i]);
					strcat(buf1,c);
				}
				printf("\n");
				//printf("\n\nbuf1=%s\n",buf1 );
				write(fd[loop][1],buf1,MAX_SIZE);
				//close(fd[i][1]);
				up(semid);
				exit(0);
			}
			else {
				char buf2[MAX_SIZE];
				buf2[0]='\0';
				down(semid);
				printf("Second half: " );
				int el,temp,mn,mnp;
				for(i=n/2;i<n-1;i++) {
					mn=a[i];mnp=i;
					for(j=i+1;j<n;j++) {
						if(a[j]<mn) {mn=a[j];mnp=j;}
					}
					temp=a[i];
					a[i]=a[mnp];
					a[mnp]=temp;					
				}
				for(i=n/2;i<n;i++) {

					printf("%d ", a[i]);
					char c[10];
					sprintf(c,"%d ",a[i]);
					strcat(buf2,c);
				}
				printf("\n");

				write(fd[loop][1],buf2,MAX_SIZE);
				//close(fd[i][1]);
				up(semid);
				exit(0);
			}
		}else{
			close(fd[i][1]);
		}
	}
	for(j=0;j<2;j++) wait(&i);
	read(fd[0][0],buf,MAX_SIZE);
	close(fd[0][0]);
	//close(fd[0][1]);
	//printf("\n\nbuf2=%s\n",buf );
	int l=strlen(buf);
	int k=0,num=0;
	for(i=0;i<l;i++) {
		if(buf[i]!=' ') num=num*10+(int)(buf[i]-'0');
		else {a[k++]=num;num=0;}
	}
	read(fd[1][0],buf5,MAX_SIZE);
	close(fd[1][0]);
	//printf("\n\nbuf2=%s\n",buf5 );
	l=strlen(buf5);
	num=0;
	for(i=0;i<l;i++) {
		if(buf5[i]!=' ') num=num*10+(int)(buf5[i]-'0');
		else {a[k++]=num;num=0;}
	}
	i=0;j=n/2;k=0;
	while(i<n/2&&j<n) {
		if(a[i]<a[j]) b[k++]=a[i++];
		else b[k++]=a[j++];
	}
	while(i<n/2) b[k++]=a[i++];
	while(j<n) b[k++]=a[j++];

	printf("whole array: ");
	for(i=0;i<n;i++) printf("%d ", b[i]);
	printf("\n");

 
 	return 0;
}