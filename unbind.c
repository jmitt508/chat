#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MY_SOCK_PATH "127.0.0.1"

int main(){
	unlink(MY_SOCK_PATH);
	return 0;
}
