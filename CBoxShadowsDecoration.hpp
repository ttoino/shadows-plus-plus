#pragma once

#include <hyprland/src/config/shared/complex/ComplexDataTypes.hpp>
#include <hyprland/src/render/WindowRenderPresentation.hpp>
#include <hyprland/src/render/decorations/IHyprWindowDecoration.hpp>

class CBoxShadowsDecoration : public IHyprWindowDecoration {
public:
  CBoxShadowsDecoration(PHLWINDOW);
  virtual ~CBoxShadowsDecoration();

  virtual SDecorationPositioningInfo getPositioningInfo();

  virtual void onPositioningReply(const SDecorationPositioningReply &reply);

  virtual void
  draw(Render::CRenderContext &ctx, PHLMONITOR, float const &a,
       const Render::SWindowRenderPresentation &presentation) override;

  virtual eDecorationType getDecorationType();

  virtual void updateWindow(PHLWINDOW);

  virtual void damageEntire();

  virtual eDecorationLayer getDecorationLayer();

  virtual uint64_t getDecorationFlags();

  virtual std::string getDisplayName();

  void render(Render::CRenderContext &ctx, PHLMONITOR, float const &a,
              const Render::SWindowRenderPresentation &presentation);

private:
  struct SShadowRenderData {
    bool valid = false;

    CBox box;
    int rounding = 0;
    float roundingPower = 0.F;
    int blurRadius = 0;

    const Config::CGradientValueData *color = nullptr;
    bool sharp = false;
  };

  SBoxExtents m_extents;
  SBoxExtents m_reportedExtents;

  PHLWINDOWREF m_window;

  Vector2D m_lastWindowPos;
  Vector2D m_lastWindowSize;

  CBox m_lastWindowBox = {0};
  CBox m_lastWindowBoxWithDecos = {0};

  bool canRender(PHLMONITOR pMonitor);

  std::vector<SShadowRenderData> getRenderData(PHLMONITOR pMonitor);

  void reposition(Render::CRenderContext &ctx);

  void
  drawShadowInternal(Render::CRenderContext &ctx, const CBox &box, int round,
                     float roundingPower, int blurRadius,
                     const Config::CGradientValueData &grad, bool sharp,
                     float a,
                     const Render::SWindowRenderPresentation &presentation);
};
