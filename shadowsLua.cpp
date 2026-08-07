#include "shadowsLua.hpp"
#include "globals.hpp"

#include <hyprland/src/plugins/PluginAPI.hpp>

extern "C" {
#include <lua.h>
}

static int luaShadowsTestDecorationCount(lua_State *L) {
  lua_pushinteger(L, static_cast<lua_Integer>(g_pShadowDecorations.size()));
  return 1;
}

static int luaShadowsTestShadowCount(lua_State *L) {
  lua_pushinteger(L, static_cast<lua_Integer>(vars.addShadows->value()));
  return 1;
}

void ShadowsPlusPlus::registerLuaFunctions() {
  HyprlandAPI::addLuaFunction(PHANDLE, "shadows", "_test_decoration_count",
                              ::luaShadowsTestDecorationCount);
  HyprlandAPI::addLuaFunction(PHANDLE, "shadows", "_test_shadow_count",
                              ::luaShadowsTestShadowCount);
}
