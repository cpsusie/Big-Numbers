#pragma once

#include <MFCUtil/ComboBoxWithHistory.h>
#include "SearchMachine.h"
#include "TextPosition.h"
void drawTriangle(CWnd *wnd);

class TextContainer {
public:
  virtual TextPositionPair searchText(const FindParameters &param) = 0;
};

class CFindDlg: public CDialog, private OptionsAccessor {
private:
  HACCEL               m_accelTable;
  CComboBoxWithHistory m_findWhatCombo;
  CFont                m_font;
  int                  m_selStart, m_selEnd;
  int                  m_currentControl;
  TextContainer       *m_textContainer;
  void                 addRegexSymbol(const TCHAR *s, int cursorpos);
public:
  CFindDlg(FindParameters &param, TextContainer *tc, CWnd *pParent = NULL);
  FindParameters &m_param;

  enum { IDD = IDD_DIALOGFIND };
  BOOL  m_matchCase;
  BOOL  m_matchWholeWord;
  BOOL  m_useRegex;
  CString   m_findWhat;
  BOOL  m_diffOnly;
  BOOL  m_nonDiffOnly;


public:
  virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
  virtual void DoDataExchange(CDataExchange* pDX);

protected:

  afx_msg void OnFindNext();
  virtual void OnCancel();
  virtual BOOL OnInitDialog();
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
  afx_msg void OnGotoFindWhat();
  afx_msg void OnSelEndOkComboFindWhat();
  afx_msg void OnSelChangeComboFindWhat();
  afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
  afx_msg void OnCheckLimitDiff();
  afx_msg void OnCheckLimitNonDiff();
  DECLARE_MESSAGE_MAP()
};

