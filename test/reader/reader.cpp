#include <dm/client.h>
#include <cstring>
using namespace DM;


int main(int argc, char *argv[]) {
  Client client;
  client.dm_init( argv[1] );

  char* test1 = new char[DIMBLOCK];
  char* test2 = new char[DIMBLOCK];
  client.dm_block_map( 1, test1 );
  client.dm_block_map( 2, test2 );

  memcpy( test1, "FOXY TEST", 9 );
  client.dm_block_write( 1 );

  client.dm_block_unmap( 1 );
  client.dm_block_unmap( 2 );

  return 0;
}
