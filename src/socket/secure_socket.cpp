/**
 * @authors Francesco Disperati, Alessio Celli
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#include <dm/socket/secure_socket.h>
#include "../logger.h"

#include <cstring>
#include <stdexcept>
#include <stdint.h>             // cross-platform types
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/rand.h>
using namespace std;


namespace DM {

SecureSocket::SecureSocket()
  : m_rsa_privkey(NULL), m_rsa_pubkey(NULL) {}


SecureSocket::~SecureSocket() {
  if ( m_rsa_pubkey )   EVP_PKEY_free( m_rsa_pubkey );
  if ( m_rsa_privkey )  EVP_PKEY_free( m_rsa_privkey );
}


bool SecureSocket::open_server( const string port, const string privkey, const string pass ) {
  if ( ! Socket::open_server(port) ) return false;

  if ( ! load_privkey(privkey) ) {
    Logger::error() << "cannot load RSA private key" << endl;
    return false;
  }
  // Pass must fit in a single RSA packet!
  if ( (int)pass.size() > m_rsa_size - 2*EVP_MAX_KEY_LENGTH ) {
    Logger::error() << "passphrase too long! Max allowed length is " << m_rsa_size - 2*EVP_MAX_KEY_LENGTH << " characters " << endl;
    return false;
  }
  else {
    m_pass = pass;
  }

  return true;
}


bool SecureSocket::open_client( const string host, const string port, const string pubkey, const string pass ) {
  if ( ! Socket::open_client(host, port) ) return false;

  if ( ! load_pubkey(pubkey) ) {
    Logger::error() << "cannot load RSA public key" << endl;
    return false;
  }
  m_pass = pass;

  // SESSION KEY GENERATION
  int rounds = 5;
  unsigned char nonce[ EVP_MAX_KEY_LENGTH ], salt[8];
  RAND_bytes( nonce, EVP_MAX_KEY_LENGTH );
  RAND_bytes( salt, 8 );

  int res = EVP_BytesToKey( CIPHER_ALG, EVP_sha1(), salt,
                            (unsigned char*) m_pass.data(),
                            m_pass.size(), rounds, m_session_key, nonce );
  if (res != EVP_MAX_KEY_LENGTH) {
    Logger::debug() << "cannot generate session key" << endl;
    return false;
  }

  // M1: Request SEND
  unsigned char message[ m_rsa_size ], encrypted[ m_rsa_size ];
  int message_size = 2*EVP_MAX_KEY_LENGTH + m_pass.size();

  memcpy( message, nonce, EVP_MAX_KEY_LENGTH );
  memcpy( message+EVP_MAX_KEY_LENGTH, m_session_key, EVP_MAX_KEY_LENGTH );
  memcpy( message+2*EVP_MAX_KEY_LENGTH, m_pass.data(), m_pass.size() );

  int encrypted_size = RSA_public_encrypt( message_size, message, encrypted, m_rsa_pubkey->pkey.rsa, RSA_PKCS1_PADDING );
  if ( encrypted_size != m_rsa_size ) {
    Logger::error() << "cannot encrypt M1" << endl;
    return false;
  }
  if ( ! send( (char*) encrypted, m_rsa_size ) ) {
    Logger::debug() << "cannot send M1" << endl;
    return false;
  }

  // M2: Response RECEIVE
  unsigned char* decrypted;
  int decrypted_size = recv_secure( decrypted );
  if ( (decrypted_size != 2*EVP_MAX_KEY_LENGTH) ||
       memcmp( decrypted, nonce, EVP_MAX_KEY_LENGTH ) ||
       memcmp( decrypted+EVP_MAX_KEY_LENGTH, m_session_key, EVP_MAX_KEY_LENGTH ) ) {
    Logger::debug() << "cannot decrypt M2" << endl;
    delete[] decrypted;
    return false;
  }

  delete[] decrypted;
  return true;
}


bool SecureSocket::accept( SecureSocket& socket ) const {
  if ( ! Socket::accept(socket) )  return false;

  // M1: Request RECEIVE
  unsigned char message[ m_rsa_size ], decrypted[ m_rsa_size ];
  int message_size = 2*EVP_MAX_KEY_LENGTH + m_pass.size();

  int encrypted_size = socket.recv( (char*) message, m_rsa_size );
  if ( encrypted_size != m_rsa_size ) {
    Logger::error() << "cannot receive M1" << endl;
    return false;
  }
  int decrypted_size = RSA_private_decrypt( m_rsa_size, message, decrypted, m_rsa_privkey->pkey.rsa, RSA_PKCS1_PADDING );
  if ( (decrypted_size != message_size) ||
       memcmp( m_pass.data(), decrypted + 2*EVP_MAX_KEY_LENGTH, m_pass.size() ) ) {
    Logger::error() << "error on M1 decryption or wrong passphrase" << endl;
    return false;
  }
  memcpy( socket.m_session_key, decrypted+EVP_MAX_KEY_LENGTH, EVP_MAX_KEY_LENGTH );

  // M2: Response SEND
  if ( ! socket.send_secure( decrypted, 2*EVP_MAX_KEY_LENGTH ) ) {
    Logger::error() << "cannot send M2" << endl;
    return false;
  }

  return true;
}


int SecureSocket::cipher( int operation, const unsigned char* input, int in_size, unsigned char*& output ) const {
  EVP_CIPHER_CTX ctx;
  EVP_CIPHER_CTX_init( &ctx );
  EVP_CipherInit_ex( &ctx, CIPHER_ALG, NULL, m_session_key, NULL, operation ); // FIXME: iv!!!

  output = new unsigned char[ in_size + EVP_CIPHER_CTX_block_size(&ctx) ];
  int out_size, pad_out_size;
  EVP_CipherUpdate( &ctx, output, &out_size, input, in_size );
  EVP_CipherFinal_ex( &ctx, output+out_size, &pad_out_size );

  EVP_CIPHER_CTX_cleanup( &ctx );
  return (out_size + pad_out_size);
}


bool SecureSocket::load_pubkey( const string path ) {
  FILE *pub_key = fopen( path.c_str(), "r" );
  if ( !pub_key )  return false;
  m_rsa_pubkey = PEM_read_PUBKEY( pub_key, NULL, NULL, NULL );
  fclose( pub_key );

  if ( m_rsa_pubkey )  m_rsa_size = EVP_PKEY_size( m_rsa_pubkey );
  return ( m_rsa_pubkey != NULL );
}


bool SecureSocket::load_privkey( const string path ) {
  FILE *priv_key = fopen( path.c_str(), "r" );
  if ( !priv_key )  return false;
  m_rsa_privkey = PEM_read_PrivateKey( priv_key, NULL, NULL, NULL );
  fclose( priv_key );

  if ( m_rsa_privkey )  m_rsa_size = EVP_PKEY_size( m_rsa_privkey );
  return ( m_rsa_privkey != NULL );
}


bool SecureSocket::send_secure( const unsigned char* data, int size ) const {
  unsigned char* encrypted;
  int encrypted_size = cipher( ENCRYPT, data, size, encrypted );
  if ( encrypted_size < 0 ) {
    delete[] encrypted;
    return false;
  }

  uint32_t pkt_size = encrypted_size;
  if (
      !Socket::send( (const char*) &pkt_size, sizeof(uint32_t) ) ||
      !Socket::send( (const char*) encrypted, encrypted_size ) ) {
    delete[] encrypted;
    return false;
  }

  delete[] encrypted;
  return true;
}


int SecureSocket::recv_secure( unsigned char*& data ) const {
  uint32_t pkt_size;
  int res = Socket::recv( (char*) &pkt_size, sizeof(uint32_t) );
  if ( res == 0 )
    return 0;
  else if ( res != sizeof(uint32_t) )
    return -1;

  unsigned char* buffer = new unsigned char[ pkt_size ];
  res = Socket::recv( (char*) buffer, pkt_size );
  if ( res == 0 )
    return 0;
  else if ( res != (int)pkt_size ) {
    delete[] buffer;
    return -1;
  }

  int decrypted_size = cipher( DECRYPT, buffer, pkt_size, data );
  if ( decrypted_size < 0 ) {
    delete[] buffer;
    return -1;
  }

  delete[] buffer;
  return decrypted_size;
}


const SecureSocket& SecureSocket::operator << ( const string& s ) const {
  if ( ! send_secure( (unsigned char*) s.data(), s.size() ) )
    throw runtime_error( "Could not send to secure socket" );

  return *this;
}


const SecureSocket& SecureSocket::operator >> ( string& s ) const {
  unsigned char* buffer;
  int size = recv_secure(buffer);

  if ( size == 0 )
    throw runtime_error( "Peer disconnected" );

  if ( size < 0 )
    throw runtime_error( "Could not receive from secure socket" );

  s.assign( (char*) buffer, size );
  return *this;
}

} // namespace DM
