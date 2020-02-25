#include "stdafx.h"
#include "WinDiff.h"
#include "FindDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CFindDlg::CFindDlg(FindParameters &param, TextContainer *tc, CWnd *pParent)
: CDialog(IDD, pParent)
, m_param(param), m_textContainer(tc)
{
  m_matchCase      = param.m_matchCase;
  m_matchWholeWord = param.m_matchWholeWord;
  m_useRegex       = param.m_useRegex;
  m_findWhat       = param.m_findWhat.cstr();
  m_diffOnly       = param.m_diffOnly;
  m_nonDiffOnly    = param.m_nonDiffOnly;
}

void CFindDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Check(   pDX, IDC_CHECKMATCHCASE     , m_matchCase     );
  DDX_Check(   pDX, IDC_CHECKMATCHWHOLEWORD, m_matchWholeWord);
  DDX_Check(   pDX, IDC_CHECKUSEREGEX      , m_useRegex      );
  DDX_CBString(pDX, IDC_COMBOFINDWHAT      , m_findWhat      );
  DDX_Check(   pDX, IDC_CHECKLIMITDIFF     , m_diffOnly      );
  DDX_Check(   pDX, IDC_CHECKLIMITNONDIFF  , m_nonDiffOnly   );
}

BEGIN_MESSAGE_MAP(CFindDlg, CDialog)
  ON_WM_DRAWITEM()
  ON_BN_CLICKED(IDC_FINDNEXT               , OnFindNext                     )
  ON_BN_CLICKED(IDC_BUTTONREGSYMBOLSMENU   , OnButtonRegSymbolsMenu         )
  ON_BN_CLICKED(IDC_CHECKLIMITDIFF         , OnCheckLimitDiff               )
  ON_BN_CLICKED(IDC_CHECKLIMITNONDIFF      , OnCheckLimitNonDiff            )
  ON_COMMAND(ID_REGSYMBOLS_ANYCHAR         , OnRegSymbolsAnyChar            )
  ON_COMMAND(ID_REGSYMBOLS_CHARINRANGE     , OnRegSymbolsCharInRange        )
  ON_COMMAND(ID_REGSYMBOLS_CHARNOTINRANGE  , OnRegSymbolsCharNotInRange     )
  ON_COMMAND(ID_REGSYMBOLS_BEGINNINGOFLINE , OnRegSymbolsBeginningOfLine    )
  ON_COMMAND(ID_REGSYMBOLS_ENDOFLINE       , OnRegSymbolsEndOfLine          )
  ON_COMMAND(ID_REGSYMBOLS_0ORMORE         , OnRegSymbols0orMoreOccurrences )
  ON_COMMAND(ID_REGSYMBOLS_1ORMORE         , OnRegSymbols1orMoreOccurrences )
  ON_COMMAND(ID_REGSYMBOLS_0OR1            , OnRegSymbols0or1Occurence      )
  ON_COMMAND(ID_REGSYMBOLS_OR              , OnRegSymbolsOr                 )
  ON_COMMAND(ID_REGSYMBOLS_GROUP           , OnRegSymbolsGroup              )
  ON_CBN_SETFOCUS(IDC_COMBOFINDWHAT        , OnSetFocusComboFindWhat        )
  ON_CBN_KILLFOCUS(IDC_COMBOFINDWHAT       , OnKillFocusComboFindWhat       )
  ON_COMMAND(ID_GOTOFINDWHAT               , OnGotoFindWhat                 )
  ON_CBN_SELENDOK(IDC_COMBOFINDWHAT        , OnSelEndOkComboFindWhat        )
  ON_CBN_SELCHANGE(IDC_COMBOFINDWHAT       , OnSelChangeComboFindWhat       )
END_MESSAGE_MAP()

BOOL CFindDlg::OnInitDialog() {
  __super::OnInitDialog();

  m_font.CreateFontIndirect(&getOptions().m_logFont);
//  SetFont(&m_font);

  m_findWhatCombo.substituteControl( this, IDC_COMBOFINDWHAT, _T("FindHistory"));
  m_findWhatCombo.SetFont(&m_font);

  m_currentControl = 0;
  m_selStart       = 0;
  m_selEnd         = m_findWhat.GetLength();

  if(m_param.m_dirUp) {
    ((CButton*)GetDlgItem(IDC_RADIOUP))->SetCheck(1);
  } else {
    ((CButton*)GetDlgItem(IDC_RADIODOWN))->SetCheck(1);
  }

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_ACCELERATORFIND));
  UpdateData(false);
  return false;
}

void CFindDlg::OnFindNext() {
  UpdateData();
  m_param.m_findWhat       = m_findWhat;
  m_param.m_matchCase      = m_matchCase      ? true : false;
  m_param.m_matchWholeWord = m_matchWholeWord ? true : false;
  m_param.m_useRegex       = m_useRegex       ? true : false;
  m_param.m_dirUp          = ((CButton*)GetDlgItem(IDC_RADIOUP))->GetCheck() ? true : false;
  m_param.m_skipCurrent    = false;
  m_param.m_diffOnly       = m_diffOnly       ? true : false;
  m_param.m_nonDiffOnly    = m_nonDiffOnly    ? true : false;
  try {
    SearchMachine sm(m_param);
    m_findWhatCombo.updateList();
    if(m_textContainer->searchText(m_param).isEmpty()) {
      if(m_param.m_useRegex) {
        showWarning(_T("Cannot find a match for the regular expression '%s'."),m_param.m_findWhat.cstr());
      } else {
        showWarning(_T("Cannot find the String '%s'."),m_param.m_findWhat.cstr());
      }
      return;
    }
  } catch(Exception e) {
    GetDlgItem(IDC_COMBOFINDWHAT)->SetFocus();
    showException(e);
    return;
  }
  __super::OnOK();
}

