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

  for (size_t i = 0; i < 10; ++i) {
    const std::string base =
        "plugin:shadows-plus-plus:shadow_" + std::to_string(i + 1);
    vars.shadowColors[i] = makeShared<Config::Values::CColorValue>(
        (base + ":color").c_str(), "Color of the shadow", 0xee1a1a1a);
    vars.shadowOffsets[i] = makeShared<Config::Values::CVec2Value>(
        (base + ":offset").c_str(), "Offset of the shadow", Config::VEC2{0, 0});
    vars.shadowBlurRadii[i] = makeShared<Config::Values::CIntValue>(
        (base + ":blur_radius").c_str(), "Blur radius of the shadow", 3);
    vars.shadowSpreadRadii[i] = makeShared<Config::Values::CIntValue>(
        (base + ":spread_radius").c_str(), "Spread radius of the shadow", 4);
    vars.shadowIgnoreWindows[i] = makeShared<Config::Values::CBoolValue>(
        (base + ":ignore_window").c_str(),
        "Whether the shadow ignores the window", true);
    vars.shadowScales[i] = makeShared<Config::Values::CFloatValue>(
        (base + ":scale").c_str(), "Scale of the shadow", 1.f);

    HyprlandAPI::addConfigValueV2(PHANDLE, vars.shadowColors[i]);
    HyprlandAPI::addConfigValueV2(PHANDLE, vars.shadowOffsets[i]);
    HyprlandAPI::addConfigValueV2(PHANDLE, vars.shadowBlurRadii[i]);
    HyprlandAPI::addConfigValueV2(PHANDLE, vars.shadowSpreadRadii[i]);
    HyprlandAPI::addConfigValueV2(PHANDLE, vars.shadowIgnoreWindows[i]);
    HyprlandAPI::addConfigValueV2(PHANDLE, vars.shadowScales[i]);
  }

  HyprlandAPI::reloadConfig();

  ShadowsPlusPlus::registerLuaFunctions();

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
