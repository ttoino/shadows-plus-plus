#pragma once

#include <string>

namespace ShadowsPlusPlus {
void registerLuaFunctions();

namespace Lua {
inline std::string shadowKey(int index, const std::string &prop) {
  return "plugin:shadows-plus-plus:shadow_" + std::to_string(index) + ":" +
         prop;
}
} // namespace Lua
} // namespace ShadowsPlusPlus
