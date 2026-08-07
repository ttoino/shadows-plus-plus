#include "geometry.hpp"

namespace ShadowsPlusPlus::Geometry {

SScissorStrips computeShadowClippingStrips(const CBox &shadowBox,
                                           const CBox &windowBox) {
  return {
      .top = {shadowBox.x, shadowBox.y, shadowBox.width,
              windowBox.y - shadowBox.y},
      .bottom = {shadowBox.x, windowBox.y + windowBox.height, shadowBox.width,
                 shadowBox.y + shadowBox.height - windowBox.y -
                     windowBox.height},
      .left = {shadowBox.x, windowBox.y, windowBox.x - shadowBox.x,
               windowBox.height},
      .right = {windowBox.x + windowBox.width, windowBox.y,
                shadowBox.x + shadowBox.width - windowBox.x - windowBox.width,
                windowBox.height},
  };
}

} // namespace ShadowsPlusPlus::Geometry
