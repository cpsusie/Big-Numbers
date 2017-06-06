#include "stdafx.h"
#include <Math/Expression/Expression.h>
#include "Showgraf.h"
#include "ParametricGraphDlg.h"

CParametricGraphDlg::CParametricGraphDlg(ParametricGraphParameters &param, CWnd *pParent)
  : m_param(param)
  , CDialog(IDD, pParent)
{
  if(!m_param.hasName()) {
    m_param.setDefaultName();
  }
}

CParametricGraphDlg::~CParametricGraphDlg() {
}

void CParametricGraphDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITEXPRX, m_exprX);
  DDX_Text(pDX, IDC_EDITEXPRY, m_exprY);
  DDX_Text(pDX, IDC_EDITNAME, m_name);
  DDX_Text(pDX, IDC_EDITTFROM, m_tFrom);
  DDX_Text(pDX, IDC_EDITTTO, m_tTo);
  DDX_Text(pDX, IDC_EDITSTEPS, m_steps);
  DDV_MinMaxUInt(pDX, m_steps, 1, 10000);
  DDX_CBString(pDX, IDC_COMBOSTYLE, m_style);
}

BEGIN_MESSAGE_MAP(CParametricGraphDlg, CDialog)
  ON_WM_SIZE()
  ON_COMMAND(   ID_FILE_NEW                                       , OnFileNew                   )
  ON_COMMAND(   ID_FILE_OPEN                                      , OnFileOpen                  )
  ON_COMMAND(   ID_FILE_SAVE                                      , OnFileSave                  )
  ON_COMMAND(   ID_FILE_SAVE_AS                                   , OnFileSaveAs                )
  ON_COMMAND(   ID_EDIT_FINDMATCHINGPARENTESIS                    , OnEditFindmatchingparentesis)
  ON_COMMAND(   ID_GOTO_NAME                                      , OnGotoName                  )
  ON_COMMAND(   ID_GOTO_STYLE                                     , OnGotoStyle                 )
  ON_COMMAND(   ID_GOTO_EXPRX                                     , OnGotoExprX                 )
  ON_COMMAND(   ID_GOTO_EXPRY                                     , OnGotoExprY                 )
  ON_COMMAND(   ID_GOTO_TINTERVAL                                 , OnGotoTInterval             )
  ON_COMMAND(   ID_GOTO_STEP                                      , OnGotoStep                  )
END_MESSAGE_MAP()


BOOL CParametricGraphDlg::PreTranslateMessage(MSG* pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

BOOL CParametricGraphDlg::OnInitDialog() {
  __super::OnInitDialog();
  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_PARAMETRICCURVE));
  LOGFONT lf;
  GetFont()->GetLogFont(&lf);
  _tcscpy(lf.lfFaceName, _T("courier new"));
  BOOL ret = m_exprFont.CreateFontIndirect(&lf);
  GetDlgItem(IDC_EDITEXPRX)->SetFont(&m_exprFont, FALSE);
  GetDlgItem(IDC_EDITEXPRY)->SetFont(&m_exprFont, FALSE);

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDOK                , RELATIVE_POSITION    );
  m_layoutManager.addControl(IDCANCEL            , RELATIVE_POSITION    );
  m_layoutManager.addControl(IDC_STATICEXPRXLABEL, PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_EDITEXPRX       , RELATIVE_WIDTH | RESIZE_FONT | PCT_RELATIVE_BOTTOM);
  m_layoutManager.addControl(IDC_STATICEXPRYLABEL, PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_EDITEXPRY       , RELATIVE_WIDTH | RESIZE_FONT | PCT_RELATIVE_TOP | RELATIVE_BOTTOM);
  
  m_layoutManager.addControl(IDC_STATICTINTERVAL , RELATIVE_Y_POS);
  m_layoutManager.addControl(IDC_EDITTFROM       , RELATIVE_Y_POS);
  m_layoutManager.addControl(IDC_STATICDASH      , RELATIVE_Y_POS);
  m_layoutManager.addControl(IDC_EDITTTO         , RELATIVE_Y_POS);
  m_layoutManager.addControl(IDC_EDITTFROM       , RELATIVE_Y_POS);
  m_layoutManager.addControl(IDC_STATICSTEPS     , RELATIVE_Y_POS);
  m_layoutManager.addControl(IDC_EDITSTEPS       , RELATIVE_Y_POS);

  m_layoutManager.scaleFont(1.5,false);
  paramToWin(m_param);
  UpdateData(FALSE);
  gotoEditBox(this, IDC_EDITEXPRX);

  return FALSE;
}

void CParametricGraphDlg::OnOK() {
  if(!UpdateData() || !validate()) return;
  winToParam(m_param);
  __super::OnOK();
}

