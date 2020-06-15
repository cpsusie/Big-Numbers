#include "stdafx.h"
#include <ThreadPool.h>
#include <MFCUtil/Clipboard.h>
#include "RegexDemoDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
public:
  enum { IDD = IDD_ABOUTBOX };
  CAboutDlg() : CDialog(IDD) {
  }
protected:
  DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CRegexDemoDlg::CRegexDemoDlg(CWnd *pParent) : CDialogWithDynamicLayout(IDD, pParent) {
  m_pattern         = EMPTYSTRING;
  m_target          = EMPTYSTRING;
  m_hIcon           = theApp.LoadIcon(IDR_MAINFRAME);
  m_debugger        = NULL;
  m_timerIsRunning  = false;
  m_blinkersVisible = true;
}

void CRegexDemoDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_CBString(pDX, IDC_COMBOPATTERN, m_pattern);
  DDX_CBString(pDX, IDC_COMBOTARGET, m_target);
}

BEGIN_MESSAGE_MAP(CRegexDemoDlg, CDialog)
  ON_WM_SYSCOMMAND()
  ON_WM_QUERYDRAGICON()
  ON_WM_SIZE()
  ON_WM_PAINT()
  ON_WM_CLOSE()
  ON_WM_TIMER()
  ON_COMMAND(ID_FILE_EXIT                      , OnFileExit                   )
  ON_COMMAND(ID_EDIT_COPY                      , OnEditCopy                   )
  ON_COMMAND(ID_EDIT_FIND                      , OnEditFind                   )
  ON_COMMAND(ID_EDIT_MATCH                     , OnEditMatch                  )
  ON_COMMAND(ID_EDIT_COMPILEPATTERN            , OnEditCompilePattern         )
  ON_COMMAND(ID_EDIT_FINDMATCHINGPAR           , OnEditFindMatchingParentesis )
  ON_COMMAND(ID_DEBUG_COMPILE                  , OnDebugCompile               )
  ON_COMMAND(ID_DEBUG_FIND                     , OnDebugFind                  )
  ON_COMMAND(ID_DEBUG_MATCH                    , OnDebugMatch                 )
  ON_COMMAND(ID_DEBUG_CONTINUE                 , OnDebugContinue              )
  ON_COMMAND(ID_DEBUG_STEP                     , OnDebugStep                  )
  ON_COMMAND(ID_DEBUG_TOGGLEBREAKPOINT         , OnDebugToggleBreakPoint      )
  ON_COMMAND(ID_OPTIONS_IGNORECASE             , OnOptionsIgnoreCase          )
  ON_COMMAND(ID_OPTIONS_SEARCHBACKWARDS        , OnOptionsSearchBackwards     )
  ON_COMMAND(ID_OPTIONS_DFA_REGEX              , OnOptionsDFARegex            )
  ON_COMMAND(ID_OPTIONS_DFA_SHOWTABLES         , OnOptionsDFAShowTables       )
  ON_COMMAND(ID_OPTIONS_DFA_NO_GRAPHICS        , OnOptionsDFANoGraphics       )
  ON_COMMAND(ID_OPTIONS_DFA_PAINT_STATES       , OnOptionsDFAPaintStates      )
  ON_COMMAND(ID_OPTIONS_DFA_ANIMATE_CREATE     , OnOptionsDFAAnimateCreate    )
  ON_COMMAND(ID_HELP_ABOUT                     , OnHelpAbout                  )
  ON_COMMAND(ID_HELP_SHOWCTRLID                , OnHelpShowctrlid             )
  ON_COMMAND(ID_GOTO_PATTERN                   , OnGotoPattern                )
  ON_COMMAND(ID_GOTO_TEXT                      , OnGotoText                   )
  ON_COMMAND(ID_GOTO_BYTECODE                  , OnGotoBytecode               )
  ON_CBN_EDITCHANGE(IDC_COMBOPATTERN           , OnEditChangeComboPattern     )
  ON_CBN_EDITCHANGE(IDC_COMBOTARGET            , OnEditChangeComboTarget      )
  ON_CBN_SELCHANGE( IDC_COMBOPATTERN           , OnSelChangeComboPattern      )
  ON_CBN_SELCHANGE( IDC_COMBOTARGET            , OnSelChangeComboTarget       )
  ON_LBN_SELCHANGE( IDC_LISTBYTECODE           , OnSelChangeListByteCode      )
  ON_MESSAGE(       ID_MSG_DEBUGGERSTATECHANGED, OnMsgDebuggerStateChanged    )
END_MESSAGE_MAP()

void CRegexDemoDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

HCURSOR CRegexDemoDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

