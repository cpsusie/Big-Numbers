#include "stdafx.h"
#include "FunctionGraphDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CFunctionGraphDlg::CFunctionGraphDlg(FunctionGraphParameters &param, int showFlags, CWnd *pParent)
: SaveLoadExprDialog<FunctionGraphParameters>(IDD, pParent, param, _T("expression"), _T("exp"))
, m_showFlags(showFlags)
{
}

void CFunctionGraphDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITEXPR, m_expr);
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
  ON_COMMAND(   ID_GOTO_STYLE                                     , OnGotoStyle                 )
  ON_COMMAND(   ID_GOTO_EXPR                                      , OnGotoExpr                  )
  ON_COMMAND(   ID_GOTO_XINTERVAL                                 , OnGotoXInterval             )
  ON_COMMAND(   ID_GOTO_STEP                                      , OnGotoStep                  )
  ON_BN_CLICKED(IDC_BUTTON_HELP                                   , OnButtonHelp                )
END_MESSAGE_MAP()

BOOL CFunctionGraphDlg::OnInitDialog() {
  __super::OnInitDialog();

  createExprHelpButton(IDC_BUTTON_HELP, IDC_EDITEXPR);

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
  gotoEditBox(this, IDC_EDITEXPR);
  return FALSE;
}

bool CFunctionGraphDlg::validate() {
  if(!validateExpr(IDC_EDITEXPR)) {
    return false;
  }
  if(m_showFlags & SHOW_INTERVAL) {
    if(!validateInterval(IDC_EDITXFROM, IDC_EDITXTO)) {
      return false;
    }
  }
  return true;
}

void CFunctionGraphDlg::OnGotoExpr() {
  gotoExpr(IDC_EDITEXPR);
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

void CFunctionGraphDlg::addToRecent(const String &fileName) {
  theApp.AddToRecentFileList(fileName.cstr());
}

void CFunctionGraphDlg::OnEditFindmatchingparentesis() {
  gotoMatchingParentesis();
}

void CFunctionGraphDlg::OnButtonHelp() {
  handleExprHelpButtonClick(IDC_BUTTON_HELP);
}

void CFunctionGraphDlg::paramToWin(const FunctionGraphParameters &param) {
  m_style    = GraphParameters::graphStyleToString(param.m_style);
  getColorButton()->SetColor(param.m_color);
  m_expr     = param.m_expr.cstr();
  m_xFrom    = param.m_interval.getMin();
  m_xTo      = param.m_interval.getMax();
  m_steps    = param.m_steps;
  __super::paramToWin(param);
}

void CFunctionGraphDlg::winToParam(FunctionGraphParameters &param) const {
  param.m_expr  = m_expr;
  param.m_style = (GraphStyle)getStyleCombo()->GetCurSel();
  param.m_color = getColorButton()->GetColor();
  param.m_interval.setFrom(m_xFrom);
  param.m_interval.setTo(m_xTo);
  param.m_steps = m_steps;
  __super::winToParam(param);
}
