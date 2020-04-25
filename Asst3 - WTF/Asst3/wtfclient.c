#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <dirent.h>
#include <errno.h>

#include "help_func.h"


int connection(int* sock, struct sockaddr_in address, char* ip, char* port){
    
    *sock = socket(AF_INET, SOCK_STREAM, 0);
	if(*sock < 0){
	printf("Error in creating socket\n");
	return -1;
	}
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(ip);
	address.sin_port = htons(atoi(port));

	int f=0;
	do
	{	
		f=connect(*sock, (struct sockaddr*)&address, sizeof(address));
		if(f<0){
			printf("Error can not find the server.\n");
			sleep(3);
		}	
	}while(f < 0);

}
 
 
int main(int argc, char* arg[]) { 
	
	int sockfd, fd;
    char buffer[10000],  msg[1000], sys[500];
    char *cli_dir = malloc(sizeof(char)*1000);
    char *temp_dir = malloc(sizeof(char)*1000);
    strcpy(cli_dir, "./client1");
    strcpy(temp_dir, cli_dir);
    struct sockaddr_in     servaddr; 
	size_t n;
    socklen_t len; 
    
    memset(buffer, 0, sizeof(buffer));
    memset(msg, 0, sizeof(msg));
	
	if(argc < 3){
		printf("Error, Invalid Arguments !!!\n");
		return -1;
	}
	
	if(str_cmp(arg[1], "configure")){
		if(argc < 4){
			printf("Error, follow this: ./WTF configure <IP> <port>\n");
			return -1;
		}
		
		if(connection(&sockfd, servaddr, arg[2], arg[3]) == -1){
			printf("Error in connecting with server\n");
			return -1;
		}
		printf("Connect to Server with ip: %s, port: %s\n", arg[2], arg[3]);
		
		//save the ip and port in .config.txt for later use
		int fd = open("config.txt", O_CREAT| O_RDWR | O_APPEND, S_IRWXU);
		dprintf(fd, "%s %s\n", arg[2], arg[3]);
		total_connections++;
		
		char *hello = "configure";
		printf("Message sent: %s\n", hello); 
		sendto(sockfd, hello, strlen(hello), 0, (struct sockaddr*)NULL, sizeof(servaddr)); 
      
		// waiting for response 
		recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL); 
		printf("Reply received: %s\n", buffer); 
		close(fd);
	}
	else{
		FILE* fs = fopen("config.txt", "r");
		char ip[15], port[5];
		fscanf(fs, "%s %s", ip, port);
		
		if(connection(&sockfd, servaddr, ip, port) == -1){
			printf("Error in connecting with server\n");
			return -1;
		}
		printf("Connect to Server with ip: %s, port: %s\n", ip, port);
		fclose(fs);
	}
  
	
	if(str_cmp(arg[1], "checkout")){
		if(argc < 3){
			printf("Error, follow this: ./WTF checkout <project name>\n");
			return -1;
		}
		strcat(temp_dir, arg[2]);
		if(check_dir(temp_dir) == 1){
			printf("sorry, there is already a project by this name.\n");
			return -1;
		}		
		strcpy(temp_dir, cli_dir);

		strcpy(msg, "checkout:");
		strcat(msg, arg[2]);
		printf("Message sent: %s\n", msg); 
		sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)NULL, sizeof(servaddr)); 
		  
		// waiting for response
		char *chk_out = malloc(4096*4096); 
		recvfrom(sockfd, chk_out, 4096*4096, 0, (struct sockaddr*)NULL, NULL);
		
		creat_proj(cli_dir, arg[2]);
		download(cli_dir, arg[2], chk_out);
		make_Manifest(cli_dir, arg [2], "/Manifest.txt");
		
		printf("Successfully created project: %s\n", arg[2]);
		free(chk_out);
	}
	
	else if(str_cmp(arg[1], "update")){
		if(argc < 3){
			printf("Error, follow this: ./WTF update <project name>\n");
			return -1;
		}
		strcpy(msg, "update:");
		strcat(msg, arg[2]);
		printf("Message sent: %s\n", msg); 
		sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)NULL, sizeof(servaddr)); 
		  
		// waiting for response 
		recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL);
		
		make_Manifest(cli_dir, arg [2], "/Manifest.txt");
		make_path(&temp_dir, arg[2], NULL, 0);
		cmp_files(temp_dir, buffer, "/Manifest.txt");		 
	}
	
	else if(str_cmp(arg[1], "upgrade")){
		if(argc < 3){
			printf("Error, follow this: ./WTF upgrade </project name>\n");
			return -1;
		}
		
		make_path(&temp_dir, arg[2], "/Update.txt", 0);
		int fd = open(temp_dir, O_RDONLY);
		if(fd < 0){
			printf("Error!!! No .Update exists, please do an update first.\n");
			close(fd);
			exit(0);
		}
		close(fd);
		strcpy(temp_dir, cli_dir);
				
		strcpy(msg, "upgrade:");
		strcat(msg, arg[2]);
		printf("Message sent: %s\n", msg); 
		sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)NULL, sizeof(servaddr)); 	  
		// waiting for response 
		recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL); 
		printf("Reply received: %s\n", buffer);
		
		if(!str_cmp("OK" , buffer))
			exit(0);
			
		char ***data;
		make_path(&temp_dir, arg[2], NULL, 0);
		int lines = file_parser(&data, temp_dir, "/Update.txt");
		strcpy(temp_dir, cli_dir);
		
		int modifications = no_of_mod(data, lines);
		strcpy(msg, itoa(modifications, msg, 10));
		sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)NULL, sizeof(servaddr)); 	  
		
		int i;
		for(i=0; i<lines; i++){
						
			if(str_cmp(data[i][2], "D")){
				strcpy(temp_dir, "rm "); 
				strcat(temp_dir, data[i][0]);
				strcat(temp_dir, data[i][1]);
				system(temp_dir);
				printf("File Deleted: %s\n", temp_dir);
				strcpy(temp_dir, cli_dir);
			}
			else if(str_cmp(data[i][2], "M") || str_cmp(data[i][2], "A")){
				strcpy(msg, arg[2]);
				strcat(msg, ":");
				strcat(msg, data[i][1]);
				sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)NULL, sizeof(servaddr)); 	  
				// waiting for response 
				recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL); 
				
				make_path(&temp_dir, arg[2], data[i][1], 1);
				if(str_cmp(data[i][2], "M"))
					fd  = open(temp_dir, O_RDWR | O_TRUNC);
				else
					fd  = open(temp_dir, O_RDWR | O_CREAT, 0666);
				if(fd < 0){
					printf("No file exists: %s\n", temp_dir);
					close(fd);
					exit(0);
				}
				write(fd, buffer, strlen(buffer));
				printf("File Modified: %s\n", temp_dir);
				strcpy(temp_dir, cli_dir);
				close(fd);
			}
			
		}	
		free(data);
		
		char str[500];
		strcpy(str, "rm ");
		make_path(&temp_dir, arg[2], "/Update.txt", 0);
		strcat(str, temp_dir);
		system(str);
	}
	
	else if(str_cmp(arg[1], "commit")){
		if(argc < 3){
			printf("Error, follow this: ./WTF commit </project name>\n");
			return -1;
		}
		make_path(&cli_dir, arg[2], "/Update.txt", 0);
		int fd = open(cli_dir, O_RDONLY);
		if(fd >= 0){
			printf("Error!!! .Update exists\n");
			close(fd);
			exit(0);
		}
		close(fd);
		strcpy(cli_dir, "./client1");
		
		strcpy(msg, "commit:");
		strcat(msg, arg[2]);
		printf("Message sent: %s\n", msg); 
		sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)NULL, sizeof(servaddr)); 
		  
		// waiting for response 
		recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL);
		
		//make new .Manifest
		make_Manifest(cli_dir, arg [2], "/Manifest1.txt");
		make_path(&temp_dir, arg[2], NULL, 0);
		
		//make comitts 
		cmp_commit(temp_dir, temp_dir, "/Manifest1.txt", "/Manifest.txt");
		strcat(temp_dir, "/Manifest1.txt");
		char lun[500];
		strcpy(lun, "rm ");
		strcat(lun, temp_dir);
		system(lun);
		
		make_path(&cli_dir, arg[2], "/Commit.txt", 0);
		fd = open(cli_dir, O_RDONLY);
		if(fd < 0){
			printf("Error!!! can not open: %s\n", cli_dir);
			close(fd);
			exit(0);
		}
		read(fd, buffer, sizeof(buffer));
			sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&servaddr, sizeof(servaddr)); 
		printf("Send .Commit file to Server\n");
		close(fd);
	}
	
	else if(str_cmp(arg[1], "push")){
		if(argc < 3){
			printf("Error, follow this: ./WTF push </project name>\n");
			return -1;
		}
		
		strcat(temp_dir, arg[2]);
		if(check_dir(temp_dir) == 1){
			
			strcpy(msg, "push:");
			strcat(msg, arg[2]);
			printf("Message sent: %s\n", msg); 
			sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)NULL, sizeof(servaddr)); 
			
			recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL);
			
			strcpy(temp_dir, cli_dir);
			char *chk_data;
			if(checkout(&chk_data, temp_dir, arg[2]) == -1){
				printf("Error in PATH\n");
				exit(0);
			}
			sendto(sockfd, chk_data, strlen(chk_data), 0, 
				(struct sockaddr*)&servaddr, sizeof(servaddr)); 
			
			printf("Send the data of project: %s\n", arg[2]);
			free(chk_data);
			
			//deletes .Commit file
			char str[500];
			strcpy(str, "rm ");
			strcpy(temp_dir, cli_dir);
			make_path(&temp_dir, arg[2], "/Commit.txt", 0);
			strcat(str, temp_dir);
			system(str);
		}
		else if(check_dir(temp_dir) == 0){
			strcpy(msg, "sorry, there is no project by this name.");
			printf("Request Rejected\n");
			sendto(sockfd, msg, strlen(msg), 0, 
				(struct sockaddr*)&servaddr, sizeof(servaddr)); 
		}
				
		
	}
	
	else if(str_cmp(arg[1], "create")){
		if(argc < 3){
			printf("Error, follow this: ./WTF create </project name>\n");
			return -1;
		}
		
		strcpy(msg, "create:");
		strcat(msg, arg[2]);
		printf("Message sent: %s\n", msg); 
		sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)NULL, sizeof(servaddr)); 
		  
		// waiting for response 
		recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL); 
		printf("Reply received: %s\n", buffer);
		if(str_cmp(buffer, "created")){
			
			creat_proj(cli_dir, arg[2]);
		} 	  
	}
	
	else if(str_cmp(arg[1], "destroy")){
		if(argc < 3){
			printf("Error, follow this: ./WTF destroy </project name>\n");
			return -1;
		}
		strcpy(msg, "destroy:");
		strcat(msg, arg[2]);
		printf("Message sent: %s\n", msg); 
		sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)NULL, sizeof(servaddr)); 
		  
		// waiting for response 
		recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL); 
		printf("Reply received: %s\n", buffer);

	}
	
	else if(str_cmp(arg[1], "add")){
		if(argc < 4){
			printf("Error, follow this: ./WTF add </project name> </filename>\n");
			return -1;
		}
		strcat(temp_dir, arg[2]);
		if(check_dir(temp_dir) == 0){
			printf("sorry, there is no project by this name.");
			return -1;
		}
		strcpy(temp_dir, cli_dir);
		
		make_path(&temp_dir, arg[2], "/Manifest.txt", 0);
		fd = open(temp_dir, O_RDWR | O_APPEND);
		if(fd < 0){
			printf("Error in opening Manifest.txt\n");
			return -1;
		}
		
		strcpy(temp_dir, cli_dir);
		make_path(&temp_dir, arg[2], arg[3], 1);
		dprintf(fd, "%s:1", temp_dir);  
		
		strcpy(sys, "sha1sum ");
		strcat(sys, temp_dir);
		strcat(sys, " > lun.txt");
		system(sys);
		
		int fd2 = open("lun.txt", O_RDONLY);
		read(fd2, buffer, sizeof(buffer));
		
		dprintf(fd, ":%s\n", buffer);
		  
		close (fd);
		close (fd2);
		//system("rm lun.txt");
		printf("File added to Manifest.txt: %s%s\n", arg[2],arg[3]);
	}
	
	else if(str_cmp(arg[1], "remove")){
		if(argc < 4){
			printf("Error, follow this: ./WTF remove </project name> </filename>\n");
			return -1;
		}
		strcat(temp_dir, arg[2]);
		if(check_dir(temp_dir) == 0){
			printf("sorry, there is no project by this name.");
			return -1;
		}
		strcpy(temp_dir, cli_dir);
		
		char *file = malloc(sizeof(char)*1000);
		strcpy(file, temp_dir);
		
		make_path(&temp_dir, arg[2], "/Manifest.txt", 0);
		make_path(&file, arg[2], arg[3], 1);
		
		rem_line(cli_dir, file);
		free(file);
		printf("File Removed from Manifest.txt: %s%s\n", arg[2],arg[3]);
	}
	
	else if(str_cmp(arg[1], "currentversion")){
		if(argc < 3){
			printf("Error, follow this: ./WTF add <project name>\n");
			return -1;
		}
		strcpy(msg, "currentversion:");
		strcat(msg, arg[2]);
		printf("Message sent: %s\n", msg); 
		sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)NULL, sizeof(servaddr)); 
		  
		// waiting for response 
		recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL); 
		printf("Reply received: %s\n", buffer);
	}
	
	else if(str_cmp(arg[1], "history")){
		if(argc < 3){
			printf("Error, follow this: ./WTF history </project name>\n");
			return -1;
		}
	}
	
	else if(str_cmp(arg[1], "rollback")){
		if(argc < 4){
			printf("Error, follow this: ./WTF rollback </project name>\n");
			return -1;
		}
		strcpy(msg, "rollback:");
		strcat(msg, arg[2]);
		strcat(msg, ":");
		strcat(msg, arg[3]);
		printf("Message sent: %s\n", msg); 
		sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)NULL, sizeof(servaddr)); 
		  
		// waiting for response 
		recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)NULL, NULL); 
		printf("Reply received: %s\n", buffer);
	}
	
	else if(!str_cmp(arg[1], "configure")){
		printf("Error, Invalid Arguments !!!\n");
		return -1;
	}

    free(cli_dir);
    free(temp_dir);
    close(sockfd); 
    return 0; 
} 
