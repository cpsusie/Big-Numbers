#include "stdafx.h"
#include "ParserDemoDlg.h"
#include "ErrorsDlg.h"
#include "SelectBreakProductionsDlg.h"
#include "SelectBreakStatesDlg.h"
#include "SelectBreakSymbolsDlg.h"
#include "TreeDlg.h"
#include "StackSizeDlg.h"
#include "ShowStateDlg.h"
#include "GotoDlg.h"
#include "ShowStateThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
public:
  CAboutDlg();

  enum { IDD = IDD_ABOUTBOX };

  protected:
  virtual void DoDataExchange(CDataExchange *pDX);

protected:
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CParserDemoDlg::CParserDemoDlg(CWnd *pParent) : CDialog(CParserDemoDlg::IDD, pParent), m_textBox(m_input) {
  m_input = EMPTYSTRING;
  m_breakOnProduction = FALSE;
  m_breakOnError      = FALSE;
  m_breakOnState      = FALSE;
  m_breakOnSymbol     = FALSE;

  m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
  m_parser.setHandler(this);
  m_breakProductions = new BitSet(m_parser.getParserTables().getProductionCount());
  m_breakStates      = new BitSet(m_parser.getParserTables().getStateCount());
  m_breakSymbols     = new BitSet(m_parser.getParserTables().getTerminalCount());
}

void CParserDemoDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text( pDX, IDC_EDITINPUTSTRING       , m_input);
  DDX_Check(pDX, IDC_CHECKBREAKONPRODUCTION, m_breakOnProduction);
  DDX_Check(pDX, IDC_CHECKBREAKONERROR     , m_breakOnError);
  DDX_Check(pDX, IDC_CHECKBREAKONSTATE     , m_breakOnState);
  DDX_Check(pDX, IDC_CHECKBREAKONSYMBOL    , m_breakOnSymbol);
}

