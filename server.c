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
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdbool.h>

#define LISTEN_BACKLOG 50
#define BUFSIZE 150

typedef struct users *user;
struct users{
	int id;
	int num;
	user next;
	user prev;
};

int main(int argc, char* argv[]){
	char* ip = strtok(argv[1], " \0\n");
	uint16_t port = strtol(strtok(argv[2], " \0\n"), NULL, 10);
	int sfd, cfd;
	char name[2] = "01";//remove later
	fd_set readSet, writeSet, exceptSet;
        struct timeval t;
//        FD_SET(cfd, &readSet);
        FD_ZERO(&writeSet);
        FD_ZERO(&exceptSet);
        t.tv_sec = 1000;

	
	struct sockaddr_in my_addr, peer_addr;
	socklen_t peer_addr_size;

	sfd = socket(AF_INET, SOCK_STREAM || SOCK_NONBLOCK, 0);
	if(sfd == -1) perror("socket");

	memset(&my_addr, 0, sizeof(struct sockaddr_in));
	my_addr.sin_family = AF_INET;
	inet_aton(ip, &my_addr.sin_addr);
	my_addr.sin_port = htons(port);
	
	if(bind(sfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in)) == -1) perror("bind");

	if(listen(sfd, LISTEN_BACKLOG) == -1) perror("listen");
	
	peer_addr_size = sizeof(struct sockaddr_in);
	char buffer[BUFSIZE];
	int numUsers = 0;
	printf("%s","here");
	struct users start;
	start.id = 0;
	start.next = NULL;
	while(1){
		if(strncmp(buffer,"squit", 5) == 0) break;
		cfd = accept4(sfd, (struct sockaddr*) &peer_addr, &peer_addr_size, SOCK_NONBLOCK);
		if(cfd == -1){
                        perror("accept");
                        break;
                }
		bool newUser = true;
		user nextUser;
		*nextUser = start;
		while(1){
			if(nextUser->id == cfd){
				newUser = false;
				break;
			}else if(nextUser->next != NULL){
				nextUser = nextUser->next;
			}else{
				break;
			}
		}
		if(newUser){
			user newU;
			newU = nextUser;
			newU->id = cfd;
			newU->next = NULL;
			newU->prev = nextUser;
			nextUser->next = newU;
			numUsers++;
			newU->num = numUsers;
		}		
		FD_SET(cfd, &readSet);
		if(write(cfd, name, 2) == -1) perror("write");
		int r=1;
		while(1){
			select(cfd+1, &readSet, &writeSet, &exceptSet, &t);  
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
