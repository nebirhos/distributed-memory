#include "server.h"
using namespace std;


namespace DM {

Server::Server(string config_path, string server_id)
  : config(config_path),
    id(server_id) {
  if (!config.is_server_id_valid(id)) {
    throw "Server not found in configuration! FIXME";
  }
  else {
    const vector<int>& blocks_id = config.find_blocks_id_by_server_id(id);
    for (unsigned int i = 0; i < blocks_id.size(); ++i) {
      Block b = Block(blocks_id[i]);
      blocks.push_back(b);
    }
  }
}

void Server::start() {
  while(1) {}
}

}
