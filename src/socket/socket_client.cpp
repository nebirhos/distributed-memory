/**
 * @authors Francesco Disperati, Alessio Celli
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#include <dm/socket/socket_client.h>
#include <stdexcept>
using namespace std;


namespace DM {

SocketClient::SocketClient( const string host, const string port, const string pubkey, const string pass ) {
  if ( ! open_client(host, port, pubkey, pass) )
    throw runtime_error( "Connection to server failed" );

  set_timeout( TCP_TIMEOUT );
}

}
