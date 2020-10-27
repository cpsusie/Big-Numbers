#include "stdafx.h"
#include <assert.h>
#include "DrawTool.h"

ChangeHueTool::ChangeHueTool(PixRectContainer *container) : DrawTool(container) {
  m_dragRectVisible = false;
}

ChangeHueTool::~ChangeHueTool() {
  if(m_dragRectVisible) {
    invertDragRect();
  }
}

void ChangeHueTool::invertDragRect() {
  static const CSize size(1,1);
  if(m_dragRectVisible) {
    getImage()->drawDragRect(&m_lastDragRect ,CSize(0,0), &m_lastDragRect, size);
  } else {
    m_lastDragRect = CRect(m_p0, m_p1);
    getImage()->drawDragRect(&m_lastDragRect, size, nullptr, size);
  }

//  getImage()->rectangle(CRect(m_p0,m_p1),0,true);

  m_dragRectVisible = !m_dragRectVisible;

  repaint();
}

void ChangeHueTool::OnLButtonDown(UINT nFlags, const CPoint &point) {
  if(m_dragRectVisible) {
    invertDragRect();
  }
  m_p0 = m_p1 = point;
  invertDragRect();
  assert(m_dragRectVisible);
}

void ChangeHueTool::OnMouseMove(UINT nFlags, const CPoint &point) {
  if(nFlags & MK_LBUTTON) {
    assert(m_dragRectVisible);
    invertDragRect(); // remove dragrect
    m_p1 = point;
    invertDragRect(); // draw dragRect
    assert(m_dragRectVisible);
  }
}

ChangeLSHOperator::ChangeLSHOperator(PixRect *pixRect, float deltaHue, float deltaSaturation, float deltaLumination)
: PixRectOperator(pixRect)
, m_deltaHue(deltaHue)
, m_deltaSat(deltaSaturation)
, m_deltaLum(deltaLumination)  {
}

void ChangeLSHOperator::apply(const CPoint &p) {
  const D3DCOLOR c = m_pixelAccessor->getPixel(p);
  LSHColor lsh = getLSHColor(c);
  const LSHColor origLSH = lsh;

  if(m_deltaHue != 0) {
    lsh.m_hue += m_deltaHue;
    if(lsh.m_hue > 1) {
      lsh.m_hue -= 1;
    } else if(lsh.m_hue < 0) {
      lsh.m_hue += 1;
    }
  }
  if(m_deltaSat > 0) {
    lsh.m_saturation += (1-lsh.m_saturation) * m_deltaSat;
  } else if(m_deltaSat < 0) {
    lsh.m_saturation += lsh.m_saturation * m_deltaSat;
  }
  lsh.m_saturation = minMax(lsh.m_saturation,0.0f,1.0f);
  if(m_deltaLum > 0) {
    lsh.m_lumination += (1-lsh.m_lumination) * m_deltaLum;
  } else if(m_deltaLum < 0) {
    lsh.m_lumination += lsh.m_lumination * m_deltaLum;
  }
  lsh.m_lumination = minMax(lsh.m_lumination,0.0f,1.0f);
  const D3DCOLOR c1 = getRGBColor(lsh);
  m_pixelAccessor->setPixel(p, c1);
}

bool ChangeHueTool::changeHue(PixRect *pr) { // static
  D3DCOLOR oldColor = pr->getAverageColor();
  CColorDialog dlg;
  dlg.m_cc.Flags |= CC_RGBINIT | CC_FULLOPEN;
  dlg.m_cc.rgbResult = D3DCOLOR2COLORREF(oldColor);
  if(dlg.DoModal() == IDOK) {
    const D3DCOLOR newColor = COLORREF2D3DCOLOR(dlg.m_cc.rgbResult);
    const LSHColor oldLSH = getLSHColor(oldColor);
    const LSHColor newLSH = getLSHColor(newColor);
    const float deltaHue = newLSH.m_hue - oldLSH.m_hue;
    if(deltaHue != 0) {
      pr->apply(ChangeLSHOperator(pr, deltaHue, 0, 0));
      return true;
    }
  }
  return false;
}

void ChangeHueTool::OnLButtonUp(UINT nFlags, const CPoint &point) {
  if(m_dragRectVisible && (m_p0 != m_p1)) {
    invertDragRect();   // remove dragrect
    const CRect rect(min(m_p0.x, m_p1.x), min(m_p0.y, m_p1.y), max(m_p0.x, m_p1.x), max(m_p0.y, m_p1.y));
    if(rect.Width() > 0 && rect.Height() > 0) {
      PixRect *copy = theApp.fetchPixRect(rect.Size());
      copy->rop(ORIGIN, rect.Size(), SRCCOPY, getImage(), rect.TopLeft());
      if(changeHue(copy)) {
        m_container->saveDocState();
        getImage()->rop(rect.TopLeft(),rect.Size(),SRCCOPY, copy, ORIGIN);
        repaint();
      }
      SAFEDELETE(copy);
    }
  } else if(m_dragRectVisible) {
    invertDragRect(); // remove dragrect
  }
}
