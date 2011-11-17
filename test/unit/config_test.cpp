#include <gtest/gtest.h>
#include <dm/config.h>


TEST(DM_Config, is_valid) {
  DM::Config c("");
  EXPECT_EQ( false, c.is_valid() );

  c = DM::Config("path_to_nowhere");
  EXPECT_EQ( false, c.is_valid() );

  c = DM::Config("config_test.yml");
  EXPECT_EQ( true, c.is_valid() );
}

TEST(DM_Config, find_all) {
  const DM::Config c("config_test.yml");
  ASSERT_EQ( true, c.is_valid() );
  EXPECT_EQ( 3U, c.find_all().size() );
}

TEST(DM_Config, find) {
  const DM::Config c("config_test.yml");
  ASSERT_EQ( true, c.is_valid() );
  EXPECT_STREQ( "1.2.3.4", c.find("1.2.3.4:123").ip.c_str() );
  EXPECT_STREQ( "123", c.find("1.2.3.4:123").port.c_str() );
  EXPECT_EQ( 3U, c.find("1.2.3.4:123").blocks_id.size() );
  EXPECT_STREQ( "a shared secret", c.find("1.2.3.4:123").pass.c_str() );
  EXPECT_STREQ( "key.priv", c.find("1.2.3.4:123").privkey.c_str() );
  EXPECT_STREQ( "key.pub", c.find("1.2.3.4:123").pubkey.c_str() );

  EXPECT_EQ( NULL, &c.find("fffffffuuuuuuuuuuuu:f7u12") );
}

TEST(DM_Config, find_server_id_by_block_id) {
  const DM::Config c("config_test.yml");
  ASSERT_EQ( true, c.is_valid() );
  EXPECT_STREQ( "1.2.3.4:123", c.find_server_id_by_block_id(1).c_str() );
  EXPECT_STREQ( "1.2.3.4:456", c.find_server_id_by_block_id(4).c_str() );
  EXPECT_STREQ( "", c.find_server_id_by_block_id(999).c_str() );
}
