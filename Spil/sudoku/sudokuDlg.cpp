#include "stdafx.h"
#include "resource.h"
#include <Random.h>
#include "Sudoku.h"
#include "SudokuDlg.h"
#include "SelectGameDlg.h"
#include "LevelDlg.h"

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


CSudokuDlg::CSudokuDlg(CWnd *pParent) : CDialog(CSudokuDlg::IDD, pParent) {
  m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
}

BEGIN_MESSAGE_MAP(CSudokuDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_QUERYDRAGICON()
    ON_WM_CREATE()
    ON_WM_PAINT()
    ON_WM_SIZE()
    ON_WM_CLOSE()
    ON_WM_TIMER()
    ON_COMMAND(ID_FILE_OPEN            , OnFileOpen           )
    ON_COMMAND(ID_FILE_NEWGAME         , OnFileNewGame        )
    ON_COMMAND(ID_FILE_SELECTGAME      , OnFileSelectGame     )
    ON_COMMAND(ID_FILE_EMPTYGAME       , OnFileEmptyGame      )
    ON_COMMAND(ID_FILE_SAVE            , OnFileSave           )
    ON_COMMAND(ID_FILE_SAVEAS          , OnFileSaveas         )
    ON_COMMAND(ID_FILE_EXIT            , OnFileExit           )
    ON_COMMAND(ID_GAME_CLEARUNMARKED   , OnGameClearUnmarked  )
    ON_COMMAND(ID_GAME_UNMARKALLFIELDS , OnGameUnmarkAllFields)
    ON_COMMAND(ID_GAME_ERRORS          , OnGameErrors         )
    ON_COMMAND(ID_GAME_LOCKFIELDS      , OnGameLockFields     )
    ON_COMMAND(ID_GAME_OPENALLFIELDS   , OnGameOpenAllFields  )
    ON_COMMAND(ID_GAME_SHOWTIME        , OnGameShowTime       )
    ON_COMMAND(ID_GAME_PAUSE           , OnGamePause          )
    ON_COMMAND(ID_GAME_RESETTIMER      , OnGameResetTimer     )
    ON_COMMAND(ID_GAME_LEVEL           , OnGameLevel          )
    ON_COMMAND(ID_HELP_ABOUTSUDOKU     , OnHelpAboutSudoku    )
    ON_BN_CLICKED(IDC_BUTTONCOUNT      , OnButtonCount        )
    ON_BN_CLICKED(IDSOLVE              , OnSolve              )
END_MESSAGE_MAP()


int CSudokuDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if(__super::OnCreate(lpCreateStruct) == -1) {
    return -1;
  }
  m_editFieldArray.Create(this);
  return 0;
}

BOOL CSudokuDlg::OnInitDialog() {
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
  SetIcon(m_hIcon, TRUE);
  SetIcon(m_hIcon, FALSE);

  m_accelTable     = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_ACCELERATOR1));
  m_level          = EASY;
  m_timerIsRunning = false;
  m_startPause     = 0;
  randomize();

  theApp.GetMainWnd()->SetWindowText(_T("Sudoku"));

  return false;
}

void CSudokuDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX)   {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

void CSudokuDlg::OnHelpAboutSudoku() {
  CAboutDlg dlg;
  dlg.DoModal();
}

void CSudokuDlg::OnPaint() {
  if (IsIconic()) {
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
    m_editFieldArray.OnPaint();
  }
}

void CSudokuDlg::putMatrix(const FieldMatrix &m) {
  m_editFieldArray.putMatrix(m);
}

FieldMatrix CSudokuDlg::getMatrix() {
  return m_editFieldArray.getMatrix();
}

void CSudokuDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);

}

void CSudokuDlg::showMessage(const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr,format);
  const String msg = vformat(format, argptr);
  va_end(argptr);
  setWindowText(this, IDC_STATICMESSAGE, msg);
}

