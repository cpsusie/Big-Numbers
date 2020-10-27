#pragma once

#include <MFCUtil/ComboBoxWithHistory.h>
#include <MFCUtil/OBMButton.h>
#include "SearchMachine.h"
#include "TextPosition.h"

class TextContainer {
public:
  virtual TextPositionPair searchText(const FindParameters &param) = 0;
};

class CFindDlg: public CDialog, private OptionsAccessor {
private:
  CComboBoxWithHistory m_findWhatCombo;
  OBMButton            m_specialCharButton;
  CFont                m_font;
  int                  m_selStart, m_selEnd;
  int                  m_currentControl;
  TextContainer       *m_textContainer;
  void                 addRegexSymbol(const TCHAR *s, int cursorpos);
  void                 gotoFindWhat();
public:
  CFindDlg(FindParameters &param, TextContainer *tc, CWnd *pParent = nullptr);
  FindParameters &m_param;

  enum { IDD = IDD_DIALOGFIND };
  BOOL    m_matchCase;
  BOOL    m_matchWholeWord;
  BOOL    m_useRegex;
  CString m_findWhat;
  BOOL    m_diffOnly;
  BOOL    m_nonDiffOnly;

public:
  virtual BOOL OnInitDialog();
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual void DoDataExchange(CDataExchange *pDX);
  afx_msg void OnFindNext();
  afx_msg void OnButtonRegSymbolsMenu();
  afx_msg void OnRegSymbolsAnyChar();
  afx_msg void OnRegSymbolsCharInRange();
  afx_msg void OnRegSymbolsCharNotInRange();
  afx_msg void OnRegSymbolsBeginningOfLine();
  afx_msg void OnRegSymbolsEndOfLine();
  afx_msg void OnRegSymbols0orMoreOccurrences();
  afx_msg void OnRegSymbols1orMoreOccurrences();
  afx_msg void OnRegSymbols0or1Occurence();
  afx_msg void OnRegSymbolsOr();
  afx_msg void OnRegSymbolsGroup();
  afx_msg void OnSetFocusComboFindWhat();
  afx_msg void OnKillFocusComboFindWhat();
  afx_msg void OnCheckLimitDiff();
  afx_msg void OnCheckLimitNonDiff();
  DECLARE_MESSAGE_MAP()
};
