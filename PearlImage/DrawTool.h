#pragma once

class PixRectContainer {
public:
  virtual PixRect *getImage()                          = 0;
  virtual D3DCOLOR getColor()                          = 0;
  virtual int      getCurrentZoomFactor()        const = 0;
  virtual int      getApproximateFillTolerance() const = 0;
  virtual void     repaint()                           = 0;
  virtual void     enableCut(bool enabled)             = 0;
  virtual void     saveDocState()                      = 0;
  virtual void     restoreOldTool()                    = 0;
};

typedef enum {
  MOVERECTANGLETOOL
 ,MOVEREGIONTOOL
 ,NULLTOOL
} DrawToolType;

class DrawTool {
protected:
  PixRectContainer *m_container;

  void repaint() {
    m_container->repaint();
  }

public:
  DrawTool(PixRectContainer *container) : m_container(container) {
    if(m_container) m_container->enableCut(false);
  }
  virtual ~DrawTool() {
  }
  virtual void OnLButtonDown(  UINT nFlags, const CPoint &point) {
  }
  virtual void OnLButtonDblClk(UINT nFlags, const CPoint &point) {
  }
  virtual void OnMouseMove(    UINT nFlags, const CPoint &point) {
  }
  virtual void OnLButtonUp(    UINT nFlags, const CPoint &point) {
  }
  virtual void cut() {
  }
  virtual void copy();
  virtual void OnDelete() {
  }
  virtual void reset() {
  }
  PixRect *getImage() {
    return m_container->getImage();
  }
  virtual DrawToolType getType()     const = 0;
  virtual int          getCursorId() const {
    return OCR_NORMAL;
  }
};

class FontParameters {
public:
  LOGFONT m_logFont;
  float   m_orientation;
  FontParameters();
};

class MoveRectangleTool : public DrawTool {
private:
  bool     m_docStateSaved;
  CPoint   m_p0, m_p1, m_lastPoint;
  CRect    m_rect, m_lastDragRect;
  bool     m_dragRectVisible;
  PixRect *m_copy, *m_old;
  void releaseCopy();
  void invertDragRect();
public:
  MoveRectangleTool(PixRectContainer *container);
  ~MoveRectangleTool();

  void OnLButtonDown(UINT nFlags, const CPoint &point);
  void OnMouseMove(  UINT nFlags, const CPoint &point);
  void OnLButtonUp(  UINT nFlags, const CPoint &point);
  void reset();
  DrawToolType getType() const {
    return MOVERECTANGLETOOL;
  }
  void cut();
  void copy();
  int getCursorId() const;
};

class MoveRegionTool : public DrawTool {
private:
  bool      m_docStateSaved;
  CPoint    m_lastPoint;
  MyPolygon m_polygon;
  CRect     m_rect;
  PixRect  *m_copy, *m_mask, *m_old;
  void releaseCopy();
  void invertPolygon(bool redraw = true);
  void createMask();
public:
  MoveRegionTool(PixRectContainer *container);
  ~MoveRegionTool();

  void OnLButtonDown(UINT nFlags, const CPoint &point);
  void OnMouseMove(  UINT nFlags, const CPoint &point);
  void OnLButtonUp(  UINT nFlags, const CPoint &point);
  void reset();
  DrawToolType getType() const {
    return MOVEREGIONTOOL;
  }
  int getCursorId() const;
};

class NullTool : public DrawTool {
public:
  NullTool() : DrawTool(NULL) {}
  DrawToolType getType() const {
    return NULLTOOL;
  }
};
