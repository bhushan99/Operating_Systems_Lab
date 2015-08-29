#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(){
	int n,a=1,b=1,i,status,temp;
	printf("n=");
	scanf("%d",&n);
	pid_t pid;
	for(i=0;i<n;i++) {// this loop runs for nfib times and each tgime a child process is created which computes the ith fibonacci number and returns in exit()
		pid=fork();
		if(pid==0) { // this condition makes sure that only child process enters this condition
			printf("%d\n",b);
			exit(b);
		}
		wait(&status); //wait till chile process ends,result is stored in status
		b=a; 		//update a and b for next iteration
		a+=status>>8;
	}
	return 0;
}
