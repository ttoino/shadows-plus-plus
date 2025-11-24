#include "CBoxShadowsDecoration.hpp"
#include "CBoxShadowsPassElement.hpp"
#include "globals.hpp"

#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/desktop/Window.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/render/Renderer.hpp>

CBoxShadowsDecoration::CBoxShadowsDecoration(PHLWINDOW pWindow)
    : IHyprWindowDecoration(pWindow), m_window(pWindow) {}

eDecorationType CBoxShadowsDecoration::getDecorationType() {
  return DECORATION_CUSTOM;
}

SDecorationPositioningInfo CBoxShadowsDecoration::getPositioningInfo() {
  SDecorationPositioningInfo info;
  info.policy = DECORATION_POSITION_ABSOLUTE;
  info.desiredExtents = m_extents;
  info.edges = DECORATION_EDGE_BOTTOM | DECORATION_EDGE_LEFT |
               DECORATION_EDGE_RIGHT | DECORATION_EDGE_TOP;

  m_reportedExtents = m_extents;
  return info;
}

void CBoxShadowsDecoration::onPositioningReply(
    const SDecorationPositioningReply &reply) {
  updateWindow(m_window.lock());
}

uint64_t CBoxShadowsDecoration::getDecorationFlags() {
  return DECORATION_NON_SOLID;
}

std::string CBoxShadowsDecoration::getDisplayName() { return "Box Shadows"; }

void CBoxShadowsDecoration::damageEntire() {
  static auto *const PSHADOWS =
      (Hyprlang::INT *const *)HyprlandAPI::getConfigValue(
          PHANDLE, "plugin:shadows-plus-plus:add_shadows")
          ->getDataStaticPtr();

  if (**PSHADOWS == 0)
    return;

  const auto PWINDOW = m_window.lock();

  CBox shadowBox = {PWINDOW->m_realPosition->value().x - m_extents.topLeft.x,
                    PWINDOW->m_realPosition->value().y - m_extents.topLeft.y,
                    PWINDOW->m_realSize->value().x + m_extents.topLeft.x +
                        m_extents.bottomRight.x,
                    PWINDOW->m_realSize->value().y + m_extents.topLeft.y +
                        m_extents.bottomRight.y};

  const auto PWORKSPACE = PWINDOW->m_workspace;
  if (PWORKSPACE && PWORKSPACE->m_renderOffset->isBeingAnimated() &&
      !PWINDOW->m_pinned)
    shadowBox.translate(PWORKSPACE->m_renderOffset->value());
  shadowBox.translate(PWINDOW->m_floatingOffset);

  static std::vector<Hyprlang::INT *const *> PSHADOWSIGNOREWINDOW;
  for (size_t i = 1; i <= 10; ++i)
    PSHADOWSIGNOREWINDOW.push_back(
        (Hyprlang::INT *const *)HyprlandAPI::getConfigValue(
            PHANDLE, "plugin:shadows-plus-plus:shadow_" + std::to_string(i) +
                         ":ignore_window")
            ->getDataStaticPtr());

  const auto ROUNDING = PWINDOW->rounding();
  const auto ROUNDINGSIZE = ROUNDING - M_SQRT1_2 * ROUNDING + 1;

  CRegion shadowRegion(shadowBox);
  if (std::all_of(PSHADOWSIGNOREWINDOW.begin(),
                  PSHADOWSIGNOREWINDOW.begin() + **PSHADOWS,
                  [](auto *const shadow) { return **shadow; })) {
    CBox surfaceBox = PWINDOW->getWindowMainSurfaceBox();
    if (PWORKSPACE && PWORKSPACE->m_renderOffset->isBeingAnimated() &&
        !PWINDOW->m_pinned)
      surfaceBox.translate(PWORKSPACE->m_renderOffset->value());
    surfaceBox.translate(PWINDOW->m_floatingOffset);
    surfaceBox.expand(-ROUNDINGSIZE);
    shadowRegion.subtract(CRegion(surfaceBox));
  }

  for (auto const &m : g_pCompositor->m_monitors) {
    if (!g_pHyprRenderer->shouldRenderWindow(PWINDOW, m)) {
      const CRegion monitorRegion({m->m_position, m->m_size});
      shadowRegion.subtract(monitorRegion);
    }
  }

  g_pHyprRenderer->damageRegion(shadowRegion);
}

void CBoxShadowsDecoration::updateWindow(PHLWINDOW pWindow) {
  const auto PWINDOW = m_window.lock();

  m_lastWindowPos = PWINDOW->m_realPosition->value();
  m_lastWindowSize = PWINDOW->m_realSize->value();

  m_lastWindowBox = {m_lastWindowPos.x, m_lastWindowPos.y, m_lastWindowSize.x,
                     m_lastWindowSize.y};
  m_lastWindowBoxWithDecos =
      g_pDecorationPositioner->getBoxWithIncludedDecos(pWindow);
}

void CBoxShadowsDecoration::draw(PHLMONITOR pMonitor, float const &a) {
  CBoxShadowsPassElement::SShadowData data;
  data.deco = this;
  data.a = a;
  g_pHyprRenderer->m_renderPass.add(makeUnique<CBoxShadowsPassElement>(data));
}

