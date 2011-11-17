/**
 * @author Francesco Disperati
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#include "server.h"
#include "message.h"
#include <sstream>
#include "logger.h"
#include <stdexcept>
using namespace std;


namespace DM {

Server::Server(string config_path, string id)
  : config(config_path),
    id(id) {
  Logger::info() << "Distributed Memory server" << endl;
  if ( !config.is_valid() ) {
    Logger::error() << "configuration file '" << config_path << "' not valid" << endl;
    throw runtime_error("");
  }
  if ( &config.find(id) == NULL ) {
    Logger::error() << "server '" << id << "' not found in configuration" << endl;
    throw runtime_error("");
  }
  const vector<int>& blocks_id = config.find(id).blocks_id;
  for (unsigned int i = 0; i < blocks_id.size(); ++i) {
    BlockServer b( blocks_id[i] );
    blocks.insert( pair<int,BlockServer>(blocks_id[i], b) );
    pthread_cond_init( &blocks_wait[blocks_id[i]], 0 );
    Logger::debug() << "create block #" << b.id() << endl;
  }
  pthread_mutex_init(&mutex, 0);
}

Server::~Server() {
  if (listen_socket) delete listen_socket;
}


void Server::start() {
  string port = config.find(id).port;
  try {
    listen_socket = new SocketServer( port.c_str() );
  }
  catch (runtime_error e) {
    Logger::error() << e.what() << endl;
    stop();
    return;
  }
  Logger::info() << "dmserver started" << endl;

  while (1) {
    SocketServer* new_socket = new SocketServer();
    try {
      listen_socket->accept( *new_socket );
    }
    catch (runtime_error e) {
      pthread_mutex_lock( &mutex );
      Logger::error() << e.what() << endl;
      pthread_mutex_unlock( &mutex );
      delete new_socket;
      continue;
    }

    pthread_t tid;
    HandlerWrapperArgs* args = new HandlerWrapperArgs;
    args->obj = this;
    args->socket = new_socket;
    pthread_create( &tid, 0, &Server::client_handler_wrapper, (void*) args );
  }
}

void Server::stop() {
  delete listen_socket;
  Logger::info() << "dmserver stopped\n" << endl;
}

void* Server::client_handler_wrapper(void* args) {
  HandlerWrapperArgs* wargs = (HandlerWrapperArgs*) args;
  wargs->obj->client_handler(wargs->socket);
  delete wargs;
  return 0;
}

void Server::client_handler(SocketServer* socket) {
  pthread_detach( pthread_self() );
  string client_id = get_client_id( *socket );
  Logger::info() << client_id << " connected" << endl;

  string message;
  try {
    while (true) {
      (*socket) >> message;
      if ( message.empty() ) break;
      Message msg(message);

      int block_id;
      string ack = Message::emit(NACK) + Message::STOP;
      map<int,BlockServer>::iterator it;

      switch ( msg.type() ) {
      case MAP:
        block_id = msg.block().id();
        pthread_mutex_lock( &mutex );
        Logger::info() << client_id << " maps block #" << block_id << endl;
        it = blocks.find(block_id);
        if ( it != blocks.end() ) {
          if ( it->second.map(client_id) == 0 ) {
            ack = Message::emit(ACK, it->second) + Message::STOP;
          }
        }
        pthread_mutex_unlock( &mutex );
        break;

      case UNMAP:
        block_id = msg.block().id();
        pthread_mutex_lock( &mutex );
        Logger::info() << client_id << " unmaps block #" << block_id << endl;
        it = blocks.find(block_id);
        if (it != blocks.end()) {
          if ( it->second.unmap(client_id) == 0 ) {
            ack = Message::emit(ACK) + Message::STOP;
          }
          else {
            Logger::error() << "block #" << block_id << " not mapped" << endl;
          }
        }
        pthread_mutex_unlock( &mutex );
        break;

      case UPDATE:
        block_id = msg.block().id();
        pthread_mutex_lock( &mutex );
        Logger::info() << client_id << " updates block #" << block_id << endl;
        it = blocks.find(block_id);
        if (it != blocks.end()) {
          if ( msg.block().revision() < it->second.revision() ) {
            Logger::debug() << "UPDATE sends entire block" << endl;
            ack = Message::emit(ACK, it->second) + Message::STOP;
          }
          else {
            Logger::debug() << "UPDATE sends shallow block" << endl;
            ack = Message::emit(ACK, it->second, true) + Message::STOP;
          }
        }
        pthread_mutex_unlock( &mutex );
        break;

      case WRITE:
        block_id = msg.block().id();
        pthread_mutex_lock( &mutex );
        Logger::info() << client_id << " writes block #" << block_id << endl;
        it = blocks.find(block_id);
        if (it != blocks.end()) {
          Logger::debug() << client_id << " block #" << block_id << " revi " << msg.block().revision() << endl;
          Logger::debug() << client_id << " block #" << block_id << " serv " << it->second.revision() << endl;
          Logger::debug() << client_id << " block #" << block_id << " data " << (char*)msg.block().data() << endl;
          if ( it->second.revision() == msg.block().revision() ) {
            it->second = msg.block();
            it->second.add_revision();
            ack = Message::emit(ACK) + Message::STOP;
          }
        }
        pthread_cond_broadcast( &blocks_wait[block_id] );
        pthread_mutex_unlock( &mutex );
        break;

      case WAIT:
        block_id = msg.block().id();
        ack = Message::emit(ACK) + Message::STOP;
        pthread_mutex_lock( &mutex );
        it = blocks.find(block_id);
        if (it != blocks.end()) {
          while ( it->second.revision() == msg.block().revision() ) {
            Logger::info() << client_id << " waits for block #" << block_id << endl;
            pthread_cond_wait( &blocks_wait[block_id], &mutex );
            Logger::info() << client_id << " stop waiting for block #" << block_id << endl;
          }
        }
        pthread_mutex_unlock( &mutex );
        break;

      default:
        pthread_mutex_lock( &mutex );
        Logger::error() << client_id << " request error: " << message << endl;
        pthread_mutex_unlock( &mutex );
        ack = Message::emit(NACK) + Message::STOP;
        break;
      }

      (*socket) << ack;
    } // end while
  }
  catch (runtime_error e) {
    pthread_mutex_lock( &mutex );
    Logger::error() << e.what() << endl;
    pthread_mutex_unlock( &mutex );
  }

  pthread_mutex_lock( &mutex );
  Logger::info() << client_id << " disconnected" << endl;
  pthread_mutex_unlock( &mutex );
  delete socket;
}

string Server::get_client_id( SocketServer& socket ) const {
  ostringstream client_id;
  client_id << socket.get_peer_ip() << ":" << socket.get_peer_port();
  return client_id.str();
}

}
