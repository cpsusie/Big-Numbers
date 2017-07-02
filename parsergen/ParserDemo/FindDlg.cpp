#include "stdafx.h"
#include "FindDlg.h"
#include "History.h"

#ifdef _DEBUG
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
  ON_COMMAND(ID_REGSYMBOLS_ANYCHARACTER       , OnRegsymbolsAnycharacter       )
  ON_COMMAND(ID_REGSYMBOLS_CHARACTERINRANGE   , OnRegsymbolsCharacterinrange   )
  ON_COMMAND(ID_REGSYMBOLS_CHARACTERNOTINRANGE, OnRegsymbolsCharacternotinrange)
  ON_COMMAND(ID_REGSYMBOLS_BEGINNINGOFLINE    , OnRegsymbolsBeginningofline    )
  ON_COMMAND(ID_REGSYMBOLS_ENDOFLINE          , OnRegsymbolsEndofline          )
  ON_COMMAND(ID_REGSYMBOLS_0ORMOREOCCURRENCES , OnRegsymbols0ormoreoccurrences )
  ON_COMMAND(ID_REGSYMBOLS_1ORMOREOCCURRENCES , OnRegsymbols1ormoreoccurrences )
  ON_COMMAND(ID_REGSYMBOLS_0OR1OCCURENCE      , OnRegsymbols0or1occurence      )
  ON_COMMAND(ID_REGSYMBOLS_OR                 , OnRegsymbolsOr                 )
  ON_COMMAND(ID_REGSYMBOLS_GROUP              , OnRegsymbolsGroup              )
  ON_CBN_SETFOCUS(IDC_COMBOFINDWHAT           , OnSetfocusCombofindwhat        )
  ON_COMMAND(ID_GOTOFINDWHAT                  , OnGotofindwhat                 )
  ON_CBN_SELENDOK(IDC_COMBOFINDWHAT           , OnSelendokCombofindwhat        )
  ON_CBN_SELCHANGE(IDC_COMBOFINDWHAT          , OnSelchangeCombofindwhat       )
  ON_WM_DRAWITEM()
END_MESSAGE_MAP()

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
    m_history.add(m_param.m_findWhat);
    if(!m_TextContainer.searchText(m_param).isSet()) {
      String msg;
      if(m_param.m_useRegExp) {
        msg = format(_T("Cannot find a match for the regular expression '%s'."), m_param.m_findWhat.cstr());
      } else {
        msg = format(_T("Cannot find the String '%s'."), m_param.m_findWhat.cstr());
      }
      MessageBox(msg.cstr(), NULL, MB_ICONWARNING);
      return;
    }
  } catch(Exception e) {
    MessageBox(e.what(), _T("Error"), MB_ICONWARNING);
    GetDlgItem(IDC_COMBOFINDWHAT)->SetFocus();
    return;
  }

  __super::OnOK();
}

BOOL FindDlg::OnInitDialog() {
  __super::OnInitDialog();
  CComboBox *combo = (CComboBox*)GetDlgItem(IDC_COMBOFINDWHAT);
  combo->AddString(m_findWhat.GetBuffer(m_findWhat.GetLength()));
  for(size_t i = 0; i < m_history.size(); i++) {
    combo->AddString(m_history[i].cstr());
  }

  m_selStart       = 0;
  m_selEnd         = m_findWhat.GetLength();

  if(m_param.m_dirUp) {
    ((CButton*)GetDlgItem(IDC_RADIOUP))->SetCheck(1);
  } else {
    ((CButton*)GetDlgItem(IDC_RADIODOWN))->SetCheck(1);
  }

  m_accelTable = LoadAccelerators(theApp.m_hInstance, MAKEINTRESOURCE(IDR_ACCELERATORFIND));
  UpdateData(false);
  return false;
}

void FindDlg::OnButtonregsymbolsmenu() {
  CMenu menu;
  int ret = menu.LoadMenu(IDR_MENUREGSYMBOLS);
  if(!ret) {
    AfxMessageBox(_T("Loadmenu failed"), MB_ICONERROR);
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

void FindDlg::OnRegsymbolsAnycharacter()        { addRegSymbol(_T("."), 1);      }
void FindDlg::OnRegsymbolsCharacterinrange()    { addRegSymbol(_T("[]"), 1);     }
void FindDlg::OnRegsymbolsCharacternotinrange() { addRegSymbol(_T("[^]"), 2);    }
void FindDlg::OnRegsymbolsBeginningofline()     { addRegSymbol(_T("^"), 1);      }
void FindDlg::OnRegsymbolsEndofline()           { addRegSymbol(_T("$"), 1);      }
void FindDlg::OnRegsymbols0ormoreoccurrences()  { addRegSymbol(_T("*"), 1);      }
void FindDlg::OnRegsymbols1ormoreoccurrences()  { addRegSymbol(_T("+"), 1);      }
void FindDlg::OnRegsymbols0or1occurence()       { addRegSymbol(_T("?"), 1);      }
void FindDlg::OnRegsymbolsOr()                  { addRegSymbol(_T("\\|"), 2);    }
void FindDlg::OnRegsymbolsGroup()               { addRegSymbol(_T("\\(\\)"), 2); }

void FindDlg::OnSetfocusCombofindwhat() {
  CComboBox *b = (CComboBox*)GetDlgItem(IDC_COMBOFINDWHAT);
  b->SetEditSel(m_selStart, m_selEnd);
}

BOOL FindDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }
  BOOL ret = __super::PreTranslateMessage(pMsg);

  if(getFocusCtrlId(this) == IDC_COMBOFINDWHAT) {
    CComboBox *b = (CComboBox*)GetDlgItem(IDC_COMBOFINDWHAT);
    DWORD w = b->GetEditSel();
    m_selStart = w & 0xff;
    m_selEnd   = w >> 16;
  }

  return ret;
}

void FindDlg::OnGotofindwhat() {
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

void drawTriangle(CWnd *wnd) {
  CClientDC dc(wnd);

  CRgn rgn;
  CPoint p[] = { CPoint(2, 2), CPoint(6, 6), CPoint(2, 10) };
  CBrush brush;
  brush.CreateSolidBrush(RGB(0, 0, 0));
  rgn.CreatePolygonRgn(p, 3, ALTERNATE);
  dc.FillRgn(&rgn, &brush);
}

void FindDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) {
  if(nIDCtl == IDC_BUTTONREGSYMBOLSMENU)
    drawTriangle(GetDlgItem(IDC_BUTTONREGSYMBOLSMENU));

  __super::OnDrawItem(nIDCtl, lpDrawItemStruct);
}
