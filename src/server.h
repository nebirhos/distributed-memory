/**
 * @file server.h
 *
 * @author Francesco Disperati
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#ifndef _DM_SERVER_H_
#define _DM_SERVER_H_

#include <dm/config.h>
#include <dm/block_server.h>
#include <pthread.h>


namespace DM {

/**
 * Implementation of server functions.
 * Server creates memory blocks based on configuration file and opens a
 * listening socket based on its ID.
 * Every client connection is served by a new thread.
 */
class Server {
public:
  /**
   * Constructor. Reads the configuration file given and instantiates
   * blocks based on given id.
   *
   * @param config_path configuration file path
   * @param id string containing the server ip:port
   *
   * @throw runtime_error
   *   - if config file is not found/not valid
   *   - if server id is not found
   */
  Server(string config_path, string id);

  /** Opens listening socket and waits for connections */
  void start();
  /** Close listening socket and quits */
  void stop();

private:
  /** Configuration data */
  const Config config;
  /** Server id (ip:port) */
  const string id;
  /** Stores managed blocks */
  map<int,BlockServer> blocks;
  /** Listening socket file descriptor */
  int listen_socket;
  /** Pthread mutex for critical sections */
  pthread_mutex_t mutex;
  /** Pthread condition for each block */
  map<int,pthread_cond_t> blocks_wait;

  /**
   * Open listening socket.
   * @return socket file descriptor or -1 if fails
   */
  int open_socket();

  /** Structure to allocate arguments to pass to pthread */
  struct HandlerWrapperArgs {
    /** Pointer to server instance */
    Server* obj;
    /** socket for client communication */
    int socket_d;
  };
  /**
   * Cause pthread expects a C-style function, we use this static method
   * to call the member method of the Server object. Maybe extern "C" is better?
   */
  static void* client_handler_wrapper(void*);
  /** Actual body of pthread */
  void client_handler(int socket_d);

  /**
   * Gets connected client ip and port.
   *
   * @param socket_d socket file descriptor
   * @return client id (ip:port)
   */
  string get_client_id(int);
};

}

#endif /* _DM_SERVER_H_ */
