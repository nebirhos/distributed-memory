/**
 * @file socket_server.h
 *
 * @author Francesco Disperati
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#ifndef _DM_SOCKET_SERVER_H_
#define _DM_SOCKET_SERVER_H_

#include "socket.h"
using namespace std;


namespace DM {

class SocketServer : public Socket {
public:

  SocketServer( string host, string port );
  SocketServer() {};
  virtual ~SocketServer() {};

  const SocketServer& operator << ( const string& ) const;
  const SocketServer& operator >> ( string& ) const;

  void accept( SocketServer& );
};

} // namespace DM

#endif /* _DM_SOCKET_SERVER_H_ */
