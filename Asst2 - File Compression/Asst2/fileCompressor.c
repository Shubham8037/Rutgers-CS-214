
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<limits.h>
#include<dirent.h>
#include"helper.h"

nodeLi * head = NULL;


int inList(char * token) { // uses a lineked list and adds all the data in its
	nodeLi * temp;
	nodeLi * ptr;
	temp = (nodeLi *)malloc(sizeof(nodeLi));
	if (temp == NULL) {
		printf("Not enough memory left.\n");
		return -1;
	}
	temp->oft = 1;
	temp->token = (char *)malloc(strlen(token) + 1);
	strncpy(temp->token, token, (strlen(token)));

	if (head == NULL) {
		head = temp;
		temp->next = NULL;
		return 1;
	}

	if (strcmp(token, (head->token)) == 0) {

		++head->oft;
		return 0;
	}
	if (temp->oft <= head->oft && head->next == NULL) {

		temp->next = head;
		head = temp;
		return 1;
	}

	ptr = head;
	while (ptr->next != NULL) {
		if(strcmp(ptr->next->token, token) == 0) {
			++ptr->next->oft;
			return 0;
		}
		ptr = ptr->next;
	}

	ptr->next = temp;
	temp->next = NULL;
	return 1;
}

unsigned int tokenize(char * input) { //searches and finds total number of tokens int he book
	if (input == NULL || strlen(input) == 0) {
		return 0;
	}
	int i = 0, j = 0;
	int plsp = 0;
	int tLen = 0;
	int length = strlen(input);
	int lwsp = 1;
	int csp = 1;
	int count = 0;

	for (i = 0; i < length; i++) {//parses through the input 

		if (input[i] != '\t' && input[i] != '\n' && input[i] != ' ') { // checks for delimeters
			++tLen;
			lwsp = 0;
			csp = 0;

		} else if (lwsp == 0) {
			char * token = (char *)malloc(sizeof(char) * (tLen + 1));
			if (token == NULL) {
				printf("Not enough memory left.\n");
				return -1;
			}
			int oft = 0;
			for (j = 0; j < tLen; ++j) {
				token[j] = input[plsp + j];
			}
			token[tLen] = '\0';

			plsp += tLen + 1;
			int inc = inList(token);
			if (inc == -1) {
				return -1;
			}
			count += inc;
			lwsp = 1;
			tLen = 0;
			csp = 1;

		} else {
			++plsp;
			csp = 1;
		}

		if (csp == 1) {
			int inc = 0;

			//takes care of space and tabs here by adding a unique code instead
			if(input[i] == ' ') {
				inc = inList("~)!(@s*#&$^");
				if (inc == -1) {
					return -1;
				}
			} else if (input[i] == '\t') {
				inc = inList("~)!(@t*#&$^");
				if (inc == -1) {
					return -1;
				}
			} else {
				inc = inList("~)!(@n*#&$^");
				if (inc == -1) {
					return -1;
				}
			}
			count += inc;
		}
	}

	//checks for the final charecter and if it is not a delimiter then it adds it
	if (tLen > 0) {
		char * token = (char *)malloc(sizeof(char) * (tLen + 1));
		if (token == NULL) {
			printf("Not enough memory.\n");
			return -1;
		}
		for (j = 0; j < tLen; ++j) {
			token[j] = input[plsp + j];
		}
		token[tLen] = '\0';
		int inc = inList(token);
		if (inc == -1) {
			return -1;
		}
		count += inc;
	}
	return count;
}


void pArray(char ** cod, char ** tokens, char * input, int length) {//goes through the codebook here
	int i = 0, j = 0;
	int whtspcL = 0;
	int stnLength = 0;
	int index = 0;
	for (i = 0; i < length - 1; ++i) {
		if (input[i] == '\t' || input[i] == '\n') {
			char * string = (char *)malloc(sizeof(char) * (stnLength + 1));
			for (j = 0; j < stnLength; ++j) {
				string[j] = input[whtspcL + j];
			}
			string[stnLength] = '\0';
			whtspcL += stnLength + 1;
			stnLength = 0;
			if (input[i] == '\t') { //the tabs will always be surrounded by codes
				cod[index] = (char *)malloc(strlen(string) + 1);
				strncpy(cod[index], string, (strlen(string)));
			} else {
				tokens[index] = (char *)malloc(strlen(string) + 1);
				strncpy(tokens[index], string, (strlen(string)));
				++index;
			}
			free(string);
		} else {
			++stnLength;
		}
	}
}

