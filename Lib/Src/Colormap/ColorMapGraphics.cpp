#include "stdafx.h"
#include <Math.h>
#include "ColorMapGraphics.h"

ColorMapGraphics::ColorMapGraphics() {
  m_rect.left = m_rect.top = -1;
  m_map                = NULL;
  m_colorScale         = NULL;
  m_sunken             = false;
  m_hasBorder          = false;
  m_clientEdge         = false;
  m_staticEdge         = false;
  m_modalFrame         = false;
  m_textAlignment      = TEXT_ALIGNLEFT;
  m_colorScaleDirty    = true;
  m_propertiesDirty    = false;
  m_backgroundColor    = ::GetSysColor(COLOR_BTNFACE);
  setCurrentLSHColor(LSHColor(0.1f,0.1f,0.5f));
}

ColorMapGraphics::~ColorMapGraphics() {
  cleanup();
}

void ColorMapGraphics::setControl(CWnd *wnd) {
  m_wnd = wnd;
}

LabelledRect::LabelledRect(int left,int top,int right,int bottom,const char *label) : CRect(left,top,right,bottom) {
  m_label = label;
}

void ColorMapGraphics::init(const CRect &rect) {
  cleanup();
  m_rect     = rect;
  CRect clientRect = m_rect;
  int marg = borderVisible() ? 4 : 2;
  clientRect.left   += marg;
  clientRect.right  -= marg;
  if(borderVisible() && m_caption != "") {
    clientRect.top += 14;
  } else {
    clientRect.top += marg;
  }
  clientRect.bottom -= marg;

#define FIELDHEIGHT 22
#define FIELDWIDTH  35
#define FIELDSTEP (FIELDHEIGHT+2)

#define ARROWWIDTH    9
#define ARROWHEIGHT   18

  int left = clientRect.right - 2*FIELDWIDTH - 35;
  int top  = clientRect.bottom - 3*FIELDSTEP;

  m_redFieldRect        = LabelledRect(left,top,left+FIELDWIDTH,top+FIELDHEIGHT,"Red"  ); top += FIELDSTEP;
  m_greenFieldRect      = LabelledRect(left,top,left+FIELDWIDTH,top+FIELDHEIGHT,"Green"); top += FIELDSTEP;
  m_blueFieldRect       = LabelledRect(left,top,left+FIELDWIDTH,top+FIELDHEIGHT,"Blue" ); top += FIELDSTEP;

  left += FIELDWIDTH + 35;
  top  = clientRect.bottom - 3*FIELDSTEP;

  m_hueFieldRect        = LabelledRect(left,top,left+FIELDWIDTH,top+FIELDHEIGHT,"Hue"); top += FIELDSTEP;
  m_saturationFieldRect = LabelledRect(left,top,left+FIELDWIDTH,top+FIELDHEIGHT,"Sat"); top += FIELDSTEP;
  m_luminationFieldRect = LabelledRect(left,top,left+FIELDWIDTH,top+FIELDHEIGHT,"Lum"); top += FIELDSTEP;

  m_resultRect.left     = clientRect.left;
  m_resultRect.top      = m_redFieldRect.top;
  m_resultRect.bottom   = clientRect.bottom;
  m_resultRect.right    = m_redFieldRect.left - 50;

  m_mapRect.left        = clientRect.left;
  m_mapRect.top         = clientRect.top;
  m_mapRect.right       = clientRect.right - 25;
  m_mapRect.bottom      = m_redFieldRect.top - 5;

  m_scaleRect.left      = clientRect.right - 20;
  m_scaleRect.top       = clientRect.top;
  m_scaleRect.right     = m_scaleRect.left + 10;
  m_scaleRect.bottom    = m_mapRect.bottom;

  m_scalePosRect.left   = m_scaleRect.right + 1;
  m_scalePosRect.top    = m_scaleRect.top;
  m_scalePosRect.right  = m_scalePosRect.left + ARROWWIDTH+1;
  m_scalePosRect.bottom = m_scaleRect.bottom;

  int width  = m_mapRect.Width();
  int height = m_mapRect.Height();

  m_map = new RGBColor*[height];
  for(int r = 0; r < height; r++) {
    m_map[r] = new RGBColor[width];
  }
  m_colorScale = new RGBColor[m_scaleRect.Height()+1];
  for(int h = 0; h < width; h++) {
    float hue       = (float)h / (width-1);
    RGBColor c0     = getRGBColor(LSHColor(hue,1,0.5));
    float red       = c0.m_red;
    float green     = c0.m_green;
    float blue      = c0.m_blue;
    float redStep   = (float)(0.5-red  ) / (height-1);
    float greenStep = (float)(0.5-green) / (height-1);
    float blueStep  = (float)(0.5-blue ) / (height-1);
    for(int s = 0; s < height; s++, red+=redStep,green+=greenStep,blue+=blueStep) {
      m_map[s][h] = RGBColor(red,green,blue);
    }
  }
/*
  LSH lsh;
  lsh.m_lumination = 0.5;
  for(int h = 0; h < width; h++) {
    lsh.m_hue = (float)h/(width-1);
    for(int s = 0; s < height; s++) {
      lsh.m_saturation = (float)s/(height-1);
      m_map[height-s-1][h] = getRGBColor(lsh);
    }
  }
*/

  m_propertiesDirty = false;
  m_colorScaleDirty = true;
}

