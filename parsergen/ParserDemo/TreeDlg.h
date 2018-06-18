#pragma once

#include "TestParser.h"
#include <MFCUtil/LayoutManager.h>

class TreeDlg : public CDialog {
private:
    HACCEL              m_accelTable;
    SimpleLayoutManager m_layoutManager;
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
	virtual BOOL PreTranslateMessage(MSG *pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);

protected:

	virtual BOOL OnInitDialog();
	afx_msg void OnButtonExpand();
	afx_msg void OnButtonExpandAll();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCollapse();
	afx_msg void OnExpand();
	virtual void OnOK();
	afx_msg void OnExpandOrCollapse();
	DECLARE_MESSAGE_MAP()
};

