#include "stdafx.h"
#include <DebugLog.h>
#include "RunLayoutManagerDlg.h"

#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CRunLayoutManagerDlg::CRunLayoutManagerDlg(int windowFlags, int list1Flags, int list2Flags, int buttonFlags, CWnd *pParent /*=NULL*/)
: CDialog(CRunLayoutManagerDlg::IDD, pParent)
{

  m_windowFlags = windowFlags;
  m_list1Flags  = list1Flags;
  m_list2Flags  = list2Flags;
  m_buttonFlags = buttonFlags;
  redirectDebugLog();
}

void CRunLayoutManagerDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRunLayoutManagerDlg, CDialog)
    ON_WM_SIZE()
    ON_WM_SIZING()
    ON_COMMAND(ID_FONT_SIZE1               , OnFontSize1               )
    ON_COMMAND(ID_FONT_SIZE15              , OnFontSize15              )
    ON_COMMAND(ID_FONT_SIZE175             , OnFontSize175             )
    ON_COMMAND(ID_FONT_SIZE2               , OnFontSize2               )
    ON_COMMAND(ID_FONT_SIZE3               , OnFontSize3               )
    ON_COMMAND(ID_LAYOUT_UPPERLISTVISIBLE  , OnLayoutUpperListVisible  )
    ON_COMMAND(ID_LAYOUT_LOWERLISTVISIBLE  , OnLayoutLowerListVisible  )
    ON_COMMAND(ID_LAYOUT_LOWERLIST_HALFSIZE, OnLayoutLowerListHalfsize )
    ON_BN_CLICKED(IDC_ONSIZE               , OnOnSize                  )
    ON_COMMAND(ID_FILE_EXIT, OnFileExit)
END_MESSAGE_MAP()

BOOL CRunLayoutManagerDlg::OnInitDialog() {
  __super::OnInitDialog();

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_RUNLAYOUTMANAGER_ACCELERATOR));

  CListBox *list1 = (CListBox*)GetDlgItem(IDC_LIST1);
  for(int i = 0; i < 10; i++) {
    list1->AddString(format(_T("Line %4d in upper list"),i+1).cstr());
  }

  CListCtrl &list2 = *(CListCtrl*)GetDlgItem(IDC_LIST2);

  list2.InsertColumn(0,_T("Titel")      , LVCFMT_LEFT, 350);
  list2.InsertColumn(1,_T("Medvirkende"), LVCFMT_LEFT, 200);
  list2.InsertColumn(2,_T("Album")      , LVCFMT_LEFT, 160);
  list2.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);

  for(int i = 0; i < 10; i++) {
    addData(list2, i,0,format(_T("Titel %d"), i+1),true);
    addData(list2, i,1,format(_T("Medvirkende %d"), i+1));
    addData(list2, i,2,format(_T("Album %d"), i+1));
  }

  m_layoutManager.OnInitDialog(this         , m_windowFlags);
  m_layoutManager.addControl(IDC_ONSIZE     , m_buttonFlags);
  m_layoutManager.addControl(IDOK           , m_buttonFlags);
  m_layoutManager.addControl(IDCANCEL       , m_buttonFlags);
  m_layoutManager.addControl(IDC_STATIC_INFO, RELATIVE_Y_POS | RELATIVE_WIDTH );
  m_layoutManager.addControl(IDC_CHECK1     , m_buttonFlags);
  m_layoutManager.addControl(IDC_LIST1      , m_list1Flags );
  m_layoutManager.addControl(IDC_LIST2      , m_list2Flags );

  showLayout();

  return TRUE;
}

BOOL CRunLayoutManagerDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

void CRunLayoutManagerDlg::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  const CSize cs = getClientRect(this).Size();
  m_layoutManager.OnSize(nType,cx,cy);
  showLayout();
}

void CRunLayoutManagerDlg::OnSizing(UINT fwSide, LPRECT pRect) {
  m_layoutManager.OnSizing(fwSide, pRect);
  __super::OnSizing(fwSide, pRect);
}

void CRunLayoutManagerDlg::OnFontSize1() {
  scaleFont(1);
}

void CRunLayoutManagerDlg::OnFontSize15() {
  scaleFont(1.5);
}

void CRunLayoutManagerDlg::OnFontSize175() {
  scaleFont(1.75);
}

void CRunLayoutManagerDlg::OnFontSize2() {
  scaleFont(2);
}

void CRunLayoutManagerDlg::OnFontSize3() {
  scaleFont(3);
}

void CRunLayoutManagerDlg::scaleFont(double scale) {
  m_layoutManager.scaleFont(scale, true);
}

void CRunLayoutManagerDlg::OnOnSize() {
  const CSize cs = getClientRect(this).Size();
  m_layoutManager.OnSize(0,cs.cx,cs.cy);
  showLayout();
}

