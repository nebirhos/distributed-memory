#include <gtest/gtest.h>
#include <dm/config.h>


TEST(DM_Config, parse) {
  const DM::Config c("config_test.yml");
  EXPECT_EQ( 3U, c.find_all().size() );
}

TEST(DM_Config, find) {
  const DM::Config c("config_test.yml");
  EXPECT_STREQ( "1.2.3.4", c.find("1.2.3.4:123").ip.c_str() );
  EXPECT_STREQ( "123", c.find("1.2.3.4:123").port.c_str() );
  EXPECT_EQ( 3U, c.find("1.2.3.4:123").blocks_id.size() );
}

TEST(DM_Config, find_server_id_by_block_id) {
  const DM::Config c("config_test.yml");
  EXPECT_STREQ( "1.2.3.4:123", c.find_server_id_by_block_id(1).c_str() );
  EXPECT_STREQ( "1.2.3.4:456", c.find_server_id_by_block_id(4).c_str() );
  EXPECT_STREQ( "", c.find_server_id_by_block_id(999).c_str() );
}
