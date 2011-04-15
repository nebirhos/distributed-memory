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
  cout << "Usage: dmserver [options] ip:port" << endl;
  cout << "    -c        specify configuration file path (default: dmconfig.yml)" << endl;
  cout << "    -h        print this help" << endl;
  exit(exit_code);
}

void parse_args(int argc, char* argv[]) {
  int c;
  while ( (c = getopt(argc, argv, "-c:h")) != -1 ) {
    switch (c) {
      case 'c':
        config_path = optarg;
        break;
      case 'h':
        show_usage(0);
      case '\1':
        if (server_id.empty()) {
          server_id = optarg;
        }
        else {
          cout << argv[0] << ": too many arguments" << endl;
          show_usage(1);
        }
        break;
      default:
        show_usage(1);
    }
  }
  if (server_id.empty()) {
    cout << "argument 'ip:port' required" << endl;
    show_usage(1);
  }
}

void kill_handler(int sig) {
  server->stop();
  cout << "Exiting..." << endl;
  exit(0);
}
