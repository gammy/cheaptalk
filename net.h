#ifndef NET_H
#define NET_H

#include "main.h"

#define DEFAULT_PORT	"13512"

extern int net_sock_client; // net.c

#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

void net_finish(void);
void *net_get_addr(struct sockaddr *sa);
unsigned int net_listen(char *port);
unsigned int net_connect(char *host, char *port);
unsigned int net_begin(unsigned int mode, char *host, char *port);
unsigned int net_send(chtype *c);
unsigned int net_recv(chtype *c);
#endif
