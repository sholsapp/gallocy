#include <iostream>

#include "./config.h"
#include "./constants.h"
#include "./httpd.h"
#include "./libgallocy.h"


int main(void) {
  init();

  uint16_t port = 8080;
  uint64_t server_sock = -1;
  uint16_t _port = reinterpret_cast<uint16_t>(port);
  uint64_t client_sock = -1;
  struct sockaddr_in client_name;
  uint64_t client_name_len = sizeof(client_name);
  pthread_t newthread;

  server_sock = startup(&_port);

  printf("main: %p\n", global_main());
  printf("end: %p\n", global_end());
  printf("base: %p\n", global_base());
  printf("httpd running on port %d\n", _port);

  while (1) {
    client_sock = accept(server_sock,
        reinterpret_cast<struct sockaddr *>(&client_name),
        reinterpret_cast<socklen_t*>(&client_name_len));
    if (client_sock == -1)
      error_die("accept");
    if (pthread_create(&newthread , NULL, accept_request, reinterpret_cast<void *>(client_sock)) != 0)
      perror("pthread_create");
  }

  close(server_sock);
  return(0);
}
