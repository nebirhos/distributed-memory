/**
 * @file block_local.h
 *
 * @author Francesco Disperati
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#ifndef _DM_BLOCK_LOCAL_H_
#define _DM_BLOCK_LOCAL_H_

#include "block.h"
#include "block_server.h"
using namespace std;


namespace DM {

/**
 * Memory container for client.
 * BlockLocal is just a Block with no memory allocated.
 *
 * valid() indicates if local block is outdated.
 */
class BlockLocal : public Block {
public:
  /**
   * Constructor. Creates block binded to given address.
   *
   * @param id numeric id
   * @param address pointer to already allocated memory
   */
  BlockLocal(int id = 0, void* address = NULL);
  /** Empty destructor */
  ~BlockLocal() {};
  /** Assignment operator for base class. */
  BlockLocal& operator=(const Block &);

  /** Valid getter. @return true if valid */
  bool valid();
  /** Valid setter. @param value true if valid  */
  void valid(bool);

private:
  /** valid private member */
  bool m_valid;
};

} // namespace DM

#endif /* _DM_BLOCK_LOCAL_H_ */
