#pragma once

class PrecisionDlg : public CDialog {
private:
    HACCEL    m_accelTabel;

public:
    PrecisionDlg(int prec, CWnd* pParent = NULL);

    enum { IDD = IDD_DIALOGPREC };
    long    m_precision;

	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:

    virtual void OnOK();
    virtual BOOL OnInitDialog();
	afx_msg void OnGotoPrecision();
    DECLARE_MESSAGE_MAP()
};
