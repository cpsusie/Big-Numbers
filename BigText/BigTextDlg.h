#pragma once

class CBigTextDlg : public CDialogEx {
private:
	HICON m_hIcon;
  CString m_text;
public:
	CBigTextDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_BIGTEXT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
  afx_msg void OnClickedMakeBigText();
  afx_msg void OnClose();
  virtual void OnCancel();
  virtual void OnOK();
	DECLARE_MESSAGE_MAP()
};
