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
