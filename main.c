
#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<string.h>
#include "PROJECT.H"

/*//This function was used to check Functionality of priority queue
void checkpq(P_Queue *q)
{
    QNode *n;
    for(n=q->first;n;n=n->next)
        printf("%c --- %d\n",n->qentry->entry.val,n->qentry->entry.freq);
}
*/

//This function was used to test functionality of some individual functions
/*void nodeprint(NEntry x)
{
    printf("\n%c - %d",x.val,x.freq);
}
*/

int main()
{
    char c,fname[30];
    FILE *fp;
    P_Queue q;
    Node *root;
    int charcount,filecharcount=0;
    HCode *codes;	//Stores address of memory block where generated Huffman code is saved.

    //clrscr();
    Create_PQ(&q);

    printf("Enter file name to generate optimal Huffman code(With the extension): ");
    gets(fname);
    fp=fopen(fname,"r");

    if(!fp)
    {
	printf("File read error\n");
    printf("\nPress Enter to exit\n");
    getch();
	return 0;
    }
    else
    {
	while((c=fgetc(fp))!=EOF)
	{
	    InsertCh(c,&q);
	    filecharcount++;//Counting the length of the given file
	}
    }

    fclose(fp);

    charcount=QSize(&q);	//Records the number of distinct characters

    codes=(HCode *)(calloc(charcount+2,sizeof(HCode))); //Set array size to store generated Huffman code

    if(!codes)
    {
	printf("\n\nError. Not enough memory available to complete the operation\n");
    printf("\nPress Enter to exit\n");
    getch();
	return 0;
    }

    //checkpq(&q); //Used to check Functionality of priority queue

    BuildOHT(&q,&root);//Building optimal Huffman tree

/*//This code segment was used to check functionality of BuildOHT function(Verify that tree has built correctly)

    printf("\nPreorder\n--------\n");
    Preorder(root,&nodeprint);
    printf("\nInorder\n--------\n");
    Inorder(root,&nodeprint);
    printf("\nPostorder\n--------\n");
    Postorder(root,&nodeprint);

*///------------------

    GetCodes(root,codes);//Getting codes for each symbol from optimal Huffman tree

    if(root)
    {
	PrintCode(charcount,codes);	//Display Huffman code an save to a new file
	CalculateRatio(charcount,filecharcount,codes);	//Calculate and print compression ratio
    }

    free(codes);
    printf("\nPress Enter to exit\n");
    getch();

    return 0;
}
