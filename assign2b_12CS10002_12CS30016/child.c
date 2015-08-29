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


int fd1[2],fd2[2];
char id[MAX];

void writer(char *buf){
	if(write(fd2[1],buf,MAX)==-1)
		perror("WRITE ERROR IN CHILD ");
}

void reader(char *buf){
	if(read(fd1[0],buf,MAX)==-1)
		perror("READ ERROR IN CHILD ");
}

void func(){
	printf("Child %s terminates\n",id);
	exit(0);
}

int main(int argc,char *argv[8]){

	time_t t;
	srand((unsigned)(2));
	int n=5;
	FILE *fp=fopen(argv[2],"r");
	int element[n],i=0,sig=0,init=0,last=n-1,pvt,j;
	for(i=0;i<n;i++)
		fscanf(fp,"%d",&element[i]);
	fclose(fp);

	// sorting the array
	for(i=0;i<n;i++){
		int index=i;
		for(j=i;j<n;j++)
			if(element[j]<element[index])
				index=j;
		int temp=element[index];
		element[index]=element[i];
		element[i]=temp;
	}

	//  setting file descriptors
	fd1[0]=atoi(argv[3]);
	fd1[1]=atoi(argv[4]);
	fd2[0]=atoi(argv[5]);
	fd2[1]=atoi(argv[6]);

	//closing unused ends of pipe
	close(fd1[1]);
	close(fd2[0]);

	// reading ID assigned parent
	reader(id);

	// child sends READY signal to parent
	char ready[MAX];
	printf("Child %s sends READY\n",id);
	sprintf(ready,"%d",READY);
	writer(ready);

	// replying to parent for different signals
	while(1){

		signal(SIGINT,func);

		char buf[MAX];

		reader(buf);
		sig=atoi(buf);

		if(sig==REQUEST){
			if(n==0){
				printf("Child %s sends -1 to parent\n",id);
				writer("-1");
			}
			else{
				char temp[MAX];
				sprintf(temp,"%d",element[init+rand()%n]);
				printf("Child %s sends %s to parent\n",id,temp);
				writer(temp);
			}
		}
		else if(sig==PIVOT){
			char reply[MAX],temp[MAX];
			reader(temp);
			pvt=atoi(temp);
			if(n==0)
				sprintf(reply,"%s","0");
			else{
				for(i=last;i>=init;i--)
					if(element[i]<=pvt)
						break;
				sprintf(reply,"%d",last-i);
			}
			printf("Child %s receives PIVOT and replies %s\n",id,reply);
			writer(reply);
		}
		else if(sig==SMALL){	
			for(i=last;i>=init;i--)
				if(element[i]<pvt)
					break;
			init=i+1;
			n=last-init+1;
			printf("In SMALL Child %s has remaining n=%d >= PIVOT\n",id,n);
			char temp[MAX];
			//printf("Child %s sends READY\n",id);
			sprintf(temp,"%d",READY);
			writer(temp);
		}
		else if(sig==LARGE){
			for(i=init;i<=last;i++)
				if(element[i]>pvt)
					break;
			last=i-1;
			n=last-init+1;
			printf("In LARGE Child %s has remaining n=%d <= PIVOT\n",id,n);
			char temp[MAX];
			sprintf(temp,"%d",READY);
			writer(temp);
		}
	}

	return 0;
}