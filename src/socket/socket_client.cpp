/**
 * @author Francesco Disperati
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#include <dm/socket/socket_client.h>
#include "../logger.h"
#include "../message.h"
#include <stdexcept>
using namespace std;


namespace DM {

SocketClient::SocketClient( string host, string port ) {
  if ( ! open_client(host, port) ) 
    throw runtime_error( "Connection to server failed" );

  set_timeout( TCP_TIMEOUT );
}	


const SocketClient& SocketClient::operator << ( const string& s ) const {
  if ( ! Socket::send( s.c_str(), s.size() ) )
    throw runtime_error( "Could not send to socket" );

  return *this;
}


const SocketClient& SocketClient::operator >> ( string& s ) const {
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

}
