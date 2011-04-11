#include <dm/block_local.h>
#include <cstring>
using namespace std;


namespace DM {

BlockLocal::BlockLocal(int id, void* address)
  : Block(id), m_valid(true) {
  m_data = (unsigned char*) address;
}

BlockLocal& BlockLocal::operator=(const Block &b) {
  m_id = b.id();
  m_revision = b.revision();
  memcpy(m_data, b.data(), M_SIZE);
  return *this;
}

bool BlockLocal::valid() {
  return m_valid;
}

void BlockLocal::valid(bool value) {
  m_valid = value;
}

} // namespace DM
