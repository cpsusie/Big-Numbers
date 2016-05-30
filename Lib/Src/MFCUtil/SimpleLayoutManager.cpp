#include "stdafx.h"
#include <MyUtil.h>
#include <MFCUtil/WinTools.h>

#pragma warning(disable : 4244)

DEFINECLASSNAME(SimpleLayoutManager);

SimpleLayoutManager::~SimpleLayoutManager() {
  for(int i = 0; i < m_attributes.size(); i++) {
    delete m_attributes[i];
  }
  m_attributes.clear();
}

#define ALL_CHILDFONTFLAGS (RESIZE_FONT|FONT_RELATIVE_SIZE|FONT_RELATIVE_POSITION)

void SimpleLayoutManager::addControl(int ctrlId, int flags) {
  CWnd *wnd = getWindow();
  if(wnd == NULL) return;
  CWnd *ctrl = wnd->GetDlgItem(ctrlId);
  if(ctrl == NULL) {
    AfxMessageBox(format(_T("%s:Control %d not found in window"), s_className, ctrlId).cstr(), MB_ICONSTOP); 
    return;
  }

// Only one bit of each group may be set.
// RELATIVE_LEFT    , PCT_RELATIVE_LEFT    , PCT_RELATIVE_X_CENTER
// RELATIVE_TOP     , PCT_RELATIVE_TOP     , PCT_RELATIVE_Y_CENTER
// RELATIVE_RIGHT   , PCT_RELATIVE_RIGHT   , PCT_RELATIVE_X_CENTER  
// RELATIVE_BOTTOM  , PCT_RELATIVE_BOTTOM  , PCT_RELATIVE_Y_CENTER 

#define ANYSET( flags, bits) (( (flags)&(bits)) != 0)
#define MANYSET(flags, bits) ((((flags)&(bits)) & (((flags)&(bits))-1)) != 0)
#define ONESET( flags, bits) (ANYSET(flags,bits) && !MANYSET(flags, bits))

#define LEFT_EDGE_BITS       (RELATIVE_LEFT   | PCT_RELATIVE_LEFT   | PCT_RELATIVE_X_CENTER)
#define TOP_EDGE_BITS        (RELATIVE_TOP    | PCT_RELATIVE_TOP    | PCT_RELATIVE_Y_CENTER)
#define RIGHT_EDGE_BITS      (RELATIVE_RIGHT  | PCT_RELATIVE_RIGHT  | PCT_RELATIVE_X_CENTER)
#define BOTTOM_EDGE_BITS     (RELATIVE_BOTTOM | PCT_RELATIVE_BOTTOM | PCT_RELATIVE_Y_CENTER)

  if(MANYSET(flags, LEFT_EDGE_BITS) || MANYSET(flags, TOP_EDGE_BITS) || MANYSET(flags, RIGHT_EDGE_BITS) || MANYSET(flags, RIGHT_EDGE_BITS)) {
    AfxMessageBox(format(_T("%s:Invalid bit-combination for ctrlId %d:%s"), s_className, ctrlId, sprintbin((char)flags).cstr()).cstr(), MB_ICONSTOP);
    return;
  }
  if((flags & CONSTANT_WIDTH) && ANYSET(flags, LEFT_EDGE_BITS)) {
    flags &= ~RIGHT_EDGE_BITS ;
  }
  if((flags & CONSTANT_HEIGHT) && ANYSET(flags, TOP_EDGE_BITS)) {
    flags &= ~BOTTOM_EDGE_BITS ;
  }

  if(flags & INIT_LISTHEADERS) {
    filloutListHeaderWidth(*(CListCtrl*)ctrl);
  }

#define WIDTH_CHANGING(flags) (ONESET(flags, RELATIVE_LEFT|RELATIVE_RIGHT) || ANYSET(flags, PCT_RELATIVE_LEFT|PCT_RELATIVE_RIGHT))

  if(WIDTH_CHANGING(flags) && (flags & RESIZE_LISTHEADERS)) {
    m_attributes.add(new ListCtrlLayoutAttribute(this, ctrlId, flags));
  } else {
    m_attributes.add(new LayoutAttribute(this, ctrlId, flags));
  }
}

bool SimpleLayoutManager::isAnyChildrenChanged() const {
  for(int i = 0; i < m_attributes.size(); i++) {
    if(m_attributes[i]->isControlWindowChanged()) {
      const LayoutAttribute *attr = m_attributes[i];
      return true;
    }
  }
  return false;
}

void SimpleLayoutManager::OnSize(UINT nType, int cx, int cy) {
  if(!isInitialized()) return;
  if(nType == SIZE_MINIMIZED) return;

  if(isAnyChildrenChanged()) {
    for(int i = 0; i < m_attributes.size(); i++) {
      m_attributes[i]->resetStartRect();
    }
    resetWinStartSize();
  } else {
    const CSize currentWinSize = getClientRect(getWindow()).Size();
    for(int i = 0; i < m_attributes.size(); i++) {
      updateChildRect(*m_attributes[i], currentWinSize);
    }
  }
}

