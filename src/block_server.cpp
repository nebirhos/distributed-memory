/**
 * @author Francesco Disperati
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#include <dm/block_server.h>
#include <iostream>
#include <cstring>
#include <algorithm>
using namespace std;


namespace DM {

BlockServer::BlockServer(int id) : Block(id) {
  m_data = new (nothrow) unsigned char[M_SIZE];
  memset(m_data, 0, M_SIZE);
}

BlockServer::~BlockServer() {
  delete[] m_data;
}

BlockServer::BlockServer(const BlockServer& b)
  : Block(b.m_id) {
  m_data = new unsigned char[M_SIZE];
  memcpy(m_data, b.m_data, M_SIZE);
}

BlockServer& BlockServer::operator=(const BlockServer &b) {
  Block::operator=(b);
  return *this;
}


int BlockServer::map(string client_id) {
  if ( find_mapped_client(client_id) == mapped_clients.end() ) {
    mapped_clients.push_back(client_id);
    return 0;
  }
  else {
    return -1;
  }
}

int BlockServer::unmap(string client_id) {
  vector<string>::iterator it = find_mapped_client(client_id);
  if (it != mapped_clients.end()) {
    mapped_clients.erase(it);
    return 0;
  }
  else {
    return -1;
  }
}

ostream& operator<<(ostream& out, const BlockServer& b) {
  out << "b.id(): " << endl;
  out << "BlockServer: {id: " << b.id();
  out << ", revision: " << b.revision();
  out << ", mapped_clients: [";
  vector<string>::const_iterator it;
  for (it = b.mapped_clients.begin(); it < b.mapped_clients.end(); ++it) {
    out << *it << ", ";
  }
  out << "], " << "data: " << (char*) b.data() << "}";
  return out;
}

vector<string>::iterator BlockServer::find_mapped_client(string client_id) {
  return find(mapped_clients.begin(), mapped_clients.end(), client_id);
}

} // namespace DM
