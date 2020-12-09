#include "stdafx.h"
#include <MyUtil.h>
#include "ErrorsDlg.h"
#include "SelectBreakProductionsDlg.h"
#include "SelectBreakStatesDlg.h"
#include "SelectBreakSymbolsDlg.h"
#include "TreeDlg.h"
#include "StackSizeDlg.h"
#include "ShowStateDlg.h"
#include "GotoDlg.h"
#include "ShowStateThread.h"
#include "ParserDemoDlg.h"

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

CParserDemoDlg::CParserDemoDlg(CWnd *pParent)
 : CDialog(IDD, pParent)
 , m_textBox(        m_input)
 , m_showStateThread(nullptr)
 , m_shuttingDown(   false  )
{
  m_input             = EMPTYSTRING;
  m_breakOnProduction = FALSE;
  m_breakOnError      = FALSE;
  m_breakOnState      = FALSE;
  m_breakOnSymbol     = FALSE;

  m_hIcon            = theApp.LoadIcon(IDR_MAINFRAME);
  m_breakProductions = new BitSet(m_parser.getParserTables().getProductionCount()); TRACE_NEW(m_breakProductions);
  m_breakStates      = new BitSet(m_parser.getParserTables().getStateCount());      TRACE_NEW(m_breakStates     );
  m_breakSymbols     = new BitSet(m_parser.getParserTables().getTermCount());       TRACE_NEW(m_breakSymbols    );
}

CParserDemoDlg::~CParserDemoDlg() {
  SAFEDELETE(m_breakProductions);
  SAFEDELETE(m_breakStates     );
  SAFEDELETE(m_breakSymbols    );
}

BEGIN_MESSAGE_MAP(CParserDemoDlg, CDialog)
  ON_WM_SYSCOMMAND()
  ON_WM_QUERYDRAGICON()
  ON_WM_CREATE()
  ON_WM_PAINT()
  ON_WM_CLOSE()
  ON_WM_LBUTTONDBLCLK()
  ON_WM_SIZE()
  ON_COMMAND(      IDOK                              , OnOk                            )
  ON_COMMAND(      ID_FILE_OPEN                      , OnFileOpen                      )
  ON_COMMAND(      ID_FILE_DUMPACTIONMATRIX          , OnFileDumpActionMatrix          )
  ON_COMMAND(      ID_FILE_DUMPSUCCESORMATRIX        , OnFileDumpSuccesorMatrix        )
  ON_COMMAND(      ID_FILE_EXIT                      , OnFileExit                      )
  ON_COMMAND(      ID_EDIT_FIND                      , OnEditFind                      )
  ON_COMMAND(      ID_EDIT_FINDNEXT                  , OnEditFindNext                  )
  ON_COMMAND(      ID_EDIT_FINDPREV                  , OnEditFindPrev                  )
  ON_COMMAND(      ID_EDIT_NEXTERROR                 , OnEditNextError                 )
  ON_COMMAND(      ID_EDIT_PREVERROR                 , OnEditPrevError                 )
  ON_COMMAND(      ID_EDIT_FINDMATCHINGPARANTHES     , OnEditFindMatchingParanthes     )
  ON_COMMAND(      ID_EDIT_GOTO                      , OnEditGoto                      )
  ON_COMMAND(      ID_EDIT_BREAKONTEXTPOSITION       , OnEditBreakOnTextPosition       )
  ON_COMMAND(      ID_EDIT_SELECTPRODTOBREAKON       , OnEditSelectProductionsToBreakOn)
  ON_COMMAND(      ID_EDIT_SELECTSTATESTOBREAKON     , OnEditSelectStatesToBreakOn     )
  ON_COMMAND(      ID_EDIT_SELECTSYMBOLSTOBREAKON    , OnEditSelectSymbolsToBreakOn    )
  ON_COMMAND(      ID_EDIT_DERIVATIONTREE            , OnEditDerivationTree            )
  ON_COMMAND(      ID_RUN_PARSEINPUT                 , OnRunParseInput                 )
  ON_COMMAND(      ID_RUN_STARTDEBUG_RESETPARSER     , OnRunStartDebugResetParser      )
  ON_COMMAND(      ID_RUN_STARTDEBUG_GO              , OnRunStartDebugGo               )
  ON_COMMAND(      ID_RUN_STARTDEBUG_STEP            , OnRunStartDebugStep             )
  ON_COMMAND(      ID_RUN_STARTDEBUG_STEPOVER        , OnRunStartDebugStepOver         )
  ON_COMMAND(      ID_OPTIONS_SHOWLEGALINPUT         , OnOptionsShowLegalInput         )
  ON_COMMAND(      ID_OPTIONS_SHOWSTATE              , OnOptionsShowState              )
  ON_COMMAND(      ID_OPTIONS_ANIMATE                , OnOptionsAnimate                )
  ON_COMMAND(      ID_OPTIONS_LISTDEBUGFROMPARSER    , OnOptionsListDebugFromParser    )
  ON_COMMAND(      ID_OPTIONS_LISTDEBUGFROMSCANNER   , OnOptionsListDebugFromScanner   )
  ON_COMMAND(      ID_OPTIONS_ERRORS                 , OnOptionsErrors                 )
  ON_COMMAND(      ID_OPTIONS_STACKSIZE              , OnOptionsStackSize              )
  ON_COMMAND(      ID_OPTIONS_MAKEDERIVATIONTREE     , OnOptionsMakeDerivationTree     )
  ON_COMMAND(      ID_HELP_ABOUTPARSERDEMO           , OnHelpAboutParserDemo           )
  ON_BN_CLICKED(   IDC_CHECKBREAKONPRODUCTION        , OnCheckBreakOnProduction        )
  ON_BN_CLICKED(   IDC_CHECKBREAKONSTATE             , OnCheckBreakOnState             )
  ON_BN_CLICKED(   IDC_CHECKBREAKONSYMBOL            , OnCheckBreakOnSymbol            )
  ON_BN_CLICKED(   IDC_CHECKBREAKONERROR             , OnCheckBreakOnError             )
  ON_EN_SETFOCUS(  IDC_EDITINPUTSTRING               , OnSetFocusEditInputString       )
  ON_EN_KILLFOCUS( IDC_EDITINPUTSTRING               , OnKillFocusEditInputString      )
  ON_EN_CHANGE(    IDC_EDITINPUTSTRING               , OnChangeEditInputString         )
  ON_EN_MAXTEXT(   IDC_EDITINPUTSTRING               , OnMaxTextEditInputString        )
  ON_LBN_SETFOCUS( IDC_LISTERRORS                    , OnSetFocusListErrors            )
  ON_LBN_KILLFOCUS(IDC_LISTERRORS                    , OnKillFocusListErrors           )
  ON_LBN_SELCHANGE(IDC_LISTERRORS                    , OnSelChangeListErrors           )
  ON_LBN_SETFOCUS( IDC_LISTDEBUG                     , OnSetFocusListDebug             )
  ON_LBN_KILLFOCUS(IDC_LISTDEBUG                     , OnKillFocusListDebug            )
  ON_LBN_SELCHANGE(IDC_LISTDEBUG                     , OnSelChangeListDebug            )
  ON_MESSAGE(      ID_MSG_AUTOSTEP_DEBUGGER          , OnMsgAutoStepDebugger           )
