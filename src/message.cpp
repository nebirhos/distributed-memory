/**
 * @author Francesco Disperati
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#include "message.h"
#include "base64.h"
#include <map>
#include <sstream>


namespace DM {

const string Message::STOP = "\r\n\r\n";

map<string,MessageType> Message::string_to_type;
map<MessageType,string> Message::type_to_string;

void Message::maps_initializer() {
  if (string_to_type.size() == 0) {
    Message::string_to_type["MAP"]    = MAP;
    Message::string_to_type["UNMAP"]  = UNMAP;
    Message::string_to_type["UPDATE"] = UPDATE;
    Message::string_to_type["WRITE"]  = WRITE;
    Message::string_to_type["WAIT"]   = WAIT;
    Message::string_to_type["ACK"]    = ACK;
    Message::string_to_type["NACK"]   = NACK;
    Message::string_to_type["UNDEF"]  = UNDEF;
  }
  if (type_to_string.size() == 0) {
    Message::type_to_string[MAP]    = "MAP";
    Message::type_to_string[UNMAP]  = "UNMAP";
    Message::type_to_string[UPDATE] = "UPDATE";
    Message::type_to_string[WRITE]  = "WRITE";
    Message::type_to_string[WAIT]   = "WAIT";
    Message::type_to_string[ACK]    = "ACK";
    Message::type_to_string[NACK]   = "NACK";
    Message::type_to_string[UNDEF]  = "UNDEF";
  }
}

Message::Message(string message)
  : m_block(NULL) {
  maps_initializer();
  if (!message.empty())
    parse(message);
}

Message::~Message() {
  delete m_block;
}

Message& Message::parse(string message) {
  try {
    istringstream ss(message);
    YAML::Parser parser(ss);
    parser.GetNextDocument(node);
    delete m_block;             // maybe there's already a block instatiated
    m_block = NULL;
  }
  catch(YAML::Exception& e) {}  // FIXME
  return *this;
}

MessageType Message::type() {
  try {
    string type;
    node["type"] >> type;
    return Message::string_to_type[type];
  }
  catch(YAML::Exception& e) {
    return UNDEF;
  }
}

const BlockServer& Message::block() {
  try {
    if (m_block == NULL) {
      int block_id;
      node["block"]["id"] >> block_id;
      m_block = new BlockServer( block_id );

      // revision key optional
      if ( const YAML::Node* nrev = node["block"].FindValue("revision") ) {
        int revision;
        *nrev >> revision;
        m_block->revision( revision );
      }
      // data key optional
      if ( const YAML::Node* ndata = node["block"].FindValue("data") ) {
        string block_data;
        *ndata >> block_data;
        char* buffer = new char[m_block->size()+1]; // size+1 due to a bug in base64 decode
        base64::decode( block_data.c_str(), block_data.size(), buffer, m_block->size()+1 );
        m_block->data( buffer );
        delete[] buffer;
      }
    }
  }
  catch(YAML::Exception& e) {}  // FIXME
  return *m_block;
}

string Message::emit(MessageType type) {
  maps_initializer();
  YAML::Emitter emt;
  emt << YAML::Flow;            // short YAML syntax - aka JSON
  emt << YAML::BeginMap;
  emt << YAML::Key << "type" << YAML::Value << type_to_string[type];
  emt << YAML::EndMap;
  return emt.c_str();
}

string Message::emit(MessageType type, const Block& block, bool shallow) {
  maps_initializer();
  YAML::Emitter emt;
  emt << YAML::Flow;
  emt << YAML::BeginMap;
  emt << YAML::Key << "type" << YAML::Value << type_to_string[type];
  emt << YAML::Key << "block" << YAML::Value;

  emt << YAML::BeginMap;
  emt << YAML::Key << "id" << YAML::Value << block.id();
  emt << YAML::Key << "revision" << YAML::Value << block.revision();
  // not shallow block
  if ( !shallow && block.data() != NULL ) {
    emt << YAML::Key << "data" << YAML::Value << YAML::Binary((unsigned char*) block.data(), block.size());
  }
  emt << YAML::EndMap;

  emt << YAML::EndMap;
  return emt.c_str();
}

} // namespace DM
