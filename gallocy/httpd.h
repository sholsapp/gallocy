#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>

#define ISspace(x) isspace((int)(x))

#define SERVER_STRING "Server: gallocy-httpd/0.1.0\r\n"

int get_line(int, char *, int);
int startup(u_short *);
void *accept_request(void *);
void add_header(int, const char *, const char *);
void cat(int, FILE *);
void error_die(const char *);
void headers(int, const char *);
void serve_file(int, const char *);

void init(void);
void admin(void*);
