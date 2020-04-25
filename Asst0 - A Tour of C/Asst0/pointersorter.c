#include <stdlib.h>
#include <string.h>
#include <stdio.h>
typedef struct node {
	char* value;
	struct node* next;
}node;
//a method to compare strings as I wanted prescedence of capital letters over lowercase ones.
int comparestr(const char *str1, const char *str2)
{
	int xlen = strlen(str1);
	int ylen = strlen(str2);
	int zlen;
	int i;
	//makes clen the shorter string length
	if(xlen > ylen)
		zlen = ylen;
	else
		zlen = xlen;
	//iterates zlen times
	for(i=0;i < zlen;i++)
	{
		//records ASCII of char at i fo both strings
		int tp1 = str1[i];
		int tp2 = str2[i];
		//if char is a  Capital letter, increase the value by 100 to give precedence over lowercase
		if(tp1 <91)
		{
			tp1 = tp1+100;
		}
	        if(tp2 <91)
                {
                        tp2 = tp2+100;
                }
		//if the priority of tp1>tp2 or vice versa or the loop continues
		if(tp1 > tp2)
			return 0;
		else if (tp1 <tp2)
			return 1;
		else
			continue;
	}
	//if str1 length> str2 length and vice versa... return 0 and 1 respectively. This is to check the case if the
	//the strings are exactly the same and whether one string goes on longer than the other
	if(xlen>ylen)
		return 0;
	return 1;
}

int main(int argc, char **argv)
{
	//if arguments are not 2... print error
	if(argc!=2)
	{
		printf("Error: There must be two arguments. Ex. ./pointersorter.c example\n");
		return 1;
	}
	//if the 2nd argument is empty... print nothing
	if(strlen(argv[1])==0)
	{
		return 1;
	}
	//define variables and linked list elements
	char *current = argv[1];
	int i=0;
	int j;
	int numstr =0;
	int beeg;
	int len = strlen(current);
	node *cur;
	node *next;
	node *head = 0;

	//iterate through the string and puts word into linked list through positions
	while(current[i]!= '\0' && i < len)
	{
		//beg records the point at which i want to copy the string from
		beeg = i;
		//this loops goes through the string until either null is hit or a nonalphabetic character is found
		while(current[i]!= '\0')
		{
			int ascii = current[i];
			if((ascii < 65 || ascii > 122) || (ascii <97 && ascii >90) )
				break;
			i++;
		}
		//allocate memory for word and creates variable k to input the string into the word array
		char *word = (char*)malloc((i-beeg+1)*(sizeof(char)));
		int k =0;
		for(j=beeg; j < i; j++)
		{
			word[k] = current[j];
			k++;
		}
		//makes last NULL
		word[k] = '\0';
		//checks if the word length is more than 0
		if(strlen(word) <=0)
		{
			free(word);
			i++;
			continue;
		}
		//creates a linked list node to insert word into the list and increments number of elements in the list
		cur = (node*)malloc(sizeof(node));
		numstr++;
		cur->value = word;
		cur->next = 0;
		//if linked list is empty, makes the head equal to the new node
		if(head==0)
			head = cur;
		else
		{
			node* temp = head;
			node* prev = 0;
			//iterates through list till it reaches a point where it should enter by comparing values of the nodes
			while(comparestr(cur->value,temp->value)!=0 && temp->next!=0)
			{
				prev = temp;
				temp = temp->next;
			}
			//3 cases of how a node is inserted. One is at the front, one is at the end, and one is in the middle of
			// the linked list
			if(comparestr(cur->value,temp->value)!=0)
			{
				temp->next = cur;
			}
			else if (prev == 0)
			{
				cur->next = head;
				head = cur;
			} else {
				cur->next = temp;
				prev->next = cur;
			}
		}
		i++;
	}
	//prints a new line and returns if there is nothing in linked list
        if(numstr ==0)
        {
                printf("\n");
                return 1;
        }
	//iterates through linked list while printing out value and a new line
	for(cur = head; cur !=0; cur = cur->next)
	{
		printf(cur->value);
		printf("\n");
	}
	//frees allocated memory in linked list
	for(cur = head; cur !=0; cur = next)
	{
		next = cur ->next;
		free(cur);
	}
	return 0;
}
