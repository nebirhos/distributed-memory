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
#include <openssl/rand.h>
using namespace std;


namespace DM {

SecureSocket::SecureSocket()
  : m_priv_key(NULL), m_pub_key(NULL) {}


SecureSocket::~SecureSocket() {
  if ( m_pub_key )   EVP_PKEY_free( m_pub_key );
	if ( m_priv_key )  EVP_PKEY_free( m_priv_key );
}


bool SecureSocket::open_server( const string port ) {
  if ( ! Socket::open_server(port) ) return false;

  if ( ! load_privkey("serverkey.priv") ) { // FIXME
    Logger::debug() << "Cannot load RSA private key" << endl;
    return false;
  }
  choose_passphrase( m_keysize - 2*EVP_MAX_KEY_LENGTH );
  return true;
}


bool SecureSocket::open_client( const string host, const string port ) {
  if ( ! Socket::open_client(host, port) ) return false;

  if ( ! load_pubkey("serverkey.pub") ) { // FIXME
    Logger::debug() << "Cannot load RSA public key" << endl;
    return false;
  }
  prompt_passphrase();

  // SESSION KEY GENERATION
  int rounds = 5;
  unsigned char key[ EVP_MAX_KEY_LENGTH ], nonce[ EVP_MAX_KEY_LENGTH ], salt[8];
  RAND_bytes( nonce, EVP_MAX_KEY_LENGTH );
  RAND_bytes( salt, 8 );

  int res = EVP_BytesToKey( EVP_aes_256_cbc(), EVP_sha1(), salt,
                            (unsigned char*) m_passphrase.c_str(),
                            m_passphrase.size(), rounds, key, nonce );
  if (res != EVP_MAX_KEY_LENGTH) {
    Logger::debug() << "Cannot generate session key" << endl;
    return false;
  }

  // Message 1: send request
  unsigned char message[ m_keysize ], encoded[ m_keysize ];
  int message_size = 2*EVP_MAX_KEY_LENGTH + m_passphrase.size();
  memcpy( message, nonce, EVP_MAX_KEY_LENGTH );
  memcpy( message+EVP_MAX_KEY_LENGTH, key, EVP_MAX_KEY_LENGTH );
  memcpy( message+2*EVP_MAX_KEY_LENGTH, m_passphrase.c_str(), m_passphrase.size() );

  int encoded_size = RSA_public_encrypt( message_size, message, encoded, m_pub_key->pkey.rsa, RSA_PKCS1_PADDING );
  if ( encoded_size != m_keysize ) {
    Logger::debug() << "Error on M1 encryption" << endl;
    return false;
  }
  if ( ! Socket::send( (char*) encoded, m_keysize ) ) {
    Logger::debug() << "Error on M1 send" << endl;
    return false;
  }

  // M2: receive response

  return true;
}


bool SecureSocket::accept( SecureSocket& socket ) const {
  if ( ! Socket::accept(socket) )  return false;

  // M1: receive request
  unsigned char buffer[m_keysize], decoded[m_keysize];
  unsigned char key[EVP_MAX_KEY_LENGTH], nonce[EVP_MAX_KEY_LENGTH];
  int message_size = 2*EVP_MAX_KEY_LENGTH + m_passphrase.size();
  socket.Socket::recv( (char*) buffer, m_keysize );
  int decoded_size = RSA_private_decrypt( m_keysize, buffer, decoded, m_priv_key->pkey.rsa, RSA_PKCS1_PADDING );
  cout << "[server] decoded_size: " << decoded_size << endl; // FIXME
  cout << "[server] decoded: " << decoded << endl; // FIXME
  if ( decoded_size != message_size ) {
    Logger::debug() << "Error on M1 decryption or wrong passphrase" << endl;
    return false;
  }
  cout << "m_passphrase.size(): " << m_passphrase.size() << endl;
  cout << "decoded_size-2*EVP_MAX_KEY_LENGTH: " << decoded_size-2*EVP_MAX_KEY_LENGTH << endl;
  if ( m_passphrase.compare((char*) decoded + 2*EVP_MAX_KEY_LENGTH) ) {
    Logger::debug() << "Wrong passphrase! Provided: " << decoded+2*EVP_MAX_KEY_LENGTH << endl;
    return false;
  }
  memcpy( decoded, nonce, EVP_MAX_KEY_LENGTH );
  memcpy( decoded+EVP_MAX_KEY_LENGTH, key, EVP_MAX_KEY_LENGTH );

  // M2: send response

  return true;
}


bool SecureSocket::send(const char* data, int size) const {
  cout << "SecureSocket::send" << endl;
  return Socket::send(data,size); // FIXME
}


int SecureSocket::recv(char* buffer, int maxsize) const {
  cout << "SecureSocket::recv" << endl;
  return Socket::recv(buffer,maxsize); // FIXME
}


void SecureSocket::prompt_passphrase() {
  cout << "Insert passphrase: ";
  system( "stty -echo" );       // FIXME
  getline( cin, m_passphrase );
  system( "stty echo" );
  cout << endl;
  cout << "m_passphrase: " << m_passphrase << endl;
}


void SecureSocket::choose_passphrase( int maxsize ) {
  string pass, pass_confirm;
  do {
    cout << "Choose a passphrase (max. " << maxsize << " characters): ";
    getline( cin, pass );
    cout << "Retype your passphrase: ";
    getline( cin, pass_confirm );
  } while ( pass.compare(pass_confirm) || (pass.size() > maxsize) );

  m_passphrase = pass;
}


bool SecureSocket::load_pubkey( const string path ) {
  FILE *pub_key = fopen( path.c_str(), "r" );
  if ( !pub_key )  return false;
  m_pub_key = PEM_read_PUBKEY( pub_key, NULL, NULL, NULL );
  fclose( pub_key );

  if ( m_pub_key )  m_keysize = EVP_PKEY_size( m_pub_key );
  return ( m_pub_key != NULL );
}


bool SecureSocket::load_privkey( const string path ) {
  FILE *priv_key = fopen( path.c_str(), "r" );
  if ( !priv_key )  return false;
  m_priv_key = PEM_read_PrivateKey( priv_key, NULL, NULL, NULL );
  fclose( priv_key );

  if ( m_priv_key )  m_keysize = EVP_PKEY_size( m_priv_key );
  return ( m_priv_key != NULL );
}

} // namespace DM
