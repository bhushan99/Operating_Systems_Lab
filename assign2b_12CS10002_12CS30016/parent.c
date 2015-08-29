#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

#define REQUEST 100
#define PIVOT 200
#define LARGE 300
#define SMALL 400
#define READY 500
#define MAX 256

void writer(int fd[],char *buf){
	if(write(fd[1],buf,MAX)==-1)
		perror("WRITE ERROR IN PARENT ");
}

void reader(int fd[],char *buf){
	if(read(fd[0],buf,MAX)==-1)
		perror("READ ERROR IN PARENT ");
}

int main(){

	time_t t;
	srand((unsigned)(2));

	int n=5;

	pid_t child[5];
	int i,k,m,response[5];
	int fd1[5][2],fd2[5][2];
	int random_no,element;

	/*creating 2 pipes followed by fork to provide 2 pipes between each parent and child pair and then loading the child.c file into each child
	fd1 is file descriptor for parent->child pipe
	fd2 is file descriptor for child->parent pipe
	index 0 => reading endfile descriptor
	index 1 => writing end file descriptor*/
	for(i=0;i<5;i++){
		while(pipe(fd1[i])!=0)
			perror("ERROR ");
		while(pipe(fd2[i])!=0)
			perror("ERROR ");
		child[i]=fork();
		if(child[i]==0){
			char *argv[8];
			int j;
			for (j = 0; j < 7; j++)
				argv[j]=(char*)malloc(11*sizeof(char));
			sprintf(argv[0],"%s","child");
			sprintf(argv[1],"%s","child");
			sprintf(argv[2],"%s%d%s","data_",i+1,".txt");
			sprintf(argv[3],"%d",fd1[i][0]);
			sprintf(argv[4],"%d",fd1[i][1]);
			sprintf(argv[5],"%d",fd2[i][0]);
			sprintf(argv[6],"%d",fd2[i][1]);
			argv[7]=NULL;
			if(execv(argv[0],argv)==-1)
				perror("ERROR ");
		}

	}

	//closing unused ends of pipe
	for(i=0;i<5;i++){
		close(fd1[i][0]);
		close(fd2[i][1]);
	}

	//alloting id's to the children
	for(i=0;i<5;i++){
		char buf[MAX];
		sprintf(buf,"%d",i+1);
		writer(fd1[i],buf);
	}

	//waiting to receive READY signal from children
	for(i=0;i<5;i++){
		char buf[MAX];
		reader(fd2[i],buf);
	}
	printf("Parent READY\n");

	k=(n%2==0)?(n*5)/2-1:(n*5+1)/2-1;

	// finding median
	while(1){

		// randomly asking for a pivot element from a child 
		do{
			char bufr[MAX];
			random_no = rand()%5;
			printf("Parent sends REQUEST to child %d\n",random_no+1);
			sprintf(bufr,"%d",REQUEST);
			writer(fd1[random_no],bufr);
			char bufw[MAX];
			reader(fd2[random_no],bufw);
			element=atoi(bufw);
			printf("Parent receives PIVOT %d from child %d\n",element,random_no+1);
		}while(element==-1);

		//sending pivot element to each child
		printf("Parent broadcasts PIVOT %d to all children\n",element);
		for(i=0;i<5;i++){
			char buf[MAX],sig[MAX];
			sprintf(buf,"%d",element);
			sprintf(sig,"%d",PIVOT);
			writer(fd1[i],sig);
			writer(fd1[i],buf);
		}
	

		//finding total no of elements larger than the current pivot among all the children
		m=0;
		for(i=0;i<5;i++){
			char buf[MAX];
			reader(fd2[i],buf);
			response[i]=atoi(buf);
			m+=response[i];
		}

		/*m>k => no of elements larger than pivot is >k => we need to consider only those elements which are larger than current
		pivot so parent sends SMALL signal to children asking them to drop elements smaller than current pivot*/
		if(m>k){
			char sig[MAX];
			sprintf(sig,"%d",SMALL);
			for(i=0;i<5;i++)
				writer(fd1[i],sig);
		}
		/*m<k => no of elements larger than pivot is <k => we need to consider only those elements which are smaller than current
		pivot so parent sends LARGE signal to children asking them to drop elements larger than current pivot*/
		else if(m<k){
			char sig[MAX];
			sprintf(sig,"%d",LARGE);
			for(i=0;i<5;i++)
				writer(fd1[i],sig);
			k-=m;
			//break;
		}
		// parent detects that the current pivot is required median. It sends terminating signal to each child
		else{
			printf("Parent: m=%d+%d+%d+%d+%d = %d , %d is median\n",response[0],response[1],response[2],response[3],response[4],m,element);
			printf("Parent sends kill signals to all children\n");
			break;
		}

		// READY signal for synchronization
		for(i=0;i<5;i++){
			char buf[MAX];
			reader(fd2[i],buf);
		}
	}

	sleep(1);
	for(i=0;i<5;i++)
		kill(child[i],SIGINT);
	for(i=0;i<5;i++)
		wait();

	//close all pipes
	for(i=0;i<5;i++){
	 close(fd1[i][0]);
	 close(fd1[i][1]);
	 close(fd2[i][0]);
	 close(fd2[i][1]);
	}
	return 0;
}