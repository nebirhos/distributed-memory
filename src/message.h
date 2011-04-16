/**
 * @file message.h
 *
 * @author Francesco Disperati
 *
 * Released under GNU GPLv3 (http://www.gnu.org/licenses/gpl-3.0.txt)
 */

#ifndef _DM_MESSAGE_H_
#define _DM_MESSAGE_H_

#include <dm/type.h>
#include <dm/block_server.h>
#include <yaml-cpp/yaml.h>


namespace DM {

/**
 * Parses and generates messages for client-server communication.
 *
 * Messages are strings formatted using JSON syntax. This implies
 * a certain overhead compared to pure binary data transfers, but
 * there are enormous advantages:
 *   - raw binary data structures are fragile and not compatible
 *     with different architectures
 *   - code can be refactored quicker and safer
 *   - debugging protocol is far more easier
 *
 * Between other string-based syntax, JSON has been chosen cause:
 *   - faster to read (for both human and machine) than XML
 *   - already included as a subset of YAML syntax
 *
 * Maybe Protocol Buffers is faster, but has more coding overhead.
 *
 * Messages have basically two forms:
 *   -# only @ref DM::MessageType "type". Eg:
@verbatim {type: MAP} @endverbatim
 *   -# type and Block informations. For this one, block can also be
 *      <em>shallow</em> if no data content is needed, saving bandwidth.
 *      Eg:
@verbatim {type: MAP, block: {id: 1, revision: 0}} @endverbatim
 *
 * references:
 *   1. http://www.json.org/
 *   2. http://www.yaml.org/
 *   3. http://code.google.com/p/protobuf/
 */
class Message {
public:
  /** Create Message and parse if a string is given */
  Message(string message = "");
  ~Message();

  /**
   * Parse the given string.
   *
   * @param message string to parse
   * @return reference to self
   */
  Message& parse(string message);

  /**
   * Get message type.
   *
   * @return type of message or DM::UNDEF if invalid
   */
  MessageType type();

  /**
   * Parse block informations in message.
   *
   * @return reference to BlockServer
   */
  const BlockServer& block();

  /** Token to mark end of message */
  static const string STOP;

  /**
   * Builds a message of the first form.
   *
   * @param type message type
   * @return formatted string
   */
  static string emit(MessageType);

  /**
   * Builds a message of the second form.
   *
   * @param type message type
   * @param block reference to the block to serialize
   * @param shallow true if block shouldn't have data content
   * @return formatted string
   */
  static string emit(MessageType, const Block&, bool shallow = false);

private:
  /** YAML library internal structure for parsing */
  YAML::Node node;
  /** Keeps parsed block for fast access */
  BlockServer* m_block;

  /** MessageType lookup map */
  static map<string,MessageType> string_to_type;
  /** MessageType lookup map */
  static map<MessageType,string> type_to_string;
  /** Instantiate static maps */
  static void maps_initializer();
};

} // namespace DM

#endif /* _DM_MESSAGE_H_ */