END_MESSAGE_MAP()

static int findCharacterPosition(const TCHAR *s, const SourcePosition &pos) {
  return SourcePosition(pos.getLineNumber()-1,pos.getColumn()).findCharIndex(s);
}

static SourcePosition findSourcePosition(const TCHAR *s, int index) {
  const SourcePosition tmp(s, index);
  return SourcePosition(tmp.getLineNumber() + 1, tmp.getColumn());
}

void CParserDemoDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text( pDX, IDC_EDITINPUTSTRING       , m_input            );
  DDX_Check(pDX, IDC_CHECKBREAKONPRODUCTION, m_breakOnProduction);
  DDX_Check(pDX, IDC_CHECKBREAKONERROR     , m_breakOnError     );
  DDX_Check(pDX, IDC_CHECKBREAKONSTATE     , m_breakOnState     );
  DDX_Check(pDX, IDC_CHECKBREAKONSYMBOL    , m_breakOnSymbol    );
}

BOOL CParserDemoDlg::OnInitDialog() {
  __super::OnInitDialog();

  // IDM_ABOUTBOX must be in the system command range.
  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu *pSysMenu = GetSystemMenu(FALSE);
  if (pSysMenu != nullptr) {
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if (!strAboutMenu.IsEmpty()) {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  SetIcon(m_hIcon, TRUE);  // Set big icon
  SetIcon(m_hIcon, FALSE); // Set small icon

  const TabOrder tabOrder(this);

  CRect rect = getWindowRect(this, IDC_EDITINPUTSTRING);
  CEdit *e = (CEdit*)GetDlgItem(IDC_EDITINPUTSTRING);
  e->DestroyWindow();
  m_textBox.CreateEx(WS_VISIBLE | WS_GROUP | WS_TABSTOP | WS_VSCROLL| WS_HSCROLL | WS_BORDER
                   | ES_MULTILINE|ES_NOHIDESEL|ES_AUTOVSCROLL|ES_AUTOHSCROLL|ES_WANTRETURN
                    ,0
                    ,rect, this, IDC_EDITINPUTSTRING);
  tabOrder.restoreTabOrder();


  m_printFont.CreateFont(12, 10, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS
                        ,CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY
                        ,DEFAULT_PITCH | FF_MODERN
                        ,_T("Courier"));

  CMFCDynamicLayout *mfcLM = GetDynamicLayout();
  CMFCDynamicLayout::MoveSettings inputMv; inputMv.m_nXRatio = 10;
  CMFCDynamicLayout::SizeSettings inputSz; inputSz.m_nXRatio = 90; inputSz.m_nYRatio = 50;
  mfcLM->AddItem(m_textBox, inputMv, inputSz);

  m_accelTable       = LoadAccelerators(theApp.m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME));
  m_animateOn        = isMenuItemChecked(this, ID_OPTIONS_ANIMATE);
  GetDlgItem(IDC_EDITINPUTSTRING)->SetFont(&m_printFont);
  GetDlgItem(IDC_EDITINPUTSTRING)->SetFocus();

  m_stackImage.init((CStatic*)GetDlgItem(IDC_STATICSTACK), m_parser);
  m_debugger.setDialog(this);
  return false;
}

BOOL CParserDemoDlg::PreTranslateMessage(MSG *pMsg) {
  BOOL result = FALSE;
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    result = TRUE;
  }

  if(!result) {
    result = __super::PreTranslateMessage(pMsg);
  }
  if(getFocusCtrlId(this) == IDC_EDITINPUTSTRING) {
    const SourcePosition pos = getSourcePosition();
    if(pos != m_lastSourcePosition) {
      OnSelChangeEditInputString();
      m_lastSourcePosition = pos;
    }
  }
  return result;
}

void CParserDemoDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CParserDemoDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}


void CParserDemoDlg::OnLButtonDblClk(UINT nFlags, CPoint point) {
  const int indexFromTop = findStackIndexFromTop(point);
  if(indexFromTop >= 0) {
    TreeDlg(m_parser.getStackTop(indexFromTop)).DoModal();
  }
  __super::OnLButtonDblClk(nFlags, point);
}

void CParserDemoDlg::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  if(IsWindowVisible()) {
    setBreakProdText();
    setBreakSymbolText();
    Invalidate(false);
  }
}

void CParserDemoDlg::OnOk() {
}

void CParserDemoDlg::OnPaint() {
  if(IsIconic()) {
    CPaintDC dc(this); // device context for painting

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
    m_textBox.OnPaint();
  }
}

void CParserDemoDlg::OnFileOpen() {
  CFileDialog dlg(true);
  if((dlg.DoModal() != IDOK) || (_tcsclen(dlg.m_ofn.lpstrFile) == 0)) {
    return;
  }

  try {
    TCHAR *fname = dlg.m_ofn.lpstrFile;
    String str = readTextFile(fname).replace('\n',_T("\r\n"));
    m_input = str.cstr();
    UpdateData(FALSE);
    m_inputHasChanged = true;
    SetWindowText(fname);
    OnMaxTextEditInputString();
    resetListBoxes();
    m_textBox.markPos(nullptr);
  } catch(Exception e) {
    showException(e);
  }
}

void CParserDemoDlg::OnFileDumpActionMatrix() {
  try {
    const String dump       = m_parser.getActionMatrixDump();
    String objname          = _T("ActionTestResult");
    String filter           = format(_T("txt-files (*.txt)%c*.*%cAll files (*.*)%c*.*%c%c"),0,0,0,0,0);
    String defaultExtension = _T("txt");
    CFileDialog dlg(FALSE, defaultExtension.cstr(), objname.cstr());
    dlg.m_ofn.lpstrTitle    = _T("Dump Action Matrix Test result");
    dlg.m_ofn.lpstrFilter   = filter.cstr();

    if((dlg.DoModal() != IDOK) || (_tcslen(dlg.m_ofn.lpstrFile) == 0)) {
      return;
    }
    FILE *f = MKFOPEN(dlg.m_ofn.lpstrFile, _T("w"));
    fputws(dump.cstr(), f);
    fclose(f);
  } catch(Exception e) {
    showException(e);
  }
}

void CParserDemoDlg::OnFileDumpSuccesorMatrix() {
  try {
    const String dump       = m_parser.getSuccessorMatrixDump();
    String objname          = _T("SuccessorTestResult");
    String filter           = format(_T("txt-files (*.txt)%c*.*%cAll files (*.*)%c*.*%c%c"),0,0,0,0,0);
    String defaultExtension = _T("txt");
    CFileDialog dlg(FALSE, defaultExtension.cstr(), objname.cstr());
    dlg.m_ofn.lpstrTitle    = _T("Dump Successor Matrix Test result");
    dlg.m_ofn.lpstrFilter   = filter.cstr();

    if((dlg.DoModal() != IDOK) || (_tcslen(dlg.m_ofn.lpstrFile) == 0)) {
      return;
    }
    FILE *f = MKFOPEN(dlg.m_ofn.lpstrFile, _T("w"));
    fputws(dump.cstr(), f);
    fclose(f);
  } catch(Exception e) {
    showException(e);
  }
}

