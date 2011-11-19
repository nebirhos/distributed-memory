/**
 * @file socket_client.h
 *
 * @authors Francesco Disperati, Alessio Celli
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#ifndef _DM_SOCKET_CLIENT_H_
#define _DM_SOCKET_CLIENT_H_

#include "secure_socket.h"
using namespace std;


namespace DM {

/** socket for use in Client processes */
class SocketClient : public SecureSocket {
public:

  SocketClient( const string host, const string port, const string pubkey, const string pass );
  virtual ~SocketClient(){};
};

}

#endif /* _DM_SOCKET_CLIENT_H_ */

