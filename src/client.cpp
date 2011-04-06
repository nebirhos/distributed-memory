#include "message.h"
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
}

int Client::dm_block_map(int id, void* address) {
  string server_id = config.find_server_id_by_block_id(id);
  if ( server_id.empty() )
    return -1;
  // block already mapped
  if ( blocks.find(id) != blocks.end() )
    return -2;

  BlockLocal block(id, address);
  block.map(server_id);
  blocks.insert( pair<int,BlockLocal>(id,block) );

  int sockfd = server_sockets[server_id];
  if (sockfd < 0)
    return -3;

  string req = Message::emit(MAP, block, true) + Message::STOP;
  send_socket(sockfd, req);

  req = receive_socket(sockfd);
  Message ack( req );
  if (ack.type() == NACK)
    return -4;

  blocks[id] = *( ack.block() );
  return 0;
}

int Client::dm_block_unmap(int id) {
  string server_id = config.find_server_id_by_block_id(id);
  if ( server_id.empty() )
    return -1;
  map<int,BlockLocal>::iterator it = blocks.find(id);
  // block not mapped
  if ( it == blocks.end() )
    return -2;

  int sockfd = server_sockets[server_id];
  if (sockfd < 0)
    return -3;

  string req = Message::emit(UNMAP, it->second, true) + Message::STOP;
  send_socket(sockfd, req);

  req = receive_socket(sockfd);
  Message ack( req );
  if (ack.type() == NACK)
    return -4;

  blocks.erase(it);
  return 0;
}

int Client::dm_block_write(int id) {
  string server_id = config.find_server_id_by_block_id(id);
  if ( server_id.empty() )
    return -1;
  map<int,BlockLocal>::iterator it = blocks.find(id);
  // block not mapped
  if ( it == blocks.end() )
    return -2;

  int sockfd = server_sockets[server_id];
  if (sockfd < 0)
    return -3;

  BlockLocal& block = it->second;
  if ( !block.valid() )
    return -4;

  string req = Message::emit(WRITE, block) + Message::STOP;
  send_socket(sockfd, req);

  req = receive_socket(sockfd);
  Message ack( req );
  if (ack.type() == NACK) {
    block.valid(false);
    return -4;
  }

  block.add_revision();
  return 0;
}


int Client::open_socket(string ip, string port) {
  cout << "Connetto a " << ip << ":" << port << endl; // FIXME
  addrinfo hints = {0};
  addrinfo *server_addrinfo, *p;
  int sockfd;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  getaddrinfo( ip.c_str(), port.c_str(), &hints, &server_addrinfo );
  for (p = server_addrinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket( p->ai_family, p->ai_socktype, p->ai_protocol )) < 0) {
      cout << "errore socket" << endl; // FIXME
      continue;
    }
    if (connect( sockfd, p->ai_addr, p->ai_addrlen ) < 0) {
      close(sockfd);
      cout << "errore connect" << endl; // FIXME
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

int Client::send_socket(int sockfd, string message) {
  return send( sockfd, (void*) message.c_str(), message.size(), 0 );
}

string Client::receive_socket(int sockfd) {
  char buffer[TCP_BUFFER_SIZE];
  string message;
  int size, token_stop;
  do {
    size = recv( sockfd, (void*) buffer, TCP_BUFFER_SIZE-1, 0 );
    buffer[size] = 0;
    message += buffer;
    token_stop = message.find( Message::STOP );
  } while ( (size != 0) && (token_stop == string::npos) );
  return message;
}

} // namespace DM
