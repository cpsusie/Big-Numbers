#pragma once

#include <FlagTraits.h>
#include <PropertyContainer.h>
#include "FindDlg.h"
#include "TextBox.h"
#include "TestParser.h"
#include "StackImage.h"
#include "Debugger.h"

typedef CompactArray<SourcePosition> SourcePositionArray;

class CParserDemoDlg : public CDialog, public TextContainer, public PropertyChangeListener {
private:
  friend class Debugger;
  friend class DebuggerJob;
  HICON               m_hIcon;
  HACCEL              m_accelTable;
  CFont               m_printFont;
  CTextBox            m_textBox;
  StackImage          m_stackImage;
  CString             m_input;
  BOOL                m_breakOnProduction;
  BOOL                m_breakOnError;
  BOOL                m_breakOnState;
  BOOL                m_breakOnSymbol;
  SourcePosition      m_lastSourcePosition;
  BitSet             *m_breakProductions;
  BitSet             *m_breakStates;
  BitSet             *m_breakSymbols;
  bool                m_inputHasChanged;
  bool                m_continueAfterBreak;
  bool                m_animateOn;
  bool                m_shuttingDown;
  FindParameter       m_findParam;
  SourcePositionArray m_errorPos, m_debugPos;
  Debugger            m_debugger;
  inline bool isDebuggerActive() const {
    return m_debugger.isRunning();
  }
  void debuggerGo(BYTE breakFlags, bool continueAfterBreak = false);
  void debuggerRestart(BYTE breakFlags);
public:
  CWinThread         *m_showStateThread;
  TestParser          m_parser;

  CParserDemoDlg(CWnd *pParent = nullptr);
  ~CParserDemoDlg();
  void             showLastDebugLine();
  void             showStatus(bool gotoLastDebug = true);
  int              findStackIndexFromTop(const CPoint &p) const;
  void             setBreakText(int controlId, const StringArray &textArray);
  void             setBreakProdText();
  void             setBreakStateText();
  void             setBreakSymbolText();
  void             OnSelChangeEditInputString();
  SourcePosition   getSourcePosition() const;
  int              getSourcePositionIndex() const;
  bool             makeDerivationTree() const;
  String           getCurrentWord() const;
  String           getInputString() const {
    return (LPCTSTR)m_input;
  }
  void             handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) final;
  void             handleError(const TextAndSourcePos &tp);
  void             handleDebug(const TextAndSourcePos &tp);

  void             updateSourcePosition(const SourcePosition &pos);
  int              findSourcePositionIndex(const SourcePositionArray &list, const SourcePosition &pos) const;
  void             gotoTextPosition(const SourcePosition &pos);
  void             gotoTextPosition(int id, const SourcePositionArray &list, const SourcePosition &pos);
  void             markSourcePosition(const TextPositionPair &pos);
  TextPositionPair searchText(const FindParameter &m_param) final;                              // virtual from textcontainer
  void             find(const FindParameter &param);
  void             resetListBoxes();

  enum { IDD = IDR_MAINFRAME };

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnOk();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnClose();
  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
  afx_msg void OnPaint();
  afx_msg void OnFileOpen();
  afx_msg void OnFileDumpActionMatrix();
  afx_msg void OnFileDumpSuccesorMatrix();
  afx_msg void OnFileExit();
  afx_msg void OnEditFind();
  afx_msg void OnEditFindNext();
  afx_msg void OnEditFindPrev();
  afx_msg void OnEditNextError();
  afx_msg void OnEditPrevError();
  afx_msg void OnEditFindMatchingParanthes();
  afx_msg void OnEditGoto();
  afx_msg void OnEditBreakOnTextPosition();
  afx_msg void OnEditSelectProductionsToBreakOn();
  afx_msg void OnEditSelectStatesToBreakOn();
  afx_msg void OnEditSelectSymbolsToBreakOn();
  afx_msg void OnEditDerivationTree();
  afx_msg void OnRunParseInput();
  afx_msg void OnRunStartDebugResetParser();
  afx_msg void OnRunStartDebugGo();
  afx_msg void OnRunStartDebugStep();
  afx_msg void OnRunStartDebugStepOver();
  afx_msg void OnOptionsShowLegalInput();
  afx_msg void OnOptionsShowState();
  afx_msg void OnOptionsAnimate();
  afx_msg void OnOptionsListDebugFromParser();
  afx_msg void OnOptionsListDebugFromScanner();
  afx_msg void OnOptionsErrors();
  afx_msg void OnOptionsStackSize();
  afx_msg void OnOptionsMakeDerivationTree();
  afx_msg void OnHelpAboutParserDemo();
  afx_msg void OnCheckBreakOnProduction();
  afx_msg void OnCheckBreakOnState();
  afx_msg void OnCheckBreakOnSymbol();
  afx_msg void OnCheckBreakOnError();
  afx_msg void OnSetFocusEditInputString();
  afx_msg void OnKillFocusEditInputString();
  afx_msg void OnChangeEditInputString();
  afx_msg void OnMaxTextEditInputString();
  afx_msg void OnSetFocusListErrors();
  afx_msg void OnKillFocusListErrors();
  afx_msg void OnSelChangeListErrors();
  afx_msg void OnSetFocusListDebug();
  afx_msg void OnKillFocusListDebug();
  afx_msg void OnSelChangeListDebug();
  afx_msg LRESULT OnMsgAutoStepDebugger(WPARAM wp, LPARAM lp);
  DECLARE_MESSAGE_MAP()
};
