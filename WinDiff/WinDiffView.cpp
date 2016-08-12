#include "stdafx.h"
#include "mainfrm.h"
#include "WinDiffDoc.h"
#include "WinDiffView.h"
#include <MFCUtil/Clipboard.h>
#include "SearchMachine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CWinDiffView, CFormView)

void CWinDiffView::DoDataExchange(CDataExchange* pDX) {
    CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWinDiffView, CFormView)
    ON_WM_SIZE()
    ON_WM_CREATE()
    ON_WM_HSCROLL()
    ON_WM_VSCROLL()
    ON_WM_DROPFILES()
    ON_WM_DESTROY()
    ON_WM_TIMER()
    ON_COMMAND(ID_FILE_PRINT        , CView::OnFilePrint       )
    ON_COMMAND(ID_FILE_PRINT_DIRECT , CView::OnFilePrint       )
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

#define SC_WIDTH 16

CWinDiffView::CWinDiffView() : CFormView(CWinDiffView::IDD) {
  m_timerIsRunning  = false;
  m_nameFontSizePct = 100;
  m_nameHeight      = SC_WIDTH;
  memset(&m_origNameFont, 0, sizeof(m_origNameFont));
}

CWinDiffView::~CWinDiffView() {
}

BOOL CWinDiffView::PreCreateWindow(CREATESTRUCT& cs) {
  return CView::PreCreateWindow(cs);
}

BOOL CWinDiffView::OnPreparePrinting(CPrintInfo* pInfo) {
  return DoPreparePrinting(pInfo);
}

void CWinDiffView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/) {
}

void CWinDiffView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/) {
}

#ifdef _DEBUG
void CWinDiffView::AssertValid() const {
  CView::AssertValid();
}

void CWinDiffView::Dump(CDumpContext& dc) const {
  CView::Dump(dc);
}

CWinDiffDoc* CWinDiffView::GetDocument() { // non-debug version is inline 
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWinDiffDoc)));
  return (CWinDiffDoc*)m_pDocument;
}
#endif //_DEBUG

TextPositionPair CWinDiffView::searchText(const FindParameters &param) {
  TextPosition cp = m_textView.getCurrentPos();

  SearchMachine sm(param);
  if(param.m_dirUp) { // search backward
    if(param.m_skipCurrent) {
      cp.m_column--;
    }
    if(cp.m_column >= 0 && m_textView.attributeMatch(param,cp.m_line)) {
      const int c = sm.findIndex(m_textView.getString(cp.m_line),cp.m_column);
      if(c >= 0) {
        return TextPositionPair(cp.m_line,c, sm.getResultLength());
      }
    }
    for(int l = cp.m_line - 1; l >= 0; l--) {
      if(!m_textView.attributeMatch(param,l)) {
        continue;
      }
      const int c = sm.findIndex(m_textView.getString(l));
      if(c >= 0) {
        return TextPositionPair(l,c,sm.getResultLength());
      }
    }
  } else { // search forward
    const String &s = m_textView.getString(cp.m_line);
    if(param.m_skipCurrent) {
      cp.m_column++;
    }
    if(cp.m_column < (int)s.length() && m_textView.attributeMatch(param,cp.m_line)) {
      const int c = sm.findIndex(s,cp.m_column);
      if(c >= 0) {
        return TextPositionPair(cp.m_line,c,sm.getResultLength());
      }
    }
    const int lineCount = m_textView.getLineCount();
    for(int l = cp.m_line + 1; l < lineCount; l++) {
      if(!m_textView.attributeMatch(param,l)) {
        continue;
      }
      const int c = sm.findIndex(m_textView.getString(l));
      if(c >= 0) {
        return TextPositionPair(l,c,sm.getResultLength());
      }
    }
  }
  return TextPositionPair(); // not found
}

void CWinDiffView::find(const FindParameters &param) {
  if(hasPartner()) {
    getPartner()->m_textView.resetAnchor();
  }
  const TextPositionPair newPos = searchText(param);
  if(!newPos.isEmpty()) {
    m_textView.gotoFoundPosition(newPos);
  }
}

void CWinDiffView::gotoLine(UINT line) {
  if(hasPartner()) {
    getPartner()->m_textView.resetAnchor();
  }
  const int lineIndex = m_textView.getDocument()->m_diff.findLineIndex(getId(),line-1);
  if(lineIndex >= 0) {
    m_textView.gotoLine(lineIndex);
  }
}

void CWinDiffView::OnDraw(CDC *pDC) {
  updateTitle();
  m_textView.OnDraw(pDC);
}

void CWinDiffView::updateTitle() {
  const DiffDoc &doc     = GetDocument()->m_diff.getDoc(getId());
  const UINT     docSize = doc.getSize();
  CWnd *nameWindow = GetDlgItem(IDC_NAME);
  nameWindow->ModifyStyle(0, SS_ENDELLIPSIS);
  nameWindow->SetWindowText(format(_T("%s (%s bytes)"), doc.getName().cstr(), format1000(docSize).cstr()).cstr());
}

