#pragma once

class CTesttooltipDlg : public CDialog {
private:
	HICON m_hIcon;
public:
	CTesttooltipDlg(CWnd *pParent = nullptr);

	enum { IDD = IDD_TESTTOOLTIP_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange *pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
  afx_msg BOOL OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnButton1();
	DECLARE_MESSAGE_MAP()
};
