
int total_connections = 0;

int active_connections = 0;

void gettok(char*** toks, char* buff){
	
	int i,c=1;
	for(i=0; i<strlen(buff); i++)
		if(buff[i] == ':')
			c++;
	
	*toks = malloc(sizeof(char*) * c);
	for(i=0; i<c; i++)
		(*toks)[i] = malloc(sizeof(char*) * 1000);
	
	char *tokken, *rest=buff;
	
	i=0;
	while((tokken = strtok_r(rest, ":", &rest))){
		strcpy((*toks)[i], tokken);
		i++;
	}
}

int check_dir(char* path){
	DIR* dir = opendir(path);
	if (dir)
	{
		/* Directory exists. */
		closedir(dir);
		return 1;
	}
	else if (ENOENT == errno)
	{
		/* Directory does not exist. */
		return 0;
	}
	else
	{
		/* opendir() failed for some other reason. */
		return -1;
	}
}

int str_cmp(char *a, char *b){
	if(strlen(a) != strlen(b))
		return 0;
	
	int i;
	for(i=0; i<strlen(a); i++)
		if(a[i] != b[i])
			return 0;
	
	return 1;
	
}

int rem_line(char* file_name, char* key){
	char *data = NULL;
	char ** temp;
	char tok[1000];
	char output[5000];
	size_t len = 0;
	FILE* file = fopen(file_name, "r");
	
	if(file == NULL){
		printf("Error in opening file %s: \n", file_name);
		return -1;
	}
	
	while((getline(&data, &len, file)) > 1){
		strcpy(tok, data);
		gettok(&temp, data);
		if( (str_cmp(temp[0], key) == 0) ){
			strcat(output, tok);
		}
	}
	
	fclose(file);
	int fd = open(file_name, O_RDWR | O_TRUNC);
	dprintf(fd, "%s",output);
	close(fd);
	if(data != NULL)
		free(data);
}

int get_curr_ver(char* file_name, char** output){
	
	char *data = NULL;
	char ** temp = NULL;

	size_t len = 0;
	FILE* file = fopen(file_name, "r");
	
	if(file == NULL){
		printf("Error in opening file %s: \n", file_name);
		return -1;
	}
	
	while((getline(&data, &len, file) != -1)){
		gettok(&temp, data);
		
		if(temp[0][0] == '.' && temp[0][1] == '/'){
		strcat((*output), " \n");
		strcat((*output), "file: ");
		strcat((*output), temp[0]);
		strcat((*output), "\tversion: ");
		strcat((*output), temp[1]);
		}
	}
	
	fclose(file);
	if(data != NULL)
		free(data);
	if(temp != NULL)	
		free(temp);

}

void make_path(char** path, char* project, char* file, int f){
	
	strcat((*path), project);
	if(f==0){
		strcat((*path), "/repos");
		if(file != NULL)
			strcat((*path), file);
	}
	else{
		strcat((*path), "/source");
		if(file != NULL)
			strcat((*path), file);
	}
		
}

void make_Manifest(char* type, char* project, char* f_name){
	
	char path[500], path2[500], path3[500], sys[1000], buffer[10000];
	char path4[500];
	strcpy(path, type);
	strcat(path, project);
	strcpy(path2, path);
	strcat(path2, "/source");
	strcpy(path3, path2);
	strcat(path, "/repos");
	strcat(path, f_name);
	
	int fd = open(path, O_CREAT | O_TRUNC, 0666);
	close(fd);
	fd = open(path, O_RDWR | O_APPEND);
	
	if(fd < 0){
		printf("Error in opening %s: \n", path);
		exit(0);
	}
	
	struct dirent *dp;
	DIR *dir = opendir(path2);
	if (dir){
		while ((dp = readdir(dir)) != NULL){
			if (str_cmp(dp->d_name, ".") == 0 && str_cmp(dp->d_name, "..") == 0){
				
				strcpy(path2, path3);
				strcpy(path4, path2);
				strcat(path2, "/");
				strcat(path2, dp->d_name);
				
				strcat(path4, ":/");
				strcat(path4, dp->d_name);
				
				dprintf(fd, "%s:1", path4);  
				
				strcpy(sys, "sha1sum ");
				strcat(sys, path2);
				strcat(sys, " > lun.txt");
				system(sys);
				
				int fd2 = open("lun.txt", O_RDONLY);
				read(fd2, buffer, sizeof(buffer));
				
				dprintf(fd, ":%s\n", buffer);
				  
				close (fd2);
				//system("rm lun.txt");
			}
		}
	}
	else
		printf("Error no directory by name of %s: \n", path2);

	close (fd);
	closedir(dir);
}

