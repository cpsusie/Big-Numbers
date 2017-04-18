#pragma once

#include "ParametricGraph.h"
#include <MFCUtil/LayoutManager.h>

class CParametricGraphDlg : public CDialog {
private:
    HACCEL              m_accelTable;
    SimpleLayoutManager m_layoutManager;
    CFont               m_exprFont;
    String              m_fullName;

    CString             m_name;
    CString             m_style;
    CString             m_exprX;
    CString             m_exprY;
    double              m_tFrom;
    double              m_tTo;
    UINT                m_steps;

    CComboBox *getStyleCombo() {
      return (CComboBox*)GetDlgItem(IDC_COMBOSTYLE);
    }
    CMFCColorButton *getColorButton() {
      return (CMFCColorButton*)GetDlgItem(IDC_BUTTONCOLOR);
    }
    bool validate();
    void paramToWin(            const ParametricGraphParameters &param);
    void winToParam(                  ParametricGraphParameters &param);
    void saveAs(                      ParametricGraphParameters &param);
    void save(const String &fileName, ParametricGraphParameters &param);
    void addToRecent(const String &fileName);

public:
  CParametricGraphDlg(ParametricGraphParameters &param, CWnd *pParent = NULL);
  virtual ~CParametricGraphDlg();

  ParametricGraphParameters &m_param;

  enum { IDD = IDD_PARAMETRICGRAPH_DIALOG };

protected:
    virtual BOOL PreTranslateMessage(MSG *pMsg);
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnGotoName();
    afx_msg void OnGotoStyle();
    afx_msg void OnGotoExprX();
    afx_msg void OnGotoExprY();
    afx_msg void OnGotoTInterval();
    afx_msg void OnGotoStep();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnFileNew();
    afx_msg void OnFileOpen();
    afx_msg void OnFileSave();
    afx_msg void OnFileSaveAs();
    afx_msg void OnEditFindmatchingparentesis();
    DECLARE_MESSAGE_MAP()
};
