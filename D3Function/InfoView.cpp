#include "stdafx.h"
#include "Mainfrm.h"
#include "InfoView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CInfoView, CFormView)

BEGIN_MESSAGE_MAP(CInfoView, CFormView)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_COMMAND(ID_FILE_PRINT        , OnFilePrint           )
  ON_COMMAND(ID_FILE_PRINT_DIRECT , OnFilePrint           )
  ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview    )
  ON_EN_VSCROLL(IDC_EDIT_INFOBOX  , OnEnVscrollEditInfoBox)
END_MESSAGE_MAP()

CInfoView::CInfoView() : CFormView(CInfoView::IDD) {
  m_topLine       = 0;
  m_scrollEnabled = true;
}

CInfoView::~CInfoView() {
}

int CInfoView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if (__super::OnCreate(lpCreateStruct) == -1) {
    return -1;
  }
  return 0;
}

void CInfoView::OnDraw(CDC *pDC) {
}

BOOL CInfoView::OnPreparePrinting(CPrintInfo *pInfo) {
  return DoPreparePrinting(pInfo);
}
void CInfoView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/) {
}
void CInfoView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/) {
}
#ifdef _DEBUG
void CInfoView::AssertValid() const {
  __super::AssertValid();
}
void CInfoView::Dump(CDumpContext& dc) const {
  __super::Dump(dc);
}

CD3FunctionDoc *CInfoView::GetDocument() { // non-debug version is inline
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CD3FunctionDoc)));
  return (CD3FunctionDoc*)m_pDocument;
}
#endif //_DEBUG

void CInfoView::OnEnVscrollEditInfoBox() {
  if(m_scrollEnabled) {
    SCROLLINFO info;
    getEditBox()->GetScrollInfo(SB_VERT, &info);
    m_topLine = info.nPos;
  }
}

void CInfoView::vprintf(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  String s = vformat(format, argptr).replace('\n', _T("\r\n"));
  CEdit *ip = getEditBox();
  setWindowText(ip, s);
  if(m_topLine > 0) ip->LineScroll(m_topLine);
}

void CInfoView::printf(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  vprintf(format, argptr);
  va_end(argptr);
}

void CInfoView::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  if(!IsWindowVisible()) {
    return;
  }
  setWindowSize(getEditBox(),getClientRect(this).Size());
  ShowScrollBar(SB_BOTH, FALSE);
  if(m_topLine > 0) {
    getEditBox()->LineScroll(m_topLine);
  }
}
