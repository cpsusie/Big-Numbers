#include "stdafx.h"
#include "D3FunctionPlotter.h"
#include "ParametricSurfaceDlg.h"
#include "afxdialogex.h"

CParametricSurfaceDlg::CParametricSurfaceDlg(const ParametricSurfaceParameters &param, CWnd* pParent /*=NULL*/)
	: CExprDialog(IDD_PARAMETRICSURFACE_DIALOG, pParent), m_param(param)
  , m_sStepCount(0)
  , m_tStepCount(0)
{
}

CParametricSurfaceDlg::~CParametricSurfaceDlg() {
}

void CParametricSurfaceDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDIT_EXPRX, m_exprX);
  DDX_Text(pDX, IDC_EDIT_EXPRY, m_exprY);
  DDX_Text(pDX, IDC_EDIT_EXPRZ, m_exprZ);
  DDX_Text(pDX, IDC_EDIT_TFROM, m_tfrom);
  DDX_Text(pDX, IDC_EDIT_TTO, m_tto);
  DDX_Text(pDX, IDC_EDIT_SFROM, m_sfrom);
  DDX_Text(pDX, IDC_EDIT_STO, m_sto);
  DDX_Check(pDX, IDC_CHECK_INCLUDETIME, m_includeTime);
  DDX_Check(pDX, IDC_CHECK_DOUBLESIDED, m_doubleSided);
  DDX_Text(pDX, IDC_EDIT_TIMEFROM, m_timeFrom);
  DDX_Text(pDX, IDC_EDIT_TIMETO, m_timeTo);
  DDX_Text(pDX, IDC_EDIT_TSTEPCOUNT, m_tStepCount);
  DDV_MinMaxUInt(pDX, m_tStepCount, 1, 200);
  DDX_Text(pDX, IDC_EDIT_SSTEPCOUNT, m_sStepCount);
  DDV_MinMaxUInt(pDX, m_sStepCount, 1, 200);
  DDX_Text(pDX, IDC_EDIT_FRAMECOUNT, m_frameCount);
  DDV_MinMaxUInt(pDX, m_frameCount, 1, 300);
  DDX_Check(pDX, IDC_CHECK_MACHINECODE, m_machineCode);
}

BEGIN_MESSAGE_MAP(CParametricSurfaceDlg, CDialog)
    ON_WM_SIZE()
    ON_COMMAND(ID_FILE_OPEN                  , OnFileOpen                       )
    ON_COMMAND(ID_FILE_SAVE                  , OnFileSave                       )
    ON_COMMAND(ID_FILE_SAVE_AS               , OnFileSaveAs                     )
    ON_COMMAND(ID_EDIT_FINDMATCHINGPARENTESIS, OnEditFindMatchingParentesis     )
    ON_COMMAND(ID_GOTO_EXPRX                 , OnGotoExprX                      )
    ON_COMMAND(ID_GOTO_EXPRY                 , OnGotoExprY                      )
    ON_COMMAND(ID_GOTO_EXPRZ                 , OnGotoExprZ                      )
    ON_COMMAND(ID_GOTO_TINTERVAL             , OnGotoTInterval                  )
    ON_COMMAND(ID_GOTO_SINTERVAL             , OnGotoSInterval                  )
    ON_COMMAND(ID_GOTO_TSTEPCOUNT            , OnGotoTStepCount                 )
    ON_COMMAND(ID_GOTO_SSTEPCOUNT            , OnGotoSStepCount                 )

    ON_COMMAND(ID_GOTO_TIMEINTERVAL          , OnGotoTimeInterval               )
    ON_COMMAND(ID_GOTO_FRAMECOUNT            , OnGotoFrameCount                 )
    ON_COMMAND_RANGE(ID_EXPRHELP_MENU_FIRST  , ID_EXPRHELP_MENU_LAST, OnExprHelp)
    ON_BN_CLICKED(IDC_BUTTON_HELPX           , OnButtonHelpX                    )
    ON_BN_CLICKED(IDC_BUTTON_HELPY           , OnButtonHelpY                    )
    ON_BN_CLICKED(IDC_BUTTON_HELPZ           , OnButtonHelpZ                    )
    ON_BN_CLICKED(IDC_CHECK_INCLUDETIME      , OnCheckIncludeTime               )
