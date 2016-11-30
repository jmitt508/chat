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
	char* port = strtok(argv[2], " \0\n");
	
	int sfd = socket(AF_UNIX, SOCK_STREAM, 0);

	struct sockaddr_un my_addr;
	memset(&my_addr, 0, sizeof(struct sockaddr_un));
	my_addr.sun_family = AF_UNIX;
//	in.sin_port = htons(&port);
//	inet_aton(ip, &in.sin_addr);
	strncpy(my_addr.sun_path, ip, sizeof(my_addr.sun_path) - 1);

	if (connect(sfd, (struct sockaddr*) &my_addr, sizeof(struct sockaddr_un)) == -1) perror("connect");

	//basic one-way communication
	char buffer[BUFSIZE];
	while((strncmp(buffer, "quit", 4) != 0) && (strncmp(buffer, "squit", 5) !=0)){
		fgets(buffer, BUFSIZE, stdin);
		char quitMsg[15] = "User has quit.\n";
		if(strncmp(buffer, "quit", 4)==0)
			int w = write(sfd, quitMsg
		int w = write(sfd, buffer, BUFSIZE);
		if(w==-1) perror("write");
	}
	return 0;
}
