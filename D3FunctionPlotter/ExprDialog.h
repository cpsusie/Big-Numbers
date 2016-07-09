#pragma once

#include <MFCUtil/LayoutManager.h>
#include <MFCUtil/OBMButton.h>

class CExprDialog : public CDialog {
private:
  OBMButton m_helpButton;
  CFont     m_exprFont;

  void createMenuExprHelp(CMenu &menu);
  void showExprError(const String &msg);
  String getExprSyntax(int index);
  void substituteSelectedText(const String &s);

protected:
  SimpleLayoutManager m_layoutManager;

  CExprDialog(int resId, CWnd *pParent) : CDialog(resId, pParent) {
  }
  void createHelpButton();
  void setExprFont();

  void gotoExpr() {
    GetDlgItem(IDC_EDIT_EXPR)->SetFocus();
  }

  CEdit *getExprField();
  String getExprString();

  void gotoMatchingParentesis();

  bool validate();
  void showExprHelpMenu();
  void handleSelectedExprHelpId(int id);
};