BOOL CRegexDemoDlg::OnInitDialog() {
  __super::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu *pSysMenu = GetSystemMenu(FALSE);
  if(pSysMenu != NULL) {
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if(!strAboutMenu.IsEmpty()) {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  SetIcon(m_hIcon, TRUE);         // Set big icon
  SetIcon(m_hIcon, FALSE);        // Set small icon

  const TabOrder tabOrder(this);
  m_codeWindow.substituteControl(   this, IDC_LISTBYTECODE);
  m_patternCombo.substituteControl( this, IDC_COMBOPATTERN      , _T("PatternHistory"));
  m_targetCombo.substituteControl(  this, IDC_COMBOTARGET       , _T("TargetHistory") );
  m_stackWindow.substituteControl(  this, IDC_STATICSTACK);

  tabOrder.restoreTabOrder();
  reloadLayoutResource();

  LOGFONT lf;
  GetFont()->GetLogFont(&lf);
  lf.lfHeight = (int)(1.5 * lf.lfHeight);
  m_comboFont.CreateFontIndirect(&lf);

  m_patternCombo.SetFont(&m_comboFont);
  m_targetCombo.SetFont(&m_comboFont);

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));

  m_charMarkers.add(new CharacterMarker(this, IDC_COMBOPATTERN,IDB_BITMAP_BLACK_DOWNARROW , true )); // COMPILE_POSMARK
  m_charMarkers.add(new CharacterMarker(this, IDC_COMBOPATTERN,IDB_BITMAP_YELLOW_DOWNARROW, true )); // PATTERN_POSMARK
  m_charMarkers.add(new CharacterMarker(this, IDC_COMBOTARGET, IDB_BITMAP_BLACK_UPARROW   , false)); // SEARCH_POSMARK
  m_charMarkers.add(new CharacterMarker(this, IDC_COMBOTARGET, IDB_BITMAP_PINK_UPARROW    , false)); // MATCH_STARTMARK
  m_charMarkers.add(new CharacterMarker(this, IDC_COMBOTARGET, IDB_BITMAP_YELLOW_DOWNARROW, true )); // MATCH_DMARK
  m_charMarkers.add(new CharacterMarker(this, IDC_COMBOTARGET, IDB_BITMAP_GREEN_UPARROW   , false)); // LASTACCEPT_MARK
  m_charMarkers.last()->setBlinking(true);

  addPropertyChangeListener(&m_charMarkers );
  addPropertyChangeListener(&m_regex       );
  addPropertyChangeListener(getCodeWindow());

  m_patternDirty = false;
  m_patternOk    = false;

  ajourDialogItems();
  startTimer();
  GetDlgItem(IDC_COMBOPATTERN)->SetFocus();
  return FALSE;
}

void CRegexDemoDlg::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  if(isGraphicsOn()) {
    PostMessage(WM_PAINT);
  }
}

void CRegexDemoDlg::OnPaint() {
  if(IsIconic()) {
    CPaintDC dc(this);

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // Draw the icon
    dc.DrawIcon(x, y, m_hIcon);
  } else {
    __super::OnPaint();
    paintRegex(true);
  }
}

BOOL CRegexDemoDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }
  BOOL ret = __super::PreTranslateMessage(pMsg);
  switch(getFocusCtrlId(this)) {
  case IDC_COMBOPATTERN:
  case IDC_COMBOTARGET :
    enableMenuItem(this, ID_EDIT_FINDMATCHINGPAR, true);
    break;
  default:
    enableMenuItem(this, ID_EDIT_FINDMATCHINGPAR, false);
    break;
  }
  return ret;
}

void CRegexDemoDlg::OnOK() {
}

void CRegexDemoDlg::OnCancel() {
}

void CRegexDemoDlg::OnClose() {
  OnFileExit();
}

void CRegexDemoDlg::OnTimer(UINT_PTR nIDEvent) {
  setProperty(PROP_BLINKERSVISIBLE, m_blinkersVisible, !m_blinkersVisible);
  __super::OnTimer(nIDEvent);
}

void CRegexDemoDlg::OnFileExit() {
  unmarkAll();
  stopTimer();
  killDebugger();
  m_charMarkers.clear();
  EndDialog(IDOK);
}

void CRegexDemoDlg::OnEditCopy() {
  String copyText;
  switch(getFocusCtrlId(this)) {
  case IDC_COMBOPATTERN: copyText = getWindowText(getPatternWindow()); break;
  case IDC_COMBOTARGET : copyText = getWindowText(getTargetWindow());  break;
  case IDC_LISTBYTECODE: copyText = getCodeWindow()->getText();        break;
  default              : return;
  }
  putClipboard(*this, copyText);
}

void CRegexDemoDlg::OnEditFind() {
  unmarkAll();
  if(checkPattern()) {
    startDebugger(searchForward() ? COMMAND_SEARCHFORWARD:COMMAND_SEARCHBACKWRD      );
  }
}

void CRegexDemoDlg::OnEditMatch()  {
  unmarkAll();
  if(checkPattern()) {
    startDebugger(COMMAND_MATCH);
  }
}

void CRegexDemoDlg::OnEditCompilePattern() {
  unmarkAll();
  killDebugger();
  try {
    m_regex.compilePattern(getCompileParameters());
    setPatternCompiledOk();
  } catch(Exception e) {
    showCompilerError(e.what());
  }
}

void CRegexDemoDlg::OnEditFindMatchingParentesis() {
  const int ctrlId = getFocusCtrlId(this);
  switch(ctrlId) {
  case IDC_COMBOPATTERN:
  case IDC_COMBOTARGET :
    { CComboBox *cb = (CComboBox*)GetDlgItem(ctrlId);
      const String text = getWindowText(this, ctrlId);
      int cursorPos = cb->GetEditSel() & 0xffff;
      const int m = findMatchingpParanthes(text.cstr(), cursorPos);
      if(m >= 0) {
        cb->SetEditSel(m, m);
      }
    }
    break;
  }
}

