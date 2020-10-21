#include "pch.h"

CView *_createView(CFrameWnd *frameWnd, CWnd *parent, CRuntimeClass *viewClass, const CRect &rect) {
  CCreateContext context;

  // Create new view and redraw.
  context.m_pNewViewClass   = viewClass;
  context.m_pCurrentDoc     = nullptr;
  context.m_pNewDocTemplate = nullptr;
  context.m_pLastView       = nullptr;
  context.m_pCurrentFrame   = frameWnd;

  CView *newView = (CView*)viewClass->CreateObject();

  if(newView == nullptr) {
    TRACE1("Warning: Dynamic create of view type %Fs failed\n",
      viewClass->m_lpszClassName);
    return nullptr;
  }

  if(!newView->Create(nullptr, nullptr, AFX_WS_DEFAULT_VIEW,
    rect, parent, AFX_IDW_PANE_FIRST, &context))
  {
    TRACE0("Warning: couldn't create view for frame\n");
    return nullptr; // Programmer can assume FALSE return value
                 // from this function means that there
                 // isn't a view.
  }

  // WM_INITIALUPDATE is define in AFXPRIV.H.
  newView->SendMessage(WM_INITIALUPDATE, 0, 0);

  return newView;
}
