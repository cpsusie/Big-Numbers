#include "stdafx.h"
#include <MFCUtil/resource.h>
#include "GridDlg.h"

IMPLEMENT_DYNAMIC(CGridDlg, CDialog)

CGridDlg::CGridDlg(CWnd* pParent /*=NULL*/)
  : m_image(NULL)
  , CPropertyDialog<GridParameters>(IDD, PROP_GRIDPARAM, pParent)
  , m_colorCount(0)
  , m_horizontalCount(0)
  , m_verticalCount(0)
  , m_cellSize(0)
{
  m_changeHandlerActive = false;
}

CGridDlg::~CGridDlg() {
  releaseImage();
}

void CGridDlg::setImage(PixRect *image) {
  releaseImage();
  if(image) {
    m_image = image->clone(true);
  }
}

void CGridDlg::releaseImage() {
  if(m_image != NULL) {
    delete m_image;
    m_image = NULL;
  }
}

void CGridDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITCELLSIZE       , m_cellSize       );
  DDX_Text(pDX, IDC_EDITHORIZONTALCOUNT, m_horizontalCount);
  DDX_Text(pDX, IDC_EDITVERTICALCOUNT  , m_verticalCount  );
  DDX_Text(pDX, IDC_EDITCOLORCOUNT     , m_colorCount     );
  DDX_Text(pDX, IDC_EDITTOTALCELLCOUNT , m_totalCellCount );
}

BEGIN_MESSAGE_MAP(CGridDlg, CDialog)
  ON_BN_CLICKED(ID_CALCULATE                   , OnClickedCalculate               )
  ON_EN_CHANGE(IDC_EDITCELLSIZE                , OnEnChangeEditCellSize           )
  ON_EN_CHANGE(IDC_EDITHORIZONTALCOUNT         , OnEnChangeEditHorizontalCellCount)
  ON_EN_CHANGE(IDC_EDITVERTICALCOUNT           , OnEnChangeEditVerticalCellCount  )
  ON_EN_CHANGE(IDC_EDITCOLORCOUNT              , OnEnChangeEditColorCount         )
  ON_NOTIFY(UDN_DELTAPOS, IDC_SPINHORIZONTAL   , OnDeltaposSpinHorizontal         )
  ON_NOTIFY(UDN_DELTAPOS, IDC_SPINVERTICAL     , OnDeltaposSpinVertical           )
  ON_NOTIFY(UDN_DELTAPOS, IDC_SPINCOLOR        , OnDeltaposSpinColor              )
  ON_COMMAND(ID_GOTO_HORIZONTALCOUNT           , OnGotoHorizontalCount            )
  ON_COMMAND(ID_GOTO_VERTICALCOUNT             , OnGotoVerticalCount              )
  ON_COMMAND(ID_GOTO_CELLSIZE                  , OnGotoCellSize                   )
  ON_COMMAND(ID_GOTO_COLORCOUNT                , OnGotoColorCount                 )
  ON_MESSAGE(_ID_MSG_RESETCONTROLS             , OnMsgResetControls               )
  ON_WM_SHOWWINDOW()
  ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CGridDlg::OnInitDialog() {
  __super::OnInitDialog();

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_PEARLGRID));
  const GridParameters &param = getStartValue();
  valueToWindow(param);
  return FALSE;
}

BOOL CGridDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

void CGridDlg::OnShowWindow(BOOL bShow, UINT nStatus) {
  __super::OnShowWindow(bShow, nStatus);
  if(bShow) {
    CheckDlgButton(IDC_CHECKAUTOCALCULATE, BST_CHECKED);
    gotoEditBox(this, IDC_EDITCELLSIZE);
  }
}

void CGridDlg::OnHideWindow() {
  ShowWindow(SW_HIDE);
}

void CGridDlg::OnClose() {
  OnHideWindow();
}

void CGridDlg::OnOK() {
  if(!validate()) return;
  windowToValue();
  OnHideWindow();
}