void CRegexDemoDlg::OnDebugCompile() {
  UpdateData();
  unmarkAll();
  startDebugCompile();
}

void CRegexDemoDlg::OnDebugFind()  {
  unmarkAll();
  if(checkPattern()) {
    startDebugger(searchForward() ? COMMAND_SEARCHFORWARD:COMMAND_SEARCHBACKWRD, true);
  }
}

void CRegexDemoDlg::OnDebugMatch() {
  unmarkAll();
  if(checkPattern()) {
    startDebugger(COMMAND_MATCH, true);
  }
}

void CRegexDemoDlg::OnDebugContinue() {
  if(isDebuggerPaused()) {
    m_debugger->go();
  }
}

void CRegexDemoDlg::OnDebugStep() {
  if(isDebuggerPaused()) {
    m_debugger->singleStep(FL_SINGLESTEP);
  }
}

void CRegexDemoDlg::OnDebugToggleBreakPoint() {
  CDebugTextWindow *cw = getCodeWindow();
  const int line = cw->getHighestBreakPointLine(cw->GetCurSel());

  if(cw->isBreakPointLine(line)) {
    cw->removeBreakPoint(line);
  } else {
    cw->addBreakPoint(line);
  }
}

void CRegexDemoDlg::OnOptionsIgnoreCase() {
  killDebugger();
  unmarkAll();
  toggleMenuItem(this, ID_OPTIONS_IGNORECASE);
  if(getCompileParameters() == m_regex.getLastCompiledPattern()) {
    fillCodeWindow(getCompiledCodeText());
  } else {
    m_patternDirty = true;
    clearCodeWindow();
  }
  ajourDialogItems();
}

void CRegexDemoDlg::OnOptionsSearchBackwards() {
  killDebugger();
  unmarkAll();
  toggleMenuItem(this, ID_OPTIONS_SEARCHBACKWARDS);
}

void CRegexDemoDlg::OnOptionsDFARegex() {
  killDebugger();
  unmarkAll();
  toggleMenuItem(this, ID_OPTIONS_DFA_REGEX);
  m_regex.setType(isMenuItemChecked(this, ID_OPTIONS_DFA_REGEX)?DFA_REGEX:EMACS_REGEX);
  if(getCompileParameters() == m_regex.getLastCompiledPattern()) {
    fillCodeWindow(getCompiledCodeText());
  } else {
    m_patternDirty = true;
    clearCodeWindow();
  }
  setWindowText(this, IDC_STATICSTACKLABEL
                    , (m_regex.getType()==DFA_REGEX)
                    ?_T("ParserStack")
                    :_T("Alternative stack"));

  ajourDialogItems();
  Invalidate();
}

void CRegexDemoDlg::OnOptionsDFAShowTables() {
  toggleMenuItem(this, ID_OPTIONS_DFA_SHOWTABLES);
  fillCodeWindow(getCompiledCodeText());
  ajourDialogItems();
}

void CRegexDemoDlg::OnOptionsDFANoGraphics() {
  setDFAGraphicsMode(ID_OPTIONS_DFA_NO_GRAPHICS);
}

void CRegexDemoDlg::OnOptionsDFAPaintStates() {
  setDFAGraphicsMode(ID_OPTIONS_DFA_PAINT_STATES);
}

void CRegexDemoDlg::OnOptionsDFAAnimateCreate() {
  setDFAGraphicsMode(ID_OPTIONS_DFA_ANIMATE_CREATE);
}

void CRegexDemoDlg::OnHelpAbout() {
  CAboutDlg().DoModal();
}

void CRegexDemoDlg::OnHelpShowctrlid() {
  showInformation(_T("Focus control has Id:%d"), getFocusCtrlId(this));
}

void CRegexDemoDlg::OnGotoPattern() {
  getPatternWindow()->SetFocus();
}

void CRegexDemoDlg::OnGotoText() {
  getTargetWindow()->SetFocus();
}

void CRegexDemoDlg::OnGotoBytecode() {
  getCodeWindow()->SetFocus();
}

void CRegexDemoDlg::OnEditChangeComboPattern() {
  if(!m_patternDirty) {
    m_patternDirty = true;
    unmarkAll();
    ajourDialogItems();
  }
}

void CRegexDemoDlg::OnSelChangeComboPattern() {
  OnEditChangeComboPattern();
}

void CRegexDemoDlg::OnEditChangeComboTarget() {
  if(!m_targetDirty) {
    m_targetDirty = true;
    unmarkAll();
    ajourDialogItems();
  }
}

void CRegexDemoDlg::OnSelChangeComboTarget() {
  OnEditChangeComboTarget();
}

void CRegexDemoDlg::OnSelChangeListByteCode() {
  ajourDialogItems();
}