void ColorMapGraphics::setHasBorder(bool hasBorder) {
  if(hasBorder != m_hasBorder) {
    m_hasBorder = hasBorder;
    m_propertiesDirty = true;
  }
}

void ColorMapGraphics::setIsSunken(bool sunken) {
  if(sunken != m_sunken) {
    m_sunken = sunken;
    m_propertiesDirty = true;
  }
}

void ColorMapGraphics::setHasClientEdge(bool clientEdge) {
  if(clientEdge != m_clientEdge) {
    m_clientEdge = clientEdge;
    m_propertiesDirty = true;
  }
}

void ColorMapGraphics::setHasStaticEdge(bool staticEdge) {
  if(staticEdge != m_staticEdge) {
    m_staticEdge = staticEdge;
    m_propertiesDirty = true;
  }
}

void ColorMapGraphics::setHasModalFrame(bool modalFrame) {
  if(modalFrame != m_modalFrame) {
    m_modalFrame = modalFrame;
    m_propertiesDirty = true;
  }
}

void ColorMapGraphics::setTextAlignment(TextAlignment textAlignment) {
  if(textAlignment != m_textAlignment) {
    m_textAlignment = textAlignment;
    m_propertiesDirty = true;
  }
}

void ColorMapGraphics::setCaption(const CString &caption) {
  if(caption != m_caption) {
    m_caption = caption;
    m_propertiesDirty = true;
  }
}

void ColorMapGraphics::cleanup() {
  if(m_map != NULL) {
    int height = m_mapRect.Height();
    for(int r = 0; r < height; r++) {
      delete[] m_map[r];
    }
    delete[] m_map;
    m_map = NULL;
    delete[] m_colorScale;
    m_colorScale = NULL;
  }
  releaseBitmaps();
}

bool ColorMapGraphics::bitmapsInitialized() {
  return m_colorBitmap.m_hObject != NULL;
}

void ColorMapGraphics::releaseBitmaps() {
  if(bitmapsInitialized()) {
    m_colorBitmap.DeleteObject();
    m_posBitmap.DeleteObject();
    m_arrowBitmap.DeleteObject();
    m_dc.DeleteDC();
  }
}

