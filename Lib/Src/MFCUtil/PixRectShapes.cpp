#include "stdafx.h"
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

void applyToLine(const CPoint &p1, const CPoint &p2, PointOperator &op) {
  applyToLine(p1.x,p1.y,p2.x,p2.y,op);
}

void applyToLine(int x1, int y1, int x2, int y2, PointOperator &op) {
  if(x1 == x2) {
    if(y1 > y2) {
      swap(y1,y2);
    }
    for(CPoint p = CPoint(x1,y1); p.y <= y2; p.y++) {
      op.apply(p);
    }
  } else if(y1 == y2) {
    if(x1 > x2) {
      swap(x1,x2);
    }
    for(CPoint p = CPoint(x1,y1); p.x <= x2; p.x++) {
      op.apply(p);
    }
  } else {
    const int vx = x2 - x1;
    const int vy = y2 - y1;
    const int dx = sign(vx);
    const int dy = sign(vy);
    for(CPoint p = CPoint(x1,y1), p1 = CPoint(p.x-x1,p.y-y1);;) {
      op.apply(p);
      if(p.x == x2 && p.y == y2) {
        break;
      }
      if(abs((p1.x+dx)*vy - p1.y*vx) < abs(p1.x*vy - (p1.y+dy)*vx)) {
        p.x  += dx;
        p1.x += dx;
      } else {
        p.y  += dy;
        p1.y += dy;
      }
    }
  }
}

void applyToRectangle(const CRect &rect, PointOperator &op) {
  const CRect r = makePositiveRect(rect);
  if(r.left == r.right) {
    if(r.top == r.bottom) {
      op.apply(r.TopLeft());                                // rect contains only 1 pixel
    } else {
      applyToLine(r.left , r.top, r.left  , r.bottom , op); // rect is a vertical line
    }
  } else if(r.top == r.bottom) {
    applyToLine(r.left , r.top   , r.right, r.top    , op); // rect is a horizontal line
  } else {                                                  // rect is a "real" rectangle
    applyToLine(r.left , r.top   , r.right-1, r.top     , op); 
    applyToLine(r.right, r.top   , r.right  , r.bottom-1, op);
    applyToLine(r.right, r.bottom, r.left+1 , r.bottom  , op);
    applyToLine(r.left , r.bottom, r.left   , r.top+1   , op);
  }
}

void applyToFullRectangle(const CRect &rect, PointOperator &op) {
  const CRect r = makePositiveRect(rect);
  CPoint p;
  for(p.y = r.top; p.y < r.bottom; p.y++) {
    for(p.x = r.left; p.x < r.right; p.x++) {
      op.apply(p);
    }
  }
}

static void applyToEllipsePart(const CPoint &start, const CPoint &end, const CPoint &center, PointOperator &op) {
  const int    minx = min(start.x,end.x);
  const int    miny = min(start.y,end.y);
  const int    maxx = max(start.x,end.x);
  const int    maxy = max(start.y,end.y);
  const int    dx   = sign(end.x - start.x);
  const int    dy   = sign(end.y - start.y);
  const int    rx   = max(abs(start.x-center.x),abs(end.x-center.x));
  const int    ry   = max(abs(start.y-center.y),abs(end.y-center.y));
  const double rx2  = rx*rx;
  const double ry2  = ry*ry;
  const double rxy2 = rx2*ry2;

  if(rx == 0 || ry == 0) {
    return;
  }

  CPoint p  = start;
  CPoint pc = p - center;
  while(minx <= p.x && p.x <= maxx && miny <= p.y && p.y <= maxy) {
    if(p == end) {
      break;
    }
    op.apply(p);
    const double d1 = fabs(ry2*sqr(pc.x+dx) + rx2*sqr(pc.y   ) - rxy2);
    const double d2 = fabs(ry2*sqr(pc.x   ) + rx2*sqr(pc.y+dy) - rxy2);
    if(d1 < d2) {
      p.x  += dx;
      pc.x += dx;
    } else {
      p.y  += dy;
      pc.y += dy;
    }
  }
}

void applyToEllipse(const CRect &rect, PointOperator &op) {
  const CRect r = makePositiveRect(rect);
  const CPoint center = r.CenterPoint();
  applyToEllipsePart(CPoint(center.x,r.top   ),CPoint(r.right,center.y ),center, op);
  applyToEllipsePart(CPoint(r.right,center.y ),CPoint(center.x,r.bottom),center, op);
  applyToEllipsePart(CPoint(center.x,r.bottom),CPoint(r.left,center.y  ),center, op);
  applyToEllipsePart(CPoint(r.left,center.y  ),CPoint(center.x,r.top   ),center, op);
}

void applyToBezier(const Point2D &start, const Point2D &cp1, const Point2D &cp2, const Point2D &end, CurveOperator &op, bool applyStart) {
  Point2D p = start;
  if(applyStart) {
    op.apply(start);
  }
  for(float t = 0.1f; t < 1.01; t += 0.1f) {
    float tt = t*t;
    float s  = 1.0f - t;
    float ss = s*s;
    Point2D np = start*(ss*s) + cp1*(3*ss*t) + cp2*(3*s*tt) + end*(t*tt);
    op.apply(np);
    p = np;
  }
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
