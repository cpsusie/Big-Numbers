#pragma once

#include <MFCUtil/LayoutManager.h>
#include "IsoCurveGraph.h"

class CIsoCurveGraphDlg : public CDialog {
private:
    HACCEL              m_accelTable;
    SimpleLayoutManager m_layoutManager;
    CFont               m_exprFont;
    String              m_fullName;

    CString             m_name;
    CString             m_style;
    CString             m_expr;
    double              m_cellSize;
    double              m_xFrom;
    double              m_xTo;
    double              m_yFrom;
    double              m_yTo;

    CComboBox *getStyleCombo() {
      return (CComboBox*)GetDlgItem(IDC_COMBOSTYLE);
    }
    CMFCColorButton *getColorButton() {
      return (CMFCColorButton*)GetDlgItem(IDC_BUTTONCOLOR);
    }

    bool validate();
    void paramToWin(            const IsoCurveGraphParameters &param);
    void winToParam(                  IsoCurveGraphParameters &param);
    void saveAs(                      IsoCurveGraphParameters &param);
    void save(const String &fileName, IsoCurveGraphParameters &param);
    void addToRecent(const String &fileName);

public:
    CIsoCurveGraphDlg(IsoCurveGraphParameters &param, CWnd* pParent = NULL);
    IsoCurveGraphParameters &m_param;

    enum { IDD = IDD_ISOCURVEGRAPH_DIALOG };

protected:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnGotoName();
    afx_msg void OnGotoStyle();
    afx_msg void OnGotoExpr();
    afx_msg void OnGotoXInterval();
    afx_msg void OnGotoYInterval();
    afx_msg void OnGotoCellSize();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnFileNew();
    afx_msg void OnFileOpen();
    afx_msg void OnFileSave();
    afx_msg void OnFileSaveAs();
    afx_msg void OnEditFindmatchingparentesis();
    DECLARE_MESSAGE_MAP()
};
