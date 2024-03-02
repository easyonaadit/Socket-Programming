#ifndef __SOCKETUTIL_H__
#define __SOCKETUTIL_H__

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

int createTCPIpv4Socket();
struct sockaddr_in* createIPv4Address(char *ip, int port);

#endif
