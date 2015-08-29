#include "varibles.h"

int main() {
	int sem_flg= IPC_CREAT | 0666;
	int mutex1=semget(mutex1_key,1,sem_flg);
	int mutex2=semget(mutex2_key,1,sem_flg);
	int mutex3=semget(mutex3_key,1,sem_flg);
	semctl(mutex1,0,SETVAL,1);
	semctl(mutex2,0,SETVAL,1);
	semctl(mutex3,0,SETVAL,1);

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

	semctl(rem1,0,SETVAL,50);
	semctl(rem2,0,SETVAL,50);
	semctl(rem3,0,SETVAL,50);
	semctl(nL1,0,SETVAL,0);
	semctl(nL2,0,SETVAL,0);
	semctl(nL3,0,SETVAL,0);
	semctl(nJ1,0,SETVAL,0);
	semctl(nJ2,0,SETVAL,0);
	semctl(nJ3,0,SETVAL,0);
	semctl(nR1,0,SETVAL,0);
	semctl(nR2,0,SETVAL,0);
	semctl(nR3,0,SETVAL,0);

	int state1=semget(state1_key,1,sem_flg);
	int state2=semget(state2_key,1,sem_flg);
	int state3=semget(state3_key,1,sem_flg);

	semctl(state1,0,SETVAL,0);
	semctl(state2,0,SETVAL,0);
	semctl(state3,0,SETVAL,0);

	int wL1=semget(wL1_key,1,sem_flg);
	int wL2=semget(wL2_key,1,sem_flg);
	int wL3=semget(wL3_key,1,sem_flg);
	int wR1=semget(wR1_key,1,sem_flg);
	int wR2=semget(wR2_key,1,sem_flg);
	int wR3=semget(wR3_key,1,sem_flg);
	int wJ1=semget(wJ1_key,1,sem_flg);
	int wJ2=semget(wJ2_key,1,sem_flg);
	int wJ3=semget(wJ3_key,1,sem_flg);
	int go=semget(goto_key,1,sem_flg);

	semctl(wL1,0,SETVAL,0);
	semctl(wL2,0,SETVAL,0);
	semctl(wL3,0,SETVAL,0);
	semctl(wR1,0,SETVAL,0);
	semctl(wR2,0,SETVAL,0);
	semctl(wR3,0,SETVAL,0);
	semctl(wJ1,0,SETVAL,0);
	semctl(wJ2,0,SETVAL,0);
	semctl(wJ3,0,SETVAL,0);
	semctl(go,0,SETVAL,0);

	return 0;
}