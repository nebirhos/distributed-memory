#include <dm/client.h>
#include <dm/type.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
using namespace std;


namespace DM {

Client::Client() : config("") {}

void Client::dm_init(char* config_file) {
  config = Config(config_file);

  ServerMap servers = config.find_all();
  ServerMap::iterator it;
  for (it = servers.begin(); it != servers.end(); ++it) {
    server_sockets[it->first] = open_socket(it->second.ip, it->second.port);
  }

  write( server_sockets["127.0.0.1:4567"], "FIXME", 5);
}

int Client::open_socket(string ip, string port) {
  cout << "Connetto a " << ip << ":" << port << endl;

  addrinfo hints = {0};
  addrinfo *server_addrinfo, *p;
  int sockfd;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  getaddrinfo( ip.c_str(), port.c_str(), &hints, &server_addrinfo );
  for (p = server_addrinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket( p->ai_family, p->ai_socktype, p->ai_protocol )) < 0) {
      cout << "errore socket" << endl;
      continue;
    }
    if (connect( sockfd, p->ai_addr, p->ai_addrlen ) < 0) {
      close(sockfd);
      cout << "errore connect" << endl;
      continue;
    }
    break;
  }
  if (p == NULL) {
    perror("Errore: ");         // FIXME
  }
  freeaddrinfo(server_addrinfo);
  return sockfd;
}

} // namespace DM
