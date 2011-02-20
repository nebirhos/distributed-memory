#include "config.h"
#include "yaml.h"
#include <fstream>
#include <iostream>

using namespace std;


namespace DM {

Config::Config(string filename) {
  try {
    ifstream file(filename.c_str());
    YAML::Parser parser(file);
    YAML::Node doc;
    // we assume that there's only one document
    parser.GetNextDocument(doc);

    for (unsigned int i = 0; i < doc.size(); ++i) {
      const YAML::Node& server_conf = doc[i];
      Server server;
      server_conf["ip"] >> server.ip;
      server_conf["port"] >> server.port;
      server.blocks = server_conf["blocks"];
      servers.push_back(server);
    }
  }
  catch(...) {
    cout << "Errore del cazzo!" << endl;
  }
}

ostream& operator<<(ostream& output, const Config& c) {
  for (unsigned int i = 0; i < c.servers.size(); ++i) {
    const Config::Server& server = c.servers[i];
    output << "Server " << i << ":" << endl;
    output << "  ip:     " << server.ip << endl;
    output << "  port:   " << server.port << endl;
    output << "  blocks: [";
    for (unsigned int i = 0; i < server.blocks.size(); ++i) {
      output << server.blocks[i];
      if (i < server.blocks.size() - 1) {
        cout << ", ";
      }
    }
    output << "]" << endl;
  }
  return output;
}

} // namespace DM
