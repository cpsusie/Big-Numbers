#include "stdafx.h"
#include "PrShow.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_WM_CREATE()
    ON_COMMAND(ID_SCROLL_LINE_DOWN  ,OnScrollLineDown )
    ON_COMMAND(ID_SCROLL_LINE_UP    ,OnScrollLineUp   )
    ON_COMMAND(ID_SCROLL_PAGE_DOWN  ,OnScrollPageDown )
    ON_COMMAND(ID_SCROLL_PAGE_UP    ,OnScrollPageUp   )
    ON_COMMAND(ID_SCROLL_LEFT       ,OnScrollLeft     )
    ON_COMMAND(ID_SCROLL_RIGHT      ,OnScrollRight    )
    ON_COMMAND(ID_SCROLL_PAGE_LEFT  ,OnScrollPageLeft )
    ON_COMMAND(ID_SCROLL_PAGE_RIGHT ,OnScrollPageRight)
    ON_COMMAND(ID_SCROLL_TO_TOP     ,OnScrollToTop    )
    ON_COMMAND(ID_SCROLL_TO_BOTTOM  ,OnScrollToBottom )
    ON_COMMAND(ID_SCROLL_TO_LEFT    ,OnScrollToLeft   )
    ON_COMMAND(ID_SCROLL_TO_RIGHT   ,OnScrollToRight  )
    ON_COMMAND(ID_FILE_EXIT, OnFileExit)
    ON_COMMAND(ID_HELP_ABOUTPRSHOW, OnHelpAboutprshow)
    ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

static UINT indicators[] = {
  ID_SEPARATOR,           // status line indicator
  ID_INDICATOR_CAPS,
  ID_INDICATOR_NUM,
  ID_INDICATOR_SCRL,
};

CMainFrame::CMainFrame() {
}

CMainFrame::~CMainFrame() {
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if(__super::OnCreate(lpCreateStruct) == -1) {
    return -1;
  }

  if(!m_wndStatusBar.Create(this) ||!m_wndStatusBar.SetIndicators(indicators,sizeof(indicators)/sizeof(UINT))) {
    TRACE0("Failed to create status bar\n");
    return -1;      // fail to create
  }

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));

  return 0;
}

BOOL CMainFrame::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

#ifdef _DEBUG
void CMainFrame::AssertValid() const {
  __super::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const {
  __super::Dump(dc);
}

#endif //_DEBUG

CImageDoc *CMainFrame::getDocument() {
  CImageView *view = getView();
  return view ? view->GetDocument() : NULL;
}

BOOL CMainFrame::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
  if(nFlags & MK_SHIFT) {
    if(zDelta > 0) {
      OnScrollLeft();
    } else {
      OnScrollRight();
    }
  } else if(nFlags & MK_CONTROL) {
    getView()->zoom(zDelta > 0);
  } else {
    return __super::OnMouseWheel(nFlags, zDelta, pt);
  }
  return TRUE;
}

CPoint CMainFrame::getMaxScroll() {
  CRect r;
  getView()->GetClientRect(&r);
  CSize rectSize = r.Size();
  CSize docSize = getDocument()->getSize();
  double zoom = getView()->getCurrentZoomFactor();
  return CPoint(max(0,(int)(docSize.cx*zoom)-rectSize.cx),max(0,(int)(docSize.cy*zoom)-rectSize.cy));
}

void CMainFrame::scroll(int dx, int dy) {
  CPoint topLeft   = getView()->GetScrollPosition();
  CPoint maxScroll = getMaxScroll();
  int newX = minMax(topLeft.x+dx,0,maxScroll.x);
  int newY = minMax(topLeft.y+dy,0,maxScroll.y);

  getView()->ScrollToPosition(CPoint(newX,newY));
}

void CMainFrame::OnScrollLineDown() {
  scroll(0,20);
}

void CMainFrame::OnScrollLineUp() {
  scroll(0,-20);
}

void CMainFrame::OnScrollPageDown() {
  CRect r;
  getView()->GetClientRect(&r);
  scroll(0,r.Height());
}

void CMainFrame::OnScrollPageUp() {
  CRect r;
  getView()->GetClientRect(&r);
  scroll(0,-r.Height());
}

void CMainFrame::OnScrollLeft() {
  scroll(-20,0);
}

void CMainFrame::OnScrollRight() {
  scroll(20,0);
}

void CMainFrame::OnScrollPageLeft() {
  CRect r;
  getView()->GetClientRect(&r);
  scroll(-r.Width(),0);
}

void CMainFrame::OnScrollPageRight() {
  CRect r;
  getView()->GetClientRect(&r);
  scroll(r.Width(),0);
}

void CMainFrame::OnScrollToTop() {
  CPoint topLeft = getView()->GetScrollPosition();
  getView()->ScrollToPosition(CPoint(topLeft.x,0));
}

void CMainFrame::OnScrollToBottom() {
  CPoint topLeft = getView()->GetScrollPosition();
  getView()->ScrollToPosition(CPoint(topLeft.x,getMaxScroll().y));
}

void CMainFrame::OnScrollToLeft() {
  CPoint topLeft = getView()->GetScrollPosition();
  getView()->ScrollToPosition(CPoint(0,topLeft.y));
}

void CMainFrame::OnScrollToRight() {
  CPoint topLeft = getView()->GetScrollPosition();
  getView()->ScrollToPosition(CPoint(getMaxScroll().x,topLeft.y));
}

void CMainFrame::setTitle() {
  CImageDoc *doc = getDocument();
  String name  = doc->getName();
  CSize  size  = doc->getSize();
  String title = format(_T("PrShow %s - %dx%d"), name.cstr(), size.cx,size.cy);
  SetWindowText(title.cstr());
}

void CMainFrame::OnFileExit() {
  exit(0);
}

void CMainFrame::OnHelpAboutprshow() {
  theApp.OnAppAbout();
}
