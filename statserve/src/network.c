#include <network.h>

int create_socket(int port)
{
    int listenfd = 0;
    struct sockaddr_in serv_addr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    check(listenfd != -1, "Failed to create socket.");
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    int rc = bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    check (rc == 0, "Failed to bind socket.");

    return listenfd;

error:
    return -1;
}
