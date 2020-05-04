#include "stdafx.h"
#include <MyUtil.h>
#include <Process.h>
#include <Random.h>
#include <ThreadPool.h>
#include "wordsepgraph.h"
#include "wordsepgraphDlg.h"
#include "ParamDlg.h"

#ifdef _DEBUG
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

CWordsepgraphDlg::CWordsepgraphDlg(CWnd *pParent) : CDialog(CWordsepgraphDlg::IDD, pParent) {
  m_ord            = EMPTYSTRING;
  m_details        = EMPTYSTRING;
  m_hIcon          = theApp.LoadIcon(IDR_MAINFRAME);
  m_trainerJob     = NULL;
  m_timerIsRunning = false;
}

void CWordsepgraphDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITORD, m_ord);
  DDX_Text(pDX, IDC_DETAILS, m_details);
}

BEGIN_MESSAGE_MAP(CWordsepgraphDlg, CDialog)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_WM_SIZE()
  ON_WM_TIMER()
  ON_WM_CLOSE()
  ON_COMMAND(ID_FILE_LOADNETWORK  , OnFileLoadNetwork  )
  ON_COMMAND(ID_FILE_SAVENETWORK  , OnFileSaveNetwork  )
  ON_COMMAND(ID_FILE_STARTTRAINING, OnFileStartTraining)
  ON_COMMAND(ID_FILE_STOPTRAINING , OnFileStopTraining )
  ON_COMMAND(ID_FILE_TRAININGDATA , OnFileTrainingdata )
  ON_COMMAND(ID_FILE_ERRORWORDS   , OnFileErrorwords   )
  ON_COMMAND(ID_FILE_QUIT         , OnFileQuit         )
  ON_COMMAND(ID_OPTIONS_TRAININGPARAMETERS, OnOptionsTrainingParameters)
  ON_BN_CLICKED(IDC_BUTTONSEPARATE, OnButtonSeparate   )
  ON_BN_CLICKED(IDC_BUTTONLEARN   , OnButtonLearn      )
  ON_COMMAND(ID_GOTO_EDITORD      , OnGotoEditord      )
  ON_EN_CHANGE(IDC_EDITORD        , OnChangeEditOrd    )
END_MESSAGE_MAP()

BOOL CWordsepgraphDlg::OnInitDialog() {
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

  SetIcon(m_hIcon, TRUE );
  SetIcon(m_hIcon, FALSE);

  m_printFont.CreateFont(10, 8, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                        DEFAULT_PITCH | FF_MODERN,
                        _T("Courier"));
//    randomize();
  m_threadPriority = THREAD_PRIORITY_NORMAL;
  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_ACCELERATOR));

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_DETAILS, RELATIVE_SIZE);
  m_layoutManager.addControl(IDC_MESSAGE, RELATIVE_Y_POS | RELATIVE_WIDTH);

  enableButtons(false);
  OnGotoEditord();
  return false;
}

void CWordsepgraphDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

void CWordsepgraphDlg::OnPaint() {
  if(IsIconic()) {
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    dc.DrawIcon(x, y, m_hIcon);
  } else {
    __super::OnPaint();
  }
}

HCURSOR CWordsepgraphDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CWordsepgraphDlg::OnFileQuit() {
  OnFileStopTraining();
  EndDialog(IDOK);
}

void CWordsepgraphDlg::OnClose() {
  OnFileQuit();
}

void CWordsepgraphDlg::OnCancel() {
}

void CWordsepgraphDlg::OnOK() {
}

void CWordsepgraphDlg::showResultDetail(const String &word) {
  String w = word;
  w.replace('-',EMPTYSTRING);
  CClientDC dc(this);
  dc.SelectObject(&m_printFont);
  dc.SetBkColor(RGB(192,192,192));
  WordWindowList wwl(w);
  m_details = EMPTYSTRING;
  for(size_t i = 0; i < wwl.size(); i++) {
    m_network.separateWordWindow(wwl[i]);
    const float result = m_network.getNetOutput();
    m_details += format(_T("%s : %.3f %s\r\n"),wwl[i].toString().cstr(),result,(result > 0.5)?_T("<-"):_T("  ")).cstr();
  }
  UpdateData(false);
}

