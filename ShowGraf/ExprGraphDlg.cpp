#include "stdafx.h"
#include <Math/Expression/Expression.h>
#include "Showgraf.h"
#include "ExprGraphDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CExprGraphDlg::CExprGraphDlg(ExpressionGraphParameters &param, int showFlags, CWnd* pParent)  : m_param(param), CDialog(CExprGraphDlg::IDD, pParent) {
    if(!param.hasName()) {
      param.setDefaultName();
    }
    paramToWin(param);
    m_showFlags = showFlags;
}

void CExprGraphDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDITEXPR , m_expr );
    DDX_Text(pDX, IDC_EDITNAME , m_name );
    DDX_Text(pDX, IDC_EDITXFROM, m_xfrom);
    DDX_Text(pDX, IDC_EDITXTO  , m_xto  );
    DDX_Text(pDX, IDC_EDITSTEPS, m_steps);
    DDV_MinMaxUInt(pDX, m_steps, 1, 10000);
    DDX_CBString(pDX, IDC_COMBOSTYLE, m_style);
}

BEGIN_MESSAGE_MAP(CExprGraphDlg, CDialog)
  ON_WM_PAINT()
  ON_WM_SIZE()
  ON_COMMAND(   ID_FILE_OPEN                  , OnFileOpen                  )
  ON_COMMAND(   ID_FILE_SAVE                  , OnFileSave                  )
  ON_COMMAND(   ID_FILE_SAVE_AS               , OnFileSaveAs                )
  ON_COMMAND(   ID_EDIT_FINDMATCHINGPARENTESIS, OnEditFindmatchingparentesis)
  ON_COMMAND(   ID_GOTO_NAME                  , OnGotoName                  )
  ON_COMMAND(   ID_GOTO_STYLE                 , OnGotoStyle                 )
  ON_COMMAND(   ID_GOTO_EXPR                  , OnGotoExpr                  )
  ON_COMMAND(   ID_GOTO_XINTERVAL             , OnGotoXInterval             )
  ON_COMMAND(   ID_GOTO_STEP                  , OnGotoStep                  )
  ON_BN_CLICKED(IDC_BUTTONCOLOR               , OnButtonColor               )
END_MESSAGE_MAP()

BOOL CExprGraphDlg::PreTranslateMessage(MSG* pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return CDialog::PreTranslateMessage(pMsg);
}

BOOL CExprGraphDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  m_accelTable = LoadAccelerators(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDR_ACCELERATOR_EXPR));
  GetDlgItem(IDC_EDITNAME)->SetFocus(); 
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
#include <tchar.h>
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
  gotoEditBox(this, IDC_EDITEXPR);

  return FALSE;
}


void CExprGraphDlg::OnOK() {
  UpdateData();
  if(!validate()) {
    return;
  }
  winToParam(m_param);
  CDialog::OnOK();
}

bool CExprGraphDlg::validate() {
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
    if(m_xfrom >= m_xto) {
      OnGotoXInterval();
      MessageBox(_T("x-from must be less than x-to"));
      return false;
    }
  }
  return true;
}

CComboBox *CExprGraphDlg::getStyleCombo() {
  return (CComboBox*)GetDlgItem(IDC_COMBOSTYLE);
}

void CExprGraphDlg::OnGotoExpr() {
  GetDlgItem(IDC_EDITEXPR)->SetFocus(); 
}

void CExprGraphDlg::OnGotoName() {
  gotoEditBox(this,IDC_EDITNAME);
}

void CExprGraphDlg::OnGotoStyle() {
  getStyleCombo()->SetFocus();
}

void CExprGraphDlg::OnGotoXInterval() {
  gotoEditBox(this,IDC_EDITXFROM);
}

void CExprGraphDlg::OnGotoStep() {
  gotoEditBox(this,IDC_EDITSTEPS);
}

void CExprGraphDlg::OnButtonColor() {
  CColorDialog dlg(m_color);
  dlg.m_cc.Flags |= CC_RGBINIT;
  if(dlg.DoModal() == IDOK) {
    m_color = dlg.m_cc.rgbResult;
    Invalidate();
  }
}

void CExprGraphDlg::OnPaint() {
  CPaintDC dc(this); // device context for painting

  WINDOWPLACEMENT wp;
  GetDlgItem(IDC_STATICCOLOR)->GetWindowPlacement(&wp);
  dc.FillSolidRect(&wp.rcNormalPosition, m_color);
}

