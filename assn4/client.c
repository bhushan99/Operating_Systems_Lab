#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_SIZE 256
#define up_key 123
#define down_key 321

typedef struct msgbuf
{
	long mtype;    // to store type of message
	char mtext[MAX_SIZE]; 
} message_buf;

typedef struct l
{
    char ID[25];    // linked list to store IDs of online clients
    struct l *next;
}node;

void insert(char* text,node** head) { // function to insert all IDs in message of type LIST<ID1><ID2>...
    node* temp;
    erase(head);
    int l=strlen(text);
    int i=5,j;
    node * x;
    while(i<l) {
        x=(node*)malloc(sizeof(node));
        x->next=NULL;
        j=0;
        while(i<l && text[i]!=' ') {
            x->ID[j++]=text[i++];
        }
        i++;
        x->ID[j]='\0';
        if(*head) {temp->next=x;temp=temp->next;}
        else {*head=x;temp=*head;}
    }
}

void erase(node** head) {    // free allocated memory
    while(*head) {
        node * temp=*head;
        *head=temp->next;
        free(temp);
    }
}

// Client receives messages of type as its pid from server from Down queue
// and sends messages of type=1 to Up queue 

int main()
{
	int Up,Down;   // to store Up and Down message queue IDs
    node* head=NULL;
    pid_t pid;
	int msgflg = IPC_CREAT | 0666;
    int i,j,j1,k;
	if ((Up = msgget((key_t)up_key, msgflg )) < 0) {   // get Up message queue ID
        perror("msgget");
        exit(1);
    }
    if ((Down = msgget((key_t)down_key, msgflg )) < 0) { // get Down message queue ID
        perror("msgget");
        exit(1);
    }
    printf("\nENTER CHAT ID\n");
    message_buf buf,rbuf;
    buf.mtype=1;
    scanf("%s",buf.mtext);
    int l=strlen(buf.mtext);
    for(i=l+4;i>=4;i--) buf.mtext[i]=buf.mtext[i-4];
    buf.mtext[0]='N';buf.mtext[1]='E';buf.mtext[2]='W';buf.mtext[3]=' ';
    if (msgsnd(Up, &buf, strlen(buf.mtext)+1, 0) < 0) {    // send message of type NEW ID to server
        perror("msgsnd");
        exit(1);
    }
    if (msgrcv(Down, &rbuf, MAX_SIZE,(long)getpid(),0)==-1) {   // receive a message from server
        perror("msgrcv");
        exit(1);
    }
    if(strncmp(rbuf.mtext,"LIST",4)==0) {
        insert(rbuf.mtext,&head);       // insert into linked list all IDs
        printf("\nOnline Clients:\n");
        node *t=head;
        while(t) {
            printf("%s\n",t->ID);
            t=t->next;
        }
        printf("\n");
    } else if(strncmp(rbuf.mtext,"MSG",3)==0) {        // a message is received from a user
        printf("\nMessage Received:\n");
        i=0;
        while(rbuf.mtext[i]!='<') i++;
        j=i+1;
        while(rbuf.mtext[i]!='>') i++;
        while(j<i) {
            printf("%c", rbuf.mtext[j]);
            j++;
        }
        printf("\n");
        while(rbuf.mtext[i]!='<') i++;
        j=i+1;
        while(rbuf.mtext[i]!='>') i++;
        printf("Sent at\t");
        while(j<i) {
            printf("%c", rbuf.mtext[j]);
            j++;
        }
        printf("\n");
        while(rbuf.mtext[i]!='<') i++;
        j=i+1;
        while(rbuf.mtext[i]!='>') i++;
        printf("Sent by\t");
        while(j<i) {
            printf("%c", rbuf.mtext[j]);
            j++;
        }
        printf("\n\n");
    }
    while(1) {
        char recID[25],tosend[MAX_SIZE],c,check;
        printf("Do you want to send a message? (y or n)\n");
        while((c=getchar())=='\n');
        check=c;
        if(check=='y'){     // If user wants to send message, ask input message
            printf("Online Clients:\n");
            node *t=head;
            while(t) {
                printf("%s\n",t->ID);
                t=t->next;
            }
            printf("\nChoose whom to send:\n");
            scanf("%s",recID);
            printf("Type a message:\n");
            c=getchar();
            gets(tosend);

            strcpy(buf.mtext,"MSG<");
            strcat(buf.mtext,tosend);
            strcat(buf.mtext,"><");
            strcat(buf.mtext,recID);
            strcat(buf.mtext,">");

            buf.mtype=1;    // type = 1 when sending to server
            if (msgsnd(Up, &buf, strlen(buf.mtext)+1,0) < 0) {
                perror("msgsnd");
                exit(1);
            }
        }
        // get message from server
        if (msgrcv(Down, &rbuf, MAX_SIZE, getpid(), 0) < 0) {
            perror("msgrcv");
            exit(1);
        }
        if(strncmp(rbuf.mtext,"LIST",4)==0) {
            insert(rbuf.mtext,&head);
            node *t=head;
            printf("\nOnline Clients Updated:\n");
            while(t) {
                printf("%s\n",t->ID);
                t=t->next;
            }
        } else if(strncmp(rbuf.mtext,"MSG",3)==0) {
            printf("\nMessage Received:\n");
            i=0;
            while(rbuf.mtext[i]!='<') i++;
            j=i+1;
            while(rbuf.mtext[i]!='>') i++;
            while(j<i) {
                printf("%c", rbuf.mtext[j]);
                j++;
            }
            printf("\n");
            while(rbuf.mtext[i]!='<') i++;
            j=i+1;
            while(rbuf.mtext[i]!='>') i++;
            printf("Sent at\t");
            while(j<i) {
                printf("%c", rbuf.mtext[j]);
                j++;
            }
            printf("\n");
            while(rbuf.mtext[i]!='<') i++;
            j=i+1;
            while(rbuf.mtext[i]!='>') i++;
            printf("Sent by\t");
            while(j<i) {
                printf("%c", rbuf.mtext[j]);
                j++;
            }
            printf("\n\n");
        }
        else {
            printf("Unknown Receiver ID. Sending Failed.\n\n");
        }
    }
    erase(&head);   // free allocated memory
	return 0;
}