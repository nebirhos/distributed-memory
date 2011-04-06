#include <dm/client.h>
#include <cstring>
using namespace DM;


int main(int argc, char *argv[]) {
  Client client;
  client.dm_init( argv[1] );

  char* test1 = new char[DIMBLOCK];
  char* test2 = new char[DIMBLOCK];
  client.dm_block_map( 1, test1 );
  client.dm_block_map( 3, test2 );

  client.dm_block_wait( 3 );
  cout << "invalid block!: " << endl; // FIXME

  client.dm_block_unmap( 1 );
  client.dm_block_unmap( 3 );

  return 0;
}
