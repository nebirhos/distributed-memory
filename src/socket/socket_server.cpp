/**
 * @author Francesco Disperati
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#include <dm/socket/socket_server.h>
#include <stdexcept>
using namespace std;


namespace DM {

SocketServer::SocketServer( string port ) {
  if ( ! open_server(port) )
    throw runtime_error( "Could not open listening socket" );
}


void SocketServer::accept( SocketServer& socket ) {
  if ( ! Socket::accept( socket ) )
    throw runtime_error( "Could not connect to client" );
}

}
