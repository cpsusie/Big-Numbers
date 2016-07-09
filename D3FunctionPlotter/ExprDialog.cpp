#include "stdafx.h"
#include <Scanner.h>
#include <Math/Expression/Expression.h>
#include "ExpressionWrapper.h"
#include "ExprDialog.h"

void CExprDialog::setExprFont() {
  CFont *dlgFont = GetFont();
  LOGFONT lf;
  dlgFont->GetLogFont(&lf);
  _tcscpy(lf.lfFaceName, _T("courier new"));
  BOOL ret = m_exprFont.CreateFontIndirect(&lf);
  getExprField()->SetFont(&m_exprFont, FALSE);
}

void CExprDialog::gotoMatchingParentesis() {
  CEdit *e = getExprField();
  int cursorPos, endChar;
  e->GetSel(cursorPos,endChar);
  String expr = getExprString();
  int m = findMatchingpParanthes(expr.cstr(), cursorPos);
  if(m >= 0) {
    e->SetSel(m, m);
  }
}

String CExprDialog::getExprString() {
  return getWindowText(this, IDC_EDIT_EXPR);
}

CEdit *CExprDialog::getExprField() {
  return (CEdit*)GetDlgItem(IDC_EDIT_EXPR);
}

bool CExprDialog::validate() {
  ExpressionWrapper expr;
  expr.compile(getExprString().cstr(), false);
  if(!expr.ok()) {
    showExprError(expr.getErrorMessage());
    return false;
  }
  if(!expr.isReturnTypeReal()) {
    gotoExpr();
    Message(_T("Expression must return real value"));
    return false;
  }
  return true;
}

void CExprDialog::showExprError(const String &msg) {
  try {
    Tokenizer tok(msg, ":");
    String posStr = tok.next();
    int line, col;
    String tmp;
    if(_stscanf(posStr.cstr(), _T("(%d,%d)"), &line,&col) == 2) {
      tmp = tok.getRemaining();
    } else {
      throwException(_T("no sourceposition"));
    }
    int charIndex = SourcePosition::findCharIndex(getExprString().cstr(), SourcePosition(line,col));
    gotoExpr();
    getExprField()->SetSel(charIndex, charIndex);
    Message(_T("%s"), tmp.cstr());
  } catch(Exception) { // ignore Exception, and just show msg
    gotoExpr();
    Message(_T(":%s"), msg.cstr());
  }
}

void CExprDialog::createHelpButton() {
  if(GetDlgItem(IDC_BUTTON_HELP) == NULL) {
    return;
  }
  const CPoint hp = getWindowPosition(this, IDC_BUTTON_HELP);
  
  GetDlgItem(IDC_BUTTON_HELP)->DestroyWindow();
  m_helpButton.Create(this, OBMIMAGE(RGARROW), hp, IDC_BUTTON_HELP);
}

void CExprDialog::showExprHelpMenu() {
  try {
    CMenu menu;
    createMenuExprHelp(menu);
    CRect r;
    GetDlgItem(IDC_BUTTON_HELP)->GetWindowRect(&r);
    menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,r.right,r.top, this);
  } catch(Exception e) {
    showException(e);
  }
}

void CExprDialog::handleSelectedExprHelpId(int id) {
  substituteSelectedText(getExprSyntax(id - ID_EXPRHELP_MENU_FIRST));
}

void CExprDialog::substituteSelectedText(const String &s) {
  if(s.length() > 0) {
    CEdit *e = getExprField();
    int startIndex, endIndex;
    e->GetSel(startIndex, endIndex);
    const int selLen = endIndex - startIndex;
    String text = getWindowText(e);
    if(selLen > 0) {
      text.remove(startIndex, selLen);
    }
    text.insert(startIndex, s);
    setWindowText(e, text);
    int newSel = startIndex + s.length();
    e->SetSel(newSel, newSel);
    gotoExpr();
  }
}

void CExprDialog::createMenuExprHelp(CMenu &menu) {
  const ExpressionDescription *helpList = ExpressionDescription::getHelpList();
  const int count = ExpressionDescription::getHelpListSize();
  int ret = menu.LoadMenu(IDR_EXPRHELP_MENU);
  if(!ret) {
    throwException(_T("Loadmenu(%s) failed"), IDR_EXPRHELP_MENU);
  }

  CMenu *m = menu.GetSubMenu(0);
  if(m == NULL) {
    throwException(_T("No submenu"));
  }
  m->RemoveMenu(ID_EXPRHELP_MENU_LAST, MF_BYCOMMAND);
  for(int i = 0; i < count; i++) {
    const ExpressionDescription &item = helpList[i];
    m->AppendMenu(MF_STRING, ID_EXPRHELP_MENU_FIRST+i, format(_T("%s  \t%s"), item.getSyntax(), item.getDescription()).cstr());
  }
}

String CExprDialog::getExprSyntax(int index) {
  const ExpressionDescription *helpList = ExpressionDescription::getHelpList();
  const int count = ExpressionDescription::getHelpListSize();
  if(index < 0 || index >= count) {
    return _T("");
  } else {
    return helpList[index].getSyntax();
  }
}
