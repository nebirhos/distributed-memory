#include "block.h"
#include <iostream>
using namespace std;


namespace DM {

Block::Block(int id) : id(id) {
  for (int i = 0; i < DIMBLOCK; ++i) {
    data[i] = 0;
  }
}

} // namespace DM
