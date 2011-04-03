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
};

}

#endif /* _DM_SERVER_H_ */