void ColorMapGraphics::initBitmaps(CDC &dc) {
  releaseBitmaps();
  if(!valid(m_mapRect)) {
    return;
  }
  m_dc.CreateCompatibleDC(&dc);

  int width  = m_mapRect.Width();
  int height = m_mapRect.Height();

  m_colorBitmap.CreateBitmap(width,height,dc.GetDeviceCaps(PLANES),dc.GetDeviceCaps(BITSPIXEL),NULL);
  m_dc.SelectObject(m_colorBitmap);

  CPoint p;
  for(p.y = 0; p.y < height; p.y++) {
    for(p.x = 0; p.x < width; p.x++) {
      const D3DCOLOR d3dc = m_map[p.y][p.x];
      m_dc.SetPixel(p,D3DCOLOR2COLORREF(d3dc));
    }
  }

#define MAPCURSORSIZE 19
#define MAPCROSSSIZE   5
#define MAPCROSSWIDTH  3
#define MAPCURSORHALFSIZE (MAPCURSORSIZE/2)

#define BLACK RGB(0,0,0)
#define WHITE RGB(255,255,255)

  m_posBitmap.CreateBitmap(MAPCURSORSIZE,MAPCURSORSIZE,dc.GetDeviceCaps(PLANES),dc.GetDeviceCaps(BITSPIXEL),NULL);
  m_dc.SelectObject(m_posBitmap);
  CBrush blackBrush;
  blackBrush.CreateSolidBrush(BLACK);

  m_dc.FillSolidRect(0                         ,0                         ,MAPCURSORSIZE ,MAPCURSORSIZE ,WHITE);
  m_dc.FillSolidRect(0                         ,MAPCURSORHALFSIZE-1       ,MAPCROSSSIZE  ,MAPCROSSWIDTH ,BLACK);
  m_dc.FillSolidRect(MAPCURSORHALFSIZE-1       ,MAPCURSORSIZE-MAPCROSSSIZE,MAPCROSSWIDTH ,MAPCROSSSIZE  ,BLACK);
  m_dc.FillSolidRect(MAPCURSORSIZE-MAPCROSSSIZE,MAPCURSORHALFSIZE-1       ,MAPCROSSSIZE  ,MAPCROSSWIDTH ,BLACK);
  m_dc.FillSolidRect(MAPCURSORHALFSIZE-1       ,0                         ,MAPCROSSWIDTH ,MAPCROSSSIZE  ,BLACK);

  m_arrowBitmap.CreateBitmap(ARROWWIDTH,ARROWHEIGHT,dc.GetDeviceCaps(PLANES),dc.GetDeviceCaps(BITSPIXEL),NULL);
  CRgn rgn;
  CPoint tp[] = { CPoint(0,ARROWWIDTH), CPoint(ARROWWIDTH,0), CPoint(ARROWWIDTH,ARROWHEIGHT) };
  rgn.CreatePolygonRgn(tp,3,ALTERNATE);
  m_dc.SelectObject(m_arrowBitmap);
  m_dc.FillSolidRect(0,0,ARROWWIDTH,ARROWHEIGHT,WHITE);
  m_dc.FillRgn(&rgn,&blackBrush);
}

void ColorMapGraphics::makeColorScale(const RGBColor &color) {
  int height      = m_scaleRect.Height()+1;
  int   middle    = height / 2;
  float red       = color.m_red;
  float green     = color.m_green;
  float blue      = color.m_blue;
  float redStep   = (float)(1.0 - red)  / (height-middle);
  float greenStep = (float)(1.0 - green)/ (height-middle);
  float blueStep  = (float)(1.0 - blue) / (height-middle);

  for(int y = middle; y < height; y++, red+=redStep, green+=greenStep, blue+=blueStep) {
    m_colorScale[y] = RGBColor(red,green,blue);
  }

  red       = color.m_red;
  green     = color.m_green;
  blue      = color.m_blue;
  redStep   = red   / (middle+1);
  greenStep = green / (middle+1);
  blueStep  = blue  / (middle+1);

  for(int y = middle; y >= 0; y--, red-=redStep, green-=greenStep, blue-=blueStep) {
    m_colorScale[y] = RGBColor(red,green,blue);
  }
  m_colorScaleDirty = false;
}

