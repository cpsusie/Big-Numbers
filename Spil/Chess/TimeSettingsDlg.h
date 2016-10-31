#pragma once

typedef enum {
  SELECTED_BOTH
 ,SELECTED_WHITE
 ,SELECTED_BLACK
} SelectedComboItem;

typedef enum {
  TAB_LEVELS
 ,TAB_TIMEDGAME
 ,TAB_SPEEDCHESS
} ClockTab;

class TimeParametersWithComboSelection {
private:
  void initSelectedComboItem();
public:
  TimeParameters    m_tm[2];
  SelectedComboItem m_selectedComboItem;
  void init(const TimeParameters &tmw, const TimeParameters &tmb);
};

class CTimeSettingsDlg : public CDialog, OptionsAccessor {
private:
  HACCEL                           m_accelTable;
  TimeParametersWithComboSelection m_tm[2];
  LevelTimeout                     m_lt;
  const bool                       m_clockRunning;
  int                              m_currentControl[3];
  void setCurrentControl(ClockTab tab, int id);
  void gotoControl(int id);
  CComboBox *getPlayerCombo(ClockTab tab);
  void selChangePlayerCombo(ClockTab tab, TimeParametersWithComboSelection &tm);
//  void paramToWindow(const TimeParametersWithComboSelection &tm, bool speedChess);
  bool windowToParam(      TimeParametersWithComboSelection &tm, bool speedChess);
  void paramToWindow(const TimeParameters &tm, bool speedChess);
  bool windowToParam(      TimeParameters &tm, bool speedChess);
  void paramToWindow(const LevelTimeout   &lt);
  bool windowToParam(      LevelTimeout   &lt);
  bool windowToParam();
  void initPlayerComboItems();
  void initPlayerComboItems(CComboBox *cb);
  void gotoSeconds(int indexl);
  void gotoMinutes(int index);
  void showLevelsTab();
  void showTimedGameTab();
  void showSpeedChessTab();
  void enableLevels(         bool enable);
  void enableTimedGame(      bool enable);
  void enableSpeedChess(     bool enable);
  void enableMoveCountFields(bool enable);
  void enableTimeFields(     bool enable);
  void setVisible(bool visible, const int *ctrlArray, int size);
  ClockTab getCurrentTab();
public:
	CTimeSettingsDlg(bool clockRunning, CWnd* pParent = NULL);
    bool getResetClocks() const {
      return m_resetClocks ? true : false;
    }
	enum { IDD = IDD_TIMESETTINGS_DIALOG };
	CTabCtrl	m_clockType;
	UINT	    m_min1;
	UINT	    m_min2;
	UINT	    m_min3;
	UINT	    m_min4;
	UINT	    m_min5;
	UINT	    m_min6;
	float	    m_sec1;
	float	    m_sec2;
	float	    m_sec3;
	float	    m_sec4;
	float	    m_sec5;
	float	    m_sec6;
    UINT        m_movesLeft;
    UINT        m_hoursLeft;
    UINT        m_minutesLeft;
    UINT        m_secondsLeft;
    UINT        m_secondsIncr;
    UINT        m_speedChessHoursLeft;
    UINT        m_speedChessMinutesLeft;
    UINT        m_speedChessSecondsLeft;
    UINT        m_speedChessSecondsIncr;
    BOOL        m_resetClocks;


	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnGotoMinutes1();
	afx_msg void OnGotoMinutes2();
	afx_msg void OnGotoMinutes3();
	afx_msg void OnGotoMinutes4();
	afx_msg void OnGotoMinutes5();
	afx_msg void OnGotoMinutes6();
    afx_msg void OnGotoPlayer();
    afx_msg void OnGotoMoveCount();
    afx_msg void OnGotoTime();
    afx_msg void OnGotoTimeIncrement();
    afx_msg void OnSelChangeComboPlayer();
    afx_msg void OnSelectTimedGame();
    afx_msg void OnSelectLevels();
    afx_msg void OnSelectSpeedChess();
    afx_msg void OnSelchangeTabClocktype(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnSetfocusComboplayer();
    afx_msg void OnSetfocusSpeedcomboplayer();
    afx_msg LRESULT OnMsgShowDropdown(      WPARAM wp, LPARAM lp);
	DECLARE_MESSAGE_MAP()
};

