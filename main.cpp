#include <unistd.h>

#include <array>
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/config/ConfigManager.hpp>
#include <hyprland/src/desktop/state/WindowState.hpp>
#include <hyprland/src/desktop/view/Window.hpp>
#include <hyprland/src/event/EventBus.hpp>
#include <hyprland/src/render/Renderer.hpp>

#include "CBoxShadowsDecoration.hpp"
#include "globals.hpp"
#include "shadowsLua.hpp"

// Do NOT change this function.
APICALL EXPORT std::string PLUGIN_API_VERSION() { return HYPRLAND_API_VERSION; }

static void onNewWindow(PHLWINDOW window) {
  if (std::ranges::any_of(window->m_windowDecorations, [](const auto &d) {
        return d->getDisplayName() == "Box Shadows";
      }))
    return;

  HyprlandAPI::addWindowDecoration(PHANDLE, window,
                                   makeUnique<CBoxShadowsDecoration>(window));
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
  PHANDLE = handle;

  const std::string HASH = __hyprland_api_get_hash();
  const std::string CLIENT_HASH = __hyprland_api_get_client_hash();

  if (HASH != CLIENT_HASH) {
    HyprlandAPI::addNotification(
        PHANDLE,
        "[shadows-plus-plus] Failure in initialization: Version mismatch "
        "(headers ver is not equal to running hyprland ver)",
        CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
    throw std::runtime_error("[bpp] Version mismatch");
  }

  vars.addShadows = makeShared<Config::Values::CIntValue>(
      "plugin:shadows-plus-plus:add_shadows", "How many extra shadows to draw",
      1, Config::Values::SIntValueOptions{.min = 0, .max = 10});
  HyprlandAPI::addConfigValueV2(PHANDLE, vars.addShadows);

  static std::array<std::string, 10> shadowColorNames;
  static std::array<std::string, 10> shadowOffsetNames;
  static std::array<std::string, 10> shadowBlurRadiusNames;
  static std::array<std::string, 10> shadowSpreadRadiusNames;
  static std::array<std::string, 10> shadowScaleNames;
  static std::array<std::string, 10> shadowSharpNames;

  for (size_t i = 0; i < 10; ++i) {
    const std::string base =
        "plugin:shadows-plus-plus:shadow_" + std::to_string(i + 1);

    shadowColorNames[i] = base + ":color";
    shadowOffsetNames[i] = base + ":offset";
    shadowBlurRadiusNames[i] = base + ":blur_radius";
    shadowSpreadRadiusNames[i] = base + ":spread_radius";
    shadowScaleNames[i] = base + ":scale";
    shadowSharpNames[i] = base + ":sharp";

    vars.shadowColors[i] = makeShared<Config::Values::CGradientValue>(
        shadowColorNames[i].c_str(), "Color of the shadow",
        CHyprColor{0xee1a1a1a});
    vars.shadowOffsets[i] = makeShared<Config::Values::CVec2Value>(
        shadowOffsetNames[i].c_str(), "Offset of the shadow",
        Config::VEC2{0, 0});
    vars.shadowBlurRadii[i] = makeShared<Config::Values::CIntValue>(
        shadowBlurRadiusNames[i].c_str(), "Blur radius of the shadow", 3);
    vars.shadowSpreadRadii[i] = makeShared<Config::Values::CIntValue>(
        shadowSpreadRadiusNames[i].c_str(), "Spread radius of the shadow", 4);
    vars.shadowScales[i] = makeShared<Config::Values::CFloatValue>(
        shadowScaleNames[i].c_str(), "Scale of the shadow", 1.f);
    vars.shadowSharps[i] = makeShared<Config::Values::CBoolValue>(
        shadowSharpNames[i].c_str(), "Whether the shadow is sharp", false);

    HyprlandAPI::addConfigValueV2(PHANDLE, vars.shadowColors[i]);
    HyprlandAPI::addConfigValueV2(PHANDLE, vars.shadowOffsets[i]);
    HyprlandAPI::addConfigValueV2(PHANDLE, vars.shadowBlurRadii[i]);
    HyprlandAPI::addConfigValueV2(PHANDLE, vars.shadowSpreadRadii[i]);
    HyprlandAPI::addConfigValueV2(PHANDLE, vars.shadowScales[i]);
    HyprlandAPI::addConfigValueV2(PHANDLE, vars.shadowSharps[i]);
  }

  ShadowsPlusPlus::registerLuaFunctions();

  HyprlandAPI::reloadConfig();

  static auto P = Event::bus()->m_events.window.open.listen(
      [&](PHLWINDOW window) { onNewWindow(window); });

  // add deco to existing windows
  for (auto &w : Desktop::windowState()->windows()) {
    if (w->isHidden() || !w->m_isMapped)
      continue;

    HyprlandAPI::addWindowDecoration(PHANDLE, w,
                                     makeUnique<CBoxShadowsDecoration>(w));
  }

  HyprlandAPI::addNotification(PHANDLE,
                               "[shadows-plus-plus] Initialized successfully!",
                               CHyprColor{0.2, 1.0, 0.2, 1.0}, 5000);

  return {"shadows-plus-plus", "A plugin to add CSS's box shadows to windows.",
          "toino", "1.0"};
}

APICALL EXPORT void PLUGIN_EXIT() {
  g_pHyprRenderer->m_renderPass.removeAllOfType("CBoxShadowsPassElement");
}
