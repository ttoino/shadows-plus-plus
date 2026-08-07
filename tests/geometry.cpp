#include "geometry.hpp"

#include <gtest/gtest.h>

using namespace ShadowsPlusPlus::Geometry;

TEST(Geometry, computeShadowClippingStripsSurroundingWindow) {
  const CBox shadow = {0, 0, 100, 100};
  const CBox window = {30, 30, 40, 40};

  const auto strips = computeShadowClippingStrips(shadow, window);

  EXPECT_EQ(strips.top.x, 0);
  EXPECT_EQ(strips.top.y, 0);
  EXPECT_EQ(strips.top.width, 100);
  EXPECT_EQ(strips.top.height, 30);

  EXPECT_EQ(strips.bottom.x, 0);
  EXPECT_EQ(strips.bottom.y, 70);
  EXPECT_EQ(strips.bottom.width, 100);
  EXPECT_EQ(strips.bottom.height, 30);

  EXPECT_EQ(strips.left.x, 0);
  EXPECT_EQ(strips.left.y, 30);
  EXPECT_EQ(strips.left.width, 30);
  EXPECT_EQ(strips.left.height, 40);

  EXPECT_EQ(strips.right.x, 70);
  EXPECT_EQ(strips.right.y, 30);
  EXPECT_EQ(strips.right.width, 30);
  EXPECT_EQ(strips.right.height, 40);
}

TEST(Geometry, computeShadowClippingStripsWindowAtOrigin) {
  const CBox shadow = {0, 0, 50, 50};
  const CBox window = {0, 0, 20, 20};

  const auto strips = computeShadowClippingStrips(shadow, window);

  EXPECT_EQ(strips.top.height, 0);
  EXPECT_EQ(strips.left.width, 0);
  EXPECT_EQ(strips.bottom.y, 20);
  EXPECT_EQ(strips.bottom.height, 30);
  EXPECT_EQ(strips.right.x, 20);
  EXPECT_EQ(strips.right.width, 30);
}
