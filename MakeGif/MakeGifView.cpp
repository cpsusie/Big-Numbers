#include "stdafx.h"
#include "MakeGifDoc.h"
#include "MakeGifView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMakeGifView, CFormView)

BEGIN_MESSAGE_MAP(CMakeGifView, CFormView)
  ON_WM_SIZE()
  ON_WM_MOUSEMOVE()
  ON_WM_RBUTTONDOWN()
  ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnTtnNeedText)
  ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnTtnNeedText)
END_MESSAGE_MAP()

CMakeGifView::CMakeGifView() : CFormView(CMakeGifView::IDD) {
}

CMakeGifView::~CMakeGifView() {
}

const PixRectArray *CMakeGifView::getVisiblePrArray() {
  CMakeGifDoc *pDoc = GetDocument();
  ASSERT_VALID(pDoc);
  switch(getShowFormat()) {
  case SHOW_RAW      : return &pDoc->getRawPrArray();
  case SHOW_SCALED   : return &pDoc->getScaledPrArray();
  case SHOW_QUANTIZED: return &pDoc->getQuantizedPrArray();
  default            : return NULL;
  }
}

void CMakeGifView::OnDraw(CDC *pDC) {
//  showPanelSize();

  loadGifFileFromDoc();
  const PixRectArray *pra = getVisiblePrArray();
  if(pra) {
    drawImgPanel(pra);
  }
}

ShowFormat CMakeGifView::getShowFormat()  const {
  return ((CMainFrame*)GetParent())->getShowFormat();
}

void CMakeGifView::drawImgPanel(const PixRectArray *prArray) {
  CWnd       *imgPanel = GetDlgItem(IDC_IMGPANEL);
  const CRect cl       = getClientRect(imgPanel);
  CClientDC   dc(imgPanel);
  prArray->paintAll(dc, cl);
}

#ifdef _DEBUG
void CMakeGifView::AssertValid() const {
  __super::AssertValid();
}

void CMakeGifView::Dump(CDumpContext& dc) const {
  __super::Dump(dc);
}

CMakeGifDoc* CMakeGifView::GetDocument() {
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMakeGifDoc)));
  return (CMakeGifDoc*)m_pDocument;
}
#endif //_DEBUG

void CMakeGifView::OnInitialUpdate() {
  __super::OnInitialUpdate();

  CSize clSize = getClientRect(this).Size();
  clSize.cy -= 43;
  CWnd *gifPanel = GetDlgItem(IDC_GIFPANEL);
  setWindowRect(gifPanel, CRect(0,0,clSize.cx, clSize.cy / 2));

  const CRect gifPanelRect = getClientRect(gifPanel);
  m_gifCtrl.Create( EMPTYSTRING, WS_VISIBLE | WS_CHILD, gifPanelRect, gifPanel, IDC_GIFCTRL);

  CWnd *imagePanel = GetDlgItem(IDC_IMGPANEL);
  setWindowRect(imagePanel, CRect(0,clSize.cy/2, clSize.cx, clSize.cy));

  m_layoutManager.OnInitDialog(this);
//  m_layoutManager.addControl(IDC_GIFCTRL , RELATIVE_SIZE);

  m_layoutManager.addControl(IDC_GIFPANEL, RELATIVE_SIZE                  );
  m_layoutManager.addControl(IDC_IMGPANEL, RELATIVE_WIDTH | RELATIVE_Y_POS);

  m_gifCtrl.addPropertyChangeListener(this);
  EnableToolTips(TRUE);
}

void CMakeGifView::OnFinalRelease() {
  m_gifCtrl.removePropertyChangeListener(this);
  m_gifCtrl.unload();
  __super::OnFinalRelease();
}

void CMakeGifView::OnRButtonDownGifPanel(UINT nFlags, CPoint point) {
}

void CMakeGifView::OnRButtonDownImgPanel(UINT nFlags, CPoint point) {
  const PixRectArray *pra = getVisiblePrArray();
  if(pra) {
    const int index = pra->findImageIndexByPoint(point);
    if(index >= 0) {
      const CSize sz = (*pra)[index]->getSize();
      showInformation(_T("size:(%d,%d)"), sz.cx, sz.cy);
    }
  }
}

PanelPoint CMakeGifView::getPanelPoint(const CPoint &viewPoint) const {
  const CRect gifRect = getRelativeClientRect(this, IDC_GIFPANEL);
  const CRect imgRect = getRelativeClientRect(this, IDC_IMGPANEL);
  CPoint p  = viewPoint;
  int    id = -1;
  if(gifRect.PtInRect(p)) {
    id = IDC_GIFPANEL;
  } else if(imgRect.PtInRect(p)) {
    id = IDC_IMGPANEL;
  }
  if(id >= 0) {
    ClientToScreen(&p);
    GetDlgItem(id)->ScreenToClient(&p);
    return PanelPoint(id, p);
  }
  return PanelPoint(id, ORIGIN);
}