void CGridDlg::OnCancel() {
  const GridParameters &v = getStartValue();
  valueToWindow(v);
  setCurrentValue(v);
  OnHideWindow();
}

LRESULT CGridDlg::OnMsgResetControls(WPARAM wp, LPARAM lp) {
  resetControls();
  return 0;
}

void CGridDlg::resetControls() {
  setNotifyEnabled(false);
  const GridParameters &v = getStartValue();
  setCurrentValue(v);
  valueToWindow(v);

  setNotifyEnabled(true);
}

void CGridDlg::OnClickedCalculate() {
  if (!validate()) return;

  setNotifyEnabled(false);
  windowToValue();
  const GridParameters v = getCurrentValue();
  GridParameters v1 = v;
  v1.m_colorCount++;
  setCurrentValue(v1);
  setNotifyEnabled(true);
  setCurrentValue(v);
}

void CGridDlg::OnEnChangeEditCellSize() {
  if(m_changeHandlerActive) return;
  m_changeHandlerActive = true;

  const String str = getWindowText(this, IDC_EDITCELLSIZE);
  double cellSize;
  if(_stscanf(str.cstr(), _T("%le"), &cellSize) == 1) {
    setCellSize(cellSize);
  }
  m_changeHandlerActive = false;
}

void CGridDlg::OnEnChangeEditHorizontalCellCount() {
  if(m_changeHandlerActive) return;
  m_changeHandlerActive = true;
  UINT value;
  if(getUintValue(IDC_EDITHORIZONTALCOUNT, value)) {
    setHorizontalCount(value);
  }
  m_changeHandlerActive = false;
}

void CGridDlg::OnEnChangeEditVerticalCellCount() {
  if(m_changeHandlerActive) return;
  m_changeHandlerActive = true;
  UINT value;
  if(getUintValue(IDC_EDITVERTICALCOUNT, value)) {
    setVerticalCount(value);
  }
  m_changeHandlerActive = false;
}

void CGridDlg::OnEnChangeEditColorCount() {
  if(m_changeHandlerActive) return;
  m_changeHandlerActive = true;
  UINT value;
  if(getUintValue(IDC_EDITCOLORCOUNT, value)) {
    setColorCount(value);
  }
  m_changeHandlerActive = false;
}

void CGridDlg::OnDeltaposSpinHorizontal(NMHDR *pNMHDR, LRESULT *pResult) {
  LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
  UINT value;
  if(getUintValue(IDC_EDITHORIZONTALCOUNT, value)) {
    setHorizontalCount(value - pNMUpDown->iDelta);
  }
  *pResult = 0;
}


void CGridDlg::OnDeltaposSpinVertical(NMHDR *pNMHDR, LRESULT *pResult) {
  LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
  UINT value;
  if(getUintValue(IDC_EDITVERTICALCOUNT, value)) {
    setVerticalCount(value - pNMUpDown->iDelta);
  }
  *pResult = 0;
}

void CGridDlg::OnDeltaposSpinColor(NMHDR *pNMHDR, LRESULT *pResult) {
  LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
  UINT value;
  if(getUintValue(IDC_EDITCOLORCOUNT, value)) {
    setColorCount(value - pNMUpDown->iDelta);
  }
  *pResult = 0;
}

void CGridDlg::OnGotoHorizontalCount() {
  gotoEditBox(this, IDC_EDITHORIZONTALCOUNT);
}
void CGridDlg::OnGotoVerticalCount() {
  gotoEditBox(this, IDC_EDITVERTICALCOUNT);
}
void CGridDlg::OnGotoCellSize() {
  gotoEditBox(this, IDC_EDITCELLSIZE);
}
void CGridDlg::OnGotoColorCount() {
  gotoEditBox(this, IDC_EDITCOLORCOUNT);
}

