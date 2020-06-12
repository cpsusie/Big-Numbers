#include "stdafx.h"
#include <FileNameSplitter.h>
#include "FunctionGraphDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CFunctionGraphDlg::CFunctionGraphDlg(FunctionGraphParameters &param, int showFlags, CWnd *pParent)
: SaveLoadExprDialog<FunctionGraphParameters>(IDD, pParent, param, _T("expression"), _T("exp"))
, m_showFlags(showFlags)
, m_createListFile(FALSE)
{
}

void CFunctionGraphDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_CBString(  pDX, IDC_COMBOSTYLE         , m_style         );
  DDX_Check(     pDX, IDC_CHECKCREATELISTFILE, m_createListFile);
  DDX_Text(      pDX, IDC_EDITEXPR           , m_expr          );
  DDX_Text(      pDX, IDC_EDITXFROM          , m_xFrom         );
  DDX_Text(      pDX, IDC_EDITXTO            , m_xTo           );
  DDX_Text(      pDX, IDC_EDITSTEPS          , m_steps         );
  DDV_MinMaxUInt(pDX, m_steps, 1, 10000);
}

BEGIN_MESSAGE_MAP(CFunctionGraphDlg, CDialog)
  ON_COMMAND(   ID_FILE_NEW                                       , OnFileNew                   )
  ON_COMMAND(   ID_FILE_OPEN                                      , OnFileOpen                  )
  ON_COMMAND(   ID_FILE_SAVE                                      , OnFileSave                  )
  ON_COMMAND(   ID_FILE_SAVE_AS                                   , OnFileSaveAs                )
  ON_COMMAND(   ID_EDIT_FINDMATCHINGPARENTESIS                    , OnEditFindmatchingparentesis)
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

void CFunctionGraphDlg::addToRecent(const String &fileName) {
  theApp.AddToRecentFileList(fileName.cstr());
}

String CFunctionGraphDlg::getListFileName() const {
  if(!m_createListFile) return __super::getListFileName();
  return FileNameSplitter(getData().getName()).setExtension(_T("lst")).getFullPath();
}

void CFunctionGraphDlg::OnEditFindmatchingparentesis() {
  gotoMatchingParentesis();
}

void CFunctionGraphDlg::OnButtonHelp() {
  handleExprHelpButtonClick(IDC_BUTTON_HELP);
}

void CFunctionGraphDlg::paramToWin(const FunctionGraphParameters &param) {
  m_style    = param.getGraphStyleStr();
  getColorButton()->SetColor(param.getColor());
  m_expr     = param.getExprText().cstr();
  m_xFrom    = param.getInterval().getMin();
  m_xTo      = param.getInterval().getMax();
  m_steps    = param.getSteps();
  __super::paramToWin(param);
}

bool CFunctionGraphDlg::winToParam(FunctionGraphParameters &param) {
  if(!__super::winToParam(param)) return false;
  param.setExprText((LPCTSTR)m_expr);
  param.setGraphStyle((GraphStyle)getStyleCombo()->GetCurSel());
  param.setColor(getColorButton()->GetColor());
  param.setInteval(m_xFrom,m_xTo);
  param.setSteps(m_steps);
  return true;
}
