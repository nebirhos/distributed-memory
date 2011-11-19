#include <dm/socket/socket_client.h>
#include <iostream>
#include <stdexcept>
using namespace std;


int main(int argc, char *argv[]) {
  if ( argc != 5 ) {
    cout << "Choose [host] [port] [rsa public key] [passphrase]!" << endl;
    return 1;
  }

  try {
    DM::SocketClient s( argv[1], argv[2], argv[3], argv[4] );
    s << "The narwhal bacons at midnight";
  }
  catch (const runtime_error& e) {
    cout << "runtime_error: " << e.what() << endl;
    return 1;
  }

  return 0;
}
