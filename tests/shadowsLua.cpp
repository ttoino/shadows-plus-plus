#include "shadowsLua.hpp"

#include <gtest/gtest.h>

TEST(ShadowsLua, shadowKeyFormatsSingleDigitIndex) {
  EXPECT_EQ(ShadowsPlusPlus::Lua::shadowKey(1, "color"),
            "plugin:shadows-plus-plus:shadow_1:color");
}

TEST(ShadowsLua, shadowKeyFormatsDoubleDigitIndex) {
  EXPECT_EQ(ShadowsPlusPlus::Lua::shadowKey(10, "blur_radius"),
            "plugin:shadows-plus-plus:shadow_10:blur_radius");
}

TEST(ShadowsLua, shadowKeyFormatsArbitraryProperty) {
  EXPECT_EQ(ShadowsPlusPlus::Lua::shadowKey(3, "ignore_window"),
            "plugin:shadows-plus-plus:shadow_3:ignore_window");
}
