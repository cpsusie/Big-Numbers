#include "stdafx.h"
#include <Scanner.h>
#include <Math/Expression/Expression.h>
#include <Math/Expression/ExpressionWrapper.h>
#include "ExprDialog.h"

void CExprDialog::createExprFont() {
  if(m_exprFont.m_hObject) return;
  CFont *dlgFont = GetFont();
  LOGFONT lf;
  dlgFont->GetLogFont(&lf);
  _tcscpy(lf.lfFaceName, _T("courier new"));
  BOOL ret = m_exprFont.CreateFontIndirect(&lf);
}

void CExprDialog::setExprFont(int id) {
  createExprFont();
  getExprField(id)->SetFont(&m_exprFont, FALSE);
}

void CExprDialog::gotoMatchingParentesis() {
  const int id = getFocusCtrlId(this);
  CEdit *e = getExprField(id);
  int cursorPos, endChar;
  e->GetSel(cursorPos,endChar);
  const String expr = getExprString(id);
  int m = findMatchingpParanthes(expr.cstr(), cursorPos);
  if(m >= 0) {
    e->SetSel(m, m);
  }
}

String CExprDialog::getExprString(int id) {
  return getWindowText(this, id);
}

CEdit *CExprDialog::getExprField(int id) {
  return (CEdit*)GetDlgItem(id);
}

bool CExprDialog::validate(int id) {
  ExpressionWrapper expr;
  expr.compile(getExprString(id), false);
  if(!expr.ok()) {
    showExprError(expr.getErrorMessage(),id);
    return false;
  }
  if(!expr.isReturnTypeReal()) {
    gotoExpr(id);
    Message(_T("Expression must return real value"));
    return false;
  }
  return true;
}

void CExprDialog::showExprError(const String &msg, int id) {
  try {
    Tokenizer tok(msg, ":");
    String posStr = tok.next();
    int line, col;
    String tmp;
    if(_stscanf(posStr.cstr(), _T("(%d,%d)"), &line,&col) == 2) {
      tmp = tok.getRemaining();
    } else {
      throwException(_T("No sourceposition"));
    }
    int charIndex = SourcePosition::findCharIndex(getExprString(id).cstr(), SourcePosition(line,col));
    gotoExpr(id);
    getExprField(id)->SetSel(charIndex, charIndex);
    Message(_T("%s"), tmp.cstr());
  } catch(Exception) { // ignore Exception, and just show msg
    gotoExpr(id);
    Message(_T(":%s"), msg.cstr());
  }
}

void CExprDialog::createHelpButton(int id) {
  if (m_helpButtonCount >= MAXHELPBUTTONS) {
    return;
  }
  if(GetDlgItem(id) == NULL) {
    return;
  }
  const CPoint hp = getWindowPosition(this, id);
  
  GetDlgItem(id)->DestroyWindow();
  m_helpButton[m_helpButtonCount++].Create(this, OBMIMAGE(RGARROW), hp, id);
}

void CExprDialog::showExprHelpMenu(int id) {
  try {
    CMenu menu;
    createMenuExprHelp(menu);
    CRect r;
    GetDlgItem(id)->GetWindowRect(&r);
    menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,r.right,r.top, this);
  } catch(Exception e) {
    showException(e);
  }
}

void CExprDialog::handleSelectedExprHelpId(int menuId, int ctrlId) {
  substituteSelectedText(ctrlId, getExprSyntax(menuId - ID_EXPRHELP_MENU_FIRST));
}

void CExprDialog::substituteSelectedText(int ctrlId, const String &s) {
  if(s.length() > 0) {
    CEdit *e = getExprField(ctrlId);
    if (e == NULL) {
      MessageBox(format(_T("No ctrlId %d in window"), ctrlId).cstr(), _T("Error"), MB_ICONWARNING);
      return;
    }
    int startIndex, endIndex;
    e->GetSel(startIndex, endIndex);
    const int selLen = endIndex - startIndex;
    String text = getWindowText(e);
    if(selLen > 0) {
      text.remove(startIndex, selLen);
    }
    text.insert(startIndex, s);
    setWindowText(e, text);
    int newSel = (int)(startIndex + s.length());
    e->SetSel(newSel, newSel);
    gotoExpr(ctrlId);
  }
}

void CExprDialog::createMenuExprHelp(CMenu &menu) {
  const ExpressionDescription *helpList = ExpressionDescription::getHelpList();
  const int count = ExpressionDescription::getHelpListSize();
  int ret = menu.LoadMenu(IDR_EXPRHELP_MENU);
  if(!ret) {
    throwException(_T("Loadmenu(%d) failed"), IDR_EXPRHELP_MENU);
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
