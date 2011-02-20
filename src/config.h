#ifndef _DM_CONFIG_H
#define _DM_CONFIG_H

#include <vector>
#include <map>
#include <iostream>

using namespace std;


namespace DM {

/** Config read and stores the configuration file.
 * The file is written in YAML syntax and MUST have these keys:
 * port, ip, blocks
 */
class Config {
  /** Stores the informations about each server */
  struct Server {
    string ip;
    string port;
    vector<int> blocks;
  };
  vector<Server> servers;

public:
  /**
   * Istantiate a new Config object and parse the YAML file.
   * \param filename The path of the config file to parse
   */
  Config(string filename);

  /** Output operator <<. Useful for debug */
  friend ostream& operator<<(ostream& output, const Config& c);
};

} // namespace DM

#endif /* _DM_CONFIG_H */