LRESULT CRegexDemoDlg::OnMsgDebuggerStateChanged(WPARAM wp, LPARAM lp) {
  try {
    const DebuggerState oldState = (DebuggerState)wp;
    const DebuggerState newState = (DebuggerState)lp;

    if(newState == DEBUGGER_RUNNING) {
      unmarkCodeLine();
      unmarkAllCharacters(PATTERN_POSMARK);
    } else {
      if(m_regex.isCodeDirty()) {
        fillCodeWindow(getCompiledCodeText());
      }

      if(isDebuggerTerminated()) {
        clearDebuggerState();
      }
      switch(m_debugger->getRegexPhase()) {
      case REGEX_COMPILING    :
        showCompilerState();
        break;
      case REGEX_COMPILEDOK   :
        setPatternCompiledOk();
        break;
      case REGEX_COMPILEDFAILED:
        showCompilerError(m_debugger->getResultMsg());
        break;
      case REGEX_SEARCHING    :
        showSearchState();
        break;
      case REGEX_MATCHING     :
        showMatchState();
        break;
      case REGEX_PATTERNFOUND :
        showPatternFound();
        break;
      case REGEX_SEARCHFAILED :
      case REGEX_MATCHFAILED  :
        showPatternNotFound();
        break;
      case REGEX_UNDEFINED:
        showResult(m_debugger->getResultMsg());
        break; // maybe an exception, access-violation or somthing like that
      }
    }
    ajourDialogItems();
  } catch(Exception e) {
    showException(e);
  }
  return 0;
}

void CRegexDemoDlg::startTimer() {
  if(m_timerIsRunning) {
    stopTimer();
  }
  if(SetTimer(1,500,NULL)) {
    m_timerIsRunning = true;
  } else {
    showWarning(_T("Cannot install timer"));
  }
}

void CRegexDemoDlg::stopTimer() {
  if(m_timerIsRunning) {
    KillTimer(1);
    m_timerIsRunning = false;
  }
}

bool CRegexDemoDlg::checkPattern() {
  if(m_patternDirty || !m_patternOk) {
    OnEditCompilePattern();
  } else {
    UpdateData();
  }
  return m_patternOk;
}

String CRegexDemoDlg::getDebuggerPhaseName() const {
  return hasDebugger()
       ? m_debugger->getPhaseName()
       : _T("No thread");
}

void CRegexDemoDlg::unmarkAll() {
  clearCyclesWindow();
  unmarkAllCharacters();
  unmarkFoundPattern();
  unpaintRegex();
  clearResult();
}

void CRegexDemoDlg::markCurrentChar(CharMarkType type, intptr_t index) {
  if(index < 0) {
    unmarkAllCharacters(type);
  } else {
    m_charMarkers[type]->setMark(index);
  }
}

void CRegexDemoDlg::unmarkAllCharacters(CharMarkType type) {
  m_charMarkers[type]->unmarkAll();
}

void CRegexDemoDlg::unmarkAllCharacters() {
  for(size_t i = 0; i < m_charMarkers.size(); i++) {
    m_charMarkers[i]->unmarkAll();
  }
}

void CRegexDemoDlg::markMultiPatternChars(const BitSet &markSet) {
  CharacterMarker &marker = *m_charMarkers[PATTERN_POSMARK];
  marker.setMultiMarksAllowed(true);
  marker.setMarks(markSet);
}

typedef enum {
  WIN_REGISTERS
 ,WIN_STACK
 ,MENU_SEARCH
 ,MENU_DEBUG
 ,MENU_BREAKPOINTS
 ,MENU_SHOWDFATABLES
 ,MENU_DFAGRAPHICS
} DialogItemFlags;

void CRegexDemoDlg::ajourDialogItems() {
  BitSet16 flags;

  if(isDebuggerPaused()
    && ((m_debugger->getCommand() == COMMAND_COMPILE)
     || (!m_patternDirty && m_patternOk && !m_targetDirty))) {
    flags.add(MENU_DEBUG);
  }
  if(!m_patternDirty && m_patternOk) {
    flags.add(MENU_SEARCH);
  }
  if(m_regex.getType() == DFA_REGEX) {
    flags.add(MENU_SHOWDFATABLES);
    flags.add(MENU_DFAGRAPHICS);
  }
  if((getCodeWindow()->GetCurSel()>=0) && (!isMenuItemChecked(this, ID_OPTIONS_DFA_REGEX) || !isMenuItemChecked(this, ID_OPTIONS_DFA_SHOWTABLES))) {
    flags.add(MENU_BREAKPOINTS);
  }

  setWindowText(this, IDC_STATICSTATENAME, getDebuggerPhaseName());
  if(hasDebugger()) {
    if(isDebuggerRunning()) {
      return;
    }
    switch(getDebuggerPhase()) {
    case REGEX_COMPILING     :
      if(m_regex.getType() == EMACS_REGEX) {
        flags.add(WIN_REGISTERS);
      }
      flags.add(WIN_STACK);
      break;
    case REGEX_COMPILEDFAILED:
      flags.remove(MENU_DEBUG);
      break;
    case REGEX_SEARCHING     :
      break;
    case REGEX_MATCHING      :
      if(m_regex.getType()==EMACS_REGEX) {
        flags.add(WIN_REGISTERS);
        flags.add(WIN_STACK);
      }
      break;
    case REGEX_SEARCHFAILED  :
    case REGEX_MATCHFAILED   :
      flags.remove(MENU_DEBUG);
      break;
    case REGEX_PATTERNFOUND:
      // NB continue case
    case REGEX_COMPILEDOK  :
      if(m_regex.getType()==EMACS_REGEX) {
        flags.add(WIN_REGISTERS);
      }
      break;
    case REGEX_UNDEFINED     :
      flags.remove(MENU_DEBUG);
      break;
    default:
      showError(_T("%s:Unnknown threadState:%d"), __TFUNCTION__, getDebuggerPhase());
      break;
    }
  }
  enableDialogItems(flags);
  setRegisterWindowMode();
}

