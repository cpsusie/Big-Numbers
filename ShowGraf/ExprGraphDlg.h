#pragma once

#include "ExpressionGraph.h"
#include <MFCUtil/LayoutManager.h>

#define SHOW_INTERVAL 0x1
#define SHOW_STEP     0x2

class CExprGraphDlg : public CDialog {
private:
    HACCEL              m_accelTable;
    SimpleLayoutManager m_layoutManager;
    CFont               m_exprFont;
    String              m_fullName;

    CString             m_name;
    CString	            m_style;
    CString             m_expr;
    double              m_xFrom;
    double              m_xTo;
    UINT                m_steps;
    int                 m_showFlags;

    CComboBox *getStyleCombo() {
      return (CComboBox*)GetDlgItem(IDC_COMBOSTYLE);
    }
    CMFCColorButton *getColorButton() {
      return (CMFCColorButton*)GetDlgItem(IDC_BUTTONCOLOR);
    }
    bool validate();
    void paramToWin(            const ExpressionGraphParameters &param);
    void winToParam(                  ExpressionGraphParameters &param);
    void saveAs(                      ExpressionGraphParameters &param);
    void save(const String &fileName, ExpressionGraphParameters &param);
    void addToRecent(const String &fileName);

public:
    CExprGraphDlg(ExpressionGraphParameters &param, int showFlags = SHOW_INTERVAL|SHOW_STEP, CWnd* pParent = NULL);   // standard constructor
    ExpressionGraphParameters &m_param;

    enum { IDD = IDD_EXPRGRAPH_DIALOG };

public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnGotoName();
    afx_msg void OnGotoStyle();
    afx_msg void OnGotoExpr();
    afx_msg void OnGotoXInterval();
    afx_msg void OnGotoStep();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnFileOpen();
    afx_msg void OnFileSave();
    afx_msg void OnFileSaveAs();
    afx_msg void OnEditFindmatchingparentesis();
    DECLARE_MESSAGE_MAP()
};
