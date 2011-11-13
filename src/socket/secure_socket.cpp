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
#include <openssl/aes.h>
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
  unsigned char nonce[ EVP_MAX_KEY_LENGTH ], salt[8];
  RAND_bytes( nonce, EVP_MAX_KEY_LENGTH );
  RAND_bytes( salt, 8 );

  int res = EVP_BytesToKey( EVP_aes_256_cbc(), EVP_sha1(), salt,
                            (unsigned char*) m_passphrase.data(),
                            m_passphrase.size(), rounds, m_session_key, nonce );
  if (res != EVP_MAX_KEY_LENGTH) {
    Logger::debug() << "Cannot generate session key" << endl;
    return false;
  }

  // M1: Request SEND
  unsigned char message[ m_keysize ], encrypted[ m_keysize ];
  int message_size = 2*EVP_MAX_KEY_LENGTH + m_passphrase.size();

  memcpy( message, nonce, EVP_MAX_KEY_LENGTH );
  memcpy( message+EVP_MAX_KEY_LENGTH, m_session_key, EVP_MAX_KEY_LENGTH );
  memcpy( message+2*EVP_MAX_KEY_LENGTH, m_passphrase.data(), m_passphrase.size() );

  int encrypted_size = RSA_public_encrypt( message_size, message, encrypted, m_pub_key->pkey.rsa, RSA_PKCS1_PADDING );
  if ( encrypted_size != m_keysize ) {
    Logger::debug() << "Error on M1 encryption" << endl;
    return false;
  }
  if ( ! Socket::send( (char*) encrypted, m_keysize ) ) {
    Logger::debug() << "Error on M1 send" << endl;
    return false;
  }

  // M2: Response RECEIVE
  unsigned char* decrypted;
  if ( ! Socket::recv( (char*) message, M2_ENC_SIZE ) ) {
    Logger::debug() << "Error on M2 receive" << endl;
    return false;
  }
  message_size = cipher( DECRYPT, message, M2_ENC_SIZE, &decrypted );
  if ( (message_size != 2*EVP_MAX_KEY_LENGTH) ||
       memcmp( decrypted, nonce, EVP_MAX_KEY_LENGTH ) ||
       memcmp( decrypted+EVP_MAX_KEY_LENGTH, m_session_key, EVP_MAX_KEY_LENGTH ) ) {
    Logger::debug() << "Error on M2 decryption" << endl;
    return false;
  }

  delete[] decrypted;
  return true;
}


bool SecureSocket::accept( SecureSocket& socket ) const {
  if ( ! Socket::accept(socket) )  return false;

  // M1: Request RECEIVE
  unsigned char message[m_keysize], decrypted[m_keysize];
  int message_size = 2*EVP_MAX_KEY_LENGTH + m_passphrase.size();

  socket.Socket::recv( (char*) message, m_keysize );
  int decrypted_size = RSA_private_decrypt( m_keysize, message, decrypted, m_priv_key->pkey.rsa, RSA_PKCS1_PADDING );
  if ( decrypted_size != message_size ) {
    Logger::debug() << "Error on M1 decryption or wrong passphrase" << endl;
    return false;
  }
  if ( memcmp( m_passphrase.data(), decrypted + 2*EVP_MAX_KEY_LENGTH, m_passphrase.size() ) ) {
    Logger::debug() << "Wrong passphrase provided!" << decrypted+2*EVP_MAX_KEY_LENGTH << endl;
    return false;
  }
  memcpy( socket.m_session_key, decrypted+EVP_MAX_KEY_LENGTH, EVP_MAX_KEY_LENGTH );

  // M2: Response SEND
  unsigned char* encrypted;
  int encrypted_size = socket.cipher( ENCRYPT, decrypted, 2*EVP_MAX_KEY_LENGTH, &encrypted );

  if ( ! socket.Socket::send( (char*) encrypted, encrypted_size ) ) {
    Logger::debug() << "Cannot send M2" << endl;
    delete[] encrypted;
    return false;
  }

  delete[] encrypted;
  return true;
}


int SecureSocket::cipher( int operation, const unsigned char* input, int in_size, unsigned char** output ) {
  *output = new unsigned char[ in_size + AES_BLOCK_SIZE ];
  int out_size, pad_out_size;

  EVP_CIPHER_CTX ctx;
  EVP_CIPHER_CTX_init( &ctx );

  EVP_CipherInit_ex( &ctx, EVP_aes_256_cbc(), NULL, m_session_key, NULL, operation ); // FIXME: iv!!!
  EVP_CipherUpdate( &ctx, *output, &out_size, input, in_size );
  EVP_CipherFinal_ex( &ctx, (*output)+out_size, &pad_out_size );

  EVP_CIPHER_CTX_cleanup( &ctx );
  return out_size + pad_out_size;
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
