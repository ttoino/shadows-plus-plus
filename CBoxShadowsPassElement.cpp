#include "CBoxShadowsPassElement.hpp"
#include "CBoxShadowsDecoration.hpp"

#include <hyprland/src/render/Renderer.hpp>

CBoxShadowsPassElement::CBoxShadowsPassElement(
    const CBoxShadowsPassElement::SShadowData &data_)
    : m_data(data_) {
  ;
}

std::vector<UP<IPassElement>>
CBoxShadowsPassElement::draw(Render::CRenderContext &ctx) {
  m_data.deco->render(ctx, ctx.m_data.pMonitor.lock(), m_data.a,
                      m_data.presentation);
  return {};
}

bool CBoxShadowsPassElement::needsLiveBlur(Render::CRenderContext &ctx) {
  return false;
}

bool CBoxShadowsPassElement::needsPrecomputeBlur(Render::CRenderContext &ctx) {
  return false;
}
