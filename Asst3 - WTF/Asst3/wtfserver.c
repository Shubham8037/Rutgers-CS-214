
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <fcntl.h>  
#include <string.h>
#include <wait.h> 
#include <pthread.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <dirent.h>
#include <errno.h>
#include "help_func.h"

pthread_mutex_t lock;


void initializeServer(int *sockfd, struct sockaddr_in address, int port){

  *sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(*sockfd < 0){
    printf("Error in creating socket\n");
    exit(0);
  }
  
  memset(&address, 0, sizeof(address));
  
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  address.sin_port = htons(port);
  
  int one = 1;
  setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
  if(bind(*sockfd, (struct sockaddr*)&address, sizeof(address)) < 0) {
    printf("Error in Binding(simulator.c)\n");
    exit(0);
  }
  
  if(listen(*sockfd, 20) < 0){
    printf("Error in Listening(simulator.c)\n");
    exit(0);
  }
}
   
void* handle_req(void* para){
	
	int sockfd = *(int*)para;
	
	char buffer[1000]; 
    char hello[1000]; 
    char *srv_dir = malloc(sizeof(char)* 100);
    strcpy(srv_dir, "./server");
	struct sockaddr_in cliaddr;
	
	int len = sizeof(cliaddr); 
    int n = recvfrom(sockfd, buffer, sizeof(buffer), 
            0, (struct sockaddr*)&cliaddr,&len); //receive message from server 
    buffer[n] = '\0'; 
    printf("Command Received: %s\n", buffer);
    
    char** commands;
    gettok(&commands, buffer);
    
    //checks the command received from client
    if(str_cmp(commands[0], "configure")){
		
		strcpy(hello, "Okay");	
		sendto(sockfd, hello, strlen(hello), 0, 
          (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
    }
    
     else if(str_cmp(commands[0], "checkout")){
		
		strcat(srv_dir, commands[1]);
		if(check_dir(srv_dir) == 1){
			
			strcpy(srv_dir, "./server");
			char *chk_data;
			
			if(checkout(&chk_data, srv_dir, commands[1]) == -1){
				printf("Error in PATH\n");
				exit(0);
			}
			sendto(sockfd, chk_data, strlen(chk_data), 0, 
				(struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
			
			strcpy(srv_dir, "./server");
			make_Manifest(srv_dir, commands[1], "/Manifest.txt");
			printf("Send the data of project: %s\n", commands[1]);
			free(chk_data);
		}
		else if(check_dir(srv_dir) == 0){
			strcpy(hello, "sorry, there is no project by this name.");
			printf("Request Rejected\n");
			sendto(sockfd, hello, strlen(hello), 0, 
				(struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
		}		
    }
    
    else if(str_cmp(commands[0], "update")){
		
		strcat(srv_dir, commands[1]);
		if(check_dir(srv_dir) == 1){
			
			strcpy(srv_dir, "./server");
			make_path(&srv_dir, commands[1], NULL, 0);			
			strcpy(hello, srv_dir);
			printf("send the .Manifest of project i.e: %s\n", commands[1]);	
			
		}
		else if(check_dir(srv_dir) == 0){
			strcpy(hello, "sorry, there is no project by this name.");
			printf("Request Rejected\n");
		}
		sendto(sockfd, hello, strlen(hello), 0, 
          (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
    }
    
    else if(str_cmp(commands[0], "upgrade")){
		
		strcat(srv_dir, commands[1]);
		if(check_dir(srv_dir) == 1){
			strcpy(srv_dir, "./server");
			strcpy(hello, "OK");
			printf("send the 'OK' msg");	
		}
		else if(check_dir(srv_dir) == 0){
			strcpy(hello, "sorry, there is no project by this name.");
			printf("Request Rejected\n");
		}
		sendto(sockfd, hello, strlen(hello), 0, 
          (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
          
        n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr,&len); //receive message from server 
		buffer[n] = '\0';
		
		int index, lim = atoi(buffer);
		for(index = 0; index < lim; index++){
		
			n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr,&len); //receive message from server 
			buffer[n] = '\0';
			gettok(&commands, buffer);
			make_path(&srv_dir, commands[0], commands[1], 1);
		
			int fd = open(srv_dir, O_RDONLY);
			if(fd < 0){
				printf("Error!!! No file exists: %s\n", srv_dir);
				close(fd);
				exit(0);
			}
			read(fd, hello, sizeof(hello));
			sendto(sockfd, hello, strlen(hello), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
			
			close(fd);
			strcpy(srv_dir, "./server");
		}
    }
    
    else if(str_cmp(commands[0], "commit")){
		
		strcat(srv_dir, commands[1]);
		if(check_dir(srv_dir) == 1){
			
			strcpy(srv_dir, "./server");
			make_path(&srv_dir, commands[1], NULL, 0);			
			strcpy(hello, srv_dir);
			printf("send the .Manifest of project i.e: %s\n", commands[1]);	
			
		}
		else if(check_dir(srv_dir) == 0){
			strcpy(hello, "sorry, there is no project by this name.");
			printf("Request Rejected\n");
		}
		sendto(sockfd, hello, strlen(hello), 0, 
          (struct sockaddr*)&cliaddr, sizeof(cliaddr));
        
      
        n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr,&len); //receive message from server 
			buffer[n] = '\0';
		
		strcpy(srv_dir, "./server");	
		make_path(&srv_dir, commands[1], "/Commit.txt", 0);
		
		int fd = open(srv_dir, O_CREAT|O_RDWR, 0666);
		if(fd < 0){
			printf("Error!!! can not open: %s\n", srv_dir);
			close(fd);
			exit(0);
		}
		write(fd, buffer, strlen(buffer));
		close(fd);
		printf("Successfully received .Commit file from Client\n");
    }

     else if(str_cmp(commands[0], "push")){
		
		//Mutex_Lock
		if(pthread_mutex_init(&lock, NULL) != 0)
				printf("Error in Mutex Lock\n");	
		pthread_mutex_lock(&lock);
		
		strcpy(hello, "OK");
		sendto(sockfd, hello, strlen(hello), 0, 
          (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
		
		char *chk_out = malloc(4096*4096); 
		recvfrom(sockfd, chk_out, 4096*4096, 0, (struct sockaddr*)NULL, NULL);
		
		creat_proj(srv_dir, commands[1]);
		
		strcat(commands[1], "copy1");
		download(srv_dir, commands[1], chk_out);
		make_Manifest(srv_dir, commands [1], "/Manifest.txt");
		printf("Successfully created project: %s\n", commands[1]);
		free(chk_out);
		
		//mutex_unlock
		pthread_mutex_unlock(&lock);
    }


    else if(str_cmp(commands[0], "create")){
		
		strcat(srv_dir, commands[1]);
		if(check_dir(srv_dir) == 0){
			
			strcpy(srv_dir, "./server");
			creat_proj(srv_dir, commands[1]);
			printf("created a directory by name: %s\n", commands[1]);
			strcpy(hello, "created");
		}
		else if(check_dir(srv_dir) == 1){
			strcpy(hello, "sorry, there already exists a project by this name.");
			printf("Request Rejected\n");
		}
		else
			strcpy(hello, "Error in formant of argument");
		sendto(sockfd, hello, strlen(hello), 0, 
          (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
    }
    
    else if(str_cmp(commands[0], "destroy")){
		
		strcat(srv_dir, commands[1]);
		if(check_dir(srv_dir) == 1){
			
			if(pthread_mutex_init(&lock, NULL) != 0)
				printf("Error in Mutex Lock\n");
			
			pthread_mutex_lock(&lock);
			char rm[100];
			strcpy(rm, "rm -r ");
			strcat(rm, srv_dir);
			system(rm);
			pthread_mutex_unlock(&lock);
			
			printf("destoyed a project by name: %s\n", commands[1]);
			strcpy(hello, "destroyed");
		}
		else if(check_dir(srv_dir) == 0){
			strcpy(hello, "sorry, there is no project by this name.");
			printf("Request Rejected\n");
		}
		sendto(sockfd, hello, strlen(hello), 0, 
          (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
    }
    
    else if(str_cmp(commands[0], "currentversion")){
		
		strcat(srv_dir, commands[1]);
		if(check_dir(srv_dir) == 1){
			
			strcpy(srv_dir, "./server");
			make_path(&srv_dir, commands[1], "/Manifest.txt", 0);
			
			char *cur_ver = malloc(sizeof(char)*5000);
			get_curr_ver(srv_dir ,(char**)&cur_ver);
			
			sendto(sockfd, cur_ver, strlen(cur_ver), 0, 
          (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
			printf("send the current version of project i.e: %s\n", commands[1]);	
			
			free(cur_ver);
		}
		else if(check_dir(srv_dir) == 0){
			strcpy(hello, "sorry, there is no project by this name.");
			printf("Request Rejected\n");
			sendto(sockfd, hello, strlen(hello), 0, 
          (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
		}
    }
    
    else if(str_cmp(commands[0], "history")){
		
		strcat(srv_dir, commands[1]);
		if(check_dir(srv_dir) == 1){
			
			
			strcpy(hello, "destroyed");
		}
		else if(check_dir(srv_dir) == 0){
			strcpy(hello, "sorry, there is no project by this name.");
			printf("Request Rejected\n");
		}
		sendto(sockfd, hello, strlen(hello), 0, 
          (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
    }

    else if(str_cmp(commands[0], "rollback")){
		
		strcat(srv_dir, commands[1]);	
		if(check_dir(srv_dir) == 1){

			if(str_cmp(commands[2], "1")){
				strcpy(hello, "sorry, there is no project by this name.");
				printf("Already, Oldest Version\n");
			}
			strcat(srv_dir, "copy1");
			if(check_dir(srv_dir) == 1){
				
				if(pthread_mutex_init(&lock, NULL) != 0)
					printf("Error in Mutex Lock\n");
				
				pthread_mutex_lock(&lock);
				char rm[100];
				strcpy(rm, "rm -r ");
				strcat(rm, srv_dir);
				system(rm);
				pthread_mutex_unlock(&lock);				
				strcpy(hello, "destroyed");
			}
		}
		else if(check_dir(srv_dir) == 0){
			strcpy(hello, "sorry, there is no project by this name.");
			printf("Request Rejected\n");
		}
		sendto(sockfd, hello, strlen(hello), 0, 
          (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
    }
        
    free(srv_dir);
   
}   
   

int main(int argc, char* arg[]) { 
		
	if(argc < 2){
		printf("Error, PORT Number is missing !!!\n");
		return -1;
	}
	
    int sockfd, newsock;
	struct sockaddr_in address, newaddr;
	pthread_t tid;

	//Initialize the server
	initializeServer(&sockfd, address, atoi(arg[1]));
	printf("Server has been setup successfully with (port: %s).\n", arg[1]);
	
	while (1){

		socklen_t len = sizeof(struct sockaddr_in);
		newsock = accept(sockfd, (struct sockaddr*)&newaddr, &len);
		
		if(newsock < 0)
			printf("Error in accepting.\n");
		else
		printf("\nServer accepted connection from %s : %d\n", inet_ntoa(newaddr.sin_addr), ntohs(newaddr.sin_port));

		//create thread to deals with client request
		pthread_create(&tid, NULL,(void*) &handle_req, (void*) &newsock);
	}

	// ... CLEANUP CODE HERE ... //
	close(newsock);
	close(sockfd);


	// Wait for the threads to complete
	pthread_exit(NULL);
	printf("SERVER: Shutting down.\n");
	return 0;
} 