void CParserDemoDlg::OnFileExit() {
  m_shuttingDown = true;
  m_debugger.stop();
  EndDialog(IDOK);
}

void CParserDemoDlg::OnClose() {
  OnFileExit();
}

void CParserDemoDlg::OnEditFind() {
  m_findParam.m_findWhat = getCurrentWord();
  if(FindDlg(m_findParam, *this).DoModal() == IDOK) {
    find(m_findParam);
  }
}

void CParserDemoDlg::OnEditFindNext() {
  if(m_findParam.isSet()) {
    m_findParam.m_dirUp       = false;
    m_findParam.m_skipCurrent = true;
    find(m_findParam);
  }
}

void CParserDemoDlg::OnEditFindPrev() {
  if(m_findParam.isSet()) {
    m_findParam.m_dirUp       = true;
    m_findParam.m_skipCurrent = true;
    find(m_findParam);
  }
}

void CParserDemoDlg::OnEditNextError() {
  CListBox *lb = (CListBox*)GetDlgItem(IDC_LISTERRORS);
  const int index = lb->GetCurSel();
  if(index < lb->GetCount()-1) {
    lb->SetCurSel(index+1);
    OnSelChangeListErrors();
  }
}

void CParserDemoDlg::OnEditPrevError() {
  CListBox *lb = (CListBox*)GetDlgItem(IDC_LISTERRORS);
  const int index = lb->GetCurSel();
  if(index > 0) {
    lb->SetCurSel(index-1);
    OnSelChangeListErrors();
  }
}

void CParserDemoDlg::OnEditFindMatchingParanthes() {
  gotoMatchingParanthes(this, IDC_EDITINPUTSTRING);
}

void CParserDemoDlg::OnEditGoto() {
  GotoDlg dlg;
  if(dlg.DoModal() == IDOK) {
    gotoTextPosition(SourcePosition(dlg.getLine(), 0));
  }
}

void CParserDemoDlg::OnEditBreakOnTextPosition() {
  SourcePosition currentPos = getSourcePosition();
  if(m_textBox.isMarked() && (currentPos == m_textBox.getMarkedPos())) {
    m_textBox.markPos(nullptr);
    Invalidate(false);
  } else {
    m_textBox.markPos(&currentPos);
    Invalidate(false);
  }
}

void CParserDemoDlg::OnEditSelectProductionsToBreakOn() {
  CSelectBreakProductionsDlg dlg(m_parser.getParserTables(), *m_breakProductions);
  if(dlg.DoModal() == IDOK) {
    m_breakOnProduction = m_breakProductions->size() > 0;
    setBreakProdText();
    UpdateData(false);
    Invalidate(false);
  }
}

void CParserDemoDlg::OnEditSelectStatesToBreakOn() {
  CSelectBreakStatesDlg dlg(m_parser.getParserTables(), *m_breakStates);
  if(dlg.DoModal() == IDOK) {
    m_breakOnState = m_breakStates->size() > 0;
    setBreakStateText();
    UpdateData(false);
    Invalidate(false);
  }
}

void CParserDemoDlg::OnEditSelectSymbolsToBreakOn() {
  CSelectBreakSymbolsDlg dlg(m_parser.getParserTables(), *m_breakSymbols);
  if(dlg.DoModal() == IDOK) {
    m_breakOnSymbol = (m_breakSymbols->size() > 0) ? TRUE : FALSE;
    setBreakSymbolText();
    UpdateData(false);
    Invalidate(false);
  }
}

void CParserDemoDlg::OnEditDerivationTree() {
  SyntaxNodep root = m_parser.getRoot();
  if(root == nullptr) {
    showMessageBox(MB_ICONEXCLAMATION, _T("No root node"));
  } else {
    TreeDlg(root).DoModal();
  }
}

void CParserDemoDlg::OnRunParseInput() {
  bool saveAnimateFlag = m_animateOn;
  m_animateOn = false;
  debuggerRestart(0);
  m_animateOn = saveAnimateFlag;
}

void CParserDemoDlg::OnRunStartDebugResetParser() {
  debuggerRestart(FL_BREAKONRESET);
}

void CParserDemoDlg::OnRunStartDebugGo() {
  debuggerGo(m_animateOn ? FL_BREAKSTEP : 0, m_animateOn);
}

