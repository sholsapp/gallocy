#include "httpd.h"


int main(void) {

  init();

  int server_sock = -1;
  u_short port = 8080;
  long client_sock = -1;
  struct sockaddr_in client_name;
  unsigned int client_name_len = sizeof(client_name);
  pthread_t newthread;

  server_sock = startup(&port);
  printf("httpd running on port %d\n", port);

  while (1) {
    client_sock = accept(server_sock,
        (struct sockaddr *)&client_name,
        &client_name_len);
    if (client_sock == -1)
      error_die("accept");
    /* accept_request(client_sock); */
    if (pthread_create(&newthread , NULL, accept_request, (void *) client_sock) != 0)
      perror("pthread_create");
  }

  close(server_sock);

  return(0);
}