void CRunLayoutManagerDlg::OnLayoutUpperListVisible() {
  const bool upperListVisible = toggleMenuItem(this, ID_LAYOUT_UPPERLISTVISIBLE);
  CRect upperRect = getRelativeWindowRect(this, IDC_LIST1);
  CRect lowerRect = getRelativeWindowRect(this, IDC_LIST2);

  if(upperListVisible) {
    const int newLowerListTop = lowerRect.CenterPoint().y;
    upperRect = CRect(lowerRect.left, lowerRect.top   , lowerRect.right, lowerRect.top + lowerRect.Height()/2);
    lowerRect = CRect(lowerRect.left, upperRect.bottom, lowerRect.right, lowerRect.bottom                    );
  } else {
    lowerRect = CRect(lowerRect.left,upperRect.top, lowerRect.right, lowerRect.bottom);;
  }
  if(upperListVisible) {
    GetDlgItem(IDC_LIST1)->ShowWindow(SW_SHOW);
    setRelativeWindowRect(this, IDC_LIST1, upperRect);
  } else {
    GetDlgItem(IDC_LIST1)->ShowWindow(SW_HIDE);
  }
  setRelativeWindowRect(this, IDC_LIST2, lowerRect);
  enableLayoutMenues();
}

void CRunLayoutManagerDlg::OnLayoutLowerListVisible() {
  const bool lowerListVisible = toggleMenuItem(this, ID_LAYOUT_LOWERLISTVISIBLE);
  CRect upperRect = getRelativeWindowRect(this, IDC_LIST1);
  CRect lowerRect = getRelativeWindowRect(this, IDC_LIST2);

  if(lowerListVisible) {
    lowerRect = CRect(upperRect.left, upperRect.top + upperRect.Height()/2, upperRect.right, upperRect.bottom);
    upperRect = CRect(upperRect.left, upperRect.top                       , upperRect.right, lowerRect.top   );
  } else {
    upperRect = CRect(upperRect.left, upperRect.top, upperRect.right, lowerRect.bottom);
  }

  if(lowerListVisible) {
    GetDlgItem(IDC_LIST2)->ShowWindow(SW_SHOW);
    setRelativeWindowRect(this, IDC_LIST2, lowerRect);
  } else {
    GetDlgItem(IDC_LIST2)->ShowWindow(SW_HIDE);
  }
  setRelativeWindowRect(this, IDC_LIST1, upperRect);
  enableLayoutMenues();
}

void CRunLayoutManagerDlg::OnLayoutLowerListHalfsize() {
  const bool lowerListHalfSize = toggleMenuItem(this, ID_LAYOUT_LOWERLIST_HALFSIZE);
  int w = getClientRect(this).Width() ;
  if(lowerListHalfSize) w /= 2;
  const int h = getWindowRect(this, IDC_LIST2).Height();
  setWindowSize(this, IDC_LIST2, CSize(w,h));
  enableLayoutMenues();
}

void CRunLayoutManagerDlg::enableLayoutMenues() {
  bool upperVisibleEnabled  = true;
  bool lowerVisibleEnabled  = true;
  bool lowerHalfSizeEnabled = true;

  if(!isMenuItemChecked(this, ID_LAYOUT_UPPERLISTVISIBLE)) {
    lowerVisibleEnabled = false;
  }
  if(!isMenuItemChecked(this, ID_LAYOUT_LOWERLISTVISIBLE)) {
    upperVisibleEnabled  = false;
    lowerHalfSizeEnabled = false;
  }

  enableMenuItem(this, ID_LAYOUT_UPPERLISTVISIBLE  , upperVisibleEnabled );
  enableMenuItem(this, ID_LAYOUT_LOWERLISTVISIBLE  , lowerVisibleEnabled );
  enableMenuItem(this, ID_LAYOUT_LOWERLIST_HALFSIZE, lowerHalfSizeEnabled);
  Invalidate(FALSE);
  showLayout();
}

String toString(const CRect &r) {
  return format(_T("(%3d,%3d,%3d,%3d)"), r.left,r.top,r.Width(),r.Height());
}

#define winStr(id) format(_T("%s:%s"), _T(#id), toString(getRelativeWindowRect(this, id)).cstr())

void CRunLayoutManagerDlg::showLayout() {
  CWnd *infoWin = GetDlgItem(IDC_STATIC_INFO);
  if(infoWin != NULL) {
    infoWin->SetWindowText(format(_T("%s %s"), winStr(IDC_LIST1).cstr(), winStr(IDC_LIST2).cstr()).cstr());
  }
}

void CRunLayoutManagerDlg::OnFileExit() {
  EndDialog(0);
}
