#pragma once

#include <MFCUtil/ComboBoxWithHistory.h>
#include <MFCUtil/OBMButton.h>
#include "RegexFilter.h"

class CRegexDlg: public CDialog {
private:
  CComboBoxWithHistory m_regexCombo;
  OBMButton            m_regexCharButton;
  void gotoRegexCombo();
public:
  CRegexDlg(RegexFilter &param, CWnd *pParent = nullptr);
  void addRegexSymbol(const TCHAR *s, int cursorpos);

  RegexFilter  &m_param;
  int           m_selStart;
  int           m_selEnd;
  int           m_currentControl;

  enum { IDD = IDD_DIALOGREGEX };
  BOOL      m_matchCase;
  BOOL      m_matchWholeWord;
  CString   m_regex;


public:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnKillFocusComboRegex();
  afx_msg void OnSetFocusComboRegex();
  afx_msg void OnButtonRegSymbolsMenu();
  afx_msg void OnRegSymbolsAnyChar();
  afx_msg void OnRegSymbolsCharInRange();
  afx_msg void OnRegSymbolsCharNotInRange();
  afx_msg void OnRegSymbolsBeginningOfLine();
  afx_msg void OnRegSymbolsEndOfLine();
  afx_msg void OnRegSymbols0OrMoreOccurrences();
  afx_msg void OnRegSymbols1OrMoreOccurrences();
  afx_msg void OnRegSymbols0Or1Occurence();
  afx_msg void OnRegSymbolsOr();
  afx_msg void OnRegSymbolsGroup();
  DECLARE_MESSAGE_MAP()
};

