#include "stdafx.h"
#include "Function2DSurfaceDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CFunction2DSurfaceDlg::CFunction2DSurfaceDlg(const Function2DSurfaceParameters &param, CWnd* pParent) : CExprDialog(CFunction2DSurfaceDlg::IDD, pParent), m_param(param) {
}

void CFunction2DSurfaceDlg::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX , IDC_EDIT_EXPR             , m_expr                      );
    DDX_Text(pDX , IDC_EDIT_XFROM            , m_xfrom                     );
    DDX_Text(pDX , IDC_EDIT_XTO              , m_xto                       );
    DDX_Text(pDX , IDC_EDIT_YFROM            , m_yfrom                     );
    DDX_Text(pDX , IDC_EDIT_YTO              , m_yto                       );
    DDX_Text(pDX , IDC_EDIT_POINTS           , m_pointCount                );
    DDX_Check(pDX, IDC_CHECK_MACHINECODE     , m_machineCode               );
    DDX_Check(pDX, IDC_CHECK_DOUBLESIDED     , m_doubleSided               );
    DDX_Check(pDX, IDC_CHECK_INCLUDETIME     , m_includeTime               );
    DDX_Text(pDX , IDC_EDIT_TIMECOUNT        , m_timeCount                 );
    DDX_Text(pDX , IDC_EDIT_TIMEFROM         , m_timeFrom                  );
    DDX_Text(pDX , IDC_EDIT_TIMETO           , m_timeTo                    );
}

BEGIN_MESSAGE_MAP(CFunction2DSurfaceDlg, CDialog)
    ON_WM_SIZE()
    ON_COMMAND(ID_FILE_OPEN                  , OnFileOpen                       )
    ON_COMMAND(ID_FILE_SAVE                  , OnFileSave                       )
    ON_COMMAND(ID_FILE_SAVE_AS               , OnFileSaveAs                     )
    ON_COMMAND(ID_EDIT_FINDMATCHINGPARENTESIS, OnEditFindMatchingParentesis     )
    ON_COMMAND(ID_GOTO_EXPR                  , OnGotoExpr                       )
    ON_COMMAND(ID_GOTO_XINTERVAL             , OnGotoXInterval                  )
    ON_COMMAND(ID_GOTO_YINTERVAL             , OnGotoYInterval                  )
    ON_COMMAND(ID_GOTO_TINTERVAL             , OnGotoTInterval                  )
    ON_COMMAND(ID_GOTO_POINTCOUNT            , OnGotoPointCount                 )
    ON_COMMAND(ID_GOTO_TIMECOUNT             , OnGotoTimeCount                  )
    ON_COMMAND_RANGE(ID_EXPRHELP_MENU_FIRST  , ID_EXPRHELP_MENU_LAST, OnExprHelp)
    ON_BN_CLICKED(IDC_BUTTON_HELP            , OnButtonHelp                     )
    ON_BN_CLICKED(IDC_CHECK_INCLUDETIME      , OnCheckIncludeTime               )
END_MESSAGE_MAP()

BOOL CFunction2DSurfaceDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  createHelpButton();

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_STATIC_FUNCTION     , PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_EDIT_EXPR           , RELATIVE_SIZE        );
  m_layoutManager.addControl(IDC_STATIC_XINTERVAL    , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_XFROM          , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_DASH1        , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_XTO            , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_YINTERVAL    , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_YFROM          , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_DASH2        , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_YTO            , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_POINTCOUNT   , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_POINTS         , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_CHECK_MACHINECODE   , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_CHECK_INCLUDETIME   , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_CHECK_DOUBLESIDED   , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_TIMEINTERVAL , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_TIMEFROM       , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_DASH3        , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_TIMETO         , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_TIMECOUNT    , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_TIMECOUNT      , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDOK                    , RELATIVE_POSITION    );
  m_layoutManager.addControl(IDCANCEL                , RELATIVE_POSITION    );

  m_accelTable = LoadAccelerators(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDR_FUNC2DSURFACE_ACCELERATOR));
  setExprFont();

  paramToWin(m_param);
  GetDlgItem(IDC_EDIT_EXPR)->SetFocus();
  return FALSE;  // return TRUE  unless you set the focus to a control
}

void CFunction2DSurfaceDlg::OnSize(UINT nType, int cx, int cy) {
  CDialog::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
}

BOOL CFunction2DSurfaceDlg::PreTranslateMessage(MSG* pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return CDialog::PreTranslateMessage(pMsg);
}

#define MAXPOINTCOUNT 200
#define MAXTIMECOUNT 200

