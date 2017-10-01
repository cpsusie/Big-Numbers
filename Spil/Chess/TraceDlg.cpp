#include "stdafx.h"
#include <MFCUtil/WinTools.h>
#include "TraceDlg.h"
#include "TraceDlgThread.h"
#include "SelectInfofieldsDlg.h"
#include "ChessGraphics.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTraceDlg::CTraceDlg(CTraceDlgThread &thread) : CDialog(IDD, NULL), m_thread(thread) {
  m_hIcon          = theApp.LoadIcon(IDR_MAINFRAME);
  m_initDone       = false;
  m_optionSizedone = false;
  m_textBox        = NULL;
  m_caretPos       = 0;
  m_keepText       = FALSE;
}

void CTraceDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Check(pDX, IDC_CHECK_KEEPTEXT, m_keepText);
}

BEGIN_MESSAGE_MAP(CTraceDlg, CDialog)
    ON_WM_MOVE()
    ON_WM_SIZE()
    ON_WM_CLOSE()
    ON_WM_TIMER()
    ON_WM_SHOWWINDOW()
    ON_BN_CLICKED(IDC_BUTTON_HIDE           , OnButtonhide            )
    ON_BN_CLICKED(IDC_BUTTON_CLEAR          , OnButtonClear           )
    ON_BN_CLICKED(IDC_CHECK_KEEPTEXT        , OnCheckKeepText         )
    ON_COMMAND(ID_FONTSIZE_75               , OnFontsize75            )
    ON_COMMAND(ID_FONTSIZE_100              , OnFontsize100           )
    ON_COMMAND(ID_FONTSIZE_125              , OnFontsize125           )
    ON_COMMAND(ID_FONTSIZE_150              , OnFontsize150           )
    ON_COMMAND(ID_FONTSIZE_175              , OnFontsize175           )
    ON_COMMAND(ID_FONTSIZE_200              , OnFontsize200           )
    ON_COMMAND(ID_VIEW_INFOFIELDS           , OnViewInfofields        )
    ON_MESSAGE(ID_MSG_PRINTTEXT             , OnMsgPrintText          )
    ON_MESSAGE(ID_MSG_UPDATEMESSAGEFIELD    , OnMsgUpdateMessageField )
    ON_MESSAGE(ID_MSG_CLEARTRACE            , OnMsgClearTrace         )
    ON_MESSAGE(ID_MSG_REPOSITIONTRACEWINDOW , OnMsgReposition         )
END_MESSAGE_MAP()

BOOL CTraceDlg::OnInitDialog() {
  __super::OnInitDialog();
  setControlText(IDD, this);
  SetIcon(m_hIcon, TRUE);
  SetIcon(m_hIcon, FALSE);
  m_textBox      = (CEdit*)GetDlgItem(IDC_EDIT_TEXTBOX);
  m_messageField = (CEdit*)GetDlgItem(IDC_EDIT_PV);

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_EDIT_TEXTBOX  , RELATIVE_SIZE         | RESIZE_FONT                 );
  m_layoutManager.addControl(IDC_EDIT_PV       , RELATIVE_Y_POS        | RELATIVE_WIDTH | RESIZE_FONT);
  m_layoutManager.addControl(IDC_BUTTON_HIDE   , PCT_RELATIVE_X_CENTER | RELATIVE_Y_POS              );
  m_layoutManager.addControl(IDC_BUTTON_CLEAR  , PCT_RELATIVE_X_CENTER | RELATIVE_Y_POS              );
  m_layoutManager.addControl(IDC_CHECK_KEEPTEXT, PCT_RELATIVE_X_CENTER | RELATIVE_Y_POS              );
  setWindowPosition(this, getOptions().getTraceWindowPos());
  setWindowSize(    this, getOptions().getTraceWindowSize());
  setFontSize(getOptions().getTraceFontSize(), false);
  m_initDone = true;
  return TRUE;
}

void CTraceDlg::OnTimer(UINT_PTR nIDEvent) {
  if(m_optionSizedone) return;
  KillTimer(1);
//  setWindowPosition(this, getOptions().getTraceWindowPos());
//  setFontSize(getOptions().getTraceFontSize(), false);
  setWindowSize(this, getOptions().getTraceWindowSize());
  m_optionSizedone = true;
  __super::OnTimer(nIDEvent);
}

void CTraceDlg::OnMove(int x, int y) {
  __super::OnMove(x, y);
  if(m_initDone) {
    getOptions().setTraceWindowPos(getWindowPosition(this));
  }
}

void CTraceDlg::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
  if(m_initDone) {
    getOptions().setTraceWindowSize(getWindowSize(this));
  }
}

void CTraceDlg::print(const String &s) {
  m_textQueue.put(s);
  SendMessage(ID_MSG_PRINTTEXT);
}

void CTraceDlg::updateMessageField(const String &s) {
  m_textQueue.put(s);
  SendMessage(ID_MSG_UPDATEMESSAGEFIELD);
}

