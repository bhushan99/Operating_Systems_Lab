#include "varibles.h"

void append(node** head_ref, int new_state,int new_ind)
{
    node* new_node = (node*) malloc(sizeof(node));
    node *last = *head_ref;
    new_node->state  = new_state;
    new_node->ind=new_ind;
    new_node->next = NULL;
    if (*head_ref == NULL)
    {
       *head_ref = new_node;
       return;
    }
    while (last->next != NULL)
        last = last->next;
    last->next = new_node;
    return;    
}

void deleteNode(node **head_ref, int new_state,int new_ind)
{
    node* temp = *head_ref, *prev;
    if (temp != NULL && temp->state == new_state && temp->ind==new_ind)
    {
        *head_ref = temp->next;   
        free(temp);               
        return;
    }
    while (temp != NULL && (temp->state != new_state || temp->ind!=new_ind))
    {
        prev = temp;
        temp = temp->next;
    }
    if (temp == NULL) return;
    prev->next = temp->next;
 
    free(temp);  // Free memory
}

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

int main() {

	node *head=NULL;

	int sem_flg= IPC_CREAT | 0666;

	int mutex1=semget(mutex1_key,1,sem_flg);
	int mutex2=semget(mutex2_key,1,sem_flg);
	int mutex3=semget(mutex3_key,1,sem_flg);
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

	int msid = msgget((key_t)msg_key, sem_flg );

	message_buf rbuf;
	int i,j,iter;
	printf("Enter number of iterations Ranger makes:\n");
	scanf("%d",&iter);
	int val=0;
	while(iter--) {
		int rnum;
		if(val==0) rnum=rand()%3+1;
		else {rnum=val;val=0;}

		for(j=0;j<3;j++) {
			int check=(rnum+j)%4,eaten=0;
			if(!check) check++;

			switch(check){

				case 1:

				down(mutex1);

				if(semctl(rem1,0,GETVAL,0)<=40 && 
					(semctl(state1,0,GETVAL,0)==NONE||semctl(state1,0,GETVAL,0)==RANGER) ) {

					printf("Ranger in control of meat-pit %d\n",check );
					semctl(state1,0,SETVAL,RANGER);
					int v=semctl(rem1,0,GETVAL,0);
					v+=10;
					semctl(rem1,0,SETVAL,v);
					eaten=1;

					while(msgrcv(msid, &rbuf, 10,1,IPC_NOWAIT)!=-1) {
						append(&head,1+rbuf.mtext[0]-'1',atoi(&(rbuf.mtext[1]))); 
					}
					while(msgrcv(msid, &rbuf, 10,2,IPC_NOWAIT)!=-1) {
						deleteNode(&head,1+rbuf.mtext[0]-'1',atoi(&(rbuf.mtext[1]))); 
					}

					if(head) {
						if(head->state==LION) semctl(state1,0,SETVAL,LION);
						else semctl(state1,0,SETVAL,JACKAL);
					} 
					else semctl(state1,0,SETVAL,NONE);
					
					semctl(go,0,SETVAL,1);
					up(mutex1);
					printf("Ranger left meat pit %d\n",check);
					z_up(wL1);z_up(wL2);z_up(wL3);///////////////////////////?????????????????
					z_up(wJ1);z_up(wJ2);z_up(wJ3);
					printf("Ranger giving signal to wait queue of all meat pit\n");
					
					
				} else {
					up(mutex1);
					
					if(j!=2) printf("Ranger denied access\n");
					else {
					
						printf("Ranger in wait queue of meat pit %d\n",check);
						down(mutex1);
						if(!semctl(nR1,0,GETVAL,0)&&semctl(state1,0,GETVAL,0)==RANGER) 
							semctl(state1,0,SETVAL,NONE); 
						up(mutex1);
						waitz(wR1);
						val=semctl(go,0,GETVAL,0);
					
					}
				}
				break;
				
				case 2:
				
				down(mutex2);

				if(semctl(rem2,0,GETVAL,0)<=40 && semctl(state2,0,GETVAL,0)==NONE ) {

					printf("Ranger in control of meat-pit %d\n",check );
					semctl(state2,0,SETVAL,RANGER);
					int v=semctl(rem2,0,GETVAL,0);
					v+=10;
					semctl(rem2,0,SETVAL,v);
					eaten=1;

					while(msgrcv(msid, &rbuf, 10,1,IPC_NOWAIT)!=-1) {
						append(&head,1+rbuf.mtext[0]-'1',atoi(&(rbuf.mtext[1]))); 
					}
					while(msgrcv(msid, &rbuf, 10,2,IPC_NOWAIT)!=-1) {
						deleteNode(&head,1+rbuf.mtext[0]-'1',atoi(&(rbuf.mtext[1]))); 
					}

					if(head) {
						if(head->state==LION) semctl(state2,0,SETVAL,LION);
						else semctl(state2,0,SETVAL,JACKAL);
					} 
					else semctl(state2,0,SETVAL,NONE);
					
					semctl(go,0,SETVAL,2);
					up(mutex2);
					printf("Ranger left meat pit %d\n",check);
					z_up(wL1);z_up(wL2);z_up(wL3);
					z_up(wJ1);z_up(wJ2);z_up(wJ3);
					printf("Ranger giving signal to wait queue of all meat pit\n");
			
					
				} else {

					up(mutex2);
					
					if(j!=2) printf("Ranger denied access\n");
					else {
					
						printf("Ranger in wait queue of meat pit %d\n",check);
						down(mutex2);
						if(!semctl(nR2,0,GETVAL,0)&&semctl(state2,0,GETVAL,0)==RANGER) 
							semctl(state2,0,SETVAL,NONE); 
						up(mutex2);
						waitz(wR2);
						val=semctl(go,0,GETVAL,0);
					
					}
				}
				break;
				
				case 3:
				
				down(mutex3);

				if(semctl(rem3,0,GETVAL,0)<=40 && semctl(state3,0,GETVAL,0)==NONE ) {

					printf("Ranger in control of meat-pit %d\n",check );
					semctl(state3,0,SETVAL,RANGER);
					int v=semctl(rem3,0,GETVAL,0);
					v+=10;
					semctl(rem3,0,SETVAL,v);
					eaten=1;

					while(msgrcv(msid, &rbuf, 10,1,IPC_NOWAIT)!=-1) {
						append(&head,1+rbuf.mtext[0]-'1',atoi(&(rbuf.mtext[1]))); 
					}
					while(msgrcv(msid, &rbuf, 10,2,IPC_NOWAIT)!=-1) {
						deleteNode(&head,1+rbuf.mtext[0]-'1',atoi(&(rbuf.mtext[1]))); 
					}

					if(head) {
						if(head->state==LION) semctl(state3,0,SETVAL,LION);
						else semctl(state3,0,SETVAL,JACKAL);
					} 
					else semctl(state3,0,SETVAL,NONE);
					
					semctl(go,0,SETVAL,3);
					up(mutex3);
					printf("Ranger left meat pit %d\n",check);
					z_up(wL1);z_up(wL2);z_up(wL3);
					z_up(wJ1);z_up(wJ2);z_up(wJ3);
					printf("Ranger giving signal to wait queue of all meat pit\n");
				
					
				} else {

					up(mutex3);
					
					if(j!=2) printf("Ranger denied access\n");
					else {
					
						printf("Ranger in wait queue of meat pit %d\n",check);
						down(mutex3);
						if(!semctl(nR3,0,GETVAL,0)&&semctl(state3,0,GETVAL,0)==RANGER) 
							semctl(state3,0,SETVAL,NONE); 
						up(mutex3);
						waitz(wR3);
						val=semctl(go,0,GETVAL,0);
					}
				}
				break;

			}
			if(eaten) break;
		}
	}
	node* temp;
	while(head) {
		temp=head;
		head=head->next;
		free(temp);
	}
	exit(0);
}