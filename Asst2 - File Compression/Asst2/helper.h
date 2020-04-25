// the helper header class for all the methods needed for this project


int total(char * input, int length);
void compress(int fd, char * input, int length, char ** cod, char ** tokens, int size);
void decompress(int fd, char * input, int length, char ** cod, char ** tokens, int size);


typedef struct nodeLi {
	unsigned int oft;
	char * token;
	struct nodeLi * next;
} nodeLi;

void huffman(int size, nodeLi * node, int fd);

typedef struct nodeT {
	char * token;
	unsigned int oft;
	struct nodeT *left, *right;
} nodeT;

typedef struct tree {
	unsigned int size;
	unsigned int mSize;
	nodeT ** nodeArr;
} tree;




