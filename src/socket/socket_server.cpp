/**
 * @author Francesco Disperati
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#include <dm/socket/socket_server.h>
#include "../message.h"
#include <stdexcept>
using namespace std;


namespace DM {

SocketServer::SocketServer( string host, string port ) {
  if ( ! open_server(host, port) )
    throw runtime_error( "Could not open listening socket" );
}


const SocketServer& SocketServer::operator << ( const string& s ) const {
  if ( ! Socket::send( s.c_str(), s.size() ) )
    throw runtime_error( "Could not send to socket" );

  return *this;
}


const SocketServer& SocketServer::operator >> ( string& s ) const {
  char buffer[ TCP_BUFFER_SIZE ];
  string message;
  int size;
  size_t token_stop;
  do {
    size = Socket::recv( buffer, TCP_BUFFER_SIZE-1 );
    buffer[size] = 0;
    message += buffer;
    token_stop = message.find( Message::STOP );
  } while ( (size > 0) && (token_stop == string::npos) );

  if (size < 0)
    throw runtime_error( "Could not receive from socket" );

  s = message;
  return *this;
}


void SocketServer::accept( SocketServer& socket ) {
  if ( ! Socket::accept( socket ) )
    throw runtime_error( "Could not connect to client" );
}

}