//these are for checkout
void swap(char *x, char *y) {
	char t = *x; *x = *y; *y = t;
}

char* reverse(char *buffer, int i, int j){
	while (i < j)
		swap(&buffer[i++], &buffer[j--]);

	return buffer;
}

char* itoa(int value, char* buffer, int base){
	if (base < 2 || base > 32)
		return buffer;

	int n = abs(value);

	int i = 0;
	while (n){
		int r = n % base;

		if (r >= 10) 
			buffer[i++] = 65 + (r - 10);
		else
			buffer[i++] = 48 + r;

		n = n / base;
	}

	if (i == 0)
		buffer[i++] = '0';
	if (value < 0 && base == 10)
		buffer[i++] = '-';
	buffer[i] = '\0'; // null terminate string
	return reverse(buffer, 0, i - 1);
}

void creat_proj(char* path, char* proj){
	
	int f = 0;
	char cli_dir[500];
	strcpy(cli_dir, path);
	strcat(cli_dir, proj);
	
	if(check_dir(cli_dir) == 1){
		
		strcat(cli_dir, "copy1");
		f = 1;
	}
	
	mkdir(cli_dir, 0700);
	strcat(cli_dir, "/repos");
	mkdir(cli_dir, 0700);
	strcat(cli_dir, "/Manifest.txt");
	creat(cli_dir, S_IRWXU | S_IRWXG);
	
	strcpy(cli_dir, path);
	strcat(cli_dir, proj);
	if(check_dir(cli_dir) == 1 && f==1){
		
		strcat(cli_dir, "copy1");
	}
	strcat(cli_dir, "/source");
	mkdir(cli_dir, 0700);
}

void download(char* path, char* proj, char* buff){
	int i, j, noOfFiles,buffIndex = 0, nameSize = 0, fNo = 0;
	char fName[200];
	char* data = malloc(4096*4096);
	char adrs[500];
	for(i = 0; buff[buffIndex] != ':'; i++, buffIndex++)
	{	
		fName[i] = buff[buffIndex];
		printf("%c", buff[buffIndex]);
	}
	fName[i] = '\0';
	noOfFiles = atoi(fName);
	int fd[noOfFiles];
	int fSize[noOfFiles];
	for(i = 0; i < noOfFiles; i++)
	{
		buffIndex++;
		for(j= 0; buff[buffIndex] != ':'; j++, buffIndex++){	
			fName[j] = buff[buffIndex];
		}
		
		fName[j] = '\0';
		nameSize  = atoi(fName);
		buffIndex++;
		
		for(j = 0; j < nameSize; j++, buffIndex++){
			fName[j] = buff[buffIndex];
		}
		fName[j] = '\0';
		strcpy(adrs, path);
		strcat(adrs, proj);
		strcat(adrs, "/source/");
		strcat(adrs, fName);
		fd[fNo]  = open(adrs, O_RDWR | O_CREAT, 0666);
		buffIndex++;
		
		for(j= 0; buff[buffIndex] != ':'; j++, buffIndex++){	
			fName[j] = buff[buffIndex];
		}
		fName[j] = '\0';
		fSize[fNo] = atoi(fName);
		fNo++;
	}
	buffIndex++;
	for(i = 0; i < noOfFiles; i++)
	{
		for(j = 0; j < fSize[i]; j++, buffIndex++){
			data[j] = buff[buffIndex];
		}
		data[j] = '\0';
		write(fd[i], data, fSize[i]);
	}

}