LRESULT CParserDemoDlg::OnMsgAutoStepDebugger(WPARAM wp, LPARAM lp) {
  m_debugger.step(FL_BREAKSTEP);
  return 0;
}

void CParserDemoDlg::OnRunStartDebugStep() {
  debuggerGo(FL_BREAKSTEP);
}

void CParserDemoDlg::OnRunStartDebugStepOver() {
  debuggerGo(FL_BREAKONSHIFT);
}

void CParserDemoDlg::debuggerGo(BYTE breakFlags, bool continueAfterBreak) {
  m_continueAfterBreak = continueAfterBreak;
  if(!m_debugger.hasJob()) {
    debuggerRestart(breakFlags);
  } else {
    m_debugger.step(breakFlags);
  }
}

void CParserDemoDlg::debuggerRestart(BYTE breakFlags) {
  m_debugger.stop();
  resetListBoxes();
  UpdateData();
  m_inputHasChanged = false;
  m_debugger.step(breakFlags);
}

void CParserDemoDlg::OnOptionsShowLegalInput() {
  toggleMenuItem(this, ID_OPTIONS_SHOWLEGALINPUT);
  showStatus(false);
}

void CParserDemoDlg::OnOptionsShowState() {
  if(m_showStateThread == nullptr) {
    CShowStateThread *thr = (CShowStateThread*)AfxBeginThread(RUNTIME_CLASS(CShowStateThread), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
    thr->m_maindialog = this;
    m_showStateThread = thr;
    m_showStateThread->ResumeThread();
  } else {
    m_showStateThread->PostThreadMessage(WM_QUIT, 0, 0);
  }
  toggleMenuItem(this, ID_OPTIONS_SHOWSTATE);
}

void CParserDemoDlg::OnOptionsAnimate() {
  if(toggleMenuItem(this, ID_OPTIONS_ANIMATE)) {
    showStatus(false);
  }
  m_animateOn = isMenuItemChecked(this, ID_OPTIONS_ANIMATE);
}

void CParserDemoDlg::OnOptionsListDebugFromParser() {
  m_parser.setDebug(toggleMenuItem(this, ID_OPTIONS_LISTDEBUGFROMPARSER));
}

void CParserDemoDlg::OnOptionsListDebugFromScanner() {
  m_parser.setDebugScanner(toggleMenuItem(this, ID_OPTIONS_LISTDEBUGFROMSCANNER));
}

void CParserDemoDlg::OnOptionsErrors() {
  ErrorsDlg dlg(m_parser.getMaxErrorCount(), m_parser.getCascadeCount());
  if(dlg.DoModal() == IDOK) {
    m_parser.setMaxErrorCount(dlg.getMaxErrorCount());
    m_parser.setCascadeCount(dlg.getCascadeCount());
  }
}

void CParserDemoDlg::OnOptionsStackSize() {
  StackSizeDlg dlg(m_parser.getStackSize(), this);
  if(dlg.DoModal() == IDOK && (int)dlg.m_stackSize != m_parser.getStackSize()) {
    m_parser.setStackSize(dlg.m_stackSize);
    OnRunStartDebugResetParser();
  }
}

void CParserDemoDlg::OnOptionsMakeDerivationTree() {
  toggleMenuItem(this, ID_OPTIONS_MAKEDERIVATIONTREE);
}

void CParserDemoDlg::OnHelpAboutParserDemo() {
  CAboutDlg().DoModal();
}

void CParserDemoDlg::OnCheckBreakOnProduction() {
  UpdateData();
  if(m_breakOnProduction && m_breakProductions->size() == 0) {
    OnEditSelectProductionsToBreakOn();
  }
  if(m_breakProductions->size() == 0) {
    m_breakOnProduction = false;
    CheckDlgButton(IDC_CHECKBREAKONPRODUCTION, BST_UNCHECKED);
  }
}

void CParserDemoDlg::OnCheckBreakOnState() {
  UpdateData();
  if(m_breakOnState && m_breakStates->size() == 0) {
    OnEditSelectStatesToBreakOn();
  }
  if(m_breakStates->size() == 0) {
    m_breakOnState = false;
    CheckDlgButton(IDC_CHECKBREAKONSTATE, BST_UNCHECKED);
  }
}

void CParserDemoDlg::OnCheckBreakOnSymbol() {
  UpdateData();
  if(m_breakOnSymbol && m_breakSymbols->isEmpty()) {
    OnEditSelectSymbolsToBreakOn();
  }
  if(m_breakSymbols->isEmpty()) {
    m_breakOnSymbol = false;
    CheckDlgButton(IDC_CHECKBREAKONSYMBOL, BST_UNCHECKED);
  }
}

void CParserDemoDlg::OnCheckBreakOnError() {
  UpdateData();
}

#define GOTFOCUSBORDER WS_DLGFRAME

void CParserDemoDlg::OnSetFocusEditInputString()  {
  OnSelChangeEditInputString();
  GetDlgItem(IDC_EDITINPUTSTRING)->ModifyStyle(0, GOTFOCUSBORDER);
  Invalidate(false);
}

void CParserDemoDlg::OnKillFocusEditInputString() {
  GetDlgItem(IDC_EDITINPUTSTRING)->ModifyStyle(GOTFOCUSBORDER, 0);
  Invalidate(false);
}

void CParserDemoDlg::OnChangeEditInputString() {
  m_inputHasChanged = true;
  UpdateData();
  OnMaxTextEditInputString();
}

void CParserDemoDlg::OnSelChangeEditInputString() {
  const SourcePosition pos = getSourcePosition();
  gotoTextPosition(IDC_LISTERRORS, m_errorPos, pos);
  gotoTextPosition(IDC_LISTDEBUG, m_debugPos, pos);
  updateSourcePosition(pos);
  m_textBox.OnPaint();
}

void CParserDemoDlg::OnMaxTextEditInputString() {
  int inputLength = m_input.GetLength();
  int maxTextSize = m_textBox.GetLimitText();
  if(inputLength > maxTextSize + 100) {
    maxTextSize = inputLength + 10000;
    m_textBox.SetLimitText(maxTextSize);
  }
}

void CParserDemoDlg::OnSetFocusListErrors() {
  OnSelChangeListErrors();
  GetDlgItem(IDC_LISTERRORS)->ModifyStyle(0, GOTFOCUSBORDER);
  Invalidate(false);
}

void CParserDemoDlg::OnKillFocusListErrors() {
  GetDlgItem(IDC_LISTERRORS)->ModifyStyle(GOTFOCUSBORDER, 0);
  Invalidate(false);
}

void CParserDemoDlg::OnSelChangeListErrors() {
  const int index = ((CListBox*)GetDlgItem(IDC_LISTERRORS))->GetCurSel();
  if(index < 0) {
    return;
  }
  const SourcePosition &pos = m_errorPos[index];
  gotoTextPosition(pos);
  gotoTextPosition(IDC_LISTDEBUG, m_debugPos, pos);
}

void CParserDemoDlg::OnSetFocusListDebug() {
  OnSelChangeListDebug();
  GetDlgItem(IDC_LISTDEBUG)->ModifyStyle(0, GOTFOCUSBORDER);
  Invalidate(false);
}

void CParserDemoDlg::OnKillFocusListDebug() {
  GetDlgItem(IDC_LISTDEBUG)->ModifyStyle(GOTFOCUSBORDER, 0);
  Invalidate(false);
}

void CParserDemoDlg::OnSelChangeListDebug() {
  const int index = ((CListBox*)GetDlgItem(IDC_LISTDEBUG))->GetCurSel();
  if(index < 0) {
    return;
  }
  const SourcePosition &pos = m_debugPos[index];
  gotoTextPosition(pos);
  gotoTextPosition(IDC_LISTERRORS, m_errorPos, pos);
}

int CParserDemoDlg::findStackIndexFromTop(const CPoint &p) const {
  const CRect r = getRelativeClientRect(this, IDC_STATICSTACK);
  if(!r.PtInRect(p)) {
    return -1;
  }
  CPoint tp = p;
  ClientToScreen(&tp);
  GetDlgItem(IDC_STATICSTACK)->ScreenToClient(&tp);
  return m_stackImage.findStackIndexFromTop(tp);
}

void CParserDemoDlg::showStatus(bool gotoLastDebug) {
  enableMenuItem(this, ID_EDIT_DERIVATIONTREE    , m_parser.getRoot() != nullptr);
  const bool enableNextPrevError = !m_errorPos.isEmpty();
  enableMenuItem(this, ID_EDIT_NEXTERROR         , enableNextPrevError);
  enableMenuItem(this, ID_EDIT_PREVERROR         , enableNextPrevError);
  
  if(!m_debugger.hasJob()) {
    setWindowText(this, IDC_LOOKAHEAD            , EMPTYSTRING        );
    setWindowText(this, IDC_ACTION               , EMPTYSTRING        );
  } else {
    setWindowText(this, IDC_STATE     , format(_T("%u"), m_parser.state()));
    setWindowText(this, IDC_CYCLECOUNT, format(_T("%u"), m_parser.getCycleCount()));

    if(m_parser.done()) {
      if(m_parser.accept()) {
        SetDlgItemText(IDC_ACTION, _T("Accept"));
      } else {
        SetDlgItemText(IDC_ACTION, _T("Dont accept"));
      }
      enableMenuItem(this, ID_RUN_STARTDEBUG_STEP    , false            );
      enableMenuItem(this, ID_RUN_STARTDEBUG_STEPOVER, false            );
      setWindowText( this, IDC_LOOKAHEAD             , EMPTYSTRING      );
    } else {
      const TCHAR *input = m_parser.getSymbolName(m_parser.input()).cstr();
      String LAtext;
      if(m_parser.getScanner()) {
        String lexeme = m_parser.getScanner()->getText();
        lexeme.replace('&', _T("&&"));
        const TCHAR *s = m_input.GetBuffer(m_input.GetLength());
        SourcePosition pos = m_parser.getScanner()->getPos();

        if(isMenuItemChecked(this, ID_OPTIONS_SHOWLEGALINPUT)) {
          LAtext = format(_T("%s \"%s\" (%d,%d) legal input:%s")
                          , input, lexeme.cstr(), pos.getLineNumber(), pos.getColumn(), m_parser.getLegalInput().cstr());
        } else {
          LAtext = format(_T("%s \"%s\" (%d,%d)"), input, lexeme.cstr(), pos.getLineNumber(), pos.getColumn());
        }

        int charIndex = findCharacterPosition(s, pos);
        ((CTextBox *)GetDlgItem(IDC_EDITINPUTSTRING))->SetSel(charIndex, charIndex + (int)m_parser.getScanner()->getLength());
      }
      enableMenuItem(this, ID_RUN_STARTDEBUG_STEP    , true                      );
      enableMenuItem(this, ID_RUN_STARTDEBUG_STEPOVER, true                      );
      setWindowText( this, IDC_LOOKAHEAD             , LAtext                    );
      setWindowText( this, IDC_ACTION                , m_parser.getActionString());
    }
    if(m_showStateThread != nullptr) {
      m_showStateThread->m_pMainWnd->SendMessage(ID_MSG_SHOWSTATE_UPDATE, 0, 0);
    }

    if(gotoLastDebug) {
      showLastDebugLine();
    }
    m_stackImage.updateImage();
  }
}

void CParserDemoDlg::showLastDebugLine() {
  CListBox *lb = (CListBox*)GetDlgItem(IDC_LISTDEBUG);
  lb->SetCurSel(lb->GetCount()-1);
}

void CParserDemoDlg::resetListBoxes() {
  ((CListBox*)GetDlgItem(IDC_LISTERRORS))->ResetContent();
  m_errorPos.clear();
  ((CListBox*)GetDlgItem(IDC_LISTDEBUG))->ResetContent();
  m_debugPos.clear();
}

void CParserDemoDlg::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  if(m_shuttingDown) return;
  switch(id) {
  case DEBUGGER_STATE :
    { const DebuggerState state = *(DebuggerState *)newValue;
      switch(state) {
      case DEBUGGER_CREATED   :
      case DEBUGGER_RUNNING   :
        setWindowText(this, IDC_THREADTIME, EMPTYSTRING);
        break;
      case DEBUGGER_TERMINATED:
        showStatus();
        break;
      case DEBUGGER_PAUSED    :
        setWindowText(this, IDC_THREADTIME, EMPTYSTRING);
        showStatus();
        if(m_continueAfterBreak) {
          PostMessage(ID_MSG_AUTOSTEP_DEBUGGER, 0, 0);
        }
        break;
      }
    }
    break;
  case DEBUGGER_RUNTIME:
    { const double t = (*(double *)newValue) / 1000;
      setWindowText(this, IDC_THREADTIME, format(_T("%.2lf"), t));
    }
    break;
  case PARSER_PROPERTY:
    { const ParserProperty &data = *(ParserProperty*)newValue;
      switch(data.m_id) {
      case PPROP_PARSERERROR    :
        handleError(data.m_textAndPos    );
        break;
      case PPROP_PARSERDEBUG    :
        handleDebug(data.m_textAndPos    );
        break;
      default                   :
        break;
      }
    }
    break;
  default:
    break;
  }
}

