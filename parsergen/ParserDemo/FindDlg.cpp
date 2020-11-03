#include "stdafx.h"
#include "FindDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

FindDlg::FindDlg(FindParameter &param, TextContainer &tc, CWnd *pParent) : CDialog(FindDlg::IDD, pParent) , m_param(param), m_TextContainer(tc) {
  m_matchCase      = param.m_matchCase;
  m_matchWholeWord = param.m_matchWholeWord;
  m_useRegExp      = param.m_useRegExp;
  m_findWhat       = param.m_findWhat.cstr();
}

void FindDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Check(pDX, IDC_CHECKMATCHCASE     , m_matchCase     );
  DDX_Check(pDX, IDC_CHECKMATCHWHOLEWORD, m_matchWholeWord);
  DDX_Check(pDX, IDC_CHECKUSEREGEXP     , m_useRegExp     );
  DDX_CBString(pDX, IDC_COMBOFINDWHAT   , m_findWhat      );
}

BEGIN_MESSAGE_MAP(FindDlg, CDialog)
  ON_BN_CLICKED(IDC_FINDNEXT                  , OnFindnext                     )
  ON_BN_CLICKED(IDC_BUTTONREGSYMBOLSMENU      , OnButtonregsymbolsmenu         )
  ON_COMMAND(ID_REGSYMBOLS_ANYCHAR            , OnRegSymbolsAnyChar            )
  ON_COMMAND(ID_REGSYMBOLS_CHARINRANGE        , OnRegSymbolsCharInRange        )
  ON_COMMAND(ID_REGSYMBOLS_CHARNOTINRANGE     , OnRegSymbolsCharNotInRange     )
  ON_COMMAND(ID_REGSYMBOLS_BEGINOFLINE        , OnRegSymbolsBeginOfLine        )
  ON_COMMAND(ID_REGSYMBOLS_ENDOFLINE          , OnRegSymbolsEndOfLine          )
  ON_COMMAND(ID_REGSYMBOLS_0ORMORE            , OnRegSymbols0orMore            )
  ON_COMMAND(ID_REGSYMBOLS_1ORMORE            , OnRegSymbols1orMore            )
  ON_COMMAND(ID_REGSYMBOLS_0OR1               , OnRegSymbols0or1               )
  ON_COMMAND(ID_REGSYMBOLS_OR                 , OnRegSymbolsOr                 )
  ON_COMMAND(ID_REGSYMBOLS_GROUP              , OnRegSymbolsGroup              )
  ON_CBN_SETFOCUS(IDC_COMBOFINDWHAT           , OnSetfocusCombofindwhat        )
  ON_CBN_SELENDOK(IDC_COMBOFINDWHAT           , OnSelendokCombofindwhat        )
  ON_CBN_SELCHANGE(IDC_COMBOFINDWHAT          , OnSelchangeCombofindwhat       )
END_MESSAGE_MAP()

BOOL FindDlg::OnInitDialog() {
  __super::OnInitDialog();
  m_findWhatCombo.substituteControl(this, IDC_COMBOFINDWHAT, _T("FindHistory"));
  const CRect cbRect = getWindowRect(this, IDC_COMBOFINDWHAT);
  const CPoint buttonPos(cbRect.right+1, cbRect.top);
  m_specialCharButton.Create(this, OBMIMAGE(RGARROW), buttonPos, IDC_BUTTONREGSYMBOLSMENU, true);
  LoadDynamicLayoutResource(m_lpszTemplateName);

  m_selStart       = 0;
  m_selEnd         = m_findWhat.GetLength();

  if(m_param.m_dirUp) {
    ((CButton*)GetDlgItem(IDC_RADIOUP))->SetCheck(1);
  } else {
    ((CButton*)GetDlgItem(IDC_RADIODOWN))->SetCheck(1);
  }

  UpdateData(false);
  return false;
}

