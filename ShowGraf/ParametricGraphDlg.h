#pragma once

#include <MFCUtil/ExprDialog.h>
#include "ParametricGraph.h"

class CParametricGraphDlg : public SaveLoadExprDialog<ParametricGraphParameters> {
private:
  CString  m_style;
  BOOL     m_createListFile;
  CString  m_commonText;
  CString  m_exprX;
  CString  m_exprY;
  double   m_tFrom;
  double   m_tTo;
  UINT     m_steps;

  inline CComboBox *getStyleCombo() const {
    return (CComboBox*)GetDlgItem(IDC_COMBOSTYLE);
  }
  inline CMFCColorButton *getColorButton() const {
    return (CMFCColorButton*)GetDlgItem(IDC_BUTTONCOLOR);
  }
  bool validate();
  void paramToWin(const ParametricGraphParameters &param);
  bool winToParam(      ParametricGraphParameters &param);
  void addToRecent(const String &fileName);
protected:
  String getListFileName() const;
public:
  CParametricGraphDlg(ParametricGraphParameters &param, CWnd *pParent = nullptr);
  virtual ~CParametricGraphDlg();

  enum { IDD = IDR_PARAMETRICCURVE };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnEditFindmatchingparentesis();
    afx_msg void OnButtonHelpX();
    afx_msg void OnButtonHelpY();
    DECLARE_MESSAGE_MAP()
};
