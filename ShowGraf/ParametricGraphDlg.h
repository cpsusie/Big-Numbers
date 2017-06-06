#pragma once

#include <MFCUtil/ExprDialog.h>
#include "ParametricGraph.h"

class CParametricGraphDlg : public SaveLoadExprDialog<ParametricGraphParameters> {
private:
    CString             m_style;
    CString             m_exprX;
    CString             m_exprY;
    double              m_tFrom;
    double              m_tTo;
    UINT                m_steps;

    CComboBox *getStyleCombo() const {
      return (CComboBox*)GetDlgItem(IDC_COMBOSTYLE);
    }
    CMFCColorButton *getColorButton() const {
      return (CMFCColorButton*)GetDlgItem(IDC_BUTTONCOLOR);
    }
    bool validate();
    void paramToWin(const ParametricGraphParameters &param);
    void winToParam(      ParametricGraphParameters &param) const;
    void addToRecent(const String &fileName);

public:
  CParametricGraphDlg(ParametricGraphParameters &param, CWnd *pParent = NULL);
  virtual ~CParametricGraphDlg();

  enum { IDD = IDR_PARAMETRICCURVE };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnGotoStyle();
    afx_msg void OnGotoExprX();
    afx_msg void OnGotoExprY();
    afx_msg void OnGotoTInterval();
    afx_msg void OnGotoStep();
    afx_msg void OnEditFindmatchingparentesis();
    afx_msg void OnButtonHelpX();
    afx_msg void OnButtonHelpY();
    DECLARE_MESSAGE_MAP()
};
