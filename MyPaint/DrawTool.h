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
  PENTOOL
 ,ERASETOOL
 ,LINETOOL
 ,RECTANGLETOOL
 ,POLYGONTOOL
 ,ELLIPSETOOL
 ,TEXTTOOL
 ,FILLTOOL
 ,FILLTRANSPARENTTOOL
 ,APPROXIMATEFILLTOOL
 ,CHANGEHUETOOL
 ,MOVERECTANGLETOOL
 ,MOVEREGIONTOOL
 ,COMBINEIMAGESTOOL
 ,INSERTIMAGETOOL
 ,NULLTOOL
} DrawToolType;

#define ORIGIN CPoint(0,0)

class DrawTool {
protected:
  PixRectContainer *m_container;

  void repaint() {
    m_container->repaint();
  }

public:
  DrawTool(PixRectContainer *container) : m_container(container) {
    m_container->enableCut(false);
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

class PenTool : public DrawTool {
private:
  CPoint m_p0;
  bool   m_hasStartPoint;
public:
  PenTool(PixRectContainer *container) : DrawTool(container) { reset(); }

  void OnLButtonDown(UINT nFlags, const CPoint &point) ;
  void OnMouseMove(  UINT nFlags, const CPoint &point);
  void OnLButtonUp(  UINT nFlags, const CPoint &point);

  DrawToolType getType() const {
    return PENTOOL;
  }
  int getCursorId() const {
    return IDC_CURSORPENCIL;
  }
  void reset() {
    m_hasStartPoint = false;
  }
};

class EraseTool : public DrawTool, public PointOperator {
private:
  CPoint m_lastPoint;

public:
  EraseTool(PixRectContainer *container) : DrawTool(container) {
  }

  void OnLButtonDown(UINT nFlags, const CPoint &point) ;
  void OnMouseMove(  UINT nFlags, const CPoint &point);
  void OnLButtonUp(  UINT nFlags, const CPoint &point);

  CRect makeRect(const CPoint &p);
  DrawToolType getType() const {
    return ERASETOOL;
  }
  int getCursorId() const {
    return IDC_CURSORERASE;
  }
  void apply(const CPoint &p);
};

class LineTool : public DrawTool {
private:
  CPoint m_p0, m_p1;
public:
  LineTool(PixRectContainer *container) : DrawTool(container) {}

  void OnLButtonDown(UINT nFlags, const CPoint &point) ;
  void OnMouseMove(  UINT nFlags, const CPoint &point);
  void OnLButtonUp(  UINT nFlags, const CPoint &point);
  DrawToolType getType() const {
    return LINETOOL;
  }
  int getCursorId() const {
    return IDC_CURSORHAIRCROSS;
  }
};

class RectangleTool : public DrawTool {
private:
  CPoint m_p0, m_p1;
public:
  RectangleTool(PixRectContainer *container) : DrawTool(container) {}

  void OnLButtonDown(UINT nFlags, const CPoint &point) ;
  void OnMouseMove(  UINT nFlags, const CPoint &point);
  void OnLButtonUp(  UINT nFlags, const CPoint &point);
  DrawToolType getType() const {
    return RECTANGLETOOL;
  }
  int getCursorId() const {
    return IDC_CURSORHAIRCROSS;
  }
};

class PolygonTool : public DrawTool {
private:
  CPoint m_p0;
  MyPolygon m_polygon;
  void invertDragLine();
public:
  PolygonTool(PixRectContainer *container) : DrawTool(container) {}

  void OnLButtonDown(  UINT nFlags, const CPoint &point) ;
  void OnLButtonDblClk(UINT nFlags, const CPoint &point);
  void OnMouseMove(    UINT nFlags, const CPoint &point);
  void OnLButtonUp(    UINT nFlags, const CPoint &point);
  DrawToolType getType() const {
    return POLYGONTOOL;
  }
  int getCursorId() const {
    return IDC_CURSORHAIRCROSS;
  }
};

class EllipseTool : public DrawTool {
private:
  CPoint m_p0, m_p1;
public:
  EllipseTool(PixRectContainer *container) : DrawTool(container) {}

  void OnLButtonDown(UINT nFlags, const CPoint &point) ;
  void OnMouseMove(  UINT nFlags, const CPoint &point);
  void OnLButtonUp(  UINT nFlags, const CPoint &point);
  DrawToolType getType() const {
    return ELLIPSETOOL;
  }
  int getCursorId() const {
    return IDC_CURSORHAIRCROSS;
  }
};

class FontParameters {
public:
  LOGFONT m_logFont;
  float   m_orientation;
  FontParameters();
};

class TextTool : public DrawTool {
  FontParameters &m_fontParameters;
  String         &m_text;
  bool            m_bezier;
public:
  TextTool(PixRectContainer *container, bool bezier, FontParameters &fontParameters, String &text);

  void OnLButtonDown(UINT nFlags, const CPoint &point) ;
  DrawToolType getType() const {
    return TEXTTOOL;
  }
};

class FillTool : public DrawTool {
public:
  FillTool(PixRectContainer *container) : DrawTool(container) {}

  void OnLButtonDown(UINT nFlags, const CPoint &point);
  DrawToolType getType() const {
    return FILLTOOL;
  }
  int getCursorId() const {
    return IDC_CURSORFILLCOLOR;
  }
};

class FillTransparentTool : public DrawTool {
public:
  FillTransparentTool(PixRectContainer *container) : DrawTool(container) {}

  void OnLButtonDown(UINT nFlags, const CPoint &point);
  DrawToolType getType() const {
    return FILLTRANSPARENTTOOL;
  }
  int getCursorId() const {
    return IDC_CURSORFILLCOLOR;
  }
};

class ApproximateFillTool : public DrawTool {
public:
  ApproximateFillTool(PixRectContainer *container) : DrawTool(container) {
  }
  void OnLButtonDown(UINT nFlags, const CPoint &point);

  DrawToolType getType() const {
    return APPROXIMATEFILLTOOL;
  }
  int getCursorId() const {
    return IDC_CURSORFILLCOLOR;
  }
};

class ChangeLSHOperator : public PixRectOperator {
private:
  const float m_deltaHue, m_deltaSat, m_deltaLum;
public:
  ChangeLSHOperator(PixRect *pixRect, float deltaHue, float deltaSaturation, float deltaLumination);
  void apply(const CPoint &p);
};

class ChangeHueTool : public DrawTool {
private:
  CPoint   m_p0, m_p1;
  CRect    m_lastDragRect;
  bool     m_dragRectVisible;
  void invertDragRect();
public:
  ChangeHueTool(PixRectContainer *container);
  ~ChangeHueTool();

  void OnLButtonDown(UINT nFlags, const CPoint &point) ;
  void OnMouseMove(  UINT nFlags, const CPoint &point);
  void OnLButtonUp(  UINT nFlags, const CPoint &point);
  DrawToolType getType() const {
    return CHANGEHUETOOL;
  }
  int getCursorId() const {
    return IDC_CURSORHAIRCROSS;
  }
  static bool changeHue(PixRect *pr);
};

class MoveRectangleTool : public DrawTool {
private:
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
  DrawToolType getType() const {
    return MOVERECTANGLETOOL;
  }
  void cut();
  void copy();
  int getCursorId() const;
};

class MoveRegionTool : public DrawTool {
private:
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
  DrawToolType getType() const {
    return MOVEREGIONTOOL;
  }
  int getCursorId() const;
};

class CombineImagesTool : public DrawTool {
private:
  CPoint   m_p0, m_p1, m_lastPoint;
  CRect    m_rect;
  PixRect *m_copy, *m_old;
  void releaseCopy();
  void invertDragRect();
  void makeFinalImage();
public:
  CombineImagesTool(PixRectContainer *container);
  ~CombineImagesTool();

  void OnLButtonDown(UINT nFlags, const CPoint &point);
  void OnMouseMove(  UINT nFlags, const CPoint &point);
  void OnLButtonUp(  UINT nFlags, const CPoint &point);
  DrawToolType getType() const {
    return COMBINEIMAGESTOOL;
  }
  int getCursorId() const;
};

class InsertImageTool : public DrawTool {
private:
  PixRect *m_image;
  PixRect *m_old;
  CPoint   m_upperLeft, m_lastPoint;
  bool     m_grabbedImage;
  void paintImage();
  void restoreOld();
  void invertImageFrame();
  CRect getImageRect() const;
public:
  InsertImageTool(PixRectContainer *container, PixRect *image);
  ~InsertImageTool();

  void OnLButtonDown(UINT nFlags, const CPoint &point);
  void OnMouseMove(  UINT nFlags, const CPoint &point);
  void OnLButtonUp(  UINT nFlags, const CPoint &point);
  void OnDelete();
  DrawToolType getType() const {
    return INSERTIMAGETOOL;
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

