#include "message.h"
#include "base64.h"
#include <map>
#include <sstream>


namespace DM {

map<string,MessageType> Message::string_to_type;
map<MessageType,string> Message::type_to_string;

void Message::maps_initializer() {
  Message::string_to_type["MAP"]    = MAP;
  Message::string_to_type["UNMAP"]  = UNMAP;
  Message::string_to_type["UPDATE"] = UPDATE;
  Message::string_to_type["WRITE"]  = WRITE;
  Message::string_to_type["WAIT"]   = WAIT;
  Message::string_to_type["ACK"]    = ACK;
  Message::string_to_type["UNDEF"]  = UNDEF;

  Message::type_to_string[MAP]    = "MAP";
  Message::type_to_string[UNMAP]  = "UNMAP";
  Message::type_to_string[UPDATE] = "UPDATE";
  Message::type_to_string[WRITE]  = "WRITE";
  Message::type_to_string[WAIT]   = "WAIT";
  Message::type_to_string[ACK]    = "ACK";
  Message::type_to_string[UNDEF]  = "UNDEF";
}

Message::Message(string message)
  : m_block(NULL) {
  static bool maps_init = false;
  if (!maps_init) {
    maps_initializer();
    maps_init = true;
  }
  if (!message.empty())
    parse(message);
}

Message& Message::parse(string message) {
  try {
    istringstream ss(message);
    YAML::Parser parser(ss);
    parser.GetNextDocument(node);
    delete m_block;             // maybe there is already a block instatiated
    m_block = NULL;
  }
  catch(YAML::Exception& e) {
    cout << e.what() << endl;   // FIXME
  }
  return *this;
}

MessageType Message::type() {
  try {
    string type;
    MessageType result = Message::string_to_type[ node["type"] ];
    if (result == 0)
      result = UNDEF;
    return result;
  }
  catch(YAML::Exception& e) {
    return UNDEF;
  }
}

const Block* Message::block() {
  try {
    if (m_block == NULL) {
      int block_id = node["block"]["id"];
      m_block = new BlockServer( block_id );
      m_block->revision( node["block"]["revision"] );

      string block_data = node["block"]["data"];
      char* buffer = new char[m_block->size()];
      base64::decode( block_data.c_str(), block_data.size(), buffer, m_block->size()+1 );
      m_block->data( buffer );
      delete[] buffer;
    }
  }
  catch(YAML::Exception& e) {
    cout << "FIXME Message::block" << endl;
    cout << e.what() << endl;
  }
  return m_block;
}

string Message::emit(MessageType type) {
  YAML::Emitter emt;
  emt << YAML::Flow;            // short YAML syntax - aka JSON
  emt << YAML::BeginMap;
  emt << YAML::Key << "type" << YAML::Value << type_to_string[type];
  emt << YAML::EndMap;
  return emt.c_str();
}

string Message::emit(MessageType type, const Block& block) {
  YAML::Emitter emt;
  emt << YAML::Flow;
  emt << YAML::BeginMap;
  emt << YAML::Key << "type" << YAML::Value << type_to_string[type];
  emt << YAML::Key << "block" << YAML::Value << block;
  emt << YAML::EndMap;
  return emt.c_str();
}

YAML::Emitter& operator<<(YAML::Emitter& out, const Block& b) {
  out << YAML::Flow;
  out << YAML::BeginMap;
  out << YAML::Key << "id" << YAML::Value << b.id();
  out << YAML::Key << "revision" << YAML::Value << b.revision();
  out << YAML::Key << "data" << YAML::Value << YAML::Binary((char*) b.data(), b.size());
  out << YAML::EndMap;
  return out;
}

} // namespace DM
