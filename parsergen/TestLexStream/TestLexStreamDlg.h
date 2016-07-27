#pragma once

#include <Scanner.h>


class CTestLexStreamDlg : public CDialog {
private:
  LexFileStream m_stream;
public:
    CTestLexStreamDlg(CWnd* pParent = NULL);

    //{{AFX_DATA(CTestLexStreamDlg)
	enum { IDD = IDD_TESTLEXSTREAM_DIALOG };
	UINT	m_count;
	CString	m_text;
	//}}AFX_DATA

    //{{AFX_VIRTUAL(CTestLexStreamDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    //}}AFX_VIRTUAL

protected:
    HICON m_hIcon;

    //{{AFX_MSG(CTestLexStreamDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    virtual void OnCancel();
    virtual void OnOK();
    afx_msg void OnClose();
    afx_msg void OnOpen();
	afx_msg void OnButtonRead();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
