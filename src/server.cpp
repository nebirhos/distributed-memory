#include "server.h"
#include "message.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>      // getaddrinfo
#include <netinet/in.h>
#include <arpa/inet.h>  // inet_ntop
#include <iostream>
#include <sstream>
#include <cstring>
#include "logger.h"
#include <stdexcept>
using namespace std;


namespace DM {

Server::Server(string config_path, string id)
  : config(config_path),
    id(id) {
  Logger::info() << "Distributed Memory server" << endl;
  if ( !config.is_valid() ) {
    Logger::error() << "configuration file '" << config_path << "' not valid" << endl;
    throw runtime_error("");
  }
  if ( &config.find(id) == NULL ) {
    Logger::error() << "server '" << id << "' not found in configuration" << endl;
    throw runtime_error("");
  }
  const vector<int>& blocks_id = config.find(id).blocks_id;
  for (unsigned int i = 0; i < blocks_id.size(); ++i) {
    BlockServer b( blocks_id[i] );
    blocks.insert( pair<int,BlockServer>(blocks_id[i], b) );
    pthread_cond_init( &blocks_wait[blocks_id[i]], 0 );
    Logger::debug() << "create block #" << b.id() << endl;
  }
  pthread_mutex_init(&mutex, 0);
}

void Server::start() {
  listen_socket = open_socket();
  Logger::info() << "dmserver started" << endl;

  while (1) {
    sockaddr_in client;
    socklen_t addrlen = sizeof(sockaddr_in);
    int socket_d = accept( listen_socket, (sockaddr*) &client, &addrlen );
    if (socket_d < 0) {
      pthread_mutex_lock( &mutex );
      Logger::error() << "cannot connect to client" << endl;
      pthread_mutex_unlock( &mutex );
      continue;
    }
    pthread_t tid;
    HandlerWrapperArgs* args = new HandlerWrapperArgs;
    args->obj = this;
    args->socket_d = socket_d;
    pthread_create( &tid, 0, &Server::client_handler_wrapper, (void*) args );
  }
}

void Server::stop() {
  close(listen_socket);
  Logger::info() << "dmserver stopped\n" << endl;
}

int Server::open_socket() {
  addrinfo hints = {0,0,0,0,0,0,0,0}; // all setted only to suppress warnings
  addrinfo *server_addrinfo, *p;
  int sockfd;
  string port = config.find(id).port;
  hints.ai_family = AF_UNSPEC;  // both IPv4 and IPv6
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  getaddrinfo(NULL, port.c_str(), &hints, &server_addrinfo);
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
  freeaddrinfo(server_addrinfo);
  if (p == NULL) {
    Logger::error() << "cannot open listening socket on port " << port << endl;
    throw runtime_error("");
  }

  if (listen( sockfd, TCP_MAX_CONNECTIONS ) < 0) {
    Logger::error() << "cannot listen on port " << port << endl;
    throw runtime_error("");
  }
  Logger::info() << "listening on port " << port << endl;
  return sockfd;
}

void* Server::client_handler_wrapper(void* args) {
  HandlerWrapperArgs* wargs = (HandlerWrapperArgs*) args;
  wargs->obj->client_handler(wargs->socket_d);
  delete wargs;
  return 0;
}

void Server::client_handler(int socket_d) {
  pthread_detach( pthread_self() );
  string client_id = get_client_id(socket_d);
  Logger::info() << client_id << " connected" << endl;
  Logger::debug() << client_id << " on socket " << socket_d << endl;

  char buffer[TCP_BUFFER_SIZE];
  string message;
  int size = 0;
  do {
    size = recv( socket_d, (void*) buffer, TCP_BUFFER_SIZE-1, 0 );
    buffer[size] = 0;
    message += buffer;

    size_t token_stop = message.find( Message::STOP );
    if (token_stop != string::npos) {
      Message msg(message);
      int block_id;
      string ack = Message::emit(NACK) + Message::STOP;
      map<int,BlockServer>::iterator it;

      switch ( msg.type() ) {
      case MAP:
        block_id = msg.block().id();
        pthread_mutex_lock( &mutex );
        Logger::info() << client_id << " maps block #" << block_id << endl;
        it = blocks.find(block_id);
        if ( it != blocks.end() ) {
          if ( it->second.map(client_id) == 0 ) {
            ack = Message::emit(ACK, it->second) + Message::STOP;
          }
        }
        pthread_mutex_unlock( &mutex );
        break;

      case UNMAP:
        block_id = msg.block().id();
        pthread_mutex_lock( &mutex );
        Logger::info() << client_id << " unmaps block #" << block_id << endl;
        it = blocks.find(block_id);
        if (it != blocks.end()) {
          if ( it->second.unmap(client_id) == 0 ) {
            ack = Message::emit(ACK) + Message::STOP;
          }
          else {
            Logger::error() << "block #" << block_id << " not mapped" << endl;
          }
        }
        pthread_mutex_unlock( &mutex );
        break;

      case UPDATE:
        block_id = msg.block().id();
        pthread_mutex_lock( &mutex );
        Logger::info() << client_id << " updates block #" << block_id << endl;
        it = blocks.find(block_id);
        if (it != blocks.end()) {
          if ( msg.block().revision() < it->second.revision() ) {
            Logger::debug() << "UPDATE sends entire block" << endl;
            ack = Message::emit(ACK, it->second) + Message::STOP;
          }
          else {
            Logger::debug() << "UPDATE sends shallow block" << endl;
            ack = Message::emit(ACK, it->second, true) + Message::STOP;
          }
        }
        pthread_mutex_unlock( &mutex );
        break;

      case WRITE:
        block_id = msg.block().id();
        pthread_mutex_lock( &mutex );
        Logger::info() << client_id << " writes block #" << block_id << endl;
        it = blocks.find(block_id);
        if (it != blocks.end()) {
          Logger::debug() << client_id << " block #" << block_id << " revi " << msg.block().revision() << endl;
          Logger::debug() << client_id << " block #" << block_id << " serv " << it->second.revision() << endl;
          Logger::debug() << client_id << " block #" << block_id << " data " << (char*)msg.block().data() << endl;
          if ( it->second.revision() == msg.block().revision() ) {
            it->second = msg.block();
            it->second.add_revision();
            ack = Message::emit(ACK) + Message::STOP;
          }
        }
        pthread_cond_broadcast( &blocks_wait[block_id] );
        pthread_mutex_unlock( &mutex );
        break;

      case WAIT:
        block_id = msg.block().id();
        ack = Message::emit(ACK) + Message::STOP;
        pthread_mutex_lock( &mutex );
        it = blocks.find(block_id);
        if (it != blocks.end()) {
          while ( it->second.revision() == msg.block().revision() ) {
            Logger::info() << client_id << " waits for block #" << block_id << endl;
            pthread_cond_wait( &blocks_wait[block_id], &mutex );
            Logger::info() << client_id << " stop waiting for block #" << block_id << endl;
          }
        }
        pthread_mutex_unlock( &mutex );
        break;

      default:
        pthread_mutex_lock( &mutex );
        Logger::error() << client_id << " request error: " << message << endl;
        pthread_mutex_unlock( &mutex );
        ack = Message::emit(NACK) + Message::STOP;
        break;
      }
      send(socket_d, (void*) ack.c_str(), ack.size(), 0);
      message.clear();
    }
  } while ( size != 0 );
  pthread_mutex_lock( &mutex );
  Logger::info() << client_id << " disconnected" << endl;
  pthread_mutex_unlock( &mutex );
  close(socket_d);
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
