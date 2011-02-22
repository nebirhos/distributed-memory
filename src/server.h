#ifndef _DM_SERVER_H_
#define _DM_SERVER_H_

#include "config.h"
#include "block.h"


namespace DM {

class Server {
public:
  Server(string config_path, string server_id);
  void start();
  void stop();

private:
  const Config config;
  const string id;
  vector<Block> blocks;
  int listen_socket;

  int open_socket();
  /**
   * Cause pthread expects a C-style function, we use this static method
   * to call the member method of the Server object
   */
  struct WrapperArgs {
    Server* obj;
    int socket_d;
  };
  static void* thread_wrapper(void*);
  void accept_request(int socket_d);
};

}

#endif /* _DM_SERVER_H_ */