void CParserDemoDlg::handleError(const TextAndSourcePos &tp) {
  ((CListBox*)GetDlgItem(IDC_LISTERRORS))->AddString(tp.getText().cstr());
  if(m_errorPos.isEmpty()) {
    enableMenuItem(this, ID_EDIT_NEXTERROR, true);
    enableMenuItem(this, ID_EDIT_PREVERROR, true);
  }
  m_errorPos.add(tp.getPos());
  if(m_animateOn) {
    Invalidate(false);
  }
}

void CParserDemoDlg::handleDebug(const TextAndSourcePos &tp) {
  ((CListBox*)GetDlgItem(IDC_LISTDEBUG))->AddString(tp.getText().cstr());
  m_debugPos.add(tp.getPos());
  if(m_animateOn) {
    Invalidate(false);
    showLastDebugLine();
  }
}

void CParserDemoDlg::updateSourcePosition(const SourcePosition &pos) {
  setWindowText(this, IDC_SOURCEPOSITION, format(_T("Ln %d, Col %d"), pos.getLineNumber(), pos.getColumn()));
}

int CParserDemoDlg::getSourcePositionIndex() const {
  int startChar, endChar;
  ((CTextBox*)GetDlgItem(IDC_EDITINPUTSTRING))->GetSel(startChar, endChar);
  return startChar;
}

