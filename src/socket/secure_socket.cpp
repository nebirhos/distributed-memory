/**
 * @author Francesco Disperati
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#include <dm/socket/secure_socket.h>
#include "../logger.h"

#include <cstring>
#include <openssl/pem.h>
#include <openssl/rsa.h>
using namespace std;


namespace DM {

SecureSocket::SecureSocket()
  : m_priv_key(0), m_pub_key(0) {}


SecureSocket::~SecureSocket() {
  if ( m_pub_key )   EVP_PKEY_free( m_pub_key );
	if ( m_priv_key )  EVP_PKEY_free( m_priv_key );
}


bool SecureSocket::open_client( const string host, const string port ) {
  if ( ! Socket::open_client(host, port) ) return false;

  cout << "[client] LOAD PUB KEY: "  << load_pubkey(  "serverkey.pub" ) << endl; // FIXME
  cout << "[client] EVP_PKEY_size( m_pub_key ): " << EVP_PKEY_size( m_pub_key ) << endl; // FIXME

  int size = EVP_PKEY_size( m_pub_key );

  // M1: REQUEST
  char* buf = new (nothrow) char[size];
  char* ct = "This the clear text";
  int length = RSA_public_encrypt( strlen(ct)+1, (unsigned char*) ct, (unsigned char*) buf, m_pub_key->pkey.rsa, RSA_PKCS1_PADDING );
  cout << "[client] length: " << length << endl; // FIXME
  Socket::send( buf, size );

  delete[] buf;
  return true;
}


bool SecureSocket::accept( SecureSocket& socket ) const {
  cout << "SecureSocket::accept()" << endl; // FIXME
  if ( ! Socket::accept(socket) )  return false;

  cout << "[server] LOAD PRIV KEY: " << socket.load_privkey( "serverkey.priv" ) << endl; // FIXME

  int size = EVP_PKEY_size( socket.m_priv_key );

  // M2: RESPONSE
  char* buf = new (nothrow) char[size];
  char* decoded = new (nothrow) char[size];
  socket.recv( buf, size );
  int length = RSA_private_decrypt( size, (unsigned char*) buf, (unsigned char*) decoded, socket.m_priv_key->pkey.rsa, RSA_PKCS1_PADDING );
  cout << "[server] length: " << length << endl;
  cout << "[server] decoded: " << decoded << endl;

  delete[] buf;
  delete[] decoded;
  return true;
}


bool SecureSocket::send(const char* data, int size) const {
  return Socket::send(data,size); // FIXME
}


int SecureSocket::recv(char* buffer, int maxsize) const {
  return Socket::recv(buffer,maxsize); // FIXME
}


bool SecureSocket::load_pubkey( const string path ) {
  FILE *pub_key = fopen( path.c_str(), "r" );
  if ( !pub_key )  return false;
  m_pub_key = PEM_read_PUBKEY( pub_key, NULL, NULL, NULL );
  fclose( pub_key );

  return ( pub_key != NULL );
}


bool SecureSocket::load_privkey( const string path ) {
  FILE *priv_key = fopen( path.c_str(), "r" );
  m_priv_key = PEM_read_PrivateKey( priv_key, NULL, NULL, NULL );
  fclose( priv_key );

  return true;
}

} // namespace DM
