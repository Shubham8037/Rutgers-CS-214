#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include<stdbool.h>
#include "mymalloc.h"
static char myMemory[MAX_MEM_SIZE];
void* mymalloc(size_t memsize, char* logfile, int line)
{
	int i = 0;
	if(memsize > MAX_MEM_SIZE)
	{
		fprintf(stderr, "Memory size is overflow\n", logfile, line);
		return NULL;
	}
	if(memsize == 0)
	{
		return NULL;
	}
	int allocable = 0;
	for(i = 0; allocable < memsize; i ++)
	{
		if(myMemory[i] == '0')
			allocable ++;
		else
			allocable = 0;
		if(allocable == memsize)
			break;
		if(i == MAX_MEM_SIZE - 1)
			return NULL;
	}
	int newRecord = i - memsize + 1;
	for(i = newRecord; i < newRecord + memsize; i ++)
	{
		myMemory[i] = memsize;
	}
	
	return &myMemory[newRecord];
}
void myfree(void *cleanMem, char* logfile, int line)
{
	int i;
	int nIndex = 0;
	int size;
	
	if(cleanMem == 0)
		fprintf(stderr, "Null pointer", logfile, line);

	for(i = 0; i < MAX_MEM_SIZE; i ++)
	{
		if(&myMemory[i] == cleanMem)
		{
			size = myMemory[i];
			break;
		}
	}
	for(nIndex = i; nIndex < i + size; nIndex++)
		myMemory[nIndex] = '0';
	return;
}
void initiate()
{
    int i;
	for (i = 0; i < MAX_MEM_SIZE; i++)
		myMemory[i]='0';
}