int ColorMapGraphics::getMapXCoord(float hue) const {
  return (int)round(m_mapRect.left + hue * (m_mapRect.Width()-1));
}

int ColorMapGraphics::getMapYCoord(float saturation) const {
  return (int)round(m_mapRect.bottom-1 - saturation * (m_mapRect.Height()-1));
}

float ColorMapGraphics::getHue(int x) const {
  return (float)(x-m_mapRect.left)/(m_mapRect.Width()-1);
}

float ColorMapGraphics::getSaturation(int y) const {
  return ((float)m_mapRect.bottom-1-y)/(m_mapRect.Height()-1);
}

float ColorMapGraphics::getLumination(int y) const {
  return ((float)m_scaleRect.bottom-1-y)/(m_scaleRect.Height()-1);
}

void ColorMapGraphics::setCurrentMapPoint(const CPoint &p) {
  if(!m_mapRect.PtInRect(p)) {
    return;
  }
  setCurrentHue(getHue(p.x));
  setCurrentSaturation(getSaturation(p.y));
}

CPoint ColorMapGraphics::getCurrentMapPoint() const {
  CPoint p;
  p.x = getMapXCoord(m_currentLSHColor.m_hue);
  p.y = getMapYCoord(m_currentLSHColor.m_saturation);
  return p;
}

void ColorMapGraphics::setCurrentScalePoint(const CPoint &p) {
  setCurrentLumination(getLumination(p.y));
}

CPoint ColorMapGraphics::getCurrentScalePoint() const {
  const CRect &scaleRect = getScaleRect();
  CPoint pos;
  pos.x = scaleRect.right;
  pos.y = scaleRect.bottom - (int)(getCurrentLumination() * scaleRect.Height());
  return pos;
}

void ColorMapGraphics::setCurrentColor(COLORREF color) {
  setCurrentLSHColor(getLSHColor(COLORREF2D3DCOLOR(color)));
}

COLORREF ColorMapGraphics::getCurrentColor() const {
  const D3DCOLOR c = getRGBColor(m_currentLSHColor);
  return D3DCOLOR2COLORREF(c);
}

void ColorMapGraphics::setCurrentLSHColor(const LSHColor &lsh) {
  setCurrentHue(lsh.m_hue);
  setCurrentSaturation(lsh.m_saturation);
  setCurrentLumination(lsh.m_lumination);
}

void ColorMapGraphics::setCurrentHue(float hue) {
  if(hue < 0 || hue > 1) {
    return;
  }
  m_currentLSHColor.m_hue = hue;
  m_colorScaleDirty = true;
}

void ColorMapGraphics::setCurrentSaturation(float saturation) {
  if(saturation < 0 || saturation > 1) {
    return;
  }
  m_currentLSHColor.m_saturation = saturation;
  m_colorScaleDirty = true;
}

void ColorMapGraphics::setCurrentLumination(float lumination) {
  if(lumination < 0 || lumination > 1) {
    return;
  }
  m_currentLSHColor.m_lumination = lumination;
}

const RGBColor &ColorMapGraphics::getMapColor(const CPoint &p) {
  return m_map[p.y - m_mapRect.top][p.x - m_mapRect.left];
}

void ColorMapGraphics::draw(CDC &dc, const CRect &rect, BOOL enabled) {
  if(rect != m_rect || m_propertiesDirty) {
    init(rect);
    dc.FillSolidRect(&rect,getBackgroundColor());
  }
  if(!bitmapsInitialized()) {
    initBitmaps(dc);
  }
  drawColorMap(dc);
  drawMapPosition(dc);
  drawColorScale(dc);
  drawResultColor(dc);
  drawScalePosition(dc);
  drawLabels(dc, enabled);
  if(borderVisible()) {
    drawBorder(dc);
  }
}

