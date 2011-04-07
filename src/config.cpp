#include <dm/config.h>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <sstream>
#include <algorithm>
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
      ostringstream id;
      id << server.ip << ":" << server.port;
      servers[id.str()] = server;
    }
  }
  catch(...) {
    cout << "FIXME" << endl;
  }
}

ServerConf Config::find(string id) const {
  if (servers.find(id) == servers.end() ) {
    throw "FIXME";
  }
  return servers.find(id)->second;
}

ServerMap Config::find_all() const {
  return servers;
}

string Config::find_server_id_by_block_id(int block_id) const {
  string result = "";
  for (ServerMap::const_iterator sit = servers.begin(); sit != servers.end(); sit++) {
    const vector<int>& blocks = sit->second.blocks_id;
    vector<int>::const_iterator bit = std::find( blocks.begin(), blocks.end(), block_id );
    if ( bit != blocks.end() ) {
      result = sit->first;
    }
  }
  return result;
}

ostream& operator<<(ostream& output, const Config& c) {
  for (ServerMap::const_iterator it = c.servers.begin(); it != c.servers.end(); it++) {
    const string server_id = it->first;
    const ServerConf& server = it->second;
    output << "Server " << server_id << endl;
    output << "  ip:     " << server.ip << endl;
    output << "  port:   " << server.port << endl;
    output << "  blocks: [";
    for (unsigned int i = 0; i < server.blocks_id.size(); ++i) {
      output << server.blocks_id[i];
      if ( i < server.blocks_id.size() - 1 ) {
        output << ", ";
      }
    }
    output << "]\n";
  }
  return output;
}

} // namespace DM
