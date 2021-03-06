/**
 * @file socket_server.h
 *
 * @authors Francesco Disperati, Alessio Celli
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#ifndef _DM_SOCKET_SERVER_H_
#define _DM_SOCKET_SERVER_H_

#include "secure_socket.h"
using namespace std;


namespace DM {

/** socket for use in Server processes */
class SocketServer : public SecureSocket {
public:

  SocketServer( const string port, const string privkey, const string pass );
  SocketServer() {};
  virtual ~SocketServer() {};

  void accept( SocketServer& );
};

} // namespace DM

#endif /* _DM_SOCKET_SERVER_H_ */
