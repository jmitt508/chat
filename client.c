//John Mitton & Blake Braun
//Client
//Usage: client <IP address> <port number>
//Some code from man 2 bind & man 7 unix

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define BUFSIZE 150

int done = 0;//supposed to quit thread, does not work

struct thread_args {
	int sfd;
};

void* thread_entry(void* args){//prints all incoming messages to console
	struct thread_args* arg_ptr = (struct thread_args*) args;
	int sfd = arg_ptr->sfd;
	char buffer[BUFSIZE];
	while(1){
		if(done==1) pthread_exit(0);
		int r =1;
		r = read(sfd, buffer, BUFSIZE);
		if(r==-1) perror("read");
		if(r==0) break;
		printf(buffer);
	}
}

int main(int argc, char* argv[]){
	if(argc<=2){
		printf("Usage: client <IP address> <port number>\n");
		return 0;
	}
	char* ip = strtok(argv[1], " \0\n");
	uint16_t port = strtol(strtok(argv[2], " \0\n"), NULL, 10);
	struct thread_args args[1];
	pthread_t ids[1];
	char name[BUFSIZE]; //for client name
	char num[2];
	char buffer[BUFSIZE];
	char joined[BUFSIZE];
		
	//create socket
	int sfd = socket(AF_INET, SOCK_STREAM || SOCK_NONBLOCK, 0);
	struct sockaddr_in my_addr;
	memset(&my_addr, 0, sizeof(struct sockaddr_in));
	my_addr.sin_family = AF_INET;
	inet_aton(ip, &my_addr.sin_addr);
	my_addr.sin_port = htons(port);
	args[0].sfd = sfd;
	
	//make the connection, start listening for messages
	if (connect(sfd, (struct sockaddr*) &my_addr, sizeof(struct sockaddr_in)) == -1) perror("connect");
	pthread_create(&ids[0], NULL, thread_entry, &args[0]);
	
	//read name, print joined message
	if(read(sfd, num, 2) == -1) perror("read");
	sprintf(name, "User %s", num);
	sprintf(joined, "%s has connected.\n", name);
	if(write(sfd, joined, BUFSIZE) == -1) perror("write");
	
	while(strncmp(buffer, "quit\n", 5) != 0){ //get input until quit
		fgets(buffer, BUFSIZE, stdin);
		char quitMsg[BUFSIZE];
		sprintf(quitMsg, "%s has quit.\n", name);
		if(strncmp(buffer, "name\n", 5) ==0){
			while(1){
				printf("Enter a new name: ");
				char nameBuf[BUFSIZE];
				fgets(nameBuf, BUFSIZE, stdin);
				if(strncmp(nameBuf, "\n", 1) != 0){
					char oldName[BUFSIZE];
					sprintf(oldName, "%s", name);
					sprintf(name, "%s", strtok(nameBuf, "\n"));
					char changeMsg[BUFSIZE];
					sprintf(changeMsg, "%s has changed their name to %s.\n", oldName, name);
					if(write(sfd, changeMsg, BUFSIZE) == -1) perror("write");
					break;
				}
			}
		}
		else if(strncmp(buffer, "quit\n", 5)==0){//quit message received
			done =1;
			int w = write(sfd, quitMsg, BUFSIZE);
			if(w==-1) perror("write");
			pthread_kill(ids[0], SIGQUIT);//not ideal, but best way I could figure
			pthread_join(ids[0], NULL);
		}
		else{//concatenate username with message and send
			char toSend[BUFSIZE];
			sprintf(toSend, "%s: %s", name, buffer);
			int w = write(sfd, toSend, BUFSIZE);
			if(w==-1) perror("write");
		}
	}

	return 0;
}
