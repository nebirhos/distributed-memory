#include <dm/block_local.h>
using namespace std;


namespace DM {

BlockLocal::BlockLocal(int id) : Block(id), m_valid(true) {}

int BlockLocal::map(string server_id) {
  if ( m_server_id.empty() ) {
    m_server_id = server_id;
    return 0;
  }
  else {
    return -1;
  }
}

bool BlockLocal::valid() {
  return m_valid;
}

void BlockLocal::valid(bool value) {
  m_valid = value;
}

} // namespace DM
