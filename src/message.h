#ifndef _DM_MESSAGE_H_
#define _DM_MESSAGE_H_

#include <dm/type.h>
#include "block_server.h"
#include <yaml-cpp/yaml.h>


namespace DM {

class Message {
public:
  Message(string message = "");
  Message& parse(string message);

  MessageType type();
  const Block* block();

  static string emit(MessageType);
  static string emit(MessageType, const Block&);

private:
  YAML::Node node;
  Block* m_block;
  // Static members to save memory
  static map<string,MessageType> string_to_type;
  static map<MessageType,string> type_to_string;
  static void maps_initializer();
  static bool maps_init;
};

YAML::Emitter& operator<<(YAML::Emitter& out, const Block& b);

} // namespace DM

#endif /* _DM_MESSAGE_H_ */
