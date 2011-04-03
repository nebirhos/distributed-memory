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
