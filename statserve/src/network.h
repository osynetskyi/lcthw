#ifndef _statserve_h
#define _statserve_h

#include "dbg.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>

void error(const char *msg);

int create_socket(int port);

#endif