void CRegexDemoDlg::enableDialogItems(BitSet16 flags) {
  const bool enableRegisters  = flags.contains(WIN_REGISTERS);
  const bool enableStack      = flags.contains(WIN_STACK    );
  const bool enableSearch     = flags.contains(MENU_SEARCH  );
  const bool enableDebug      = flags.contains(MENU_DEBUG   );

  enableRegisterWindow(enableRegisters );
  GetDlgItem(IDC_STATICSTACKLABEL       )->EnableWindow(enableStack     );
  GetDlgItem(IDC_STATICSTACK            )->EnableWindow(enableStack     );

  enableMenuItem(this, ID_DEBUG_TOGGLEBREAKPOINT, flags.contains(MENU_BREAKPOINTS));

  enableMenuItem(           this, ID_EDIT_FIND               , enableSearch);
  enableMenuItem(           this, ID_EDIT_MATCH              , enableSearch);
  enableMenuItem(           this, ID_DEBUG_FIND              , enableSearch);
  enableMenuItem(           this, ID_DEBUG_MATCH             , enableSearch);

  enableMenuItem(           this, ID_DEBUG_CONTINUE          , enableDebug);
  enableMenuItem(           this, ID_DEBUG_STEP              , enableDebug);
  enableMenuItem(           this, ID_OPTIONS_DFA_SHOWTABLES  , flags.contains(MENU_SHOWDFATABLES));
  enableSubMenuContainingId(this, ID_OPTIONS_DFA_NO_GRAPHICS , flags.contains(MENU_DFAGRAPHICS  ));
}

void CRegexDemoDlg::setPatternCompiledOk() {
  const String codeText = getCompiledCodeText();

  if(codeText != getCodeWindow()->getText()) {
    fillCodeWindow(codeText);
  }
  m_patternDirty = false;
  m_patternOk    = true;
  m_patternCombo.updateList();
  unmarkAll();
  ajourDialogItems();
  clearResult();
  paintRegex();
  if(m_target.GetLength() == 0) {
    OnGotoText();
  } else {
    OnGotoBytecode();
  }
}

CompileParameters CRegexDemoDlg::getCompileParameters() {
  UpdateData();
  const String pattern    = m_pattern;
  const bool   ignoreCase = isMenuItemChecked(this, ID_OPTIONS_IGNORECASE);
  return CompileParameters(pattern, ignoreCase);
}

void CRegexDemoDlg::showCompilerError(const String &errorMsg) {
  String msg = errorMsg;
  int errorIndex = -1;
  if(_stscanf(msg.cstr(), _T("(%d):"), &errorIndex) == 1) {
    intptr_t colon = msg.find(':');
    msg = substr(msg, colon+1, errorMsg.length());
  }
  clearCodeWindow();
  showResult(msg);
  m_patternDirty = true;
  m_patternOk    = false;
  unmarkAll();
  ajourDialogItems();
  OnGotoPattern();
  if(errorIndex >= 0) {
    getPatternWindow()->SetEditSel(errorIndex, errorIndex);
  }
}

void CRegexDemoDlg::startDebugCompile() {
  try {
    killDebugger();
    const String pattern = m_pattern;
    m_debugger = new Debugger(m_regex, getCompileParameters(), getCodeWindow()->getBreakPoints()); TRACE_NEW(m_debugger);
    m_debugger->addPropertyChangeListener(this);
    ThreadPool::executeNoWait(*m_debugger);
    m_debugger->singleStep(FL_SINGLESTEP);
  } catch(Exception e) {
    showException(e);
  }
}

void CRegexDemoDlg::startDebugger(RegexCommand command, bool singleStep) {
  try {
    killDebugger();
    m_targetCombo.updateList();
    m_targetDirty = false;
    const String target = m_target;

    m_debugger = new Debugger(command, m_regex, target, getCodeWindow()->getBreakPoints()); TRACE_NEW(m_debugger);
    m_debugger->addPropertyChangeListener(this);
    ThreadPool::executeNoWait(*m_debugger);
    m_debugger->singleStep(singleStep ? FL_SINGLESTEP : 0);
  } catch(Exception e) {
    showException(e);
  }
}

void CRegexDemoDlg::killDebugger() {
  if(hasDebugger()) {
    m_debugger->removePropertyChangeListener(this);
    SAFEDELETE(m_debugger);
  }
}

void CRegexDemoDlg::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  switch(id) {
  case DEBUGGER_STATE:
    { const DebuggerState oldState = *(DebuggerState*)oldValue;
      const DebuggerState newState = *(DebuggerState*)newValue;
      SendMessage(ID_MSG_DEBUGGERSTATECHANGED, oldState, newState);
    }
    break;
  default:
    showError(_T("%s:Unknown property:%d"), __TFUNCTION__,id);
    break;
  }
}

