#include <dm/socket/socket_server.h>
#include <iostream>
#include <stdexcept>
using namespace std;


int main(int argc, char *argv[]) {
  if ( argc != 4 ) {
    cout << "Choose [port] [rsa private key] [passphrase]!" << endl;
    return 1;
  }

  DM::SocketServer listen( argv[1], argv[2], argv[3] );

  while (1) {
    DM::SocketServer* incoming = new DM::SocketServer();
    try {
      listen.accept( *incoming );
    }
    catch (runtime_error e) {
      delete incoming;
      continue;
    }

    string message;
    (*incoming) >> message;
    cout << "Message: ---" << message << "---" << endl;
    delete incoming;
  }

  return 0;
}
