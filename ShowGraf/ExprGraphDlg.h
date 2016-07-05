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
    CComboBox *getStyleCombo();

    bool validate();
    void paramToWin(            const ExpressionGraphParameters &param);
    void winToParam(                  ExpressionGraphParameters &param);
    void saveAs(                      ExpressionGraphParameters &param);
    void save(const String &fileName, ExpressionGraphParameters &param);
    void addToRecent(const String &fileName);

public:
    CExprGraphDlg(ExpressionGraphParameters &param, int showFlags = SHOW_INTERVAL|SHOW_STEP, CWnd* pParent = NULL);   // standard constructor
    ExpressionGraphParameters &m_param;
    COLORREF                   m_color;
    int                        m_showFlags;

    //{{AFX_DATA(CExprGraphDlg)
	enum { IDD = IDD_EXPRGRAPH_DIALOG };
    CString m_expr;
    CString m_name;
    double  m_xfrom;
    double  m_xto;
    UINT    m_steps;
	CString	m_style;
	//}}AFX_DATA

    //{{AFX_VIRTUAL(CExprGraphDlg)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    //}}AFX_VIRTUAL

protected:
    //{{AFX_MSG(CExprGraphDlg)
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnGotoName();
	afx_msg void OnGotoStyle();
    afx_msg void OnGotoExpr();
    afx_msg void OnGotoXInterval();
    afx_msg void OnGotoStep();
    afx_msg void OnButtonColor();
    afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnEditFindmatchingparentesis();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
