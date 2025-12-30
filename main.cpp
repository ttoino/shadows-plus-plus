#include <unistd.h>

#include <any>
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/config/ConfigManager.hpp>
#include <hyprland/src/desktop/view/Window.hpp>
#include <hyprland/src/render/Renderer.hpp>

#include "CBoxShadowsDecoration.hpp"
#include "globals.hpp"

// Do NOT change this function.
APICALL EXPORT std::string PLUGIN_API_VERSION() { return HYPRLAND_API_VERSION; }

void onNewWindow(void *self, std::any data) {
  // data is guaranteed
  const auto PWINDOW = std::any_cast<PHLWINDOW>(data);

  HyprlandAPI::addWindowDecoration(PHANDLE, PWINDOW,
                                   makeUnique<CBoxShadowsDecoration>(PWINDOW));
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

  HyprlandAPI::addConfigValue(PHANDLE, "plugin:shadows-plus-plus:add_shadows",
                              Hyprlang::INT{1});

  for (size_t i = 1; i <= 10; ++i) {
    const std::string base =
        "plugin:shadows-plus-plus:shadow_" + std::to_string(i);
    HyprlandAPI::addConfigValue(PHANDLE, base + ":color",
                                Hyprlang::INT{0xee1a1a1a});
    HyprlandAPI::addConfigValue(PHANDLE, base + ":offset",
                                Hyprlang::VEC2{0, 0});
    HyprlandAPI::addConfigValue(PHANDLE, base + ":blur_radius",
                                Hyprlang::INT{3});
    HyprlandAPI::addConfigValue(PHANDLE, base + ":spread_radius",
                                Hyprlang::INT{4});
    // HyprlandAPI::addConfigValue(PHANDLE, base + ":inset", Hyprlang::INT{0});
    HyprlandAPI::addConfigValue(PHANDLE, base + ":ignore_window",
                                Hyprlang::INT{1});
    HyprlandAPI::addConfigValue(PHANDLE, base + ":scale", Hyprlang::FLOAT{1.f});
  }

  HyprlandAPI::reloadConfig();

  static auto P = HyprlandAPI::registerCallbackDynamic(
      PHANDLE, "openWindow",
      [&](void *self, SCallbackInfo &info, std::any data) {
        onNewWindow(self, data);
      });

  // add deco to existing windows
  for (auto &w : g_pCompositor->m_windows) {
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
