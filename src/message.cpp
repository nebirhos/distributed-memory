#include "message.h"
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

Message::Message(string message) {
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
  }
  catch(YAML::Exception& e) {
    cout << e.what() << endl;
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

int Message::block() {
  try {
    return node["block"];
  }
  catch(YAML::Exception& e) {
    return -1;
  }
}

string Message::emit(MessageType type) {
  YAML::Emitter emt;
  emt << YAML::Flow;            // short YAML syntax - aka JSON
  emt << YAML::BeginMap;
  emt << YAML::Key << "type";
  emt << YAML::Value << type_to_string[type];
  emt << YAML::EndMap;
  string result(emt.c_str());
  return result;
}

} // namespace DM
