#include "stdafx.h"
#include <Process.h>
#include <Random.h>
#include <TreeMap.h>
#include "wordsepgraph.h"
#include "wordsepgraphDlg.h"
#include "ParamDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
public:
  CAboutDlg();

  enum { IDD = IDD_ABOUTBOX };

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CWordsepgraphDlg::CWordsepgraphDlg(CWnd *pParent) : CDialog(CWordsepgraphDlg::IDD, pParent) {
  m_ord            = _T("");
  m_details        = _T("");
  m_hIcon          = theApp.LoadIcon(IDR_MAINFRAME);
  m_trainerThread  = NULL;
  m_timerIsRunning = false;
}

void CWordsepgraphDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
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
  CDialog::OnInitDialog();

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
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
  } else {
    CDialog::OnSysCommand(nID, lParam);
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
    CDialog::OnPaint();
  }
}

HCURSOR CWordsepgraphDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CWordsepgraphDlg::OnFileQuit() {
  OnClose();
}

void CWordsepgraphDlg::OnClose() {
  OnFileStopTraining();
  EndDialog(IDOK);
}

void CWordsepgraphDlg::OnCancel() {
}

void CWordsepgraphDlg::OnOK() {
}

void CWordsepgraphDlg::showResultDetail(const String &word) {
  String w = word;
  w.replace('-',_T(""));
  CClientDC dc(this);
  dc.SelectObject(&m_printFont);
  dc.SetBkColor(RGB(192,192,192));
  WordWindowList wwl(w);
  m_details = _T("");
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
    word.replace('-',_T(""));
    const String result = m_network.separateWord(word);
//    debugLog(_T("\"%s\"->\"%s\"\n"), word.cstr(), result.cstr());
    setWindowText(this, IDC_STATICRESULT, result);
    showResultDetail(word);
  } catch(Exception e) {
    MessageBox(e.what());
  }
}

void CWordsepgraphDlg::OnButtonLearn() {
  try {
    UpdateData();
    String word = (LPCTSTR)m_ord;
    m_network.learnWord(word);
    word.replace('-',_T(""));
    word = m_network.separateWord(word);
    setWindowText(this, IDC_STATICRESULT, word);
    showResultDetail(word);
  } catch(Exception e) {
    MessageBox(e.what());
  }
}

void CWordsepgraphDlg::OnFileLoadNetwork() {
  try {
    m_network.load();
  } catch(Exception e) {
    MessageBox(e.what());
  }
}

