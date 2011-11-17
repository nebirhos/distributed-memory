#include <dm/socket/socket_client.h>
#include <iostream>
using namespace std;


int main(int argc, char *argv[]) {
  if ( argc != 3 ) {
    cout << "Choose host and port!" << endl;
    return 1;
  }

  DM::SocketClient s( argv[1], argv[2] );

  s << "The narwhal bacons at midnight";

  return 0;
}
