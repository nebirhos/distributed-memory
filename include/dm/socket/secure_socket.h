/**
 * @file secure_socket.h
 *
 * @author Francesco Disperati
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#ifndef _DM_SECURE_SOCKET_H_
#define _DM_SECURE_SOCKET_H_

#define ENCRYPT       1
#define DECRYPT       0

#include "socket.h"

#include <openssl/evp.h>
using namespace std;


namespace DM {

class SecureSocket : public Socket {
public:
  const SecureSocket& operator << ( const string& ) const;
  const SecureSocket& operator >> ( string& ) const;

protected:
  SecureSocket();
  virtual ~SecureSocket();

  bool open_server(const string port, const string privkey, const string pass);
  bool open_client(const string host, const string port, const string pubkey, const string pass);

  bool accept( SecureSocket& ) const;

  bool send_cipher(const unsigned char* data, int size) const;
  int recv_cipher(unsigned char*& buffer) const;

private:
  EVP_PKEY* m_priv_key;
  EVP_PKEY* m_pub_key;
  int m_keysize;
  unsigned char m_session_key[ EVP_MAX_KEY_LENGTH ];

  string m_passphrase;

  bool load_privkey( const string path );
  bool load_pubkey( const string path );

  int cipher( int, const unsigned char*, int, unsigned char*& ) const;
};

}

#endif /* _DM_SECURE_SOCKET_H_ */