void FindDlg::OnFindnext() {
  UpdateData();
  m_param.m_findWhat       = m_findWhat;
  m_param.m_matchCase      = m_matchCase      ? true : false;
  m_param.m_matchWholeWord = m_matchWholeWord ? true : false;
  m_param.m_useRegExp      = m_useRegExp      ? true : false;
  m_param.m_dirUp          = ((CButton*)GetDlgItem(IDC_RADIOUP))->GetCheck() ? true : false;
  m_param.m_skipCurrent    = false;
  try {
    SearchMachine sm(m_param);
    m_findWhatCombo.updateList();
    if(!m_TextContainer.searchText(m_param).isSet()) {
      if(m_param.m_useRegExp) {
        showWarning(_T("Cannot find a match for the regular expression '%s'."), m_param.m_findWhat.cstr());
      } else {
        showWarning(_T("Cannot find the String '%s'."), m_param.m_findWhat.cstr());
      }
      return;
    }
  } catch(Exception e) {
    showException(e);
    gotoFindWhat();
    return;
  }
  __super::OnOK();
}

void FindDlg::OnButtonregsymbolsmenu() {
  CMenu menu;
  int ret = menu.LoadMenu(IDR_MENUREGSYMBOLS);
  if(!ret) {
    showWarning(_T("Loadmenu failed"));
    return;
  }
  CRect r;
  GetDlgItem(IDC_BUTTONREGSYMBOLSMENU)->GetWindowRect(&r);
  menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, r.right, r.top, this );

}

void FindDlg::addRegSymbol(const TCHAR *s, int cursorpos) {
  CComboBox *b = (CComboBox*)GetDlgItem(IDC_COMBOFINDWHAT);
  UpdateData();
  String reg = m_findWhat.GetBuffer(m_findWhat.GetLength());
  reg = substr(reg, 0, m_selStart) + s + substr(reg, m_selEnd, reg.length());
  m_findWhat  = reg.cstr();
  m_useRegExp = true;
  m_selStart  = m_selEnd = m_selStart + cursorpos;
  UpdateData(false);
  b->SetFocus();
}

void FindDlg::OnRegSymbolsAnyChar()         { addRegSymbol(_T(".")     , 1); }
void FindDlg::OnRegSymbolsCharInRange()     { addRegSymbol(_T("[]")    , 1); }
void FindDlg::OnRegSymbolsCharNotInRange()  { addRegSymbol(_T("[^]")   , 2); }
void FindDlg::OnRegSymbolsBeginOfLine()     { addRegSymbol(_T("^")     , 1); }
void FindDlg::OnRegSymbolsEndOfLine()       { addRegSymbol(_T("$")     , 1); }
void FindDlg::OnRegSymbols0orMore()         { addRegSymbol(_T("*")     , 1); }
void FindDlg::OnRegSymbols1orMore()         { addRegSymbol(_T("+")     , 1); }
void FindDlg::OnRegSymbols0or1()            { addRegSymbol(_T("?")     , 1); }
void FindDlg::OnRegSymbolsOr()              { addRegSymbol(_T("\\|")   , 2); }
void FindDlg::OnRegSymbolsGroup()           { addRegSymbol(_T("\\(\\)"), 2); }

void FindDlg::OnSetfocusCombofindwhat() {
  CComboBox *b = (CComboBox*)GetDlgItem(IDC_COMBOFINDWHAT);
  b->SetEditSel(m_selStart, m_selEnd);
}

BOOL FindDlg::PreTranslateMessage(MSG *pMsg) {
  BOOL ret = __super::PreTranslateMessage(pMsg);

  if(getFocusCtrlId(this) == IDC_COMBOFINDWHAT) {
    CComboBox *b = (CComboBox*)GetDlgItem(IDC_COMBOFINDWHAT);
    DWORD w = b->GetEditSel();
    m_selStart = w & 0xff;
    m_selEnd   = w >> 16;
  }

  return ret;
}

void FindDlg::gotoFindWhat() {
  GetDlgItem(IDC_COMBOFINDWHAT)->SetFocus();
}

void FindDlg::OnSelendokCombofindwhat() {
  CComboBox *b = (CComboBox*)GetDlgItem(IDC_COMBOFINDWHAT);
  int f = b->GetCurSel();
}

void FindDlg::OnSelchangeCombofindwhat() {
  CComboBox *b = (CComboBox*)GetDlgItem(IDC_COMBOFINDWHAT);
  int f = b->GetCurSel();
}
