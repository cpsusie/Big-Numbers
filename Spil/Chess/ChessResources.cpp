#include "stdafx.h"
#include <Math.h>
#include "ChessGraphics.h"

int           ChessResources::s_instanceCount    = 0;
const Point2D ChessResources::s_upperLeftCorner0 = CPoint(351,110);
const Size2D  ChessResources::s_fieldSize0       = CSize(74,74);

Image        *ChessResources::s_boardImage;
ImageArray    ChessResources::s_pieceImage[2];
ImageArray    ChessResources::s_markImage;
Image        *ChessResources::s_selectionFrameImage;
Image        *ChessResources::s_playerIndicator;
CFont         ChessResources::s_boardTextFont;
CFont         ChessResources::s_debugInfoFont;

#define MINSCALE 0.4

ChessResources::ChessResources()
: m_hourGlassImage(     IDB_HOURGLASS, 6)
{
  m_scale = Point2D(1,1);
  m_hourGlassImage.setSecondsPerCycle(2,0.6);

  if(s_instanceCount++ == 0) {
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
 ,MOVEFROMMARK  , IDB_SRCMARK     , RESOURCE_PNG   , true
 ,MOVETOMARK    , IDB_DSTMARK     , RESOURCE_PNG   , true
};

void ChessResources::load() {
  s_boardImage              = new Image(IDR_BOARD, RESOURCE_JPEG); TRACE_NEW(s_boardImage);
  for(int i = 0; i < ARRAYSIZE(fmattr); i++) {
    const FieldMarkAttributes &fma = fmattr[i];
    Image *im = new Image(fma.m_resid, fma.m_type, fma.m_transparentWhite); TRACE_NEW(im);
    s_markImage.add(im);
  }

  s_selectionFrameImage     = new Image(IDB_SELECTIONFRAME , RESOURCE_PNG); TRACE_NEW(s_selectionFrameImage);
  s_playerIndicator         = new Image(IDB_PLAYERINDICATOR, RESOURCE_PNG); TRACE_NEW(s_playerIndicator    );

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
      Image *im = new Image(pieceImages[p][i],RESOURCE_PNG); TRACE_NEW(im);
      imageArray.add(im);
    }
  }

  m_boardSize0           = s_boardImage->getSize();
  m_imageSize0           = s_pieceImage[0][0]->getSize();
  m_selectionFrameSize0  = s_selectionFrameImage->getSize();
  setClientRectSize(s_boardImage->getSize());
}

void ChessResources::unload() {
  for(int i = 0; i < 2; i++) {
    ImageArray &a = s_pieceImage[i];
    for(size_t j = 0; j < a.size(); j++) {
      SAFEDELETE(a[j]);
    }
    a.clear();
  }
  SAFEDELETE(s_boardImage);
  for(size_t i = 0; i < s_markImage.size(); i++) {
    SAFEDELETE(s_markImage[i]);
  }
  s_markImage.clear();

  SAFEDELETE(s_selectionFrameImage);
  SAFEDELETE(s_playerIndicator    );
}

CBitmap &ChessResources::getSmallPieceBitmap(CBitmap &dst, PieceKey pk) const { // for promote-menu
  const Size2D size0     = s_fieldSize0;
  const CSize  imageSize = Size2D(max(getAvgScale()*0.8,MINSCALE) * size0);
  PixRect pr(theApp.m_device, PIXRECT_PLAINSURFACE, s_fieldSize0);
  pr.rop(ORIGIN
        ,s_fieldSize0
        ,SRCCOPY
        ,getBoardImage()
        ,(GET_PLAYER_FROMKEY(pk)==WHITEPLAYER)
        ?Point2D(s_upperLeftCorner0+Size2D(0,s_fieldSize0.cy))
        :s_upperLeftCorner0);
  getPieceImage(pk)->paintImage(pr, ORIGIN);

  HDC tmpDC = CreateCompatibleDC(NULL);
  dst.CreateBitmap(imageSize.cx, imageSize.cy, GetDeviceCaps(tmpDC, PLANES), GetDeviceCaps(tmpDC, BITSPIXEL), NULL);
  HGDIOBJ oldBitmap = SelectObject(tmpDC, dst);
  SetStretchBltMode(tmpDC, COLORONCOLOR /*HALFTONE*/);
  PixRect::stretchBlt(tmpDC, ORIGIN,imageSize,SRCCOPY,&pr,ORIGIN,size0);
  SelectObject(tmpDC, oldBitmap);
  DeleteDC(tmpDC);
  return dst;
}

const Image *ChessResources::getFieldMarkImage(FieldMark m) const {
  return s_markImage[m];
}

void ChessResources::setClientRectSize(const CSize &size) {
  if(size != m_crSize) {
    m_crSize  = size;
    m_scale.x = max((double)m_crSize.cx / m_boardSize0.cx, MINSCALE);
    m_scale.y = max((double)m_crSize.cy / m_boardSize0.cy, MINSCALE);
  }
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
    throwInvalidArgumentException(__TFUNCTION__, _T("Imagetype=%d"), type);
  }
}
