#pragma once

class CMessageDlg : public CDialog {
private:
  HACCEL m_accelTable;
  int    m_milliSeconds;
  String m_caption;
  String m_message;
  bool   m_timerIsRunning;

  void startTimer(UINT milliSeconds);
  void stopTimer();
public:
  CMessageDlg(int milliSeconds, const String &caption, const String &message, CWnd* pParent = NULL);

	enum { IDD = IDD_MESSAGE_DIALOG };


	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:

	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnCloseWindow();
	DECLARE_MESSAGE_MAP()
};