END_MESSAGE_MAP()


BOOL CParametricSurfaceDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  createHelpButton(IDC_BUTTON_HELPX);
  createHelpButton(IDC_BUTTON_HELPY);
  createHelpButton(IDC_BUTTON_HELPZ);

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_STATIC_FUNCTIONX    , PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_EDIT_EXPRX          , PCT_RELATIVE_BOTTOM | RELATIVE_WIDTH);
  m_layoutManager.addControl(IDC_STATIC_FUNCTIONY    , PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_BUTTON_HELPY        , PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_EDIT_EXPRY          , PCT_RELATIVE_TOP    | PCT_RELATIVE_BOTTOM | RELATIVE_WIDTH);
  m_layoutManager.addControl(IDC_STATIC_FUNCTIONZ    , PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_BUTTON_HELPZ        , PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_EDIT_EXPRZ          , PCT_RELATIVE_TOP    | RELATIVE_BOTTOM     | RELATIVE_WIDTH);
  m_layoutManager.addControl(IDC_STATIC_TINTERVAL    , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_TFROM          , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_DASH1        , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_TTO            , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_SINTERVAL    , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_SFROM          , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_DASH2        , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_STO            , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_TSTEPCOUNT   , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_TSTEPCOUNT     , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_SSTEPCOUNT   , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_SSTEPCOUNT     , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_CHECK_MACHINECODE   , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_CHECK_INCLUDETIME   , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_CHECK_DOUBLESIDED   , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_TIMEINTERVAL , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_TIMEFROM       , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_DASH3        , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_TIMETO         , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_FRAMECOUNT   , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_FRAMECOUNT     , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDOK                    , RELATIVE_POSITION    );
  m_layoutManager.addControl(IDCANCEL                , RELATIVE_POSITION    );

  m_accelTable = LoadAccelerators(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDR_PARAMETRICSURFACE_ACCELERATOR));
  setExprFont(IDC_EDIT_EXPRX);
  setExprFont(IDC_EDIT_EXPRY);
  setExprFont(IDC_EDIT_EXPRZ);

  paramToWin(m_param);
  gotoEditBox(this, IDC_EDIT_EXPRX);
  return FALSE;  // return TRUE  unless you set the focus to a control
}

void CParametricSurfaceDlg::OnSize(UINT nType, int cx, int cy) {
  CDialog::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
}

BOOL CParametricSurfaceDlg::PreTranslateMessage(MSG* pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return CDialog::PreTranslateMessage(pMsg);
}

#define MAXPOINTCOUNT 200
#define MAXFRAMECOUNT 300

bool CParametricSurfaceDlg::validate() {
  if(!CExprDialog::validate(IDC_EDIT_EXPRX)) {
    return false;
  }
  if(!CExprDialog::validate(IDC_EDIT_EXPRY)) {
    return false;
  }
  if(!CExprDialog::validate(IDC_EDIT_EXPRZ)) {
    return false;
  }
  if(m_tfrom >= m_tto) {
    gotoEditBox(this, IDC_EDIT_TFROM);
    Message(_T("Invalid interval"));
    return false;
  }
  if(m_tStepCount == 0 || m_tStepCount > MAXPOINTCOUNT) {
    gotoEditBox(this, IDC_EDIT_TSTEPCOUNT);
    Message(_T("Number of t-steps must be between 0 and %d"), MAXPOINTCOUNT);
    return false;
  }
  if(m_sfrom >= m_sto) {
    gotoEditBox(this, IDC_EDIT_SFROM);
    Message(_T("Invalid interval"));
    return false;
  }
  if(m_sStepCount == 0 || m_sStepCount > MAXPOINTCOUNT) {
    gotoEditBox(this, IDC_EDIT_SSTEPCOUNT);
    Message(_T("Number of s-steps must be between 0 and %d"), MAXPOINTCOUNT);
    return false;
  }

  if(m_includeTime) {
    if(m_frameCount <= 0 || m_frameCount > MAXFRAMECOUNT) {
      gotoEditBox(this, IDC_EDIT_FRAMECOUNT);
      Message(_T("Number of frames must be between 1 and %d"), MAXFRAMECOUNT);
      return false;
    }
    if(m_timeFrom >= m_timeTo) {
      gotoEditBox(this, IDC_EDIT_TIMEFROM);
      Message(_T("Invalid interval"));
      return false;
    }
  }
  return true;
}