bool CFunction2DSurfaceDlg::validate() {
  if(!CExprDialog::validate()) {
    return false;
  }
  if(m_pointCount == 0 || m_pointCount > MAXPOINTCOUNT) {
    gotoEditBox(this, IDC_EDIT_POINTS);
    Message(_T("Number of points must be between 0 and %d"), MAXPOINTCOUNT);
    return false;
  }
  if(m_xfrom >= m_xto) {
    gotoEditBox(this, IDC_EDIT_XFROM);
    Message(_T("Invalid interval"));
    return false;
  }
  if(m_yfrom >= m_yto) {
    gotoEditBox(this, IDC_EDIT_YFROM);
    Message(_T("Invalid interval"));
    return false;
  }

  if(m_includeTime) {
    if(m_timeCount <= 0 || m_timeCount > MAXTIMECOUNT) {
      gotoEditBox(this, IDC_EDIT_TIMECOUNT);
      Message(_T("Number of times must be between 1 and %d"), MAXTIMECOUNT);
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

void CFunction2DSurfaceDlg::OnOK() {
  UpdateData();
  if(!validate()) {
    return;
  }
  
  winToParam(m_param);
  CDialog::OnOK();
}

void CFunction2DSurfaceDlg::OnCheckIncludeTime() {
  UpdateData(TRUE);
  enableTimeFields();
}

void CFunction2DSurfaceDlg::enableTimeFields() {
  BOOL enable = IsDlgButtonChecked(IDC_CHECK_INCLUDETIME);
  GetDlgItem(IDC_STATIC_TIMEINTERVAL)->EnableWindow(enable);
  GetDlgItem(IDC_EDIT_TIMEFROM      )->EnableWindow(enable);
  GetDlgItem(IDC_EDIT_TIMETO        )->EnableWindow(enable);
  GetDlgItem(IDC_EDIT_TIMECOUNT     )->EnableWindow(enable);
  setWindowText(this, IDC_STATIC_FUNCTION, enable ? _T("F(t,&x,y) =") : _T("F(&x,y) ="));
}

static const TCHAR *fileDialogExtensions = _T("Expression-files (*.exp)\0*.exp\0All files (*.*)\0*.*\0\0");

void CFunction2DSurfaceDlg::OnFileOpen() {
  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrFilter = fileDialogExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Open expression");
  if((dlg.DoModal() != IDOK) || (_tcslen(dlg.m_ofn.lpstrFile) == 0)) {
    return;
  }

  try {
    Function2DSurfaceParameters param;
    param.load(dlg.m_ofn.lpstrFile);
    param.setName(dlg.m_ofn.lpstrFile);
    paramToWin(param);
  } catch(Exception e) {
    showException(e);
  }
}

void CFunction2DSurfaceDlg::OnFileSave() {
  UpdateData();
  if(!validate()) {
    return;
  }
  
  Function2DSurfaceParameters param;
  winToParam(param);

  if(param.hasDefaultName()) {
    saveAs(param);
  } else {
    save(param.getName(), param);
  }
}

void CFunction2DSurfaceDlg::OnFileSaveAs() {
  UpdateData();
  if(!validate()) {
    return;
  }
  
  Function2DSurfaceParameters param;
  winToParam(param);
  saveAs(param);
}

void CFunction2DSurfaceDlg::saveAs(Function2DSurfaceParameters &param) {
  CString objname = param.getName().cstr();
  CFileDialog dlg(FALSE, _T("*.exp"), objname);
  dlg.m_ofn.lpstrFilter = fileDialogExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Save expression");
  if((dlg.DoModal() != IDOK) ||(_tcslen(dlg.m_ofn.lpstrFile) == 0)) {
    return;
  }
  save(dlg.m_ofn.lpstrFile, param);
}

void CFunction2DSurfaceDlg::save(const String &fileName, Function2DSurfaceParameters &param) {
  try {
    param.setName(fileName);
    param.save(fileName);
    paramToWin(param);
  } catch(Exception e) {
    showException(e);
  }
}

void CFunction2DSurfaceDlg::OnEditFindMatchingParentesis() {
  gotoMatchingParentesis();
}

void CFunction2DSurfaceDlg::OnGotoExpr() {
  gotoExpr();
}

void CFunction2DSurfaceDlg::OnGotoXInterval() {
  gotoEditBox(this, IDC_EDIT_XFROM);
}

void CFunction2DSurfaceDlg::OnGotoYInterval() {
  gotoEditBox(this, IDC_EDIT_YFROM);
}

void CFunction2DSurfaceDlg::OnGotoPointCount() {
  gotoEditBox(this, IDC_EDIT_POINTS);
}

void CFunction2DSurfaceDlg::OnGotoTInterval() {
  gotoEditBox(this, IDC_EDIT_TIMEFROM);
}

void CFunction2DSurfaceDlg::OnGotoTimeCount() {
  gotoEditBox(this, IDC_EDIT_TIMECOUNT);
}

void CFunction2DSurfaceDlg::OnButtonHelp() {
  showExprHelpMenu();
}

void CFunction2DSurfaceDlg::OnExprHelp(UINT id) {
  handleSelectedExprHelpId(id);
}

void CFunction2DSurfaceDlg::paramToWin(const Function2DSurfaceParameters &param) {
  m_expr          = param.m_expr.cstr();
  m_xfrom         = param.getXInterval().getMin();
  m_xto           = param.getXInterval().getMax();
  m_yfrom         = param.getYInterval().getMin();
  m_yto           = param.getYInterval().getMax();
  m_timeFrom      = param.getTInterval().getMin();
  m_timeTo        = param.getTInterval().getMax();
  m_pointCount    = param.m_pointCount;
  m_timeCount     = param.m_timeCount;
  m_name          = param.getName().cstr();
  m_machineCode   = param.m_machineCode ? TRUE : FALSE;
  m_includeTime   = param.m_includeTime ? TRUE : FALSE;
  m_doubleSided   = param.m_doubleSided ? TRUE : FALSE;

  UpdateData(false);
  enableTimeFields();

  SetWindowText(format(_T("Plot function (%s)"), param.getDisplayName().cstr()).cstr());
}

void CFunction2DSurfaceDlg::winToParam(Function2DSurfaceParameters &param) const {
  param.setName((LPCTSTR)m_name);
  param.m_expr        = m_expr;
  param.m_xInterval.setFrom(m_xfrom);
  param.m_xInterval.setTo(  m_xto);
  param.m_yInterval.setFrom(m_yfrom);
  param.m_yInterval.setTo(m_yto);
  param.m_tInterval.setFrom(m_timeFrom);
  param.m_tInterval.setTo(  m_timeTo);
  param.m_pointCount  = m_pointCount;
  param.m_timeCount   = m_timeCount;
  param.m_machineCode = m_machineCode?true:false;
  param.m_includeTime = m_includeTime?true:false;
  param.m_doubleSided = m_doubleSided?true:false;
}
