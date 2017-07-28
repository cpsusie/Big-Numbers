#pragma once

#include <MFCUtil/ExprDialog.h>
#include "FunctionGraph.h"

#define SHOW_INTERVAL 0x1
#define SHOW_STEP     0x2

class CFunctionGraphDlg : public SaveLoadExprDialog<FunctionGraphParameters> {
private:
  CString             m_style;
  CString             m_expr;
  double              m_xFrom;
  double              m_xTo;
  UINT                m_steps;
  int                 m_showFlags;

  inline CComboBox *getStyleCombo() const {
    return (CComboBox*)GetDlgItem(IDC_COMBOSTYLE);
  }
  inline CMFCColorButton *getColorButton() const {
    return (CMFCColorButton*)GetDlgItem(IDC_BUTTONCOLOR);
  }
  bool validate();
  void paramToWin(const FunctionGraphParameters &param);
  void winToParam(      FunctionGraphParameters &param) const;
  void addToRecent(const String &fileName);
public:
  CFunctionGraphDlg(FunctionGraphParameters &param, int showFlags = SHOW_INTERVAL|SHOW_STEP, CWnd *pParent = NULL);   // standard constructor
  enum { IDD = IDR_FUNCTION };
protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnGotoStyle();
    afx_msg void OnGotoExpr();
    afx_msg void OnGotoXInterval();
    afx_msg void OnGotoStep();
    afx_msg void OnEditFindmatchingparentesis();
    afx_msg void OnButtonHelp();
    DECLARE_MESSAGE_MAP()
};
