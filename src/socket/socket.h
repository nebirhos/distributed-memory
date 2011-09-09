/**
 * @file socket.h
 *
 * @author Francesco Disperati
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#ifndef _DM_SOCKET_H_
#define _DM_SOCKET_H_

#include <dm/type.h>
#include <string>
#include <netdb.h>    // getaddrinfo
using namespace std;


namespace DM {

class Socket {
public:
  Socket();
  virtual ~Socket();

  bool open_server(const string host, const string port);
  bool open_client(const string host, const string port);

  bool send(const char* data, int size) const;
  int recv(char* buffer, int maxsize) const;

  bool close();

  bool is_valid() const { return m_sockfd != -1; }

private:
  int m_sockfd;
  addrinfo m_hints;
};

} // namespace DM

#endif /* _DM_SOCKET_H_ */
