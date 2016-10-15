#include "stdafx.h"
#include <MFCUtil/WinTools.h>
#include "TraceDlg.h"
#include "TraceDlgThread.h"
#include "ChessGraphics.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTraceDlg::CTraceDlg(CTraceDlgThread &thread) : CDialog(CTraceDlg::IDD, NULL), m_thread(thread) {
  m_hIcon    = theApp.LoadIcon(IDR_MAINFRAME);
  m_textBox  = NULL;
  m_caretPos = 0;
  m_keepText = FALSE;
}

void CTraceDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Check(pDX, IDC_CHECK_KEEPTEXT, m_keepText);
}

BEGIN_MESSAGE_MAP(CTraceDlg, CDialog)
    ON_WM_SIZE()
    ON_MESSAGE(ID_MSG_PRINTTEXT             , OnPrintText          )
    ON_MESSAGE(ID_MSG_UPDATEMESSAGEFIELD    , OnUpdateMessageField )
    ON_MESSAGE(ID_MSG_CLEARTRACE            , OnClearTrace         )
    ON_MESSAGE(ID_MSG_REPOSITIONTRACEWINDOW , OnReposition         )
    ON_BN_CLICKED(IDC_BUTTON_HIDE           , OnButtonhide         )
    ON_WM_CLOSE()
    ON_WM_SHOWWINDOW()
    ON_BN_CLICKED(IDC_BUTTON_CLEAR          , OnButtonClear        )
    ON_BN_CLICKED(IDC_CHECK_KEEPTEXT        , OnCheckKeepText      )
    ON_COMMAND(ID_FONTSIZE_75               , OnFontsize75         )
    ON_COMMAND(ID_FONTSIZE_100              , OnFontsize100        )
    ON_COMMAND(ID_FONTSIZE_125              , OnFontsize125        )
    ON_COMMAND(ID_FONTSIZE_150              , OnFontsize150        )
    ON_COMMAND(ID_FONTSIZE_175              , OnFontsize175        )
    ON_COMMAND(ID_FONTSIZE_200              , OnFontsize200        )
END_MESSAGE_MAP()

BOOL CTraceDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  setControlText(IDD, this);
  SetIcon(m_hIcon, TRUE);
  SetIcon(m_hIcon, FALSE);
  m_textBox      = (CEdit*)GetDlgItem(IDC_EDIT_TEXTBOX);
  m_messageField = (CStatic*)GetDlgItem(IDC_STATIC_PV);

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_EDIT_TEXTBOX  , RELATIVE_SIZE         | RESIZE_FONT                 );
  m_layoutManager.addControl(IDC_STATIC_PV     , RELATIVE_Y_POS        | RELATIVE_WIDTH | RESIZE_FONT);
  m_layoutManager.addControl(IDC_BUTTON_HIDE   , PCT_RELATIVE_X_CENTER | RELATIVE_Y_POS              );
  m_layoutManager.addControl(IDC_BUTTON_CLEAR  , PCT_RELATIVE_X_CENTER | RELATIVE_Y_POS              );
  m_layoutManager.addControl(IDC_CHECK_KEEPTEXT, PCT_RELATIVE_X_CENTER | RELATIVE_Y_POS              );

  OnReposition(0,0);

  setFontSize(getOptions().getTraceFontSize(), false);

  return TRUE;
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

long CTraceDlg::OnPrintText(WPARAM wp, LPARAM lp) {
  if(m_textQueue.isEmpty()) {
    return 0;
  }
  String text = m_textQueue.get();
  text.replace('\n', _T("\r\n"));
  CString currentText;
  m_textBox->GetWindowText(currentText);
  UINT currentLength = currentText.GetLength();
  if(m_caretPos < currentLength) {
    currentText = currentText.Left(m_caretPos);
  }
  intptr_t bsIndex = text.find('\b');
  if(bsIndex >= 0) {  // dont incr caretPos
    text.cstr()[bsIndex] = '\0';
    currentText += text.cstr();
    m_textBox->SetWindowText(currentText);
  } else {
    currentText += text.cstr();
    m_textBox->SetWindowText(currentText);
    m_caretPos = currentText.GetLength();
  }
  scrollToBottom();
  return 0;
}

long CTraceDlg::OnUpdateMessageField(WPARAM wp, LPARAM lp) {
  if(m_textQueue.isEmpty()) {
    return 0;
  }
  m_messageField->SetWindowText(m_textQueue.get().cstr());
  return 0;
}

long CTraceDlg::OnClearTrace(WPARAM wp, LPARAM lp) {
  m_textQueue.clear();
  m_textBox->SetWindowText(_T(""));
  m_messageField->SetWindowText(_T(""));
  m_caretPos = 0;
  return 0;
}

long CTraceDlg::OnReposition(WPARAM wp, LPARAM lp) {
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
  return 0;
}

void CTraceDlg::OnSize(UINT nType, int cx, int cy) {
  CDialog::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
}

void CTraceDlg::scrollToBottom() {
  m_textBox->LineScroll(m_textBox->GetLineCount());
}

void CTraceDlg::OnButtonhide() {
  OnClose();
}

void CTraceDlg::OnButtonClear() {
  OnClearTrace(0,0);
}

void CTraceDlg::OnCancel() {
  // ignore
}

void CTraceDlg::OnClose() {
  m_thread.setActive(false);
}

void CTraceDlg::OnShowWindow(BOOL bShow, UINT nStatus) {
  CDialog::OnShowWindow(bShow, nStatus);

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
