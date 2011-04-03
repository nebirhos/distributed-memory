#ifndef _DM_BLOCK_H_
#define _DM_BLOCK_H_

#include "type.h"
#include <vector>
#include <string>
using namespace std;


namespace DM {

class Block {
public:
  Block(int id);

private:
  int id;
  unsigned char data[DIMBLOCK];
  int revision;
  vector<string> mapped_processes;
};

} // namespace DM

#endif /* _DM_BLOCK_H_ */
