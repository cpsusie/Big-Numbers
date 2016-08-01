#pragma once

#include "AvlTree.h"

class CAvlTreeWndDlg : public CDialog, public AvlGraphics {
private:
  CString             m_currentKey;
  SimpleLayoutManager m_layoutManager;
  AvlTree             m_testTree;
  AvlIterator        *m_itererator;
  HACCEL              m_accelTable;
  CFont               m_font;
  AvlNode            *m_selectedNode;
public:
  CAvlTreeWndDlg(CWnd* pParent = NULL);
  void vmessage(COLORREF color, const TCHAR *format, va_list argptr);
  CRect getTreeRect();
  CWnd *getWindow();
  CSize getCharSize();

    enum { IDD = IDD_MAINDIALOG };

	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:
  HICON m_hIcon;

    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnEditInsert();
    afx_msg void OnEditDeletekey();
    afx_msg void OnEditSearchkey();
    afx_msg void OnEditClear();
    afx_msg void OnFileExit();
    afx_msg void OnOptionsTrace();
    afx_msg void OnOptionsShowbalance();
    afx_msg void OnEditInsertmany();
    afx_msg void OnEditFibonaccitreeType1();
    afx_msg void OnEditFibonaccitreeType2();
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnClose();
    afx_msg void OnHelpAboutavtree();
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    afx_msg void OnMenuDelete();
  DECLARE_MESSAGE_MAP()
};

