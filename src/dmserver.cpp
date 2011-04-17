/**
 * @author Francesco Disperati
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#include "server.h"
#include <iostream>
#include <cstdlib>
#include <signal.h>
#include <stdexcept>
using namespace std;


string config_path = DEFAULT_CONFIG_PATH;
string server_id;
DM::Server* server;

void show_usage(int exit_code);
void parse_args(int argc, char* argv[]);
void kill_handler(int sig);

int main(int argc, char *argv[]) {
  parse_args(argc, argv);
  try {
    server = new DM::Server(config_path, server_id);
    signal(SIGINT, kill_handler);
    server->start();
  }
  catch(runtime_error e) {
    cout << "Something nasty happened" << e.what() << ", exiting..." << endl;
    exit(1);
  }
  return 0;
}

void show_usage(int exit_code) {
  cout << "Usage: dmserver [OPTIONS] -i IP:PORT" << endl;
  cout << "    -i        IP:PORT of this server" << endl;
  cout << "    -c        specify configuration file path (default: dmconfig.yml)" << endl;
  cout << "    -h        print this help" << endl;
  exit(exit_code);
}

void parse_args(int argc, char* argv[]) {
  int c;
  while ( (c = getopt(argc, argv, "-c:i:h")) != -1 ) {
    switch (c) {
      case 'c':
        config_path = optarg;
        break;
      case 'h':
        show_usage(0);
      case 'i':
        server_id = optarg;
        break;
      default:
        show_usage(1);
    }
  }
  if (server_id.empty()) {
    cout << "argument -i IP:PORT required" << endl;
    show_usage(1);
  }
}

void kill_handler(int sig) {
  server->stop();
  cout << "Exiting..." << endl;
  exit(0);
}