void CRegexDemoDlg::clearDebuggerState() {
  unmarkCodeLine();
  setWindowText(this, IDC_STATICSTACK, EMPTYSTRING);
  unmarkAllCharacters(SEARCH_POSMARK );
  unmarkAllCharacters(MATCH_STARTMARK);
  unmarkAllCharacters(MATCH_DMARK    );
}

void CRegexDemoDlg::showCompilerState() {
  switch(m_regex.getType()) {
  case EMACS_REGEX:
    showEmacsCompilerState();
    break;
  case DFA_REGEX:
    showDFACompilerState();
    break;
  }
}

void CRegexDemoDlg::showEmacsCompilerState() {
  const _RegexCompilerState  &state = m_debugger->getEmacsCompilerState();
  if(state.m_codeText != getCodeWindow()->getText()) {
    fillCodeWindow(state.m_codeText);
    getCodeWindow()->scrollToBottom();
  }
  markCurrentChar(COMPILE_POSMARK, state.m_scannerIndex);

  String stateStr = format(_T("State:\n%s"), state.m_currentState.cstr());
  if(state.m_usedCharSets.length() > 0) {
    stateStr += _T("\n\nCharacterSets:\n") + state.m_usedCharSets;
  }
  if(state.m_fastMap.length() > 0) {
    stateStr += _T("\n\nFastMap:\n") + state.m_fastMap;
  }
  if(state.m_registerInfo.length() > 0) {
    stateStr += _T("\n\nRegisters:\n") + state.m_registerInfo;
  }
  setRegisterWindowText(stateStr.replace('\n',_T("\r\n")));

  String stackStr = state.m_compilerStack;
  m_stackWindow.SetWindowText(stackStr.replace('\n',_T("\r\n")).cstr());
}

bool CRegexDemoDlg::isGraphicsOn() {
  switch(getDFAGraphicsMode()) {
  case ID_OPTIONS_DFA_NO_GRAPHICS   :
    return false;
  case ID_OPTIONS_DFA_PAINT_STATES  :
  case ID_OPTIONS_DFA_ANIMATE_CREATE:
    return true;
  default:
    showError(_T("%s:Unknown DFAGraphicsmode:%d"), __TFUNCTION__,getDFAGraphicsMode());
    return false;
  }
}

void CRegexDemoDlg::showDFACompilerState() {
  const _DFARegexCompilerState &state = m_debugger->getDFACompilerState();
  if(state.m_codeText != getCodeWindow()->getText()) {
    fillCodeWindow(state.m_codeText);
    getCodeWindow()->scrollToBottom();
  }
  markCurrentChar(COMPILE_POSMARK, state.m_scannerIndex);
  String stackStr = state.m_compilerStack;
  m_stackWindow.SetWindowText(stackStr.replace('\n',_T("\r\n")).cstr());
  switch(getDFAGraphicsMode()) {
  case ID_OPTIONS_DFA_NO_GRAPHICS   :
    break;
  case ID_OPTIONS_DFA_PAINT_STATES  :
    paintRegex();
    break;
  case ID_OPTIONS_DFA_ANIMATE_CREATE:
    paintRegex(false, true);
    break;
  }
}

void CRegexDemoDlg::showSearchState() {
  switch(m_regex.getType()) {
  case EMACS_REGEX:
    showEmacsSearchState();
    break;
  case DFA_REGEX:
    showDFASearchState();
    break;
  }
  showCycleCount();
}

void CRegexDemoDlg::showEmacsSearchState() {
  const _RegexSearchState &state = m_debugger->getEmacsSearchState();
  unmarkAllCharacters(MATCH_STARTMARK);
  unmarkAllCharacters(MATCH_DMARK    );
  markCurrentChar(  SEARCH_POSMARK, state.m_charIndex);
}

void CRegexDemoDlg::showDFASearchState() {
  const _DFARegexSearchState &state = m_debugger->getDFASearchState();
  unmarkAllCharacters(MATCH_STARTMARK);
  unmarkAllCharacters(MATCH_DMARK    );
  markCurrentChar(  SEARCH_POSMARK, state.m_charIndex);
  paintRegex();
}

void CRegexDemoDlg::showMatchState() {
  switch(m_regex.getType()) {
  case EMACS_REGEX:
    showEmacsMatchState();
    break;
  case DFA_REGEX:
    showDFAMatchState();
    break;
  }
  showCycleCount();
}

void CRegexDemoDlg::showEmacsMatchState() {
  const _RegexMatchState &state = m_debugger->getEmacsMatchState();
  setCurrentCodeLine(state.getDBGLineNumber());
  markCurrentChar(PATTERN_POSMARK, state.getDBGPatternCharIndex());
  setRegisterWindowText(state.registersToString());
  showMatchStack(state);

  unmarkAllCharacters(SEARCH_POSMARK);
  markCurrentChar(    MATCH_STARTMARK, state.getPos());
  if(state.m_sp == NULL) {
    unmarkAllCharacters(MATCH_DMARK);
  } else {
    const intptr_t fromEnd   = state.m_spEnd - state.m_sp;
    const intptr_t fromStart = m_target.GetLength() - fromEnd;
    markCurrentChar(MATCH_DMARK, fromStart);
  }
}

