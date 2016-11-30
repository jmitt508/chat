//John Mitton & Blake Braun
//Server
//Usage: server <IP address> <port number>
//Some code from man 2 bind & man 7 unix

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define LISTEN_BACKLOG 50
#define BUFSIZE 150

int main(int argc, char* argv[]){
	char* ip = strtok(argv[1], " \0\n");
	char* port = strtok(argv[2], " \0\n");
	int sfd, cfd;
	
	struct sockaddr_un my_addr, peer_addr;
	socklen_t peer_addr_size;

	sfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sfd == -1) perror("socket");

	memset(&my_addr, 0, sizeof(struct sockaddr_un));
	my_addr.sun_family = AF_UNIX;
	strncpy(my_addr.sun_path, ip, sizeof(my_addr.sun_path) - 1);
	
	if(bind(sfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_un)) == -1) perror("bind");

	if(listen(sfd, LISTEN_BACKLOG) == -1) perror("listen");
	
	peer_addr_size = sizeof(struct sockaddr_un);
	char buffer[BUFSIZE];
	while(1){
		if(strncmp(buffer,"squit", 5) == 0) break;
		cfd = accept(sfd, (struct sockaddr*) &peer_addr, &peer_addr_size);
		if(cfd == -1){
			perror("accept");
			break;
		}

		int r=1;
		while(1){ 
			r = read(cfd, buffer, BUFSIZE);
			if(strncmp(buffer,"squit",5)==0) break;
			if(r == 0) break;
			if(r == -1) {
				perror("read");
				break;
			}
			printf(buffer);
		}
	}

	unlink(ip);
	return 0;
}
