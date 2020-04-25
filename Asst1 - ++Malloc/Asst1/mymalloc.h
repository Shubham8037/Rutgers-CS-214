#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define malloc(x) mymalloc(x,__FILE__,__LINE__)
#define free(x) myfree(x,__FILE__,__LINE__)
#define MAX_MEM_SIZE 4096
void *mymalloc(size_t memsize, char* logfile, int line);
void myfree(void *clean, char *logfile, int line);
void intitiate();
