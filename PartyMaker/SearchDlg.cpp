#include "stdafx.h"
#include <MFCUtil/Clipboard.h>
#include "PartyMaker.h"
#include "SearchDlg.h"
#include "EditTagDlg.h"
#include "ShowDuplicatesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SearchDlg::SearchDlg(const MediaArray &mediaArray, bool showMenu, CWnd *pParent)
: CDialog(SearchDlg::IDD, pParent)
, m_mediaArray(mediaArray)
, m_showMenu(showMenu)
{
    m_workerThread   = NULL;
    m_timerIsRunning = false;
    m_searchText = EMPTYSTRING;
}

SearchDlg::~SearchDlg() {
  destroyWorkerThread();
}

void SearchDlg::destroyWorkerThread() {
  if(m_workerThread) {
    m_workerThread->stop();
    while(m_workerThread->stillActive());
    delete m_workerThread;
    m_workerThread = NULL;
  }
}

void SearchDlg::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_SEARCHTEXT, m_searchText);
}

BEGIN_MESSAGE_MAP(SearchDlg, CDialog)
    ON_BN_CLICKED(IDC_CHOOSE     , OnChoose          )
    ON_LBN_DBLCLK(IDC_LISTRESULT , OnDblclkListResult)
    ON_COMMAND(ID_GOTO_SEARCHTEXT, OnGotoSearchText  )
    ON_COMMAND(ID_EDIT_COPY      , OnEditCopy        )
    ON_MESSAGE(ID_ADDMEDIAFILE   , OnAddMediaFile    )
    ON_WM_TIMER()
    ON_COMMAND(ID_EDIT_MODIFYINFO, OnEditModifyInfo)
    ON_LBN_SELCHANGE(IDC_LISTRESULT, OnSelChangeListResult)
    ON_COMMAND(ID_FIND_DUPLICATES, OnFindDuplicates)
END_MESSAGE_MAP()

BOOL SearchDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  m_accelTable   = LoadAccelerators(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDR_SEARCH_ACCELERATOR));
  m_workerThread = new SearchThread(*this);

  if(!m_showMenu) {
    SetMenu(NULL);
  }
  m_workerThread->start();

  startTimer();

  OnSelChangeListResult();
  gotoEditBox();
  return FALSE;
}

void SearchDlg::startRescan() {
  GetParent()->PostMessage(WM_COMMAND, MAKELONG(ID_OPTIONS_RESCANCATALOG, 0), 0);
}

#define TIMERUPDATERATE 400

void SearchDlg::startTimer() {
  if(!m_timerIsRunning && SetTimer(1,TIMERUPDATERATE,NULL)) {
    m_timerIsRunning = true;
  }
}

void SearchDlg::stopTimer() {
  if(m_timerIsRunning) {
    KillTimer(1);
    m_timerIsRunning = false;
  }
}

void SearchDlg::addToResult(const MediaFile &f) {
  m_searchResult.add(f);
  getListBox()->AddString(format(_T("%-40.40s %-40.40s %-40.40s"),f.getTitle(),f.getArtist(),f.getAlbum()).cstr());
}

void SearchDlg::clearResult() {
  m_searchResult.clear();
  getListBox()->ResetContent();
}

void SearchDlg::gotoEditBox() {
  getEditBox()->SetFocus();
}

void SearchDlg::gotoListBox() {
  CListBox *lb = getListBox();
  lb->SetFocus();
  lb->SetCurSel(0);
}

void SearchDlg::OnTimer(UINT_PTR nIDEvent) {
  if(m_workerThread) {
    CString text;
    getEditBox()->GetWindowText(text);
    if(text != m_lastSearchText) {
      if(text.GetLength() > 0) {
        m_workerThread->startSearch((LPCTSTR)text);
      } else {
        clearResult();
      }
      m_lastSearchText = text;
    }
  }
  CDialog::OnTimer(nIDEvent);
}

void SearchDlg::OnOK() {
  if(GetFocus() == getListBox()) {
    OnChoose();
  } else if((GetFocus() == getEditBox()) && (getListBox()->GetCount() > 0)) {
    gotoListBox();
  }
}

void SearchDlg::OnChoose() {
  if(findSelected(m_selected).size() > 0) {
    stopTimer();
    destroyWorkerThread();
    CDialog::OnOK();
  }
}

void SearchDlg::OnCancel() {
  stopTimer();
  destroyWorkerThread();
  m_selected.clear();
  CDialog::OnCancel();
}

MediaArray &SearchDlg::findSelected(MediaArray &selected) { // return selected
  selected.clear();
  CListBox *lb = getListBox();
  const int count = lb->GetSelCount();
  if(count > 0) {
    int *items = new int[count];
    lb->GetSelItems(count,items);
    for(int i = 0; i < count; i++) {
      selected.add(m_searchResult[items[i]]);
    }
    delete[] items;
  }
  return selected;
}

void SearchDlg::OnDblclkListResult() {
  OnChoose();
}

void SearchDlg::OnGotoSearchText() {
  gotoEditBox();
}

LRESULT SearchDlg::OnAddMediaFile(WPARAM wp, LPARAM lp) {
  if(wp) {
    clearResult();
  }
  if(lp >= 0) {
    addToResult(m_mediaArray[lp]);
  }
  return 0;
}

