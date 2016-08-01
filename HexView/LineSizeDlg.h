#pragma once

class CLineSizeDlg : public CDialog, SettingsAccessor {
private:
  HACCEL     m_accelTable;

  void enableLineSize();
public:
  CLineSizeDlg(CWnd* pParent = NULL);
  BOOL OnInitDialog();

	enum { IDD = IDD_DIALOGLINESIZE };
    UINT  m_lineSize;
	BOOL  m_fitLines;


	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
  virtual void DoDataExchange(CDataExchange* pDX);

protected:

    afx_msg void OnGotoLineSize();
	afx_msg void OnCheckFitLines();
	virtual void OnOK();
  DECLARE_MESSAGE_MAP()
};