void CFindDlg::OnCancel() {
  __super::OnCancel();
}

void CFindDlg::OnButtonRegSymbolsMenu() {
  CMenu menu;
  int ret = menu.LoadMenu(IDR_MENUREGSYMBOLS);
  if(!ret) {
    showError(_T("Loadmenu failed"));
    return;
  }
  CRect r;
  GetDlgItem(IDC_BUTTONREGSYMBOLSMENU)->GetWindowRect(&r);
  menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,r.right,r.top, this );
}

void CFindDlg::addRegexSymbol(const TCHAR *s, int cursorpos) {
  CComboBox *b = (CComboBox*)GetDlgItem(IDC_COMBOFINDWHAT);

  UpdateData();

  String reg = m_findWhat.GetBuffer(m_findWhat.GetLength());

  reg = substr(reg,0,m_selStart) + s + substr(reg,m_selEnd,reg.length());

  m_findWhat  = reg.cstr();
  m_useRegex  = true;
  m_selStart  = m_selEnd = m_selStart + cursorpos;
  UpdateData(false);

  b->SetFocus();
}

void CFindDlg::OnRegSymbolsAnyChar() {
  addRegexSymbol(_T("."),1);
}

void CFindDlg::OnRegSymbolsCharInRange() {
  addRegexSymbol(_T("[]"),1);
}

void CFindDlg::OnRegSymbolsCharNotInRange() {
  addRegexSymbol(_T("[^]"),2);
}

void CFindDlg::OnRegSymbolsBeginningOfLine() {
  addRegexSymbol(_T("^"),1);
}

void CFindDlg::OnRegSymbolsEndOfLine() {
  addRegexSymbol(_T("$"),1);
}

void CFindDlg::OnRegSymbols0orMoreOccurrences() {
  addRegexSymbol(_T("*"),1);
}

void CFindDlg::OnRegSymbols1orMoreOccurrences() {
  addRegexSymbol(_T("+"),1);
}

void CFindDlg::OnRegSymbols0or1Occurence() {
  addRegexSymbol(_T("?"),1);
}

void CFindDlg::OnRegSymbolsOr() {
  addRegexSymbol(_T("\\|"),2);
}

void CFindDlg::OnRegSymbolsGroup() {
  addRegexSymbol(_T("\\(\\)"),2);
}

void CFindDlg::OnSetFocusComboFindWhat() {
  CComboBox *b = (CComboBox*)GetDlgItem(IDC_COMBOFINDWHAT);
  b->SetEditSel(m_selStart,m_selEnd);
  m_currentControl = IDC_COMBOFINDWHAT;
}

void CFindDlg::OnKillFocusComboFindWhat() {
  m_currentControl = 0;
}

BOOL CFindDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }

  BOOL ret = __super::PreTranslateMessage(pMsg);

  if(m_currentControl == IDC_COMBOFINDWHAT) {
    CComboBox *b = (CComboBox*)GetDlgItem(IDC_COMBOFINDWHAT);
    DWORD w = b->GetEditSel();
    m_selStart = w & 0xff;
    m_selEnd   = w >> 16;
  }

  return ret;
}

void CFindDlg::OnGotoFindWhat() {
  GetDlgItem(IDC_COMBOFINDWHAT)->SetFocus();
}

void CFindDlg::OnSelEndOkComboFindWhat() {
  CComboBox *b = (CComboBox*)GetDlgItem(IDC_COMBOFINDWHAT);
  int f = b->GetCurSel();
}

void CFindDlg::OnSelChangeComboFindWhat() {
  CComboBox *b = (CComboBox*)GetDlgItem(IDC_COMBOFINDWHAT);
  int f = b->GetCurSel();
}

void drawTriangle(CWnd *wnd) {
  CClientDC dc(wnd);

  CRgn rgn;
  CPoint p[] = { CPoint(2,2),CPoint(6,6), CPoint(2,10) };
  CBrush brush;
  brush.CreateSolidBrush(RGB(0,0,0));
  rgn.CreatePolygonRgn(p,3,ALTERNATE);
  dc.FillRgn(&rgn,&brush);
}

void CFindDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) {
  if(nIDCtl == IDC_BUTTONREGSYMBOLSMENU) {
    drawTriangle(GetDlgItem(IDC_BUTTONREGSYMBOLSMENU));
  }

  __super::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CFindDlg::OnCheckLimitDiff() {
  UpdateData();
  if(m_diffOnly) {
    m_nonDiffOnly = false;
    UpdateData(false);
  }
}

void CFindDlg::OnCheckLimitNonDiff() {
  UpdateData();
  if(m_nonDiffOnly) {
    m_diffOnly = false;
    UpdateData(false);
  }
}