BEGIN_MESSAGE_MAP(CParserDemoDlg, CDialog)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_WM_SIZE()
  ON_WM_LBUTTONDBLCLK()
  ON_WM_CREATE()
  ON_WM_CLOSE()
  ON_COMMAND(      IDOK                              , OnOk                            )
  ON_COMMAND(      ID_FILE_OPEN                      , OnFileOpen                      )
  ON_COMMAND(      ID_FILE_EXIT                      , OnFileExit                      )
  ON_COMMAND(      ID_EDIT_SELECTPRODUCTIONSTOBREAKON, OnEditSelectProductionsToBreakOn)
  ON_COMMAND(      ID_EDIT_SELECTSTATESTOBREAKON     , OnEditSelectStatesToBreakOn     )
  ON_COMMAND(      ID_EDIT_FIND                      , OnEditFind                      )
  ON_COMMAND(      ID_EDIT_FINDNEXT                  , OnEditFindNext                  )
  ON_COMMAND(      ID_EDIT_FINDPREV                  , OnEditFindPrev                  )
  ON_COMMAND(      ID_EDIT_NEXTERROR                 , OnEditNextError                 )
  ON_COMMAND(      ID_EDIT_PREVERROR                 , OnEditPrevError                 )
  ON_COMMAND(      ID_EDIT_BREAKONTEXTPOSITION       , OnEditBreakOnTextPosition       )
  ON_COMMAND(      ID_EDIT_SELECTSYMBOLSTOBREAKON    , OnEditSelectSymbolsToBreakOn    )
  ON_COMMAND(      ID_EDIT_FINDMATCHINGPARANTHES     , OnEditFindMatchingParanthes     )
  ON_COMMAND(      ID_EDIT_GOTO                      , OnEditGoto                      )
  ON_COMMAND(      ID_EDIT_DERIVATIONTREE            , OnEditDerivationTree            )
  ON_COMMAND(      ID_RUN_STARTDEBUG_RESETPARSER     , OnRunStartDebugResetParser      )
  ON_COMMAND(      ID_RUN_STARTDEBUG_GO              , OnRunStartDebugGo               )
  ON_COMMAND(      ID_RUN_STARTDEBUG_STEP            , OnRunStartDebugStep             )
  ON_COMMAND(      ID_RUN_STARTDEBUG_STEPOVER        , OnRunStartDebugStepOver         )
  ON_COMMAND(      ID_RUN_PARSEINPUT                 , OnRunParseInput                 )
  ON_COMMAND(      ID_OPTIONS_ERRORS                 , OnOptionsErrors                 )
  ON_COMMAND(      ID_OPTIONS_SHOWLEGALINPUT         , OnOptionsShowLegalInput         )
  ON_COMMAND(      ID_OPTIONS_LISTDEBUGFROMPARSER    , OnOptionsListDebugFromParser    )
  ON_COMMAND(      ID_OPTIONS_LISTDEBUGFROMSCANNER   , OnOptionsListDebugFromScanner   )
  ON_COMMAND(      ID_OPTIONS_ANIMATE                , OnOptionsAnimate                )
  ON_COMMAND(      ID_OPTIONS_STACKSIZE              , OnOptionsStackSize              )
  ON_COMMAND(      ID_OPTIONS_SHOWSTATE              , OnOptionsShowState              )
  ON_COMMAND(      ID_HELP_ABOUTPARSERDEMO           , OnHelpAboutParserDemo           )
  ON_COMMAND(      ID_CHECKBREAKONPRODUCTION         , OnCheckBreakOnProduction        )
  ON_COMMAND(      ID_CHECKBREAKONERROR              , OnCheckBreakOnError             )
  ON_COMMAND(      ID_GOTOINPUTCONTROL               , OnGotoInputControl              )
  ON_COMMAND(      ID_GOTODEBUGCONTROL               , OnGotoDebugControl              )
  ON_COMMAND(      ID_GOTOERRORSCONTROL              , OnGotoErrorsControl             )
  ON_EN_CHANGE(    IDC_EDITINPUTSTRING               , OnChangeEditInputString         )
  ON_EN_SETFOCUS(  IDC_EDITINPUTSTRING               , OnSetFocusEditInputString       )
  ON_EN_KILLFOCUS( IDC_EDITINPUTSTRING               , OnKillFocusEditInputString      )
  ON_EN_MAXTEXT(   IDC_EDITINPUTSTRING               , OnMaxTextEditInputString        )
  ON_LBN_SETFOCUS( IDC_LISTERRORS                    , OnSetFocusListErrors            )
  ON_LBN_SETFOCUS( IDC_LISTDEBUG                     , OnSetFocusListDebug             )
  ON_LBN_KILLFOCUS(IDC_LISTERRORS                    , OnKillFocusListErrors           )
  ON_LBN_KILLFOCUS(IDC_LISTDEBUG                     , OnKillFocusListDebug            )
  ON_LBN_SELCHANGE(IDC_LISTERRORS                    , OnSelChangeListErrors           )
  ON_LBN_SELCHANGE(IDC_LISTDEBUG                     , OnSelChangeListDebug            )
  ON_BN_CLICKED(   IDC_CHECKBREAKONSTATE             , OnCheckBreakOnState             )
  ON_BN_CLICKED(   IDC_CHECKBREAKONSYMBOL            , OnCheckBreakOnSymbol            )
  ON_BN_CLICKED(   IDC_CHECKBREAKONPRODUCTION        , OnCheckBreakOnProduction        )
  ON_BN_CLICKED(   IDC_CHECKBREAKONERROR             , OnCheckBreakOnError             )
END_MESSAGE_MAP()

static int findCharacterPosition(const TCHAR *s, const SourcePosition &pos) {
  int lineCount = 1;
  int col       = 0;
  int i;
  for(i = 0; *s; i++, s++) {
    if(lineCount == pos.getLineNumber() && col == pos.getColumn() ) {
      break;
    }
    if(*s == '\n') {
      lineCount++;
      col = 0;
    } else {
      col++;
    }
  }
  return i;
}

static SourcePosition findSourcePosition(const TCHAR *s, int index) {
  SourcePosition pos(1, 0);
  for(int i = 0; i < index && *s; i++, s++) {
    if(*s == '\n') {
      pos.incrLineNumber();
    } else {
      pos.incrColumn();
    }
  }
  return pos;
}

static bool isIdentifierChar(unsigned char ch) {
  return _istalpha(ch) || ch == '_';
}

class CharacterClass {
public:
  static unsigned char charClass[256];
  CharacterClass();
};

unsigned char CharacterClass::charClass[256];

CharacterClass::CharacterClass() {
  for(int i = 0; i < ARRAYSIZE(charClass); i++) {
    charClass[i] = 0;
  }

  for(int i = 0; i < ARRAYSIZE(charClass); i++) {
    if(isIdentifierChar(i)) {
      charClass[i] = 1;
    }
  }
  for(int i = 0; i < ARRAYSIZE(charClass); i++) {
    if(_istdigit(i)) {
      charClass[i] = 2;
    }
  }
  charClass['#'] = 3;
}

