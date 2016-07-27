#pragma once

#include "TestParser.h"
#include <LayoutManager.h>

class TreeDlg : public CDialog {
private:
    HACCEL              m_accelTable;
    SimpleLayoutManager m_layoutManager;
    SyntaxNodep         m_tree;
    CImageList          m_images;

    void traverse(         CTreeCtrl *ctrl, SyntaxNodep tree, HTREEITEM p);
    void expandAll(        CTreeCtrl *ctrl, HTREEITEM p);
    void getDerivation(    CTreeCtrl *ctrl, HTREEITEM item, String &derivation, int maxlen);
    String getDerivation(  CTreeCtrl *ctrl, HTREEITEM item,int maxlen);
    HTREEITEM findTreeItem(CTreeCtrl *ctrl, const CPoint &pt);

public:
	TreeDlg(SyntaxNodep tree, CWnd* pParent = NULL);


	//{{AFX_DATA(TreeDlg)
	enum { IDD = IDD_DIALOGTREE };
	//}}AFX_DATA


	//{{AFX_VIRTUAL(TreeDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(TreeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonExpand();
	afx_msg void OnButtonExpandAll();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCollapse();
	afx_msg void OnExpand();
	virtual void OnOK();
	afx_msg void OnExpandOrCollapse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
