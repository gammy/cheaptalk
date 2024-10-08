#include "net.h"

int net_sock_client;

void net_finish(void) {
	chtype tmp = UI_KEY_EOT;
	net_send(&tmp);
	fsync(net_sock_client);
	close(net_sock_client);
}

void *net_get_addr(struct sockaddr *sa) {
	if(sa->sa_family == AF_INET)
		return &(((struct sockaddr_in*)sa)->sin_addr);

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

unsigned int net_listen(char *port) {
	// Based on http://beej.us/guide/bgnet/output/html/multipage/clientserver.html#simpleserver
	// (Note subtle differences since we're not a daemon and need not fork/reap)

	int sock_listen;
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family   = AF_UNSPEC;   // IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP
/*
        If  the  AI_PASSIVE  flag  is  specified in hints.ai_flags, and node is
        NULL,  then  the  returned  socket  addresses  will  be  suitable   for
        bind(2)ing  a  socket  that  will  accept(2) connections.  The returned
        socket address will contain the "wildcard address" (INADDR_ANY for IPv4
        addresses, IN6ADDR_ANY_INIT for IPv6 address)
*/
        hints.ai_flags    = AI_PASSIVE;

        // Get addresses which we can bind to
	if((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "Error: getaddrinfo: %s\n", gai_strerror(rv));
		return(0);
	}

	// Find and bind to the first possible local address
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if((sock_listen = socket(p->ai_family, 
					 p->ai_socktype,
					 p->ai_protocol)) == -1) {
			fprintf(stderr, "Error: socket: %s\n", strerror(errno));
			continue;
		}

		if(setsockopt(sock_listen, 
			       SOL_SOCKET, 
			       SO_REUSEADDR, 
			       &yes, 
			       sizeof(int)) == -1) {
			fprintf(stderr, "Error: setsockopt: %s\n", strerror(errno));
			return(0);
		}

		if(bind(sock_listen, 
			p->ai_addr, 
			p->ai_addrlen) == -1) {

			close(sock_listen);
			fprintf(stderr, "Error: bind: %s\n", strerror(errno));
			continue;
		}

		break;
	}

	if(p == NULL)  {
		fprintf(stderr, "Error: Couldn't bind listener: %s\n", strerror(errno));
		return(0);
	}

	freeaddrinfo(servinfo); // all done with this structure

	if(listen(sock_listen, 1) == -1) {
		fprintf(stderr, "Error: listen: %s\n", strerror(errno));
		return(0);
	}

	printf("Waiting for a connection on port %s\n", port);

	while(1) { // Loop until we successfully accept a connection
	
		sin_size = sizeof(their_addr);

		net_sock_client = accept(sock_listen, (struct sockaddr *)&their_addr, &sin_size);

		if(net_sock_client == -1) {
			fprintf(stderr, "Error: failed to accept connection: %s\n", strerror(errno));
			continue;
		}

		inet_ntop(their_addr.ss_family,
			  net_get_addr((struct sockaddr *)&their_addr),
			  s, 
			  sizeof(s));

		printf("Accepted connection from %s\n", s);

		close(sock_listen);

		break;
	}

	return(1);
}

unsigned int net_connect(char *host, char *port) {
	// Based on http://beej.us/guide/bgnet/output/html/multipage/clientserver.html#simpleclient

	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family   = AF_UNSPEC;   // IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP

        // Get host address
	if((rv = getaddrinfo(host, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return(0);
	}


	while(1) {

		net_sock_client = -1;

		// loop through all the results and connect to the first we can
		for(p = servinfo; p != NULL; p = p->ai_next) {

			if((net_sock_client = socket(p->ai_family, 
						     p->ai_socktype,
						     p->ai_protocol)) == -1) {
				fprintf(stderr, "Error: socket: %s\n", strerror(errno));
				sleep(2);
				continue;
			}

			printf("Connecting to %s on port %s: ", host, port);
			fflush(stdout);

			if(connect(net_sock_client, 
				   p->ai_addr, 
				   p->ai_addrlen) == -1) {

				close(net_sock_client);
				fprintf(stderr, "%s\n", strerror(errno));
				sleep(2);
				continue;
			}

			break;
		}

		if(p)
			break;

	}

	inet_ntop(p->ai_family, 
		  net_get_addr((struct sockaddr *)p->ai_addr),
		  s, 
		  sizeof(s));

	printf("Connected\n");

	freeaddrinfo(servinfo); // all done with this structure

	return(1);
}

unsigned int net_begin(unsigned int mode, char *host, char *port) {

	if(mode != MODE_SERVER && mode != MODE_CLIENT) {
		fprintf(stderr, "Error: net_begin called with invalid mode\n");
		return(0);
	}
	
	if(mode == MODE_SERVER)
		return(net_listen(port));
	
	return(net_connect(host, port));
}

unsigned int net_send(chtype *c) {
	
	if(send(net_sock_client, c, sizeof(char) * sizeof(chtype), MSG_DONTWAIT) == -1) {
		endwin();
		fprintf(stderr, "Error: send: %s\n", strerror(errno));
		abort();
	}

	return(1);
}

unsigned int net_recv(chtype *c) {

	int rx = 0;

	if((rx = recv(net_sock_client, c, sizeof(char) * sizeof(chtype) , MSG_DONTWAIT)) == -1) {

		if(errno == EAGAIN || errno == EWOULDBLOCK)
			return(0);
		
		endwin();
		fprintf(stderr, "Error: recv: %s\n", strerror(errno));
		abort();
	}

	return(1);
}
