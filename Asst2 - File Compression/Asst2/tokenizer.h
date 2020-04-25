
int total(char * input, int length);
void compress(int fd, char * input, int length, char ** codes, char ** tokens, int size);
void decompress(int fd, char * input, int length, char ** codes, char ** tokens, int size);
