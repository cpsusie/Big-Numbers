#pragma once

#include <MFCUtil/ComboBoxWithHistory.h>
#include <MFCUtil/OBMButton.h>

class InterruptableRunnable;
class CFindDlg : public CDialog, SettingsAccessor {
private:
  HACCEL                 m_accelTable;
  CComboBoxWithHistory   m_findWhatCombo;
  OBMButton              m_specialCharButton;
  SearchParameters      &m_searchParameters;
  bool                   m_timerIsRunning;
  bool                   m_waitCursorOn;
  int                    m_currentControl, m_selStart, m_selEnd;
  AddrRange              m_result;
  CString                m_findWhat;

  CMainFrame &getMainWin();
  void addSpecialChar(const String &s, int cursorPos);
  void updateByteSequence(const CString &findWhat);
  CComboBox *getFindWhatCombo();
  SearchMachine *getSearchMachine() {
    return getMainWin().getSearchMachine();
  }
  void gotoFindWhat();
  void startTimer();
  void stopTimer();
  void waitCursor(bool on);
  void enableControls(bool enable);
public:
  CFindDlg(CMainFrame *parent);
  ~CFindDlg();
  const AddrRange &getSearchResult() const {
    return m_result;
  }

  enum { IDD = IDD_DIALOGFIND };

  virtual BOOL OnInitDialog();
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual void OnCancel();
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg void OnFindNext();
  afx_msg void OnButtonSpecialChar();
  afx_msg void OnSpecialCharCarriageReturn();
  afx_msg void OnSpecialCharNewline();
  afx_msg void OnSpecialCharBackspace();
  afx_msg void OnSpecialCharTab();
  afx_msg void OnSpecialCharFormfeed();
  afx_msg void OnSpecialCharEscapeCharacter();
  afx_msg void OnSpecialCharOctalNumber();
  afx_msg void OnSpecialCharDecimalNumber();
  afx_msg void OnSpecialCharHexadecimalNumber();
  afx_msg void OnSpecialCharBackslash();
  afx_msg void OnSetFocusComboFindWhat();
  afx_msg void OnKillFocusComboFindWhat();
  afx_msg void OnEditChangeComboFindWhat();
  afx_msg void OnEditUpdateComboFindWhat();
  afx_msg void OnSelChangeComboFindWhat();
  afx_msg void OnSelendOkComboFindWhat();
  afx_msg void OnSelendCancelComboFindWhat();
  DECLARE_MESSAGE_MAP()
};
