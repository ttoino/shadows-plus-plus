#include "shadowsLua.hpp"
#include "globals.hpp"

#include <hyprland/src/config/ConfigValue.hpp>
#include <hyprland/src/config/lua/bindings/LuaBindingsInternal.hpp>
#include <hyprland/src/config/lua/types/LuaConfigBool.hpp>
#include <hyprland/src/config/lua/types/LuaConfigFloat.hpp>
#include <hyprland/src/config/lua/types/LuaConfigGradient.hpp>
#include <hyprland/src/config/lua/types/LuaConfigInt.hpp>
#include <hyprland/src/config/lua/types/LuaConfigVec2.hpp>
#include <hyprland/src/config/shared/complex/ComplexDataTypes.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprutils/utils/ScopeGuard.hpp>

#include <format>
#include <functional>
#include <string>

extern "C" {
#include <lauxlib.h>
#include <lua.h>
}

using namespace Config;

using ShadowsPlusPlus::Lua::shadowKey;

static void setConfigInt(const std::string &name, Config::INTEGER value) {
  CConfigValue<Config::INTEGER> cfg(name);
  *cfg.ptr() = value;
}

static void setConfigFloat(const std::string &name, Config::FLOAT value) {
  CConfigValue<Config::FLOAT> cfg(name);
  *cfg.ptr() = value;
}

static void setConfigBool(const std::string &name, Config::BOOL value) {
  CConfigValue<Config::BOOL> cfg(name);
  *cfg.ptr() = value;
}

static void setConfigGradient(const std::string &name,
                              const Config::CGradientValueData &value) {
  CConfigValue<Config::IComplexConfigValue> cfg(name);
  auto *gradient = dynamic_cast<Config::CGradientValueData *>(cfg.ptr());
  if (gradient)
    *gradient = value;
}

static void setConfigVec2(const std::string &name, const Config::VEC2 &value) {
  CConfigValue<Config::VEC2> cfg(name);
  *cfg.ptr() = value;
}

template <typename Parser, typename T>
static int parseAndSet(lua_State *L, const char *fieldName, Parser &&parser,
                       const std::function<void(const T &)> &setter) {
  lua_getfield(L, -1, fieldName);
  Hyprutils::Utils::CScopeGuard guard([L] { lua_pop(L, 1); });

  if (lua_isnil(L, -1))
    return 0;

  auto err = parser.parse(L);
  if (err.errorCode != Config::Lua::PARSE_ERROR_OK)
    return Config::Lua::Bindings::Internal::configError(
        L, std::format("failed to parse '{}': {}", fieldName, err.message));

  setter(parser.parsed());
  return 0;
}

static int parseShadowTable(lua_State *L, int index) {
  int ret = 0;

  ret = parseAndSet<Lua::CLuaConfigGradient, Config::CGradientValueData>(
      L, "color", Lua::CLuaConfigGradient(CHyprColor{0}),
      [&](const Config::CGradientValueData &v) {
        setConfigGradient(shadowKey(index, "color"), v);
      });
  if (ret != 0)
    return ret;

  ret = parseAndSet<Lua::CLuaConfigVec2, Config::VEC2>(
      L, "offset", Lua::CLuaConfigVec2({0, 0}), [&](const Config::VEC2 &v) {
        setConfigVec2(shadowKey(index, "offset"), v);
      });
  if (ret != 0)
    return ret;

  ret = parseAndSet<Lua::CLuaConfigInt, Config::INTEGER>(
      L, "blur_radius", Lua::CLuaConfigInt(0), [&](const Config::INTEGER &v) {
        setConfigInt(shadowKey(index, "blur_radius"), v);
      });
  if (ret != 0)
    return ret;

  ret = parseAndSet<Lua::CLuaConfigInt, Config::INTEGER>(
      L, "spread_radius", Lua::CLuaConfigInt(0), [&](const Config::INTEGER &v) {
        setConfigInt(shadowKey(index, "spread_radius"), v);
      });
  if (ret != 0)
    return ret;

  ret = parseAndSet<Lua::CLuaConfigBool, Config::BOOL>(
      L, "ignore_window", Lua::CLuaConfigBool(true),
      [&](const Config::BOOL &v) {
        setConfigBool(shadowKey(index, "ignore_window"), v);
      });
  if (ret != 0)
    return ret;

  ret = parseAndSet<Lua::CLuaConfigFloat, Config::FLOAT>(
      L, "scale", Lua::CLuaConfigFloat(1.F), [&](const Config::FLOAT &v) {
        setConfigFloat(shadowKey(index, "scale"), v);
      });
  if (ret != 0)
    return ret;

  ret = parseAndSet<Lua::CLuaConfigBool, Config::BOOL>(
      L, "sharp", Lua::CLuaConfigBool(false), [&](const Config::BOOL &v) {
        setConfigBool(shadowKey(index, "sharp"), v);
      });
  if (ret != 0)
    return ret;

  return 0;
}