void CSudokuDlg::showTime(UINT64 sec) {
  int h =  (int)(sec / 3600);
  int m = (int)((sec / 60) % 60);
  int s =  (int)(sec % 60);
  setWindowText(this, IDC_STATICTIME, format(_T("%02d:%02d:%02d"), h, m, s));
}


HCURSOR CSudokuDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CSudokuDlg::OnFileExit() {
  EndDialog(IDOK);
}

void CSudokuDlg::OnCancel() {
}

void CSudokuDlg::OnClose() {
  OnFileExit();
}

MarkableEditField *CSudokuDlg::findField(const CPoint &p) {
  return m_editFieldArray.findField(p);
}

BOOL CSudokuDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  switch(pMsg->message) {
  case WM_KEYDOWN:
    switch(pMsg->wParam) {
    case VK_ESCAPE:;
      ShowWindow(SW_MINIMIZE);
      break;
    }
    break;
  case WM_LBUTTONDBLCLK:
    { MarkableEditField *f = findField(pMsg->pt);
      if(f && !f->isFixed()) {
        UpdateData();
        f->setError(false);
        if(f->getValue() != 0) {
          f->setMarked(!f->isMarked());
        }
        Invalidate();
      }
    }
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

void CSudokuDlg::OnSolve() {
  showMessage(EMPTYSTRING);
  Game        game;
  FieldMatrix m = getMatrix();
  if(game.solve(m)) {
    const FieldMatrix solution = game.getSolution();
    putMatrix(solution);
    showMessage(_T("%d kombinationer gennemløbet"),game.getTryCount());
  } else {
    showInformation(_T("Kan ikke løses"));
  }
}

void CSudokuDlg::OnButtonCount() {
  FieldMatrix matrix = getMatrix();
  Game        game;
#define MAXSOLUTIONCOUNT 100
  game.solve(matrix,MAXSOLUTIONCOUNT);
  int solutionCount = game.getSolutionCount();
  switch(solutionCount) {
  case 0                : showMessage(_T("Ingen løsninger")                  ); break;
  case 1                : showMessage(_T("%d løsning"),1                     ); break;
  case MAXSOLUTIONCOUNT : showMessage(_T("mindst %d løsninger"),solutionCount); break;
  default               : showMessage(_T("%d løsninger"),solutionCount       ); break;
  }
}

void CSudokuDlg::OnGameClearUnmarked() {
  FieldMatrix m = getMatrix();

  for(int r = 0; r < 9; r++) {
    for(int c = 0; c < 9; c++) {
      const MarkableEditField &f = m_editFieldArray.getEditField(r,c);
      if(f.IsWindowEnabled()) {
        m.put(r,c,0);
      }
    }
  }
  putMatrix(m);
}

void CSudokuDlg::messageFreeFieldCount() {
  int count = 0;
  for(int r = 0; r < 9; r++) {
    for(int c = 0; c < 9; c++) {
      const MarkableEditField &f = m_editFieldArray.getEditField(r,c);
      if(!f.isFixed()) {
        count++;
      }
    }
  }
  showMessage(_T("%d frie felter"),count);
}

void CSudokuDlg::OnGameLockFields() {
  FieldMatrix m = getMatrix();;
  for(int r = 0; r < 9; r++) {
    for(int c = 0; c < 9; c++) {
      MarkableEditField &f = m_editFieldArray.getEditField(r,c);
      f.setMarked(false);
      f.setError(false);
      f.setFixed(m.get(r,c) != 0);
    }
  }
  Invalidate();
}

void CSudokuDlg::OnGameOpenAllFields() {
  for(int r = 0; r < 9; r++) {
    for(int c = 0; c < 9; c++) {
      MarkableEditField &f = m_editFieldArray.getEditField(r,c);
      f.setMarked(false);
      f.setError(false);
      f.setFixed(false);
    }
  }
  Invalidate();
}

void CSudokuDlg::setName(const CString &name) {
  SetWindowText(name);
}

CString CSudokuDlg::getName() {
  CString name;
  GetWindowText(name);
  return name;
}

void CSudokuDlg::newGame() {
  Game game;
  theApp.BeginWaitCursor();
  game.generate(m_level);
  theApp.EndWaitCursor();
  putMatrix(game.getMatrix());
  OnGameLockFields();
  setName(_T("Untitled"));
  messageFreeFieldCount();
  startTimer();
  Invalidate();
}

void CSudokuDlg::OnFileNewGame() {
  randomize();
  newGame();
}

void CSudokuDlg::OnFileSelectGame() {
  CSelectGameDlg dlg;
  if(dlg.DoModal() == IDOK) {
    unsigned int seed = dlg.m_seed;
    srand(seed);
    newGame();
  }
}

void CSudokuDlg::OnGameLevel() {
  CLevelDlg dlg(m_level);
  if(dlg.DoModal() == IDOK) {
    m_level = dlg.m_level;
  }
}

void CSudokuDlg::OnFileEmptyGame() {
  FieldMatrix m;
  putMatrix(m);
  OnGameOpenAllFields();
  showMessage(EMPTYSTRING);
  setName(_T("Untitled"));
}

void CSudokuDlg::OnGameUnmarkAllFields() {
  for(int r = 0; r < 9; r++) {
    for(int c = 0; c < 9; c++) {
      MarkableEditField &f = m_editFieldArray.getEditField(r,c);
      if(!f.isFixed()) {
        f.setMarked(false);
      }
    }
  }
  Invalidate();
}

bool CSudokuDlg::loadGame(FILE *f) {
  for(int r = 0; r < 9; r++) {
    for(int c = 0; c < 9; c++) {
      int a;
      if(_ftscanf(f,_T("%d"), &a) != 1) {
        return false;
      }
      MarkableEditField &f = m_editFieldArray.getEditField(r,c);
      f.setFixed( (a & 0x1) ? true : false);
      f.setMarked((a & 0x2) ? true : false);
    }
  }
  FieldMatrix m;
  for(int r = 0; r < 9; r++) {
    for(int c = 0; c < 9; c++) {
      int v;
      if(_ftscanf(f,_T("%d"),&v) != 1) {
        return false;
      }
      if(v >= 0 && v <= 9) {
        m.put(r,c,v);
      } else {
        return false;
      }
    }
  }
  messageFreeFieldCount();
  putMatrix(m);
  Invalidate();
  return true;
}

void CSudokuDlg::saveGame(FILE *f) {
  for(int r = 0; r < 9; r++) {
    for(int c = 0; c < 9; c++) {
      const MarkableEditField &mf = m_editFieldArray.getEditField(r,c);
      int a = 0;
      if(mf.isFixed())  a += 1;
      if(mf.isMarked()) a += 2;
      _ftprintf(f,_T("%d "),a);
    }
    _ftprintf(f,_T("\n"));
  }
  FieldMatrix m = getMatrix();
  for(int r = 0; r < 9; r++) {
    for(int c = 0; c < 9; c++) {
      _ftprintf(f,_T("%d "), m.get(r,c));
    }
    _ftprintf(f,_T("\n"));
  }
}

static const TCHAR *FileDialogExtensions = _T("Sudoku files (*.sud)\0*.sud\0"
                                              "Text files (*.txt)\0*.txt\0"
                                              "All files (*.*)\0*.*\0\0");

void CSudokuDlg::save(const TCHAR *fname) {
  try {
    FILE *f = FOPEN(fname,_T("w"));
    saveGame(f);
    setName(fname);
    fclose(f);
  } catch(Exception e) {
    showException(e);
  }
}

void CSudokuDlg::OnFileSave() {
  TCHAR fname[1000];
  CString name = getName();
  if(name == _T("Untitled")) {
    CFileDialog dlg(FALSE,_T("sud"),(LPCTSTR)name);
    dlg.m_ofn.lpstrFilter = FileDialogExtensions;
    if(dlg.DoModal() == IDOK) {
      _tcscpy(fname,dlg.m_ofn.lpstrFile);
    } else {
      return;
    }
  } else {
    _tcscpy(fname,(LPCTSTR)name);
  }

  save(fname);
}

void CSudokuDlg::OnFileSaveas() {
  TCHAR fname[1000];
  CString name = getName();
  CFileDialog dlg(FALSE,_T("sud"),(LPCTSTR)name);
  dlg.m_ofn.lpstrFilter = FileDialogExtensions;
  if(dlg.DoModal() == IDOK) {
    _tcscpy(fname,dlg.m_ofn.lpstrFile);
  } else {
    return;
  }
  save(fname);
}

void CSudokuDlg::OnFileOpen() {
  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrFilter = FileDialogExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Open files");

  if(dlg.DoModal() == IDOK) {
    try {
      CString t = dlg.m_ofn.lpstrFile;
      const TCHAR *fname = (LPCTSTR)t;
      FILE *f = fopen(fname, _T("r"));
      if(loadGame(f)) {
        setName(fname);
        startTimer();
      } else {
        showWarning(_T("Error loading %s"), fname);
      }
      fclose(f);
    } catch(Exception e) {
      showException(e);
    }
  }
}


void CSudokuDlg::OnGameErrors() {
  FieldMatrix       m         = getMatrix();
  const FieldMatrix userInput = m;
  for(int r = 0; r < 9; r++) {
    for(int c = 0; c < 9; c++) {
      const MarkableEditField &mf = m_editFieldArray.getEditField(r,c);
      if(!mf.isFixed()) {
        m.put(r,c,0);
      }
    }
  }
  Game game;
  if(!game.solve(m)) {
    showMessage(_T("Ingen løsninger"));
    return;
  } else {
    const FieldMatrix solution = game.getSolution();
    for(int r = 0; r < 9; r++) {
      for(int c = 0; c < 9; c++) {
        MarkableEditField &mf = m_editFieldArray.getEditField(r,c);
        if(!mf.isFixed() && (userInput.get(r,c) != 0) && userInput.get(r,c) != solution.get(r,c)) {
          mf.setError(true);
        } else {
          mf.setError(false);
        }
      }
    }
  }
  Invalidate();
}

void CSudokuDlg::startTimer() {
  if(!m_timerIsRunning && SetTimer(1,1000,NULL)) {
    m_timerIsRunning = true;
    time(&m_startTime);
  }
}

void CSudokuDlg::stopTimer() {
  if(m_timerIsRunning) {
    KillTimer(1);
    m_timerIsRunning = false;
  }
}

void CSudokuDlg::pauseTimer(bool paused) {
  if(paused) {
    time(&m_startPause);
  } else {
    time_t now;
    time(&now);
    m_startTime += now - m_startPause;
    m_startPause = 0;
  }
}

void CSudokuDlg::OnTimer(UINT_PTR nIDEvent) {
  time_t now;
  time(&now);
  bool checked = isGamePaused();
  if(checked) {
    showTime(m_startPause - m_startTime);
  } else {
    showTime(now - m_startTime);
  }

  __super::OnTimer(nIDEvent);
}

void CSudokuDlg::OnGameShowTime() {
  GetDlgItem(IDC_STATICTIME)->ShowWindow(toggleMenuItem(this,ID_GAME_SHOWTIME) ? SW_SHOW : SW_HIDE);
}

void CSudokuDlg::OnGamePause() {
  pauseTimer(toggleMenuItem(this,ID_GAME_PAUSE));
}

void CSudokuDlg::OnGameResetTimer() {
  time(&m_startTime);
  m_startPause = m_startTime;
}

void CSudokuDlg::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);

  if(cx == 0 && cy == 0) {
    OnGamePause();
  } else {
    if(isGamePaused()) {
      OnGamePause();
    }
  }
}
