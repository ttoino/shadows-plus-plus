#pragma once

#include <hyprland/src/render/decorations/IHyprWindowDecoration.hpp>

class CBoxShadowsDecoration : public IHyprWindowDecoration {
public:
  CBoxShadowsDecoration(PHLWINDOW);
  virtual ~CBoxShadowsDecoration() = default;

  virtual SDecorationPositioningInfo getPositioningInfo();

  virtual void onPositioningReply(const SDecorationPositioningReply &reply);

  virtual void draw(PHLMONITOR, float const &a);

  virtual eDecorationType getDecorationType();

  virtual void updateWindow(PHLWINDOW);

  virtual void damageEntire();

  virtual eDecorationLayer getDecorationLayer();

  virtual uint64_t getDecorationFlags();

  virtual std::string getDisplayName();

  void render(PHLMONITOR, float const &a);

private:
  SBoxExtents m_extents;
  SBoxExtents m_reportedExtents;

  PHLWINDOWREF m_window;

  Vector2D m_lastWindowPos;
  Vector2D m_lastWindowSize;

  CBox m_lastWindowBox = {0};
  CBox m_lastWindowBoxWithDecos = {0};

  void drawShadowInternal(const CBox &box, int round, float roundingPower,
                          int blurRadius, CHyprColor color, float a);
};
