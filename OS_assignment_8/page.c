#include <stdio.h>
#include <stdlib.h>

typedef struct node
{
	int page;
	int reference_bit;
	int frequency;
	struct node* next;
} node;


node* newnode(int pg)
{
	node* temp=(node*) malloc(sizeof(node));
	temp->next=NULL;
	temp->page=pg;
	return temp;
}

void erase(node** head)
{
	node* temp;
	while(*head) 
	{
		temp=*head;
		*head=temp->next;
		free(temp);
	}
}

void generate_reference_string(int n,int d,int* ref) 
{
	int i,j,k;
	int A[n][n],B[n][n],out[n][n];
	int ast=50,curr=0;
	for(i=0;i<n;i++) for(j=0;j<n;j++) 
	{
		if(curr==d) {ast++;curr=0;}
		A[i][j]=ast;
		curr++;
	}
	ast=500,curr=0;
	for(i=0;i<n;i++) for(j=0;j<n;j++) 
	{
		if(curr==d) {ast++;curr=0;}
		B[i][j]=ast;
		curr++;
	}
	ast=5000,curr=0;
	for(i=0;i<n;i++) for(j=0;j<n;j++) 
	{
		if(curr==d) {ast++;curr=0;}
		out[i][j]=ast;
		curr++;
	}
	int x=0;
	for(i=0;i<n;i++) for(j=0;j<n;j++) 
	{	
		for(k=0;k<n;k++) 
		{
			ref[x++]=A[i][k];
			printf("%d\n",ref[x-1] );
			ref[x++]=B[k][j];
			printf("%d\n",ref[x-1] );
		}
		ref[x++]=out[i][j];
		printf("%d\n",ref[x-1] );
	}
	return;
}

void FIFO(int size,int* ref,int iter)
{
	int frames;
	FILE* fw=fopen("FIFO.txt","w");
	for(frames=1;frames<=iter;frames++)
	{
		int pagefaults=0;
		int qnum=0;
		node* head=NULL;
		int i;
		for(i=0;i<size;i++) 
		{
			if(!head) 
			{
				pagefaults++;
				qnum++;
				node* temp=newnode(ref[i]);
				head=temp;
			}
			else
			{
				node * temp=head,*temp1=NULL;
				while(temp&&temp->page!=ref[i]) {temp1=temp;temp=temp->next;}
				if(temp) continue;

				if(qnum==frames) 
				{
					pagefaults++;
					if(qnum==1) head->page=ref[i];
					else {
						temp=head;
						head=head->next;
						free(temp);
						temp1->next=newnode(ref[i]);
					}
				}
				else
				{
					pagefaults++;
					qnum++;
					temp1->next=newnode(ref[i]);
				}
			}
		}
		erase(&head);
		fprintf(fw,"%d %d\n",frames,pagefaults);
	}
	fclose(fw);
	return;
}

void LRU(int size,int* ref,int iter)
{
	int frames;
	FILE* fw=fopen("LRU.txt","w");
	for(frames=1;frames<=iter;frames++)
	{	
		int pagefaults=0;
		int qnum=0;
		node* head=NULL;
		int i;
		for(i=0;i<size;i++) 
		{
			if(!head) 
			{
				pagefaults++;
				qnum++;
				node* temp=newnode(ref[i]);
				head=temp;
			}
			else
			{
				node * temp=head,*temp1=NULL;
				while(temp&&temp->page!=ref[i]) {temp1=temp;temp=temp->next;}
				if(temp) 
				{
					if(temp1==NULL) continue;
					temp1->next=temp->next;
					free(temp);
					temp=head;
					head=newnode(ref[i]);
					head->next=temp;
					continue;
				}
				pagefaults++;
				if(qnum==frames)
				{
					if(qnum==1) head->page=ref[i];
					else {
						node* x=head;
						while(x->next!=temp1) x=x->next;
						x->next=NULL;
						free(temp1);
						temp=head;
						head=newnode(ref[i]);
						head->next=temp;
					}
				}
				else
				{
					temp=head;
					head=newnode(ref[i]);
					head->next=temp;
					qnum++;
				}
			}
		}
		erase(&head);
		fprintf(fw,"%d %d\n",frames,pagefaults);
	}
	fclose(fw);
	return;
}

