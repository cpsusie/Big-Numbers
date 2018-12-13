#pragma once

#include <MFCUtil/Coordinatesystem/CoordinateSystem.h>

class CoordinateSystemContainer {
public:
  virtual CWnd              *getWin()                    = 0;
  virtual CCoordinateSystem &getCoordinateSystem()       = 0;
  virtual void               repaint()                   = 0;
};

typedef enum {
  DRAGTOOL
 ,FINDZEROTOOL
 ,FINDMAXTOOL
 ,FINDMINTOOL
 ,FINDINTERSECTIONTOOL
} MouseToolType;

class MouseTool {
private:
  CRect                      m_dragRect;
  bool                       m_dragging;
  CPoint                     m_mouseDownPoint;
  RectangleTransformation    m_mouseDownTransform;
protected:
  CoordinateSystemContainer *m_container;

  void repaint() {
    m_container->repaint();
  }
  inline CWnd *getWin() const {
    return m_container->getWin();
  }
  inline CCoordinateSystem &getSystem() {
    return m_container->getCoordinateSystem();
  }

  inline void setSystemCursor(int cursor) {
    ::setWindowCursor(&getSystem(), MAKEINTRESOURCE(cursor));
  }

  inline void setWindowCursor(int cursor) {
    ::setWindowCursor(getWin(), MAKEINTRESOURCE(cursor));
  }

  CRect getSystemClientRect() const {
    return getClientRect(getWin(), IDC_SYSTEMPANEL);
  }
  inline bool isDragging() const {
    return m_dragging;
  }
  const CRect &getDragRect() const {
    return m_dragRect;
  }
  void beginDragRect(     UINT nFlags, const CPoint &point);
  void endDragRect(       UINT nFlags, const CPoint &point);
  void updateDragRect(    UINT nFlags, const CPoint &point);
  void beginMarkInterval( UINT nFlags, const CPoint &point);
  void endMarkInterval(   UINT nFlags, const CPoint &point);
  void updateMarkInterval(UINT nFlags, const CPoint &point);

public:
  MouseTool(CoordinateSystemContainer *container) : m_container(container), m_dragging(false) {
  }
  virtual ~MouseTool() {
  }
  virtual void OnLButtonDown(  UINT nFlags, const CPoint &point) {
  }
  virtual void OnMouseMove(    UINT nFlags, const CPoint &point) {
  }
  virtual void OnLButtonUp(    UINT nFlags, const CPoint &point) {
  }
  virtual void reset() {
  }
  virtual MouseToolType getType() const = 0;
};

class DragTool : public MouseTool {
public:
  DragTool(CoordinateSystemContainer *container) : MouseTool(container) { reset(); }

  void OnLButtonDown(UINT nFlags, const CPoint &point) ;
  void OnMouseMove(  UINT nFlags, const CPoint &point);
  void OnLButtonUp(  UINT nFlags, const CPoint &point);

  MouseToolType getType() const {
    return DRAGTOOL;
  }
};

class SearchIntervalTool : public MouseTool {
private:
  const int m_buttonUpmessage;
protected:
  void postButtonUpMessage();
public:
  SearchIntervalTool(CoordinateSystemContainer *container, int buttonUpmessage)
    : MouseTool(container)
    , m_buttonUpmessage(buttonUpmessage)
  {
    reset();
  }

  void OnLButtonDown(UINT nFlags, const CPoint &point);
  void OnMouseMove(  UINT nFlags, const CPoint &point);
  void OnLButtonUp(  UINT nFlags, const CPoint &point);
};

class FindZeroTool : public SearchIntervalTool {
public:
  FindZeroTool(CoordinateSystemContainer *container) : SearchIntervalTool(container, ID_MSG_SEARCHZEROESININTERVAL) {
  }
  MouseToolType getType() const {
    return FINDZEROTOOL;
  }
};

class FindMaxTool : public SearchIntervalTool {
public:
  FindMaxTool(CoordinateSystemContainer *container) : SearchIntervalTool(container, ID_MSG_SEARCHMAXININTERVAL) {
  }
  MouseToolType getType() const {
    return FINDMAXTOOL;
  }
};

class FindMinTool : public SearchIntervalTool {
public:
  FindMinTool(CoordinateSystemContainer *container) : SearchIntervalTool(container, ID_MSG_SEARCHMINININTERVAL) {
  }
  MouseToolType getType() const {
    return FINDMINTOOL;
  }
};

class FontParameters {
public:
  LOGFONT m_logFont;
  float   m_orientation;
  FontParameters();
};
