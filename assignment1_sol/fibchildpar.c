#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(){
	int i,status,nfib;
	printf("n=");
	scanf("%d",&nfib);
	int fib[nfib];
	pid_t pid[nfib]; // stores process id of n childs running in parallel where pid[i] will store process id of child computing ith fibonacci number
	for(i=0;i<nfib;i++){
		pid[i]=fork();
		if(pid[i]==0){
				int j,a=1,b=1;
				for(j=0;j<i;j++){   // ith child computes ith fibonacci number
					a=a+b;
					b=a-b;
				}
				exit(b);
		}
	}
	for(i=0;i<nfib;i++){
		waitpid(pid[i],&status,0); // wait n times for n child processes with id pid[i] where i varies from 0 to n-1
		fib[i]=status>>8;
		printf("%d\n",fib[i]);
	}
	return 0;
}
