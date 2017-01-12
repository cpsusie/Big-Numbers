#include "stdafx.h"
#include <Math/Expression/Expression.h>
#include "Showgraf.h"
#include "FunctionGraphDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CFunctionGraphDlg::CFunctionGraphDlg(FunctionGraphParameters &param, int showFlags, CWnd *pParent)
: m_param(param)
, CDialog(CFunctionGraphDlg::IDD, pParent)
{
  if(!m_param.hasName()) {
    m_param.setDefaultName();
  }
  m_showFlags = showFlags;
}

void CFunctionGraphDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITEXPR, m_expr);
  DDX_Text(pDX, IDC_EDITNAME, m_name);
  DDX_Text(pDX, IDC_EDITXFROM, m_xFrom);
  DDX_Text(pDX, IDC_EDITXTO, m_xTo);
  DDX_Text(pDX, IDC_EDITSTEPS, m_steps);
  DDV_MinMaxUInt(pDX, m_steps, 1, 10000);
  DDX_CBString(pDX, IDC_COMBOSTYLE, m_style);
}

BEGIN_MESSAGE_MAP(CFunctionGraphDlg, CDialog)
  ON_WM_SIZE()
  ON_COMMAND(   ID_FILE_NEW                                       , OnFileNew                   )
  ON_COMMAND(   ID_FILE_OPEN                                      , OnFileOpen                  )
  ON_COMMAND(   ID_FILE_SAVE                                      , OnFileSave                  )
  ON_COMMAND(   ID_FILE_SAVE_AS                                   , OnFileSaveAs                )
  ON_COMMAND(   ID_EDIT_FINDMATCHINGPARENTESIS                    , OnEditFindmatchingparentesis)
  ON_COMMAND(   ID_GOTO_NAME                                      , OnGotoName                  )
  ON_COMMAND(   ID_GOTO_STYLE                                     , OnGotoStyle                 )
  ON_COMMAND(   ID_GOTO_EXPR                                      , OnGotoExpr                  )
  ON_COMMAND(   ID_GOTO_XINTERVAL                                 , OnGotoXInterval             )
  ON_COMMAND(   ID_GOTO_STEP                                      , OnGotoStep                  )
END_MESSAGE_MAP()

BOOL CFunctionGraphDlg::PreTranslateMessage(MSG* pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return CDialog::PreTranslateMessage(pMsg);
}

BOOL CFunctionGraphDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  m_accelTable = LoadAccelerators(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDR_ACCELERATOR_FUNCTION));
  if(!(m_showFlags & SHOW_INTERVAL)) {
    GetDlgItem(IDC_STATICINTERVAL)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_EDITXFROM     )->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STATICDASH    )->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_EDITXTO       )->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_EDITXFROM     )->ShowWindow(SW_HIDE);
  }
  if(!(m_showFlags & SHOW_STEP)) {
    GetDlgItem(IDC_STATICSTEPS   )->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_EDITSTEPS     )->ShowWindow(SW_HIDE);
  }
  LOGFONT lf;
  GetFont()->GetLogFont(&lf);
  _tcscpy(lf.lfFaceName, _T("courier new"));
  BOOL ret = m_exprFont.CreateFontIndirect(&lf);
  GetDlgItem(IDC_EDITEXPR)->SetFont(&m_exprFont, FALSE);

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDOK                , RELATIVE_POSITION    );
  m_layoutManager.addControl(IDCANCEL            , RELATIVE_POSITION    );
  m_layoutManager.addControl(IDC_STATICEXPRLABEL , PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_EDITEXPR        , RELATIVE_SIZE        | RESIZE_FONT);
  
  if(m_showFlags & SHOW_INTERVAL) {
    m_layoutManager.addControl(IDC_STATICINTERVAL, RELATIVE_Y_POS);
    m_layoutManager.addControl(IDC_EDITXFROM     , RELATIVE_Y_POS);
    m_layoutManager.addControl(IDC_STATICDASH    , RELATIVE_Y_POS);
    m_layoutManager.addControl(IDC_EDITXTO       , RELATIVE_Y_POS);
    m_layoutManager.addControl(IDC_EDITXFROM     , RELATIVE_Y_POS);
  }
  if(m_showFlags & SHOW_STEP) {
    m_layoutManager.addControl(IDC_STATICSTEPS   , RELATIVE_Y_POS);
    m_layoutManager.addControl(IDC_EDITSTEPS     , RELATIVE_Y_POS);
  }
  m_layoutManager.scaleFont(1.5,false);
  paramToWin(m_param);
  UpdateData(FALSE);
  gotoEditBox(this, IDC_EDITEXPR);

  return FALSE;
}

void CFunctionGraphDlg::OnOK() {
  if(!UpdateData() || !validate()) return;
  winToParam(m_param);
  CDialog::OnOK();
}

