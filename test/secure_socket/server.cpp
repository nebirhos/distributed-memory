#include <dm/socket/socket_server.h>
#include <iostream>
using namespace std;


int main(int argc, char *argv[]) {
  if ( argc != 2 ) {
    cout << "Choose a port!" << endl;
    return 1;
  }

  DM::SocketServer listen( argv[1] );

  while (1) {
    DM::SocketServer* incoming = new DM::SocketServer();
    listen.accept( *incoming );

    string message;
    (*incoming) >> message;
    cout << "Message: ---" << message << "---" << endl;
    delete incoming;
  }

  return 0;
}
