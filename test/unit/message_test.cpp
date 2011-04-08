#include <gtest/gtest.h>
#include <message.h>
#include <dm/block_server.h>
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
  EXPECT_EQ( DM::NACK, p.parse("{type: NACK}").type() );
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
  char block_data[128] = "foo bar";
  int block_id = 1;
  int block_rev = 123;
  DM::MessageType msg_type = DM::MAP;

  DM::BlockServer block(block_id);
  block.revision(block_rev);
  block.data(block_data);
  string emitted = DM::Message::emit(msg_type, block);

  DM::Message p(emitted);
  EXPECT_EQ( msg_type, p.type() );
  EXPECT_EQ( block_id, p.block()->id() );
  EXPECT_EQ( block_rev, p.block()->revision() );
  EXPECT_STREQ( block_data, (const char*) p.block()->data() );
}

TEST(DM_Message, emit_block_force_shallow) {
  DM::BlockServer block(1);
  block.data("foo bar");

  EXPECT_STREQ( "{type: MAP, block: {id: 1, revision: 0}}",
                DM::Message::emit(DM::MAP, block, true).c_str() );
}

TEST(DM_Message, emit_block_binary) {
  char block_data[128];
  unsigned int* ptr_data = (unsigned int*) block_data;
  *(ptr_data)    = 65535u;
  *(ptr_data+1)  = 65534u;
  *(ptr_data+2)  = 65533u;

  DM::BlockServer block(1);
  block.data( block_data );

  string emitted = DM::Message::emit(DM::MAP, block);
  DM::Message p(emitted);

  ptr_data = (unsigned int*) p.block()->data();
  ASSERT_EQ( 65535u, *(ptr_data) );
  ASSERT_EQ( 65534u, *(ptr_data+1) );
  ASSERT_EQ( 65533u, *(ptr_data+2) );

  for (int i = 0; i < 128; ++i) {
    ASSERT_EQ( block_data[i], ((char*)p.block()->data())[i] );
  }
}