SourcePosition CParserDemoDlg::getSourcePosition() const {
  return findSourcePosition(getInputString().cstr(), getSourcePositionIndex());
}

int CParserDemoDlg::findSourcePositionIndex(const SourcePositionArray &list, const SourcePosition &pos) const {
  for(int i = (int)list.size(); i-- > 0;) {
    if(list[i] == pos)  {
      return i;
    }
  }
  return -1;
}

void CParserDemoDlg::gotoTextPosition(const SourcePosition &pos) {
  const int selstart = findCharacterPosition(m_input, pos);
  ((CTextBox*)GetDlgItem(IDC_EDITINPUTSTRING))->SetSel(selstart, selstart+1);
  updateSourcePosition(pos);
}

void CParserDemoDlg::gotoTextPosition(int id, const SourcePositionArray &list, const SourcePosition &pos) {
  const int index = findSourcePositionIndex(list, pos);
  if(index >= 0) {
    ((CListBox*)GetDlgItem(id))->SetCurSel(index);
  }
}

String CParserDemoDlg::getCurrentWord() const {
  return getWord(getInputString().cstr(), getSourcePositionIndex());
}

void CParserDemoDlg::markSourcePosition(const TextPositionPair &pos) {
  int start = findCharacterPosition(m_input, pos.m_pos1);
  int end   = findCharacterPosition(m_input, pos.m_pos2);
  ((CTextBox*)GetDlgItem(IDC_EDITINPUTSTRING))->SetSel(start, end);
  updateSourcePosition(pos.m_pos1);
}

