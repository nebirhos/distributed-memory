#include <gtest/gtest.h>
#include <block_server.h>
#include <cstring>

TEST(DM_BlockServer, constructor) {
  DM::BlockServer b(123);
  EXPECT_EQ( 123, b.id() );
  EXPECT_EQ( 0, b.revision() );
  EXPECT_NE( NULL, (int) b.data() );

  DM::BlockServer b2(456);
  EXPECT_NE( b.data(), b2.data() );
}

TEST(DM_BlockServer, copy) {
  DM::BlockServer b1(123);
  DM::BlockServer b2(b1);

  EXPECT_EQ( b1.id(), b2.id() );
  EXPECT_EQ( b1.revision(), b2.revision() );
  EXPECT_NE( b1.data(), b2.data() );
  for (int i = 0; i < b1.size(); ++i) {
    ASSERT_EQ( ((unsigned char*) b1.data())[i],
               ((unsigned char*) b2.data())[i] );
  }
}

TEST(DM_BlockServer, setter) {
  DM::BlockServer b(123);
  b.revision(10);
  unsigned char* data = new unsigned char[b.size()];
  memset(data, 0xFF, b.size());
  b.data(data);

  EXPECT_EQ( 10, b.revision() );
  for (int i = 0; i < b.size(); ++i) {
    ASSERT_EQ( 0xFF, ((unsigned char*) b.data())[i] );
  }
  delete[] data;
}

TEST(DM_BlockServer, map) {
  DM::BlockServer b;
  string client_id = "1.2.3.4:5678";
  EXPECT_EQ( 0, b.map(client_id) );
  EXPECT_NE( 0, b.map(client_id) );
  EXPECT_EQ( true , b.is_client_mapped(client_id) );
  EXPECT_NE( true , b.is_client_mapped("foo:bar") );
}

TEST(DM_BlockServer, unmap) {
  DM::BlockServer b;
  string client_id1 = "1.2.3.4:5678";
  string client_id2 = "5.6.7.8:1234";
  b.map(client_id1);
  b.map(client_id2);
  EXPECT_EQ( 0, b.unmap(client_id1) );
  EXPECT_NE( 0, b.unmap(client_id1) );
  EXPECT_EQ( true , b.is_client_mapped(client_id2) );
  EXPECT_NE( true , b.is_client_mapped(client_id1) );
}
