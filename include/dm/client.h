#ifndef _DM_CLIENT_H_
#define _DM_CLIENT_H_

#include "config.h"
#include <dm/block_local.h>
#include <iostream>


namespace DM {

class Client {
public:
  Client();
  void dm_init(char* config_file);
  int dm_block_map(int id, void* address);
  int dm_block_unmap(int id);
  int dm_block_update(int id);
  int dm_block_write(int id);
  int dm_block_wait(int id);

private:
  Config config;
  map<string,int> server_sockets;
  map<int,BlockLocal> blocks;

  int open_socket(string, string);
  int send_socket(int, string);
  string receive_socket(int);
};

} // namespace DM

#endif /* _DM_CLIENT_H_ */