void CWordsepgraphDlg::OnButtonSeparate() {
  try {
    UpdateData();
    String word = (LPCTSTR)m_ord;
    word.replace('-',EMPTYSTRING);
    const String result = m_network.separateWord(word);
//    debugLog(_T("\"%s\"->\"%s\"\n"), word.cstr(), result.cstr());
    setWindowText(this, IDC_STATICRESULT, result);
    showResultDetail(word);
  } catch(Exception e) {
    showException(e);
  }
}

void CWordsepgraphDlg::OnButtonLearn() {
  try {
    UpdateData();
    String word = (LPCTSTR)m_ord;
    m_network.learnWord(word);
    word.replace('-',EMPTYSTRING);
    word = m_network.separateWord(word);
    setWindowText(this, IDC_STATICRESULT, word);
    showResultDetail(word);
  } catch(Exception e) {
    showException(e);
  }
}

void CWordsepgraphDlg::OnFileLoadNetwork() {
  try {
    m_network.load();
  } catch(Exception e) {
    showException(e);
  }
}

void CWordsepgraphDlg::OnFileSaveNetwork() {
  try {
    m_network.save();
  } catch(Exception e) {
    showException(e);
  }
}

void CWordsepgraphDlg::OnOptionsTrainingParameters() {
  ParamDlgParameters params(m_network.getParameters(),m_threadPriority);
  ParamDlg           dlg(params);
  if(dlg.DoModal() == IDOK) {
    m_network.setParameters(params);
    setTrainingPriority(params.m_priority);
  }
}

void CWordsepgraphDlg::startTimer() {
  if(!m_timerIsRunning && SetTimer(1,1000,NULL)) {
    m_timerIsRunning = true;
  }
}

void CWordsepgraphDlg::stopTimer() {
  if(m_timerIsRunning) {
    KillTimer(1);
    m_timerIsRunning = false;
  }
}

void CWordsepgraphDlg::OnFileStartTraining() {
  m_trainerJob = new TrainerJob(m_network, m_threadPriority); TRACE_NEW(m_trainerJob);
  ThreadPool::executeNoWait(*m_trainerJob);
  startTimer();
  enableMenuItem(this,ID_FILE_STARTTRAINING,false);
  enableMenuItem(this,ID_FILE_STOPTRAINING ,true );
}

void CWordsepgraphDlg::stopTraining() {
  stopTimer();
  m_trainerJob->setInterrupted();
  m_trainerJob->waitUntilJobDone();
  m_network = m_trainerJob->getWordBpn();
  SAFEDELETE(m_trainerJob);
  enableMenuItem(this,ID_FILE_STARTTRAINING,true );
  enableMenuItem(this,ID_FILE_STOPTRAINING ,false);
}

void CWordsepgraphDlg::setTrainingPriority(int p) {
  m_threadPriority = p;
  if(m_trainerJob != NULL) {
    m_trainerJob->setPriority(p);
  }
}

void CWordsepgraphDlg::OnFileStopTraining() {
  if(m_trainerJob == NULL) {
    return;
  }
  stopTraining();
}

void CWordsepgraphDlg::showMessage(_In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr,format);
  setWindowText(this, IDC_MESSAGE, vformat(format,argptr));
  va_end(argptr);
}

void CWordsepgraphDlg::OnTimer(UINT_PTR nIDEvent) {
  if(m_trainerJob == NULL) {
    return;
  }
  showMessage(_T("%s"), m_trainerJob->getMessage().cstr());
  __super::OnTimer(nIDEvent);
}


BOOL CWordsepgraphDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

void CWordsepgraphDlg::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
}

static void showfile(char *fname) {
  if(_spawnl(_P_NOWAIT,"c:\\windows\\system32\\notepad.exe","notepad.exe",fname, NULL ) < 0) {
    showWarning(_T("Kan ikke starte notepad"));
  }
}

void CWordsepgraphDlg::OnFileTrainingdata() {
  showfile("ordliste");
}

void CWordsepgraphDlg::OnFileErrorwords() {
  showfile("fejlord.txt");
}

void CWordsepgraphDlg::OnGotoEditord() {
  gotoEditBox(this, IDC_EDITORD);
}

void CWordsepgraphDlg::OnChangeEditOrd() {
  enableButtons(getWindowText(this, IDC_EDITORD).length() > 0);
}

void CWordsepgraphDlg::enableButtons(bool enabled) {
  GetDlgItem(IDC_BUTTONSEPARATE)->EnableWindow(enabled);
  GetDlgItem(IDC_BUTTONLEARN   )->EnableWindow(enabled);
}