int checkout(char** output, char* dirName, char*proj){
	int noOfFiles = 0, fd = 0;
	char  list[2000], temp[500], temp1[200], file_name[500];
	char filedata[100000];
	char filedata1[100000];
	
	char *data = malloc(4096*4096);
	
	make_path(&dirName, proj, NULL, 1);
	
	struct dirent *dp;
	DIR *dir = opendir(dirName);
	if (dir)
	{
		while ((dp = readdir(dir)) != NULL)
		{
			if (str_cmp(dp->d_name, ".") == 0 && str_cmp(dp->d_name, "..") == 0)
			{
				strcat(list, dp->d_name);
				strcat(list, " ");
				noOfFiles++;
			}
		}
		closedir(dir);
		
	
		strcat(data, itoa(noOfFiles, temp1, 10)); //itoa here
		strcat(data, ":");
		int i = 0, bytes = 0;
		while(list[i] != '\0')
		{
			strcpy(file_name, dirName);
			strcat(file_name, "/");
	
			if(list[i] != ' ')
			{
				temp[bytes] = list[i];
				bytes++;
				temp[bytes] = '\0';
			}
			else
			{
				strcat(data, itoa(bytes, temp1, 10)); 
				strcat(data, ":");
				strcat(data, temp);
				strcat(data, ":");
				
				strcat(file_name, temp);
				fd = open(file_name, O_RDWR);
				if(fd == -1){
					printf("Error: %s\n", strerror(errno));
					exit(0);
				}
				bytes = read(fd, filedata, sizeof(filedata));
				strcat(data, itoa(bytes, temp1, 10));
				strcat(data, ":");
				strcat(filedata1, filedata);
				close(fd);
				bytes = 0;
			}
			i++;
		}
		strcat(data, filedata1);
		(*output) = data;
		return 1;
	}
	else
		return -1;
}


// for update, commit, upgrade
int no_of_lines(char* path, char* file_name){
	
	
	char lpc[500];
	strcpy(lpc, path);
	strcat(lpc, file_name);
	
	char *data = NULL;
	char ** temp = NULL;
	size_t len = 0;
	FILE* file = fopen(lpc, "r");
	int lines = 0;
	
	if(file == NULL){
		printf("Error in opening file %s: \n", lpc);
		return -1;
	}
	while((getline(&data, &len, file) != -1)){
		gettok(&temp, data);
		if(temp[0][0] == '.' && temp[0][1] == '/'){
			
				lines++;
		}
	}
	
	fclose(file);
	if(data != NULL)
		free(data);
	if(temp != NULL)
		free(temp);
	return lines;
}

int file_parser(char ****output , char* path, char *file_name){
	
	int lines = no_of_lines(path, file_name);
	
	(*output) = malloc(sizeof(char**) * lines);
	int i;
	
	char lpc[500];
	strcpy(lpc, path);
	strcat(lpc, file_name);
	
	char *data = NULL;
	char ** temp = NULL;
	size_t len = 0;
	FILE* file = fopen(lpc, "r");
	
	if(file == NULL){
		printf("Error in opening file %s: \n", lpc);
		return -1;
	}
	i=0;
	while(i!=lines){
		getline(&data, &len, file);
		gettok(&temp, data);
		if(temp[0][0] == '.' && temp[0][1] == '/'){
			(*output)[i] = temp;
			i++;
		}
	}
	
	fclose(file);
	if(data != NULL)
		free(data);
	return lines;
}