void CParametricSurfaceDlg::OnOK() {
  UpdateData();
  if(!validate()) {
    return;
  }
  winToParam(m_param);
  CDialog::OnOK();
}

void CParametricSurfaceDlg::OnCheckIncludeTime() {
  UpdateData(TRUE);
  enableTimeFields();
}

void CParametricSurfaceDlg::enableTimeFields() {
  BOOL enable = IsDlgButtonChecked(IDC_CHECK_INCLUDETIME);
  GetDlgItem(IDC_STATIC_TIMEINTERVAL)->EnableWindow(enable);
  GetDlgItem(IDC_EDIT_TIMEFROM      )->EnableWindow(enable);
  GetDlgItem(IDC_EDIT_TIMETO        )->EnableWindow(enable);
  GetDlgItem(IDC_EDIT_FRAMECOUNT    )->EnableWindow(enable);
  setWindowText(this, IDC_STATIC_FUNCTIONX, enable ? _T("&X(time,t,s) =") : _T("&X(t,s) ="));
  setWindowText(this, IDC_STATIC_FUNCTIONY, enable ? _T("&Y(time,t,s) =") : _T("&Y(t,s) ="));
  setWindowText(this, IDC_STATIC_FUNCTIONZ, enable ? _T("&Z(time,t,s) =") : _T("&Z(t,s) ="));
}

static const TCHAR *fileDialogExtensions = _T("Expression-files (*.par)\0*.par\0All files (*.*)\0*.*\0\0");

void CParametricSurfaceDlg::OnFileOpen() {
  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrFilter = fileDialogExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Open Parametric Surface file");
  if((dlg.DoModal() != IDOK) || (_tcslen(dlg.m_ofn.lpstrFile) == 0)) {
    return;
  }

  try {
    ParametricSurfaceParameters param;
    param.load(   dlg.m_ofn.lpstrFile);
    param.setName(dlg.m_ofn.lpstrFile);
    paramToWin(param);
  } catch(Exception e) {
    showException(e);
  }
}

void CParametricSurfaceDlg::OnFileSave() {
  UpdateData();
  if(!validate()) {
    return;
  }
  
  ParametricSurfaceParameters param;
  winToParam(param);
  if(param.hasDefaultName()) {
    saveAs(param);
  } else {
    save(param.getName(), param);
  }
}

void CParametricSurfaceDlg::OnFileSaveAs() {
  UpdateData();
  if(!validate()) {
    return;
  }
  
  ParametricSurfaceParameters param;
  winToParam(param);
  saveAs(param);
}

void CParametricSurfaceDlg::saveAs(ParametricSurfaceParameters &param) {
  CString objname = param.getName().cstr();
  CFileDialog dlg(FALSE, _T("*.par"), objname);
  dlg.m_ofn.lpstrFilter = fileDialogExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Save Parametric Surface");
  if((dlg.DoModal() != IDOK) ||(_tcslen(dlg.m_ofn.lpstrFile) == 0)) {
    return;
  }
  save(dlg.m_ofn.lpstrFile, param);
}

void CParametricSurfaceDlg::save(const String &fileName, ParametricSurfaceParameters &param) {
  try {
    param.setName(fileName);
    param.save(fileName);
    paramToWin(param);
  } catch(Exception e) {
    showException(e);
  }
}

void CParametricSurfaceDlg::OnEditFindMatchingParentesis() {
  gotoMatchingParentesis();
}

