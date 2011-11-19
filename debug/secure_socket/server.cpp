#include <dm/socket/socket_server.h>
#include <iostream>
#include <stdexcept>
using namespace std;


int main(int argc, char *argv[]) {
  if ( argc != 4 ) {
    cout << "Choose [port] [rsa private key] [passphrase]!" << endl;
    return 1;
  }

  try {
    DM::SocketServer listen( argv[1], argv[2], argv[3] );

    while (1) {
      DM::SocketServer* incoming = new DM::SocketServer();

      try {
        listen.accept( *incoming );

        string message;
        (*incoming) >> message;
        cout << "Message: ---" << message << "---" << endl;
      }
      catch (runtime_error e) {
        cout << "runtime_error: " << e.what() << endl;
        delete incoming;
        continue;
      }

      delete incoming;
    }
  }
  catch (const runtime_error& e) {
    cout << "runtim_error: " << e.what() << endl;
    return 1;
  }


  return 0;
}
