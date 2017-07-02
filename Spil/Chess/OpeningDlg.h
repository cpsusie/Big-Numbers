#pragma once

#include <MFCUTIL/LayoutManager.h>
#include "OpeningLibrary.h"

class COpeningDlg : public CDialog {
private:
  HACCEL              m_accelTable;
  SimpleLayoutManager m_layoutManager;
  OpeningLibrary      m_lib;
  CImageList          m_images;
  String              m_dialogName;

  CTreeCtrl *getTreeCtrl() {
    return (CTreeCtrl*)GetDlgItem(IDC_TREE_OPENING);
  }
  Game &getSelectedVariant(Game &g);
  String getActiveOpenings();
public:
    COpeningDlg(CWnd *pParent = NULL);

    enum { IDD = IDD_OPENINGLIB_DIALOG };

    void traverse(   CTreeCtrl *ctrl, HTREEITEM p, const LibraryState &state, Game &g);
    void expandAll(  CTreeCtrl *ctrl, HTREEITEM p);
    void collapseAll(CTreeCtrl *ctrl, HTREEITEM p);

    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG *pMsg);
    virtual void DoDataExchange(CDataExchange *pDX);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnButtonExpand();
    afx_msg void OnButtonExpandAll();
    afx_msg void OnOpeningExpand();
    afx_msg void OnSelChangedOpeningTree(NMHDR *pNMHDR, LRESULT *pResult);
    DECLARE_MESSAGE_MAP()
};

