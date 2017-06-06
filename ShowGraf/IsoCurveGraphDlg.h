#pragma once

#include <MFCUtil/ExprDialog.h>
#include "IsoCurveGraph.h"

class CIsoCurveGraphDlg : public SaveLoadExprDialog<IsoCurveGraphParameters> {
private:
    CString             m_style;
    CString             m_expr;
    double              m_cellSize;
    double              m_xFrom;
    double              m_xTo;
    double              m_yFrom;
    double              m_yTo;

    CComboBox *getStyleCombo() const {
      return (CComboBox*)GetDlgItem(IDC_COMBOSTYLE);
    }
    CMFCColorButton *getColorButton() const {
      return (CMFCColorButton*)GetDlgItem(IDC_BUTTONCOLOR);
    }

    bool validate();
    void paramToWin(const IsoCurveGraphParameters &param);
    void winToParam(      IsoCurveGraphParameters &param) const;
    void addToRecent(const String &fileName);
public:
    CIsoCurveGraphDlg(IsoCurveGraphParameters &param, CWnd *pParent = NULL);
    enum { IDD = IDR_ISOCURVE };
protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnGotoStyle();
    afx_msg void OnGotoExpr();
    afx_msg void OnGotoXInterval();
    afx_msg void OnGotoYInterval();
    afx_msg void OnGotoCellSize();
    afx_msg void OnEditFindmatchingparentesis();
    afx_msg void OnButtonHelp();
    DECLARE_MESSAGE_MAP()
};