void CRegexDemoDlg::showDFAMatchState() {
  const _DFARegexMatchState &state = m_debugger->getDFAMatchState();
  setCurrentCodeLine(state.getDBGLineNumber());
  markLastAcceptLine(state.getDBGLastAcceptLine());
  const BitSet *patternIndexSet = state.getDBGPatternIndexSet();
  if(patternIndexSet) {
    markMultiPatternChars(*patternIndexSet);
  } else {
    unmarkAllCharacters(PATTERN_POSMARK);
  }

  unmarkAllCharacters(SEARCH_POSMARK );
  markCurrentChar(MATCH_STARTMARK, state.getPos());
  markCurrentChar(MATCH_DMARK    , state.getDBGTextCharIndex());
  markCurrentChar(LASTACCEPT_MARK, state.getDBGLastAcceptIndex());
  paintRegex();
}

void CRegexDemoDlg::showPatternFound() {
  OnGotoText();
  showResult(m_debugger->getResultMsg(), m_debugger->registersToString());
  markFoundPattern();
  unmarkAllCharacters();
  showCycleCount();
  paintRegex();
  setCurrentCodeLine(m_regex.getPatternFoundCodeLine());
}

void CRegexDemoDlg::showPatternNotFound() {
  showResult(m_debugger->getResultMsg());
  unpaintRegex();
  showCycleCount();
}

void CRegexDemoDlg::markFoundPattern() {
  int start, end;
  m_debugger->getFoundPosition(start, end);
  getTargetWindow()->SetEditSel(start, end);
}

void CRegexDemoDlg::unmarkFoundPattern() {
  DWORD sel = getTargetWindow()->GetEditSel();
  getTargetWindow()->SetEditSel(HIWORD(sel),HIWORD(sel));
}

void CRegexDemoDlg::showCycleCount() {
  getCyclesWindow()->EnableWindow();
  showCyclesText(format(_T("Cycles:%d"), m_regex.getCycleCount()));
}

void CRegexDemoDlg::clearCyclesWindow() {
  getCyclesWindow()->EnableWindow(FALSE);
  showCyclesText(EMPTYSTRING);
}

void CRegexDemoDlg::showCyclesText(const String &text) {
  setWindowText(getCyclesWindow(), text);
}

void CRegexDemoDlg::showResult(const String &result, const String &registerString) {
  setWindowText(this, IDC_STATICRESULT, result);
  setRegisterWindowText(registerString);
}

void CRegexDemoDlg::clearRegisterWindow() {
  setRegisterWindowText(EMPTYSTRING);
}

void CRegexDemoDlg::setRegisterWindowText(const String &str) {
  setWindowText(getRegisterWindow(), str);
}

void CRegexDemoDlg::setRegisterWindowMode() {
  if(m_regex.getType() == EMACS_REGEX) {
    setRegisterWindowVisible(true);
    setGraphicsWindowVisible(false);
    setCylceAndStackWindowTop(getWindowRect(getRegisterWindow()).bottom, getItemLayout(IDC_STATICREGISTERS)->getBottomMoveRatio());
  } else {
    setRegisterWindowVisible(false);
    if(isGraphicsOn()) {
/*
    CRect rect;
    rect.left   = getWindowRect(this, IDC_STATICSTACKLABEL).left;
    rect.top    = getWindowRect(this, IDC_STATICRESULT).top;
    rect.right  = getWindowRect(this, IDC_STATICSTACK).right;
    rect.bottom = getClientRect(this).Height() - 200;
*/
      setGraphicsWindowVisible(true);
      setCylceAndStackWindowTop(getWindowRect(getGraphicsWindow()).bottom, getItemLayout(IDC_STATICDFAGRAPHICSWINDOW)->getBottomMoveRatio());
    }
  }
}

void CRegexDemoDlg::setRegisterWindowVisible(bool visible) {
  int mode = visible?SW_SHOW:SW_HIDE;
  GetDlgItem(IDC_STATICREGISTERSLABEL)->ShowWindow(mode);
  getRegisterWindow()->ShowWindow(mode);
}

void CRegexDemoDlg::enableRegisterWindow(bool enable) {
  GetDlgItem(IDC_STATICREGISTERSLABEL)->EnableWindow(enable);
  getRegisterWindow()->EnableWindow(enable);
}

void CRegexDemoDlg::setGraphicsWindowVisible(bool visible) {
  getGraphicsWindow()->ShowWindow(visible ? SW_SHOW : SW_HIDE);
}

bool CRegexDemoDlg::isGraphicsWindowVisible() {
  return getGraphicsWindow()->IsWindowVisible() ? true : false;
}