static CharacterClass dummy;

String getWord(const TCHAR *s, int pos) { 
  String tmp(s);
  int start = pos, end = pos;
  _TUCHAR ch = s[pos];
  _TUCHAR chClass = CharacterClass::charClass[ch];

  if(_istspace(ch)) {
    return EMPTYSTRING;
  } else {
    while((start > 0) && (CharacterClass::charClass[(unsigned char)(s[start-1])] == chClass)) {
      start--;
    }
    while(CharacterClass::charClass[(unsigned char)(s[end+1])] == chClass) {
      end++;
    }
  }

  return substr(tmp, start, end-start+1);
}

void CParserDemoDlg::handleError(const SourcePosition &pos, const TCHAR *form, va_list argptr) {
  const String tmp = format(_T("error in (%d,%d):%s"), pos.getLineNumber(), pos.getColumn(), vformat(form, argptr).cstr());
  ((CListBox*)GetDlgItem(IDC_LISTERRORS))->AddString(tmp.cstr());
  enableMenuItem(this, ID_EDIT_NEXTERROR, true);
  enableMenuItem(this, ID_EDIT_PREVERROR, true);
  m_errorPos.add(pos);
  if(m_animateOn)
    Invalidate(false);
}

void CParserDemoDlg::handleDebug(const SourcePosition &pos, const TCHAR *form, va_list argptr) {
  String line = format(_T("(%d,%d):%s"), pos.getLineNumber(), pos.getColumn(), vformat(form, argptr).cstr());
  line.replace('\n', _T("\\n")).replace('\r', _T("\\r")).replace('\t', _T("\\t"));
  ((CListBox*)GetDlgItem(IDC_LISTDEBUG))->AddString(line.cstr());
  m_debugPos.add(pos);
  if(m_animateOn) {
    Invalidate(false);
    showLastDebugLine();
  }
}

int CParserDemoDlg::handleReduction(unsigned int prod) {
  if(prod == 0) {
    enableMenuItem(this, ID_EDIT_DERIVATIONTREE, true);
  }
  return 0;
}

static void printf(CClientDC &dc, int x, int y, int width, const TCHAR *form, ...) {
  va_list argptr;
  va_start(argptr, form);
  dc.TextOut(x, y, format(_T("%-*.*s"), width, width, vformat(form, argptr).cstr()).cstr());
  va_end(argptr);
}

void CParserDemoDlg::message(const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  MessageBox(vformat(format, argptr).cstr());
  va_end(argptr);
}

int CParserDemoDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if(__super::OnCreate(lpCreateStruct) == -1) {
    return -1;
  }
    
  CRect rect;
  rect.left   = 255;
  rect.top    = 0;
  rect.right  = 1025;
  rect.bottom = 260;

  m_textBox.CreateEx(WS_VISIBLE | WS_GROUP | WS_TABSTOP | WS_VSCROLL| WS_HSCROLL | WS_BORDER
                   | ES_MULTILINE|ES_NOHIDESEL|ES_AUTOVSCROLL|ES_AUTOHSCROLL|ES_WANTRETURN
                    ,0
                    ,rect, this, IDC_EDITINPUTSTRING);
  return 0;
}
    
void CParserDemoDlg::OnMaxTextEditInputString() {
  int inputLength = m_input.GetLength();
  int maxTextSize = m_textBox.GetLimitText();
  if(inputLength > maxTextSize + 100) {
    maxTextSize = inputLength + 10000;
    m_textBox.SetLimitText(maxTextSize);
  }
}

