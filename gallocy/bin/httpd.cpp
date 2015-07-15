#include <iostream>

#include "config.h"
#include "constants.h"
#include "httpd.h"
#include "libgallocy.h"


int main(void) {

  init();

  gallocy::string host;
  int port;
  gallocy::string me;
  peer_list_t peers;

  read_config(host, port, me, peers);

  int server_sock = -1;

  u_short _port = (unsigned short) port;
  long client_sock = -1;
  struct sockaddr_in client_name;
  unsigned int client_name_len = sizeof(client_name);
  pthread_t newthread;

  server_sock = startup(&_port);

  printf("main: %p\n", global_main());
  printf("end: %p\n", global_end());
  printf("base: %p\n", global_base());
  printf("httpd running on port %d\n", _port);

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
