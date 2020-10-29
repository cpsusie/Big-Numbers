#pragma once

#include <MFCUtil/AbstractMouseHandler.h>
#include "PolygonSet.h"

class DrawTool : public AbstractMouseHandler {
private:
  CBrush m_blackBrush, m_redBrush;
  TCHAR  m_infostr[400];

protected:
  ProfileEditor         &m_editor;
  PolygonSet             m_polygonSet;
  Point2DRefArray        m_selectedPoints;
  ProfileEditorState     m_state;

  DrawTool &drawState();
  DrawTool &paintBox(                  Point2D          *p, bool selected);
  DrawTool &paintPoints(               ProfilePolygon2D *p, bool selected);
  DrawTool &repaintPolygon(            ProfilePolygon2D *p, bool selected);
  DrawTool &selectPolygonsInRect(const Rectangle2D      &r);
  DrawTool &selectPointsInRect(  const Rectangle2D      &r);
  DrawTool &select(                    ProfilePolygon2D *p);
  DrawTool &unselect(                  ProfilePolygon2D *p);
  bool      isSelected(                ProfilePolygon2D *p) const;
  DrawTool &select(                    Point2D          *p);
  DrawTool &unselect(                  Point2D          *p);
  bool      isSelected(                Point2D          *p) const;
public:
  DrawTool(ProfileEditor *editor);
  virtual ~DrawTool() {
  }
  bool OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) override;
  bool      canConnect() const;
  DrawTool &connect();
  bool      canInvertNormals() const;
  DrawTool &invertNormals();
  bool      canMirror() const;
  DrawTool &mirror(bool horizontal);
  bool      canDelete();
  DrawTool &deleteSelected();
  bool      canCopy();
  DrawTool &copySelected();
  bool      canCut();
  DrawTool &cutSelected();
  DrawTool &paste();
  inline void initState() {
    m_state = IDLE;
  }
  DrawTool &unselectAll();
  DrawTool &repaintProfile();
  DrawTool &repaintScreen();
  DrawTool &repaintAll();
};