bool ColorMapGraphics::borderVisible() const {
  return m_hasBorder || m_sunken || m_clientEdge || m_staticEdge || m_modalFrame;
}

void ColorMapGraphics::draw(CDC &dc, BOOL enabled) {
  draw(dc,m_rect, enabled);
}

void ColorMapGraphics::drawBorder(CDC &dc) {
  UINT edgeStyle = m_modalFrame ? EDGE_BUMP : m_clientEdge ? EDGE_RAISED : m_sunken ? EDGE_SUNKEN : EDGE_ETCHED;
  if(m_caption == "") {
    dc.DrawEdge(m_rect,edgeStyle,BF_RECT);
  } else {
    CRect r = m_rect;
    r.top += 5;
    dc.DrawEdge(r,edgeStyle,BF_RECT);
    dc.SetBkColor(getBackgroundColor());
    const int textSize = dc.GetTextExtent(m_caption).cx;
    int textPos = 0;
    switch(m_textAlignment) {
    case TEXT_ALIGNLEFT  : textPos = r.left+4;               break;
    case TEXT_ALIGNRIGHT : textPos = r.right - textSize - 4; break;
    case TEXT_ALIGNCENTER: textPos = (r.right - textSize)/2; break;
    }
    dc.TextOut(textPos, r.top-6, m_caption);
  }
/*
  const DWORD oldStyle = m_wnd->GetExStyle();
  const DWORD newStyle = (m_clientEdge ? WS_EX_CLIENTEDGE    : 0) 
                       | (m_staticEdge ? WS_EX_STATICEDGE    : 0)
                       | (m_modalFrame ? WS_EX_DLGMODALFRAME : 0);

#define BORDERFLAGS (WS_EX_CLIENTEDGE|WS_EX_STATICEDGE|WS_EX_DLGMODALFRAME)
//  WINDOWPLACEMENT wp;
//  m_wnd->GetWindowPlacement(&wp);
  const DWORD dwRemove = ( oldStyle & ~newStyle) & BORDERFLAGS;
  const DWORD dwAdd    = (~oldStyle &  newStyle) & BORDERFLAGS; 
  if(dwRemove || dwAdd) {
    m_wnd->ModifyStyleEx(dwRemove, dwAdd);
  }
*/
}

void ColorMapGraphics::drawSurroundingEdge(CDC &dc, const CRect &r) {
  CRect r1 = r;
  r1.left   -= 2;
  r1.top    -= 2;
  r1.right  += 2;
  r1.bottom += 2;
  dc.DrawEdge(r1,EDGE_SUNKEN,BF_RECT);
}

void ColorMapGraphics::drawColorMap(CDC &dc) {
  if(!valid(m_mapRect)) {
    return;
  }
  m_dc.SelectObject(m_colorBitmap);
  dc.BitBlt(m_mapRect.left, m_mapRect.top, m_mapRect.right, m_mapRect.bottom, &m_dc, 0, 0, SRCCOPY);
  drawSurroundingEdge(dc,m_mapRect);
}

void ColorMapGraphics::drawMapPosition(CDC &dc) {
  if(!valid(m_mapRect)) {
    return;
  }
  CPoint p1,p2,p3;
  CPoint pos = getCurrentMapPoint();
  p1.x = max(pos.x-MAPCURSORHALFSIZE,m_mapRect.left);
  p1.y = max(pos.y-MAPCURSORHALFSIZE,m_mapRect.top);
  p2.x = min(MAPCURSORSIZE,m_mapRect.right  - pos.x+MAPCURSORHALFSIZE);
  p2.y = min(MAPCURSORSIZE,m_mapRect.bottom - pos.y+MAPCURSORHALFSIZE);
  p3.x = max(MAPCURSORHALFSIZE - (pos.x-m_mapRect.left),0);
  p3.y = max(MAPCURSORHALFSIZE - (pos.y-m_mapRect.top) ,0);

  m_dc.SelectObject(m_posBitmap);
  dc.BitBlt(p1.x,p1.y,p2.x,p2.y, &m_dc, p3.x,p3.y, SRCAND);
}

