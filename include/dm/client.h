#ifndef _DM_CLIENT_H_
#define _DM_CLIENT_H_

#include "config.h"
#include <iostream>


namespace DM {

class Client {
public:
  Client();
  void dm_init(char* config_file);
  int dm_block_map(int ID, void* address);
  int dm_block_unmap(int ID);
  int dm_block_update(int ID);
  int dm_block_write(int ID);
  int dm_block_wait(int ID);

private:
  Config config;
  map<string,int> server_sockets;

  int open_socket(string, string);
};

} // namespace DM

#endif /* _DM_CLIENT_H_ */
