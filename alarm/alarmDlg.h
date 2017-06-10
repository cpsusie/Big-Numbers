#pragma once

#include <MP3Player.h>

#define MAXTIMERCOUNT  10

#define ALARM_TIMER     1
#define PLAYSOUND_TIMER 2
#define SHOWTIME_TIMER  3

class CAlarmDlg : public CDialog {
public:
  CAlarmDlg(CWnd *pParent = NULL);
private:
  HICON     m_hIcon;
  CString   m_soundFileName;
  bool      m_hasStatusIcon;
  bool      m_timerIsRunning[MAXTIMERCOUNT];
  MP3Player m_player;
  HACCEL    m_accelTable;

  void startTimer(UINT timerIndex, int waitSeconds);
  void stopTimer( UINT timerIndex);
  void showTime();
  void readSetup();
  void writeSetup();
  void addStatusIcon();
  void deleteStatusIcon();
  bool startPlayer();
  void stopPlayer();
  void gotoField(        int id);
  bool isMenuItemChecked(int id) const;
  void checkMenuItem(    int id, bool checked);
  bool toggleMenuItem(   int id);

	enum { IDD = IDD_ALARM_DIALOG };
  CString m_time;

	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
  virtual void DoDataExchange(CDataExchange* pDX);
  virtual BOOL OnInitDialog();
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  virtual void OnCancel();
  virtual void OnOK();
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg void OnFileExit();
  afx_msg void OnOptionsSound();
  afx_msg void OnButtonPlay();
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnMenuExit();
  afx_msg void OnMenuOpen();
	afx_msg void OnClose();
	afx_msg void OnHelpAboutalarm();
	afx_msg void OnSetalarm();
	afx_msg void OnGotoAlarmTime();
	afx_msg void OnOptionsHidewhensetalarm();
  DECLARE_MESSAGE_MAP()
};
