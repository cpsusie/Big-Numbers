#include "stdafx.h"
#include <Math/FFT.h>
#include <Thread.h>
#include <DebugLog.h>
#include "Spectrum.h"
#include "SpectrumDlg.h"
#include "FrequenceDlg.h"

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

CSpectrumDlg::CSpectrumDlg(CWnd *pParent) : CDialog(CSpectrumDlg::IDD, pParent) {
  m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
}

void CSpectrumDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSpectrumDlg, CDialog)
  ON_WM_SYSCOMMAND()
  ON_WM_QUERYDRAGICON()
  ON_WM_PAINT()
  ON_WM_SIZE()
  ON_WM_CLOSE()
  ON_COMMAND(ID_FILE_EXIT, OnFileExit)
  ON_COMMAND(ID_CAPTURE_START, OnCaptureStart)
  ON_COMMAND(ID_CAPTURE_STOP, OnCaptureStop)
  ON_COMMAND(ID_SOUND_BEEP, OnSoundBeep)
  ON_COMMAND(ID_SOUND_SILENT, OnSoundSilent)
  ON_COMMAND(ID_SOUND_FREQUENCE, OnSoundFrequence)
END_MESSAGE_MAP()

#define SAMPLECOUNT 1024 /* must be a power of 2 */

BOOL CSpectrumDlg::OnInitDialog() {
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

  // Set the icon for this dialog.  The framework does this automatically
  //  when the application's main window is not a dialog
  SetIcon(m_hIcon, TRUE);         // Set big icon
  SetIcon(m_hIcon, FALSE);        // Set small icon
  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_ACCELERATOR1));

  m_waveSystem.substituteControl(     this, IDC_WAVESYSTEM);
  m_frequenceSystem.substituteControl(this, IDC_FREQUENCESYSTEM);
  m_waveSystem.setDataRange(DataRange(0,SAMPLECOUNT,-10,255), true);
  m_frequenceSystem.setDataRange(DataRange(0,SAMPLECOUNT,-10,255), true);
  m_beepFrequence = 1000;

  try {
    m_capture = new MMCapture(CAPTURE_VIDEO|CAPTURE_AUDIO, *this,1,false,SAMPLECOUNT);
  } catch(Exception e) {
    showException(e);
    exit(-1);
  }

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSpectrumDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX)  {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

BOOL CSpectrumDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  m_handlingEvent = false;
  switch(pMsg->message) {
  case WM_PAINT:
  case WM_SIZE:
    m_handlingEvent = true;
  default:
    break;
  }
  if(m_handlingEvent) {
    m_sampleDone.wait();
    m_sampleDone.notify();
  }
  debugLog(_T("Msg:%s\n"), getMessageName(pMsg->message).cstr());
  BOOL ret = __super::PreTranslateMessage(pMsg);
  m_handlingEvent = false;
  return ret;
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSpectrumDlg::OnPaint() {
  if (IsIconic()) {
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
  }
}

HCURSOR CSpectrumDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CSpectrumDlg::OnSize(UINT nType, int cx, int cy) {
  CWnd *waveWin      = GetDlgItem(IDC_WAVESYSTEM);
  CWnd *frequenceWin = GetDlgItem(IDC_FREQUENCESYSTEM);
  if(waveWin == NULL || frequenceWin == NULL) {
    return;
  }

  __super::OnSize(nType, cx, cy);
  CRect cr;
  GetClientRect(&cr);

  WINDOWPLACEMENT wpl;
  waveWin->GetWindowPlacement(&wpl);
  wpl.rcNormalPosition.right  = cr.right;
  wpl.rcNormalPosition.bottom = cr.bottom / 2;
  waveWin->SetWindowPlacement(&wpl);

  wpl.rcNormalPosition.top = wpl.rcNormalPosition.bottom;
  wpl.rcNormalPosition.bottom = cr.bottom;
  frequenceWin->SetWindowPlacement(&wpl);
  Invalidate();
}

void CSpectrumDlg::OnOK() {
}

void CSpectrumDlg::OnCancel() {
}

void CSpectrumDlg::OnClose() {
  OnFileExit();
}

void CSpectrumDlg::OnFileExit() {
  exit(0);
}

void CSpectrumDlg::OnCaptureStart() {
  m_firstSample = true;
  m_capture->startCapture();
}

void CSpectrumDlg::OnCaptureStop() {
  m_capture->stopCapture();
}

static double *fftConvert(double *result, const unsigned char *data, int n) {
  CompactArray<Complex> v;
  for(int i = 0; i < n; i++) {
    v.add(data[i]-128);
  }
  fft(v);
  double m = 0;
  const int n2 = n/2;
  for(int i = 0; i <= n2; i++) {
    const double d = result[i] = fabs(v[i]);
    if(d > m) {
      m = d;
    }
  }
  if(m != 0) {
    const double scaleFactor = 255.0 / m;
    for(int i = 0; i <= n2; i++) {
      result[i] *= scaleFactor;
    }
  }
  return result;
}

LRESULT CSpectrumDlg::captureWaveStreamCallback(MMCapture &capture, WAVEHDR *audioHeader) {
  m_sampleDone.wait();
  if(m_handlingEvent) {
    m_sampleDone.notify();
  }
  int sampleCount = audioHeader->dwBufferLength;
  assert(sampleCount == SAMPLECOUNT);
  const WAVEFORMATEX &waveFormat = capture.getAudioFormat();
  static double maxFrequence,maxTime;
  unsigned long samplesPerSecond = waveFormat.nSamplesPerSec;

  if(m_firstSample) {
    m_waveSystem.setDataRange(DataRange(0,(double)SAMPLECOUNT/samplesPerSecond,-10,255), true);
    m_frequenceSystem.setDataRange(DataRange(0,(double)samplesPerSecond/2,-10,255), true);
    maxTime      = m_waveSystem.getDataRange().getMaxX();
    maxFrequence = m_frequenceSystem.getDataRange().getMaxX();
    m_firstSample = false;
  }

  double waveResult[SAMPLECOUNT];
  const int amplitudeCount = SAMPLECOUNT/2+1;
  double frequenceResult[amplitudeCount];

  const unsigned char *data = (unsigned char*)(audioHeader->lpData);
  for(int i = 0; i < SAMPLECOUNT; i++) {
    const double y = data[i];
    waveResult[i] = y;
  }
  fftConvert(frequenceResult, data, sampleCount);

  CClientDC wdc(&m_waveSystem);
  m_waveSystem.paint(wdc);

  Viewport2D *vp = &m_waveSystem.getViewport();
  CDC *oldDC = vp->setDC(&wdc);
  vp->setClipping(true);
  double t = 0, tStep = maxTime / (sampleCount-1);
  vp->MoveTo(0,waveResult[0]);
  int i;
  for(i = 1, t += tStep; i < sampleCount; t += tStep) {
    vp->LineTo(t,waveResult[i++]);
  }
  vp->setClipping(false);
  vp->setDC(oldDC);

  CClientDC fdc(&m_frequenceSystem);
  m_frequenceSystem.paint(fdc);
  vp = &m_frequenceSystem.getViewport();
  oldDC = vp->setDC(&fdc);
  vp->setClipping(true);
  double f = 0, fStep = maxFrequence / (amplitudeCount-1);
  vp->MoveTo(f,frequenceResult[0]);
  for(i = 1, f += fStep ; i < amplitudeCount; f += fStep) {
    vp->LineTo(f, frequenceResult[i++]);
  }
  vp->setClipping(false);
  vp->setDC(oldDC);
  m_sampleDone.notify();
  return 0;
}

class BeepThread : public Thread {
  unsigned int m_frequence;
  bool m_cont;
public:
  BeepThread(unsigned int frequence);
  void start();
  void stop() {
    m_cont = false;
  }
  void setFrequence(unsigned int frequence) {
    m_frequence = frequence;
  }
  UINT run();
};

BeepThread::BeepThread(unsigned int frequence)
: Thread(_T("Bepper"))
, m_frequence(frequence) {
}

void BeepThread::start() {
  m_cont = true;
  Thread::start();
}

UINT BeepThread::run() {
  for(;;) {
    while(m_cont) {
      Beep(m_frequence,2000);
    }
    suspend();
  }
  return 0;
}

BeepThread beeper(1000);

void CSpectrumDlg::OnSoundBeep() {
  beeper.setFrequence(m_beepFrequence);
  beeper.start();
}

void CSpectrumDlg::OnSoundFrequence() {
  CFrequenceDlg dlg(m_beepFrequence);
  if(dlg.DoModal() == IDOK) {
    m_beepFrequence = dlg.getFrequence();
  }
}

void CSpectrumDlg::OnSoundSilent() {
  beeper.stop();
}
