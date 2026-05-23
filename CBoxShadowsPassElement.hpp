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

  virtual std::vector<UP<IPassElement>> draw() override;
  virtual bool needsLiveBlur() override;
  virtual bool needsPrecomputeBlur() override;

  virtual const char *passName() override { return "CBoxShadowsPassElement"; }

  virtual ePassElementType type() override { return EK_CUSTOM; }

private:
  SShadowData m_data;
};
