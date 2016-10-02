#include "stdafx.h"
#include <Math.h>
#include <MFCUtil/WinTools.h>
#include "ChessGraphics.h"

int          ChessResources::s_instanceCount    = 0;
CSize        ChessResources::s_screenSize;
const CPoint ChessResources::s_upperLeftCorner0 = CPoint(351,110); 
const CSize  ChessResources::s_fieldSize0       = CSize(74,74);

Image       *ChessResources::s_boardImage;
ImageArray   ChessResources::s_pieceImage[2];
ImageArray   ChessResources::s_markImage;
Image       *ChessResources::s_selectionFrameImage;
Image       *ChessResources::s_playerIndicator;
CFont        ChessResources::s_boardTextFont;
CFont        ChessResources::s_debugInfoFont;

ChessResources::ChessResources() 
: m_hourGlassImage(     IDB_HOURGLASS, 6)
{
  m_scale = -1;
  m_hourGlassImage.setSecondsPerCycle(2,0.6);

  if(s_instanceCount++ == 0) {
    s_screenSize = ::getScreenSize(true);
    s_boardTextFont.CreateFont( 18, 18, 0, 0, 700, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS
                                ,CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY
                                ,DEFAULT_PITCH | FF_MODERN
                                ,_T("Courier") );

    s_debugInfoFont.CreateFont(8, 8, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS
                                ,CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY
                                ,DEFAULT_PITCH | FF_MODERN
                                ,_T("Courier") );
    load();
  }
}

ChessResources::~ChessResources() {
  if(--s_instanceCount == 0) {
    unload();
  }
}

void ChessResources::reload() {
  unload();
  load();
}

typedef struct {
  FieldMark m_mark;
  int       m_resid;
  ImageType m_type;
  bool      m_transparentWhite;
} FieldMarkAttributes;

static const FieldMarkAttributes fmattr[] = {
  BLUEMARK      , IDB_BLUEFIELD   , RESOURCE_BITMAP, false
 ,GREENMARK     , IDB_GREENFIELD  , RESOURCE_BITMAP, false
 ,PINKMARK      , IDB_PINKFIELD   , RESOURCE_BITMAP, false
 ,PURPLEMARK    , IDB_PURPLEFIELD , RESOURCE_BITMAP, false
 ,YELLOWMARK    , IDB_YELLOWFIELD , RESOURCE_BITMAP, false
 ,CHECKEDKING   , IDB_CHECKEDKING , RESOURCE_BITMAP, false
 ,MOVEFROMMARK  , IDB_SRCMARK     , RESOURCE_BITMAP, true
 ,MOVETOMARK    , IDB_DSTMARK     , RESOURCE_BITMAP, true
};

void ChessResources::load() {
  s_boardImage              = new Image(IDR_BOARD          , RESOURCE_JPEG);
  for(int i = 0; i < ARRAYSIZE(fmattr); i++) {
    const FieldMarkAttributes &fma = fmattr[i];
    s_markImage.add(new Image(fma.m_resid, fma.m_type, fma.m_transparentWhite));
  }

  s_selectionFrameImage     = new Image(IDB_SELECTIONFRAME , RESOURCE_BITMAP, true);
  s_playerIndicator         = new Image(IDB_PLAYERINDICATOR, RESOURCE_BITMAP, true);

  static const int pieceImages[][7] = {
    {  IDB_BITMAPNOPIECE
      ,IDB_BITMAPWHITEKING
      ,IDB_BITMAPWHITEQUEEN
      ,IDB_BITMAPWHITEROOK
      ,IDB_BITMAPWHITEBISHOP
      ,IDB_BITMAPWHITEKNIGHT
      ,IDB_BITMAPWHITEPAWN
    },
    {  IDB_BITMAPNOPIECE
      ,IDB_BITMAPBLACKKING
      ,IDB_BITMAPBLACKQUEEN
      ,IDB_BITMAPBLACKROOK
      ,IDB_BITMAPBLACKBISHOP
      ,IDB_BITMAPBLACKKNIGHT
      ,IDB_BITMAPBLACKPAWN
    }
  };

  forEachPlayer(p) {
    ImageArray &imageArray = s_pieceImage[p];
    for(int i = 0; i < ARRAYSIZE(pieceImages[p]); i++) {
      imageArray.add(new Image(pieceImages[p][i], RESOURCE_BITMAP, true));
    }
  }

  m_boardSize0           = s_boardImage->getSize();
  m_selectionFrameSize0  = s_selectionFrameImage->getSize();
  const double maxScaleX = (double)s_screenSize.cx / m_boardSize0.cx;
  const double maxScaleY = (double)s_screenSize.cy / m_boardSize0.cy;
  m_maxScale = min(maxScaleX, maxScaleY);

  setScale(1.0);
}