void SearchDlg::OnEditModifyInfo() {
  MediaArray editArray;
  if(findSelected(editArray).size() > 0) {
    CEditTagDlg dlg(editArray);
    if((dlg.DoModal() == IDOK) && dlg.dataHasChanged()) {
      startRescan();
      getEditBox()->Clear();
    }
  }
}

void SearchDlg::OnEditCopy() {
  CListBox *lb = getListBox();
  CEdit    *eb = getEditBox();

  if(GetFocus() == lb) {
    StringArray fnames;
    int count =  lb->GetSelCount();
    int *items = new int[count];
    lb->GetSelItems(count,items);
    for(int i = 0; i < count; i++) {
      fnames.add(m_searchResult[items[i]].getSourceURL());
    }
    try {
      clipboardDropFiles(m_hWnd, fnames);
    } catch(Exception e) {
      Message(_T("clipboardDropFiles failed:%s"),e.what());
    }
    delete[] items;
  } else if(GetFocus() == eb) {
    UpdateData();
    int start,end;
    eb->GetSel(start,end);
    int len = end - start;
    if(len > 0) {
      try {
        putClipboard(m_hWnd, (LPCTSTR)m_searchText.Mid(start,len));
      } catch(Exception e) {
        Message(_T("copy to clipboard failed:%s"),e.what());
      }
    }
  }
}

void SearchDlg::OnFindDuplicates() {
  CShowDuplicatesDlg dlg(m_mediaArray);
  dlg.DoModal();
  if(dlg.isAnyDeleted()) {
    startRescan();
    getEditBox()->Clear();
  }
}

CListBox *SearchDlg::getListBox() {
  return (CListBox*)GetDlgItem(IDC_LISTRESULT);
}

CEdit *SearchDlg::getEditBox() {
  return (CEdit*)GetDlgItem(IDC_SEARCHTEXT);
}

void SearchDlg::OnSelChangeListResult() {
  enableMenuItem(this, ID_EDIT_MODIFYINFO, getListBox()->GetSelCount() > 0);
}

BOOL SearchDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }

  switch(pMsg->message) {
  case WM_KEYDOWN:
    switch(pMsg->wParam) {
    case VK_DOWN:
      if((GetFocus() == getEditBox()) && (m_searchResult.size() > 0)) {
        gotoListBox();
        return TRUE;
      }
      break;
    }
    break;
  }

  return CDialog::PreTranslateMessage(pMsg);
}

SearchThread::SearchThread(SearchDlg &searchDlg) : m_searchDlg(searchDlg) {
  setDeamon(true);
  m_state = 0;
}

#define STATE_BUSY 0x01
#define STATE_STOP 0x02
#define STATE_KILL 0x04

void SearchThread::startSearch(const String &searchText) {
  if(searchText.length()) {
    m_requestQueue.clear();
    m_state |= STATE_STOP;
    m_requestQueue.put(searchText);
  }
}

void SearchThread::stop() {
  m_requestQueue.clear();
  m_state |= STATE_STOP | STATE_KILL;
  m_requestQueue.put(EMPTYSTRING);
}

UINT SearchThread::run() {
  for(;;) {
    const String request = m_requestQueue.get();
    m_state &= ~STATE_STOP;
    if(m_state & STATE_KILL) {
      break;
    }
    m_state |=  STATE_BUSY;
    doSearch(request);
    m_state &= ~STATE_BUSY;
  }
  return 0;
}

static TCHAR *strRemoveSpec(TCHAR *s) {
  _TUCHAR *t2 = (_TUCHAR*)s;
  for(_TUCHAR *t1 = (_TUCHAR*)s; *t1;) {
    const _TUCHAR ch = *(t1++);
    if(eIsalnum(ch) || eIsspace(ch)) {
      *(t2++) = String::upperCaseAccentTranslate[ch];
    }
  }
  *t2 = _T('\0');
  return s;
}

static TCHAR *convertString(TCHAR *dst, const TCHAR *src) {
  return strRemoveSpec(_tcscpy(dst,src));
}

void SearchThread::doSearch(const String &request) {
  TCHAR searchText[2000];
  convertString(searchText, request.cstr());
  bool firstInsert = true;
  if(_tcslen(searchText) == 0) {
    addToResult(firstInsert, -1); // none found and not interrupted => clear result-list.
    return;
  }
  m_BMAutomate.compilePattern(searchText, true, String::upperCaseAccentTranslate);
  const MediaArray &list = m_searchDlg.getMediaArray();
  list.wait();
  const size_t n = list.size();
  size_t i;
  for(i = 0; (m_state == STATE_BUSY) && (i < n); i++) {
    const MediaFile &mf = list[i];
    TCHAR tmp[2000];
    if((m_BMAutomate.search(convertString(tmp, mf.getTitle() )) >= 0)
     ||(m_BMAutomate.search(convertString(tmp, mf.getAlbum() )) >= 0)
     ||(m_BMAutomate.search(convertString(tmp, mf.getArtist())) >= 0)) {
      addToResult(firstInsert, (int)i);
    }
  }
  list.notify();
  if((i == n) && firstInsert) {
    addToResult(firstInsert, -1); // none found and not interrupted => clear result-list.
  }
}

void SearchThread::addToResult(bool &firstInsert, int index) {
  m_searchDlg.SendMessage(ID_ADDMEDIAFILE, firstInsert ? TRUE : FALSE, index);
  firstInsert = false;
}
