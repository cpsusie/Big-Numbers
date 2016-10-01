#pragma once

class CTablebaseDlg : public CDialog, OptionsAccessor {
private:
  HACCEL                  m_accelTable;
public:
    CTablebaseDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_TABLEBASE_DIALOG };
    int     m_moveCount;
	UINT	m_defendStrength;
    int     m_tablebaseMetric;
	int		m_depthFormat;
    CString m_tablebasePath;


public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:

    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnGotoMoveCount();
    afx_msg void OnGotoDefendStrength();
    afx_msg void OnGotoPath();
	afx_msg void OnButtonPath();
	afx_msg void OnButtonDecompressAll();
	afx_msg void OnDeltaposSpinMoveCount(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpinDefendStrength(NMHDR* pNMHDR, LRESULT* pResult);
    DECLARE_MESSAGE_MAP()
};