static int luaShadowsSetAll(lua_State *L) {
  if (!lua_istable(L, 1))
    return Config::Lua::Bindings::Internal::configError(
        L, "expected a table of shadow definitions");

  const int count = static_cast<int>(lua_rawlen(L, 1));
  if (count < 0 || count > 10)
    return Config::Lua::Bindings::Internal::configError(
        L, "shadow count must be between 0 and 10");

  setConfigInt("plugin:shadows-plus-plus:add_shadows", count);

  for (int i = 1; i <= count; ++i) {
    lua_rawgeti(L, 1, i);
    Hyprutils::Utils::CScopeGuard guard([L] { lua_pop(L, 1); });

    if (!lua_istable(L, -1))
      return Config::Lua::Bindings::Internal::configError(
          L, std::format("shadow {} must be a table", i));

    int ret = parseShadowTable(L, i);
    if (ret != 0)
      return ret;
  }

  return 0;
}

static int luaShadowsSetOne(lua_State *L) {
  if (!lua_isnumber(L, 1))
    return Config::Lua::Bindings::Internal::configError(
        L, "expected shadow index as first argument");

  const int index = static_cast<int>(lua_tointeger(L, 1));
  if (index < 1 || index > 10)
    return Config::Lua::Bindings::Internal::configError(
        L, "shadow index must be between 1 and 10");

  if (!lua_istable(L, 2))
    return Config::Lua::Bindings::Internal::configError(
        L, "expected shadow properties table as second argument");

  lua_pushvalue(L, 2);
  Hyprutils::Utils::CScopeGuard guard([L] { lua_pop(L, 1); });

  return parseShadowTable(L, index);
}

static int luaShadowsSet(lua_State *L) {
  const int argc = lua_gettop(L);

  if (argc == 1)
    return luaShadowsSetAll(L);
  if (argc == 2)
    return luaShadowsSetOne(L);

  return Config::Lua::Bindings::Internal::configError(
      L, "expected either a table of shadows or (index, table)");
}

static int luaShadowsSetCount(lua_State *L) {
  if (!lua_isnumber(L, 1))
    return Config::Lua::Bindings::Internal::configError(L, "expected a number");

  const int count = static_cast<int>(lua_tointeger(L, 1));
  if (count < 0 || count > 10)
    return Config::Lua::Bindings::Internal::configError(
        L, "shadow count must be between 0 and 10");

  setConfigInt("plugin:shadows-plus-plus:add_shadows", count);
  return 0;
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
  HyprlandAPI::addLuaFunction(PHANDLE, "shadows", "set", ::luaShadowsSet);
  HyprlandAPI::addLuaFunction(PHANDLE, "shadows", "set_count",
                              ::luaShadowsSetCount);
  HyprlandAPI::addLuaFunction(PHANDLE, "shadows", "_test_decoration_count",
                              ::luaShadowsTestDecorationCount);
  HyprlandAPI::addLuaFunction(PHANDLE, "shadows", "_test_shadow_count",
                              ::luaShadowsTestShadowCount);
}
