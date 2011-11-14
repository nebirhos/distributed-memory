/**
 * @author Francesco Disperati
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#include <dm/socket/socket.h>
#include "../logger.h"
#include "../message.h"

#include <cstring>
#include <cerrno>
#include <arpa/inet.h>
#include <stdexcept>
using namespace std;


namespace DM {

Socket::Socket()
  : m_sockfd(-1),
    m_peer_valid(false) {
  memset( &m_peer_addr, 0, sizeof( m_peer_addr ) );
}


Socket::~Socket() {
  close();
}


bool Socket::open_server(const string port) {
  addrinfo *server_addrinfo, *p, hints;
  memset( &hints, 0, sizeof(hints) );
  hints.ai_family = AF_INET; // FIXME: IPv6
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;


  getaddrinfo( NULL, port.c_str(), &hints, &server_addrinfo );

  for ( p = server_addrinfo; p != NULL; p = p->ai_next ) {
    if ((m_sockfd = socket( p->ai_family, p->ai_socktype, p->ai_protocol )) < 0) {
      Logger::debug() << "error on socket(): " << strerror(errno) << endl;
      continue;
    }
    if (bind( m_sockfd, p->ai_addr, p->ai_addrlen ) < 0) {
      close();
      Logger::debug() << "error on bind():" << strerror(errno) << endl;
      continue;
    }
    // if we reach this point we have a valid socket
    break;
  }
  if (p == NULL) {
    Logger::error() << "could not open listening socket on port " << port << endl;
    close();
    return false;
  }
  freeaddrinfo(server_addrinfo);

  if (listen( m_sockfd, TCP_MAX_CONNECTIONS ) < 0) {
    Logger::error() << "could not listen on port " << port << endl;
    close();
    return false;
  }

  Logger::info() << "listening on port " << port << endl;
  return true;
}


bool Socket::open_client(const string host, const string port) {
  addrinfo *server_addrinfo, *p, hints;
  memset( &hints, 0, sizeof(hints) );
  hints.ai_family = AF_INET; // FIXME: IPv6
  hints.ai_socktype = SOCK_STREAM;

  getaddrinfo( host.c_str(), port.c_str(), &hints, &server_addrinfo );

  for (p = server_addrinfo; p != NULL; p = p->ai_next) {
    if ((m_sockfd = socket( p->ai_family, p->ai_socktype, p->ai_protocol )) < 0) {
      Logger::debug() << "error on socket(): " << strerror(errno) << endl;
      continue;
    }
    if (connect( m_sockfd, p->ai_addr, p->ai_addrlen ) < 0) {
      close();
      Logger::debug() << "error on connect():" << strerror(errno) << endl;
      continue;
    }
    // if we reach this point we have a valid socket
    break;
  }
  if (p == NULL) {
    Logger::debug() << "error connecting " << host << ":" << port << ": " << strerror(errno) << endl;
    close();
    return false;
  }
  freeaddrinfo(server_addrinfo);

  return true;
}


bool Socket::accept( Socket& socket ) const {
  socket.m_sockfd = ::accept( m_sockfd, NULL, NULL );
  return ( socket.m_sockfd > 0 );
}


bool Socket::send(const char* data, int size) const {
  int status = ::send( m_sockfd, (void*) data, size, 0 );
  return ( status == size );    // FIXME: log error
}


int Socket::recv(char* buffer, int maxsize) const {
  int size = ::recv( m_sockfd, (void*) buffer, maxsize, 0 );
  return size;                  // FIXME: log error
}


void Socket::set_timeout(int seconds) {
  timeval timeout;
  timeout.tv_sec = seconds;
  timeout.tv_usec = 0;
  setsockopt( m_sockfd, SOL_SOCKET, SO_RCVTIMEO, (timeval*) &timeout, sizeof(timeval) );
  setsockopt( m_sockfd, SOL_SOCKET, SO_SNDTIMEO, (timeval*) &timeout, sizeof(timeval) );
}


bool Socket::close() {
  if ( is_valid() ) {
    ::close( m_sockfd );
    m_sockfd = -1;
    return true;
  }
  return false;
}


const Socket& Socket::operator << ( const string& s ) const {
  if ( ! send( s.c_str(), s.size() ) )
    throw runtime_error( "Could not send to socket" );

  return *this;
}


const Socket& Socket::operator >> ( string& s ) const {
  char buffer[ TCP_BUFFER_SIZE ];
  string message;
  int size;
  size_t token_stop;
  do {
    size = recv( buffer, TCP_BUFFER_SIZE-1 );
    buffer[size] = 0;
    message += buffer;
    token_stop = message.find( Message::STOP );
  } while ( (size > 0) && (token_stop == string::npos) );

  if (size < 0)
    throw runtime_error( "Could not receive from socket" );

  s = message;
  return *this;
}


string Socket::get_peer_ip() {
  get_peer_addr();

  char client_ip[ INET_ADDRSTRLEN ];
  inet_ntop( AF_INET, &m_peer_addr.sin_addr.s_addr, client_ip, sizeof(client_ip) );
  return string( client_ip );
}


int Socket::get_peer_port() {
  get_peer_addr();

  return (int) m_peer_addr.sin_port;
}


void Socket::get_peer_addr() {
  if ( !m_peer_valid ) {
    socklen_t addr_size = sizeof( m_peer_addr );
    getpeername( m_sockfd, (sockaddr*) &m_peer_addr, &addr_size );

    m_peer_valid = true;
  }
}

} // namespace DM
