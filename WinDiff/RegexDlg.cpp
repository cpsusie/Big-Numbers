#include "stdafx.h"
#include "WinDiff.h"
#include "FindDlg.h"
#include "RegexDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CRegexDlg::CRegexDlg(RegexFilter &param, CWnd *pParent) : m_param(param), CDialog(CRegexDlg::IDD, pParent) {
  m_matchCase      = m_param.m_matchCase;
  m_matchWholeWord = m_param.m_matchWholeWord;
  m_regex          = m_param.m_regex.cstr();
}

void CRegexDlg::DoDataExchange(CDataExchange* pDX) {
  __super::DoDataExchange(pDX);
  DDX_Check(pDX   , IDC_CHECKMATCHCASE     , m_matchCase     );
  DDX_Check(pDX   , IDC_CHECKMATCHWHOLEWORD, m_matchWholeWord);
  DDX_CBString(pDX, IDC_COMBOREGEX         , m_regex        );
}


BEGIN_MESSAGE_MAP(CRegexDlg, CDialog)
  ON_CBN_KILLFOCUS(IDC_COMBOREGEX             , OnKillFocusComboRegex          )
  ON_CBN_SETFOCUS(IDC_COMBOREGEX              , OnSetFocusComboRegex           )
  ON_COMMAND(ID_GOTOREGEX                     , OnGotoRegex                    )
  ON_BN_CLICKED(IDC_BUTTONREGSYMBOLSMENU      , OnButtonRegSymbolsMenu         )
  ON_COMMAND(ID_REGSYMBOLS_ANYCHAR            , OnRegSymbolsAnyChar            )
  ON_COMMAND(ID_REGSYMBOLS_CHARINRANGE        , OnRegSymbolsCharInRange        )
  ON_COMMAND(ID_REGSYMBOLS_CHARNOTINRANGE     , OnRegSymbolsCharNotInRange     )
  ON_COMMAND(ID_REGSYMBOLS_BEGINNINGOFLINE    , OnRegSymbolsBeginningOfLine    )
  ON_COMMAND(ID_REGSYMBOLS_ENDOFLINE          , OnRegSymbolsEndOfLine          )
  ON_COMMAND(ID_REGSYMBOLS_0ORMORE            , OnRegSymbols0OrMoreOccurrences )
  ON_COMMAND(ID_REGSYMBOLS_1ORMORE            , OnRegSymbols1OrMoreOccurrences )
  ON_COMMAND(ID_REGSYMBOLS_0OR1               , OnRegSymbols0Or1Occurence      )
  ON_COMMAND(ID_REGSYMBOLS_OR                 , OnRegSymbolsOr                 )
  ON_COMMAND(ID_REGSYMBOLS_GROUP              , OnRegSymbolsGroup              )
  ON_WM_DRAWITEM()
END_MESSAGE_MAP()

BOOL CRegexDlg::OnInitDialog() {
  __super::OnInitDialog();

  m_regexCombo.substituteControl(this, IDC_COMBOREGEX, "RegexHistory");

  m_currentControl = 0;
  m_selStart       = 0;
  m_selEnd         = (int)m_param.m_regex.length();

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_ACCELERATORREGEX));
  UpdateData(false);
  return false;
}

void CRegexDlg::OnOK() {
  UpdateData();
  if(m_regex.GetLength() == 0) {
    Message(_T("Empty regular expression not allowed"));
    return;
  }
  m_param.m_regex          = m_regex;
  m_param.m_matchCase      = m_matchCase      ? true : false;
  m_param.m_matchWholeWord = m_matchWholeWord ? true : false;
  try {
    m_param.compile();
    m_regexCombo.updateList();
    __super::OnOK();
  } catch(Exception e) {
    GetDlgItem(IDC_COMBOREGEX)->SetFocus();
    showException(e);
  }
}

void CRegexDlg::OnSetFocusComboRegex() {
  CComboBox *b = (CComboBox*)GetDlgItem(IDC_COMBOREGEX);
  b->SetEditSel(m_selStart,m_selEnd);
  m_currentControl = IDC_COMBOREGEX;
}

void CRegexDlg::OnKillFocusComboRegex() {
  m_currentControl = 0;
}

BOOL CRegexDlg::PreTranslateMessage(MSG* pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }

  BOOL ret = __super::PreTranslateMessage(pMsg);

  if(m_currentControl == IDC_COMBOREGEX) {
    CComboBox *b = (CComboBox*)GetDlgItem(IDC_COMBOREGEX);
    DWORD w = b->GetEditSel();
    m_selStart = w & 0xff;
    m_selEnd   = w >> 16;
  }
  return ret;
}

void CRegexDlg::OnGotoRegex() {
  GetDlgItem(IDC_COMBOREGEX)->SetFocus();
}

void CRegexDlg::OnButtonRegSymbolsMenu() {
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

void CRegexDlg::addRegexSymbol(const TCHAR *s, int cursorpos) {
  CComboBox *b = (CComboBox*)GetDlgItem(IDC_COMBOREGEX);

  UpdateData();

  String reg = m_regex;

  reg = substr(reg,0,m_selStart) + s + substr(reg,m_selEnd,reg.length());

  m_regex    = reg.cstr();
  m_selStart = m_selEnd = m_selStart + cursorpos;
  UpdateData(false);

  b->SetFocus();
}

void CRegexDlg::OnRegSymbolsAnyChar()             { addRegexSymbol(_T("."),1);      }
void CRegexDlg::OnRegSymbolsCharInRange()         { addRegexSymbol(_T("[]"),1);     }
void CRegexDlg::OnRegSymbolsCharNotInRange()      { addRegexSymbol(_T("[^]"),2);    }
void CRegexDlg::OnRegSymbolsBeginningOfLine()     { addRegexSymbol(_T("^"),1);      }
void CRegexDlg::OnRegSymbolsEndOfLine()           { addRegexSymbol(_T("$"),1);      }
void CRegexDlg::OnRegSymbols0OrMoreOccurrences()  { addRegexSymbol(_T("*"),1);      }
void CRegexDlg::OnRegSymbols1OrMoreOccurrences()  { addRegexSymbol(_T("+"),1);      }
void CRegexDlg::OnRegSymbols0Or1Occurence()       { addRegexSymbol(_T("?"),1);      }
void CRegexDlg::OnRegSymbolsOr()                  { addRegexSymbol(_T("\\|"),2);    }
void CRegexDlg::OnRegSymbolsGroup()               { addRegexSymbol(_T("\\(\\)"),2); }

void CRegexDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) {
  if(nIDCtl == IDC_BUTTONREGSYMBOLSMENU) {
    drawTriangle(GetDlgItem(IDC_BUTTONREGSYMBOLSMENU));
  }

  __super::OnDrawItem(nIDCtl, lpDrawItemStruct);
}