void CExprGraphDlg::OnSize(UINT nType, int cx, int cy) {
  m_layoutManager.OnSize(nType,cx,cy);
  CDialog::OnSize(nType, cx, cy);
}

static const TCHAR *fileDialogExtensions = _T("Expression-files (*.exp)\0*.exp\0All files (*.*)\0*.*\0\0");

void CExprGraphDlg::OnFileOpen() {
  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrFilter = fileDialogExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Open expression");
  if((dlg.DoModal() != IDOK) || (_tcslen(dlg.m_ofn.lpstrFile) == 0)) {
    return;
  }

  FILE *f = NULL;
  try {
    f = FOPEN(dlg.m_ofn.lpstrFile, "rb");

    ExpressionGraphParameters param;
    param.read(f);
    fclose(f); f = NULL;
    param.setName(dlg.m_ofn.lpstrFile);
    paramToWin(param);
    addToRecent(dlg.m_ofn.lpstrFile);
    UpdateData(false);
  } catch(Exception e) {
    if(f) {
      fclose(f);
    }
    showException(e);
  }
}

void CExprGraphDlg::OnFileSave() {
  UpdateData();
  if(!validate()) {
    return;
  }
  
  ExpressionGraphParameters param;
  winToParam(param);

  if(param.hasDefaultName()) {
    saveAs(param);
  } else {
    save(param.getFullName(), param);
  }
}

void CExprGraphDlg::OnFileSaveAs() {
  UpdateData();
  if(!validate()) {
    return;
  }
  
  ExpressionGraphParameters param;
  winToParam(param);
  saveAs(param);
}

void CExprGraphDlg::saveAs(ExpressionGraphParameters &param) {
  CString objname = param.getFullName().cstr();
  CFileDialog dlg(FALSE,_T("*.exp"), objname);
  dlg.m_ofn.lpstrFilter = fileDialogExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Save expression");
  if((dlg.DoModal() != IDOK) ||(_tcslen(dlg.m_ofn.lpstrFile) == 0)) {
    return;
  }
  save(dlg.m_ofn.lpstrFile, param);
}

void CExprGraphDlg::save(const String &fileName, ExpressionGraphParameters &param) {
  FILE *f = NULL;
  try {
    f = FOPEN(fileName, "wb");
    param.setName(fileName);
    param.write(f);
    paramToWin(param);
    fclose(f); f = NULL;
    addToRecent(fileName);
    UpdateData(FALSE);
  } catch(Exception e) {
    if(f) {
      fclose(f);
    }
    showException(e);
  }
}

void CExprGraphDlg::addToRecent(const String &fileName) {
  AfxGetApp()->AddToRecentFileList(fileName.cstr());
}
/*
void CExprGraphDlg::OnFindMatchingParanthes() {
  if(getFocusCtrlId(this) != IDC_EDITEXPR) {
    return;
  }
  gotoMatchingParanthes(this, IDC_EDITEXPR);
}
*/
void CExprGraphDlg::OnEditFindmatchingparentesis() {
  gotoMatchingParanthes(this, IDC_EDITEXPR);
}

void CExprGraphDlg::paramToWin(const ExpressionGraphParameters &param) {
  m_fullName = param.getFullName();
  m_name     = param.getPartialName().cstr();
  m_style    = GraphParameters::graphStyleToString(param.m_style).cstr();
  m_color    = param.m_color;
  m_expr     = param.m_expr.cstr();
  m_xfrom    = param.m_interval.getMin();
  m_xto      = param.m_interval.getMax();
  m_steps    = param.m_steps;
}

void CExprGraphDlg::winToParam(ExpressionGraphParameters &param) {
  FileNameSplitter info(m_fullName);
  if(info.getFileName() != m_name) {
    m_fullName = info.setFileName(m_name).getFullPath();
  }
  param.setName(m_fullName);
  param.m_expr  = m_expr;
  param.m_style = (GraphStyle)getStyleCombo()->GetCurSel();
  param.m_color = m_color;
  param.m_interval.setFrom(m_xfrom);
  param.m_interval.setTo(m_xto);
  param.m_steps = m_steps;
}

