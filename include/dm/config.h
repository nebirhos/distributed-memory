#ifndef _DM_CONFIG_H
#define _DM_CONFIG_H

#include "type.h"
#include <vector>
#include <map>
#include <iostream>
using namespace std;


namespace DM {

/**
 * Read and stores the configuration file data.
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
  bool is_valid() const;
  const ServerConf& find(string id) const;
  const ServerMap& find_all() const;
  const string find_server_id_by_block_id(int) const;
  /** Output operator <<. Useful for debug */
  friend ostream& operator<<(ostream& output, const Config& c);

private:
  ServerMap servers;
};

} // namespace DM

#endif /* _DM_CONFIG_H */
