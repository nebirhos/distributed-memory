#ifndef _DM_SERVER_H_
#define _DM_SERVER_H_

#include <dm/config.h>
#include <dm/block_server.h>
#include <pthread.h>


namespace DM {

class Server {
public:
  Server(string config_path, string id);
  void start();
  void stop();

private:
  const Config config;
  const string id;
  map<int,BlockServer> blocks;
  int listen_socket;
  pthread_mutex_t mutex;

  int open_socket();
  /**
   * Cause pthread expects a C-style function, we use this static method
   * to call the member method of the Server object. Maybe exter "C" is better
   */
  struct HandlerWrapperArgs {
    Server* obj;
    int socket_d;
  };
  static void* client_handler_wrapper(void*);
  void client_handler(int socket_d);

  string get_client_id(int);
};

}

#endif /* _DM_SERVER_H_ */
