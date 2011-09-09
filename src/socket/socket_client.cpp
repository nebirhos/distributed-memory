/**
 * @author Francesco Disperati
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#include "socket_client.h"
#include "../logger.h"
#include "../message.h"
using namespace std;


namespace DM {

SocketClient::SocketClient( string host, string port ) {
  if ( ! open_client(host, port) ) 
    throw "Could not create client socket.";
  
}	


const SocketClient& SocketClient::operator << ( const string& s ) const {
  if ( ! Socket::send( s.c_str(), s.size() ) )
    throw "Could not write to socket.";
  
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

  if (size < 0) {
    message.clear();
    Logger::debug() << "TIMEOUT recv(): " << size << endl;
  }

  s += message;
  return *this;
}

}