int CWinDiffView::getId() {
  CWinDiffSplitterWnd *parent = (CWinDiffSplitterWnd*)GetParent();
  if(parent->getPanelCount() != 2) {
    return 0;
  }

  if(parent->getDiffView(0) == this) {
    return 0;
  } else if(parent->getDiffView(1) == this) {
    return 1;
  } else {
    return 0;
  }
}

void CWinDiffView::setActive(bool active) {
  CWinDiffView *partner = getPartner();
  if(active && partner) {
    partner->m_textView.setActive(false);
  }
  m_textView.setActive(active);
}

CWinDiffView *CWinDiffView::getPartner() {
  CWinDiffSplitterWnd *parent = (CWinDiffSplitterWnd*)GetParent();
  if(parent->getPanelCount() < 2) {
    return NULL;
  }
  return parent->getDiffView(1-getId());
}

void CWinDiffView::OnSize(UINT nType, int cx, int cy) {
  CFormView::OnSize(nType, cx, cy);

  if(!IsWindowVisible()) {
    return;
  }
  adjustChildPositions();
}

void CWinDiffView::adjustChildPositions() {
  const CSize size = getClientRect(this).Size();

  CWnd *nameWindow = GetDlgItem(IDC_NAME);
  if(nameWindow != NULL) {
    WINDOWPLACEMENT wp;
    nameWindow->GetWindowPlacement(&wp);
    wp.rcNormalPosition.left   = 0;
    wp.rcNormalPosition.top    = 0;
    wp.rcNormalPosition.right  = size.cx;
    wp.rcNormalPosition.bottom = m_nameHeight;
    nameWindow->SetWindowPlacement(&wp);
  }
  const int id = getId();
  CWinDiffView *partner = getPartner();

  CScrollBar *scv = (CScrollBar*)GetDlgItem(IDC_SCROLLBARV);
  int tvWidth = size.cx;
  if(scv != NULL) {
    WINDOWPLACEMENT wp;
    scv->GetWindowPlacement(&wp);
    wp.rcNormalPosition.left   = tvWidth - SC_WIDTH;
    wp.rcNormalPosition.top    = m_nameHeight;
    wp.rcNormalPosition.right  = tvWidth;
    wp.rcNormalPosition.bottom = size.cy - SC_WIDTH;
    scv->SetWindowPlacement(&wp);
    if(id > 0 || partner == NULL) {
      tvWidth -= SC_WIDTH;
    } else {
      scv->ShowWindow(SW_HIDE);
    }
  }

  CScrollBar *sch = (CScrollBar*)GetDlgItem(IDC_SCROLLBARH);
  if(sch != NULL) {
    WINDOWPLACEMENT wp;
    sch->GetWindowPlacement(&wp);
    wp.rcNormalPosition.left   = 0;
    wp.rcNormalPosition.top    = size.cy - SC_WIDTH;
    wp.rcNormalPosition.right  = tvWidth;
    wp.rcNormalPosition.bottom = size.cy;
    sch->SetWindowPlacement(&wp);
  }

  WINDOWPLACEMENT wp;
  m_textView.GetWindowPlacement(&wp);
  wp.rcNormalPosition.top      = m_nameHeight;
  wp.rcNormalPosition.right    = tvWidth;
  wp.rcNormalPosition.bottom   = size.cy - SC_WIDTH;
  m_textView.SetWindowPlacement(&wp);

  setScrollRange(true);
}

void CWinDiffView::setNameFontSizePct(int pct, bool updatePartner) {
  CWnd *nameWindow = GetDlgItem(IDC_NAME);
  if(m_origNameFont.lfFaceName[0] == 0) { // not yet initialized
    CFont *origFont = nameWindow->GetFont();
    if(!origFont->GetLogFont(&m_origNameFont)) {
      MessageBox(_T("GetLogFont  failed"),_T("Error"), MB_ICONWARNING);
      return;
    }
/*
    CClientDC dc(nameWindow);
    dc.SelectObject(&origFont);
    m_nameFrameSize = getWindowSize(nameWindow).cy - getTextExtent(dc, _T("W")).cy;
*/
  }
  if(m_currentNameFont.m_hObject != NULL) {
    m_currentNameFont.DeleteObject();
  }
  LOGFONT lf = m_origNameFont;
  lf.lfHeight = (int)((double)pct * lf.lfHeight/100.0);
  if(!m_currentNameFont.CreateFontIndirect(&lf)) {
    MessageBox(_T("CreateFontIndirect failed"), _T("Error"), MB_ICONWARNING);
    return;
  }
  nameWindow->SetFont(&m_currentNameFont);
  m_nameFontSizePct = pct;

  CClientDC dc(nameWindow);
  dc.SelectObject(&m_currentNameFont);
  m_nameHeight = getTextExtent(dc, _T("Wj()")).cy + 6;

  adjustChildPositions();

  if(updatePartner && hasPartner()) { // prevent infinte recursion
    getPartner()->setNameFontSizePct(pct, false);
  } 
}

