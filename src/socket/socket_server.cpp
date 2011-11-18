/**
 * @authors Francesco Disperati, Alessio Celli
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#include <dm/socket/socket_server.h>
#include <stdexcept>
using namespace std;


namespace DM {

SocketServer::SocketServer( const string port, const string privkey, const string pass ) {
  if ( ! open_server(port, privkey, pass) )
    throw runtime_error( "Could not open listening socket" );
}


void SocketServer::accept( SocketServer& socket ) {
  if ( ! SecureSocket::accept( socket ) )
    throw runtime_error( "Could not connect to client" );
}

}
