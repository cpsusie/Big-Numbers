#pragma once

#include "CoordinateSystemContainer.h"

class MouseTool {
protected:
  CoordinateSystemContainer *m_container;
  CRect                      m_dragRect;
  bool                       m_dragging;
  CPoint                     m_mouseDownPoint;
  Rectangle2DTransformation  m_mouseDownTransform;

  static void postMsg(int msg, WPARAM wp=0, LPARAM lp=0) {
    theApp.getMainWindow()->PostMessage(msg,wp,lp);
  }
  void repaint() {
    m_container->repaint();
  }
  inline CWnd *getWin() const {
    return m_container->getWin();
  }
  inline CCoordinateSystem &getSystem() {
    return m_container->getCoordinateSystem();
  }
  inline CFont &getAxisFont() const {
    return m_container->getAxisFont();
  }
  inline CShowGrafDoc *getDoc() {
    return theApp.getMainWindow()->getDoc();
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
  const CRect &getDragRect() const {
    return m_dragRect;
  }
  void clipCursor();
  void unClipCursor();
  void beginDragRect(     UINT nFlags, const CPoint &point);
  void beginMarkInterval( UINT nFlags, const CPoint &point);
  void updateMarkInterval(UINT nFlags, const CPoint &point);
  void endMarkInterval();

  MouseTool(CoordinateSystemContainer *container) : m_container(container), m_dragging(false) {
  }
  MouseTool(MouseTool &prevTool) : m_container(prevTool.m_container), m_dragging(false) {
  }

public:
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
  inline bool isDragging() const {
    return m_dragging;
  }
  void endDragRect();
  inline const CPoint &getMouseDownPoint() const {
    return m_mouseDownPoint;
  }

};

class IdleTool : public MouseTool {
public:
  IdleTool(CoordinateSystemContainer *container) : MouseTool(container) { reset(); }

  void OnLButtonDown(UINT nFlags, const CPoint &point);

  MouseToolType getType() const {
    return IDLETOOL;
  }
};

class DragTool : public MouseTool {
private:
  void takeoverDrag(MouseTool &mdTool); // called, to take over dragging, after mdTool, that handled MouseDown-event
  void updateDragRect(UINT nFlags, const CPoint &point);
public:
  DragTool(MouseTool &prevTool) : MouseTool(prevTool) {
    reset();
    takeoverDrag(prevTool);
  }
  void OnMouseMove(  UINT nFlags, const CPoint &point);
  void OnLButtonUp(  UINT nFlags, const CPoint &point);

  MouseToolType getType() const {
    return DRAGTOOL;
  }
};


class MovePointTool : public MouseTool {
private:
  GraphArray         *m_ga;
  MoveablePoint      *m_mp;
  CSize               m_offset;

  void takeoverDrag(MouseTool &mdTool);
  void beginMovePoint(const CPoint &point);
  void movePoint(UINT nFlags, const CPoint &point);
  void endMovePoint();
public:
  MovePointTool(MouseTool &prevTool) : MouseTool(prevTool) {
    reset();
    takeoverDrag(prevTool);
  }
  void OnMouseMove(  UINT nFlags, const CPoint &point);
  void OnLButtonUp(  UINT nFlags, const CPoint &point);

  MouseToolType getType() const {
    return MOVEPOINTTOOL;
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
