#ifndef _DM_MESSAGE_H_
#define _DM_MESSAGE_H_

#include "type.h"
#include <yaml.h>


namespace DM {

class Message {
public:
  Message(string message = "");
  Message& parse(string message);

  MessageType type();
  int block();

  static string emit(MessageType);

private:
  YAML::Node node;
  // Static members to save memory
  static map<string,MessageType> string_to_type;
  static map<MessageType,string> type_to_string;
  static void maps_initializer();
  static bool maps_init;
};

} // namespace DM

#endif /* _DM_MESSAGE_H_ */
