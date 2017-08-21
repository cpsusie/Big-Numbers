#pragma once

#include <MFCUtil/LayoutManager.h>
#include <MFCUtil/StaticBottomAligned.h>
#include <MFCUtil/ComboBoxWithHistory.h>
#include <TinyBitSet.h>
#include "DebugThread.h"
#include "CharacterMarker.h"
#include "DebugTextWindow.h"

typedef enum {
  PATTERN_POSMARK
 ,SEARCH_POSMARK
 ,MATCH_STARTMARK
 ,MATCH_DMARK
 ,LASTACCEPT_MARK
} CharMarkType;

class CRegexDemoDlg : public CDialog, public PropertyChangeListener {
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
  DebugRegex                     m_regex;
  DebugThread                   *m_debugThread;

  void clearResult() {
    showResult(EMPTYSTRING);
  }
  void showResult(const String &result, const String &registerString = _T(""));
  void showRegisters(const RegexRegisters &registers);
  void clearRegisterWindow();
  void setRegisterWindowMode();
  void setCylceAndStackWindowTop(int top);
  void clearThreadState();
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
  void showMatchState();
  void showEmacsMatchState();
  void showMatchStack(const _RegexMatchState &state);
  void showDFAMatchState();
  void showPatternFound();
  void markFoundPattern();
  void unmarkFoundPattern();
  void clearCyclesWindow();
  void showCyclesText(const String &text);
  void showCycleCount();
  void markCurrentChar(CharMarkType type, intptr_t index);
  void markMultiPatternChars(const BitSet &markSet);
  void unmarkAllCharacters(CharMarkType type);
  void unmarkAllCharacters();
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
    setCurrentCodeLine(-1);
  }
  void startThread(ThreadCommand command, bool singleStep=false);
  void startDebugCompile();
  void killThread();
  bool hasThread() const {
    return m_debugThread != NULL;
  }
  bool searchForward() {
    return !isMenuItemChecked(this, ID_OPTIONS_SEARCHBACKWARDS);
  }
  bool isThreadStopped() const {
    return hasThread() && !m_debugThread->isRunning() && !m_debugThread->isFinished();
  }
  bool isThreadFinished() const {
    return hasThread() && !m_debugThread->isRunning() && m_debugThread->isFinished();
  }

  String getThreadStateName() const;
  RegexPhaseType getThreadState() const {
    return hasThread() ? m_debugThread->getRegexPhase() : REGEX_UNDEFINED;
  }

  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);

  CComboBox *getPatternWindow() {
    return (CComboBox*)GetDlgItem(IDC_COMBOPATTERN);
  }

  CComboBox *getTargetWindow() {
    return (CComboBox*)GetDlgItem(IDC_COMBOTARGET);
  }

  CDebugTextWindow *getCodeWindow() {
    return &m_codeWindow;
  }

  CStatic *getRegistersWindow() {
    return (CStatic*)GetDlgItem(IDC_STATICREGISTERS);
  }

  CStatic *getGraphicsWindow() {
    return (CStatic*)GetDlgItem(IDC_STATICDFAGRAPHICSWINDOW);
  }

  CStaticBottomAligned *getStackWindow() {
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
    virtual BOOL DestroyWindow();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnPaint();
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnClose();
    afx_msg void OnFileExit();
    afx_msg void OnEditCopy();
    afx_msg void OnEditFind();
    afx_msg void OnEditMatch();
    afx_msg void OnEditCompilePattern();
    afx_msg void OnEditFindMatchingParentesis();
    afx_msg void OnEditStandardTest();
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
    afx_msg void OnSelChangeListbyteCode();
    afx_msg LRESULT OnMsgThreadRunning(WPARAM wp, LPARAM lp);
    DECLARE_MESSAGE_MAP()
};