int CWinDiffView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if(CFormView::OnCreate(lpCreateStruct) == -1) {
    return -1;
  }
  RECT r;
  r.left          = 0;
  r.top           = SC_WIDTH;
  r.right         = lpCreateStruct->cy;
  r.bottom        = lpCreateStruct->cx;

  m_textView.Create(r, this,  GetDocument()->m_diff);
  return 0;
}

void CWinDiffView::OnDestroy() {
  CWinDiffView *partner = getPartner();
  if(partner) {
    partner->m_textView.setActive(true);
  }
  CFormView::OnDestroy();
}

void CWinDiffView::handleAnchor() {
  if(shiftKeyPressed()) {
    m_textView.dropAnchor();
  } else {
    m_textView.resetAnchor();
  }
  CWinDiffView *partner = getPartner();
  if(partner) {
    partner->m_textView.resetAnchor();
  }
}

void CWinDiffView::copyToClipboard() {
  try {
    putClipboard(m_hWnd,m_textView.getSelectedText());
  } catch(Exception e) {
    MessageBox(format(_T("copy to clipboard failed:%s"),e.what()).cstr(),_T("Error"), MB_ICONWARNING);
  }
}

void CWinDiffView::startTimer() {
  if(!m_timerIsRunning && SetTimer(1,100,NULL)) {
    m_timerIsRunning = true;
  }
}

void CWinDiffView::stopTimer() {
  if(m_timerIsRunning) {
    KillTimer(1);
    m_timerIsRunning = false;
  }
}

void CWinDiffView::OnTimer(UINT_PTR nIDEvent) {
  if(m_mousePos.m_distance.x < 0) {
    m_textView.scrollLeft(-m_mousePos.m_distance.x);
  } else if(m_mousePos.m_distance.x > 0) {
    m_textView.scrollRight(m_mousePos.m_distance.x);
  }

  if(m_mousePos.m_distance.y < 0) {
    m_textView.scrollUp(-m_mousePos.m_distance.y);
  } else if(m_mousePos.m_distance.y > 0) {
    m_textView.scrollDown(m_mousePos.m_distance.y);
  }
  m_textView.setCaret(m_mousePos);
  m_textView.displayCaret();
  CView::OnTimer(nIDEvent);
}

BOOL CWinDiffView::PreTranslateMessage(MSG* pMsg) {
  switch(pMsg->message) {
  case WM_LBUTTONDOWN:
    { m_mousePos = m_textView.getNearestCaretPos(pMsg->pt);
      if(m_mousePos.isOutsideWindow()) {
        return false;
      }
      SetCapture();
      m_textView.setCaret(m_mousePos);
      CWinDiffView *partner = getPartner();
      if(partner) {
        partner->m_textView.resetAnchor();
      }
      if((pMsg->wParam & MK_SHIFT) == 0) {
        m_textView.resetAnchor();
      }
      if(m_textView.isActive()) {
        m_textView.displayCaret();
      } else if(partner) {
        partner->m_textView.activatePartner();
      }
      return true;
    }

  case WM_MOUSEMOVE:
    { if(pMsg->wParam & MK_LBUTTON) {
        m_mousePos = m_textView.getNearestCaretPos(pMsg->pt);
        if(m_mousePos.isOutsideWindow()) {
          startTimer();
        } else {
          stopTimer();
          m_textView.setCaret(m_mousePos);
          if(m_textView.m_anchor.isEmpty()) {
            m_textView.setAnchor();
          }
          m_textView.displayCaret();
        }
      }
    }
    break;

  case WM_LBUTTONUP:
    { ReleaseCapture();
      stopTimer();
      break;
    }

  case WM_MOUSEWHEEL:
    { handleAnchor();
      const short zDelta = (short)(pMsg->wParam >> 16);
      if(zDelta > 0) {
        m_textView.scrollUp(3);
      } else {
        m_textView.scrollDown(3);
      }
      return TRUE;
    }
    break;
  }
  return CFormView::PreTranslateMessage(pMsg);
}

void CWinDiffView::setScrollPos() {
  const TextPosition &topLeft = m_textView.getTopLeft();
  CScrollBar *scv = (CScrollBar*)GetDlgItem(IDC_SCROLLBARV);
  CScrollBar *sch = (CScrollBar*)GetDlgItem(IDC_SCROLLBARH);

  scv->SetScrollPos(topLeft.m_line);
  sch->SetScrollPos(topLeft.m_column);
}

