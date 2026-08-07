#pragma once

#include <array>
#include <hyprland/src/config/values/types/BoolValue.hpp>
#include <hyprland/src/config/values/types/ColorValue.hpp>
#include <hyprland/src/config/values/types/FloatValue.hpp>
#include <hyprland/src/config/values/types/GradientValue.hpp>
#include <hyprland/src/config/values/types/IntValue.hpp>
#include <hyprland/src/config/values/types/Vec2Value.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <set>

inline HANDLE PHANDLE = nullptr;

class CBoxShadowsDecoration;
inline std::set<CBoxShadowsDecoration *> g_pShadowDecorations;

struct SVars {
  SP<Config::Values::CIntValue> addShadows;
  std::array<SP<Config::Values::CGradientValue>, 10> shadowColors;
  std::array<SP<Config::Values::CVec2Value>, 10> shadowOffsets;
  std::array<SP<Config::Values::CIntValue>, 10> shadowBlurRadii;
  std::array<SP<Config::Values::CIntValue>, 10> shadowSpreadRadii;
  std::array<SP<Config::Values::CBoolValue>, 10> shadowIgnoreWindows;
  std::array<SP<Config::Values::CFloatValue>, 10> shadowScales;
  std::array<SP<Config::Values::CBoolValue>, 10> shadowSharps;
};

inline SVars vars = {};
