#include "CBoxShadowsPassElement.hpp"
#include "CBoxShadowsDecoration.hpp"

#include <hyprland/src/render/Renderer.hpp>

CBoxShadowsPassElement::CBoxShadowsPassElement(
    const CBoxShadowsPassElement::SShadowData &data_)
    : m_data(data_) {
  ;
}

std::vector<UP<IPassElement>> CBoxShadowsPassElement::draw() {
  m_data.deco->render(g_pHyprRenderer->m_renderData.pMonitor.lock(), m_data.a);
  return {};
}

bool CBoxShadowsPassElement::needsLiveBlur() { return false; }

bool CBoxShadowsPassElement::needsPrecomputeBlur() { return false; }
