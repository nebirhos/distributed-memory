#include <dm/socket/socket_client.h>
#include <iostream>
using namespace std;


int main(int argc, char *argv[]) {
  if ( argc != 4 ) {
    cout << "Choose host port password!" << endl;
    return 1;
  }

  DM::SocketClient s( argv[1], argv[2] );

  s << "fanculo!!!";

  return 0;
}
