#pragma once

#include <MFCUtil/LayoutManager.h>
#include <MFCUtil/OBMButton.h>

#define MAXHELPBUTTONS 10

class CExprDialog : public CDialog {
private:
  int       m_helpButtonCount;
  OBMButton m_helpButton[MAXHELPBUTTONS];
  CFont     m_exprFont;

  void createExprFont();
  void createMenuExprHelp(CMenu &menu);
  void showExprError(const String &msg, int id = IDC_EDIT_EXPR);
  String getExprSyntax(int index);
  void substituteSelectedText(int ctrlId, const String &s);

protected:
  SimpleLayoutManager m_layoutManager;

  CExprDialog(int resId, CWnd *pParent) : CDialog(resId, pParent) {
    m_helpButtonCount = 0;
  }
  void createHelpButton(int id = IDC_BUTTON_HELP);
  void setExprFont(int id = IDC_EDIT_EXPR);

  void gotoExpr(int id = IDC_EDIT_EXPR) {
    GetDlgItem(id)->SetFocus();
  }

  CEdit *getExprField( int id = IDC_EDIT_EXPR);
  String getExprString(int id = IDC_EDIT_EXPR);

  void gotoMatchingParentesis();

  bool validate(int id = IDC_EDIT_EXPR);
  void showExprHelpMenu(int id = IDC_BUTTON_HELP);
  void handleSelectedExprHelpId(int menuId, int ctrlId = IDC_EDIT_EXPR);
};