void CTraceDlg::clear() {
  if(m_keepText) {
    return;
  }
  SendMessage(ID_MSG_CLEARTRACE);
}

void CTraceDlg::reposition() {
  SendMessage(ID_MSG_REPOSITIONTRACEWINDOW);
}

LRESULT CTraceDlg::OnMsgPrintText(WPARAM wp, LPARAM lp) {
  if(m_textQueue.isEmpty()) {
    return 0;
  }
  String text = m_textQueue.get();
  text.replace('\n', _T("\r\n"));
  String currentText = getWindowText(m_textBox);
  if(m_caretPos < currentText.length()) {
    currentText = left(currentText, m_caretPos);
  }
  const intptr_t bsIndex = text.find('\b');
  if(bsIndex >= 0) {  // dont incr caretPos
    text.cstr()[bsIndex] = '\0';
    currentText += text.cstr();
    setWindowText(m_textBox,currentText);
  } else {
    currentText += text;
    setWindowText(m_textBox, currentText);
    m_caretPos = (UINT)currentText.length();
  }
  scrollToBottom();
  return 0;
}

LRESULT CTraceDlg::OnMsgUpdateMessageField(WPARAM wp, LPARAM lp) {
  if(m_textQueue.isEmpty()) {
    return 0;
  }
  setWindowText(m_messageField, m_textQueue.get());
  return 0;
}

LRESULT CTraceDlg::OnMsgClearTrace(WPARAM wp, LPARAM lp) {
  m_textQueue.clear();
  m_textBox->SetWindowText(EMPTYSTRING);
  m_messageField->SetWindowText(EMPTYSTRING);
  m_caretPos = 0;
  return 0;
}

LRESULT CTraceDlg::OnMsgReposition(WPARAM wp, LPARAM lp) {
  putWindowBesideWindow(this, theApp.GetMainWnd());
/*
  const CSize screenSize = getScreenSize();
  WINDOWPLACEMENT mainWinWpl, wpl;
  theApp.GetMainWnd()->GetWindowPlacement(&mainWinWpl);
  const CSize mainWinSize = CRect(mainWinWpl.rcNormalPosition).Size();
  GetWindowPlacement(&wpl);
  const CSize size = CRect(wpl.rcNormalPosition).Size();
  if(mainWinWpl.rcNormalPosition.right + size.cx > screenSize.cx) {
    mainWinWpl.rcNormalPosition.left = max(0,screenSize.cx - size.cx - mainWinSize.cx);
    mainWinWpl.rcNormalPosition.right = mainWinWpl.rcNormalPosition.left + mainWinSize.cx;
    theApp.GetMainWnd()->SetWindowPlacement(&mainWinWpl);
  }
  wpl.rcNormalPosition.left   = mainWinWpl.rcNormalPosition.right;
  wpl.rcNormalPosition.right  = wpl.rcNormalPosition.left + size.cx;
  wpl.rcNormalPosition.top    = mainWinWpl.rcNormalPosition.top;
  wpl.rcNormalPosition.bottom = wpl.rcNormalPosition.top + size.cy;
  SetWindowPlacement(&wpl);
*/
  return 0;
}

void CTraceDlg::scrollToBottom() {
  m_textBox->LineScroll(m_textBox->GetLineCount());
}

void CTraceDlg::OnButtonhide() {
  OnClose();
}

void CTraceDlg::OnButtonClear() {
  OnMsgClearTrace(0,0);
}

void CTraceDlg::OnCancel() {
  // ignore
}

void CTraceDlg::OnClose() {
  m_thread.setActive(false);
}

void CTraceDlg::OnShowWindow(BOOL bShow, UINT nStatus) {
  __super::OnShowWindow(bShow, nStatus);

  if(bShow) {
    BringWindowToTop();
  }
  m_thread.setActiveProperty(bShow?true:false);
}

void CTraceDlg::OnCheckKeepText() {
  m_keepText = IsDlgButtonChecked(IDC_CHECK_KEEPTEXT);
}

void CTraceDlg::OnFontsize75()  { setFontSize(75 , true); }
void CTraceDlg::OnFontsize100() { setFontSize(100, true); }
void CTraceDlg::OnFontsize125() { setFontSize(125, true); }
void CTraceDlg::OnFontsize150() { setFontSize(150, true); }
void CTraceDlg::OnFontsize175() { setFontSize(175, true); }
void CTraceDlg::OnFontsize200() { setFontSize(200, true); }

void CTraceDlg::setFontSize(int pct, bool redraw) {
  m_layoutManager.scaleFont((double)pct / 100, redraw);
  getOptions().setTraceFontSize(pct);
  FontSizeMenuManager::setFontSize(this, pct);
}

void CTraceDlg::OnViewInfofields(){
  CSelectInfoFieldsDlg().DoModal();
}
