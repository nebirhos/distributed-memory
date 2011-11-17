/**
 * @file config.h
 *
 * @author Francesco Disperati
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#ifndef _DM_CONFIG_H
#define _DM_CONFIG_H

#include "type.h"
#include <vector>
#include <map>
#include <iostream>
using namespace std;


namespace DM {

/**
 * Reads and stores the configuration file data.
 * The file is written using YAML syntax and MUST have
 * these keys for each server:
 *   - ip:      ip or canonical address of server
 *   - port:    port of server (number or service name)
 *   - blocks:  list of block ids for that server
 *   - pass:    secret passphrase for client authentication
 *   - privkey: path to the RSA private key
 *   - pubkey:  path to the RSA public key
 *
 * Example:
@verbatim
  port: http
  ip: localhost
  blocks: [1, 2, 3]
  pass: "A shared secret"
  privkey: ~/.ssh/id.priv
  pubkey:  ~/.ssh/id.pub
@endverbatim
 *
 * references:
 *   1. http://www.yaml.org/
 */
class Config {
public:
  /**
   * Constructor. Instantiates a new Config object and parses the YAML file.
   *
   * @param filename The path of the configuration file to parse
   */
  Config(string filename);

  /**
   * Checks if the configuration is valid. Configuration may be invalid if
   * provided file doesn't exists, or due to bad syntax.
   *
   * @return true if configuration was parsed successfully, false otherwise
   */
  bool is_valid() const;

  /**
   * Finds and returns details of a server configuration. Every server is
   * uniquely identified by the pair ip:port, as specified in configuration file
   *
   * @see ServerConf
   * @param id string containing the server ip:port
   * @return reference to ServerConf, or a void reference if element isn't found
   */
  const ServerConf& find(string id) const;

  /**
   * Returns all server details found in configuration. The details are
   * stored in a std::map of <server_id,ServerConf>.
   *
   * @see ServerMap
   * @return reference to ServerMap
   */
  const ServerMap& find_all() const;

  /**
   * Finds the server managing the specified block.
   *
   * @param block_id numeric id of the block
   * @return server id, or empty string if nothing is found
   */
  const string find_server_id_by_block_id(int) const;

  /** Output operator <<, useful for debug */
  friend ostream& operator<<(ostream& output, const Config& c);

private:
  /** Stores the configs for every server */
  ServerMap servers;
};

} // namespace DM

#endif /* _DM_CONFIG_H */
