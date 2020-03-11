#include "stdafx.h"
#include "TestWebCam.h"
#include "TestWebCamDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
public:
  CAboutDlg() : CDialog(IDD) {
  }
  enum { IDD = IDD_ABOUTBOX };
protected:
  DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CTestWebCamDlg::CTestWebCamDlg(CWnd *pParent) : CDialog(IDD, pParent) {
  m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
  m_timerIsRunning  = false;
  m_edgeDetectionOn = false;
  m_lastImage       = 0;
}

void CTestWebCamDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
}

void CTestWebCamDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

HCURSOR CTestWebCamDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

BEGIN_MESSAGE_MAP(CTestWebCamDlg, CDialog)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_WM_TIMER()
  ON_WM_CLOSE()
  ON_WM_MOVE()
  ON_BN_CLICKED(IDOK                  , OnBnClickedOk       )
  ON_BN_CLICKED(IDCANCEL              , OnBnClickedCancel   )
  ON_BN_CLICKED(IDC_BUTTONSTARTCAPTURE, OnButtonStartCapture)
  ON_BN_CLICKED(IDC_BUTTONSTOPCAPTURE , OnButtonStopCapture )
  ON_BN_CLICKED(IDC_BUTTONSTARTTIMER  , OnButtonStartTimer  )
  ON_BN_CLICKED(IDC_BUTTONSTOPTIMER   , OnButtonStopTimer   )
  ON_COMMAND(ID_FILE_EXIT             , OnFileExit          )
  ON_COMMAND(ID_FILTERS_EDGE          , OnFiltersEdge       )
END_MESSAGE_MAP()

BOOL CTestWebCamDlg::OnInitDialog() {
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

  Array<MMCaptureDevice> names = MMCaptureDevice::getDevices();

  try {
    m_capture = new MMCapture(CAPTURE_VIDEO|CAPTURE_AUDIO, *this,15,true,512);
  } catch(Exception e) {
    showException(e, MB_ICONSTOP);
    exit(-1);
  }
  CheckDlgButton(IDC_CHECKLOG,BST_CHECKED);

//    startTimer();

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void fatalError(TCHAR *format, ... ) {
  va_list argptr;
  va_start(argptr, format );
  vshowMessageBox(MB_ICONSTOP, format,argptr);
  va_end(argptr);
  exit(-1);
}

void showError(const Exception &e) {
  fatalError(_T("%s"), e.what());
}

void CTestWebCamDlg::initLog() {
  CListBox *textBox = (CListBox*)GetDlgItem(IDC_TEXTINFO);
  textBox->ResetContent();
}

void CTestWebCamDlg::vlog(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  if(!IsDlgButtonChecked(IDC_CHECKLOG)) {
    return;
  }
  CListBox *textBox = (CListBox *)GetDlgItem(IDC_TEXTINFO);
  if(textBox != NULL) {
    String line = vformat(format,argptr).cstr();
    int lineCount = textBox->GetCount();
//    if(lineCount == 15)
//      initLog();
    textBox->AddString(line.cstr());
    textBox->SetScrollRange(SB_VERT,0,lineCount-1);
    int scrollPos = lineCount - 1 - 30;
    textBox->SetCurSel(lineCount - 1);
    if(scrollPos > 0)
      textBox->SetScrollPos(SB_VERT,scrollPos);
    textBox->PostMessage(WM_PAINT);
  }
}

LRESULT CTestWebCamDlg::captureVideoStreamCallback(MMCapture &capture, PixRect *image) {
  m_gate.wait();
  CRect r = image->getRect();
  if (m_lastImage == NULL) {
    m_lastImage = image->clone(true);
  } else {
    m_lastImage->rop(r, SRCCOPY, image, r);
  }
  if (m_edgeDetectionOn) {
    m_lastImage->rop(CPoint(1,1), r.Size(), SRCINVERT, image, ORIGIN);
  }
  m_gate.notify();
  PostMessage(WM_PAINT);
  return S_OK;
}

LRESULT CTestWebCamDlg::captureStatusCallback(MMCapture &capture, int id, const TCHAR *description) {
  log(_T("captureStatusCallback id=%d, description=\"%s\""), id, description);
  if(id == IDS_CAP_BEGIN) {
    GetDlgItem(IDC_BUTTONSTARTCAPTURE)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTONSTOPCAPTURE)->EnableWindow(TRUE);
  } else if(id == IDS_CAP_END) {
    GetDlgItem(IDC_BUTTONSTARTCAPTURE)->EnableWindow(TRUE);
    GetDlgItem(IDC_BUTTONSTOPCAPTURE)->EnableWindow(FALSE);
  }
  return S_OK;
}

LRESULT CTestWebCamDlg::captureControlCallback(MMCapture &capture, int state) {
  if(state == CONTROLCALLBACK_PREROLL) {
    log(_T("captureControlCallback state=CONTROLCALLBACK_PREROLL"));
  }
  return S_OK;
}

LRESULT CTestWebCamDlg::captureErrorCallback(MMCapture &capture, int id, const TCHAR *message) {
  log(_T("captureErrorCallback id=%d, message=\"%s\""), id, message);
  return S_OK;
}

void CTestWebCamDlg::OnButtonStartCapture() {
  initLog();
  m_capture->startCapture();
}

void CTestWebCamDlg::OnButtonStopCapture() {
  m_capture->stopCapture();
}

void CTestWebCamDlg::startTimer() {
  if(m_timerIsRunning) {
    return;
  }
  if(SetTimer(1,1000,NULL)) {
    log(_T("Timer started"));
    m_timerIsRunning = true;
  } else {
    fatalError(_T("Cannot install timer"));
  }
}

void CTestWebCamDlg::stopTimer() {
  if(m_timerIsRunning) {
    KillTimer(1);
    log(_T("Timer stopped"));
    m_timerIsRunning = false;
  }
}

void CTestWebCamDlg::OnTimer(UINT_PTR nIDEvent) {
  log(_T("OnTimer called (id=%d) videoFrames:%4d audioSamples=%6d "),(UINT)nIDEvent,m_capture->getVideoFrameCount(),m_capture->getAudioSampleCount());
  __super::OnTimer(nIDEvent);
}

void CTestWebCamDlg::OnPaint() {
  if (IsIconic())   {
    CPaintDC dc(this);

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    dc.DrawIcon(x, y, m_hIcon);
  } else {
    if(m_capture->isCapturing()) {
      m_gate.wait();
      if(m_lastImage) {
        CWnd *videoWin = GetDlgItem(IDC_VIDEOFRAME);
        CClientDC dc(videoWin);
        CRect cr;
        videoWin->GetClientRect(&cr);
        PixRect::bitBlt(dc, cr, SRCCOPY, m_lastImage, ORIGIN);
      }
      m_gate.notify();
    }
    __super::OnPaint();
  }
}

void CTestWebCamDlg::OnButtonStartTimer() {
  startTimer();
}

void CTestWebCamDlg::OnButtonStopTimer() {
  stopTimer();
}

void CTestWebCamDlg::OnFileExit() {
  EndDialog(IDOK);
}

void CTestWebCamDlg::OnClose() {
  OnFileExit();
}

void CTestWebCamDlg::OnMove(int x, int y) {
  __super::OnMove(x, y);
}

void CTestWebCamDlg::OnBnClickedOk() {
}

void CTestWebCamDlg::OnBnClickedCancel() {
}

void CTestWebCamDlg::OnFiltersEdge() {
  m_edgeDetectionOn = toggleMenuItem(this, ID_FILTERS_EDGE);
}
