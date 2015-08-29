#include "varibles.h"

union semun {
               int val;
               struct semid_ds *buf;
               ushort *array;
          	} arg;

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

void z_up(int semid) {
	semctl(semid,0,SETVAL,0);
}

void waitz(int semid) {
	semctl(semid,0,SETVAL,1);
	struct sembuf sop;
	sop.sem_num=0;
	sop.sem_op=0;
	sop.sem_flg=0;
	semop(semid, &sop, 1);
}

int main(){

	int nL,iter,i,index=-1,j;
	int sem_flg= IPC_CREAT | 0666;
	int msid = msgget((key_t)msg_key, sem_flg );
	printf("Enter number of lions:\n");
	scanf("%d",&nL);
	printf("Enter number of iterations each lion make:\n");
	scanf("%d",&iter);

	int mutex1=semget(mutex1_key,1,sem_flg);
	int mutex2=semget(mutex2_key,1,sem_flg);
	int mutex3=semget(mutex3_key,1,sem_flg);

	for(i=0;i<nL;i++) {
		if(fork()==0) {
			index=i+1;
			break;
		}
		else {
			if(i==nL-1) 
			{
				for(j=0;j<nL;j++) wait();
				exit(0);
			}
		}
	}
	
	int wL1=semget(wL1_key,1,sem_flg);
	int wL2=semget(wL2_key,1,sem_flg);
	int wL3=semget(wL3_key,1,sem_flg);
	int wR1=semget(wR1_key,1,sem_flg);
	int wR2=semget(wR2_key,1,sem_flg);
	int wR3=semget(wR3_key,1,sem_flg);
	int wJ1=semget(wJ1_key,1,sem_flg);
	int wJ2=semget(wJ2_key,1,sem_flg);
	int wJ3=semget(wJ3_key,1,sem_flg);
	int state1=semget(state1_key,1,sem_flg);
	int state2=semget(state2_key,1,sem_flg);
	int state3=semget(state3_key,1,sem_flg);
	int rem1=semget(rem1_key,1,sem_flg);
	int rem2=semget(rem2_key,1,sem_flg);
	int rem3=semget(rem3_key,1,sem_flg);
	int nJ1=semget(nJ1_key,1,sem_flg);
	int nJ2=semget(nJ2_key,1,sem_flg);
	int nJ3=semget(nJ3_key,1,sem_flg);
	int nL1=semget(nL1_key,1,sem_flg);
	int nL2=semget(nL2_key,1,sem_flg);
	int nL3=semget(nL3_key,1,sem_flg);
	int nR1=semget(nR1_key,1,sem_flg);
	int nR2=semget(nR2_key,1,sem_flg);
	int nR3=semget(nR3_key,1,sem_flg);
	
	int go=semget(goto_key,1,sem_flg);

	int val=0;
	
	message_buf buf;

	for(i=0;i<iter;i++) {
		if(i==iter-1) printf("%d\n",index );

		int rnum;
		if(val==0) rnum=rand()%3+1;
		else {rnum=val;val=0;}

		for(j=0;j<3;j++) {
			
			int eaten=0;
			int check=(rnum+j)%4;
			if(!check) check++;
			printf("Lion %d requesting food from meat-pit %d\n",index,check);

			switch(check){
				case 1:

				down(mutex1);

				if(semctl(rem1,0,GETVAL,0) &&
					(semctl(state1,0,GETVAL,0)==NONE||semctl(state1,0,GETVAL,0)==LION)) {

					printf("Lion %d in control of meat-pit %d\n",index,check );
					int v=semctl(nL1,0,GETVAL,0);
					v++;
					semctl(nL1,0,SETVAL,v);
					semctl(state1,0,SETVAL,LION);
					v=semctl(rem1,0,GETVAL,0);
					v--;
					semctl(rem1,0,SETVAL,v);
					
					eaten=1;
					up(mutex1);
					
					down(mutex1);
					v=semctl(nL1,0,GETVAL,0);
					v--;
					semctl(nL1,0,SETVAL,v);

					if(semctl(rem1,0,GETVAL,0) && v==0) {
					
						semctl(state1,0,SETVAL,NONE);
						
						semctl(go,0,SETVAL,1);
						up(mutex1);
						printf("Lion %d left meat pit %d\n",index,check);
						z_up(wJ1);z_up(wJ2);z_up(wJ3);
						z_up(wR1);z_up(wR2);z_up(wR3);
						printf("Lion %d giving signal to wait queue of all meat pit\n",index);
						
					
					} else {
						if(!semctl(nL1,0,GETVAL,0)) semctl(state1,0,SETVAL,NONE); 
						up(mutex1);
						printf("Lion %d left meat pit %d\n",index,check);
					}

				} else {

					up(mutex1);
					
					if(j!=2) printf("Lion %d denied access\n", index );
					else {
					
						if(semctl(rem1,0,GETVAL,0)) 
							printf("Other species has access to meat pit %d\n",check);
						else 
							printf("Meat pit %d empty\n",check);
						printf("Lion %d in wait queue of meat pit %d\n",index,check);
						char c[10];
						sprintf(c,"1%d",index);
						buf.mtype=1;
						strcpy(buf.mtext,c);
						msgsnd(msid, &buf, strlen(buf.mtext)+1, 0);
						down(mutex1);
						if(!semctl(nL1,0,GETVAL,0)&&semctl(state1,0,GETVAL,0)==LION) 
							semctl(state1,0,SETVAL,NONE); 
						up(mutex1);
						waitz(wL1);
						buf.mtype=2;
						strcpy(buf.mtext,c);
						msgsnd(msid, &buf, strlen(buf.mtext)+1, 0);
						val=semctl(go,0,GETVAL,0);
					
					}
				}
				break;
				
				case 2:
				
				down(mutex2);

				if(semctl(rem2,0,GETVAL,0) &&
					(semctl(state2,0,GETVAL,0)==NONE||semctl(state2,0,GETVAL,0)==LION)) {

					printf("Lion %d in control of meat-pit %d\n",index,check );
					int v=semctl(nL2,0,GETVAL,0);
					v++;
					semctl(nL2,0,SETVAL,v);
					semctl(state2,0,SETVAL,LION);
					v=semctl(rem2,0,GETVAL,0);
					v--;
					semctl(rem2,0,SETVAL,v);
					
					eaten=1;
					up(mutex2);
					down(mutex2);
					v=semctl(nL2,0,GETVAL,0);
					v--;
					semctl(nL2,0,SETVAL,v);

					if(semctl(rem2,0,GETVAL,0) && v==0) {
					
						semctl(state2,0,SETVAL,NONE);
						
						semctl(go,0,SETVAL,2);
						up(mutex2);
						printf("Lion %d left meat pit %d\n",index,check);
						z_up(wJ1);z_up(wJ2);z_up(wJ3);
						z_up(wR1);z_up(wR2);z_up(wR3);
						printf("Lion %d giving signal to wait queue of all meat pit\n",index);
				
					
					} else {
						if(!semctl(nL2,0,GETVAL,0)) semctl(state2,0,SETVAL,NONE); 
						up(mutex2);
						printf("Lion %d left meat pit %d\n",index,check);
					}

				} else {

					up(mutex2);
					
					if(j!=2) printf("Lion %d denied access\n", index );
					else {
					
						if(semctl(rem2,0,GETVAL,0)) 
							printf("Other species has access to meat pit %d\n",check);
						else 
							printf("Meat pit %d empty\n",check);
						printf("Lion %d in wait queue of meat pit %d\n",index,check);
						char c[10];
						sprintf(c,"1%d",index);
						buf.mtype=1;
						strcpy(buf.mtext,c);
						msgsnd(msid, &buf, strlen(buf.mtext)+1, 0);
						down(mutex2);
						if(!semctl(nL2,0,GETVAL,0) && semctl(state2,0,GETVAL,0)==LION)
						 	semctl(state2,0,SETVAL,NONE); 
						up(mutex2);
						waitz(wL2);
						buf.mtype=2;
						strcpy(buf.mtext,c);
						msgsnd(msid, &buf, strlen(buf.mtext)+1, 0);
						val=semctl(go,0,GETVAL,0);
					
					}
				}
				break;

				case 3:
				
				down(mutex3);

				if(semctl(rem3,0,GETVAL,0) &&
					(semctl(state3,0,GETVAL,0)==NONE||semctl(state3,0,GETVAL,0)==LION)) {

					printf("Lion %d in control of meat-pit %d\n",index,check );
					int v=semctl(nL3,0,GETVAL,0);
					v++;
					semctl(nL3,0,SETVAL,v);
					semctl(state3,0,SETVAL,LION);
					v=semctl(rem3,0,GETVAL,0);
					v--;
					semctl(rem3,0,SETVAL,v);
					
					eaten=1;
					up(mutex3);
					down(mutex3);
					v=semctl(nL3,0,GETVAL,0);
					v--;
					semctl(nL3,0,SETVAL,v);

					if(semctl(rem3,0,GETVAL,0) && v==0) {
					
						semctl(state3,0,SETVAL,NONE);
						
						semctl(go,0,SETVAL,3);
						up(mutex3);
						printf("Lion %d left meat pit %d\n",index,check);
						z_up(wJ1);z_up(wJ2);z_up(wJ3);
						z_up(wR1);z_up(wR2);z_up(wR3);
						printf("Lion %d giving signal to wait queue of all meat pit\n",index);
					
					
					} else {
						if(!semctl(nL3,0,GETVAL,0)) semctl(state3,0,SETVAL,NONE); 
						up(mutex3);
						printf("Lion %d left meat pit %d\n",index,check);
					}

				} else {

					up(mutex3);
					
					if(j!=2) printf("Lion %d denied access\n", index );
					else {
					
						if(semctl(rem3,0,GETVAL,0)) 
							printf("Other species has access to meat pit %d\n",check);
						else 
							printf("Meat pit %d empty\n",check);
						printf("Lion %d in wait queue of meat pit %d\n",index,check);
						char c[10];
						sprintf(c,"1%d",index);
						buf.mtype=1;
						strcpy(buf.mtext,c);
						msgsnd(msid, &buf, strlen(buf.mtext)+1, 0);
						down(mutex3);
						if(!semctl(nL3,0,GETVAL,0)&&semctl(state3,0,GETVAL,0)==LION) 
							semctl(state3,0,SETVAL,NONE); 
						up(mutex3);
						waitz(wL3);
						buf.mtype=2;
						strcpy(buf.mtext,c);
						msgsnd(msid, &buf, strlen(buf.mtext)+1, 0);
						val=semctl(go,0,GETVAL,0);
					
					}
				}
				break;
				
			}
			if(eaten) break;
		}
	}

	exit(0);

}
