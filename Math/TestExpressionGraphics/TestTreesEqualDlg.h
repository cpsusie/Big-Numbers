#pragma once

#include <MFCUtil/ComboBoxWithHistory.h>

class CTestTreesEqualDlg : public CDialog {
private:
  HACCEL                  m_accelTabel;
  SimpleLayoutManager     m_layoutManager;
  PixRectDevice           m_device;
  CComboBoxWithHistory    m_cb[2];
  Expression              m_e[2];
  ExpressionImage         m_image[2];
  int                     m_contextWinIndex, m_focusCtrlId;

  bool     compile(     int index);
  void     makeImage(   int index);
  void     paintImage(  int index);
  void     destroyImage(int index);
  void     paintImage(int winId, const ExpressionImage &image);
  ExpressionImage makeImage(const Expression &e);
  void ajourButtons();

public:
    CTestTreesEqualDlg(CWnd *pParent = NULL);

	enum { IDD = IDD_TREESEQUAL_DIALOG };
	CString	m_expr1;
	CString	m_expr2;


	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:

	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    afx_msg void OnTestTreesEqual();
	afx_msg void OnTestTreesEqualMinus();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonCompile();
    afx_msg void OnEditFindMatchingParentesis();
	afx_msg void OnButtonConvert();
    afx_msg void OnContextMenuShowTree();
    afx_msg void OnGotoExpr1();
    afx_msg void OnGotoExpr2();
	afx_msg void OnSetFocusEditExpr1();
	afx_msg void OnKillFocusEditExpr1();
	afx_msg void OnSetFocusEditExpr2();
	afx_msg void OnKillFocusEditExpr2();
    DECLARE_MESSAGE_MAP()
};