int recFcn(int fd, char * file, char flag, char ** cod, char ** tokens, int size) { // recursive function for everything
	DIR * direc;
	struct dirent * de;
	int sum = 0;
	if (!(direc = opendir(file))) {
		printf(" Not able to open the file %s.\n", file);
		return -1;
	}
	while ((de = readdir(direc)) != NULL) { // goes through the directories and sub directories to find files

		char nPath[1024];
		snprintf(nPath, sizeof(nPath), "%s/%s", file, de->d_name);
		if (de->d_type == DT_DIR) {	// checks if its a directory or a file. continues if a directory
			if (strcmp(de->d_name, "..") == 0 || strcmp(de->d_name, ".") == 0) {
				continue;
			}
			sum += recFcn(fd, nPath, flag, cod, tokens, size);
		} else { // if the file is found 
			int dirFl = open(nPath, O_RDONLY);
			char * temp = (char *)malloc(sizeof(char) * INT_MAX);
			int cLength = read(dirFl, temp, INT_MAX);
			char * input = (char *)malloc(sizeof(char) * (cLength + 1));
			strcpy(input, temp); 
			free(temp);
			if (flag == 'b') {
				sum += tokenize(input);//total amount of the tokens found in the file
			} else if (flag == 'c') {
				char * hczfile = strcat(nPath, ".hcz");
				int fcnHz = open(hczfile, O_WRONLY | O_CREAT , 0644);
				if (fcnHz < 0) {
					printf("Not able to create/open %s. Therefore skipped %s.\n", hczfile, nPath);
					continue;
				}
				compress(fcnHz, input, cLength, cod, tokens, size);
				close(fcnHz);
			} else if (flag == 'd') {
				char * test = (char *)malloc(sizeof(char) * 5);
				test = nPath + strlen(nPath) - 4;
				if (strcmp(test, ".hcz") != 0) {//only works if its a .hcz file otherwise skips
					continue;
				}
				char * resfile = nPath;
				resfile[strlen(nPath) - 4] = '\0';	
				int fcnDir = open(resfile, O_WRONLY | O_CREAT , 0644);
				if (fcnDir < 0) {
					printf("Not able to create/open %s. Therefore skipped %s.\n", resfile, nPath);
					continue;
				}
				decompress(fcnDir, input, cLength, cod, tokens, size);
				close(fcnDir);
			}
		}
	}
	closedir(direc);
	return sum;
}

