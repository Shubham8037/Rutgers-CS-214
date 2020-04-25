#include<string.h>
#include<stdio.h>
#include<stdlib.h>


int codebookTotal(char * input, int length) { // gets the total lines in codebook
	int count = 0;
	int i = 0;
	for (i = 0; i < length; ++i) {
		if (input[i] == '\t') { // gets the amount of tabs 
			++count;
		}
	}
	return count;
}

int search(char ** arr, int size, char * string) { // looks for the string
	int i = 0;
	for (i = 0; i < size; ++i) {
		if (strcmp(arr[i], string) == 0) {
			return i;
		}
	}
	return -1;
}

void compress(int fd, char * input, int length, char ** cod, char ** tokens, int size) { // creates the hcz file using the codebook
	int i = 0, j = 0;
	int lToken = 0;
	int whtspcL = 0;

	//speacial charecters were used to find spaces tabs

	int sIndex = search(tokens, size, "~)!(@s*#&$^"); 
	int taIndex = search(tokens, size, "~)!(@t*#&$^");
	int nlIndex = search(tokens, size, "~)!(@n*#&$^");
	for (i = 0; i < length; ++i) {

		if (input[i] == '\t' || input[i] == '\n' || input[i] == ' ') {
			if (lToken > 0) {
				char * string = (char *)malloc(sizeof(char) * (lToken + 1));
				for (j = 0; j < lToken; ++j) {
					string[j] = input[whtspcL + j];
				}
				string[lToken] = '\0';
				whtspcL += lToken + 1;
				lToken = 0;
				write(fd, cod[search(tokens, size, string)], strlen(cod[search(tokens, size, string)])); // writes the tokens that were created
				free(string);
			} else {
				++whtspcL;
			}

			if (input[i] == ' ') { // whitespaces are dealt with here
				write(fd, cod[sIndex], strlen(cod[sIndex]));
			} else if (input[i] == '\t') {
				write(fd, cod[taIndex], strlen(cod[taIndex]));
			} else {
				write(fd, cod[nlIndex], strlen(cod[nlIndex]));
			}

		} else {
			++lToken;
		}
	}
}


void decompress(int fd, char * input, int length, char ** cod, char ** tokens, int size) { // checks the codebook and the hcz file to decompress and create/overwrite a new file
	int i = 0, j = 0;
	int brkL = 0;
	int codLength = 0;
	for (i = 0; i < length; ++i) {
		++codLength;
		char * code = (char *)malloc(sizeof(char) * (codLength + 1));
		for (j = 0; j < codLength; ++j) {
			code[j] = input[brkL + j];
		}
		code[codLength] = '\0';
		int index = search(cod, size, code);// chearches the codebook for the codes
		if (index >= 0) {
			if (strcmp(tokens[index], "~)!(@s*#&$^") == 0) { // Spaces and tabs are dealt with here
				write(fd, " ", strlen(" "));
			} else if (strcmp(tokens[index], "~)!(@t*#&$^") == 0) {
				write(fd, "\t", strlen("\t"));
			} else if (strcmp(tokens[index], "~)!(@n*#&$^") == 0) {
				write(fd, "\n", strlen("\n"));
			} else {
				write(fd, tokens[index], strlen(tokens[index]));
			}
			brkL += codLength;
			codLength = 0;
		}
		free(code);
	}
}
