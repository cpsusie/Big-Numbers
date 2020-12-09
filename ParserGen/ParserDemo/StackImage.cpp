#include "stdafx.h"
#include <MFCUtil/ColorSpace.h>
#include "StackImage.h"


String StackImageElement::toString() const {
  const TCHAR *symbolString = m_node ? m_node->getSymbol() : EMPTYSTRING;
  return format(_T("%4d (%3d,%2d) %s")
                , m_elem.m_state, m_elem.m_pos.getLineNumber(), m_elem.m_pos.getColumn()
                , symbolString);
}

StackImage::StackImage()
  : m_wnd(       nullptr)
  , m_parser(    nullptr)
  , m_winSize(   0,0    )
  , m_paintLevel(0      )
{
}

StackImage::~StackImage() {
  destroyWorkBitmap();
}

void StackImage::init(CStatic *wnd, TestParser &parser) {
  m_wnd    = wnd;
  m_parser = &parser;
  m_font.CreateFont(12, 10, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS
                   , CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY
                   , DEFAULT_PITCH | FF_MODERN
                   , _T("Courier"));

  TEXTMETRIC tm;
  CClientDC dc(m_wnd);
  dc.SelectObject(m_font);
  dc.GetTextMetrics(&tm);
  m_charSize = CSize(tm.tmMaxCharWidth, tm.tmHeight);
  OnSize();
}

bool StackImage::createWorkBitmap() {
  const CSize newSize = getClientRect(m_wnd).Size();
  if((newSize == m_winSize) && (m_workBitmap.m_hObject != nullptr)) {
    return false;
  }
  destroyWorkBitmap();
  CClientDC   screen(m_wnd);
  m_winSize = newSize;
  m_workDC.CreateCompatibleDC(&screen);
  m_workBitmap.CreateBitmap(newSize.cx,newSize.cy,screen.GetDeviceCaps(PLANES),screen.GetDeviceCaps(BITSPIXEL),nullptr);
  m_workBitmap.SetBitmapDimension(newSize.cx,newSize.cy);
  m_workDC.SelectObject(m_workBitmap);
  m_workDC.SelectObject(m_font);

  m_maxChars = CSize((newSize.cx - 4)/m_charSize.cx, newSize.cy/m_charSize.cy);
  setAllWhite();
  return true;
}

void StackImage::destroyWorkBitmap() {
  if(m_workBitmap.m_hObject != nullptr) {
    m_workBitmap.DeleteObject();
  }
  if(m_workDC.m_hDC != nullptr) {
    m_workDC.DeleteDC();
  }
}

void StackImage::OnSize() {
  if(!createWorkBitmap()) {
    return;
  }
  repaintAll();
}

void StackImage::endPaint() {
  if(--m_paintLevel == 0) {
    flushImage();
  }
}

void StackImage::flushImage() {
  CClientDC dc(m_wnd);
  dc.BitBlt(0, 0, m_winSize.cx, m_winSize.cy, &m_workDC, 0, 0, SRCCOPY);
}

inline bool operator==(const ParserStackElement &e1, const ParserStackElement &e2) {
  return (e1.m_state == e2.m_state) && (e1.m_symbol == e2.m_symbol);
}
inline bool operator!=(const ParserStackElement &e1, const ParserStackElement &e2) {
  return !(e1 == e2);
}

void StackImage::updateImage() {
  const int ph = m_parser->getStackHeight();

  startPaint();

  int dh = ph - m_stack.getHeight();
  
  while(dh < 0) { // parserstak is lower than private stack. Remove extra top-elements
    pop();
    dh++;
  }
  // then remove top-elements from local stack which doesn't match parsers stack
  while(!m_stack.isEmpty() && (dh < ph) && (m_stack.top().getElement() != m_parser->getStackTopElement(dh))) {
    pop();
    dh++;
  }
  while(dh > 0) { // then push the missing elements to make them equal
    push(--dh);
  }

  endPaint();
}

void StackImage::push(UINT fromTop) {
  const ParserStackElement &elem = m_parser->getStackTopElement(fromTop);
  const SyntaxNodep         n    = (fromTop+1 >= m_parser->getStackHeight()) ? nullptr : m_parser->getStackTop(fromTop);
  m_stack.push(StackImageElement(elem, n));
  paintTopElement();
}

void StackImage::pop() {
  if(m_stack.isEmpty()) {
    return;
  }
  clearTopElement();
  m_stack.pop();
}

#define POSY(y)     ((y)*m_charSize.cy)

void StackImage::clearTopElement() {
  const StackImageElement &top = m_stack.top();
  const UINT               y   = POSY(m_stack.getHeight());
  startPaint();
  m_workDC.FillSolidRect(3, y, m_winSize.cx-3, m_charSize.cy, WHITE);
  endPaint();
}

void StackImage::paintTopElement() {
  if(m_stack.isEmpty()) {
    return;
  }
  const StackImageElement &top = m_stack.top();
  const UINT y = POSY(m_stack.getHeight());
  startPaint();
  textOut(m_workDC, 3, y, left(top.toString(), m_maxChars.cx));
  endPaint();
}

void StackImage::repaintAll() {
  startPaint();
  setAllWhite();
  const UINT maxElement = min(m_parser->getStackHeight(), (UINT)m_maxChars.cy);
  m_stack.clear();
  for(int i = maxElement - 1; i >= 0; i--) {
    push(i);
  }
  endPaint();
}

void StackImage::setAllWhite() {
  startPaint();
  m_workDC.FillSolidRect(0,0,m_winSize.cx,m_winSize.cy, WHITE);
  endPaint();
}

int StackImage::findStackIndexFromTop(const CPoint &p) const {
  if(m_stack.isEmpty()) {
    return -1;
  }
  const int stackBottomPos = POSY(1);
  const int stackTopPos    = POSY(m_stack.getHeight()+1);
  if((p.y >= stackBottomPos) && (p.y <= stackTopPos)) {
    return (stackTopPos - p.y) / m_charSize.cy;
  }
  return -1;
}