void CBoxShadowsDecoration::render(PHLMONITOR pMonitor, float const &a) {
  const auto PWINDOW = m_window.lock();

  if (!validMapped(PWINDOW) ||
      !PWINDOW->m_ruleApplicator->decorate().valueOrDefault() ||
      PWINDOW->m_ruleApplicator->noShadow().valueOrDefault())
    return;

  static auto *const PSHADOWS =
      (Hyprlang::INT *const *)HyprlandAPI::getConfigValue(
          PHANDLE, "plugin:shadows-plus-plus:add_shadows")
          ->getDataStaticPtr();

  if (**PSHADOWS == 0)
    return;

  static std::vector<Hyprlang::INT *const *> PCOLORS;
  static std::vector<Hyprlang::VEC2 *const *> POFFSETS;
  static std::vector<Hyprlang::INT *const *> PBLURRADII;
  static std::vector<Hyprlang::INT *const *> PSPREADRADII;
  static std::vector<Hyprlang::INT *const *> PIGNOREWINDOWS;
  static std::vector<Hyprlang::FLOAT *const *> PSCALES;

  for (size_t i = 1; i <= 10; ++i) {
    const std::string base =
        "plugin:shadows-plus-plus:shadow_" + std::to_string(i);
    PCOLORS.push_back((Hyprlang::INT *const *)HyprlandAPI::getConfigValue(
                          PHANDLE, base + ":color")
                          ->getDataStaticPtr());
    POFFSETS.push_back((Hyprlang::VEC2 *const *)HyprlandAPI::getConfigValue(
                           PHANDLE, base + ":offset")
                           ->getDataStaticPtr());
    PBLURRADII.push_back((Hyprlang::INT *const *)HyprlandAPI::getConfigValue(
                             PHANDLE, base + ":blur_radius")
                             ->getDataStaticPtr());
    PSPREADRADII.push_back((Hyprlang::INT *const *)HyprlandAPI::getConfigValue(
                               PHANDLE, base + ":spread_radius")
                               ->getDataStaticPtr());
    PIGNOREWINDOWS.push_back(
        (Hyprlang::INT *const *)HyprlandAPI::getConfigValue(
            PHANDLE, base + ":ignore_window")
            ->getDataStaticPtr());
    PSCALES.push_back((Hyprlang::FLOAT *const *)HyprlandAPI::getConfigValue(
                          PHANDLE, base + ":scale")
                          ->getDataStaticPtr());
  }

  const auto ROUNDINGBASE = PWINDOW->rounding();
  const auto ROUNDINGPOWER = PWINDOW->roundingPower();
  const auto ROUNDING =
      ROUNDINGBASE > 0 ? ROUNDINGBASE + PWINDOW->getRealBorderSize() : 0;
  const auto PWORKSPACE = PWINDOW->m_workspace;
  const auto WORKSPACEOFFSET = PWORKSPACE && !PWINDOW->m_pinned
                                   ? PWORKSPACE->m_renderOffset->value()
                                   : Vector2D();

  updateWindow(PWINDOW);
  m_lastWindowPos += WORKSPACEOFFSET;

  g_pHyprOpenGL->scissor(nullptr);
  g_pHyprOpenGL->m_renderData.currentWindow = m_window;

  for (size_t i = 0; i < **PSHADOWS; ++i) {
    const CHyprColor PCOLOR{(uint64_t)**PCOLORS.at(i)};
    const auto POFFSET = **POFFSETS.at(i);
    const auto PBLURRADIUS = **PBLURRADII.at(i);
    const auto PSPREADRADIUS = **PSPREADRADII.at(i);
    const auto PIGNOREWINDOW = **PIGNOREWINDOWS.at(i);
    const auto PSCALE = std::clamp(**PSCALES.at(i), 0.f, 1.f);

    const auto PSIZE = PBLURRADIUS + PSPREADRADIUS;
    CBox box = m_lastWindowBoxWithDecos;
    box.translate(-pMonitor->m_position + WORKSPACEOFFSET);
    box.x -= PSIZE;
    box.y -= PSIZE;
    box.w += 2 * PSIZE;
    box.h += 2 * PSIZE;

    box.scaleFromCenter(PSCALE).translate({POFFSET.x, POFFSET.y});

    m_extents.topLeft.x =
        std::max(m_extents.topLeft.x,
                 m_lastWindowPos.x - box.x - pMonitor->m_position.x + 2);
    m_extents.topLeft.y =
        std::max(m_extents.topLeft.y,
                 m_lastWindowPos.y - box.y - pMonitor->m_position.y + 2);
    m_extents.bottomRight.x =
        std::max(m_extents.bottomRight.x,
                 box.x + box.width + pMonitor->m_position.x -
                     m_lastWindowPos.x - m_lastWindowSize.x + 2);
    m_extents.bottomRight.y =
        std::max(m_extents.bottomRight.y,
                 box.y + box.height + pMonitor->m_position.y -
                     m_lastWindowPos.y - m_lastWindowSize.y + 2);

    box.translate(PWINDOW->m_floatingOffset);

    if (box.width < 1 || box.height < 1)
      continue;

    box.scale(pMonitor->m_scale).round();

    drawShadowInternal(box, (ROUNDING + PSPREADRADIUS) * pMonitor->m_scale,
                       ROUNDINGPOWER, PBLURRADIUS * pMonitor->m_scale, PCOLOR,
                       a);
  }

  if (m_extents != m_reportedExtents)
    g_pDecorationPositioner->repositionDeco(this);

  g_pHyprOpenGL->m_renderData.currentWindow.reset();
}

eDecorationLayer CBoxShadowsDecoration::getDecorationLayer() {
  return DECORATION_LAYER_BOTTOM;
}

void CBoxShadowsDecoration::drawShadowInternal(const CBox &box, int round,
                                               float roundingPower,
                                               int blurRadius, CHyprColor color,
                                               float a) {
  if (box.w < 1 || box.h < 1)
    return;

  g_pHyprOpenGL->blend(true);

  color.a *= a;

  g_pHyprOpenGL->renderRoundedShadow(box, round, roundingPower, 2 * blurRadius,
                                     color, 1.F);
}
