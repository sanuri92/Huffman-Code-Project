#include <stdio.h>

typedef int Position;
typedef enum{FALSE,TRUE} Boolean;

void Error(const char *message)
{
    fprintf(stderr,"\nError.%s\n",message);
    printf("\nPress Enter to exit\n");
    getch();
    exit(1);
}

typedef struct
{
    char symbol;//stores the character
    char scode[20];//stores the code for the particular character
    int f;//stores frequency of the particular character
}HCode;


//Huffman Tree Node Implementation

typedef struct
{
    int freq;
    char val;
    int key;    //Used as indicator that a character from file is stored or not in the node
}NEntry;

typedef struct node
{
    struct node *right;
    struct node *left;
    struct node *up;
    NEntry entry;
}Node;
//----------------------

Node * GetNode();

//Linked Priority Queue implementation

typedef Node * QEntry;

typedef struct qnode
{
    QEntry qentry;
    struct qnode *next;
    struct qnode *previous;
}QNode;

typedef struct
{
    int count;
    Boolean full;
    QNode *first;
    QNode *last;
}P_Queue;

void Create_PQ(P_Queue *q)
{
    q->count=0;
    q->full=FALSE;
    q->first=NULL;
    q->last=NULL;
}

Boolean IsQEmpty(const P_Queue *q)
{
    return(q->first==NULL);
}

Boolean IsQFull(const P_Queue *q)
{
    return(q->full);
}

int QSize(const P_Queue *q)
{
    return(q->count);
}

QNode * MakeQNode(QEntry x)
{
    QNode *p=(QNode *)malloc(sizeof(QNode));
    if(p)
    {
        p->qentry=x;
        p->next=NULL;
        p->previous=NULL;
    }

    return(p);
}

void Insert(QEntry x,P_Queue *q)
{
    QNode *current,*newnode;
    if(IsQFull(q))
    {
        Error("Queue is full");
    }
    else if((newnode=MakeQNode(x))==NULL)
    {
        q->full=TRUE;
        Error("Memory full, cannot complete operation");
    }
    else if(IsQEmpty(q))
    {
        q->first=q->last=newnode;
        q->count++;
    }
    else
    {
        for(current=q->last;current&&x->entry.freq>=current->qentry->entry.freq;current=current->previous)
            ;
        if(current==NULL)	//QEntry x has the lowest priority,Highest frequency
        {
            q->first->previous=newnode;
            newnode->next=q->first;
            q->first=newnode;
        }
        else if(current==q->last)	//If QEntry x has the highest priority,lowest frequency
        {
            q->last->next=newnode;
            newnode->previous=q->last;
            q->last=newnode;
        }
        else
        {
            newnode->next=current->next;
            current->next->previous=newnode;
            newnode->previous=current;
            current->next=newnode;
        }
        q->count++;
    }
}

void ServeMaxPriority(P_Queue *q,QEntry *x)
{
    if(IsQEmpty(q))
    {
        printf("Queue is empty\n");
    }
    else if(QSize(q)==1)
    {
        *x=q->last->qentry;
        free(q->last);
        q->first=q->last=NULL;
        q->count--;
    }
    else
    {
        *x=q->last->qentry;
        q->last=q->last->previous;
        free(q->last->next);
        q->last->next=NULL;
        q->count--;
    }
}

void SwapQNodeEntry(QNode *node1,QNode *node2)
{
    QEntry temp=node1->qentry;
    node1->qentry=node2->qentry;
    node2->qentry=temp;
}

//Used to Insert,tally occurrences and sort according to the priority(Frequency of each character) at the same time.
void InsertCh(char x,P_Queue *q)
{
    Node *newnode;
    QNode *current,*newqnode;
    int charmatched=0;

    if(IsQEmpty(q))
    {
        newnode=GetNode();
        newnode->entry.val=x;
        newnode->entry.freq=1;
        newnode->entry.key=1;
        Insert(newnode,q);
    }
    else
    {
        for(current=q->first;current;current=current->next)
        {
            if(current->qentry->entry.val==x)
            {
                if(current!=q->first)
                {
                    newqnode=current;
                    for(;current->previous&&current->qentry->entry.freq==current->previous->qentry->entry.freq;current=current->previous)
                        ;
                    if(newqnode!=current)
                        SwapQNodeEntry(newqnode,current);
                }
                current->qentry->entry.freq++;
                charmatched=1;
                break;	//Stop unnessasary iterations
            }
        }
        if(charmatched==0)
        {
            newnode=GetNode();
            newnode->entry.val=x;
            newnode->entry.freq=1;
            newnode->entry.key=1;
            Insert(newnode,q);
        }
    }
}
//-------------------------------------------------



//Huffman Tree Implementation

void Preorder(Node *root,void (*visit)(NEntry))
{
    if(root)
    {
        visit(root->entry);
        Preorder(root->left,visit);
        Preorder(root->right,visit);
    }
}

void Inorder(Node *root,void (*visit)(NEntry))
{
    if(root)
    {
        Inorder(root->left,visit);
        visit(root->entry);
        Inorder(root->right,visit);
    }
}

