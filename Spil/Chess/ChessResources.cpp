#include "stdafx.h"
#include <Math.h>
#include "ChessGraphics.h"

int           ChessResources::s_instanceCount    = 0;
const CPoint  ChessResources::s_upperLeftCorner0(351,110);
const CSize   ChessResources::s_fieldSize0(      74 ,74 );
CSize         ChessResources::s_imageSize0;
CSize         ChessResources::s_boardSize0;
CSize         ChessResources::s_selectionFrameSize0;

Image        *ChessResources::s_boardImage0;
ImageArray    ChessResources::s_pieceImage0[2];
ImageArray    ChessResources::s_markImage0;
Image        *ChessResources::s_selectionFrameImage0;
Image        *ChessResources::s_playerIndicator0;
CFont         ChessResources::s_boardTextFont0;
CFont         ChessResources::s_debugInfoFont;

#define MINSCALE 0.4

ChessResources::ChessResources()
: m_hourGlassImage0(     IDB_HOURGLASS, 6)
{
  m_scale = Point2D(1,1);
  m_hourGlassImage0.setSecondsPerCycle(2,0.6);

  if(s_instanceCount++ == 0) {
    s_boardTextFont0.CreateFont( 18, 18, 0, 0, 700, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS
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
    releaseFontCache();
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
 ,MOVEFROMMARK  , IDB_SRCMARK     , RESOURCE_PNG   , true
 ,MOVETOMARK    , IDB_DSTMARK     , RESOURCE_PNG   , true
};

void ChessResources::load() {
  s_boardImage0              = new Image(IDR_BOARD, RESOURCE_JPEG); TRACE_NEW(s_boardImage0);
  for(int i = 0; i < ARRAYSIZE(fmattr); i++) {
    const FieldMarkAttributes &fma = fmattr[i];
    Image *im = new Image(fma.m_resid, fma.m_type, fma.m_transparentWhite); TRACE_NEW(im);
    s_markImage0.add(im);
  }

  s_selectionFrameImage0    = new Image(IDB_SELECTIONFRAME , RESOURCE_PNG); TRACE_NEW(s_selectionFrameImage0);
  s_playerIndicator0        = new Image(IDB_PLAYERINDICATOR, RESOURCE_PNG); TRACE_NEW(s_playerIndicator0    );

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
    ImageArray &imageArray = s_pieceImage0[p];
    for(int i = 0; i < ARRAYSIZE(pieceImages[p]); i++) {
      Image *im = new Image(pieceImages[p][i],RESOURCE_PNG); TRACE_NEW(im);
      imageArray.add(im);
    }
  }

  s_boardSize0           = s_boardImage0->getSize();
  s_imageSize0           = s_pieceImage0[0][0]->getSize();
  s_selectionFrameSize0  = s_selectionFrameImage0->getSize();
  setClientRectSize(s_boardImage0->getSize());
}

void ChessResources::unload() {
  for(int i = 0; i < 2; i++) {
    ImageArray &a = s_pieceImage0[i];
    for(size_t j = 0; j < a.size(); j++) {
      SAFEDELETE(a[j]);
    }
    a.clear();
  }
  SAFEDELETE(s_boardImage0);
  for(size_t i = 0; i < s_markImage0.size(); i++) {
    SAFEDELETE(s_markImage0[i]);
  }
  s_markImage0.clear();

  SAFEDELETE(s_selectionFrameImage0);
  SAFEDELETE(s_playerIndicator0    );
}

CBitmap &ChessResources::getSmallPieceBitmap(CBitmap &dst, PieceKey pk) const { // for promote-menu
  const Size2D size0(FIELDSIZE0);
  const CSize imageSize = (CSize)Size2D(max(getAvgScale()*0.8,MINSCALE) * size0);
  PixRect pr(theApp.m_device, PIXRECT_PLAINSURFACE, s_fieldSize0);
  pr.rop(ORIGIN
        ,s_fieldSize0
        ,SRCCOPY
        ,getBoardImage0()
        ,(GET_PLAYER_FROMKEY(pk)==WHITEPLAYER)
        ?s_upperLeftCorner0+CSize(0,s_fieldSize0.cy)
        :s_upperLeftCorner0);
  getPieceImage0(pk)->paintImage(pr, ORIGIN);

  HDC tmpDC = CreateCompatibleDC(nullptr);
  dst.CreateBitmap(imageSize.cx, imageSize.cy, GetDeviceCaps(tmpDC, PLANES), GetDeviceCaps(tmpDC, BITSPIXEL), nullptr);
  HGDIOBJ oldBitmap = SelectObject(tmpDC, dst);
  SetStretchBltMode(  tmpDC, COLORONCOLOR /*HALFTONE*/);
  PixRect::stretchBlt(tmpDC, ORIGIN,imageSize,SRCCOPY,&pr,ORIGIN,FIELDSIZE0);
  SelectObject(       tmpDC, oldBitmap);
  DeleteDC(           tmpDC);
  return dst;
}

const Image *ChessResources::getFieldMarkImage0(FieldMark m) const {
  return s_markImage0[m];
}

void ChessResources::setClientRectSize(const CSize &size) {
  if(size != m_crSize) {
    m_crSize       = size;
    const CSize &bs0 = BOARDSIZE0;
    m_scale.x() = max((double)m_crSize.cx / bs0.cx, MINSCALE);
    m_scale.y() = max((double)m_crSize.cy / bs0.cy, MINSCALE);
  }
}

Rectangle2D ChessResources::scaleRect(const CRect &r) const {
  CPoint lb(r.left, r.bottom);
  CSize  sz(r.Size());
  return Rectangle2D(scalePoint(lb), scaleSize(sz));
}

CFont &ChessResources::getBoardFont() const {
  return getScaledFont(getBoardFont0(), getAvgScale());
}

void ChessResources::loadBitmap(CBitmap &dst, int resId, ImageType type) { // static
  if(dst.m_hObject != nullptr) {
    dst.DeleteObject();
  }
  switch(type) {
  case RESOURCE_BITMAP:
    dst.LoadBitmap(resId);
    break;
  case RESOURCE_JPEG  :
    { Image tmp(resId, type);
      HDC  hdc   = tmp.getDC();
      HDC  tmpDC = CreateCompatibleDC(nullptr);
      CDC *dcp   = CDC::FromHandle(tmpDC);
      dst.CreateBitmap(tmp.getWidth(), tmp.getHeight(), GetDeviceCaps(tmpDC, PLANES), GetDeviceCaps(tmpDC, BITSPIXEL), nullptr);
      CBitmap *oldBitmap = dcp->SelectObject(&dst);
      BitBlt(*dcp, 0, 0, tmp.getWidth(), tmp.getHeight(), hdc, 0, 0, SRCCOPY);
      dcp->SelectObject(oldBitmap);
      DeleteDC(tmpDC);
      tmp.releaseDC(hdc);
    }
    break;
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("Imagetype=%d"), type);
  }
}


CFont &ChessResources::getScaledFont(const CFont &src, double scale) const {
  if(scale == 1) {
    return (CFont&)src;
  }
  LOGFONT lf;
  ((CFont&)src).GetLogFont(&lf);
  UINT key = lf.lfHeight = (int)(scale * lf.lfHeight);
  CFont *result, **fontp = m_fontCache.get(key);
  if(fontp != nullptr) {
    result = *fontp;
  } else {
    result = new CFont(); TRACE_NEW(newFont);
    result->CreateFontIndirect(&lf);
    m_fontCache.put(key, result);
  }
  return *result;
}

void ChessResources::releaseFontCache() {
  for(auto it = m_fontCache.getIterator(); it.hasNext();) {
    SAFEDELETE(it.next().getValue());
  }
  m_fontCache.clear();
}