BOOL CParserDemoDlg::OnInitDialog() {
  __super::OnInitDialog();

  // IDM_ABOUTBOX must be in the system command range.
  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu *pSysMenu = GetSystemMenu(FALSE);
  if (pSysMenu != NULL) {
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if (!strAboutMenu.IsEmpty()) {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  // Set the icon for this dialog.  The framework does this automatically
  //  when the application's main window is not a dialog

  SetIcon(m_hIcon, TRUE);  // Set big icon
  SetIcon(m_hIcon, FALSE); // Set small icon

  m_printFont.CreateFont(12, 10, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS
                        ,CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY
                        ,DEFAULT_PITCH | FF_MODERN
                        ,_T("Courier"));


  TEXTMETRIC tm;

  CDC *pDC = GetDC();
  pDC->SelectObject(m_printFont);
  pDC->GetTextMetrics(&tm);

  m_layoutManager.OnInitDialog(this);

  m_layoutManager.addControl(IDC_EDITINPUTSTRING       , RELATIVE_SIZE                  );
  m_layoutManager.addControl(IDC_SOURCEPOSITION        , RELATIVE_POSITION              );
  m_layoutManager.addControl(IDC_STATELABEL            , RELATIVE_Y_POS                 );
  m_layoutManager.addControl(IDC_STATE                 , RELATIVE_Y_POS                 );
  m_layoutManager.addControl(IDC_LOOKAHEADLABEL        , RELATIVE_Y_POS                 );
  m_layoutManager.addControl(IDC_LOOKAHEAD             , RELATIVE_Y_POS | RELATIVE_WIDTH);
  m_layoutManager.addControl(IDC_ACTIONLABEL           , RELATIVE_Y_POS                 );
  m_layoutManager.addControl(IDC_ACTION                , RELATIVE_Y_POS | RELATIVE_WIDTH);
  m_layoutManager.addControl(IDC_CHECKBREAKONPRODUCTION, RELATIVE_Y_POS                 );
  m_layoutManager.addControl(IDC_CHECKBREAKONSTATE     , RELATIVE_Y_POS                 );
  m_layoutManager.addControl(IDC_CHECKBREAKONSYMBOL    , RELATIVE_Y_POS                 );
  m_layoutManager.addControl(IDC_CHECKBREAKONERROR     , RELATIVE_Y_POS                 );
  m_layoutManager.addControl(IDC_BREAKPRODTEXT         , RELATIVE_Y_POS | RELATIVE_WIDTH);
  m_layoutManager.addControl(IDC_BREAKSTATETEXT        , RELATIVE_Y_POS | RELATIVE_WIDTH);
  m_layoutManager.addControl(IDC_BREAKSYMBOLTEXT       , RELATIVE_Y_POS | RELATIVE_WIDTH);
  m_layoutManager.addControl(IDC_LISTERRORS            , RELATIVE_Y_POS | RELATIVE_WIDTH);
  m_layoutManager.addControl(IDC_LISTDEBUG             , RELATIVE_Y_POS | RELATIVE_WIDTH);
  m_layoutManager.addControl(IDC_ERRORSLABEL           , RELATIVE_Y_POS                 );
  m_layoutManager.addControl(IDC_DEBUGLABEL            , RELATIVE_Y_POS                 );
  m_layoutManager.addControl(IDC_STATICSTACK           , RELATIVE_HEIGHT                );

  m_charSize.cx = tm.tmMaxCharWidth;
  m_charSize.cy = tm.tmHeight;

  m_showStateThread  = NULL;

  m_accelTable       = LoadAccelerators(theApp.m_hInstance, MAKEINTRESOURCE(IDR_ACCELERATORMAIN));
  m_animateOn        = isMenuItemChecked(this, ID_OPTIONS_ANIMATE);
  GetDlgItem(IDC_EDITINPUTSTRING)->SetFont(&m_printFont);
  GetDlgItem(IDC_EDITINPUTSTRING)->SetFocus();
  return false;
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
  }
  else {
    WINDOWPLACEMENT rect;
#define CHARHEIGHT   m_charSize.cy
#define CHARWIDTH    m_charSize.cx
#define POSX        (rect.rcNormalPosition.left+3)
#define POSY(y)     (rect.rcNormalPosition.top+((y)+1)*CHARHEIGHT)
#define RECTPWIDTH  (rect.rcNormalPosition.right-rect.rcNormalPosition.left-3)
#define RECTWIDTH   (RECTPWIDTH/CHARWIDTH)
#define RECTHEIGHT  ((rect.rcNormalPosition.bottom-rect.rcNormalPosition.top)/CHARHEIGHT)

    CClientDC dc(this);
    dc.SelectObject(&m_printFont);

    GetDlgItem(IDC_STATICSTACK)->GetWindowPlacement(&rect);

    int maxElement = min((int)m_parser.getStackHeight(), RECTHEIGHT);
    int i;
    for(i = 0; i < maxElement; i++) {
      const ParserStackElement &elem   = m_parser.getStackElement(i);
      const TCHAR *symbolString = (i == 0) ? EMPTYSTRING : m_parser.getUserStack()[i]->getSymbol(); // (i == 0) ? _T(""= : m_parser.getSymbolName(elem.m_symbol);
      printf(dc, POSX, POSY(i), RECTWIDTH, _T("%3d (%2d,%2d) %s")
            ,elem.m_state
            ,elem.m_pos.getLineNumber(),elem.m_pos.getColumn()
            ,symbolString);
    }
    for(;i < RECTHEIGHT-1;i++) {
      printf(dc, POSX, POSY(i), RECTWIDTH, EMPTYSTRING);
    }

//    dc.SetBkColor(RGB(255, 255, 255));
    __super::OnPaint();
    m_textBox.OnPaint();
  }
}

int CParserDemoDlg::findStackElement(const CPoint &p) {
  CClientDC dc(this);
  TEXTMETRIC tm;
  dc.SelectObject(&m_printFont);
  dc.GetTextMetrics(&tm);
  WINDOWPLACEMENT wp;
  GetDlgItem(IDC_STATICSTACK)->GetWindowPlacement(&wp);
  CRect r;
  r.left   = wp.rcNormalPosition.left;
  r.right  = wp.rcNormalPosition.right;
  r.top    = wp.rcNormalPosition.top;
  r.bottom = wp.rcNormalPosition.bottom;
  if(!r.PtInRect(p)) {
    return -1;
  }
  return (p.y - r.top) / CHARHEIGHT - 1;
}

void CParserDemoDlg::showStatus(bool gotoLastDebug) {
  String state = format(_T("%d"), m_parser.state());
  SetDlgItemText(IDC_STATE, state.cstr());

  if(m_parser.done()) {
    if(m_parser.accept()) {
      SetDlgItemText(IDC_ACTION, _T("Accept"));
    } else {
      SetDlgItemText(IDC_ACTION, _T("Dont accept"));
    }
    enableMenuItem(this, ID_RUN_STARTDEBUG_STEP    , false);
    enableMenuItem(this, ID_RUN_STARTDEBUG_STEPOVER, false);
  } else {
    const TCHAR *input = m_parser.getSymbolName(m_parser.input());
    String LAtext;
    if(m_parser.getScanner()) {
      String lexeme = m_parser.getScanner()->getText();
      lexeme.replace('&', _T("&&"));
      const TCHAR *s = m_input.GetBuffer(m_input.GetLength());
      SourcePosition pos = m_parser.getScanner()->getPos();

      if(isMenuItemChecked(this, ID_OPTIONS_SHOWLEGALINPUT)) {
        LAtext = format(_T("%s \"%s\" (%d,%d) legal input:%s")
                       ,input, lexeme.cstr(), pos.getLineNumber(), pos.getColumn(), m_parser.getLegalInput().cstr());
      } else {
        LAtext = format(_T("%s \"%s\" (%d,%d)"), input, lexeme.cstr(), pos.getLineNumber(), pos.getColumn());
      }

      int charIndex = findCharacterPosition(s, pos);
      ((CTextBox*)GetDlgItem(IDC_EDITINPUTSTRING))->SetSel(charIndex, charIndex+(int)m_parser.getScanner()->getLength());
    }
    SetDlgItemText(IDC_LOOKAHEAD, LAtext.cstr());
    SetDlgItemText(IDC_ACTION, m_parser.getActionString().cstr());
  }
  if(m_showStateThread != NULL) {
    m_showStateThread->m_pMainWnd->PostMessage(ID_SHOWSTATE_UPDATE, 0, 0);
  }

  if(gotoLastDebug) {
    showLastDebugLine();
  }
  Invalidate(false);
}

void CParserDemoDlg::showLastDebugLine() {
  CListBox *lb = (CListBox*)GetDlgItem(IDC_LISTDEBUG);
  lb->SetCurSel(lb->GetCount()-1);
}

void CParserDemoDlg::OnFileOpen() {
  CFileDialog dlg(true);
  if(dlg.DoModal() != IDOK) {
    return;
  }
  if(_tcsclen(dlg.m_ofn.lpstrFile) == 0) {
    return;
  }

  try {
    TCHAR *fname = dlg.m_ofn.lpstrFile;
    struct _stat st = STAT(fname);
    int fileSize = st.st_size;
    char *buffer = new char[fileSize+1];
    memset(buffer, 0, fileSize);
    FILE *f = FOPEN(fname, _T("rb"));
    fread(buffer, 1, fileSize, f);
    fclose(f);
    buffer[fileSize] = '\0';
    m_input = buffer;
    UpdateData(FALSE);
    m_inputHasChanged = true;
    SetWindowText(fname);
    OnMaxTextEditInputString();
    resetListBoxes();
    m_textBox.markPos(NULL);
    delete[] buffer;
  } catch(Exception e) {
    message(_T("%s"), e.what());
  }
}

void CParserDemoDlg::OnFileExit() {
  PostMessage(WM_CLOSE);
}

void CParserDemoDlg::OnRunStartDebugStep() {
  if(m_inputHasChanged) {
    beginParse();
    return;
  }
  m_parser.parseStep();
  showStatus();
}

void CParserDemoDlg::OnRunStartDebugStepOver() {
  if(m_inputHasChanged) {
    beginParse();
  }
  const SourcePosition startPos = m_parser.getScanner()->getPos();
  do {
    m_parser.parseStep();
  } while(!m_parser.done() && m_parser.getScanner()->getPos() == startPos);
  showStatus();
}

void CParserDemoDlg::OnRunStartDebugResetParser() {
  beginParse();
}

void CParserDemoDlg::OnRunStartDebugGo() {
  if(m_parser.done() || m_inputHasChanged) {
    beginParse();
  }
  UINT errorCount = (UINT)m_errorPos.size();
  while(!m_parser.done()) {
    int action = m_parser.getNextAction();
    if(m_breakOnError && (m_errorPos.size() > errorCount || (action == _ParserError))) {
      break;
    }
    if(m_breakOnProduction && (action <= 0) && m_breakProductions->contains(-action)) {
      break;
    }
    if(m_breakOnState && m_breakStates->contains(m_parser.state())) {
      break;
    }
    if(m_breakOnSymbol && m_breakSymbols->contains(m_parser.input())) {
      break;
    }
    if(m_textBox.isMarked() && !m_breakPosDone) {
      if(m_parser.getScanner()->getPos() >= m_textBox.getMarkedPos()) {
        m_breakPosDone = true;
        break;
      }
    }
    m_parser.parseStep();
    if(m_animateOn) {
      showStatus();
    }
  }
  showStatus();
}

void CParserDemoDlg::OnRunParseInput() {
  bool saveAnimateFlag = m_animateOn;
  m_animateOn = false;

  beginParse();
  while(!m_parser.done())
    m_parser.parseStep();

  m_animateOn = saveAnimateFlag;

  showStatus();
}

void CParserDemoDlg::resetListBoxes() {
  ((CListBox*)GetDlgItem(IDC_LISTERRORS))->ResetContent();
  m_errorPos.clear();
  ((CListBox*)GetDlgItem(IDC_LISTDEBUG))->ResetContent();
  m_debugPos.clear();
}

void CParserDemoDlg::beginParse() {
  resetListBoxes();
  UpdateData();
  m_inputHasChanged = false;
  m_parser.setNewInput(getInputString().cstr());
  m_parser.parseBegin();
  m_breakPosDone    = false;
  enableMenuItem(this, ID_RUN_STARTDEBUG_STEP, true);
  enableMenuItem(this, ID_RUN_STARTDEBUG_STEPOVER, true);
  enableMenuItem(this, ID_EDIT_DERIVATIONTREE, false);
  enableMenuItem(this, ID_EDIT_NEXTERROR, false);
  enableMenuItem(this, ID_EDIT_PREVERROR, false);
  showStatus();

//  gettotallegalinput(m_parser);
}

void CParserDemoDlg::OnChangeEditInputString() {
  m_inputHasChanged = true;
  UpdateData();
  OnMaxTextEditInputString();
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

void CParserDemoDlg::updateSourcePosition(const SourcePosition &pos) {
  setWindowText(this, IDC_SOURCEPOSITION, format(_T("Ln %d, Col %d"), pos.getLineNumber(), pos.getColumn()));
}

int CParserDemoDlg::getSourcePositionIndex() {
  int startChar, endChar;
  ((CTextBox*)GetDlgItem(IDC_EDITINPUTSTRING))->GetSel(startChar, endChar);
  return startChar;
}

SourcePosition CParserDemoDlg::getSourcePosition() {
  return findSourcePosition(getInputString().cstr(), getSourcePositionIndex());
}

int CParserDemoDlg::findSourcePositionIndex(const Array<SourcePosition> &list, const SourcePosition &pos) {
  for(int i = (int)list.size() - 1; i >= 0; i--) {
    const SourcePosition &s = list[i];
    if(s.getLineNumber() == pos.getLineNumber() && s.getColumn() == pos.getColumn())  {
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

void CParserDemoDlg::gotoTextPosition(int id, const Array<SourcePosition> &list, const SourcePosition &pos) {
  int index = findSourcePositionIndex(list, pos);
  if(index >= 0) {
    ((CListBox*)GetDlgItem(id))->SetCurSel(index);
  }
}

String CParserDemoDlg::getCurrentWord() { 
  return getWord(getInputString().cstr(), getSourcePositionIndex());
}

void CParserDemoDlg::markSourcePosition(const TextPositionPair &pos) {
  int start = findCharacterPosition(m_input, pos.m_pos1);
  int end   = findCharacterPosition(m_input, pos.m_pos2);
  ((CTextBox*)GetDlgItem(IDC_EDITINPUTSTRING))->SetSel(start, end);
  updateSourcePosition(pos.m_pos1);
}

void CParserDemoDlg::OnSelChangeEditInputString() {
  const SourcePosition pos = getSourcePosition();
  gotoTextPosition(IDC_LISTERRORS, m_errorPos, pos);
  gotoTextPosition(IDC_LISTDEBUG, m_debugPos, pos);
  updateSourcePosition(pos);
  m_textBox.OnPaint();
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

void CParserDemoDlg::OnSelChangeListDebug() {
  const int index = ((CListBox*)GetDlgItem(IDC_LISTDEBUG))->GetCurSel();
  if(index < 0) {
    return;
  }
  const SourcePosition &pos = m_debugPos[index];
  gotoTextPosition(pos);
  gotoTextPosition(IDC_LISTERRORS, m_errorPos, pos);
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

void CParserDemoDlg::OnSetFocusListErrors() {
  OnSelChangeListErrors();
  GetDlgItem(IDC_LISTERRORS)->ModifyStyle(0, GOTFOCUSBORDER);
  Invalidate(false);
}

void CParserDemoDlg::OnKillFocusListErrors()      {
  GetDlgItem(IDC_LISTERRORS)->ModifyStyle(GOTFOCUSBORDER, 0);
  Invalidate(false);
}

void CParserDemoDlg::OnSetFocusListDebug()        {
  OnSelChangeListDebug();
  GetDlgItem(IDC_LISTDEBUG)->ModifyStyle(0, GOTFOCUSBORDER);
  Invalidate(false);
}

void CParserDemoDlg::OnKillFocusListDebug()       {
  GetDlgItem(IDC_LISTDEBUG)->ModifyStyle(GOTFOCUSBORDER, 0);
  Invalidate(false);
}

void CParserDemoDlg::OnGotoInputControl() {
  GetDlgItem(IDC_EDITINPUTSTRING)->SetFocus();
}

void CParserDemoDlg::OnGotoErrorsControl() {
  GetDlgItem(IDC_LISTERRORS)->SetFocus();   
}

void CParserDemoDlg::OnGotoDebugControl() {
  GetDlgItem(IDC_LISTDEBUG)->SetFocus();
}

void CParserDemoDlg::OnEditNextError() {
  CListBox *lb = (CListBox*)GetDlgItem(IDC_LISTERRORS);
  int index = lb->GetCurSel();
  if(index < lb->GetCount()-1) {
    lb->SetCurSel(index+1);
    OnSelChangeListErrors();
  }
}

void CParserDemoDlg::OnEditPrevError() {
  CListBox *lb = (CListBox*)GetDlgItem(IDC_LISTERRORS);
  int index = lb->GetCurSel();
  if(index > 0) {
    lb->SetCurSel(index-1);
    OnSelChangeListErrors();
  }
}

void CParserDemoDlg::OnOptionsShowLegalInput() {
  toggleMenuItem(this, ID_OPTIONS_SHOWLEGALINPUT);
  showStatus(false);
}

void CParserDemoDlg::OnOptionsAnimate() {
  if(toggleMenuItem(this, ID_OPTIONS_ANIMATE))
    showStatus(false);
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
    m_parser.setMaxErrorCount(dlg.m_maxErrorCount);
    m_parser.setCascadeCount(dlg.m_cascadeCount);
  }
}

void CParserDemoDlg::OnOptionsShowState() {
  if(m_showStateThread == NULL) {
    CShowStateThread *thr = (CShowStateThread*)AfxBeginThread(RUNTIME_CLASS(CShowStateThread), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
    thr->m_maindialog = this;
    m_showStateThread = thr;
    m_showStateThread->ResumeThread();
  } else {
    m_showStateThread->PostThreadMessage(WM_QUIT, 0, 0);
  }
  toggleMenuItem(this, ID_OPTIONS_SHOWSTATE);
}

void CParserDemoDlg::OnOptionsStackSize() {
  StackSizeDlg dlg(m_parser.getStackSize(), this);
  if(dlg.DoModal() == IDOK && (int)dlg.m_stackSize != m_parser.getStackSize()) {
    m_parser.setStackSize(dlg.m_stackSize);
    OnRunStartDebugResetParser();
  }
}

void CParserDemoDlg::OnHelpAboutParserDemo() {
  CAboutDlg dlg;
  dlg.DoModal();
}

void CParserDemoDlg::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);

  if(IsWindowVisible()) {
    setBreakProdText();
    setBreakSymbolText();
    Invalidate(false);
  }
}

void CParserDemoDlg::OnLButtonDblClk(UINT nFlags, CPoint point) {
  const int index = findStackElement(point);
  if(index > 0 && index < (int)m_parser.getStackHeight()) {
    TreeDlg dlg(m_parser.getStackTop(m_parser.getStackHeight() - index - 1));
    dlg.DoModal();
  }
  __super::OnLButtonDblClk(nFlags, point);
}

void CParserDemoDlg::OnEditFindMatchingParanthes() {
  gotoMatchingParanthes(this, IDC_EDITINPUTSTRING);
}

void CParserDemoDlg::setBreakText(int controlId, const StringArray &textArray) {
  CWnd *ctrl = GetDlgItem(controlId);

  const String s = textArray.toString(_T(" "));
  ctrl->ModifyStyle(0, SS_ENDELLIPSIS);
  ctrl->SetWindowText(s.cstr());
}

void CParserDemoDlg::setBreakProdText() {
  StringArray textArray;
  for(Iterator<size_t> it = m_breakProductions->getIterator(); it.hasNext();) {
    textArray.add(format(_T("%d"), (int)it.next()));
  }
  setBreakText(IDC_BREAKPRODTEXT, textArray);
}

void CParserDemoDlg::setBreakStateText() {
  StringArray textArray;
  for(Iterator<size_t> it = m_breakStates->getIterator(); it.hasNext();) {
    textArray.add(format(_T("%d"), (int)it.next()));
  }
  setBreakText(IDC_BREAKSTATETEXT, textArray);
}

void CParserDemoDlg::setBreakSymbolText() {
  StringArray textArray;
  for(Iterator<size_t> it = m_breakSymbols->getIterator(); it.hasNext();) {
    textArray.add(m_parser.getSymbolName((int)it.next()));
  }
  setBreakText(IDC_BREAKSYMBOLTEXT, textArray);
}

void CParserDemoDlg::OnCheckBreakOnError() {
  UpdateData();
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
  if(m_breakOnSymbol && m_breakSymbols->size() == 0) {
    OnEditSelectSymbolsToBreakOn();
  }
  if(m_breakSymbols->size() == 0) {
    m_breakOnSymbol = false;
    CheckDlgButton(IDC_CHECKBREAKONSYMBOL, BST_UNCHECKED);
  }
}

void CParserDemoDlg::OnEditBreakOnTextPosition() {
  SourcePosition currentPos = getSourcePosition();
  if(m_textBox.isMarked() && currentPos == m_textBox.getMarkedPos()) {
    m_textBox.markPos(NULL);
    Invalidate(false);
  } else {
    m_textBox.markPos(&currentPos);
    m_breakPosDone = false;
    Invalidate(false);
  }
}

void CParserDemoDlg::OnEditFind() {
  m_findParam.m_findWhat = getCurrentWord();
  FindDlg dlg(m_findParam, *this);
  if(dlg.DoModal() == IDOK) {
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

void CParserDemoDlg::OnEditGoto() {
  GotoDlg dlg;
  if(dlg.DoModal() == IDOK) {
    gotoTextPosition(SourcePosition(dlg.m_line, 0));
  }
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

void CParserDemoDlg::OnEditDerivationTree() {
  TreeDlg dlg(m_parser.getRoot());
  dlg.DoModal();
}
