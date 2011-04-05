#include <dm/client.h>
using namespace DM;


int main(int argc, char *argv[]) {
  Client client;
  client.dm_init( argv[1] );

  char* test = new char[128];
  client.dm_block_map( 1, test );

  return 0;
}
