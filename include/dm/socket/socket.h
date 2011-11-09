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
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>              // getaddrinfo
using namespace std;


namespace DM {

class Socket {
protected:
  Socket();
  virtual ~Socket();

  bool open_server(const string port);
  bool open_client(const string host, const string port);

  bool accept( Socket& ) const;

  bool send(const char* data, int size) const;
  int recv(char* buffer, int maxsize) const;

  void set_timeout(int seconds);
  bool close();

public:
  bool is_valid() const { return m_sockfd != -1; }
  string get_peer_ip();
  int get_peer_port();

private:
  int m_sockfd;
  /** Contains info about destination address */
  sockaddr_in m_peer_addr;

  bool m_peer_valid;
  void get_peer_addr();
};

} // namespace DM

#endif /* _DM_SOCKET_H_ */
