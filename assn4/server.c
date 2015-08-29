#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <sys/ipc.h> 
#include <sys/msg.h>

#define up_key 123
#define down_key 321
#define MSGSZ 256
#define MAX_CLIENT 25

typedef struct mymsg{     	// structure of message
	long mtype;
	char mtext[MSGSZ];
}mymsg;

typedef struct client{ 		// structure to store pid for each online user
	pid_t pid;
	char chatID[MSGSZ];
}client;

// Server receives messages of type=1 from client from Up queue
// and sends messages of type as pid to Down queue 

int main(){

	int Up_msqid,Down_msqid,TotalClient=0;
	client clientID[MAX_CLIENT];

	if((Up_msqid=msgget((key_t)up_key,IPC_CREAT|0666))==-1){ 	// get Up message queue ID
		perror("msgget ");
		return 0;
	}
	if((Down_msqid=msgget((key_t)down_key,IPC_CREAT|0666))==-1){	// get Down message queue ID
		perror("msgget ");
		return 0;
	}

	while(1){
		mymsg* msgp=(mymsg*)malloc(sizeof(mymsg));
		if(msgrcv(Up_msqid,msgp,(size_t)MSGSZ,1,0)==-1){ // receive a message
			perror("server msgrcv ");
			exit(1);
		}
		if(strncmp(msgp->mtext,"NEW",3)==0){
			// If new user is online send messages to all clients
			int i;
			struct msqid_ds buf;
			if(msgctl(Up_msqid,IPC_STAT,&buf)==-1){
				perror("Server ");
				exit(1);
			}
			// store PID and chatID
			if(TotalClient==MAX_CLIENT) {
				printf("Clients limit is 25. More can't be added.\n");
				free(msgp);
				continue;
			}
			clientID[TotalClient].pid=buf.msg_lspid;
			strcpy(clientID[TotalClient].chatID,msgp->mtext+4);
			TotalClient++;
			char list[200]="LIST ";
			for(i=0;i<TotalClient;i++){
				strcat(list,clientID[i].chatID);
				strcat(list," ");
			}
			for(i=0;i<TotalClient;i++){
				mymsg IDlist;
				IDlist.mtype=(long)clientID[i].pid;
				strcpy(IDlist.mtext,list);
				if(msgsnd(Down_msqid,&IDlist,strlen(IDlist.mtext)+1,0)==-1){
					perror("server msgsnd ");
					exit(1);
				}
			}
			printf("New client added: %s , %d\n",clientID[TotalClient-1].chatID,clientID[TotalClient-1].pid);
			printf("Total number of clients online = %d\n",TotalClient);
		}
		else if(strncmp(msgp->mtext,"MSG",3)==0){
			// send message to corresponding receiver
			int i=0,j,k,rcvpid,sndpid,l;
			char rcvID[MSGSZ];
			mymsg SendingMsg;
			strcpy(SendingMsg.mtext,"MSG<");
			//finding sender's chatID
			struct msqid_ds buf;
			if(msgctl(Up_msqid,IPC_STAT,&buf)==-1){
				perror("Server ");
				exit(1);
			}
			for(k=0;k<TotalClient;k++)
				if(buf.msg_lspid==clientID[k].pid)
					break;
			// finding receiver's pid
			// taking out message sent by sender
			while(msgp->mtext[i]!='<')i++;
			j=i+1;
			while(msgp->mtext[i]!='>')i++;
			strncat(SendingMsg.mtext,msgp->mtext+j,i-j);
			while(msgp->mtext[i]!='<')i++;
			j=i+1;
			while(msgp->mtext[i]!='>')i++;
			l=0;
			while(j<i)
				rcvID[l++]=msgp->mtext[j++];
			rcvID[l]='\0';
			for(j=0;j<TotalClient;j++)
				if(strcmp(rcvID,clientID[j].chatID)==0)
					break;

			if(j==TotalClient) { 	// if receiver not found, send error message
				printf("Unknown receiver ID %s\n",rcvID);
				SendingMsg.mtype=(long)buf.msg_lspid;
				strcpy(SendingMsg.mtext,"FAIL");
				if(msgsnd(Down_msqid,&SendingMsg,strlen(SendingMsg.mtext)+1,0)==-1){
					perror("Server Send ");
					exit(1);
				}
				free(msgp);
				continue;
			}

			SendingMsg.mtype=(long)clientID[j].pid;
			strcat(SendingMsg.mtext,"><");
			strcat(SendingMsg.mtext,ctime(&(buf.msg_stime)));
			SendingMsg.mtext[strlen(SendingMsg.mtext)-1]='>';
			strcat(SendingMsg.mtext,"<");
			strcat(SendingMsg.mtext,clientID[k].chatID);
			strcat(SendingMsg.mtext,">");
			if(msgsnd(Down_msqid,&SendingMsg,strlen(SendingMsg.mtext)+1,0)==-1){
				perror("Server Send ");
				exit(1);
			}
		} 

		free(msgp);
	}

	return 0;
}