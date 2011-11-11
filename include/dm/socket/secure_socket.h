/**
 * @file secure_socket.h
 *
 * @author Francesco Disperati
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#ifndef _DM_SECURE_SOCKET_H_
#define _DM_SECURE_SOCKET_H_

#include "socket.h"

#include <openssl/evp.h>
using namespace std;


namespace DM {

class SecureSocket : public Socket {
protected:
  SecureSocket();
  virtual ~SecureSocket();

  bool open_server(const string port);
  bool open_client(const string host, const string port);

  bool accept( SecureSocket& ) const;

  bool send(const char* data, int size) const;
  int recv(char* buffer, int maxsize) const;

private:
  EVP_PKEY* m_priv_key;
  EVP_PKEY* m_pub_key;
  int m_keysize;

  string m_passphrase;

  void prompt_passphrase();
  void choose_passphrase( int maxsize );

  bool load_privkey( const string path );
  bool load_pubkey( const string path );

  bool rsa_encrypt( const void* data, const void* buffer );
  bool rsa_decrypt( const void* data, const void* buffer );
};

}

#endif /* _DM_SECURE_SOCKET_H_ */