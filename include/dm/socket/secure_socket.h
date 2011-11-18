/**
 * @file secure_socket.h
 *
 * @authors Francesco Disperati, Alessio Celli
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#ifndef _DM_SECURE_SOCKET_H_
#define _DM_SECURE_SOCKET_H_

/** Operation code for OpenSSL EVP encryption */
#define ENCRYPT       1
/** Operation code for OpenSSL EVP decryption */
#define DECRYPT       0
/** Algorithm for symmetric encryption */
#define CIPHER_ALG    EVP_aes_256_cbc()

#include "socket.h"
#include <openssl/evp.h>
using namespace std;


namespace DM {

/**
 * Enhances Socket class with security features.
 *
 * This class implements a security protocol for authentication
 * and establishment of a secure connection between server and client.
 * Given the RSA public key of the server and a shared passphrase
 * the protocol generates a session key for the encryption of data exchanged.
 *
 * See doc/progetto-sicurezza2011.pdf for the full requirements (track 3),
 * and doc/report-sicurezza2011.pdf for a short report about
 * project realization (both in Italian).
 */
class SecureSocket : public Socket {
public:
  /** Encrypts and sends a string */
  const SecureSocket& operator << ( const string& ) const;
  /** Receives and decrypts a string */
  const SecureSocket& operator >> ( string& ) const;

protected:
  SecureSocket();
  virtual ~SecureSocket();

  /** Opens server listening socket and sets private key and pass */
  bool open_server( const string port, const string privkey, const string pass );
  /** Opens connection to server with security protocol */
  bool open_client( const string host, const string port, const string pubkey, const string pass );
  /** Accepts connection from client with security protocol */
  bool accept( SecureSocket& ) const;

  /** Encrypts and sends data to socket */
  bool send_secure( const unsigned char* data, int size ) const;
  /** Receives and decrypts data from socket */
  int recv_secure( unsigned char*& data ) const;

private:
  /** Shared secret */
  string m_pass;
  EVP_PKEY* m_rsa_privkey;
  EVP_PKEY* m_rsa_pubkey;
  int m_rsa_size;
  /** Key for session encryption */
  unsigned char m_session_key[ EVP_MAX_KEY_LENGTH ];

  bool load_privkey( const string path );
  bool load_pubkey( const string path );

  /**
   * Encrypts/decrypts input data into output buffer (automatically allocated).
   *
   * @param operation  Operation code @see ENCRYPT @see DECRYPT
   * @param input      data to encrypt/decrypt
   * @param in_size    number of input bytes
   * @param output     reference to pointer for output
   *
   * @return Bytes written in output buffer or -1 on error
   */
  int cipher( int operation, const unsigned char* input, int in_size, unsigned char*& output ) const;
};

}

#endif /* _DM_SECURE_SOCKET_H_ */
