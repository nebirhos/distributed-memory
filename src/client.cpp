#include "message.h"
#include "logger.h"
#include <dm/client.h>
#include <dm/type.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>             // strerror
#include <stdexcept>
using namespace std;


namespace DM {

Client::Client() : config("") {}

void Client::dm_init(char* config_file) {
  config = Config(config_file);
  if ( !config.is_valid() ) {
    throw runtime_error("configuration file '" + string(config_file) + "' not valid");
  }

  ServerMap servers = config.find_all();
  ServerMap::iterator it;
  timeval timeout;
  timeout.tv_sec = TCP_TIMEOUT;
  timeout.tv_usec = 0;
  for (it = servers.begin(); it != servers.end(); ++it) {
    int sockfd = open_socket(it->second.ip, it->second.port);
    server_sockets[it->first] = sockfd;
    int res = setsockopt( sockfd, SOL_SOCKET, SO_RCVTIMEO, (timeval*) &timeout, sizeof(timeval) );
    Logger::debug() << "set RCVTIMEO: " << res << " " << strerror(errno) << endl;
    res = setsockopt( sockfd, SOL_SOCKET, SO_SNDTIMEO, (timeval*) &timeout, sizeof(timeval) );
    Logger::debug() << "set SNDTIMEO: " << res << " " << strerror(errno) << endl;
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
  blocks.insert( pair<int,BlockLocal>(id,block) );

  int sockfd = server_sockets[server_id];
  if (sockfd < 0)
    return -3;

  string req = Message::emit(MAP, block, true) + Message::STOP;
  send_socket(sockfd, req);

  req = receive_socket(sockfd);
  if (req.empty())
    return -4;

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
  if (req.empty())
    return -4;

  Message ack( req );
  if (ack.type() == NACK)
    return -4;

  blocks.erase(it);
  return 0;
}

int Client::dm_block_update(int id) {
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
  string req = Message::emit(UPDATE, block, true) + Message::STOP;
  Logger::debug() << "UPDATE message: " << req << endl; // FIXME
  send_socket(sockfd, req);

  req = receive_socket(sockfd);
  if (req.empty())
    return -4;

  Message ack( req );
  if (ack.type() == NACK)
    return -4;

  // updates only if local block is invalid
  if (block.revision() != ack.block()->revision()) {
    Logger::debug() << "before: " << block.revision() << endl; // FIXME
    block = *( ack.block() );
    block.valid(true);
    Logger::debug() << "after: " << block.revision() << endl; // FIXME
  }

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
  Logger::debug() << "req: " << req << endl;
  send_socket(sockfd, req);

  req = receive_socket(sockfd);
  if (req.empty())
    return -4;

  Message ack( req );
  if (ack.type() == NACK) {
    block.valid(false);
    return -4;
  }

  block.add_revision();
  return 0;
}

int Client::dm_block_wait(int id) {
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
    return 0;

  string req = Message::emit(WAIT, block, true) + Message::STOP;
  send_socket(sockfd, req);

  req = receive_socket(sockfd);
  if (req.empty())
    return -4;

  Message ack( req );
  if (ack.type() != ACK) {
    return -4;
  }

  block.valid(false);
  return 0;
}

int Client::open_socket(string ip, string port) {
  Logger::debug() << "connects to " << ip << ":" << port << endl;
  addrinfo hints = {0,0,0,0,0,0,0,0}; // all setted only to suppress warnings
  addrinfo *server_addrinfo, *p;
  int sockfd;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  getaddrinfo( ip.c_str(), port.c_str(), &hints, &server_addrinfo );
  for (p = server_addrinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket( p->ai_family, p->ai_socktype, p->ai_protocol )) < 0) {
      Logger::debug() << "error on socket()" << endl;
      continue;
    }
    if (connect( sockfd, p->ai_addr, p->ai_addrlen ) < 0) {
      close(sockfd);
      Logger::debug() << "error on connect()" << endl;
      continue;
    }
    // if we reach this point we have a valid socket
    break;
  }
  if (p == NULL) {
    Logger::error() << "error connecting " << ip << ":" << port << ": " << strerror(errno) << endl;
    sockfd = -1;
  }
  freeaddrinfo(server_addrinfo);
  return sockfd;
}

int Client::send_socket(int sockfd, string message) {
  int result = send( sockfd, (void*) message.c_str(), message.size(), 0 );
  if (result < 0) {
    Logger::debug() << "TIMEOUT send(): " << result << endl; 
  }
  return result;
}

string Client::receive_socket(int sockfd) {
  char buffer[TCP_BUFFER_SIZE];
  string message;
  int size;
  size_t token_stop;
  do {
    size = recv( sockfd, (void*) buffer, TCP_BUFFER_SIZE-1, 0 );
    buffer[size] = 0;
    message += buffer;
    token_stop = message.find( Message::STOP );
  } while ( (size > 0) && (token_stop == string::npos) );
  if (size < 0) {
    message.clear();
    Logger::debug() << "TIMEOUT recv(): " << size << endl; 
  }

  return message;
}

} // namespace DM
