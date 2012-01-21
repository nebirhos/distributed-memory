#include <gtest/gtest.h>
#include <message.h>
#include <dm/block_server.h>
#include <cstring>


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
  EXPECT_EQ( DM::NACK, p.parse("{type: NACK}").type() );
}

TEST(DM_Message, parse_notvalid) {
  DM::Message p("invalid JSON syntax");
  EXPECT_EQ( DM::UNDEF, p.type() );
  EXPECT_EQ( NULL, &p.block() );
}

// Emitting
TEST(DM_Message, emit) {
  EXPECT_STREQ( "{type: ACK}", DM::Message::emit(DM::ACK).c_str() );
}

TEST(DM_Message, emit_shallow_block) {
  DM::Block block(1);
  block.revision(123);
  EXPECT_STREQ( "{type: MAP, block: {id: 1, revision: 123}}",
                DM::Message::emit(DM::MAP, block).c_str() );
}

TEST(DM_Message, emit_block) {
  char block_data[DIMBLOCK];
  memset(block_data, 'A', DIMBLOCK);
  block_data[DIMBLOCK-1] = 0;
  int block_id = 1;
  int block_rev = 123;
  DM::MessageType msg_type = DM::MAP;

  DM::BlockServer block(block_id);
  block.revision(block_rev);
  block.data(block_data);
  string emitted = DM::Message::emit(msg_type, block);

  DM::Message p(emitted);
  EXPECT_EQ( msg_type, p.type() );
  EXPECT_EQ( block_id, p.block().id() );
  EXPECT_EQ( block_rev, p.block().revision() );
  EXPECT_STREQ( block_data, (const char*) p.block().data() );
}

TEST(DM_Message, emit_block_force_shallow) {
  DM::BlockServer block(1);

  EXPECT_STRNE( "{type: MAP, block: {id: 1, revision: 0}}",
                DM::Message::emit(DM::MAP, block, false).c_str() );
  EXPECT_STREQ( "{type: MAP, block: {id: 1, revision: 0}}",
                DM::Message::emit(DM::MAP, block, true).c_str() );
}

TEST(DM_Message, emit_block_binary) {
  char block_data[DIMBLOCK];
  DM::BlockServer block(1);
  block.data( block_data );

  string emitted = DM::Message::emit(DM::MAP, block);
  DM::Message p(emitted);

  for (int i = 0; i < DIMBLOCK; ++i) {
    ASSERT_EQ( block_data[i], ((char*)p.block().data())[i] );
  }
}
