#pragma once

#include <hyprland/src/render/WindowRenderPresentation.hpp>
#include <hyprland/src/render/pass/PassElement.hpp>

class CBoxShadowsDecoration;

class CBoxShadowsPassElement : public IPassElement {
public:
  struct SShadowData {
    CBoxShadowsDecoration *deco = nullptr;
    float a = 1.F;
    Render::SWindowRenderPresentation presentation;
  };

  CBoxShadowsPassElement(const SShadowData &data_);
  virtual ~CBoxShadowsPassElement() = default;

  virtual std::vector<UP<IPassElement>>
  draw(Render::CRenderContext &ctx) override;
  virtual bool needsLiveBlur(Render::CRenderContext &ctx) override;
  virtual bool needsPrecomputeBlur(Render::CRenderContext &ctx) override;

  virtual const char *passName() override { return "CBoxShadowsPassElement"; }

  virtual ePassElementType type() override { return EK_CUSTOM; }

private:
  SShadowData m_data;
};
