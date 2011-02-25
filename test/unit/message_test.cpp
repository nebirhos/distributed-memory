#include <gtest/gtest.h>
#include <message.h>

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

TEST(DM_Message, parse_block) {
  DM::Message p;
  EXPECT_EQ( 123, p.parse("{type: MAP, block: 123}").block() );
  EXPECT_EQ( -1, p.parse("{block: FOO}").block() );
}

// Emitting
TEST(DM_Message, emit) {
  EXPECT_STREQ( "--- {type: ACK}", DM::Message::emit(DM::ACK).c_str() );
}

// Combo
TEST(DM_Message, emit_and_parse) {
  string emitted = DM::Message::emit(DM::UPDATE);
  DM::Message p(emitted);
  EXPECT_EQ( DM::UPDATE, p.type() );
}