int main(int argc, char ** argv) {//main funtion to take the input
	if (argc < 3) { // only works if there are atleast 3 args
		printf("Need more flags or arguments.\n");
		return EXIT_FAILURE;
	} else if (argc > 5) { // fails if arguments are more than 5
		printf("Need less flags or arguments.\n");
		return EXIT_FAILURE;
	}
	int recursive = 0;
	int booFile = 1;
	if (strcmp("-R",argv[1]) == 0 || strcmp("-R",argv[2]) == 0) {
		recursive = 1;
	}
	char * file = argv[3 + (recursive - 1)];
	char flag = ' ';
	if (recursive) {
		if (strcmp("-R", argv[1]) == 0) {
			flag = argv[2][1];
		} else {
			flag = argv[1][1];
		}
	} else {
		flag = argv[1][1];
	}
	if (flag != 'b' && flag != 'c' && flag != 'd') {
		printf("Enter the correct flag (-b, -c, or -d).\n");
		return EXIT_FAILURE;
	}
	if (recursive) {
		struct stat pstat;
		if (stat(file, &pstat) != 0) {
			printf("You don't have the permission or the file does not exist.\n");
			return EXIT_FAILURE;
		}
		booFile = S_ISREG(pstat.st_mode); //checks if the input was correct or not
		if (booFile) {
			int i = i;
			int chcPath = 0;
			for (i = 0; i < strlen(file); ++i) {
				if (file[i] == '/') {
					chcPath = 1;
					break;
				}
		
			}
			if (!chcPath) { // checks if a file was given instead of path as recursivey working on a file is same as working on it non recursively
				printf("File given instead of path.\n");
			}
		}
		if (file[strlen(file) - 1] == '/') {
			file[strlen(file) - 1] = '\0';
		}
	}
	if (booFile) {
		int dirFl = open(file, O_RDONLY);
		if (dirFl < 0) {
			printf("Can't open file \"%s\".\n", file);
			return EXIT_FAILURE;
		}
		char * temp = (char *)malloc(sizeof(char) * INT_MAX);
		int cLength = read(dirFl, temp, INT_MAX);
		char * input = (char *)malloc(sizeof(char) * (cLength + 1));
		strcpy(input, temp);
		free(temp);
		if (flag == 'b') {
			if (argc > (4 + recursive - 1)) {
				printf("Invalid number of flags/arguments. Too many.\n");
				return EXIT_FAILURE;
			} else if (argc < (4 + recursive - 1)) {
				printf("Invalid number of flags/arguments. Not enough.\n");
				return EXIT_FAILURE;
			}
			int fcnCodebook = open("./HuffmanCodebook", O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fcnCodebook < 0) {
				printf("HuffmanCodebook not created. FAILURE.\n");
				return EXIT_FAILURE;
			}

			int size = tokenize(input);//total number of unique tokens
			if (size > 0) {
				huffman(size, head, fcnCodebook);
			} 

			write(fcnCodebook,"\n",1);
			close(fcnCodebook);
		}
		if (flag == 'c' || flag == 'd') {
			if (argc > (5 + recursive - 1)) {
				printf("Invalid number of flags/arguments. Too many.\n");
				return EXIT_FAILURE;
			} else if (argc < (5 + recursive - 1)) {
				printf("Invalid number of flags/arguments. Not enough.\n");
				return EXIT_FAILURE;
			}
			int fcnCodebook = open(argv[argc - 1], O_RDONLY);
			if (fcnCodebook < 0) {
				printf("Can't open the file \"%s\".\n", argv[argc - 1]);
				return EXIT_FAILURE;
			}

			temp = (char *)malloc(sizeof(char) * INT_MAX);
			int lenCode = read(fcnCodebook, temp, INT_MAX);
			char * inCodeb = (char *)malloc(sizeof(char) * (lenCode + 1));
			strcpy(inCodeb, temp);
			free(temp);

			int size = codebookTotal(inCodeb, lenCode);// gets total number of tokens in the codebook
			char ** cod = (char **)malloc(sizeof(char *) * size);
			char ** tokens = (char **)malloc(sizeof(char *) * size);

			pArray(cod, tokens, inCodeb, lenCode);// adds all the tokens from codebook
			if (flag == 'c') {
				char * hczfile = strcat(file, ".hcz");// creates the .hcz file
				int fcnHz = open(hczfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
				if (fcnHz < 1) {
					printf("Can't open the file \"%s\".\n\n", hczfile);
					return EXIT_FAILURE;
				}
				compress(fcnHz, input, cLength, cod, tokens, size);
				close(fcnHz);
			} else {
				char * test = (char *)malloc(sizeof(char) * 5);
				test = file + strlen(file) - 4;
				if (strcmp(test, ".hcz") != 0) {
					printf("Input a valid file with .hcz.\n");
					return EXIT_FAILURE;
				}
				char * resfile = file;
				resfile[strlen(file) - 4] = '\0';	
				int fcnDir = open(resfile, O_WRONLY | O_CREAT , 0644);
				if (fcnDir < 1) {
					printf("Can't open the file \"%s\".\n", resfile);
					return EXIT_FAILURE;
				}
				decompress(fcnDir, input, cLength, cod, tokens, size);
				close(fcnDir);
			}
			close(fcnCodebook);
		}
		close(dirFl);
	} else {// if recursive
		if (flag == 'b') {
			if (argc < 4) {
				printf("Invalid number of flags/arguments. Not enough.\n");
				return EXIT_FAILURE;
			} else if (argc > 4) {
				printf("Invalid number of flags/arguments. Too many.\n");
				return EXIT_FAILURE;
			}
			int fcnCodebook = open("./HuffmanCodebook", O_WRONLY | O_CREAT | O_TRUNC, 0644);	
			if (fcnCodebook < 0) {
				printf("HuffmanCodebook not created. FAILURE.\n");
				return EXIT_FAILURE;
			}
			int size = recFcn(fcnCodebook, file, flag, NULL, NULL, 0);//creates one codebook for all the files
			if (size > 0) {
				huffman(size, head, fcnCodebook);	
			} 
			write(fcnCodebook,"\n",1);
			close(fcnCodebook);
		} else if (flag == 'c' || flag == 'd') {
			if (argc < 5) {
				printf("Invalid number of flags/arguments. Not enough.\n");
				return EXIT_FAILURE;
			} else if (argc > 5) {
				printf("Invalid number of flags/arguments. Too many.\n");
				return EXIT_FAILURE;
			}
			int fcnCodebook = open(argv[argc - 1], O_RDONLY);
			if (fcnCodebook < 0) {
				printf("Can't open the file \"%s\".\n", argv[argc - 1]);
				return EXIT_FAILURE;
			}
			char * temp = (char *)malloc(sizeof(char) * INT_MAX); // gets the entire codebook in a string
			int lenCode = read(fcnCodebook, temp, INT_MAX);
			char * inCodeb = (char *)malloc(sizeof(char) * (lenCode + 1));
			strcpy(inCodeb, temp);
			free(temp);
			int size = codebookTotal(inCodeb, lenCode);
			char ** cod = (char **)malloc(sizeof(char *) * size);
			char ** tokens = (char **)malloc(sizeof(char *) * size);
			pArray(cod, tokens, inCodeb, lenCode);
			recFcn(0, file, flag, cod, tokens, size);
			free(cod);
			free(tokens);
			close(fcnCodebook);
		}
	}
	free(head);
	return EXIT_SUCCESS;
}
