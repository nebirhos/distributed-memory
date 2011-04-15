#include <dm/block_local.h>
#include <cstring>
using namespace std;


namespace DM {

BlockLocal::BlockLocal(int id, void* address)
  : Block(id), m_valid(true) {
  m_data = (unsigned char*) address;
}

BlockLocal& BlockLocal::operator=(const Block &b) {
  Block::operator=(b);
  return *this;
}

bool BlockLocal::valid() {
  return m_valid;
}

void BlockLocal::valid(bool value) {
  m_valid = value;
}

} // namespace DM
