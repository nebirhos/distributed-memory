#ifndef _DM_TYPE_H_
#define _DM_TYPE_H_

#define DIMBLOCK 128
#define DEFAULT_CONFIG_PATH "dmconfig.yml"

#include <string>
#include <vector>
using namespace std;


namespace DM {

struct ServerConf {
  string ip;
  string port;
  vector<int> blocks_id;
};

} // namespace DM

#endif /* _DM_TYPE_H_ */