void ColorMapGraphics::drawColorScale(CDC &dc) {
  if(!valid(m_scaleRect)) {
    return;
  }
  if(m_colorScaleDirty) {
    makeColorScale(getMapColor(getCurrentMapPoint()));
  }

  int height = m_scaleRect.Height();
  for(int y = 0; y < height; y++) {
    CPen mypen;
    const D3DCOLOR c3d = m_colorScale[height-y];
    mypen.CreatePen(PS_SOLID,1,D3DCOLOR2COLORREF(c3d));
    dc.SelectObject(&mypen);
    dc.MoveTo(m_scaleRect.left ,m_scaleRect.top+y);
    dc.LineTo(m_scaleRect.right,m_scaleRect.top+y);
  }
  drawSurroundingEdge(dc,m_scaleRect);
}

void ColorMapGraphics::drawScalePosition(CDC &dc) {
  if(!valid(m_scaleRect)) {
    return;
  }
  const CRect &scaleRect = getScaleRect();
  CPoint pos;
  BITMAP info;
  m_arrowBitmap.GetObject(sizeof(info),&info);
  pos.x = scaleRect.right + 2;
  pos.y = scaleRect.bottom - (int)(getCurrentLumination() * scaleRect.Height()) - (ARROWHEIGHT/2);
  dc.FillSolidRect(m_scalePosRect,getBackgroundColor());
  m_dc.SelectObject(m_arrowBitmap);
  int h  = ARROWHEIGHT;
  int sy = 0;
  int d = pos.y - scaleRect.top;
  if(d < 0) {
    pos.y = scaleRect.top;
    h += d;
    sy -= d;
  } else {
    d = scaleRect.bottom - (pos.y + ARROWHEIGHT);
    if(d < 0) {
      h += d;
    }
  }
  dc.BitBlt(pos.x,pos.y,ARROWWIDTH,h,&m_dc,0,sy,SRCAND);
}

void ColorMapGraphics::drawResultColor(CDC &dc) {
  if(!valid(m_resultRect)) {
    return;
  }
  CBrush brush;
  brush.CreateSolidBrush(getCurrentColor());
  dc.FillRect(&m_resultRect,&brush);
  drawSurroundingEdge(dc,m_resultRect);
}

void ColorMapGraphics::drawLabels(CDC &dc, BOOL enabled) {
  drawLabel(dc,m_redFieldRect       , enabled);
  drawLabel(dc,m_greenFieldRect     , enabled);
  drawLabel(dc,m_blueFieldRect      , enabled);
  drawLabel(dc,m_hueFieldRect       , enabled);
  drawLabel(dc,m_saturationFieldRect, enabled);
  drawLabel(dc,m_luminationFieldRect, enabled);
}

void ColorMapGraphics::drawLabel(CDC &dc, LabelledRect &r, BOOL enabled) {
  CSize cs = dc.GetTextExtent(r.m_label);
  dc.SetTextColor(enabled ? RGB(0,0,0) : RGB(160,160,160));
  dc.SetBkColor(getBackgroundColor());
  dc.TextOut(r.left-cs.cx-2,r.top+cs.cy/2,r.m_label);
}

long textAlignmentToLong(TextAlignment textAlignment) {
  switch(textAlignment) {
  case TEXT_ALIGNLEFT  : return 0;
  case TEXT_ALIGNRIGHT : return 2;
  case TEXT_ALIGNCENTER: return 1;
  }
  return 0;
}

TextAlignment longToTextAlignment(long value) {
  switch(value) {
  case 0: return TEXT_ALIGNLEFT;
  case 1: return TEXT_ALIGNCENTER;
  case 2: return TEXT_ALIGNRIGHT;
  }
  return TEXT_ALIGNLEFT;
}