CBitmap &ChessResources::getSmallPieceBitmap(CBitmap &dst, PieceKey pk) const { // for promote-menu
  const int size  = (int)(56 * m_scale);
  PixRect pr(theApp.m_device, PIXRECT_PLAINSURFACE, s_fieldSize0);
  pr.rop(ORIGIN, s_fieldSize0, SRCCOPY, getBoardImage(), (GET_PLAYER_FROMKEY(pk)==WHITEPLAYER)?(s_upperLeftCorner0+CSize(0,s_fieldSize0.cy)):s_upperLeftCorner0);
  getPieceImage(pk)->paintImage(pr, ORIGIN);

  HDC tmpDC = CreateCompatibleDC(NULL);
  CDC *dcp = CDC::FromHandle(tmpDC);
  dst.CreateBitmap(size, size, GetDeviceCaps(tmpDC, PLANES), GetDeviceCaps(tmpDC, BITSPIXEL), NULL);
  CBitmap *oldBitmap = dcp->SelectObject(&dst);
  SetStretchBltMode(*dcp, COLORONCOLOR /*HALFTONE*/);
  PixRect::stretchBlt(*dcp, 0,0,size,size,SRCCOPY,&pr,0,0,s_fieldSize0.cx,s_fieldSize0.cy);
  dcp->SelectObject(oldBitmap);
  DeleteDC(tmpDC);
  return dst;
}

void ChessResources::unload() {
  for(int i = 0; i < 2; i++) {
    ImageArray &a = s_pieceImage[i];
    for(size_t j = 0; j < a.size(); j++) {
      delete a[j];
    }
    a.clear();
  }
  delete s_boardImage; s_boardImage = NULL;
  for(size_t i = 0; i < s_markImage.size(); i++) {
    delete s_markImage[i];
  }
  s_markImage.clear();

  delete s_selectionFrameImage;  s_selectionFrameImage = NULL;
  delete s_playerIndicator;      s_playerIndicator     = NULL;
}

const Image *ChessResources::getFieldMarkImage(FieldMark m) const {
  return s_markImage[m];
}

static int intRound(double x) {
  return (int)round(x);
}

CPoint operator*(const CPoint &pt, double factor) {
  return CPoint(intRound((double)pt.x*factor), intRound((double)pt.y*factor));
}

CPoint operator/(const CPoint &pt, double factor) {
  return CPoint(intRound((double)pt.x/factor), intRound((double)pt.y/factor));
}

const CSize &ChessResources::setScale(double scale) {
  if(scale != m_scale) {
    m_scale              = minMax(scale, 0.4, m_maxScale);
    m_boardSize          = m_boardSize0          * m_scale;
    m_upperLeftCorner    = s_upperLeftCorner0    * m_scale;
    m_fieldSize          = s_fieldSize0          * m_scale;
    m_selectionFrameSize = m_selectionFrameSize0 * m_scale;
  }
  return m_boardSize;
}

CPoint ChessResources::scalePoint(const CPoint &p) const {
  return p * m_scale;
}

CPoint ChessResources::unscalePoint(const CPoint &p) const {
  return p / m_scale;
}

CSize ChessResources::scaleSize(const CSize &s) const {
  return s * m_scale;
}

CRect ChessResources::scaleRect(const CRect &r) const {
  Point2D ul(r.left * m_scale, r.top * m_scale);
  Point2D lr(ul.x + r.Width() * m_scale, ul.y + r.Height() * m_scale);
  return CRect((int)ul.x, (int)ul.y, (int)ceil(lr.x), (int)ceil(lr.y));
}

void ChessResources::loadBitmap(CBitmap &dst, int resId, ImageType type) { // static
  if(dst.m_hObject != NULL) {
    dst.DeleteObject();
  }
  switch(type) {
  case RESOURCE_BITMAP:
    dst.LoadBitmap(resId);
    break;
  case RESOURCE_JPEG  :
    { Image tmp(resId, type);
      HDC hdc   = tmp.getDC();
      HDC tmpDC = CreateCompatibleDC(NULL);
      CDC *dcp = CDC::FromHandle(tmpDC);
      dst.CreateBitmap(tmp.getWidth(), tmp.getHeight(), GetDeviceCaps(tmpDC, PLANES), GetDeviceCaps(tmpDC, BITSPIXEL), NULL);
      CBitmap *oldBitmap = dcp->SelectObject(&dst);
      BitBlt(*dcp, 0, 0, tmp.getWidth(), tmp.getHeight(), hdc, 0, 0, SRCCOPY);
      dcp->SelectObject(oldBitmap);
      DeleteDC(tmpDC);
      tmp.releaseDC(hdc);
    }
    break;
  default:
    throwInvalidArgumentException(_T("Inage"), _T("Imagetype=%d"), type);
  }
}