void CRegexDemoDlg::setCylceAndStackWindowTop(int top, int topMoveRatio) {
  CRect      cycleRect = getCtrlRect(IDC_STATICCYCLES    );
  CRect      labelRect = getCtrlRect(IDC_STATICSTACKLABEL);
  CRect      stackRect = getCtrlRect(IDC_STATICSTACK     );

  ItemLayout cycleItem = *getItemLayout(     IDC_STATICCYCLES    );
  ItemLayout labelItem = *getItemLayout(     IDC_STATICSTACKLABEL);
  ItemLayout stackItem = *getItemLayout(     IDC_STATICSTACK     );

  const int dt = top - cycleRect.top;

  cycleRect.top    += dt; cycleRect.bottom += dt; cycleItem.m_moveSettings.m_nYRatio = topMoveRatio;
  labelRect.top    += dt; labelRect.bottom += dt; labelItem.m_moveSettings.m_nYRatio = topMoveRatio;
  stackRect.top    += dt;                         stackItem.m_moveSettings.m_nYRatio = topMoveRatio;
                                                  stackItem.m_sizeSettings.m_nYRatio = 100 - topMoveRatio;
  setCtrlRect(  IDC_STATICCYCLES    , cycleRect, &cycleItem.getMoveSettings());
  setCtrlRect(  IDC_STATICSTACKLABEL, labelRect, &labelItem.getMoveSettings());
  setCtrlRect(  IDC_STATICSTACK     , stackRect, &stackItem.getMoveSettings(), &stackItem.getSizeSettings());
}

void CRegexDemoDlg::showMatchStack(const _RegexMatchState &state) {
  m_stackWindow.SetWindowText(state.stackToString().cstr());
  showCyclesText(format(_T("Cycles:%d %s"), m_regex.getCycleCount(), state.countersToString().cstr()));
}

void CRegexDemoDlg::clearCodeWindow() {
  getCodeWindow()->setText(EMPTYSTRING);
  getCodeWindow()->setAllowMarking(false);
  unmarkCodeLine();
  unmarkLastAcceptLine();
  setWindowText(this, IDC_STATICFASTMAP, EMPTYSTRING);
  unmarkAllCharacters();
}

String CRegexDemoDlg::getCompiledCodeText() const {
  if(isCodeTextDFATables()) {
    return m_regex.getDFATablesToString();
  } else {
    return m_regex.codeToString();
  }
}

void CRegexDemoDlg::fillCodeWindow(const String &codeText) {
  CDebugTextWindow *cw = getCodeWindow();

  cw->setText(codeText);

  if(m_regex.isCompiled() && !isCodeTextDFATables()) {
    const BitSet blSet = m_regex.getPossibleBreakPointLines();
    cw->setAllowMarking(true, &blSet);
  } else {
    cw->setAllowMarking(false);
  }

  String fastMapStr;
  if(m_regex.isCompiled()) {
    fastMapStr = m_regex.fastMapToString();
    if(m_regex.getMatchEmpty()) {
      fastMapStr += _T("\r\nMatch empty string");
    }
  }
  setWindowText(this, IDC_STATICFASTMAP, fastMapStr);
}

void CRegexDemoDlg::paintRegex(bool msgPaint, bool animate) {
  if(isGraphicsOn()) {
    CWnd *wnd = getGraphicsWindow();
    if(msgPaint) {
      m_regex.paint(wnd, CPaintDC(wnd), animate);
    } else {
      m_regex.paint(wnd, CClientDC(wnd), animate);
    }
  }
}

void CRegexDemoDlg::unpaintRegex() {
  if(isGraphicsOn()) {
    CWnd *wnd = getGraphicsWindow();
    m_regex.unmarkAll(wnd, CClientDC(wnd));
  }
  unmarkCodeLine();
  unmarkLastAcceptLine();
}

bool CRegexDemoDlg::isCodeTextDFATables() const {
  return isMenuItemChecked(this, ID_OPTIONS_DFA_REGEX) && isMenuItemChecked(this, ID_OPTIONS_DFA_SHOWTABLES) && m_regex.hasDFATables();
}

void CRegexDemoDlg::setDFAGraphicsMode(int id) {
  const bool wasGraphicsOn = isGraphicsOn();
  static const int menuIds[] = {
    ID_OPTIONS_DFA_NO_GRAPHICS
   ,ID_OPTIONS_DFA_PAINT_STATES
   ,ID_OPTIONS_DFA_ANIMATE_CREATE
  };
  for(int i = 0; i < ARRAYSIZE(menuIds); i++) {
    checkMenuItem(this, menuIds[i], menuIds[i] == id);
  }
  if(wasGraphicsOn) {
    unmarkAll();
  }
  ajourDialogItems();
  if(isGraphicsOn()) {
    if(m_regex.isCompiled()) {
      paintRegex();
    }
  }
}

int CRegexDemoDlg::getDFAGraphicsMode() {
  if(!isMenuItemChecked(this, ID_OPTIONS_DFA_REGEX)) {
    return ID_OPTIONS_DFA_NO_GRAPHICS;
  }
  if(isMenuItemChecked(this, ID_OPTIONS_DFA_PAINT_STATES)) {
    return ID_OPTIONS_DFA_PAINT_STATES;
  } else if(isMenuItemChecked(this, ID_OPTIONS_DFA_ANIMATE_CREATE)) {
    return ID_OPTIONS_DFA_ANIMATE_CREATE;
  } else {
    return ID_OPTIONS_DFA_NO_GRAPHICS;
  }
}
