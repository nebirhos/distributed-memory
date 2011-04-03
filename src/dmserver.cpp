#include "server.h"
#include <iostream>
#include <cstdlib>
#include <signal.h>
using namespace std;


string config_path = DEFAULT_CONFIG_PATH;
string server_id;

void show_usage(int exit_code);
void parse_args(int argc, char* argv[]);
void kill_handler(int sig);

int main(int argc, char *argv[]) {
  parse_args(argc, argv);
  cout << "server_id: " << server_id << endl;
  cout << "config_path: " << config_path << endl;

  DM::Server server = DM::Server(config_path, server_id);
  signal(SIGINT, kill_handler);
  server.start();

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
}

void kill_handler(int sig) {
  cout << "Exiting..." << endl;
  exit(0);
}
