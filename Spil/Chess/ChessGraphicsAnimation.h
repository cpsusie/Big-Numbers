#pragma once

#include "ChessGraphics.h"
#include <MFCUtil/ViewPort2D.h>
#include <Math/SigmoidIterator.h>

class PieceDragger {
private:
  const ChessResources &m_resources;
  const OffboardPiece  &m_obp;
  const Image          *m_pieceImage;
  PixRect              *m_background, *m_oldBackground, *m_helper;
  CPoint                m_lastTopLeft, m_lastDragPoint, m_savedDragPoint, m_lastPaintedPoint;
  CSize                 m_size;
  void saveDragPosition(const CPoint &p);
  void restoreDragPosition(HDC dc = NULL);
  void paintImage(const CPoint &p, HDC dc);
  void unpaintImage(HDC dc);
  void initPaintedPoint();
public:
  PieceDragger(ChessGraphics *graphics, const CPoint &point, const OffboardPiece *obp, HDC dc);
 ~PieceDragger();
  void drag(const CPoint &point, HDC dc);
  void endDrag(HDC dc);

  PieceKey getPieceKey() const {
    return m_obp.getKey();
  }
};

class MovePieceAnimation {
private:
  static PixRect       *background;
  static int            animationsAllocated;
  const ChessResources &m_resources;
  HDC                   m_dc;
  const Point2DP        m_from, m_to;
  const Image          *m_pieceImage;
  const CSize           m_size;
  const double          m_scale;
  PixRect              *m_oldBackground;
  Point2DP              m_pos;
  SigmoidIterator       m_it;
  CPoint                m_paintedPoint, m_backgroundPoint, m_restoredPoint;
  void paintImage(const CPoint &p);
  void initPaintedPoint();
  void saveBackground(const CPoint &p);
  void restoreBackground();
  void initBackgroundPoint();
public:
  MovePieceAnimation(ChessGraphics *graphics, HDC dc, const int from, const int to, int steps = 0);
  ~MovePieceAnimation();
  bool step();
  void paint();
  void unpaint();
  void flush();
  unsigned int getSteps() const {
    return m_it.getSteps();
  }
};

class RotatePieceAnimation {
private:
  ChessGraphics    &m_graphics;
  HDC               m_dc;
  const int         m_position;
public:
  RotatePieceAnimation(ChessGraphics *graphics, HDC dc, int position);
  void animate();
};

