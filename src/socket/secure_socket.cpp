/**
 * @author Francesco Disperati
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#include <dm/socket/secure_socket.h>
#include "../logger.h"

#include <cstring>
#include <stdexcept>
#include <stdint.h>
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
  unsigned char* message = new unsigned char[ m_keysize ];
  unsigned char encrypted[ m_keysize ];
  int message_size = 2*EVP_MAX_KEY_LENGTH + m_passphrase.size();

  memcpy( message, nonce, EVP_MAX_KEY_LENGTH );
  memcpy( message+EVP_MAX_KEY_LENGTH, m_session_key, EVP_MAX_KEY_LENGTH );
  memcpy( message+2*EVP_MAX_KEY_LENGTH, m_passphrase.data(), m_passphrase.size() );

  int encrypted_size = RSA_public_encrypt( message_size, message, encrypted, m_pub_key->pkey.rsa, RSA_PKCS1_PADDING );
  if ( encrypted_size != m_keysize ) {
    Logger::debug() << "Error on M1 encryption" << endl;
    delete[] message;
    return false;
  }
  if ( ! send( (char*) encrypted, m_keysize ) ) {
    Logger::debug() << "Error on M1 send" << endl;
    delete[] message;
    return false;
  }
  delete[] message;

  // M2: Response RECEIVE
  unsigned char* decrypted;
  int decrypted_size = recv_cipher( decrypted );
  // cout << "decrypted_size: " << decrypted_size << endl;
  // cout << "2*EVP_MAX_KEY_LENGTH: " << 2*EVP_MAX_KEY_LENGTH << endl;
  if ( (decrypted_size != 2*EVP_MAX_KEY_LENGTH) ||
       memcmp( decrypted, nonce, EVP_MAX_KEY_LENGTH ) ||
       memcmp( decrypted+EVP_MAX_KEY_LENGTH, m_session_key, EVP_MAX_KEY_LENGTH ) ) {
    Logger::debug() << "Error on M2 decryption" << endl;
    delete[] decrypted;
    return false;
  }

  delete[] decrypted;
  return true;
}


bool SecureSocket::accept( SecureSocket& socket ) const {
  if ( ! Socket::accept(socket) )  return false;

  // M1: Request RECEIVE
  unsigned char message[ m_keysize ], decrypted[ m_keysize ];
  int message_size = 2*EVP_MAX_KEY_LENGTH + m_passphrase.size();

  int encrypted_size = socket.recv( (char*) message, m_keysize );
  if ( encrypted_size <= 0 ) {
    Logger::debug() << "Error on M1 receive" << endl;
    return false;
  }
  // cout << "before RSA: " << endl;
  int decrypted_size = RSA_private_decrypt( m_keysize, message, decrypted, m_priv_key->pkey.rsa, RSA_PKCS1_PADDING );
  // cout << "after RSA: " << endl;
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
  if ( ! socket.send_cipher( decrypted, 2*EVP_MAX_KEY_LENGTH ) ) {
    Logger::debug() << "Cannot send M2" << endl;
    return false;
  }

  return true;
}


int SecureSocket::cipher( int operation, const unsigned char* input, int in_size, unsigned char*& output ) const {
  output = new unsigned char[ in_size + AES_BLOCK_SIZE ];
  int out_size, pad_out_size;

  EVP_CIPHER_CTX ctx;
  EVP_CIPHER_CTX_init( &ctx );

  EVP_CipherInit_ex( &ctx, EVP_aes_256_cbc(), NULL, m_session_key, NULL, operation ); // FIXME: iv!!!
  EVP_CipherUpdate( &ctx, output, &out_size, input, in_size );
  EVP_CipherFinal_ex( &ctx, output+out_size, &pad_out_size );

  EVP_CIPHER_CTX_cleanup( &ctx );
  return out_size + pad_out_size;
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


bool SecureSocket::send_cipher(const unsigned char* data, int size) const {
  // cout << "SecureSocket::send_cipher!" << endl;
  unsigned char* encrypted;
  // cout << "send_cipher: size: " << size << endl;
  // cout << "data: " << data << endl;
  int encrypted_size = cipher( ENCRYPT, data, size, encrypted );
  // cout << "send_cipher: after cipher: " << encrypted_size << endl;

  uint32_t pkt_size = encrypted_size;
  if (
      !Socket::send( (const char*) &pkt_size, sizeof(uint32_t) ) ||
      !Socket::send( (const char*) encrypted, encrypted_size ) ) {
    // cout << "suca! pkt_size: " << pkt_size << endl;
    delete[] encrypted;
    return false;
  }
  // cout << "pkt_size: " << pkt_size << endl;
  delete[] encrypted;
  return true;
}


int SecureSocket::recv_cipher(unsigned char*& data) const {
  // cout << "SecureSocket::recv_cipher!" << endl;
  uint32_t pkt_size;
  if ( Socket::recv( (char*) &pkt_size, sizeof(uint32_t) ) != sizeof(uint32_t) ) {
    // cout << "suca! recv pkt_size: " << pkt_size << endl;
    return -1;
  }
  unsigned char *buffer = new unsigned char[ pkt_size ];

  if ( Socket::recv( (char*) buffer, pkt_size ) != pkt_size ) {
    delete[] buffer;
    return -1;
  }
  // cout << "pkt_size: " << pkt_size << endl;
  int decrypted_size = cipher( DECRYPT, buffer, pkt_size, data );
  // cout << "decrypted_size: " << decrypted_size << endl;
  // cout << "data: " << data << endl;
  if ( decrypted_size < 0 ) {
    delete[] buffer;
    return -1;
  }

  delete[] buffer;
  return decrypted_size;
}


const SecureSocket& SecureSocket::operator << ( const string& s ) const {
  // cout << "SecureSocket <<" << endl;
  if ( ! send_cipher( (unsigned char*) s.data(), s.size() ) )
    throw runtime_error( "Could not send to secure socket" );

  return *this;
}


const SecureSocket& SecureSocket::operator >> ( string& s ) const {
  // cout << "SecureSocket >>" << endl;
  unsigned char* buffer;
  int size = recv_cipher(buffer);

  if ( size < 0 )
    throw runtime_error( "Could not receive from secure socket" );

  s.assign( (char*) buffer, size );
  return *this;
}

} // namespace DM
