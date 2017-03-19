#include "stdafx.h"
#include "MakeGifDoc.h"
#include "MakeGifView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMakeGifView, CFormView)

void CMakeGifView::DoDataExchange(CDataExchange *pDX) {
    CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMakeGifView, CFormView)
    ON_WM_CREATE()
    ON_WM_SIZE()
END_MESSAGE_MAP()

CMakeGifView::CMakeGifView() : CFormView(CMakeGifView::IDD) {
}

CMakeGifView::~CMakeGifView() {
}

BOOL CMakeGifView::PreCreateWindow(CREATESTRUCT& cs) {
  return CFormView::PreCreateWindow(cs);
}

void CMakeGifView::OnDraw(CDC *pDC) {
//  showPanelSize();
  CMakeGifDoc* pDoc = GetDocument();
  ASSERT_VALID(pDoc);

  loadGifFileFromDoc();
  switch(getShowFormat()) {
  case SHOW_RAW       : drawImagePanel(pDoc->getRawPrArray());       break;
  case SHOW_SCALED    : drawImagePanel(pDoc->getScaledPrArray());    break;
  case SHOW_QUANTIZED : drawImagePanel(pDoc->getQuantizedPrArray()); break;
  }
}

ShowFormat CMakeGifView::getShowFormat()  const {
  return ((CMainFrame*)GetParent())->getShowFormat();
}

void CMakeGifView::drawImagePanel(const PixRectArray &prArray) {
  CWnd       *imagePanel = GetDlgItem(IDC_IMAGEPANEL);
  const CRect cl         = getClientRect(imagePanel);
  CClientDC   dc(imagePanel);
  prArray.paintAll(dc, cl);
}

#ifdef _DEBUG
void CMakeGifView::AssertValid() const {
    CFormView::AssertValid();
}

void CMakeGifView::Dump(CDumpContext& dc) const {
    CFormView::Dump(dc);
}

CMakeGifDoc* CMakeGifView::GetDocument() {
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMakeGifDoc)));
    return (CMakeGifDoc*)m_pDocument;
}
#endif //_DEBUG

int CMakeGifView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if(CFormView::OnCreate(lpCreateStruct) == -1) {
    return -1;
  }
  return 0;
}

void CMakeGifView::OnInitialUpdate() {
  CFormView::OnInitialUpdate();

  CSize clSize = getClientRect(this).Size();
  clSize.cy -= 43;
  CWnd *gifPanel = GetDlgItem(IDC_GIFPANEL);
  setWindowRect(gifPanel, CRect(0,0,clSize.cx, clSize.cy / 2));

  const CRect gifPanelRect = getClientRect(gifPanel);
  m_gifCtrl.Create( EMPTYSTRING, WS_VISIBLE | WS_CHILD, gifPanelRect, gifPanel, IDC_GIFCTRL);

  CWnd *imagePanel = GetDlgItem(IDC_IMAGEPANEL);
  setWindowRect(imagePanel, CRect(0,clSize.cy/2, clSize.cx, clSize.cy));

  m_layoutManager.OnInitDialog(this);
//  m_layoutManager.addControl(IDC_GIFCTRL , RELATIVE_SIZE);

  m_layoutManager.addControl(IDC_GIFPANEL  , RELATIVE_SIZE                  );
  m_layoutManager.addControl(IDC_IMAGEPANEL, RELATIVE_WIDTH | RELATIVE_Y_POS);

  m_gifCtrl.addPropertyChangeListener(this);
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
    const Timestamp oldUpdate = m_lastCtrlUpdate;
    m_lastCtrlUpdate = doc->getLastUpdate();
    notifyPropertyChanged(CONTROL_UPDATE_TIME, &oldUpdate, &m_lastCtrlUpdate);
  }
}

void CMakeGifView::OnSize(UINT nType, int cx, int cy) {
  CFormView::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);

  if(m_layoutManager.isInitialized()) {
    CWnd *gifPanel = GetDlgItem(IDC_GIFPANEL);

    const CRect gifPanelRect = getClientRect(gifPanel);
    setWindowSize(&m_gifCtrl, gifPanelRect.Size());
  }
}

String rectToString(const CRect &r) {
  return format(_T("(%d,%d,%d,%d)"), r.left,r.top,r.right,r.bottom);
}

void CMakeGifView::showPanelSize() {
  const CRect r1 = getWindowRect(this, IDC_GIFPANEL);
  const CRect r2 = getWindowRect(this, IDC_IMAGEPANEL);
  const CRect cl = getClientRect(this);
  const CRect r3 = getWindowRect(&m_gifCtrl);
  String msg = format(_T("CL:%s, GIFPANEL:%s, GIFCTRL:%s, IMAGE:%s")
                     ,rectToString(cl).cstr()
                     ,rectToString(r1).cstr()
                     ,rectToString(r3).cstr()
                     ,rectToString(r2).cstr());

  CClientDC dc(GetDlgItem(IDC_IMAGEPANEL));
  dc.TextOut(10,10, msg.cstr(), msg.length());
}
