/**
 * @file client.h
 *
 * @author Francesco Disperati
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

/**
 * @mainpage Distributed Memory Documentation
 *
 * @section intro What is Distributed Memory?
 * A simplified distributed memory system for POSIX compliant systems written in C++.
 *
 * The system involves a server application (<b>dmserver</b>) that offers a certain
 * amount of memory to the clients, and a static library to access this
 * memory (<b>libdm</b>).
 * Memory is arranged in blocks uniquely identified by a number. The number
 * of blocks, servers and their mapping are managed by a configuration
 * file.
 * Of course, server and client can run on different nodes on the
 * network, and use JSON messages over TCP/IP for communication.
 *
 * This project is intended for the course of Operating Systems And
 * Distributed Programming, held by prof. Giuseppe Lipari.
 *
 * @section howto How do I use this documentation?
 * Documentation provides a brief explanation on how to develop a Distributed Memory
 * client using the library, and an analysis of overall implementation.
 *
 * Before starting, you should take a look at the project specifications
 * (doc/progetto-unix2010.pdf) and README.
 *
 * Start @link DM::Client here @endlink
 */

#ifndef _DM_CLIENT_H_
#define _DM_CLIENT_H_

#include "config.h"
#include "block_local.h"
#include "socket/socket_client.h"
#include <iostream>

/** Distributed Memory interfaces and implementations */
namespace DM {

/**
 * Provides all the methods to interface with servers.
 * Global memory is arranged in @ref Block "blocks" of
 * @ref DIMBLOCK "DIMBLOCK" bytes. Every server has an unique
 * copy of a block, identified by a numeric id.
 *
 * Mapping between blocks and servers is specified in the
 * Config file.
 *
 * Locally to the client blocks are pointers to memory area.
 *
 * Steps to use a block:
 *   -# Connect to the servers (dm_init())
 *   -# Map a remote block to a local block (dm_block_map())
 *   -# ... operates on the block (dm_block_update(),
 *      dm_block_write(), dm_block_wait())
 *   -# When done, unmap the block (dm_block_unmap())
 */
class Client {
public:
  Client();
  ~Client();

  /**
   * Reads and parses configuration file and connects to
   * servers.
   *
   * @param config_file configuration file path
   *
   * @throw runtime_error
   *   - if config file is not found/not valid
   *   - if connection to server fails
   */
  void dm_init(char* config_file);

  /**
   * Binds local block to remote block.
   * Client requests the block specified and server
   * responds by sending the block data. Data is copied
   * in the area pointed by address, both client and
   * server store association.
   *
   * @param id block id
   * @param address memory area pointer
   *
   * @return status code:
   *   -  0: everything OK
   *   - -1: block id not valid
   *   - -2: block already mapped
   *   - -3: connection failed
   *   - -4: connection timeout
   *   - -5: request rejected by server
   */
  int dm_block_map(int id, void* address);

  /**
   * Unbinds remote block.
   * Notifies server that client is no more interested
   * in the block. Client and server remove association.
   *
   * @param id block id
   *
   * @return status code:
   *   -  0: everything OK
   *   - -1: block id not valid
   *   - -2: block not mapped
   *   - -3: connection failed
   *   - -4: connection timeout
   *   - -5: request rejected by server
   */
  int dm_block_unmap(int id);

  /**
   * Update local block with remote block data.
   * If local block is already updated to the latest version,
   * no data is transfered from server.
   *
   * @param id block id
   *
   * @return status code:
   *   -  0: everything OK
   *   - -1: block id not valid
   *   - -2: block not mapped
   *   - -3: connection failed
   *   - -4: connection timeout
   *   - -5: request rejected by server
   */
  int dm_block_update(int id);

  /**
   * Sync local block with remote block data.
   * If local block is not updated to the latest version,
   * synchronization cannot be done and method returns.
   *
   * @param id block id
   *
   * @return status code:
   *   -  0: everything OK
   *   - -1: block id not valid
   *   - -2: block not mapped
   *   - -3: connection failed
   *   - -4: connection timeout
   *   - -5: local block outdated, writing failed
   */
  int dm_block_write(int id);

  /**
   * Wait until local block becomes outdated.
   * Returns when server notifies the block is outdated,
   * or when a timeout occurs.
   *
   * @param id block id
   *
   * @return status code:
   *   -  0: everything OK
   *   - -1: block id not valid
   *   - -2: block not mapped
   *   - -3: connection failed
   *   - -4: connection timeout
   *   - -5: request rejected by server
   */
  int dm_block_wait(int id);

private:
  /** Configuration data */
  Config config;
  /** Stores communication sockets for each server */
  map<string,SocketClient*> sockets;
  /** Stores local blocks */
  map<int,BlockLocal> blocks;
};

} // namespace DM

#endif /* _DM_CLIENT_H_ */