bool CFunctionGraphDlg::validate() {
  if(m_name.GetLength() == 0) {
    OnGotoName();
    MessageBox(_T("Must specify name"));
    return false;
  }

  Expression expr;
  expr.compile((LPCTSTR)m_expr, true);
  if(!expr.isOk()) {
    OnGotoExpr();
    if(expr.getErrors().size() == 0) {
      MessageBox(_T("Unknown error!!!"));
    } else {
      MessageBox(expr.getErrors()[0].cstr());
    }
    return false;
  }
  if(m_showFlags & SHOW_INTERVAL) {
    if(m_xFrom >= m_xTo) {
      OnGotoXInterval();
      MessageBox(_T("x-from must be less than x-to"));
      return false;
    }
  }
  return true;
}

void CFunctionGraphDlg::OnGotoExpr() {
  GetDlgItem(IDC_EDITEXPR)->SetFocus(); 
}

void CFunctionGraphDlg::OnGotoName() {
  gotoEditBox(this,IDC_EDITNAME);
}

void CFunctionGraphDlg::OnGotoStyle() {
  getStyleCombo()->SetFocus();
}

void CFunctionGraphDlg::OnGotoXInterval() {
  gotoEditBox(this,IDC_EDITXFROM);
}

void CFunctionGraphDlg::OnGotoStep() {
  gotoEditBox(this,IDC_EDITSTEPS);
}

void CFunctionGraphDlg::OnSize(UINT nType, int cx, int cy) {
  m_layoutManager.OnSize(nType,cx,cy);
  CDialog::OnSize(nType, cx, cy);
}

void CFunctionGraphDlg::OnFileNew() {
  FunctionGraphParameters param;
  paramToWin(param);
  UpdateData(false);
}

static const TCHAR *fileDialogExtensions = _T("Expression-files (*.exp)\0*.exp\0All files (*.*)\0*.*\0\0");

void CFunctionGraphDlg::OnFileOpen() {
  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrFilter = fileDialogExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Open expression");
  if((dlg.DoModal() != IDOK) || (_tcslen(dlg.m_ofn.lpstrFile) == 0)) {
    return;
  }

  try {
    FunctionGraphParameters param;
    const String fileName = dlg.m_ofn.lpstrFile;
    param.load(fileName);
    paramToWin(param);
    addToRecent(fileName);
    UpdateData(false);
  } catch(Exception e) {
    showException(e);
  }
}

void CFunctionGraphDlg::OnFileSave() {
  if(!UpdateData() || !validate()) return;
  
  FunctionGraphParameters param;
  winToParam(param);

  if(param.hasDefaultName()) {
    saveAs(param);
  } else {
    save(param.getFullName(), param);
  }
}

void CFunctionGraphDlg::OnFileSaveAs() {
  if(!UpdateData() || !validate()) return;
  
  FunctionGraphParameters param;
  winToParam(param);
  saveAs(param);
}

void CFunctionGraphDlg::saveAs(FunctionGraphParameters &param) {
  CString objname = param.getFullName().cstr();
  CFileDialog dlg(FALSE,_T("*.exp"), objname);
  dlg.m_ofn.lpstrFilter = fileDialogExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Save expression");
  if((dlg.DoModal() != IDOK) ||(_tcslen(dlg.m_ofn.lpstrFile) == 0)) {
    return;
  }
  save(dlg.m_ofn.lpstrFile, param);
}

void CFunctionGraphDlg::save(const String &fileName, FunctionGraphParameters &param) {
  try {
    param.save(fileName);
    paramToWin(param);
    addToRecent(fileName);
    UpdateData(FALSE);
  } catch(Exception e) {
    showException(e);
  }
}

void CFunctionGraphDlg::addToRecent(const String &fileName) {
  AfxGetApp()->AddToRecentFileList(fileName.cstr());
}
/*
void CFunctionGraphDlg::OnFindMatchingParanthes() {
  if(getFocusCtrlId(this) != IDC_EDITEXPR) {
    return;
  }
  gotoMatchingParanthes(this, IDC_EDITEXPR);
}
*/
void CFunctionGraphDlg::OnEditFindmatchingparentesis() {
  gotoMatchingParanthes(this, IDC_EDITEXPR);
}

void CFunctionGraphDlg::paramToWin(const FunctionGraphParameters &param) {
  m_fullName = param.getFullName();
  m_name     = param.getPartialName().cstr();
  m_style    = GraphParameters::graphStyleToString(param.m_style);
  getColorButton()->SetColor(param.m_color);
  m_expr     = param.m_expr.cstr();
  m_xFrom    = param.m_interval.getMin();
  m_xTo      = param.m_interval.getMax();
  m_steps    = param.m_steps;
}

void CFunctionGraphDlg::winToParam(FunctionGraphParameters &param) {
  FileNameSplitter info(m_fullName);
  if(info.getFileName() != m_name) {
    m_fullName = info.setFileName(m_name).getFullPath();
  }
  param.setName(m_fullName);
  param.m_expr  = m_expr;
  param.m_style = (GraphStyle)getStyleCombo()->GetCurSel();
  param.m_color = getColorButton()->GetColor();
  param.m_interval.setFrom(m_xFrom);
  param.m_interval.setTo(m_xTo);
  param.m_steps = m_steps;
}
