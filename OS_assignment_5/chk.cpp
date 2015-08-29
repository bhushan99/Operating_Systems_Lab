#include "varibles.h"

int main(){
	int sem_flg= IPC_CREAT | 0666;
	int state1=semget(state1_key,1,sem_flg);
	int state2=semget(state2_key,1,sem_flg);
	int state3=semget(state3_key,1,sem_flg);
	
	printf("%d\n",semctl(state1,0,GETVAL,0));
	printf("%d\n",semctl(state2,0,GETVAL,0));
	printf("%d\n",semctl(state3,0,GETVAL,0));
	int rem1=semget(rem1_key,1,sem_flg);
	printf("%d\n",semctl(rem1,0,GETVAL,0));
	int rem2=semget(rem2_key,1,sem_flg);
	printf("%d\n",semctl(rem2,0,GETVAL,0));
	int rem3=semget(rem3_key,1,sem_flg);
	printf("%d\n",semctl(rem3,0,GETVAL,0));

	int nL1=semget(nL1_key,1,sem_flg);
	int nL2=semget(nL2_key,1,sem_flg);
	int nL3=semget(nL3_key,1,sem_flg);
	int nR1=semget(nR1_key,1,sem_flg);
	int nR2=semget(nR2_key,1,sem_flg);
	int nR3=semget(nR3_key,1,sem_flg);
	int nJ1=semget(nJ1_key,1,sem_flg);
	int nJ2=semget(nJ2_key,1,sem_flg);
	int nJ3=semget(nJ3_key,1,sem_flg);

	printf("%d\n",semctl(nL1,0,GETZCNT,0));
	printf("%d\n",semctl(nL2,0,GETZCNT,0));
	printf("%d\n",semctl(nL3,0,GETZCNT,0));
	printf("%d\n",semctl(nR1,0,GETZCNT,0));
	printf("%d\n",semctl(nR2,0,GETZCNT,0));
	printf("%d\n",semctl(nR3,0,GETZCNT,0));
	printf("%d\n",semctl(nJ1,0,GETZCNT,0));
	printf("%d\n",semctl(nJ2,0,GETZCNT,0));
	printf("%d\n",semctl(nJ3,0,GETZCNT,0));

	int go=semget(goto_key,1,sem_flg);
	printf("%d\n",semctl(go,0,GETVAL,0));
	
	return 0;
}