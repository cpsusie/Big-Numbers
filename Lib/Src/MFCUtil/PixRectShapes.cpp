#include "pch.h"
#include <MFCUtil/PixRect.h>

void PixRect::line(int x1, int y1, int x2, int y2, D3DCOLOR color, bool invert) {
  line(CPoint(x1,y1), CPoint(x2,y2), color, invert);
}

void PixRect::line(const CPoint &p1, const CPoint &p2, D3DCOLOR color, bool invert) {
  if(invert) {
    applyToLine(p1,p2,InvertColor(this));
  } else {
    applyToLine(p1,p2,SetColor(color, this));
  }
}

void PixRect::rectangle(int x1, int y1, int x2, int y2, D3DCOLOR color, bool invert) {
  rectangle(CRect(x1,y1,x2,y2),color,invert);
}

void PixRect::rectangle(const CPoint &p0, const CSize &size, D3DCOLOR color, bool invert) {
  rectangle(CRect(p0,size),color,invert);
}

void PixRect::rectangle(const CRect &rect, D3DCOLOR color, bool invert) {
  if(invert) {
    applyToRectangle(rect, InvertColor(this));
  } else {
    applyToRectangle(rect, SetColor(color, this));
  }
}

void PixRect::drawDragRect(CRect *newRect, const CSize &newSize, CRect *lastRect, const CSize &lastSize) {
  HDC hdc = getDC();
  CDC *dc = CDC::FromHandle(hdc);
  CRect tmp;
  if(lastRect) {
    tmp = makePositiveRect(*lastRect);
  }
  dc->DrawDragRect(&makePositiveRect(newRect), newSize,(lastRect!=NULL) ? tmp : NULL, lastSize);
  releaseDC(hdc);
}

void PixRect::polygon(const MyPolygon &polygon, D3DCOLOR color, bool invert, bool closingEdge) {
  if(invert) {
    polygon.applyToEdge(InvertColor(this), closingEdge);
  } else {
    polygon.applyToEdge(SetColor(color, this), closingEdge);
  }
}

void PixRect::ellipse(const CRect &rect, D3DCOLOR color, bool invert) {
  if(invert) {
    applyToEllipse(rect,InvertColor(this));
  } else {
    applyToEllipse(rect,SetColor(color, this));
  }
}

void PixRect::bezier(const Point2D &start, const Point2D &cp1, const Point2D &cp2, const Point2D &end, D3DCOLOR color, bool invert) {
  applyToBezier(start,cp1,cp2,end,PixRectTextMaker(this,Point2D(0,0), color,invert));
}

void PixRect::fillRect(const CRect &rect, D3DCOLOR color, bool invert) {
  if(rect.Width() == 0 || rect.Height() == 0) {
    return;
  }
  CRect dstRect = makePositiveRect(rect);
  HDC hdc = getDC();
  try {
    if (invert) {
      BitBlt( hdc, dstRect.left, dstRect.top, dstRect.Width(), dstRect.Height(), NULL, 0, 0, DSTINVERT);
    }
    else {
      HBRUSH brush = CreateSolidBrush(D3DCOLOR2COLORREF(color));
      FillRect(hdc, &dstRect, brush);
      DeleteObject(brush);
    }
    releaseDC(hdc);
  }
  catch (...) {
    releaseDC(hdc);
    throw;
  }
}

void PixRect::fillRect(const CPoint &p0, const CSize &size, D3DCOLOR color, bool invert) {
  fillRect(CRect(p0,size),color,invert);
}

void PixRect::fillRect(int x1, int y1, int x2, int y2, D3DCOLOR color, bool invert) {
  fillRect(CRect(x1,y1,x2-x1,y2-y1),color,invert);
}

void PixRect::replicate(int x, int y, int w, int h, const PixRect *src) {
  if(x < 0) {
    w += x;
    x = 0;
  }
  if(y < 0) {
    h += y;
    y = 0;
  }
  if(x >= getWidth() || y >= getHeight()) {
    return;
  }
  w = min(w,getWidth()-x);
  h = min(h,getHeight()-y);
  if(w == 0 || h == 0) {
    return;
  }
  for(int r = 0; r < h; r += src->getHeight()) {
    for(int c = 0; c < w; c += src->getWidth()) {
      rop(x+c,y+r,src->getWidth(),src->getHeight(),SRCCOPY, src,0,0);
    }
  }
}

PixRect &PixRect::apply(PixRectOperator &op) {
  op.setPixRect(this);
  applyToFullRectangle(CRect(CPoint(0,0), getSize()), op);
  op.setPixRect(NULL);
  return *this;
}

PixRect &PixRect::apply(PixRectFilter &filter) {
  filter.setPixRect(this);
  applyToFullRectangle(filter.getRect(),filter);
  if(filter.m_result != this) {
    rop(0,0,getWidth(),getHeight(),SRCCOPY,filter.m_result,0,0);
  }
  filter.setPixRect(NULL);
  return *this;
}


void PixRect::fillEllipse(const CRect &rect, D3DCOLOR color, bool invert) {
  CRect r = rect;
  if((rect.Width() == 0) || (rect.Height() == 0)) {
    return;
  }
  r -= rect.TopLeft();
  PixRect *psrc  = new PixRect(m_device, getType(), r.Size(), getPool(), getPixelFormat());
  PixRect *pmask = new PixRect(m_device, getType(), r.Size(), getPool(), getPixelFormat());
  psrc->fillRect(0,0,r.Width(),r.Height(),color);
  pmask->fillRect(0,0,r.Width(),r.Height(),BLACK);  // set mask to black
  pmask->ellipse(r,WHITE);                          // draw white ellipse on mask
  pmask->fill(r.CenterPoint(),WHITE);

//  rop(rect.left,rect.top,rect.Width(),rect.Height(),DSTINVERT,NULL,0,0);
//  mask(rect.left,rect.top,rect.Width(),rect.Height(), MAKEROP4(SRCCOPY,DSTINVERT), psrc, 0,0, pmask);
  mask(rect.left,rect.top,rect.Width(),rect.Height(), SRCCOPY, psrc, 0,0, pmask);

  delete pmask;
  delete psrc;
}
