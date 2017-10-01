#pragma once

#include <MFCUtil/ViewPort2D.h>
#include <Math/SigmoidIterator.h>
#include "ChessGraphics.h"

class ChessAnimation {
private:
  SavedImageRect m_save;
protected:
  ChessGraphics &m_graphics;
  const ChessResources &getResources() const {
    return m_graphics.getResources();
  }
  void restoreBackground(const CRect &r) {
    m_graphics.restoreBackground(r);
  }
  void saveImageRect(const CRect &r) {
    m_graphics.saveImageRect(m_save, r);
  }
  void restoreImageRect() {
    m_graphics.restoreImageRect(m_save);
  }
  inline const CRect &getSavedRect() const {
    return m_save.getSavedRect();
  }
  CPoint unscalePoint(const CPoint &p) const {
    return m_graphics.getResources().unscalePoint(p);
  }
  ChessAnimation(ChessGraphics *graphics) : m_graphics(*graphics) {
  }
};

class AbstractPieceMoveAnimation : public ChessAnimation {
private:
  PixRect     *m_helper;
protected:
  const Image *m_pieceImage;
  const CSize  m_imageSize;
  const CSize  m_fieldSize;
  void paintImage(const CPoint &p);
public:
  AbstractPieceMoveAnimation(ChessGraphics *graphics, const Image *image, const CSize &size)
    : ChessAnimation(graphics)
    , m_pieceImage(image)
    , m_imageSize(graphics->getResources().getImageSize0())
    , m_fieldSize(graphics->getResources().getFieldSize0())
    , m_helper(new PixRect(theApp.m_device, PIXRECT_PLAINSURFACE,size))
  {
    TRACE_NEW(m_helper);
  }
  ~AbstractPieceMoveAnimation() {
    SAFEDELETE(m_helper);
  }
};

class MoveSinglePieceAnimation : public AbstractPieceMoveAnimation {
private:
  const Point2DP        m_from, m_to;
  Point2DP              m_pos;
  SigmoidIterator       m_it;
public:
  MoveSinglePieceAnimation(ChessGraphics *graphics, const int from, const int to, int steps = 0);
  bool step();
  void paint();
  void unpaint();
  UINT getSteps() const {
    return m_it.getSteps();
  }
};

class MoveAnimation : private CompactArray<MoveSinglePieceAnimation*> {
private:
  ChessGraphics *m_graphics;
  int            m_steps;
  inline void beginPaint() {
    m_graphics->beginPaint();
  }
  inline void endPaint() {
    m_graphics->endPaint();
  }
public:
  MoveAnimation(ChessGraphics *graphics) : m_graphics(graphics), m_steps(0) {
  }
  ~MoveAnimation() {
    for (size_t i = 0; i < size(); i++) SAFEDELETE((*this)[i]);
    clear();
  }
  void addMovePiece(const int from, const int to);
  void animate();
};

class PieceDragger : public AbstractPieceMoveAnimation {
private:
  const OffboardPiece  &m_obp;
  CPoint                m_offset; // offset of mouse-click to topLeft of image
public:
  PieceDragger(ChessGraphics *graphics, const CPoint &point, const OffboardPiece *obp);
  void drag(const CPoint &point);
  void endDrag();

  PieceKey getPieceKey() const {
    return m_obp.getKey();
  }
};

class MateAnimation : public ChessAnimation {
private:
  const int m_position;
public:
  MateAnimation(ChessGraphics *graphics, int position);
  void animate();
};