void CParametricSurfaceDlg::OnGotoExprX() {
  gotoExprX();
}
void CParametricSurfaceDlg::OnGotoExprY() {
  gotoExprY();
}
void CParametricSurfaceDlg::OnGotoExprZ() {
  gotoExprZ();
}
void CParametricSurfaceDlg::OnGotoTInterval() {
  gotoEditBox(this, IDC_EDIT_TFROM);
}
void CParametricSurfaceDlg::OnGotoSInterval() {
  gotoEditBox(this, IDC_EDIT_SFROM);
}
void CParametricSurfaceDlg::OnGotoTStepCount() {
  gotoEditBox(this, IDC_EDIT_TSTEPCOUNT);
}
void CParametricSurfaceDlg::OnGotoSStepCount() {
  gotoEditBox(this, IDC_EDIT_SSTEPCOUNT);
}
void CParametricSurfaceDlg::OnGotoTimeInterval() {
  gotoEditBox(this, IDC_EDIT_TIMEFROM);
}
void CParametricSurfaceDlg::OnGotoFrameCount() {
  gotoEditBox(this, IDC_EDIT_FRAMECOUNT);
}

void CParametricSurfaceDlg::OnButtonHelpX() {
  m_selectedExprId = IDC_EDIT_EXPRX;
  showExprHelpMenu(IDC_BUTTON_HELPX);
}
void CParametricSurfaceDlg::OnButtonHelpY() {
  m_selectedExprId = IDC_EDIT_EXPRY;
  showExprHelpMenu(IDC_BUTTON_HELPY);
}
void CParametricSurfaceDlg::OnButtonHelpZ() {
  m_selectedExprId = IDC_EDIT_EXPRZ;
  showExprHelpMenu(IDC_BUTTON_HELPZ);
}

void CParametricSurfaceDlg::OnExprHelp(UINT id) {
  handleSelectedExprHelpId(id, m_selectedExprId);
}

void CParametricSurfaceDlg::paramToWin(const ParametricSurfaceParameters &param) {
  m_exprX         = param.m_exprX.cstr();
  m_exprY         = param.m_exprY.cstr();
  m_exprZ         = param.m_exprZ.cstr();
  m_tfrom         = param.getTInterval().getMin();
  m_tto           = param.getTInterval().getMax();
  m_sfrom         = param.getSInterval().getMin();
  m_sto           = param.getSInterval().getMax();
  m_timeFrom      = param.getTimeInterval().getMin();
  m_timeTo        = param.getTimeInterval().getMax();
  m_tStepCount    = param.m_tStepCount;
  m_sStepCount    = param.m_sStepCount;
  m_frameCount    = param.m_frameCount;
  m_name          = param.getName().cstr();
  m_machineCode   = param.m_machineCode ? TRUE : FALSE;
  m_includeTime   = param.m_includeTime ? TRUE : FALSE;
  m_doubleSided   = param.m_doubleSided ? TRUE : FALSE;

  UpdateData(false);
  enableTimeFields();

  SetWindowText(format(_T("Draw Parametric Surface (%s)"), param.getDisplayName().cstr()).cstr());
}

void CParametricSurfaceDlg::winToParam(ParametricSurfaceParameters &param) const {
  param.setName((LPCTSTR)m_name);
  param.m_exprX       = m_exprX;
  param.m_exprY       = m_exprY;
  param.m_exprZ       = m_exprZ;
  param.m_tInterval.setFrom(   m_tfrom);
  param.m_tInterval.setTo(     m_tto);
  param.m_sInterval.setFrom(   m_sfrom);
  param.m_sInterval.setTo(     m_sto);
  param.m_timeInterval.setFrom(m_timeFrom);
  param.m_timeInterval.setTo(  m_timeTo);
  param.m_tStepCount  = m_tStepCount;
  param.m_sStepCount  = m_sStepCount;
  param.m_frameCount  = m_frameCount;
  param.m_machineCode = m_machineCode ? true : false;
  param.m_includeTime = m_includeTime ? true : false;
  param.m_doubleSided = m_doubleSided ? true : false;
}
