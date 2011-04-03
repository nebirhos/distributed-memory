#include "server.h"
#include "message.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>      // getaddrinfo
#include <netinet/in.h>
#include <arpa/inet.h>  // inet_ntop
#include <iostream>
#include <pthread.h>
#include <sstream>
#include <cstring>
using namespace std;


namespace DM {

Server::Server(string config_path, string id)
  : config(config_path),
    id(id) {
  const vector<int>& blocks_id = config.find(id).blocks_id;
  for (unsigned int i = 0; i < blocks_id.size(); ++i) {
    BlockServer b( blocks_id[i] );
    blocks[ blocks_id[i] ] = b;
    cout << "b.id(): " << b.id() << endl; // FIXME
    cout << "b.revision(): " << b.revision() << endl; // FIXME
    cout << "b.data(): " << b.data() << endl; // FIXME
    cout << blocks[blocks_id[i]].dump_hex(); // FIXME
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
    HandlerWrapperArgs* args = new HandlerWrapperArgs;
    args->obj = this;
    args->socket_d = socket_d;
    pthread_create( &tid, 0, &Server::client_handler_wrapper, (void*) args );
    cout << "New thread tid: " << tid << endl;
  }
}

void Server::stop() {
  close(listen_socket);
}

int Server::open_socket() {
  addrinfo hints = {0};
  addrinfo *server_addrinfo, *p;
  int sockfd;
  hints.ai_family = AF_UNSPEC;  // both IPv4 and IPv6
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  getaddrinfo(NULL, config.find(id).port.c_str(), &hints, &server_addrinfo);
  for ( p = server_addrinfo; p != NULL; p = p->ai_next ) {
    if ((sockfd = socket( p->ai_family, p->ai_socktype, p->ai_protocol )) < 0) {
      continue;
    }
    if (bind( sockfd, p->ai_addr, p->ai_addrlen ) < 0) {
      close(sockfd);
      continue;
    }
    break;
  }
  if (p == NULL) {
    throw "open_socket failed FIXME";
  }
  freeaddrinfo(server_addrinfo);

  if (listen( sockfd, TCP_MAX_CONNECTIONS ) < 0) {
    throw "Listen failed FIXME";
  }
  return sockfd;
}

/** We don't want static here! */
void* Server::client_handler_wrapper(void* args) {
  HandlerWrapperArgs* wargs = (HandlerWrapperArgs*) args;
  wargs->obj->client_handler(wargs->socket_d);
  delete wargs;
  return 0;
}

void Server::client_handler(int socket_d) {
  pthread_detach( pthread_self() );
  // cout << "get_client_id(socket_d): " << get_client_id(socket_d) << endl;
  string client_id = get_client_id(socket_d);
  char buffer[TCP_BUFFER_SIZE];
  string message;
  int size = 0;
  do {
    size = recv( socket_d, (void*) buffer, TCP_BUFFER_SIZE-1, 0 );
    buffer[size] = 0;
    message += buffer;

    size_t token_stop = message.find(STOP_REQ);
    if (token_stop != string::npos) {
      Message msg(message);
      int block_id;
      string ack;
      map<int,BlockServer>::iterator it;

      switch ( msg.type() ) {
      case MAP:
        block_id = msg.block()->id();
        cout << "block_id: " << block_id << endl;
        it = blocks.find(block_id);
        if (it != blocks.end()) {
          it->second.map(client_id);
          ack = Message::emit(ACK, it->second);
          cout << "ack.size(): " << ack.size() << endl;
          send(socket_d, (void*) ack.c_str(), ack.size(), 0);
        }
        break;
      case UNMAP:
        block_id = msg.block()->id();
        cout << "block_id: " << block_id << endl;
        it = blocks.find(block_id);
        if (it != blocks.end()) {
          it->second.unmap(client_id);
          ack = Message::emit(ACK);
          send(socket_d, (void*) ack.c_str(), ack.size(), 0);
        }
        break;
      case UPDATE:
      //   Block& b = blocks.find(msg.block());
      //   if (msg.block().rev() < b.rev()) {
      //     string ack = Message::emit(ACK, b);
      //   }
      //   else {
      //     string ack = Message::emit(ACK);
      //   }
      //   send(ack);
        break;
      }
      message.clear();
    }
  } while ( size != 0 );
  close(socket_d);
  cout << "Here is the message: ---" << message << "---" << endl; // FIXME
}

string Server::get_client_id(int socket_d) {
  sockaddr_in client;
  socklen_t addrlen = sizeof(sockaddr_in);
  getpeername(socket_d, (sockaddr*) &client, &addrlen);
  char client_ip[16];
  inet_ntop( AF_INET, &client.sin_addr.s_addr, client_ip, INET_ADDRSTRLEN );
  ostringstream client_id;
  client_id << client_ip << ":" << client.sin_port;
  return client_id.str();
}

}
