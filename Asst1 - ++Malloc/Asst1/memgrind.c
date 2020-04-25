////	Testing and Instrumentation
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "mymalloc.h"
#include <time.h>

void* cnts[MAX_SIZE];
void reset()
{
	int cnt;
	for(cnt = 0; cnt < MAX_SIZE; cnt++)
		cnts[cnt]=0;
	return;
}
void Test_A()
{
	int cnt;
	char* chPtr= NULL;
	for(cnt = 0; cnt < 150; cnt++)
	{
		chPtr = malloc(1);
		free(chPtr);
	}
}

void Test_B()
{
	int cnt;
	int i;
	char* chPtr[150];
	
	for(cnt =0; cnt < 150; cnt++)
	{
		chPtr[cnt]=malloc(1);
		if(cnt+1%50 ==0)
		{
			for(i= cnt-49;i++; i < cnt)
			{
				free(chPtr[i]);
			}
		}
	}
}
void Test_C()
{
	int cntMallocs=0;
	int cntFrees=0;
	char* chPtr[50];
	while (cntMallocs!= 50)
	{
		int n = rand() % 2;
		if(n == 0)
		{
			chPtr[cntMallocs] = malloc(1);
			cntMallocs= cntMallocs+1;
		}
		else
		{
			if(cntMallocs>cntFrees)
			{
				free(chPtr[cntFrees]);
				cntFrees = cntFrees +1;
			}
		}
	}
	while(cntFrees!=cntMallocs)
	{
		free(chPtr[cntFrees]);
		cntFrees= cntFrees+1;
	}
}

void Test_D()
{
        int cntMallocs=0;
        int cntFrees=0;
        char* chPtr[50];		
        while (cntMallocs!= 50)
        {
                int n = rand()%2;
				int m = rand()%64 + 1;
                if(n == 0)
                {
                        chPtr[cntMallocs] = malloc(m);
                       	cntMallocs= cntMallocs+1;
                }
                else
                {
                        if(cntMallocs>cntFrees)
                        {
                                free(chPtr[cntFrees]);
                                cntFrees = cntFrees +1;
                        }
                }
        }
        while(cntFrees!=cntMallocs)
        {
                free(chPtr[cntFrees]);
                cntFrees= cntFrees+1;
        }
}
void Test_EF()
{
    char* chPtr[500];
    int i = 0;
    int rval = 0;

    //Allocate blocks of 1 to 3
    for(i = 0; i < 500; i++)
    {
        chPtr[i] = malloc(rand()%3+1);
    }

    for(i = 0; i < 500; i++)
    {
        rval = rand()%3;
        if (rval == 0) {
            free(chPtr[i]);
        }
        else continue;
    }

    for(i = 0; i < 50; i++) //Malloc 
    {

            rval = rand()%3+1;
            chPtr[i] = malloc(rval);
    }
}

void test6()
{
    int rval = rand()%1000+1000;
    char *chPtr[rval];
    int i=0;
    for (i=0;i<rval; i++) {
        chPtr[i] = malloc(1);
    }
    for (i=0;i<rval/2;i++) {
        free(chPtr[rval-i-1]);
        free(chPtr[i]);
    }
}

int main(int argc, char*argv[])
{
	int cnt;
	struct timeval sTime, eTime;
	gettimeofday(&sTime,NULL);
	for(cnt=0; cnt<100; cnt++)
	{
		initiate();
		Test_A();
	}
	gettimeofday(&eTime, NULL);
	printf("Test_A: %f seconds\n", ((eTime.tv_sec - sTime.tv_sec)+((eTime.tv_usec - sTime.tv_usec)/1000000.0))/100);
        gettimeofday(&sTime,NULL);
        for(cnt=0; cnt<100; cnt++)
        {
                initiate();
                Test_B();
        }
        gettimeofday(&eTime, NULL);
        printf("Test_B: %f seconds\n", ((eTime.tv_sec - sTime.tv_sec)+((eTime.tv_usec - sTime.tv_usec)/1000000.0))/100);

        gettimeofday(&sTime,NULL);
        for(cnt=0; cnt<100; cnt++)
        {
                initiate();
                Test_C();
        }
        gettimeofday(&eTime, NULL);
        printf("Test_C: %f seconds\n", ((eTime.tv_sec - sTime.tv_sec)+((eTime.tv_usec - sTime.tv_usec)/1000000.0))/100);
        gettimeofday(&sTime,NULL);
        for(cnt=0; cnt<100; cnt++)
        {
                initiate();
                Test_D();
        }
        gettimeofday(&eTime, NULL);
        printf("Test_D: %f seconds\n", ((eTime.tv_sec - sTime.tv_sec)+((eTime.tv_usec - sTime.tv_usec)/1000000.0))/100);
        gettimeofday(&sTime,NULL);
        for(cnt=0; cnt<100; cnt++)
        {
                initiate();
                Test_EF();
        }
        gettimeofday(&eTime, NULL);
        printf("Test_EF: %f seconds\n", ((eTime.tv_sec - sTime.tv_sec)+((eTime.tv_usec - sTime.tv_usec)/1000000.0))/100);
        gettimeofday(&sTime,NULL);
        for(cnt=0; cnt<100; cnt++)
        {
                initiate();
                test6();
        }
        gettimeofday(&eTime, NULL);
        printf("test6: %f seconds\n", ((eTime.tv_sec - sTime.tv_sec)+((eTime.tv_usec - sTime.tv_usec)/1000000.0))/100);

	return 0;
}
