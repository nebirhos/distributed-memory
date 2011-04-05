#ifndef _DM_BLOCK_H_
#define _DM_BLOCK_H_

#include <dm/type.h>
#include <string>
using namespace std;


namespace DM {

class Block {
public:
  Block(int id);
  virtual ~Block() {};

  int size() const;
  int id() const;
  int revision() const;
  void* data() const;

  void revision(int);
  virtual void data(const void*);

  string dump_hex() const;

protected:
  static const int M_SIZE = DIMBLOCK;
  int m_id;
  int m_revision;
  unsigned char* m_data;
};

} // namespace DM

#endif /* _DM_BLOCK_H_ */