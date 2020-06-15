#pragma once

#include "TestParser.h"

class TreeDlg : public CDialog {
private:
  HACCEL              m_accelTable;
  SyntaxNodep         m_tree;
  CImageList          m_images;

  CTreeCtrl *getTreeCtrl();
  void       traverse(     CTreeCtrl *ctrl, SyntaxNodep tree, HTREEITEM p);
  void       getDerivation(CTreeCtrl *ctrl, HTREEITEM item, String &derivation, int maxlen);
  String     getDerivation(CTreeCtrl *ctrl, HTREEITEM item,int maxlen);
public:
	TreeDlg(SyntaxNodep tree, CWnd *pParent = NULL);
	enum { IDD = IDD_DIALOGTREE };
public:
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG *pMsg);
	afx_msg void OnButtonExpand();
	afx_msg void OnButtonExpandAll();
	afx_msg void OnCollapse();
	afx_msg void OnExpand();
	afx_msg void OnExpandOrCollapse();
	DECLARE_MESSAGE_MAP()
};

