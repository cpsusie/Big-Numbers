#pragma once

#include <Scanner.h>

class CTestLexStreamDlg : public CDialog {
private:
  HICON         m_hIcon;
  LexFileStream m_stream;
public:
  CTestLexStreamDlg(CWnd *pParent = nullptr);

    enum { IDD = IDD_TESTLEXSTREAM_DIALOG };
    UINT      m_count;
    CString m_text;

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    virtual void OnCancel();
    virtual void OnOK();
    afx_msg void OnClose();
    afx_msg void OnOpen();
    afx_msg void OnButtonRead();
    DECLARE_MESSAGE_MAP()
};
