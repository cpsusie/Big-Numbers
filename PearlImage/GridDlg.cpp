#include "stdafx.h"
#include <MFCUtil/resource.h>
#include <ExternProcess.h>
#include "GridDlg.h"

IMPLEMENT_DYNAMIC(CGridDlg, CDialog)

CGridDlg::CGridDlg(CWnd* pParent /*=NULL*/)
  : CPropertyDialog<GridParameters>(IDD, PROP_GRIDPARAM, pParent)
  , m_image(NULL)
{
  GridParameters defaultValue;
  m_cellSize        = defaultValue.m_cellSize;
  m_colorCount      = defaultValue.m_colorCount;
  m_horizontalCount = defaultValue.m_cellCount.cx;
  m_verticalCount   = defaultValue.m_cellCount.cy;
  m_cellSizeMM      = defaultValue.m_cellSizeMM;
  m_changeHandlerActive = false;
}

CGridDlg::~CGridDlg() {
  releaseImage();
}

void CGridDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITCELLSIZE       , m_cellSize       );
  DDX_Text(pDX, IDC_EDITHORIZONTALCOUNT, m_horizontalCount);
  DDX_Text(pDX, IDC_EDITVERTICALCOUNT  , m_verticalCount  );
  DDX_Text(pDX, IDC_EDITCELLSIZEMM     , m_cellSizeMM     );
}

BEGIN_MESSAGE_MAP(CGridDlg, CDialog)
  ON_BN_CLICKED(IDC_BUTTONCALCULATE              , OnClickedCalculate               )
  ON_BN_CLICKED(IDC_CHECKAUTOCALCULATE           , OnBnClickedCheckAutoCalculate    )
  ON_BN_CLICKED(IDC_BUTTONDIAGRAM                , OnBnClickedButtonDiagram         )
  ON_EN_CHANGE(IDC_EDITCELLSIZE                  , OnEnChangeEditCellSize           )
  ON_EN_CHANGE(IDC_EDITHORIZONTALCOUNT           , OnEnChangeEditHorizontalCount    )
  ON_EN_CHANGE(IDC_EDITVERTICALCOUNT             , OnEnChangeEditVerticalCount      )
  ON_EN_CHANGE(IDC_EDITCOLORCOUNT                , OnEnChangeEditColorCount         )
  ON_EN_CHANGE(IDC_EDITCELLSIZEMM                , OnEnChangeEditCellSizeMM         )
  ON_NOTIFY(UDN_DELTAPOS, IDC_SPINHORIZONTALCOUNT, OnDeltaposSpinHorizontalCount    )
  ON_NOTIFY(UDN_DELTAPOS, IDC_SPINVERTICALCOUNT  , OnDeltaposSpinVerticalCount      )
  ON_NOTIFY(UDN_DELTAPOS, IDC_SPINCOLORCOUNT     , OnDeltaposSpinColorCount         )
  ON_COMMAND(ID_GOTO_HORIZONTALCOUNT             , OnGotoHorizontalCount            )
  ON_COMMAND(ID_GOTO_VERTICALCOUNT               , OnGotoVerticalCount              )
  ON_COMMAND(ID_GOTO_CELLSIZE                    , OnGotoCellSize                   )
  ON_COMMAND(ID_GOTO_COLORCOUNT                  , OnGotoColorCount                 )
  ON_MESSAGE(_ID_MSG_RESETCONTROLS               , OnMsgResetControls               )
  ON_MESSAGE(ID_MSG_NEWIMAGE                     , OnMsgNewImage                    )
  ON_MESSAGE(ID_MSG_WINDOWTOTOP                  , OnMsgWindowToTop                 )
  ON_WM_SHOWWINDOW()
  ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CGridDlg::OnInitDialog() {
  __super::OnInitDialog();

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_PEARLGRID));
  const GridParameters &param = getStartValue();
  valueToWindow(param);
  PostMessage(ID_MSG_WINDOWTOTOP);
  return FALSE;
}

BOOL CGridDlg::PreTranslateMessage(MSG *pMsg) {
  BOOL ret = TranslateAccelerator(m_hWnd,m_accelTable,pMsg);
  if(!ret) {
    ret = __super::PreTranslateMessage(pMsg);
  }
  GetDlgItem(IDC_BUTTONCALCULATE)->EnableWindow(!IsDlgButtonChecked(IDC_CHECKAUTOCALCULATE));
  return ret;
}

