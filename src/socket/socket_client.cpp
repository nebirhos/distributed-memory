/**
 * @author Francesco Disperati
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */
#include "socket_client.h"
#include "../logger.h"
using namespace std;


namespace DM {

SocketClient::SocketClient( string host, string port ) {
  if ( ! open_client(host, port) ) 
      throw "Could not create client socket.";
  
}	


const SocketClient& SocketClient::operator << ( const string& s ) const {
  if ( ! Socket::send( s ) ) 
      throw "Could not write to socket.";
  
  return *this;
}


const SocketClient& SocketClient::operator >> ( string& s ) const {
  if ( ! Socket::recv ( s ) ) 
      throw "Could not read from socket.";
  
  return *this;
}

}