void Postorder(Node *root,void (*visit)(NEntry))
{
    if(root)
    {
        Postorder(root->left,visit);
        Postorder(root->right,visit);
        visit(root->entry);
    }
}

void DeleteTree(Node **root)
{
	if(*root)
	{
		if((*root)->left)
		{
			DeleteTree(&(*root)->left);
		}
		if((*root)->right)
		{
			DeleteTree(&(*root)->right);
		}
		free(*root);
		*root=NULL;
	}
}

Node * GetNode()
{
    Node *t=(Node *)(malloc(sizeof(Node)));
    if(!t)
    {
        Error("Not Enough Memory! Cannot complete operation");
    }
    else
    {
        t->right=NULL;
        t->left=NULL;
        t->up=NULL;
        t->entry.freq=0;
        t->entry.val='\0';
        t->entry.key=0;
    }
    return t;
}

//Insert the address of parent node to each nodes 'up' element,
//this is used in GetCodes function to efficiently traverse the tree to get code words.
void SetPaths(Node *root)
{
    if(root)
    {
        if(root->left)
        {
            root->left->up=root;
            SetPaths(root->left);
        }
        if(root->right)
        {
            root->right->up=root;
            SetPaths(root->right);
        }
    }
}

void BuildOHT(P_Queue *q,Node **root)	//Building Optimal Huffman tree from priority queue and returns its root.
{
    Node *newnode,*n;
    while(QSize(q)>=2)
    {
        newnode=GetNode();
        ServeMaxPriority(q,&n);
        newnode->right=n;
        ServeMaxPriority(q,&n);
        newnode->left=n;
        newnode->entry.freq=newnode->left->entry.freq+newnode->right->entry.freq;
        Insert(newnode,q);
    }
    if(IsQEmpty(q))
        printf("\nNo data to build a Tree\n");
    else
    {
        ServeMaxPriority(q,root);
        SetPaths(*root);
    }
}
//---------------------




void GetCodes(Node *root,HCode codes[])
{
    int i,j=0;
    Node *current=root;
    if(current&&current->entry.key==1)	//In case that only one node in the tree (and in the case that root is null)
	{
		codes[j].symbol=current->entry.val;
		codes[j].scode[0]='0';
		codes[j].f=current->entry.freq;
		printf("\nOnly one distinct character present in whole file!\n");
		DeleteTree(&current);   //Only releasing memory, variable root will still contain address value
	}
	else
	{
		for(i=0;current;i++)
		{
			if(current->left)
			{
				codes[j].scode[i]='0';
				codes[j].scode[i+1]='\0';//Mark the end of string
				current=current->left;
			}
			else if(current->right)
			{
				codes[j].scode[i]='1';
				codes[j].scode[i+1]='\0';//Mark the end of string
				current=current->right;
			}
			else
			{
				if(current->entry.key==1)
				{
					codes[j].symbol=current->entry.val;
					codes[j].f=current->entry.freq;
					j++;
				}
				if(current->up==NULL)  //If loop reached root(final node)
				{
				    free(root);
					break;
				}
				else if(codes[j-1].scode[i-1]=='0')
				{
					current->up->left=NULL;
					DeleteTree(&current);
				}
				else if(codes[j-1].scode[i-1]=='1')
				{
					current->up->right=NULL;
					DeleteTree(&current);
				}
				current=root;	//Make next loop to traverse tree again starting from root
				i=-1;	//Set i to 0 on next loop
			}
		}
	}
}

void PrintCode(int count,HCode codes[])
{
    FILE *hc;
    int i;

    hc=fopen("H_Code.txt","w");
    printf("\nThe optimal Huffman code is:\n-----------------\n");
    if(hc)
    {
        for(i=0;i<count;i++)
        {
            fprintf(hc,"%c ",codes[i].symbol);
            fputs(codes[i].scode,hc);
            fprintf(hc,"\n");
            printf("%c : ",codes[i].symbol);
            puts(codes[i].scode);
        }
        printf("\nThe Optimal Huffman code is saved to the text file 'H_Code.txt'\n");
    }
    else
    {
        for(i=0;i<count;i++)
        {
            printf("%c : ",codes[i].symbol);
            puts(codes[i].scode);
        }
        printf("\nError creating a new text file to save optimal Huffman code\n");
    }
    fclose(hc);
}

void CalculateRatio(int count,int length,HCode codes[])
{
    int i;
    float abr=0,ratio;
    for(i=0;i<count;i++)
    {
        abr+=codes[i].f*strlen(codes[i].scode);
        /*//This code line was used to check weather all the values are counted correctly and to manually calculate compression ratio.
        printf("%c - %d - %d\n",codes[i].symbol,codes[i].f,strlen(codes[i].scode));
        */
    }
    if(length!=0&&abr!=0)
    {
        abr/=length;
        ratio=8/abr;
        printf("\n\nThe compression ratio is : %.2f\n\n",ratio);
    }
}

