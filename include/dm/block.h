/**
 * @file block.h
 *
 * @author Francesco Disperati
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#ifndef _DM_BLOCK_H_
#define _DM_BLOCK_H_

#include "type.h"
#include <string>
using namespace std;


namespace DM {

/**
 * Fixed length memory container. Every block refers to a portion of memory and
 * it's used to exchange data between client and server. No allocation
 * strategy is implemented for memory in base class.
 *
 * There are two types of blocks:
 *   -# BlockServer self-containing block, every block has its own memory
 *   -# BlockLocal points to an external memory area.
 *
 * Every block has some metadata besides the data content:
 *   - id: numeric id, unique for every block
 *   - revision: number incremented every time the block changes
 */
class Block {
public:
  /**
   * Constructor. Instantiate a block object with given id.
   *
   * @param id numeric id of the block
   */
  Block(int id);
  /** Virtual destructor, defined in derived classes */
  virtual ~Block() {};
  /** Assignment operator. Performs a deep-copy of the memory pointed by block */
  Block& operator=(const Block &);

  /** Size getter. @return size in byte */
  int size() const;
  /** Id getter. @return numeric id */
  int id() const;
  /** Revision getter. @return revision number */
  int revision() const;
  /** Data getter. @return void* pointer to data */
  void* data() const;

  /** Revision setter. @param rev new revision to assign */
  void revision(int);
  /** Data setter. @param data deep-copy of data into block */
  void data(const void*);

  /** Increment revision by one */
  void add_revision();
  /** See the data content in hex */
  string dump_hex() const;

protected:
  /** Size of a block. Defined at compile time. @see DIMBLOCK */
  static const int M_SIZE = DIMBLOCK;
  /** id private member */
  int m_id;
  /** revision private member */
  int m_revision;
  /** data private member */
  unsigned char* m_data;
};

} // namespace DM

#endif /* _DM_BLOCK_H_ */
