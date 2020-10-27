#pragma once

#include <TinyBitSet.h>
#include "HighScore.h"

class NumberSet : public BitSet16 {
public:
  NumberSet() {
    addAll();
  }
  void addAll() {
    add(1,10);
  }
};

class CRegneTesterDlg : public CDialog {
private:
  HICON       m_hIcon;
  HACCEL      m_accelTable;
  bool        m_timerIsRunning;
  Timestamp   m_startTime;
  double      m_usedTime;
  int         m_watchTime;;
  NumberSet   m_numbersLeft;
  int         m_tableNumber;
  int         m_answerCount;
  int         m_execiseType;
  int         m_expectedAnswer;
  Options     m_options;
  CDC         m_watchDC;
  CBitmap     m_watchBitmap;
  CDC         m_watchWorkDC;
  CBitmap     m_watchWorkBitmap;
  BITMAP      m_watchInfo;
  void startTimer();
  void stopTimer();
  void setWatchBitmap();
  void setEinsteinBitmap();
  void generateExecise();
  void setAnswerCount(int value);
  void updateTime();
  void updateWatch();
  void drawWatch(CDC &dc, int t);

  void setExeciseType(int type       );
  void setTableNumber(int tableNumber);
  void startExecise();
  void stopExecise(bool checkHighScore);
  void resetExecise();
  void ajourStartButtonVisible();
public:
    CRegneTesterDlg(CWnd *pParent = nullptr);

    enum { IDD = IDD_REGNETESTER_DIALOG };
    CString m_result;

protected:
    virtual BOOL PreTranslateMessage(MSG *pMsg);
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    virtual void OnCancel();
    virtual void OnOK();
    afx_msg void OnClose();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnFileHighscore();
    afx_msg void OnFileIndstillinger();
    afx_msg void OnFileAfslut();
    afx_msg void OnTabel1tabel();
    afx_msg void OnTabel2tabel();
    afx_msg void OnTabel3tabel();
    afx_msg void OnTabel4tabel();
    afx_msg void OnTabel5tabel();
    afx_msg void OnTabel6tabel();
    afx_msg void OnTabel7tabel();
    afx_msg void OnTabel8tabel();
    afx_msg void OnTabel9tabel();
    afx_msg void OnTabel10tabel();
    afx_msg void OnHelpAboutRegneTester();
    afx_msg void OnOpgavetypeAdd();
    afx_msg void OnOpgavetypeSubtract();
    afx_msg void OnOpgavetypeMultiply();
    afx_msg void OnOpgavetypeDivide();
    afx_msg void OnBnClickedButtonStart();
    DECLARE_MESSAGE_MAP()

//  afx_msg LRESULT OnMsgDrawWatch(WPARAM wp, LPARAM lp);

};