void CWinDiffView::setScrollRange(bool redraw) {
  CScrollBar *scv = (CScrollBar*)GetDlgItem(IDC_SCROLLBARV);
  CScrollBar *sch = (CScrollBar*)GetDlgItem(IDC_SCROLLBARH);

  CWinDiffDoc *doc = GetDocument();

  scv->SetScrollRange(0,max(0,doc->m_diff.getLineCount()-1),redraw);
  sch->SetScrollRange(0,max(0,doc->m_diff.getMaxLineLength()-1),redraw);

  SCROLLINFO info;
  scv->GetScrollInfo(&info);
  info.nPage = m_textView.m_winSize.cy;
  scv->SetScrollInfo(&info);
  sch->GetScrollInfo(&info);
  info.nPage = m_textView.m_winSize.cx;
  sch->SetScrollInfo(&info);
}

void CWinDiffView::OnInitialUpdate() {
  CFormView::OnInitialUpdate();
  DragAcceptFiles(true);
  setScrollRange(false);
}

void CWinDiffView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
  if(pScrollBar == NULL) {
    return;
  }
  switch(nSBCode) {
  case SB_BOTTOM        :                                                                   break;
  case SB_ENDSCROLL     :                                                                   break;
  case SB_LINEDOWN      : handleAnchor(); m_textView.scrollDown(1);                         break;
  case SB_LINEUP        : handleAnchor(); m_textView.scrollUp(1);                           break;
  case SB_PAGEDOWN      : handleAnchor(); m_textView.scrollDown(m_textView.m_winSize.cy);   break;
  case SB_PAGEUP        : handleAnchor(); m_textView.scrollUp(  m_textView.m_winSize.cy);   break;
  case SB_THUMBPOSITION :                                                                   break;
  case SB_THUMBTRACK    :
    { // dont use parameter nPos as it is only 16-bits int
      SCROLLINFO scrinfo;
      pScrollBar->GetScrollInfo(&scrinfo);
      const TextPosition &topLeft = m_textView.getTopLeft();
      nPos = scrinfo.nTrackPos;
      if((int)nPos > topLeft.m_line) {
        handleAnchor();
        m_textView.scrollDown(nPos - topLeft.m_line);
      } else if((int)nPos < topLeft.m_line) {
        handleAnchor();
        m_textView.scrollUp(topLeft.m_line - nPos);
      }
      break;
    }
  case SB_TOP           :
    break;
  }
  CFormView::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CWinDiffView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
  if(pScrollBar == NULL) {
    return;
  }
  switch(nSBCode) {
  case SB_BOTTOM        :                                                                   break;
  case SB_ENDSCROLL     :                                                                   break;
  case SB_LINERIGHT     : handleAnchor(); m_textView.scrollRight(1);                        break;
  case SB_LINELEFT      : handleAnchor(); m_textView.scrollLeft(1);                         break;
  case SB_PAGERIGHT     : handleAnchor(); m_textView.scrollRight(m_textView.m_winSize.cx);  break;
  case SB_PAGELEFT      : handleAnchor(); m_textView.scrollLeft( m_textView.m_winSize.cx);  break;
  case SB_THUMBPOSITION :                                                                   break;
  case SB_THUMBTRACK    :
    { // dont use parameter nPos as it is only 16-bits int
      SCROLLINFO scrinfo;
      pScrollBar->GetScrollInfo(&scrinfo);
      const TextPosition &topLeft = m_textView.getTopLeft();
      nPos = scrinfo.nTrackPos;
      if((int)nPos > topLeft.m_column) {
        handleAnchor(); 
        m_textView.scrollRight(nPos - topLeft.m_column);
      } else if((int)nPos < topLeft.m_column) {
        handleAnchor(); 
        m_textView.scrollLeft(topLeft.m_column - nPos);
      }
      break;
    }
  case SB_TOP           :
    break;
  }
  CFormView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CWinDiffView::OnDropFiles(HDROP hDropInfo) {
  TCHAR fname[256];
  int count = DragQueryFile(hDropInfo,-1,fname,ARRAYSIZE(fname));

  CWinDiffDoc *doc = GetDocument();
  if(count == 1) {
    DragQueryFile(hDropInfo,0,fname,ARRAYSIZE(fname));
    doc->setDoc(getId(),DIFFDOC_FILE,fname);
  } else {
    DragQueryFile(hDropInfo,0,fname,ARRAYSIZE(fname));
    doc->setDoc(0,DIFFDOC_FILE,fname);
    DragQueryFile(hDropInfo,1,fname,ARRAYSIZE(fname));
    doc->setDoc(1,DIFFDOC_FILE,fname);
  }

  doc->recompare();
  m_textView.refreshBoth();
  CFormView::OnDropFiles(hDropInfo);
}
