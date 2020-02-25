#include "stdafx.h"
#include "MainFrm.h"

CView *createView(CWnd *parent, CRuntimeClass *viewClass, const CRect &rect) {
  CMainFrame    *mainFrame = theApp.getMainFrame();
  CCreateContext context;
  CView         *currentView;

  // If no active view for the frame, return FALSE because this
  // function retrieves the current document from the active view.
  if((currentView = mainFrame->GetActiveView()) == NULL) {
    return FALSE;
  }

  // If you're already displaying this kind of view, no need to go
  // further.
  if(currentView->IsKindOf(viewClass))
    return NULL;

  // Get pointer to CDocument object so that it can be used in the
  // creation process of the new view.
  CDocument *pDoc = currentView->GetDocument();

  // Create new view and redraw.
  context.m_pNewViewClass   = viewClass;
  context.m_pCurrentDoc     = NULL;
  context.m_pNewDocTemplate = NULL;
  context.m_pLastView       = NULL;
  context.m_pCurrentFrame   = mainFrame;

  CView *newView = (CView*)viewClass->CreateObject();

  if(newView == NULL) {
    TRACE1("Warning: Dynamic create of view type %Fs failed\n",
      viewClass->m_lpszClassName);
    return NULL;
  }

  if(!newView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
    rect, parent, AFX_IDW_PANE_FIRST, &context))
  {
    TRACE0("Warning: couldn't create view for frame\n");
    return NULL; // Programmer can assume FALSE return value
                 // from this function means that there
                 // isn't a view.
  }

  // WM_INITIALUPDATE is define in AFXPRIV.H.
  newView->SendMessage(WM_INITIALUPDATE, 0, 0);

  return newView;
}
