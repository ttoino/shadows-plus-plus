#pragma once

#include <hyprutils/math/Box.hpp>

using CBox = Hyprutils::Math::CBox;

namespace ShadowsPlusPlus::Geometry {

struct SScissorStrips {
  CBox top;
  CBox bottom;
  CBox left;
  CBox right;
};

SScissorStrips computeShadowClippingStrips(const CBox &shadowBox,
                                           const CBox &windowBox);

} // namespace ShadowsPlusPlus::Geometry