void CMakeGifView::OnRButtonDown(UINT nFlags, CPoint point) {
  const PanelPoint pp = getPanelPoint(point);
  switch(pp.m_panelId) {
  case IDC_GIFPANEL:
    OnRButtonDownGifPanel(nFlags, pp.m_point);
    break;
  case IDC_IMGPANEL:
    OnRButtonDownImgPanel(nFlags, pp.m_point);
    break;
  }
  __super::OnRButtonDown(nFlags, point);
}

void CMakeGifView::startPlay() {
  if(isPlaying()) {
    return;
  }
  loadGifFileFromDoc();
  if(m_gifCtrl.isLoaded()) {
    m_gifCtrl.play();
  }
}

void CMakeGifView::stopPlay() {
  if(!isPlaying()) {
    return;
  }
  m_gifCtrl.stop();
}

void CMakeGifView::showAll() {
  if(isPlaying()) {
    return;
  }
  loadGifFileFromDoc();
  if(m_gifCtrl.isLoaded()) {
    m_gifCtrl.paintAllFrames();
  }
}

void CMakeGifView::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  switch(id) {
  case GIF_LOADSTATUS   :
  case GIF_PLAYSTATUS   :
  case GIF_SHOWALLSTATUS:
  case GIF_VISIBLESTATUS:
    notifyPropertyChanged(GIFCTRL_STATUS, oldValue, newValue);
    break;
  }
}

void CMakeGifView::loadGifFileFromDoc() {
  CMakeGifDoc *doc = GetDocument();
  if(doc->getLastUpdate() > m_lastCtrlUpdate) {
    if(doc->hasGifFile()) {
      m_gifCtrl.loadFromGifFile(doc->getGifFile());
    } else {
      m_gifCtrl.unload();
    }
    setProperty(CONTROL_UPDATE_TIME, m_lastCtrlUpdate, doc->getLastUpdate());
  }
}

void CMakeGifView::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);

  if(m_layoutManager.isInitialized()) {
    setWindowSize(&m_gifCtrl, getClientRect(GetDlgItem(IDC_GIFPANEL)).Size());
  }
}

String toString(const CRect &r) {
  return format(_T("(%d,%d,%d,%d)"), r.left,r.top,r.right,r.bottom);
}

void CMakeGifView::showPanelSize() {
  const CRect r1 = getWindowRect(this, IDC_GIFPANEL);
  const CRect r2 = getWindowRect(this, IDC_IMGPANEL);
  const CRect cl = getClientRect(this);
  const CRect r3 = getWindowRect(&m_gifCtrl);
  const String msg = format(_T("CL:%s, GIFPANEL:%s, GIFCTRL:%s, IMAGE:%s")
                           ,toString(cl).cstr()
                           ,toString(r1).cstr()
                           ,toString(r3).cstr()
                           ,toString(r2).cstr());

  CClientDC dc(GetDlgItem(IDC_IMGPANEL));
  textOut(dc, 10,10, msg);
}

BOOL CMakeGifView::OnTtnNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult) {
  UNREFERENCED_PARAMETER(id);

  NMTTDISPINFO *pTTT = (NMTTDISPINFO *)pNMHDR;
  UINT_PTR nID = pNMHDR->idFrom;
  BOOL bRet = FALSE;

  if(pTTT->uFlags & TTF_IDISHWND) {
    // idFrom is actually the HWND of the tool
    const PanelPoint pp = getPanelPoint(m_currentMouse);
    if(pp.m_panelId == IDC_IMGPANEL) {
      const PixRectArray *pra = getVisiblePrArray();
      if(pra) {
        const int index = pra->findImageIndexByPoint(m_currentMouse);
        if(index >= 0) {
          const CSize sz = (*pra)[index]->getSize();
          _stprintf_s(pTTT->szText, ARRAYSIZE(pTTT->szText),_T("size:(%d,%d)"), sz.cx, sz.cy);
          pTTT->hinst = AfxGetResourceHandle();
          bRet = TRUE;
        }
      }
    }
  }
  *pResult = 0;
  return bRet;
}


void CMakeGifView::OnMouseMove(UINT nFlags, CPoint point) {
  m_currentMouse = point;
  __super::OnMouseMove(nFlags, point);
}
