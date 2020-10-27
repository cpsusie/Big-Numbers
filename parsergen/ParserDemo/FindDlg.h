#pragma once


#include <Scanner.h>
#include <MFCUtil/ComboBoxWithHistory.h>
#include <MFCUtil/OBMButton.h>
#include "SearchMachine.h"

class TextPosition : public SourcePosition {
public:
  TextPosition() {
    reset();
  }
  TextPosition(int lineNumber, int column) : SourcePosition(lineNumber, column) {
  }
  TextPosition(const SourcePosition &pos) : SourcePosition(pos) {
  }

  void reset() {
    setLocation(-1, -1);
  }
  bool isSet() const {
    return getLineNumber() >= 0;
  }
  int cmp(const TextPosition &a) const;
};

class TextPositionPair {
public:
  TextPosition m_pos1, m_pos2;
  TextPositionPair() {}
  TextPositionPair(int line1, short col1, int line2, short col2) : m_pos1(line1, col1), m_pos2(line2, col2) {}
  TextPositionPair(int line, short col, int len) : m_pos1(line, col), m_pos2(line, col+len) {}
  TextPositionPair(const TextPosition &pos1, const TextPosition &pos2) : m_pos1(pos1), m_pos2(pos2) {}
  bool isSet() const { return m_pos1.isSet(); }
};

class TextContainer {
public:
  virtual TextPositionPair searchText(const FindParameter &param) = 0;
};

class FindDlg : public CDialog {
private:
  TextContainer       &m_TextContainer;
  FindParameter       &m_param;
  CComboBoxWithHistory m_findWhatCombo;
  OBMButton            m_specialCharButton;
  int                  m_selStart, m_selEnd;
  void                 addRegSymbol(const TCHAR *s, int cursorPos);
  void                 gotoFindWhat();
// Construction
public:
  FindDlg(FindParameter &param, TextContainer &tc, CWnd *pParent = nullptr);
  enum { IDD = IDD_DIALOGFIND };
  BOOL    m_matchCase;
  BOOL    m_matchWholeWord;
  BOOL    m_useRegExp;
  CString m_findWhat;

  virtual BOOL OnInitDialog();
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual void DoDataExchange(CDataExchange *pDX);
  afx_msg void OnFindnext();
  afx_msg void OnButtonregsymbolsmenu();
  afx_msg void OnRegsymbolsAnycharacter();
  afx_msg void OnRegsymbolsCharacterinrange();
  afx_msg void OnRegsymbolsCharacternotinrange();
  afx_msg void OnRegsymbolsBeginningofline();
  afx_msg void OnRegsymbolsEndofline();
  afx_msg void OnRegsymbols0ormoreoccurrences();
  afx_msg void OnRegsymbols1ormoreoccurrences();
  afx_msg void OnRegsymbols0or1occurence();
  afx_msg void OnRegsymbolsOr();
  afx_msg void OnRegsymbolsGroup();
  afx_msg void OnSetfocusCombofindwhat();
  afx_msg void OnSelendokCombofindwhat();
  afx_msg void OnSelchangeCombofindwhat();
  DECLARE_MESSAGE_MAP()
};
