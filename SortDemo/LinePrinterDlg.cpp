#include "stdafx.h"
#include <MFCUtil/Clipboard.h>
#include "LinePrinterDlg.h"
#include "LinePrinterThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CLinePrinterDlg::CLinePrinterDlg(CLinePrinterThread &thread) : CDialog(CLinePrinterDlg::IDD, NULL), m_thread(thread) {
  m_hIcon     = theApp.LoadIcon(IDR_MAINFRAME);
  m_lineList  = NULL;
  m_visible   = false;
}

void CLinePrinterDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLinePrinterDlg, CDialog)
    ON_MESSAGE(ID_MSG_SETTITLE    , OnSetTitle    )
    ON_MESSAGE(ID_MSG_CLEARWINDOW , OnClearWindow )
    ON_MESSAGE(ID_MSG_RECEIVELINE , OnReceiveLine )
    ON_WM_SIZE()
    ON_WM_SHOWWINDOW()
    ON_BN_CLICKED(IDC_BUTTON_COPY , OnButtonCopy  )
    ON_BN_CLICKED(IDC_BUTTON_SAVE , OnButtonSave  )
    ON_BN_CLICKED(IDC_BUTTON_CLOSE, OnButtonClose )
END_MESSAGE_MAP()

BOOL CLinePrinterDlg::OnInitDialog() {
  __super::OnInitDialog();

  SetIcon(m_hIcon, TRUE);
  SetIcon(m_hIcon, FALSE);

  CRect cr;
  GetClientRect(&cr);

  m_lineList = (CListBox*)GetDlgItem(IDC_LISTBOX_LINE);

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_LISTBOX_LINE, RELATIVE_SIZE );
  m_layoutManager.addControl(IDC_BUTTON_COPY , RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_BUTTON_SAVE , RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_BUTTON_CLOSE, RELATIVE_X_POS);

  return TRUE;
}

void CLinePrinterDlg::setTitle(const String &title) {
  m_receivedText = title;
  SendMessage(ID_MSG_SETTITLE);
}

void CLinePrinterDlg::addLine(const String &line) {
  m_receivedText = line;
  SendMessage(ID_MSG_RECEIVELINE);
}

LRESULT CLinePrinterDlg::OnSetTitle(WPARAM wp, LPARAM lp) {
  SetWindowText(m_receivedText.cstr());
  return 0;
}

LRESULT CLinePrinterDlg::OnClearWindow(WPARAM wp, LPARAM lp) {
  m_lineList->ResetContent();
  return 0;
}

LRESULT CLinePrinterDlg::OnReceiveLine(WPARAM wp, LPARAM lp) {
  m_lineList->AddString(m_receivedText.cstr());
  return 0;
}

void CLinePrinterDlg::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType,cx,cy);
}

void CLinePrinterDlg::OnShowWindow(BOOL bShow, UINT nStatus) {
  __super::OnShowWindow(bShow, nStatus);
  if(bShow) {
    BringWindowToTop();
  }
  m_thread.setVisible(bShow ? true : false);
}

String CLinePrinterDlg::getContent() {
  const int n = m_lineList->GetCount();
  String result;
  for(int i = 0; i < n; i++) {
    CString s;
    m_lineList->GetText(i, s);
    result += s;
  }
  return result;
}

void CLinePrinterDlg::OnButtonCopy() {
  putClipboard(*this, getContent());
}

static int getInt(Tokenizer &tok) {
  String s = tok.next();
  int result;
  if(_stscanf(s.cstr(), _T("%d"), &result) != 1) {
    throwException(_T("Expected int:<%s>"), s.cstr());
  }
  return result;
}

static double getDouble(Tokenizer &tok) {
  String s = tok.next();
  double result;
  if(_stscanf(s.cstr(), _T("%le"), &result) != 1) {
    throwException(_T("Expected double:<%s>"), s.cstr());
  }
  return result;
}

void CLinePrinterDlg::OnButtonSave() {
  String title = getWindowText(this);
  title.replace(':',' ').replace('\\', ' ').replace('/',' ');

  StringArray lines(Tokenizer(getContent(), _T("\n")));
  lines.removeIndex(0);
  lines.removeLast();
  lines.removeLast();

  const TCHAR *nameSuffix[] = {
    _T("Time")
   ,_T("Compares")
   ,_T("TimeConst1")
   ,_T("CompareConst1")
   ,_T("TimeConst2")
   ,_T("CompareConst2")
  };

  StringArray fileNames;
  FileNameSplitter finfo(__FILE__);
  finfo.setDir(FileNameSplitter::getChildName(finfo.getDir(), _T("Results")));
  CompactArray<FILE*> files;

  try {
    for(int i = 0; i < ARRAYSIZE(nameSuffix); i++) {
      String fileName = finfo.setFileName(format(_T("%s_%s"), title.cstr(), nameSuffix[i])).setExtension(_T("dat")).getFullPath();
      fileNames.add(fileName);
      files.add(MKFOPEN(fileName, _T("w")));
    }
    for(size_t i = 0; i < lines.size(); i++) {
      const String line = lines[i];
      Tokenizer tok(line, _T(" "));
      const int n = getInt(tok);
      for(size_t j = 0; j < files.size(); j++) {
        _ftprintf(files[j], _T("%d %le\n"), n, getDouble(tok));
      }
    }
    for(size_t  i = 0; i < files.size(); i++) {
      fclose(files[i]);
    }
  } catch(...) {
    for(size_t i = 0; i < files.size(); i++) {
      fclose(files[i]);
    }
    throw;
  }

}

void CLinePrinterDlg::OnButtonClose() {
  OnOK();
}

