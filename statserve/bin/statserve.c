#include "network.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    int socket = create_socket(7789);
    int connfd = 0;

    char *buf = calloc(1, 1025);

    listen(socket, 10);

    while (1) {
        connfd = accept(socket, (struct sockaddr_in *)NULL, NULL);

	int rc = recv(connfd, buf, 1024, 0);
	printf("Received: %d %s\n", strlen(buf), buf);
	int rc2 = write(connfd, buf, strlen(buf));
	
	close(connfd);
    }

    close(socket);
    free(buf);

    return 0;
}