void LFU(int size,int* ref,int iter)
{
	int frames;
	FILE* fw=fopen("LFU.txt","w");
	for(frames=1;frames<=iter;frames++)
	{
		int pagefaults=0;
		int qnum=0;
		node* head=NULL;
		int i;
		for(i=0;i<size;i++) 
		{
			if(!head) 
			{
				pagefaults++;
				qnum++;
				node* temp=newnode(ref[i]);
				head=temp;
				head->frequency=1;
			}
			else
			{
				node * temp=head,*temp1=NULL;
				while(temp&&temp->page!=ref[i]) {temp1=temp;temp=temp->next;}
				if(temp) 
				{
					temp->frequency=temp->frequency+1;
					continue;
				}
				pagefaults++;
				if(qnum==frames)
				{
					if(qnum==1) 
					{
						head->page=ref[i];
						head->frequency=1;
					}
					else 
					{
						int mf=head->frequency;
						node* mp=head, *mpp=NULL;
						node* x=head->next , *y=head;
						while(x) 
						{
							if(x->frequency<mf) 
							{
								mf=x->frequency;
								mp=x;
								mpp=y;
							}
							x=x->next;
							y=y->next;
						}
						if(mpp)
						{
							mpp->next=mp->next;
							free(mp);
						}
						else
						{
							temp=head;
							head=head->next;
							free(temp);
						}
						x=head;
						while(x->next) x=x->next;
						x->next=newnode(ref[i]);
						x->next->frequency=1;
					}
				}
				else
				{
					qnum++;
					node* x=head;
					while(x->next) x=x->next;
					x->next=newnode(ref[i]);
					x->next->frequency=1;
				}
			}
		}
		erase(&head);
		fprintf(fw,"%d %d\n",frames,pagefaults);
	}
	fclose(fw);
	return;
}

void IICHANCE(int size,int* ref,int iter)
{
	int frames;
	FILE* fw=fopen("IICHANCE.txt","w");
	for(frames=1;frames<=iter;frames++)
	{
		int pagefaults=0;
		int qnum=0;
		node* head=NULL;
		int i;
		for(i=0;i<size;i++) 
		{
			if(!head) 
			{
				pagefaults++;
				qnum++;
				node* temp=newnode(ref[i]);
				head=temp;
				head->reference_bit=0;
			}
			else
			{
				node * temp=head,*temp1=NULL;
				while(temp&&temp->page!=ref[i]) {temp1=temp;temp=temp->next;}
				if(temp) 
				{
					temp->reference_bit=1;
					continue;
				}
				pagefaults++;
				if(qnum==frames)
				{
					if(qnum==1) 
					{
						head->page=ref[i];
						head->reference_bit=0;
					}
					else 
					{
						node* x=head,*xp=NULL;
						while(1)
						{
							if(x->reference_bit==1) x->reference_bit=0;
							else break;
							xp=x;
							x=x->next;
							if(x==NULL) {x=head;xp=NULL;} 
						}
						if(xp) 
						{
							xp->next=x->next;
							free(x);
						}
						else
						{
							x=head->next;
							free(head);
							head=x;
						}
						x=head;
						while(x->next) x=x->next;
						x->next=newnode(ref[i]);
						x->next->reference_bit=0;
					}
				}
				else
				{
					qnum++;
					node* x=head;
					while(x->next) x=x->next;
					x->next=newnode(ref[i]);
					x->next->reference_bit=0;
				}
			}
		}
		erase(&head);
		fprintf(fw,"%d %d\n",frames,pagefaults);
	}
	fclose(fw);
	return;
}

int main() 
{
	FILE* fp=fopen("datafile.txt","r");
	if(!fp) {
		perror("fopen");
		return -1;
	}
	int n,d;
	fscanf(fp,"%d%d",&n,&d);
	fclose(fp);
	int size=n*n*(2*n+1);
	int iter=3*n*n/d+2;
	int ref[size];
	generate_reference_string(n,d,ref);

	int input;
	while(1)
	{
		printf("Enter Option (0:exit, 1:FIFO, 2:LRU, 3:LFU, 4:2nd Chance) : ");
		scanf("%d",&input);
		if(!input) break;
		switch(input)
		{
			case 1:FIFO(size,ref,iter);break;
			case 2:LRU(size,ref,iter);break;
			case 3:LFU(size,ref,iter);break;
			case 4:IICHANCE(size,ref,iter);break;
		}
	}

	return 0;
}