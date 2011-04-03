#ifndef _DM_CONFIG_H
#define _DM_CONFIG_H

#include "type.h"
#include <vector>
#include <map>
#include <iostream>
using namespace std;


namespace DM {

/**
 * Config read and stores the configuration file.
 * The file is written using YAML syntax and MUST have these keys:
 * port, ip, blocks
 */
class Config {
public:
  /**
   * Istantiate a new Config object and parse the YAML file.
   * \param filename The path of the config file to parse
   */
  Config(string filename);
  bool is_server_id_valid(string server_id) const;
  vector<int> find_blocks_id_by_server_id(string server_id) const;
  /** Output operator <<. Useful for debug */
  friend ostream& operator<<(ostream& output, const Config& c);

private:
  typedef map<string,ServerConf> ServerHash;
  ServerHash servers;
};

} // namespace DM

#endif /* _DM_CONFIG_H */