bool CParametricGraphDlg::validate() {
  if(m_name.GetLength() == 0) {
    OnGotoName();
    MessageBox(_T("Must specify name"));
    return false;
  }

  Expression expr;
  expr.compile((LPCTSTR)m_exprX, true);
  if(!expr.isOk()) {
    OnGotoExprX();
    if(expr.getErrors().size() == 0) {
      MessageBox(_T("Unknown error!!!"));
    } else {
      MessageBox(expr.getErrors()[0].cstr());
    }
    return false;
  }
  expr.compile((LPCTSTR)m_exprY, true);
  if(!expr.isOk()) {
    OnGotoExprY();
    if(expr.getErrors().size() == 0) {
      MessageBox(_T("Unknown error!!!"));
    } else {
      MessageBox(expr.getErrors()[0].cstr());
    }
    return false;
  }
  if(m_tFrom >= m_tTo) {
    OnGotoTInterval();
    MessageBox(_T("x-from must be less than x-to"));
    return false;
  }
  return true;
}

void CParametricGraphDlg::OnGotoExprX() {
  GetDlgItem(IDC_EDITEXPRX)->SetFocus(); 
}

void CParametricGraphDlg::OnGotoExprY() {
  GetDlgItem(IDC_EDITEXPRY)->SetFocus(); 
}

void CParametricGraphDlg::OnGotoName() {
  gotoEditBox(this,IDC_EDITNAME);
}

void CParametricGraphDlg::OnGotoStyle() {
  getStyleCombo()->SetFocus();
}

void CParametricGraphDlg::OnGotoTInterval() {
  gotoEditBox(this,IDC_EDITTFROM);
}

void CParametricGraphDlg::OnGotoStep() {
  gotoEditBox(this,IDC_EDITSTEPS);
}

void CParametricGraphDlg::OnSize(UINT nType, int cx, int cy) {
  m_layoutManager.OnSize(nType,cx,cy);
  __super::OnSize(nType, cx, cy);
}

void CParametricGraphDlg::OnFileNew() {
  ParametricGraphParameters param;
  paramToWin(param);
  UpdateData(false);
}

static const TCHAR *fileDialogExtensions = _T("Parametric curve-files (*.par)\0*.par\0All files (*.*)\0*.*\0\0");

void CParametricGraphDlg::OnFileOpen() {
  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrFilter = fileDialogExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Open parametric curve");
  if((dlg.DoModal() != IDOK) || (_tcslen(dlg.m_ofn.lpstrFile) == 0)) {
    return;
  }

  try {
    ParametricGraphParameters param;
    const String fileName = dlg.m_ofn.lpstrFile;
    param.load(fileName);
    paramToWin(param);
    addToRecent(fileName);
    UpdateData(false);
  } catch(Exception e) {
    showException(e);
  }
}

void CParametricGraphDlg::OnFileSave() {
  if(!UpdateData() || !validate()) return;
  
  ParametricGraphParameters param;
  winToParam(param);

  if(param.hasDefaultName()) {
    saveAs(param);
  } else {
    save(param.getName(), param);
  }
}

void CParametricGraphDlg::OnFileSaveAs() {
  if(!UpdateData() || !validate()) return;
  
  ParametricGraphParameters param;
  winToParam(param);
  saveAs(param);
}

void CParametricGraphDlg::saveAs(ParametricGraphParameters &param) {
  CString objname = param.getName().cstr();
  CFileDialog dlg(FALSE,_T("*.par"), objname);
  dlg.m_ofn.lpstrFilter = fileDialogExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Save parametric curve");
  if((dlg.DoModal() != IDOK) ||(_tcslen(dlg.m_ofn.lpstrFile) == 0)) {
    return;
  }
  save(dlg.m_ofn.lpstrFile, param);
}

void CParametricGraphDlg::save(const String &fileName, ParametricGraphParameters &param) {
  try {
    param.save(fileName);
    paramToWin(param);
    addToRecent(fileName);
    UpdateData(FALSE);
  } catch(Exception e) {
    showException(e);
  }
}

void CParametricGraphDlg::addToRecent(const String &fileName) {
  theApp.AddToRecentFileList(fileName.cstr());
}
/*
void CParametricGraphDlg::OnFindMatchingParanthes() {
  if(getFocusCtrlId(this) != IDC_EDITEXPR) {
    return;
  }
  gotoMatchingParanthes(this, IDC_EDITEXPR);
}
*/
void CParametricGraphDlg::OnEditFindmatchingparentesis() {
  gotoMatchingParanthes(this, IDC_EDITEXPR);
}

void CParametricGraphDlg::paramToWin(const ParametricGraphParameters &param) {
  m_fullName = param.getName();
  m_name     = param.getDisplayName().cstr();
  m_style    = GraphParameters::graphStyleToString(param.m_style);
  getColorButton()->SetColor(param.m_color);
  m_exprX    = param.m_exprX.cstr();
  m_exprY    = param.m_exprY.cstr();
  m_tFrom    = param.m_interval.getMin();
  m_tTo      = param.m_interval.getMax();
  m_steps    = param.m_steps;
}

void CParametricGraphDlg::winToParam(ParametricGraphParameters &param) {
  FileNameSplitter info(m_fullName);
  if(info.getFileName() != m_name) {
    m_fullName = info.setFileName(m_name).getFullPath();
  }
  param.setName(m_fullName);
  param.m_exprX = m_exprX;
  param.m_exprY = m_exprY;
  param.m_style = (GraphStyle)getStyleCombo()->GetCurSel();
  param.m_color = getColorButton()->GetColor();
  param.m_interval.setFrom(m_tFrom);
  param.m_interval.setTo(m_tTo);
  param.m_steps = m_steps;
}
