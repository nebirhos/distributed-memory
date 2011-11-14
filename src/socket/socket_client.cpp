/**
 * @author Francesco Disperati
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#include <dm/socket/socket_client.h>
#include <stdexcept>
using namespace std;


namespace DM {

SocketClient::SocketClient( string host, string port ) {
  if ( ! open_client(host, port) )
    throw runtime_error( "Connection to server failed" );

  set_timeout( TCP_TIMEOUT );
}

}
