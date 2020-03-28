#pragma once

#include <MFCUtil/LayoutManager.h>
#include <MFCUtil/StaticBottomAligned.h>
#include <MFCUtil/ComboBoxWithHistory.h>
#include <TinyBitSet.h>
#include "Debugger.h"
#include "CharacterMarker.h"
#include "DebugTextWindow.h"

typedef enum {
  COMPILE_POSMARK
 ,PATTERN_POSMARK
 ,SEARCH_POSMARK
 ,MATCH_STARTMARK
 ,MATCH_DMARK
 ,LASTACCEPT_MARK
} CharMarkType;

typedef enum {
  PROP_BLINKERSVISIBLE
} DialogProperty;

class CRegexDemoDlg : public CDialog, public PropertyChangeListener, public PropertyContainer {
private:
  HICON                          m_hIcon;
  HACCEL                         m_accelTable;
  CFont                          m_comboFont;
  CComboBoxWithHistory           m_patternCombo, m_targetCombo;
  CDebugTextWindow               m_codeWindow;
  CStaticBottomAligned           m_stackWindow;
  SimpleLayoutManager            m_layoutManager;
  CharacterMarkerArray           m_charMarkers;
  bool                           m_patternOk;
  bool                           m_patternDirty;
  bool                           m_targetDirty;
  bool                           m_timerIsRunning;
  // Change every 500msec by timer
  bool                           m_blinkersVisible;
  DebugRegex                     m_regex;
  Debugger                      *m_debugger;

  void clearResult() {
    showResult(EMPTYSTRING);
  }
  void startTimer();
  void stopTimer();
  void showResult(const String &result, const String &registerString = EMPTYSTRING);
  void clearRegisterWindow();
  void setRegisterWindowText(const String &str);
  void setRegisterWindowMode();
  void setRegisterWindowVisible(bool visible);
  void enableRegisterWindow(bool enable);
  void setGraphicsWindowVisible(bool visible);
  bool isGraphicsWindowVisible();
  void setCylceAndStackWindowTop(int top);
  void clearDebuggerState();
  CompileParameters getCompileParameters();
  void showCompilerState();
  void setPatternCompiledOk();
  void showCompilerError(const String &errorMsg);
  void showEmacsCompilerState();
  int  getDFAGraphicsMode();
  void setDFAGraphicsMode(int id);
  bool isGraphicsOn();
  void showDFACompilerState();
  void showSearchState();
  void showEmacsSearchState();
  void showDFASearchState();
  // assume thread exists and is stopped but not finished
  void showMatchState();
  // assume thread exists and is stopped but not finished
  void showEmacsMatchState();
  void showMatchStack(const _RegexMatchState &state);
  void showDFAMatchState();
  void showPatternFound();
  void showPatternNotFound();
  void markFoundPattern();
  void unmarkFoundPattern();
  void clearCyclesWindow();
  void showCyclesText(const String &text);
  void showCycleCount();
  void markCurrentChar(CharMarkType type, intptr_t index);
  void markMultiPatternChars(const BitSet &markSet);
  void unmarkAllCharacters(CharMarkType type);
  void unmarkAllCharacters();
  void unmarkAll();
  bool checkPattern();
  void ajourDialogItems();
  void enableDialogItems(BitSet16 flags);
  void clearCodeWindow();
  String getCompiledCodeText() const;
  void fillCodeWindow(const String &codeText);
  void paintRegex(bool msgPaint=false, bool animate=false);
  void unpaintRegex();
  bool isCodeTextDFATables() const;
  inline void setCurrentCodeLine(int line) {
    getCodeWindow()->markCurrentLine(line);
  }
  inline void unmarkCodeLine() {
    getCodeWindow()->unmarkCurrentLine();
  }
  inline void markLastAcceptLine(int line) {
    getCodeWindow()->markLastAcceptLine(line);
  }
  inline void unmarkLastAcceptLine() {
    getCodeWindow()->unmarkLastAcceptLine();
  }
  void startDebugger(RegexCommand command, bool singleStep=false);
  void startDebugCompile();
  void killDebugger();
  inline bool hasDebugger() const {
    return m_debugger != NULL;
  }
  inline bool searchForward() {
    return !isMenuItemChecked(this, ID_OPTIONS_SEARCHBACKWARDS);
  }
  inline bool isDebuggerState(DebuggerState state) const {
    return hasDebugger() && (m_debugger->getState() == state);
  }
  inline bool isDebuggerPaused() const {
    return isDebuggerState(DEBUGGER_PAUSED);
  }
  inline bool isDebuggerRunning() const {
    return isDebuggerState(DEBUGGER_RUNNING);
  }
  inline bool isDebuggerTerminated() const {
    return isDebuggerState(DEBUGGER_TERMINATED);
  }

  String getDebuggerPhaseName() const;
  inline RegexPhaseType getDebuggerPhase() const {
    return hasDebugger() ? m_debugger->getRegexPhase() : REGEX_UNDEFINED;
  }

  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);

  inline CComboBox *getPatternWindow() {
    return (CComboBox*)GetDlgItem(IDC_COMBOPATTERN);
  }

  inline CComboBox *getTargetWindow() {
    return (CComboBox*)GetDlgItem(IDC_COMBOTARGET);
  }

  inline CDebugTextWindow *getCodeWindow() {
    return &m_codeWindow;
  }

  inline CStatic *getRegisterWindow() {
    return (CStatic*)GetDlgItem(IDC_STATICREGISTERS);
  }
  inline CStatic *getGraphicsWindow() {
    return (CStatic*)GetDlgItem(IDC_STATICDFAGRAPHICSWINDOW);
  }
  inline CStatic *getCyclesWindow() {
    return (CStatic*)GetDlgItem(IDC_STATICCYCLES);
  }
  inline CStaticBottomAligned *getStackWindow() {
    return &m_stackWindow;
  }

public:
  CRegexDemoDlg(CWnd *pParent = NULL);
    enum { IDD = IDR_MAINFRAME };
    CString m_pattern;
    CString m_target;

    virtual BOOL PreTranslateMessage(MSG *pMsg);
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnPaint();
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnClose();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnFileExit();
    afx_msg void OnEditCopy();
    afx_msg void OnEditFind();
    afx_msg void OnEditMatch();
    afx_msg void OnEditCompilePattern();
    afx_msg void OnEditFindMatchingParentesis();
    afx_msg void OnDebugCompile();
    afx_msg void OnDebugFind();
    afx_msg void OnDebugMatch();
    afx_msg void OnDebugContinue();
    afx_msg void OnDebugStep();
    afx_msg void OnDebugToggleBreakPoint();
    afx_msg void OnOptionsIgnoreCase();
    afx_msg void OnOptionsSearchBackwards();
    afx_msg void OnOptionsDFARegex();
    afx_msg void OnOptionsDFAShowTables();
    afx_msg void OnOptionsDFANoGraphics();
    afx_msg void OnOptionsDFAPaintStates();
    afx_msg void OnOptionsDFAAnimateCreate();
    afx_msg void OnHelpAbout();
    afx_msg void OnHelpShowctrlid();
    afx_msg void OnGotoPattern();
    afx_msg void OnGotoText();
    afx_msg void OnGotoBytecode();
    afx_msg void OnEditChangeComboPattern();
    afx_msg void OnEditChangeComboTarget();
    afx_msg void OnSelChangeComboPattern();
    afx_msg void OnSelChangeComboTarget();
    afx_msg void OnSelChangeListByteCode();
    afx_msg LRESULT OnMsgDebuggerStateChanged(WPARAM wp, LPARAM lp);
    DECLARE_MESSAGE_MAP()
};