void cmp_files(char* path1, char* path2, char* file){
	
	char ***data1;
	char ***data2;
	
	int l1 = file_parser(&data1, path1, file);
	int l2 = file_parser(&data2, path2, file);
	
	char update_path[500];
	strcpy(update_path, path1);
	strcat(update_path, "/Update.txt");
	int fd = open(update_path, O_CREAT | O_RDWR | O_APPEND | O_TRUNC, 0666);
	if(fd < 0){
		printf("Error in opening file: %s\n", update_path);
		exit(0);
	}
	
	int i, j, f=0, flag = 0;
	for(i=0; i<l1; i++){
		for(j=0; j<l2; j++){
			
			if(str_cmp(data1[i][1], data2[j][1]) && atoi(data1[i][2])>atoi(data2[j][2])){
				printf("U %s%s\n", data1[i][0], data1[i][1]);
				f = 1; flag = 1;
				break;
			}
			else if(str_cmp(data1[i][1], data2[j][1]) && atoi(data1[i][2])<atoi(data2[j][2])){
				printf("M %s%s\n", data1[i][0], data1[i][1]);
				dprintf(fd,"%s:%s:M:%s:%s\n", data1[i][0], data1[i][1],data1[i][2], data1[i][3]);
				f = 1; flag = 1;
				break;
			}
			else if(str_cmp(data1[i][1], data2[j][1])){
				f = 1;
				break;
			}
		}
		if(f == 0){
			printf("D %s%s\n", data1[i][0], data1[i][1]);
			dprintf(fd,"%s:%s:D:%s:%s\n", data1[i][0], data1[i][1],data1[i][2], data1[i][3]);
			printf("Please DELETE this file than try again for UPDATE\n");   
			flag = 1;
		}
		f = 0;
	}
	
	f = 0;
	for(i=0; i<l2; i++){
		for(j=0; j<l1; j++){
			if(str_cmp(data2[i][1], data1[j][1]) == 1){
				f = 1;
				break;
			}
		}
		if(f == 0){
			printf("A %s%s\n", data2[i][0], data2[i][1]);
			dprintf(fd,"%s:%s:A:%s:%s\n", data2[i][0], data2[i][1],data2[i][2], data2[i][3]);
			printf("Please ADD this file than try again for UPDATE\n");   
			flag = 1;
		}
		f = 0;
	}
	
	if(flag == 0)
		printf("Your Project is already up-to-date\n");
	close(fd);
	free(data1);
	free(data2);
}

int no_of_mod(char *** data, int lines){
	
	int i, l = 0;
	for(i=0; i<lines; i++){		
		if(str_cmp(data[i][2], "M") || str_cmp(data[i][2], "A")){
			l++;
		}
	}
	return l;
}

void cmp_commit(char* path1, char* path2, char* file1, char* file2){
	
	char ***data1;
	char ***data2;
	
	int l1 = file_parser(&data1, path1, file1);
	int l2 = file_parser(&data2, path2, file2);
	
	char update_path[500];
	strcpy(update_path, path1);
	strcat(update_path, "/Commit.txt");
	int fd = open(update_path, O_CREAT | O_RDWR | O_APPEND | O_TRUNC, 0666);
	if(fd < 0){
		printf("Error in opening file: %s\n", update_path);
		exit(0);
	}
	
	int i, j, f=0, flag = 0;
	for(i=0; i<l1; i++){
		for(j=0; j<l2; j++){
			
			if(str_cmp(data1[i][1], data2[j][1]) && !str_cmp(data1[i][3], data2[j][3])){
				flag = atoi(data1[i][2]);
				flag++;
				data1[i][2] = itoa(flag, data1[i][2], 10);
				dprintf(fd, "%s:%s:%s:%s\n", data1[i][0], data1[i][1], data1[i][2], data1[i][3]);
				f = 1;
				break;
			}
		}
		f = 0;
	}
	printf("Created .Commit file\n");
	close(fd);
	free(data1);
	free(data2);
}


