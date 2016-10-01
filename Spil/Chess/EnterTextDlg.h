#pragma once

class CEnterTextDlg : public CDialog {
public:
	CEnterTextDlg(const String &str, CWnd* pParent = NULL);

	enum { IDD = IDD_ENTERTEXT_DIALOG };
	CString	m_text;


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:

	virtual void OnOK();
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};

