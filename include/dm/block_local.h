#ifndef _DM_BLOCK_LOCAL_H_
#define _DM_BLOCK_LOCAL_H_

#include "block.h"
#include "block_server.h"
using namespace std;


namespace DM {

class BlockLocal : public Block {
public:
  BlockLocal(int id = 0, void* address = NULL);
  ~BlockLocal() {};
  BlockLocal& operator=(const Block &);

  int map(string server_id);
  bool valid();
  void valid(bool);

private:
  string m_server_id;
  bool m_valid;
};

} // namespace DM

#endif /* _DM_BLOCK_LOCAL_H_ */
