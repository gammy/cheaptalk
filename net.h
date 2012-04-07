#ifndef NET_H
#define NET_H

#include "main.h"

#define DEFAULT_PORT	"13512"

int net_sock_client;

#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

void net_deinit(void);
unsigned int net_init(void);
void *net_get_addr(struct sockaddr *sa);
unsigned int net_listen(char *port);
unsigned int net_connect(char *host, char *port);
unsigned int net_begin(unsigned int mode, char *host, char *port);
unsigned int net_send(unsigned int mode, chtype *c);
unsigned int net_recv(unsigned int mode, chtype *c);
#endif
