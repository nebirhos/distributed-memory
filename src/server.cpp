#include "server.h"
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <pthread.h>
using namespace std;


namespace DM {

Server::Server(string config_path, string server_id)
  : config(config_path),
    id(server_id) {
  const vector<int>& blocks_id = config.find(id).blocks_id;
  for (unsigned int i = 0; i < blocks_id.size(); ++i) {
    Block b = Block(blocks_id[i]);
    blocks.push_back(b);
  }
}

void Server::start() {
  listen_socket = open_socket();

  while (1) {
    sockaddr_in client;
    socklen_t addrlen = sizeof(sockaddr_in);
    int socket_d = accept( listen_socket, (sockaddr*) &client, &addrlen );
    if (socket_d < 0) {
      throw "Accept failed FIXME";
    }
    pthread_t tid;
    WrapperArgs* args = new WrapperArgs;
    args->obj = this;
    args->socket_d = socket_d;
    pthread_create( &tid, 0, &Server::thread_wrapper, (void*) args );
  }
}

void Server::stop() {
  close(listen_socket);
}

/** We don't want static here! */
void* Server::thread_wrapper(void* args) {
  WrapperArgs* wargs = (WrapperArgs*) args;
  wargs->obj->accept_request(wargs->socket_d);
  delete wargs;
  return 0;
}

void Server::accept_request(int socket_d) {
  pthread_detach( pthread_self() );
  char buffer[TCP_BUFFER_SIZE];
  string message;
  int size = 0;
  do {
    size = recv( socket_d, (void*) buffer, TCP_BUFFER_SIZE-1, 0 );
    buffer[size] = 0;
    message += buffer;
    send(socket_d, (void*) "Thanks!\n", 9, 0); // FIXME
  } while ( size != 0 );
  close(socket_d);
  cout << "Here is the message: ---" << message << "---" << endl; // FIXME
}

int Server::open_socket() {
  sockaddr_in socket_conf = {0};
  socket_conf.sin_family = AF_INET;
  socket_conf.sin_port = htons( config.find(id).port );
  socket_conf.sin_addr.s_addr = INADDR_ANY;
  int socket_d = socket( AF_INET, SOCK_STREAM, 0 );
  if (socket_d < 0) {
    throw "Socket failed FIXME";
  }
  if (bind( socket_d, (sockaddr*) &socket_conf, sizeof(sockaddr_in) ) < 0) {
    throw "Bind failed FIXME";
  }
  if (listen( socket_d, TCP_MAX_CONNECTIONS) < 0) {
    throw "Listen failed FIXME";
  }
  return socket_d;
}

}
