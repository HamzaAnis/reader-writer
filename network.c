#include <assert.h> 
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "requests.h"
#include "network.h"
#include "server.h"

extern int errno;


void send_http_response(int fd, char *message) {
    char buffer[OUTPUT_BUFFER_SIZE];
    time_t timer;
    struct tm *tm_info;

    assert(message != NULL);
    assert(strlen(message) < OUTPUT_BUFFER_SIZE);

    write(fd, "HTTP/1.0 200 OK\n", 16);

    time(&timer);
    tm_info = localtime(&timer);
    strftime(buffer, 100, "%d %b %Y %X %Z", tm_info);
    write(fd, buffer, strlen(buffer));

    write(fd, "Content-Type: text/html\n", 24);

    sprintf(buffer, "Content-Length: %d\n", (int)strlen(message));
    write(fd, buffer, strlen(buffer));
    write(fd, "\n", 1);

    write(fd, message, strlen(message));
}


pc_op_t parse_request(char *request, char *value) {
    char *s, *t;
    pc_op_t result = UNKNOWN;

    assert(request != NULL);
    assert(value != NULL);

    if (strncmp(request, "GET ", 4) != 0) {
        return UNKNOWN;
    }

    s = strstr(request, "op=");
    if (s == NULL) {
        return UNKNOWN;
    }

    s += 3;

    if (strncmp(s, "read", 4) == 0) {
        result = READ;
        /* Still need to extract parameter below... */
    } else if (strncmp(s, "write", 5) == 0) {
        result = WRITE;
        /* Still need to extract parameter below... */
    } else if (strncmp(s, "meetup", 6) == 0) {
        result = MEETUP;
        /* Still need to extract parameter below... */
    } else {
        /* No meaningful op specific, so just return. */
        return UNKNOWN;
    }

    /* If code reaches this point, it must be because we need to
     * extract the parameter for the op. However, if it is the
     * READ command, there is no parameter to be received.
     */
    if (result == READ) {
        value[0] = '\0';
        return READ;
    }

    s = strstr(request, "val=");
    if (s == NULL) {
        return UNKNOWN;
    } 

    s += 4;
    for (t = s; *t != ' ' && *t; t++) { }
    strncpy(value, s, (t - s));
    value[t - s + 1] = '\0';

    return result;
}


int setup_listener() {
    int listen_fd;
    int bind_result;

    struct sockaddr_in servaddr;
    int option = 1;

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    bzero( &servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(COMM_PORT);

    if (bind_result = bind(listen_fd,
        (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
    {
        fprintf(stderr, "Problems binding to port %d\n", COMM_PORT);
        fprintf(stderr, "System error is %s\n", strerror(errno));
        exit(1);
    }

    listen(listen_fd, 10*MAX_THREADS);

    return listen_fd;
}

