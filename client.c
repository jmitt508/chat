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

#define BUFSIZE 150

int main(int argc, char* argv[]){
	char* ip = strtok(argv[1], " \0\n");
	uint16_t port = strtol(strtok(argv[2], " \0\n"), NULL, 10);
	int sfd = socket(AF_INET, SOCK_STREAM || SOCK_NONBLOCK, 0);
	struct sockaddr_in my_addr;
	memset(&my_addr, 0, sizeof(struct sockaddr_in));
	my_addr.sin_family = AF_INET;
	inet_aton(ip, &my_addr.sin_addr);
	my_addr.sin_port = htons(port);

	if (connect(sfd, (struct sockaddr*) &my_addr, sizeof(struct sockaddr_in)) == -1) perror("connect");
	char name[7];
	char num[2];
	if(read(sfd, num, 2) == -1) perror("read");
	sprintf(name, "User %s", num);
	//basic one-way communication
	char joined[BUFSIZE];
	sprintf(joined, "%s has connected.\n", name);
	if(write(sfd, joined, BUFSIZE) == -1) perror("write");
	char buffer[BUFSIZE];
	while((strncmp(buffer, "quit\n", 5) != 0) && (strncmp(buffer, "squit", 5) !=0)){
		fgets(buffer, BUFSIZE, stdin);
		char quitMsg[BUFSIZE];
		sprintf(quitMsg, "%s has quit.\n", name);
		if(strncmp(buffer, "quit\n", 5)==0){
			int w = write(sfd, quitMsg, BUFSIZE);
			if(w==-1) perror("write");
		}
		else{
			char toSend[BUFSIZE + 7];
			sprintf(toSend, "%s: %s", name, buffer);
			int w = write(sfd, toSend, BUFSIZE+7);
			if(w==-1) perror("write");
		}
	}
	return 0;
}
