#pragma once

#include <hyprland/src/render/pass/PassElement.hpp>

class CBoxShadowsDecoration;

class CBoxShadowsPassElement : public IPassElement {
public:
  struct SShadowData {
    CBoxShadowsDecoration *deco = nullptr;
    float a = 1.F;
  };

  CBoxShadowsPassElement(const SShadowData &data_);
  virtual ~CBoxShadowsPassElement() = default;

  virtual void draw(const CRegion &damage);
  virtual bool needsLiveBlur();
  virtual bool needsPrecomputeBlur();

  virtual const char *passName() { return "CBoxShadowsPassElement"; }

private:
  SShadowData m_data;
};
