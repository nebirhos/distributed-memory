#include "config.h"
#include "yaml.h"
#include <fstream>
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
      ServerConf server;
      server_conf["ip"] >> server.ip;
      server_conf["port"] >> server.port;
      server.blocks_id = server_conf["blocks"];
      string id = server.ip + ":" + server.port;
      servers[id] = server;
    }
  }
  catch(...) {
    cout << "FIXME" << endl;
  }
}

bool Config::is_server_id_valid(string server_id) const {
  return (servers.find(server_id) != servers.end());
}

vector<int> Config::find_blocks_id_by_server_id(string server_id) const {
  return servers.find(server_id)->second.blocks_id;
}

ostream& operator<<(ostream& output, const Config& c) {
  for (Config::ServerHash::const_iterator it = c.servers.begin(); it != c.servers.end(); it++) {
    const string server_id = it->first;
    const ServerConf& server = it->second;
    output << "Server " << server_id << endl;
    output << "  ip:     " << server.ip << endl;
    output << "  port:   " << server.port << endl;
    output << "  blocks: [";
    for (unsigned int i = 0; i < server.blocks_id.size(); ++i) {
      output << server.blocks_id[i];
      if (i < server.blocks_id.size() - 1) {
        cout << ", ";
      }
    }
    output << "]" << endl;
  }
  return output;
}

} // namespace DM
