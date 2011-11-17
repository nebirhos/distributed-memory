/**
 * @author Francesco Disperati
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#include "message.h"
#include "logger.h"
#include <dm/client.h>
#include <dm/type.h>
#include <stdexcept>
using namespace std;


namespace DM {

Client::Client() : config("") {}


Client::~Client() {
  map<string,SocketClient*>::iterator it;
  for (it = sockets.begin(); it != sockets.end(); ++it) {
    delete it->second;
  }
}


void Client::dm_init(char* config_file) {
  config = Config(config_file);
  if ( !config.is_valid() ) {
    throw runtime_error("configuration file not valid");
  }

  ServerMap servers = config.find_all();
  ServerMap::iterator it;
  for (it = servers.begin(); it != servers.end(); ++it) {
    sockets[it->first] = new SocketClient( it->second.ip, it->second.port, it->second.pubkey, it->second.pass );
  }
}


int Client::dm_block_map(int id, void* address) {
  string server_id = config.find_server_id_by_block_id(id);
  if ( server_id.empty() ) {
    Logger::debug() << "block #" << id << " not found" << endl;
    return -1;
  }
  if ( blocks.find(id) != blocks.end() ) {
    Logger::debug() << "block #" << id << " already mapped" << endl;
    return -2;
  }
  SocketClient& sock = *sockets[server_id];
  if ( !sock.is_valid() ) {
    Logger::debug() << "socket not valid" << endl;
    return -3;
  }

  BlockLocal block(id, address);
  blocks.insert( pair<int,BlockLocal>(id,block) );

  string req = Message::emit(MAP, block, true) + Message::STOP;
  try {
    sock << req;
    sock >> req;
  }
  catch(runtime_error e) {
    Logger::debug() << e.what() << endl;
    return -4;
  }
  Message ack( req );
  if (ack.type() == NACK) {
    Logger::debug() << "NACK received mapping block #" << id << endl;
    return -5;
  }

  blocks[id] = ack.block();
  return 0;
}


int Client::dm_block_unmap(int id) {
  string server_id = config.find_server_id_by_block_id(id);
  if ( server_id.empty() ) {
    Logger::debug() << "block #" << id << " not found" << endl;
    return -1;
  }
  map<int,BlockLocal>::iterator it = blocks.find(id);
  if ( it == blocks.end() ){
    Logger::debug() << "block #" << id << " not mapped" << endl;
    return -2;
  }
  SocketClient& sock = *sockets[server_id];
  if ( !sock.is_valid() ) {
    Logger::debug() << "socket not valid" << endl;
    return -3;
  }

  string req = Message::emit(UNMAP, it->second, true) + Message::STOP;
  try {
    sock << req;
    sock >> req;
  }
  catch(runtime_error e) {
    Logger::debug() << e.what() << endl;
    return -4;
  }
  Message ack( req );
  if (ack.type() == NACK) {
    Logger::debug() << "NACK received unmapping block #" << id << endl;
    return -5;
  }

  blocks.erase(it);
  return 0;
}


int Client::dm_block_update(int id) {
  string server_id = config.find_server_id_by_block_id(id);
  if ( server_id.empty() ) {
    Logger::debug() << "block #" << id << " not found" << endl;
    return -1;
  }
  map<int,BlockLocal>::iterator it = blocks.find(id);
  if ( it == blocks.end() ){
    Logger::debug() << "block #" << id << " not mapped" << endl;
    return -2;
  }
  SocketClient& sock = *sockets[server_id];
  if ( !sock.is_valid() ) {
    Logger::debug() << "socket not valid" << endl;
    return -3;
  }

  BlockLocal& block = it->second;
  string req = Message::emit(UPDATE, block, true) + Message::STOP;
  try {
    sock << req;
    sock >> req;
  }
  catch(runtime_error e) {
    Logger::debug() << e.what() << endl;
    return -4;
  }
  Message ack( req );
  if (ack.type() == NACK) {
    Logger::debug() << "NACK received" << endl;
    return -5;
  }

  // updates only if local block is invalid
  if ( block.revision() != ack.block().revision() ) {
    block = ack.block();
    block.valid(true);
  }

  return 0;
}


int Client::dm_block_write(int id) {
  string server_id = config.find_server_id_by_block_id(id);
  if ( server_id.empty() ) {
    Logger::debug() << "block #" << id << " not found" << endl;
    return -1;
  }
  map<int,BlockLocal>::iterator it = blocks.find(id);
  if ( it == blocks.end() ){
    Logger::debug() << "block #" << id << " not mapped" << endl;
    return -2;
  }
  SocketClient& sock = *sockets[server_id];
  if ( !sock.is_valid() ) {
    Logger::debug() << "socket not valid" << endl;
    return -3;
  }

  BlockLocal& block = it->second;
  if ( !block.valid() ) {
    Logger::debug() << "block #" << id << " invalid" << endl;
    return -5;
  }

  string req = Message::emit(WRITE, block) + Message::STOP;
  try {
    sock << req;
    sock >> req;
  }
  catch(runtime_error e) {
    Logger::debug() << e.what() << endl;
    return -4;
  }
  Message ack( req );
  if (ack.type() == NACK) {
    block.valid(false);
    Logger::debug() << "writing failed for block #" << id << endl;
    return -5;
  }

  block.add_revision();
  return 0;
}


int Client::dm_block_wait(int id) {
  string server_id = config.find_server_id_by_block_id(id);
  if ( server_id.empty() ) {
    Logger::debug() << "block #" << id << " not found" << endl;
    return -1;
  }
  map<int,BlockLocal>::iterator it = blocks.find(id);
  if ( it == blocks.end() ){
    Logger::debug() << "block #" << id << " not mapped" << endl;
    return -2;
  }
  SocketClient& sock = *sockets[server_id];
  if ( !sock.is_valid() ) {
    Logger::debug() << "socket not valid" << endl;
    return -3;
  }

  BlockLocal& block = it->second;
  if ( !block.valid() ) {
    Logger::debug() << "block #" << id << " invalid" << endl;
    return 0;
  }

  string req = Message::emit(WAIT, block, true) + Message::STOP;
  try {
    sock << req;
    sock >> req;
  }
  catch(runtime_error e) {
    Logger::debug() << e.what() << endl;
    return -4;
  }
  Message ack( req );
  if (ack.type() == NACK) {
    Logger::debug() << "NACK received waiting block #" << id << endl;
    return -5;
  }

  block.valid(false);
  return 0;
}

} // namespace DM
