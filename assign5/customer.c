#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <sys/ipc.h> 
#include <sys/sem.h>

#define BARBER 1
#define CUSTOMER 2
#define MUTEX 3
#define FREESEATS 4

void up(int semid) {
	struct sembuf sop;
	sop.sem_num=0;
	sop.sem_op=1;
	sop.sem_flg=0;
	semop(semid, &sop, 1);
}

void down(int semid) {
	struct sembuf sop;
	sop.sem_num=0;
	sop.sem_op=-1;
	sop.sem_flg=0;
	semop(semid, &sop, 1);
}

int main(){
	int barber,customer,mutex,n,i,freeseats;

	printf("enter number of customers:\n");
	scanf("%d",&n);
	barber=semget((key_t)BARBER,1,IPC_CREAT|0666);
	customer=semget((key_t)CUSTOMER,1,IPC_CREAT|0666);
	mutex=semget((key_t)MUTEX,1,IPC_CREAT|0666);
	freeseats=semget((key_t)FREESEATS,1,IPC_CREAT|0666);

	for(i=0;i<n;i++){
		if(fork()==0){
			pid_t pid=getpid();
			/*acquiring mutex*/
			down(mutex);
			/*entering critical region*/
			printf("%d:Customer enters the shop\n",pid);
			/*customer checks number of waiting customers and leaves if no of waiting customers = no of chairs*/
			int seats;
			if((seats=semctl(freeseats,0,GETVAL,0))==-1)
				perror("customer semctl()");
			
			if(seats<=0){
				printf("%d:No place. Customer leaves the shop\n",pid);
				up(mutex);
				/*releasing mutex*/
				exit(1);
			}
			if(semctl(freeseats,0,SETVAL,seats-1)==-1)
				perror("waiting semctl()");
			up(customer);
			printf("%d,%d:Customer added to waitlist\n",pid,seats);
			/*leaving critical section*/
			up(mutex);
			/*releasing mutex*/

			/*waking up barber if he is idle*/
			down(barber);

			printf("%d:Customer leaves shop after having a haircut\n",pid);
			exit(1);
		}
	}
	for(i=0;i<n;i++)
		wait();

	return 0;
}

