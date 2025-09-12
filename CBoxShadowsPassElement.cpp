#include "CBoxShadowsPassElement.hpp"
#include "CBoxShadowsDecoration.hpp"

#include <hyprland/src/render/OpenGL.hpp>

CBoxShadowsPassElement::CBoxShadowsPassElement(
    const CBoxShadowsPassElement::SShadowData &data_)
    : m_data(data_) {
  ;
}

void CBoxShadowsPassElement::draw(const CRegion &damage) {
  m_data.deco->render(g_pHyprOpenGL->m_renderData.pMonitor.lock(), m_data.a);
}

bool CBoxShadowsPassElement::needsLiveBlur() { return false; }

bool CBoxShadowsPassElement::needsPrecomputeBlur() { return false; }
