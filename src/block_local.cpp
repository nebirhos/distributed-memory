/**
 * @author Francesco Disperati
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

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
