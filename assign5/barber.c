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
	int n,barber,customer,mutex,freeseats;
	printf("Enter Number of chairs for customers: ");
	scanf("%d",&n);

	barber=semget((key_t)BARBER,1,IPC_CREAT|0666);
	customer=semget((key_t)CUSTOMER,1,IPC_CREAT|0666);
	mutex=semget((key_t)MUTEX,1,IPC_CREAT|0666);
	freeseats=semget((key_t)FREESEATS,1,IPC_CREAT|0666);

	if(semctl(mutex,0,SETVAL,1)==-1)
		perror("mutex semctl()");

	if(semctl(barber,0,SETVAL,0)==-1)
		perror("barber semctl()");

	if(semctl(customer,0,SETVAL,0)==-1)
		perror("customer semctl()");

	if(semctl(freeseats,0,SETVAL,n)==-1)
		perror("freeseats semctl()");


	while(1){
		/*checking if customers present*/
		printf("Checking if customers are present\n");
		down(customer);
		/*acquiring mutex*/
		down(mutex);
		/*entering critical region*/
		int seats;
		if((seats=semctl(freeseats,0,GETVAL,0))==-1)
			perror("barber semctl()");
		if(semctl(freeseats,0,SETVAL,seats+1)==-1)
				perror("waiting semctl()");
		up(barber);
		printf("Barber ready for haircut\n");
		/*leaving critical region*/
		up(mutex);
		/*releasing mutex*/
	}

	return 0;
}