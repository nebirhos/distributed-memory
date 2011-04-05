#ifndef _DM_BLOCK_SERVER_H_
#define _DM_BLOCK_SERVER_H_

#include "block.h"
#include <vector>
using namespace std;


namespace DM {

class BlockServer : public Block {
public:
  BlockServer(int id = 0);
  ~BlockServer();
  BlockServer(const BlockServer &);

  int map(string client_id);
  int unmap(string client_id);
  bool is_client_mapped(string client_id);

private:
  vector<string> mapped_clients;

  vector<string>::iterator find_mapped_client(string client_id);
};

} // namespace DM

#endif /* _DM_BLOCK_SERVER_H_ */
