#pragma once

#include <Thread.h>
#include <MFCUtil/LayoutManager.h>
#include <MFCUtil/OBMButton.h>
#include "FindHistory.h"

class CFindDlg : public CDialog, SettingsAccessor {
private:
  HACCEL              m_accelTable;
  SimpleLayoutManager m_layoutManager;
  FindHistory         m_history;
  SearchMachine      &m_searchMachine;
  bool                m_timerIsRunning;
  bool                m_waitCursorOn;
  int                 m_currentControl, m_selStart, m_selEnd;
  AddrRange           m_result;
  Thread             *m_searchThread;

  void addSpecialChar(const String &s, int cursorPos);
  void updateByteSequence(const CString &findWhat);
  CComboBox *getComboFindWhat();
  void startTimer();
  void stopTimer();
  void waitCursor(bool on);
  void enableControls(bool enable);
public:
  CFindDlg(SearchMachine &searchMachine, CWnd* pParent = NULL);
  ~CFindDlg();
  const AddrRange &getSearchResult() const {
    return m_result;
  }

	enum { IDD = IDD_DIALOGFIND };
	OBMButton	m_specialCharButton;
	CString	  m_findWhat;

	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
  virtual void DoDataExchange(CDataExchange* pDX);

protected:

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnFindNext();
	afx_msg void OnButtonSpecialChar();
  afx_msg void OnGotoFindWhat();
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
	virtual void OnCancel();
  DECLARE_MESSAGE_MAP()
};

