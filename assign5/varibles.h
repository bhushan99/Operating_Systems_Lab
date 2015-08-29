#include    <stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#include    <sys/types.h>
#include    <sys/ipc.h>
#include    <sys/sem.h>
#include 	<sys/msg.h>
#include 	<unistd.h>

#define wL1_key 111
#define wL2_key 222
#define wL3_key 333
#define wR1_key 11
#define wR2_key 22
#define wR3_key 33
#define wJ1_key 1111
#define wJ2_key 2222
#define wJ3_key 3333
#define mutex1_key 41
#define mutex2_key 42
#define mutex3_key 43
#define goto_key 555
#define rem1_key 61
#define rem2_key 62
#define rem3_key 63
#define nJ1_key 71
#define nJ2_key 72
#define nJ3_key 73
#define nL1_key 74
#define nL2_key 75
#define nL3_key 76
#define nR1_key 77
#define nR2_key 78
#define nR3_key 79
#define state1_key 81
#define state2_key 82
#define state3_key 83

#define msg_key 88

#define NONE 0
#define LION 1
#define JACKAL 2
#define RANGER 3

typedef struct $
{
	int state,ind;
	struct $* next;
}node;

typedef struct mf
{
	long mtype;    // to store type of message
	char mtext[10]; 
} message_buf;