bool CParserDemoDlg::makeDerivationTree() const {
  return isMenuItemChecked(this, ID_OPTIONS_MAKEDERIVATIONTREE);
}

void CParserDemoDlg::setBreakText(int controlId, const StringArray &textArray) {
  CWnd *ctrl = GetDlgItem(controlId);

  const String s = textArray.toString(_T(" "));
  ctrl->ModifyStyle(0, SS_ENDELLIPSIS);
  ctrl->SetWindowText(s.cstr());
}

void CParserDemoDlg::setBreakProdText() {
  StringArray textArray;
  for(auto it = m_breakProductions->getIterator(); it.hasNext();) {
    textArray.add(format(_T("%d"), (int)it.next()));
  }
  setBreakText(IDC_BREAKPRODTEXT, textArray);
}

void CParserDemoDlg::setBreakStateText() {
  StringArray textArray;
  for(auto it = m_breakStates->getIterator(); it.hasNext();) {
    textArray.add(format(_T("%d"), (int)it.next()));
  }
  setBreakText(IDC_BREAKSTATETEXT, textArray);
}

void CParserDemoDlg::setBreakSymbolText() {
  StringArray textArray;
  for(auto it = m_breakSymbols->getIterator(); it.hasNext();) {
    textArray.add(m_parser.getSymbolName((int)it.next()));
  }
  setBreakText(IDC_BREAKSYMBOLTEXT, textArray);
}

void CParserDemoDlg::find(const FindParameter &param) {
  const TextPositionPair newPos = searchText(param);
  if(newPos.m_pos1.isSet()) {
    markSourcePosition(newPos);
    enableMenuItem(this, ID_EDIT_FINDNEXT, true);
    enableMenuItem(this, ID_EDIT_FINDPREV, true);
  }
}

TextPositionPair CParserDemoDlg::searchText(const FindParameter &param) {
  const String inputString = getInputString();
  int c = getSourcePositionIndex();
  SearchMachine sm(param);
  if(param.m_dirUp) { // search backward
    if(param.m_skipCurrent) c--;
    c = sm.findIndex(inputString, c);
    if(c >= 0) {
      return TextPositionPair(findSourcePosition(inputString.cstr(), c), findSourcePosition(inputString.cstr(), c+sm.resultLen()));
    }
  } else {            // search forward
    if(param.m_skipCurrent) c++;
    c = sm.findIndex(inputString, c);
    if(c >= 0) {
      return TextPositionPair(findSourcePosition(inputString.cstr(), c), findSourcePosition(inputString.cstr(), c+sm.resultLen()));
    }
  }
  return TextPositionPair(); // not found
}
