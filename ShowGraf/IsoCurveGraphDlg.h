#pragma once

#include <MFCUtil/ExprDialog.h>
#include "IsoCurveGraph.h"

class CIsoCurveGraphDlg : public SaveLoadExprDialog<IsoCurveGraphParameters> {
private:
  CString  m_style;
  BOOL     m_createListFile;
  CString  m_expr;
  double   m_cellSize;
  double   m_xFrom;
  double   m_xTo;
  double   m_yFrom;
  double   m_yTo;

  inline CComboBox *getStyleCombo() const {
    return (CComboBox*)GetDlgItem(IDC_COMBOSTYLE);
  }
  inline CMFCColorButton *getColorButton() const {
    return (CMFCColorButton*)GetDlgItem(IDC_BUTTONCOLOR);
  }

  bool validate();
  void paramToWin(const IsoCurveGraphParameters &param);
  bool winToParam(      IsoCurveGraphParameters &param);
  void addToRecent(const String &fileName);
protected:
  String getListFileName() const;
public:
  CIsoCurveGraphDlg(IsoCurveGraphParameters &param, CWnd *pParent = NULL);
  enum { IDD = IDR_ISOCURVE };
protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnEditFindmatchingparentesis();
    afx_msg void OnButtonHelp();
    DECLARE_MESSAGE_MAP()
};