void SimpleLayoutManager::updateChildRect(LayoutAttribute &attr, const CSize &currentWinSize) {
  CWnd *ctrl = getChild(attr.getCtrlId());

  if((ctrl == NULL) /*|| !ctrl->IsWindowVisible()*/) {
    return;
  }
  const CSize &winStartSize = getWinStartSize();
  const int    flags        = attr.getFlags();
  const CRect  origRect     = attr.getStartRect();
  CRect        ctrlRect     = origRect;

  switch(flags & LEFT_EDGE_BITS) {
  case 0                    : // keep left edge of ctrl constant
    break;

  case RELATIVE_LEFT        : // adjust left edge of ctrl with the amount as mainwindow size.
    { const int dx = currentWinSize.cx - winStartSize.cx;
      ctrlRect.left += dx;
      if(flags & CONSTANT_WIDTH) ctrlRect.right += dx;
    }
    break;

  case PCT_RELATIVE_LEFT    :
    if(winStartSize.cx != 0) {
      const int dx = (double)ctrlRect.left / winStartSize.cx * currentWinSize.cx - ctrlRect.left;
      ctrlRect.left += dx;
      if(flags & CONSTANT_WIDTH) ctrlRect.right += dx;
    }
    break;

  case PCT_RELATIVE_X_CENTER:
    if(winStartSize.cx != 0) {
      const int dx =  (double)ctrlRect.CenterPoint().x / winStartSize.cx * currentWinSize.cx - ctrlRect.CenterPoint().x;
      ctrlRect.left  += dx;
      ctrlRect.right += dx;
    }
    break;
  }

  switch(flags & TOP_EDGE_BITS) {
  case 0                    : // keep top of ctrl constant
    break;

  case RELATIVE_TOP         : // adjust top edge of ctrl with the amount as mainwindow size.
    { const int dy = currentWinSize.cy - winStartSize.cy;
      ctrlRect.top  += dy;
      if(flags & CONSTANT_HEIGHT) ctrlRect.bottom += dy;
    }
    break;

  case PCT_RELATIVE_TOP     :
    if(winStartSize.cy != 0) {
      const int dy =  (double)ctrlRect.top / winStartSize.cy * currentWinSize.cy - ctrlRect.top;
      ctrlRect.top  += dy;
      if(flags & CONSTANT_HEIGHT) ctrlRect.bottom += dy;
    }
    break;

  case PCT_RELATIVE_Y_CENTER:
    if(winStartSize.cy != 0) {
      const int dy = (double)ctrlRect.CenterPoint().y / winStartSize.cy * currentWinSize.cy - ctrlRect.CenterPoint().y;
      ctrlRect.top    += dy;
      ctrlRect.bottom += dy;
    }
    break;
  }

  switch(flags & (RELATIVE_RIGHT | PCT_RELATIVE_RIGHT)) {
  case 0                    :
    break;

  case RELATIVE_RIGHT       :
    { const int dx = currentWinSize.cx - winStartSize.cx;
      ctrlRect.right += dx;
      if(flags & CONSTANT_WIDTH) ctrlRect.left += dx;
    }
    break;

  case PCT_RELATIVE_RIGHT   :
    if(winStartSize.cx != 0) {
      const int dx = (double)ctrlRect.right / winStartSize.cx * currentWinSize.cx - ctrlRect.right;
      ctrlRect.right += dx;
      if(flags & CONSTANT_WIDTH) ctrlRect.left += dx;
    }
    break;
  }

  switch(flags & (RELATIVE_BOTTOM | PCT_RELATIVE_BOTTOM)) {
  case 0                      :
    break;

  case RELATIVE_BOTTOM      :
    { const int dy = currentWinSize.cy - winStartSize.cy;
      ctrlRect.bottom += dy;
      if(flags & CONSTANT_HEIGHT) ctrlRect.top += dy;
    }
    break;

  case PCT_RELATIVE_BOTTOM  :
    if(winStartSize.cy != 0) {
      const int dy = (double)ctrlRect.bottom / winStartSize.cy * currentWinSize.cy - ctrlRect.bottom;
      ctrlRect.bottom += dy;
      if(flags & CONSTANT_HEIGHT) ctrlRect.top += dy;
    }
    break;
  }
  if(ctrlRect != origRect) {
    attr.setControlRect(ctrlRect);
  }
}

void SimpleLayoutManager ::scaleFont(double scale, bool redraw) {
  if(!isInitialized()) return;

  const int flags = getFlags();
  if(flags & (RESIZE_FONT|FONT_RELATIVE_SIZE)) {
    setFontScale(scale, (flags & FONT_RELATIVE_SIZE) ? true : false, redraw);
  }
//if(m_flags & RESIZE_MENUFONT) {
//  CMenu *menu = getWindow()->GetMenu();
//}
  const CSize currentWinSize = getClientRect(getWindow()).Size();
  for(int i = 0; i < m_attributes.size(); i++) {
    LayoutAttribute &attr = *m_attributes[i];
    if(attr.getFlags() & ALL_CHILDFONTFLAGS) {
      updateFontScale(attr, scale, redraw, currentWinSize);
    }
  }
}

void SimpleLayoutManager ::updateFontScale(LayoutAttribute &attr, double scale, bool redraw, const CSize &currentWinSize) {
  const int flags = getFlags(); // parent
  attr.setFontScale(scale);
  getChild(attr.getCtrlId())->SetFont(attr.getFont(), redraw);
  if(attr.getFlags() & FONT_RELATIVE_SIZE) {
    updateChildRect(attr, currentWinSize);
  }
}