void CWordsepgraphDlg::OnFileSaveNetwork() {
  try {
    m_network.save();
  } catch(Exception e) {
    MessageBox(e.what());
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

class WordWindowCheck {
public:
  String m_ord;
  bool   m_allowSeparation;
  int    m_count;
  WordWindowCheck(const String &s, bool allowSeparation) {
    m_ord             = s;
    m_allowSeparation = allowSeparation;
    m_count           = 1;
  }
};

typedef StringTreeMap<WordWindowCheck> WordWindowMap;
typedef Array<WordWindow>              TrainingList;

static void readTrainingData(TrainingList &list, WordWindowMap &wwmap) {
  FILE *f = FOPEN(_T("ordliste"),_T("r"));
  TCHAR line[512];
  while(FGETS(line,ARRAYSIZE(line),f)) {
    strRemove(line,'\n');
    String s(strTrim(line));
    WordWindowList ww(line);
    for(UINT i = 0; i < ww.size(); i++) {
      WordWindowCheck *wwc = wwmap.get(ww[i].m_window);
      if(wwc == NULL) {
        wwmap.put(ww[i].m_window,WordWindowCheck(s,ww[i].m_allowSeparation));
      } else {
        if(wwc->m_allowSeparation == ww[i].m_allowSeparation) {
          wwc->m_count++;
        } else {
          fclose(f);
          throwException(_T("Ordvindue <%s> findes i både %s og %s med forskellig deling")
                        ,ww[i].toString().cstr()
                        ,wwc->m_ord.cstr()
                        ,s.cstr()
                        );
        }
      }
    }
  }
  fclose(f);
  for(Iterator<Entry<String,WordWindowCheck> > wwit = wwmap.entrySet().getIterator(); wwit.hasNext(); ) {
    Entry<String,WordWindowCheck> &entry = wwit.next();
    list.add(WordWindow(entry.getKey().cstr(),entry.getValue().m_allowSeparation));
  }

  FILE *wwfile = fopen(_T("wwdat.txt"),_T("w"));
  if(wwfile != NULL) {
    for(Iterator<Entry<String,WordWindowCheck> > wwit = wwmap.entrySet().getIterator(); wwit.hasNext(); ) {
      Entry<String,WordWindowCheck> &entry = wwit.next();
      _ftprintf(wwfile,_T("%d %s\n"),entry.getValue().m_count,entry.getKey().cstr());
    }
    fclose(wwfile);
  }
}

static void readCycleCount(int &cycleCount, int &errorCount) { // find the last line in error.dat and return cycleCount.
  cycleCount = 0;
  errorCount = 0;

  FILE *f = fopen(_T("error.dat"),_T("r"));
  if(f == NULL) return;
  TCHAR line[100];
  while(FGETS(line, ARRAYSIZE(line),f)) {
    if(_stscanf(line,_T("%d %d"),&cycleCount,&errorCount) != 2) {
      break;
    }
  }
  fclose(f);
}

TrainigThread::TrainigThread(const WordBpn &n) : m_trainingNetwork(n) {
  setDeamon(true);
  m_done      = false;
  m_terminate = false;
}

UINT TrainigThread::run() {
  try {
    int cycleCount;
    int lastCycleErrorCount;
    readCycleCount(cycleCount,lastCycleErrorCount);
    TrainingList trainingData;
    WordWindowMap wwmap;
    updateMessage(_T("Read trainingData"));
    readTrainingData(trainingData,wwmap);

    while(!m_terminate) {
      int cycleErrorCount    = 0;
      trainingData.shuffle();
      CompactIntArray fejlord;
      double starttime = getProcessTime();
      for(UINT i = 0; !m_terminate && i < trainingData.size(); i++) {
        if(m_trainingNetwork.learnWordWindow(trainingData[i])) {
          cycleErrorCount++;
          fejlord.add(i);
        }
        if(i % 200 == 0) {
          updateMessage(_T("cycle (%2d %3.0lf%%) %5d (%6.3lf%%) errors in this cycle. last cycle:%5d, time:%2.3lf ")
                       ,cycleCount
                       ,(double)i/trainingData.size()*100
                       ,cycleErrorCount
                       ,(double)cycleErrorCount/(i+1)*100
                       ,lastCycleErrorCount
                       ,(getProcessTime() - starttime)/1000000);
        }
      }
      cycleCount++;
      FILE *logfile = FOPEN(_T("error.dat"),_T("a"));
      _ftprintf(logfile, _T("%d %d\n"),cycleCount,cycleErrorCount);
      fclose(logfile);
      FILE *fejlordfile = FOPEN(_T("fejlord.txt"),_T("w"));
      StringArray tmp;
      size_t i;
      for(i = 0; i < fejlord.size(); i++) {
        int index = fejlord[i];
        WordWindowCheck *wwc = wwmap.get(trainingData[index].m_window);
        tmp.add(format(_T("%-40s %s"),(wwc)?wwc->m_ord.cstr():_T(""),trainingData[index].toString().cstr()));
      }
      tmp.sort(stringHashCmp);
      for(i = 0; i < tmp.size(); i++) {
        _ftprintf(fejlordfile,_T("%s\n"),tmp[i].cstr());
      }
      fclose(fejlordfile);
      lastCycleErrorCount = cycleErrorCount;
    }
  } catch(Exception e) {
    updateMessage(_T("%s"),e.what());
  }
  m_done = true;
  return 0;
}

void TrainigThread::updateMessage(const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr,format);
  const String tmp = vformat(format, argptr);
  va_end(argptr);
  m_sem.wait();
  m_msg = tmp;
  m_sem.signal();
}

String TrainigThread::getMessage() {
  m_sem.wait();
  const String res = m_msg;
  m_sem.signal();
  return res;
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
  m_trainerThread  = new TrainigThread(m_network);
  m_trainerThread->setPriority(m_threadPriority);
  m_trainerThread->start();
  startTimer();
  enableMenuItem(this,ID_FILE_STARTTRAINING,MF_GRAYED );
  enableMenuItem(this,ID_FILE_STOPTRAINING ,MF_ENABLED);
}

void CWordsepgraphDlg::stopTraining() {
  stopTimer();
  m_trainerThread->terminate();
  while(m_trainerThread->stillActive()) {
    Sleep(300);
  }
  m_network = m_trainerThread->getBpn();
  delete m_trainerThread;
  m_trainerThread = NULL;
  enableMenuItem(this,ID_FILE_STARTTRAINING,MF_ENABLED);
  enableMenuItem(this,ID_FILE_STOPTRAINING ,MF_GRAYED );
}

void CWordsepgraphDlg::setTrainingPriority(int p) {
  m_threadPriority = p;
  if(m_trainerThread != NULL) {
    m_trainerThread->setPriority(p);
  }
}

void CWordsepgraphDlg::OnFileStopTraining() {
  if(m_trainerThread == NULL) {
    return;
  }
  stopTraining();
}

void CWordsepgraphDlg::showMessage(const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr,format);
  setWindowText(this, IDC_MESSAGE, vformat(format,argptr));
  va_end(argptr);
}

void CWordsepgraphDlg::OnTimer(UINT_PTR nIDEvent) {
  if(m_trainerThread == NULL) {
    return;
  }
  showMessage(_T("%s"), m_trainerThread->getMessage().cstr());
  if(m_trainerThread->isDone()) {
    stopTraining();
  }
  CDialog::OnTimer(nIDEvent);
}


BOOL CWordsepgraphDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return CDialog::PreTranslateMessage(pMsg);
}

void CWordsepgraphDlg::OnSize(UINT nType, int cx, int cy) {
  CDialog::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
}

static void showfile(char *fname) {
  if(_spawnl(_P_NOWAIT,"c:\\windows\\system32\\notepad.exe","notepad.exe",fname, NULL ) < 0) {
    AfxMessageBox(_T("Kan ikke starte notepad"),MB_ICONEXCLAMATION);
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
