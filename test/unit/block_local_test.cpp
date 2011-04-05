#include <gtest/gtest.h>
#include <dm/block_local.h>

TEST(DM_BlockLocal, constructor) {
  DM::BlockLocal b(123);
  EXPECT_EQ( 123, b.id() );
  EXPECT_EQ( 0, b.revision() );
  EXPECT_EQ( NULL, (int) b.data() );
}

TEST(DM_BlockLocal, setter) {
  DM::BlockLocal b(10);
  b.revision(20);
  b.data(&b);
  EXPECT_EQ( 10, b.id() );
  EXPECT_EQ( 20, b.revision() );
  EXPECT_EQ( (void*) &b, (void*) b.data() );
}

TEST(DM_BlockLocal, assignment) {
  DM::BlockLocal b1(1);
  DM::BlockLocal b2(2);
  unsigned char* data1 = new unsigned char[b1.size()];
  memset(data1, 0xFF, b1.size());
  b1.data(data1);
  unsigned char* data2 = new unsigned char[b2.size()];
  memset(data2, 0x00, b2.size());
  b2.data(data2);

  b2 = b1;
  EXPECT_EQ( b1.id(), b2.id() );
  EXPECT_EQ( b1.revision(), b2.revision() );
  EXPECT_NE( b1.data(), b2.data() );
  for (int i = 0; i < b1.size(); ++i) {
    ASSERT_EQ( ((unsigned char*) b1.data())[i],
               ((unsigned char*) b2.data())[i] );
  }
  delete[] data1;
  delete[] data2;
}

TEST(DM_BlockLocal, valid) {
  DM::BlockLocal b(123);
  EXPECT_EQ( true, b.valid() );
  b.valid(false);
  EXPECT_EQ( false, b.valid() );
}

TEST(DM_BlockLocal, map) {
  DM::BlockLocal b(123);
  EXPECT_EQ( 0, b.map("foo") );
  EXPECT_NE( 0, b.map("foo again") );
}
