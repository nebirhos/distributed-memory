#ifndef _DM_MESSAGE_H_
#define _DM_MESSAGE_H_

#include <dm/type.h>
#include <dm/block_server.h>
#include <yaml-cpp/yaml.h>


namespace DM {

class Message {
public:
  Message(string message = "");
  ~Message();
  Message& parse(string message);

  MessageType type();
  const BlockServer& block();

  static const string STOP;     // token to mark end of message
  static string emit(MessageType);
  static string emit(MessageType, const Block&, bool shallow = false);

private:
  YAML::Node node;
  BlockServer* m_block;

  static map<string,MessageType> string_to_type;
  static map<MessageType,string> type_to_string;
  static void maps_initializer();
};

} // namespace DM

#endif /* _DM_MESSAGE_H_ */
