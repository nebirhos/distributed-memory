/**
 * @file type.h
 *
 * Defines constants and data types for DM namespace.
 *
 * @author Francesco Disperati
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#ifndef _DM_TYPE_H_
#define _DM_TYPE_H_

/** Size (bytes) of a block */
#define DIMBLOCK 128
/** Default path for configuration file */
#define DEFAULT_CONFIG_PATH "dmconfig.yml"
/** TCP max concurrent connections for servers */
#define TCP_MAX_CONNECTIONS 64
/** TCP buffer size (bytes) for receive */
#define TCP_BUFFER_SIZE 1024
/** TCP timeout (seconds) for connections */
#define TCP_TIMEOUT 20

#include <string>
#include <vector>
#include <map>
using namespace std;


namespace DM {

/** Stores the server configuration details. */
struct ServerConf {
  /** listening ip of the server */
  string ip;
  /** listening port of the server */
  string port;
  /** vector of blocks number in the server */
  vector<int> blocks_id;
  /** Secret passphrase for client authentication */
  string pass;
  /** RSA private key path */
  string privkey;
  /** RSA public key path */
  string pubkey;
};

/**
 * Binds each server id with its own configuration.
 * id is the concatenation of ip and port (eg "127.0.0.1:123"). */
typedef map<string,ServerConf> ServerMap;

/**
 * Type of messages for the server-client communication protocol.
 *
 * @see Client for a full explanation about actions
 */
enum MessageType {
  MAP    = 1,                   /**< map action */
  UNMAP  = 2,                   /**< unmap action */
  UPDATE = 3,                   /**< update action */
  WRITE  = 4,                   /**< write action */
  WAIT   = 5,                   /**< wait action */
  ACK    = 6,                   /**< request completed successfully */
  NACK   = 7,                   /**< request not performed */
  UNDEF  = 0                    /**< invalid request */
};

/**
 * Specifies priority levels for Logger. The items are in ascending priority.
 * If a message has a priority lower than LOG_LEVEL is discarded.
 *
 * @see Makefile for LOG_LEVEL definition
 */
enum LogLevel {
  LOG_DEBUG,                    /**< Debug and other needless infos */
  LOG_INFO,                     /**< Notice infos */
  LOG_ERROR                     /**< Error condition infos */
};

} // namespace DM

#endif /* _DM_TYPE_H_ */
