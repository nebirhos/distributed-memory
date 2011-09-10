/**
 * @file socket_client.h
 *
 * @author Francesco Disperati
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#ifndef _DM_SOCKET_CLIENT_H_
#define _DM_SOCKET_CLIENT_H_

#include "socket.h"
using namespace std;


namespace DM {

class SocketClient : public Socket {
public:

  SocketClient( string host, string port );
  virtual ~SocketClient(){};

  const SocketClient& operator << ( const string& ) const;
  const SocketClient& operator >> ( string& ) const;
};

}

#endif /* _DM_SOCKET_CLIENT_H_ */

