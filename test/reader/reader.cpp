#include <dm/client.h>
using namespace DM;


int main(int argc, char *argv[]) {
  Client client;
  client.dm_init( argv[1] );

  return 0;
}
