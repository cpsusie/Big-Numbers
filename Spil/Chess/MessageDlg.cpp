#include "stdafx.h"
#include <MFCUtil/WinTools.h>
#include "Chess.h"
#include "MessageDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CMessageDlg::CMessageDlg(int milliSeconds, const String &caption, const String &message, CWnd *pParent) : CDialog(CMessageDlg::IDD, pParent) {
  m_milliSeconds   = milliSeconds;
  m_caption        = caption;
  m_message        = _T("\n\n") + message;
  m_message.replace('\n',_T("\r\n"));
  m_timerIsRunning = false;
}

void CMessageDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMessageDlg, CDialog)
  ON_WM_TIMER()
  ON_COMMAND(ID_CLOSE_WINDOW, OnCloseWindow                      )
END_MESSAGE_MAP()

BOOL CMessageDlg::OnInitDialog() {
  __super::OnInitDialog();

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_MESSAGE_ACCELERATOR));
  CEdit *textBox = (CEdit*)GetDlgItem(IDC_EDIT_TEXTBOX);
  textBox->SetWindowText(m_message.cstr());
  textBox->SetSel(0,0);
  SetWindowText(m_caption.cstr());

  CClientDC captionDC(textBox);
  const CSize captionSize = getTextExtent(captionDC, m_caption);
  int newlineCount = 0;
  for(const TCHAR *s = m_message.cstr(); *s; s++) {
    if(*s =='\n') newlineCount++;
  }

  CClientDC tdc(textBox);
  CClientDC dc(this);
  const CSize messageSize = getTextExtent(dc, m_message);
  TEXTMETRIC tm;
  dc.GetTextMetrics(&tm);

  const int textBoxClientWidth  = max(captionSize.cx + 70, messageSize.cx + 70);
  const int textBoxClientHeight = tm.tmHeight * (newlineCount+2);
  const CSize textBoxClientSize(textBoxClientWidth, textBoxClientHeight*2);

  setClientRectSize(textBox, textBoxClientSize);

  const CSize textBoxWindowSize = getWindowSize(textBox);

  setWindowPosition(textBox,CPoint(0,0));
  setClientRectSize(this,textBoxWindowSize);

  CRect endTextBoxClientRect, endWindowClientRect1, endWindowClientRect2;
  GetClientRect(&endWindowClientRect1);

  centerWindow(this);

  textBox->GetClientRect(&endTextBoxClientRect);
  GetClientRect(&endWindowClientRect2);

  startTimer(m_milliSeconds);
  return TRUE;
}

void CMessageDlg::startTimer(UINT milliSeconds) {
  if(!m_timerIsRunning && SetTimer(1,milliSeconds,nullptr)) {
    m_timerIsRunning = true;
  }
}

void CMessageDlg::stopTimer() {
  if(m_timerIsRunning) {
    KillTimer(1);
    m_timerIsRunning = false;
  }
}

void CMessageDlg::OnTimer(UINT_PTR nIDEvent) {
  __super::OnTimer(nIDEvent);
  stopTimer();
  OnOK();
}

void CMessageDlg::OnCloseWindow() {
  OnOK();
}

BOOL CMessageDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}
