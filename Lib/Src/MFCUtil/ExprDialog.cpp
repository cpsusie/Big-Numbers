#include "pch.h"
#include <Scanner.h>
#include <Math/Expression/Expression.h>
#include <Math/Expression/ExpressionWrapper.h>
#include <MFCUtil/Resource.h>
#include <MFCUtil/ExprDialog.h>

using namespace Expr;

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

void CExprDialog::openListFile() {
  closeListFile();
  const String fileName = getListFileName();
  m_listFile = MKFOPEN(fileName, _T("w"));
}

void CExprDialog::closeListFile() {
  if(isListFileOpen()) {
    fclose(m_listFile);
    m_listFile = NULL;
  }
}

bool CExprDialog::validateAllExpr() {
  const bool genListFile = generateListFile();
  if(genListFile) openListFile();
  bool ok = true;
  for(Iterator<int> it = m_helpButtonMap.values().getIterator(); ok && it.hasNext();) {
    ok = validateExpr(it.next());
  }
  closeListFile();
  return ok;
}

bool CExprDialog::validateExpr(int id) {
  ExpressionWrapper expr;
  if(!generateListFile()) {
    expr.compile(getExprText(id), false);
  } else {
    const bool lfWasOpen = isListFileOpen();
    if(!lfWasOpen) openListFile();
    expr.compile(getExprText(id), true, m_listFile);
    if(!lfWasOpen) closeListFile();
  }
  if(!expr.ok()) {
    showExprError(expr.getErrorMessage(),id);
    return false;
  }
  if(!expr.isReturnTypeFloat()) {
    gotoExpr(id);
    showWarning(_T("Expression must return real value"));
    return false;
  }
  return true;
}

bool CExprDialog::validateInterval(int fromId, int toId) {
  double from,to;
  if(!getEditValue(this, fromId, from)) return false;
  if(!getEditValue(this, toId  , to  )) return false;
  if(from >= to) {
    gotoEditBox(this, fromId);
    showWarning(_T("Invalid interval"));
    return false;
  }
  return true;
}

bool CExprDialog::validateMinMax(int id, double min, double max) {
  double value;
  if(!getEditValue(this, id, value)) return false;
  if((value < min) || (value > max)) {
    gotoEditBox(this, id);
    showWarning(_T("Value must be in range [%lg..%lg]"), min, max);
    return false;
  }
  return true;
}

void CExprDialog::showExprError(const String &msg, int id) {
  try {
    String     errorMsg  = msg;
    UINT       charIndex = Expression::decodeErrorString(getExprText(id), errorMsg);
    const UINT prefixLen = (UINT)getCommonExprText().length();
    if(charIndex < prefixLen) {
      gotoExpr(getCommonExprFieldId());
      setCaretPos(getExprField(getCommonExprFieldId()), charIndex);
    } else {
      gotoExpr(id);
      setCaretPos(getExprField(id), charIndex - prefixLen);
    }
    showWarning(errorMsg);
  } catch(Exception) { // ignore Exception, and just show msg
    gotoExpr(id);
    showWarning(msg);
  }
}

void CExprDialog::createExprHelpButton(int buttonId, int exprEditId) {
  if(m_helpButtonCount >= MAXHELPBUTTONS) {
    showWarning(_T("Too many expr-helpbuttons. Max=%d"), MAXHELPBUTTONS);
    return;
  }
  CButton *but  = (CButton*)GetDlgItem(buttonId  );
  CEdit   *edit = (CEdit  *)GetDlgItem(exprEditId);
  if(but == NULL) {
    showWarning(_T("Button %d doesn't exist"), buttonId);
    return;
  }
  if(edit == NULL) {
    showWarning(_T("EditBox %d doesn't exist"), exprEditId);
    return;
  }
  const CPoint bp = getWindowPosition(this, buttonId);

  but->DestroyWindow();
  setExprFont(exprEditId);
  m_helpButton[m_helpButtonCount++].Create(this, OBMIMAGE(RGARROW), bp, buttonId);
  m_helpButtonMap.put(buttonId, exprEditId);
}

void CExprDialog::handleExprHelpButtonClick(int buttonId) {
  const int *editId = m_helpButtonMap.get(buttonId);
  if(editId) {
    m_selectedExprId = *editId;
    showExprHelpMenu(buttonId);
  }
}

BOOL CExprDialog::PreTranslateMessage(MSG *pMsg) {
  switch(pMsg->message) {
  case WM_COMMAND:
    if((pMsg->wParam >= _ID_EXPRHELP_MENU_FIRST) && (pMsg->wParam <= _ID_EXPRHELP_MENU_LAST)) {
      handleSelectedExprHelpId((int)pMsg->wParam, m_selectedExprId);
      return TRUE;
    }
  }
  return __super::PreTranslateMessage(pMsg);
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
  substituteSelectedText(ctrlId, getExprSyntax(menuId - _ID_EXPRHELP_MENU_FIRST));
}

void CExprDialog::substituteSelectedText(int ctrlId, const String &s) {
  if(s.length() > 0) {
    CEdit *e = getExprField(ctrlId);
    if(e == NULL) {
      showWarning(_T("No ctrlId %d in window"), ctrlId);
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
    setCaretPos(e, (int)(startIndex + s.length()));
    gotoExpr(ctrlId);
  }
}

void CExprDialog::createMenuExprHelp(CMenu &menu) {
  const ExpressionDescription *helpList = ExpressionDescription::getHelpList();
  const int count = ExpressionDescription::getHelpListSize();
  int ret = menu.LoadMenu(_IDR_EXPRHELP_MENU);
  if(!ret) {
    throwException(_T("Loadmenu(%d) failed"), _IDR_EXPRHELP_MENU);
  }

  CMenu *m = menu.GetSubMenu(0);
  if(m == NULL) {
    throwException(_T("No submenu"));
  }
  m->RemoveMenu(_ID_EXPRHELP_MENU_LAST, MF_BYCOMMAND);
  for(int i = 0; i < count; i++) {
    const ExpressionDescription &item = helpList[i];
    m->AppendMenu(MF_STRING, _ID_EXPRHELP_MENU_FIRST+i, format(_T("%s  \t%s"), item.getSyntax(), item.getDescription()).cstr());
  }
}

String CExprDialog::getExprSyntax(int index) {
  const ExpressionDescription *helpList = ExpressionDescription::getHelpList();
  const int count = ExpressionDescription::getHelpListSize();
  if(index < 0 || index >= count) {
    return EMPTYSTRING;
  } else {
    return helpList[index].getSyntax();
  }
}
