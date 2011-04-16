/**
 * @file block_server.h
 *
 * @author Francesco Disperati
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#ifndef _DM_BLOCK_SERVER_H_
#define _DM_BLOCK_SERVER_H_

#include "block.h"
#include <vector>
using namespace std;


namespace DM {

/**
 * Memory container for server.
 * Memory data is contained inside every object, properly
 * managing dynamic memory in constructor/destructor.
 *
 * BlockServer stores also the id of every client that has
 * requested the block. Client id are in the same form of server
 * id (ip:port).
 */
class BlockServer : public Block {
public:
  /**
   * Constructor. Instantiate a block object with given id.
   *
   * @param id numeric id of the block
   */
  BlockServer(int id = 0);
  /** Destructor. Frees allocated memory. */
  ~BlockServer();
  /** Copy constructor. Performs a deep-copy. */
  BlockServer(const BlockServer &);
  /** Assignment operator. Redefines the default class assignment to keep mapped clients info */
  BlockServer& operator=(const BlockServer &);

  /**
   * Stores the client id.
   *
   * @param client_id string with ip:port
   *
   * @return
   *  -  0: mapping successful
   *  - -1: client already mapped
   */
  int map(string client_id);

  /**
   * Remove the client id stored.
   *
   * @param client_id string (ip:port)
   *
   * @return
   *  -  0: unmapping successful
   *  - -1: client not mapped
   */
  int unmap(string client_id);

  /** Output operator <<, useful for debug  */
  friend ostream& operator<<(ostream&, const BlockServer&);

private:
  /** Stores mapped client ids */
  vector<string> mapped_clients;

  /** Find mapped client for given id, if any */
  vector<string>::iterator find_mapped_client(string client_id);
};

} // namespace DM

#endif /* _DM_BLOCK_SERVER_H_ */
