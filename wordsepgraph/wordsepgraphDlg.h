#pragma once

#include "TrainerJob.h"

class CWordsepgraphDlg : public CDialog {
private:
  HICON       m_hIcon;
  HACCEL      m_accelTable;
  CFont       m_printFont;
  int         m_threadPriority;
  TrainerJob *m_trainerJob;
  WordBpn     m_network;
  bool        m_timerIsRunning;

  void enableButtons(bool enabled);
  void showResultDetail(const String &word);
  void showMessage(_In_z_ _Printf_format_string_ TCHAR const * const format,...);
  void stopTraining();
  void startTimer();
  void stopTimer();
  void setTrainingPriority(int p);

public:
  CWordsepgraphDlg(CWnd *pParent = NULL);

  enum { IDD = IDD_WORDSEPGRAPH_DIALOG };
  CString m_ord;
  CString m_details;

protected:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual void OnOK();
  virtual void OnCancel();
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg void OnClose();
  afx_msg void OnFileLoadNetwork();
  afx_msg void OnFileSaveNetwork();
  afx_msg void OnFileStartTraining();
  afx_msg void OnFileStopTraining();
  afx_msg void OnFileTrainingdata();
  afx_msg void OnFileErrorwords();
  afx_msg void OnFileQuit();
  afx_msg void OnOptionsTrainingParameters();
  afx_msg void OnButtonSeparate();
  afx_msg void OnButtonLearn();
  afx_msg void OnGotoEditord();
  afx_msg void OnChangeEditOrd();
  DECLARE_MESSAGE_MAP()
};
