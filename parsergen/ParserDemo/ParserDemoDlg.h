#pragma once

#include "FindDlg.h"
#include "TextBox.h"
#include "TestParser.h"

class CParserDemoDlg : public CDialog, public TextContainer, public ParserHandler {
private:
  HICON                 m_hIcon;
  CFont                 m_printFont;
  HACCEL                m_accelTable;
  CTextBox              m_textBox;
  CString               m_input;
  BOOL                  m_breakOnProduction;
  BOOL                  m_breakOnError;
  BOOL                  m_breakOnState;
  BOOL                  m_breakOnSymbol;
  SourcePosition        m_lastSourcePosition;
  BitSet               *m_breakProductions;
  BitSet               *m_breakStates;
  BitSet               *m_breakSymbols;
  bool                  m_breakPosDone;
  bool                  m_inputHasChanged;
  bool                  m_animateOn;
  CSize                 m_charSize;
  FindParameter         m_findParam;
  Array<SourcePosition> m_errorPos, m_debugPos;

public:
  CWinThread           *m_showStateThread;
  TestParser            m_parser;

  CParserDemoDlg(CWnd *pParent = nullptr);
  ~CParserDemoDlg();
  void             showLastDebugLine();
  void             showStatus(bool gotoLastDebug = true);
  int              findStackElement(const CPoint &p);
  void             setBreakText(int controlId, const StringArray &textArray);
  void             setBreakProdText();
  void             setBreakStateText();
  void             setBreakSymbolText();
  void             OnSelChangeEditInputString();
  void             beginParse();
  SourcePosition   getSourcePosition();
  int              getSourcePositionIndex();
  String           getCurrentWord();

  String           getInputString() {
    return m_input.GetBuffer(m_input.GetLength());
  }

  void             updateSourcePosition(const SourcePosition &pos);
  int              findSourcePositionIndex(const Array<SourcePosition> &list, const SourcePosition &pos);
  void             gotoTextPosition(const SourcePosition &pos);
  void             gotoTextPosition(int id, const Array<SourcePosition> &list, const SourcePosition &pos);
  void             markSourcePosition(const TextPositionPair &pos);
  TextPositionPair searchText(const FindParameter &m_param);                                    // virtual from textcontainer
  void             find(const FindParameter &param);
  void             resetListBoxes();
  int              handleReduction(unsigned int prod);                                          // virtual from parserhandler
  void             handleError(const SourcePosition &pos, const TCHAR *form, va_list argptr);   // do
  void             handleDebug(const SourcePosition &pos, const TCHAR *form, va_list argptr);   // do

  enum { IDD = IDR_MAINFRAME };

protected:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnFileOpen();
  afx_msg void OnFileDumpActionMatrix();
  afx_msg void OnFileExit();
  afx_msg void OnOk();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
  afx_msg void OnEditSelectProductionsToBreakOn();
  afx_msg void OnEditSelectStatesToBreakOn();
  afx_msg void OnEditFind();
  afx_msg void OnEditFindNext();
  afx_msg void OnEditFindPrev();
  afx_msg void OnEditBreakOnTextPosition();
  afx_msg void OnEditNextError();
  afx_msg void OnEditPrevError();
  afx_msg void OnEditSelectSymbolsToBreakOn();
  afx_msg void OnEditFindMatchingParanthes();
  afx_msg void OnEditGoto();
  afx_msg void OnEditDerivationTree();
  afx_msg void OnRunStartDebugResetParser();
  afx_msg void OnRunStartDebugGo();
  afx_msg void OnRunStartDebugStep();
  afx_msg void OnRunStartDebugStepOver();
  afx_msg void OnRunParseInput();
  afx_msg void OnOptionsStackSize();
  afx_msg void OnOptionsShowState();
  afx_msg void OnOptionsErrors();
  afx_msg void OnOptionsShowLegalInput();
  afx_msg void OnOptionsListDebugFromParser();
  afx_msg void OnOptionsListDebugFromScanner();
  afx_msg void OnOptionsAnimate();
  afx_msg void OnCheckBreakOnSymbol();
  afx_msg void OnCheckBreakOnProduction();
  afx_msg void OnCheckBreakOnError();
  afx_msg void OnCheckBreakOnState();
  afx_msg void OnChangeEditInputString();
  afx_msg void OnSelChangeListErrors();
  afx_msg void OnSelChangeListDebug();
  afx_msg void OnSetFocusEditInputString();
  afx_msg void OnSetFocusListErrors();
  afx_msg void OnSetFocusListDebug();
  afx_msg void OnKillFocusEditInputString();
  afx_msg void OnKillFocusListErrors();
  afx_msg void OnKillFocusListDebug();
  afx_msg void OnMaxTextEditInputString();
  afx_msg void OnHelpAboutParserDemo();
  DECLARE_MESSAGE_MAP()
};
