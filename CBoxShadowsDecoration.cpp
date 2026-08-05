#include "CBoxShadowsDecoration.hpp"
#include "CBoxShadowsPassElement.hpp"
#include "globals.hpp"

#include <algorithm>
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/desktop/view/Window.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/render/Renderer.hpp>
#include <hyprland/src/state/MonitorState.hpp>

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

static size_t shadowCount() {
  return std::clamp<Config::INTEGER>(
      vars.addShadows->value(), 0,
      static_cast<Config::INTEGER>(vars.shadowIgnoreWindows.size()));
}

void CBoxShadowsDecoration::damageEntire() {
  const size_t NUMSHADOWS = shadowCount();
  if (NUMSHADOWS == 0)
    return;

  const auto PWINDOW = m_window.lock();

  const auto WINDOWPOS =
      PWINDOW->position(Desktop::View::IGeometric::GEOMETRIC_CURRENT);
  const auto WINDOWSIZE =
      PWINDOW->size(Desktop::View::IGeometric::GEOMETRIC_CURRENT);

  CBox shadowBox = {
      WINDOWPOS.x - m_extents.topLeft.x, WINDOWPOS.y - m_extents.topLeft.y,
      WINDOWSIZE.x + m_extents.topLeft.x + m_extents.bottomRight.x,
      WINDOWSIZE.y + m_extents.topLeft.y + m_extents.bottomRight.y};

  const auto PWORKSPACE = PWINDOW->m_workspace;
  if (PWORKSPACE && PWORKSPACE->m_renderOffset->isBeingAnimated() &&
      !PWINDOW->m_pinned)
    shadowBox.translate(PWORKSPACE->m_renderOffset->value());
  shadowBox.translate(PWINDOW->m_floatingOffset);

  const auto ROUNDING = PWINDOW->rounding();
  const auto ROUNDINGSIZE = ROUNDING - M_SQRT1_2 * ROUNDING + 1;

  CRegion shadowRegion(shadowBox);
  if (std::all_of(vars.shadowIgnoreWindows.begin(),
                  vars.shadowIgnoreWindows.begin() + NUMSHADOWS,
                  [](const auto &shadow) { return shadow->value(); })) {
    CBox surfaceBox = PWINDOW->getWindowMainSurfaceBox();
    if (PWORKSPACE && PWORKSPACE->m_renderOffset->isBeingAnimated() &&
        !PWINDOW->m_pinned)
      surfaceBox.translate(PWORKSPACE->m_renderOffset->value());
    surfaceBox.translate(PWINDOW->m_floatingOffset);
    surfaceBox.expand(-ROUNDINGSIZE);
    shadowRegion.subtract(CRegion(surfaceBox));
  }

  for (auto const &m : State::monitorState()->monitors()) {
    if (!g_pHyprRenderer->shouldRenderWindow(PWINDOW, m)) {
      const CRegion monitorRegion(CBox{m->m_position, m->m_size});
      shadowRegion.subtract(monitorRegion);
    }
  }

  g_pHyprRenderer->damageRegion(shadowRegion);
}

void CBoxShadowsDecoration::updateWindow(PHLWINDOW pWindow) {
  const auto PWINDOW = m_window.lock();

  m_lastWindowPos =
      PWINDOW->position(Desktop::View::IGeometric::GEOMETRIC_CURRENT);
  m_lastWindowSize =
      PWINDOW->size(Desktop::View::IGeometric::GEOMETRIC_CURRENT);

  m_lastWindowBox = {m_lastWindowPos.x, m_lastWindowPos.y, m_lastWindowSize.x,
                     m_lastWindowSize.y};
  m_lastWindowBoxWithDecos =
      g_pDecorationPositioner->getBoxWithIncludedDecos(pWindow);
}

void CBoxShadowsDecoration::draw(PHLMONITOR pMonitor, float const &a) {
  CBoxShadowsPassElement::SShadowData data;
  data.deco = this;
  data.a = a;
  g_pHyprRenderer->addPassElement(makeUnique<CBoxShadowsPassElement>(data));
}

void CBoxShadowsDecoration::render(PHLMONITOR pMonitor, float const &a) {
  const auto PWINDOW = m_window.lock();

  if (!validMapped(PWINDOW) ||
      !PWINDOW->m_ruleApplicator->decorate().valueOrDefault() ||
      PWINDOW->m_ruleApplicator->noShadow().valueOrDefault())
    return;

  const size_t NUMSHADOWS = shadowCount();
  if (NUMSHADOWS == 0)
    return;

  const auto BORDERSIZE = PWINDOW->getRealBorderSize();
  const auto ROUNDINGBASE = PWINDOW->rounding();
  const auto ROUNDINGPOWER = PWINDOW->roundingPower();
  const auto CORRECTIONOFFSET =
      BORDERSIZE * (M_SQRT2 - 1) * std::max(2.0 - ROUNDINGPOWER, 0.0);
  const auto ROUNDING =
      ROUNDINGBASE > 0 ? (ROUNDINGBASE + BORDERSIZE) - CORRECTIONOFFSET : 0;
  const auto PWORKSPACE = PWINDOW->m_workspace;
  const auto WORKSPACEOFFSET = PWORKSPACE && !PWINDOW->m_pinned
                                   ? PWORKSPACE->m_renderOffset->value()
                                   : Vector2D();

  updateWindow(PWINDOW);
  m_lastWindowPos += WORKSPACEOFFSET;

  g_pHyprRenderer->disableScissor();
  g_pHyprRenderer->m_renderData.currentWindow = m_window;

  for (size_t i = 0; i < NUMSHADOWS; ++i) {
    const CHyprColor PCOLOR{
        static_cast<uint64_t>(vars.shadowColors[i]->value())};
    const auto POFFSET = vars.shadowOffsets[i]->value();
    const auto PBLURRADIUS = vars.shadowBlurRadii[i]->value();
    const auto PSPREADRADIUS = vars.shadowSpreadRadii[i]->value();
    const auto PIGNOREWINDOW = vars.shadowIgnoreWindows[i]->value();
    const auto PSCALE = std::clamp(vars.shadowScales[i]->value(), 0.f, 1.f);

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

    // TODO: Handle ignore_window case

    drawShadowInternal(box, (ROUNDING + PSPREADRADIUS) * pMonitor->m_scale,
                       ROUNDINGPOWER, PBLURRADIUS * pMonitor->m_scale, PCOLOR,
                       a);
  }

  if (m_extents != m_reportedExtents)
    g_pDecorationPositioner->repositionDeco(this);

  g_pHyprRenderer->m_renderData.currentWindow.reset();
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

  g_pHyprRenderer->blend(true);

  color.a *= a;

  g_pHyprRenderer->drawShadow(box, round, roundingPower, 2 * blurRadius,
                              Config::CGradientValueData{color}, a);
}