void CGridDlg::OnShowWindow(BOOL bShow, UINT nStatus) {
  __super::OnShowWindow(bShow, nStatus);
  if(bShow) {
    CheckDlgButton(IDC_CHECKAUTOCALCULATE, BST_CHECKED);
    gotoEditBox(this, IDC_EDITCELLSIZE);
    calculate();
    PostMessage(ID_MSG_WINDOWTOTOP);
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

LRESULT CGridDlg::OnMsgNewImage(WPARAM wp, LPARAM lp) {
  cellCountFromSize();
  flushData();
  calculate();
  return 0;
}

LRESULT CGridDlg::OnMsgWindowToTop(WPARAM wp, LPARAM lp) {
  if(!::BringWindowToTop(*this)) {
    showWarning(getLastErrorText());
  };
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
  calculate();
}

void CGridDlg::calculate() {
  if(!validate()) return;

  setNotifyEnabled(false);
  windowToValue();
  const GridParameters v = getCurrentValue();
  GridParameters v1 = v;
  v1.m_colorCount++;
  setCurrentValue(v1);
  setNotifyEnabled(true);
  setCurrentValue(v);
}

void CGridDlg::OnBnClickedCheckAutoCalculate() {
  if(IsDlgButtonChecked(IDC_CHECKAUTOCALCULATE)) {
    calculate();
  }
}

String getTempFileName(const String &fileName) {
  return FileNameSplitter::getChildName(_T("c:\\temp"), fileName);
}

String createTempFileName(const String &ext) {
  String fileName = getTempFileName(_T("cXXXXXX"));
  _tmktemp(fileName.cstr());
  return FileNameSplitter(fileName).setExtension(ext).getAbsolutePath();
}

void CGridDlg::OnBnClickedButtonDiagram() {
  if(!validate()) return;

  windowToValue();
  const GridParameters v = getCurrentValue();
  PearlDiagram diagram;
  PixRect *tmp = v.calculateImage(getImage(), &diagram);
  SAFEDELETE(tmp);

  const String dumpFileName = createTempFileName(_T("txt"));
  FILE *f = NULL;
  try {
    const String dstr = diagram.toString();
    f = MKFOPEN(dumpFileName,_T("w"));

    _ftprintf(f, _T("%s"), dstr.cstr());
    fclose(f); f = NULL;

    ExternProcess::run(false, _T("c:\\windows\\system32\\notepad.exe"), dumpFileName.cstr(), NULL);
    UNLINK(dumpFileName);
  } catch (Exception e) {
    if(f) { fclose(f); f = NULL; }
    showException(e);
  }
}

void CGridDlg::OnEnChangeEditCellSize() {
  if(m_changeHandlerActive) return;
  m_changeHandlerActive = true;

  double value;
  if(getDoubleValue(IDC_EDITCELLSIZE, value)) {
    setCellSize(value);
  }
  m_changeHandlerActive = false;
}

void CGridDlg::OnEnChangeEditCellSizeMM() {
  if(m_changeHandlerActive) return;
  m_changeHandlerActive = true;

  double value;
  if(getDoubleValue(IDC_EDITCELLSIZEMM, value)) {
    setCellSizeMM(value);
  }
  m_changeHandlerActive = false;
}

void CGridDlg::OnEnChangeEditHorizontalCount() {
  if(m_changeHandlerActive) return;
  m_changeHandlerActive = true;
  UINT value;
  if(getUintValue(IDC_EDITHORIZONTALCOUNT, value)) {
    setHorizontalCount(value);
  }
  m_changeHandlerActive = false;
}

void CGridDlg::OnEnChangeEditVerticalCount() {
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
  if(getUintEmptyZero(IDC_EDITCOLORCOUNT, value)) {
    setColorCount(value);
  }
  m_changeHandlerActive = false;
}

void CGridDlg::OnDeltaposSpinHorizontalCount(NMHDR *pNMHDR, LRESULT *pResult) {
  LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
  UINT value;
  if(getUintValue(IDC_EDITHORIZONTALCOUNT, value)) {
    setHorizontalCount(value - pNMUpDown->iDelta);
  }
  *pResult = 0;
}

void CGridDlg::OnDeltaposSpinVerticalCount(NMHDR *pNMHDR, LRESULT *pResult) {
  LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
  UINT value;
  if(getUintValue(IDC_EDITVERTICALCOUNT, value)) {
    setVerticalCount(value - pNMUpDown->iDelta);
  }
  *pResult = 0;
}

void CGridDlg::OnDeltaposSpinColorCount(NMHDR *pNMHDR, LRESULT *pResult) {
  LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
  UINT value;
  if(getUintEmptyZero(IDC_EDITCOLORCOUNT, value)) {
    const int delta = -pNMUpDown->iDelta;
    if(value == 0) {
      if(delta < 0) {
        setColorCount(400);
      }
    } else {
      setColorCount(value + delta);
    }
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
    showWarning(_T("No image"));
    return false;
  }
  if(!getData()) {
    return false;
  }
  if(m_cellSize < 1) {
    gotoEditBox(this, IDC_EDITCELLSIZE);
    showWarning(_T("Must be >= 1"));
    return false;
  }

  if(m_cellSizeMM <= 0) {
    gotoEditBox(this, IDC_EDITCELLSIZEMM);
    showWarning(_T("Must be > 0"));
    return false;
  }

  const CSize sz = getImageSize();
  if((m_horizontalCount < 5) || (m_horizontalCount > (UINT)sz.cx)) {
    gotoEditBox(this, IDC_EDITHORIZONTALCOUNT);
    showWarning(_T("Must be in range 5-%d"), sz.cx);
    return false;
  }
  if((m_verticalCount < 5) || (m_verticalCount > (UINT)sz.cy)) {
    gotoEditBox(this, IDC_EDITVERTICALCOUNT);
    showWarning(_T("Must be in range 5-%d"), m_verticalCount);
    return false;
  }
  if((m_colorCount < 2) && (m_colorCount != 0)) {
    gotoEditBox(this, IDC_EDITCOLORCOUNT);
    showWarning(_T("Must be >= 2"));
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
  param.m_cellSizeMM    = m_cellSizeMM;
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
  m_colorCount        = param.m_colorCount;
  m_cellSizeMM        = param.m_cellSizeMM;
  flushData();
}

void CGridDlg::setCellSize(double value) {
  if(value >= 1) {
    m_cellSize = value;
    cellCountFromSize();
    flushData();
    windowToValue();
  }
}

void CGridDlg::setCellSizeMM(double value) {
  if(value > 0) {
    m_cellSizeMM = value;
    flushData();
    windowToValue();
  }
}

void CGridDlg::setHorizontalCount(UINT value) {
  const CSize sz = getImageSize();
  if((5 <= value) && (value <= (UINT)sz.cx)) {
    m_horizontalCount = value;
    m_verticalCount   = (UINT)round((double)sz.cy * m_horizontalCount / sz.cx);
    m_cellSize        =       round((double)sz.cx / m_horizontalCount,3);
    flushData();
    windowToValue();
  }
}

void CGridDlg::setVerticalCount(UINT value) {
  const CSize sz = getImageSize();
  if((5 <= value) && (value <= (UINT)sz.cy)) {
    m_verticalCount   = value;
    m_horizontalCount = (UINT)round((double)sz.cx * m_verticalCount / sz.cy);
    m_cellSize        =       round((double)sz.cy / m_verticalCount,3);
    flushData();
    windowToValue();
  }
}

void CGridDlg::setColorCount(int value) {
  if(value >= 2 || (value == 0)) {
    CEdit *e = (CEdit*)GetDlgItem(IDC_EDITCOLORCOUNT);
    const DWORD sel = e->GetSel();
    m_colorCount = value;
    flushData();
    windowToValue();
    e->SetSel(sel);
  }
}

bool CGridDlg::getData() {
  if(!UpdateData()) return false;
  return getUintEmptyZero(IDC_EDITCOLORCOUNT, m_colorCount);
}

void CGridDlg::flushData() {
  UpdateData(false);
  setUintEmptyZero(IDC_EDITCOLORCOUNT, m_colorCount);
  setWindowText(this, IDC_STATICTOTALCOUNT, format1000(m_horizontalCount * m_verticalCount));
  setWindowText(this, IDC_STATICIMAGESIZEMM, format(_T("%.1lf x %.1lf"), m_cellSizeMM*m_horizontalCount, m_cellSizeMM * m_verticalCount));
}

void CGridDlg::setImage(const PixRect *image) {
  releaseImage();
  if(image) {
    m_image = image->clone(true);
    if(isVisible()) {
      PostMessage(ID_MSG_NEWIMAGE);
    }
  }
}

void CGridDlg::releaseImage() {
  SAFEDELETE(m_image);
}

void CGridDlg::cellCountFromSize() {
  const CSize sz = getImageSize();
  m_horizontalCount = (UINT)round((double)sz.cx / m_cellSize);
  m_verticalCount   = (UINT)round((double)sz.cy / m_cellSize);
}

bool CGridDlg::getUintValue(int id, UINT &value) {
  const String str = getWindowText(this, id);
  return _stscanf(str.cstr(), _T("%u"), &value) == 1;
}

bool CGridDlg::getUintEmptyZero(int id, UINT &value) {
  const String str = getWindowText(this, id).trim();
  if(str.length() == 0) {
    value = 0;
    return true;
  } else {
    return _stscanf(str.cstr(), _T("%u"), &value) == 1;
  }
}

void CGridDlg::setUintEmptyZero(int id, UINT value) {
  setWindowText(this, id, value?format(_T("%u"),value):EMPTYSTRING);
}

bool CGridDlg::getDoubleValue(int id, double &value) {
  const String str = getWindowText(this, id);
  return _stscanf(str.cstr(), _T("%le"), &value) == 1;
}

CSize CGridDlg::getImageSize() const {
  return m_image ? m_image->getSize() : CSize(1,1);
}