bool CGridDlg::validate() {
  if(m_image == NULL) {
    MessageBox(_T("No image"), _T("Error"), MB_ICONWARNING);
    return false;
  }
  if(!UpdateData()) return false;
  if(m_cellSize < 1) {
    MessageBox(_T("Must be >= 1"), _T("Error"), MB_ICONWARNING);
    gotoEditBox(this, IDC_EDITCELLSIZE);
    return false;
  }

  const CSize sz = m_image->getSize();
  if((m_horizontalCount < 5) || (m_horizontalCount > (UINT)sz.cx)) {
    MessageBox(format(_T("Must be in range 5-%d"), sz.cx).cstr(), _T("Error"), MB_ICONWARNING);
    gotoEditBox(this, IDC_EDITHORIZONTALCOUNT);
    return false;
  }
  if((m_verticalCount < 5) || (m_verticalCount > (UINT)sz.cy)) {
    MessageBox(format(_T("Must be in range 5-%d"), m_verticalCount).cstr(), _T("Error"), MB_ICONWARNING);
    gotoEditBox(this, IDC_EDITVERTICALCOUNT);
    return false;
  }
  if(m_colorCount < 2) {
    MessageBox(_T("Must be >= 2"), _T("Error"), MB_ICONWARNING);
    gotoEditBox(this, IDC_EDITCOLORCOUNT);
    return false;
  }
  return true;
}

void CGridDlg::windowToValue() {
  GridParameters param;
  param.m_cellSize      = m_cellSize;
  param.m_cellCount.cx  = m_horizontalCount;
  param.m_cellCount.cy  = m_verticalCount;
  param.m_colorCount    = m_colorCount;
  const bool wasEnabled = isNotifyEnabled();
  if(!IsDlgButtonChecked(IDC_CHECKAUTOCALCULATE)) {
    setNotifyEnabled(false);
  }
  setCurrentValue(param);
  setNotifyEnabled(wasEnabled);
}

void CGridDlg::valueToWindow(const GridParameters &param) {
  m_cellSize            = param.m_cellSize;
  if(getArea(param.m_cellCount) == 0) {
    cellCountFromSize();
  } else {
    m_horizontalCount = param.m_cellCount.cx;
    m_verticalCount   = param.m_cellCount.cy;
  }
  updateTotalCellCount();
  m_colorCount        = param.m_colorCount;
  UpdateData(FALSE);
}

void CGridDlg::setCellSize(double value) {
  if(value >= 1) {
    m_cellSize = value;
    cellCountFromSize();
    UpdateData(FALSE);
    windowToValue();
  }
}

void CGridDlg::setHorizontalCount(UINT value) {
  const CSize sz = m_image->getSize();
  if((5 <= value) && (value <= (UINT)sz.cx)) {
    m_horizontalCount = value;
    m_verticalCount   = (UINT)round((double)sz.cy * m_horizontalCount / sz.cx);
    m_cellSize        =       round((double)sz.cx / m_horizontalCount,3);
    updateTotalCellCount();
    UpdateData(FALSE);
    windowToValue();
  }
}

void CGridDlg::setVerticalCount(UINT value) {
  const CSize sz = m_image->getSize();
  if((5 <= value) && (value <= (UINT)sz.cy)) {
    m_verticalCount   = value;
    m_horizontalCount = (UINT)round((double)sz.cx * m_verticalCount / sz.cy);
    m_cellSize        =       round((double)sz.cy / m_verticalCount,3);
    updateTotalCellCount();
    UpdateData(FALSE);
    windowToValue();
  }
}

void CGridDlg::setColorCount(UINT value) {
  if(value >= 2) {
    m_colorCount = value;
    UpdateData(FALSE);
    windowToValue();
  }
}

void CGridDlg::cellCountFromSize() {
  const CSize sz = m_image->getSize();
  m_horizontalCount = (UINT)round((double)sz.cx / m_cellSize);
  m_verticalCount   = (UINT)round((double)sz.cy / m_cellSize);
  updateTotalCellCount();
}

void CGridDlg::updateTotalCellCount() {
  m_totalCellCount = m_horizontalCount * m_verticalCount;
}

bool CGridDlg::getUintValue(int id, UINT &value) {
  const String str = getWindowText(this, id);
  return _stscanf(str.cstr(), _T("%u"), &value) == 1;
}
