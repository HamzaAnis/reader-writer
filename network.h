#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "requests.h"

#define COMM_PORT  18300

pc_op_t parse_request(char *request, char *value);
void    send_http_response(int fd, char *message);
int     setup_listener();

#endif
