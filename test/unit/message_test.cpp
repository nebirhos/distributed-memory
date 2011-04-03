#include <gtest/gtest.h>
#include <message.h>
#include <block_server.h>
#include <base64.h>


// Parsing
TEST(DM_Message, parse_unknown_keys) {
  DM::Message p;
  EXPECT_EQ( DM::UNDEF, p.parse("{type: map}").type() );
  EXPECT_EQ( DM::UNDEF, p.parse("{type: FOO}").type() );
  EXPECT_EQ( DM::UNDEF, p.parse("{foo:  MAP}").type() );
}

TEST(DM_Message, parse_type) {
  DM::Message p;
  EXPECT_EQ( DM::MAP, p.parse("{type: MAP}").type() );
  EXPECT_EQ( DM::UNMAP, p.parse("{type: UNMAP}").type() );
  EXPECT_EQ( DM::UPDATE, p.parse("{type: UPDATE}").type() );
  EXPECT_EQ( DM::WRITE, p.parse("{type: WRITE}").type() );
  EXPECT_EQ( DM::WAIT, p.parse("{type: WAIT}").type() );
  EXPECT_EQ( DM::ACK, p.parse("{type: ACK}").type() );
}

// Emitting
TEST(DM_Message, emit) {
  EXPECT_STREQ( "{type: ACK}", DM::Message::emit(DM::ACK).c_str() );
}

TEST(DM_Message, emit_and_parse) {
  const char block_data[128] = "foo bar";
  const int block_id = 123;
  const DM::MessageType msg_type = DM::MAP;
  DM::BlockServer block(block_id);
  block.data(block_data);
  string emitted = DM::Message::emit(msg_type, block);

  DM::Message p(emitted);
  EXPECT_EQ( msg_type, p.type() );
  EXPECT_EQ( block_id, p.block()->id() );
  EXPECT_STREQ( block_data, (const char*) p.block()->data() );
}
