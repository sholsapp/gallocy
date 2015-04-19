#include <utility>

#include "httpd.h"
#include "libgallocy.h"


typedef
  std::map<std::string, void (*)(void*),
  std::less<std::string>,
  STLAllocator<std::pair<std::string, void (*)(void*)>, SingletonHeapType> >
    routing_table_t;


routing_table_t routing_table;


void init() {
  routing_table["/admin"] = &admin;
  fprintf(stderr, "/admin is at %p\n", &admin);
}


void admin(void* arg) {
  fprintf(stderr, "ADMIN\n");
  return;
}

/**
 * Process a request.
 *
 * :param arg: The socket connected to the client.
 */
void *accept_request(void *arg) {
  long client = (long) arg;
  char buf[1024];
  int numchars;
  char method[255];
  char url[255];
  char path[512];
  size_t i, j;
  char *query_string = NULL;

  numchars = get_line(client, buf, sizeof(buf));
  i = 0; j = 0;
  while (!ISspace(buf[j]) && (i < sizeof(method) - 1)) {
    method[i] = buf[j];
    i++; j++;
  }
  method[i] = '\0';

  if (strcasecmp(method, "GET") && strcasecmp(method, "POST")) {
    return NULL;
  }

  i = 0;
  while (ISspace(buf[j]) && (j < sizeof(buf)))
    j++;
  while (!ISspace(buf[j]) && (i < sizeof(url) - 1) && (j < sizeof(buf))) {
    url[i] = buf[j];
    i++; j++;
  }
  url[i] = '\0';

  if (strcasecmp(method, "GET") == 0) {
    query_string = url;
    while ((*query_string != '?') && (*query_string != '\0'))
      query_string++;
    if (*query_string == '?') {
      *query_string = '\0';
      query_string++;
    }
  }

  fprintf(stderr, "====================================\n");
  fprintf(stderr, "> method = %s\n", method);
  fprintf(stderr, "> url    = %s\n", url);
  fprintf(stderr, "> path   = %s\n", path);
  fprintf(stderr, "> query  = %s\n", query_string);
  fprintf(stderr, "====================================\n");

  fprintf(stderr, "Map size = %lu\n", routing_table.size());

  //for (auto const &it : routing_table) {
    //fprintf(stderr, "Key = %s, Size = %d\n", it.first, strlen(it.first));
    //fprintf(stderr, "Compared to /admin: %d\n", strcmp(it.first, "/admin"));
  //  fprintf(stderr, "WTF %p\n", it.second);
    //it.second(NULL);
  //}

  void (*func)(void*) = routing_table[std::string(url)];

  if (func)
    func(NULL);


  //void (*func)(void*) = routing_table[std::string(url).c_str()];
  //fprintf(stderr, "What is length of %d\n", strlen(url));
  //fprintf(stderr, "What is %p\n", func);
  //func(NULL);
  //
  while ((numchars = get_line(client, buf, sizeof(buf))) > 2) {
    fprintf(stderr, "%s", buf);
  }


  headers(client, NULL);

  char json_buffer[512];
  snprintf(json_buffer, 512,
    "{\"foo\": \"bar\"}");
  send(client, json_buffer, strlen(json_buffer), 0);

  shutdown(client, SHUT_RDWR);
  close(client);
  return NULL;
}


/**
 * Cat the contents of a file to a socket.
 *
 * :param client: The client socket.
 * :param resource: The file to write to the socket.
 */
void cat(int client, FILE *resource) {
  char buf[1024];
  fgets(buf, sizeof(buf), resource);
  while (!feof(resource)) {
    send(client, buf, strlen(buf), 0);
    fgets(buf, sizeof(buf), resource);
  }
}


/**
 * Die.
 *
 * :param sc: A custom error to pass alont to ``perror``.
 */
void error_die(const char *sc) {
  perror(sc);
  exit(1);
}


/**
 * Get a line from a socket.
 *
 * Lines end in a newline, a carriage return, or a CRLF combination. This
 * function terminates the string read with a null character. If no newline
 * indicator is found before the end of the buffer, the string is terminated
 * with a null. If any of the above three line terminators is read, the last
 * character of the string will be a linefeed and the string will be terminated
 * with a null character.
 *
 * :param sock: The socket descriptor.
 * :param buf: The buffer to save data in.
 * :param size: The size of the buffer.
 * :returns: The number of bytes stored (excluding null)..
 */
int get_line(int sock, char *buf, int size) {
  int i = 0;
  char c = '\0';
  int n;
  while ((i < size - 1) && (c != '\n')) {
    n = recv(sock, &c, 1, 0);
    /* DEBUG printf("%02X\n", c); */
    if (n > 0) {
      if (c == '\r') {
        n = recv(sock, &c, 1, MSG_PEEK);
        /* DEBUG printf("%02X\n", c); */
        if ((n > 0) && (c == '\n'))
          recv(sock, &c, 1, 0);
        else
          c = '\n';
      }
      buf[i] = c;
      i++;
    }
    else
      c = '\n';
  }
  buf[i] = '\0';
  return(i);
}


/**
 * Writes HTTP headers to socket.
 *
 * :param client: The client socket descriptor.
 * :param filename: The file being send, or NULL.
 */
void headers(int client, const char *filename) {
  char buf[1024];
  (void)filename;
  strcpy(buf, "HTTP/1.0 200 OK\r\n");
  send(client, buf, strlen(buf), 0);
  strcpy(buf, SERVER_STRING);
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "Content-Type: application/json\r\n");
  send(client, buf, strlen(buf), 0);
  strcpy(buf, "\r\n");
  send(client, buf, strlen(buf), 0);
}

void add_header(int client, const char *header, const char *value) {
  char buf[1024];
  snprintf(buf, 1024, "%s: %s\r\n", header, value);
  send(client, buf, strlen(buf), 0);
}


/**
 * Write a file to a socket.
 *
 * :param client: The client socket descriptor.
 * :param filename: The file to write to the socket.
 */
void serve_file(int client, const char *filename) {
  FILE *resource = NULL;
  int numchars = 1;
  char buf[1024];

  buf[0] = 'A'; buf[1] = '\0';
  while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
    numchars = get_line(client, buf, sizeof(buf));

  resource = fopen(filename, "r");
  if (resource == NULL)
    return;
  else {
    headers(client, filename);
    cat(client, resource);
  }
  fclose(resource);
}


/**
 * Start the server.
 *
 * If the port is 0, then dynamically allocate a port and modify the original
 * port variable to reflect the actual port.
 *
 * :param port: The port to start the server on.
 * :returns: The socket descriptor.
 */
int startup(u_short *port) {
  int httpd = 0;
  struct sockaddr_in name;

  httpd = socket(PF_INET, SOCK_STREAM, 0);
  if (httpd == -1)
    error_die("socket");

  int optval = 1;

#ifdef __APPLE__
  setsockopt(httpd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
#else
  setsockopt(httpd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
#endif

  memset(&name, 0, sizeof(name));
  name.sin_family = AF_INET;
  name.sin_port = htons(*port);
  name.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(httpd, (struct sockaddr *) &name, sizeof(name)) < 0)
    error_die("bind");
  if (*port == 0)  /* if dynamically allocating a port */ {
    unsigned int namelen = sizeof(name);
    if (getsockname(httpd, (struct sockaddr *) &name, &namelen) == -1)
      error_die("getsockname");
    *port = ntohs(name.sin_port);
  }
  if (listen(httpd, 5) < 0)
    error_die("listen");
  return(httpd);
}
