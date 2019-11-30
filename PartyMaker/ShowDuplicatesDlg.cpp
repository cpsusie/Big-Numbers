#include "stdafx.h"
#include "partymaker.h"
#include "ShowDuplicatesDlg.h"
#include "PasswordDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CShowDuplicatesDlg::CShowDuplicatesDlg(const MediaArray &mediaArray, CWnd *pParent)
: m_mediaArray(mediaArray)
, CDialog(CShowDuplicatesDlg::IDD, pParent)
{
    m_filterBySize     = FALSE;
    m_filterByDuration = FALSE;

    m_workerThread     = NULL;
    m_changed          = false;
    m_timerIsRunning   = false;
}

CShowDuplicatesDlg::~CShowDuplicatesDlg() {
  if(m_workerThread) {
    m_workerThread->stop();
    while(m_workerThread->stillActive());
    delete m_workerThread;
    m_workerThread = NULL;
  }
}

void CShowDuplicatesDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_DUPLICATELIST        , m_duplicateList   );
    DDX_Check(  pDX, IDC_CHECKFILTERBYSIZE    , m_filterBySize    );
    DDX_Check(  pDX, IDC_CHECKFILTERBYDURATION, m_filterByDuration);
}

BEGIN_MESSAGE_MAP(CShowDuplicatesDlg, CDialog)
    ON_WM_SIZE()
    ON_WM_TIMER()
    ON_WM_CLOSE()
    ON_COMMAND(   ID_EDIT_DELETE           , OnEditDelete           )
    ON_BN_CLICKED(IDC_CHECKFILTERBYSIZE    , OnCheckFilterBySize    )
    ON_BN_CLICKED(IDC_CHECKFILTERBYDURATION, OnCheckFilterByDuration)
    ON_MESSAGE(ID_SHOW_STATEMESSAGE        , OnShowStateMessage     )
END_MESSAGE_MAP()

BOOL CShowDuplicatesDlg::OnInitDialog() {
  __super::OnInitDialog();

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_SHOWDUPLICATES_ACCELERATOR));

  m_duplicateList.InsertColumn(0,_T("Titel")      , LVCFMT_LEFT, 275);
  m_duplicateList.InsertColumn(1,_T("Medvirkende"), LVCFMT_LEFT, 130);
  m_duplicateList.InsertColumn(2,_T("Album")      , LVCFMT_LEFT, 160);
  m_duplicateList.InsertColumn(3,_T("Varighed")   , LVCFMT_LEFT,  40);
  m_duplicateList.InsertColumn(4,_T("Beskyttet")  , LVCFMT_LEFT,  40);
  m_duplicateList.InsertColumn(5,_T("Størrelse")  , LVCFMT_LEFT,  50);
  m_duplicateList.InsertColumn(6,_T("Filnavn")    , LVCFMT_LEFT, 450);
  m_duplicateList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);

  m_layoutManager.OnInitDialog(this);

  m_layoutManager.addControl(IDC_DUPLICATELIST        , RELATIVE_SIZE | INIT_LISTHEADERS| RESIZE_LISTHEADERS);
  m_layoutManager.addControl(IDC_CHECKFILTERBYSIZE    , RELATIVE_Y_POS   );
  m_layoutManager.addControl(IDC_CHECKFILTERBYDURATION, RELATIVE_Y_POS   );
  m_layoutManager.addControl(IDC_STATETEXT            , RELATIVE_Y_POS   );
  m_layoutManager.addControl(IDOK                     , RELATIVE_POSITION);
  m_layoutManager.addControl(IDCANCEL                 , RELATIVE_POSITION);
  OnGotoList();

  m_workerThread = new FindDuplicatesThread(*this);
  m_workerThread->start();
  startTimer();

  return FALSE;
}

void CShowDuplicatesDlg::OnTimer(UINT_PTR nIDEvent) {
  if(!m_workerThread->stillActive()) {
    stopTimer();
    showDuplicateList();
  }
  __super::OnTimer(nIDEvent);
}

typedef bool (*MediaFilter)(const MediaFile &f1, const MediaFile &f2);

static bool filterBySize(const MediaFile &f1, const MediaFile &f2) {
  return f1.getFileSize() == f2.getFileSize();
}

static bool filterByDuration(const MediaFile &f1, const MediaFile &f2) {
  return f1.getDuration() == f2.getDuration();
}

static MediaArray filterMediaArray(const MediaArray &array, MediaFilter filter) {
  const size_t n = array.size();
  MediaArray result;
  if(n > 1) {
    const MediaFile *last = &array[0];
    bool lastAdded = false;
    for(size_t i = 1; i < n; i++) {
      const MediaFile *f = &array[i];
      if(filter(*f,*last)) {
        if(!lastAdded) {
          result.add(*last);
        }
        result.add(*f);
        lastAdded = true;
      } else {
        lastAdded = false;
      }
      last = f;
    }
  }
  return result;
}

void CShowDuplicatesDlg::showDuplicateList() {
  if(m_workerThread->stillActive()) {
    return;
  }
  MediaArray result = m_duplicateArray;

  if(m_filterBySize) {
    result = filterMediaArray(result, filterBySize);
  }
  if(m_filterByDuration) {
    result = filterMediaArray(result, filterByDuration);
  }
  showMediaArray(result);
}

void CShowDuplicatesDlg::showMediaArray(const MediaArray &mediaArray) {
  const size_t n = mediaArray.size();
  m_duplicateList.DeleteAllItems();
  for(size_t i = 0; i < n; i++) {
    addMediaFile((int)i, mediaArray[i]);
  }
}

void CShowDuplicatesDlg::addMediaFile(int i, const MediaFile &f) {
  addData(m_duplicateList, i, 0, f.getTitle(),true);
  addData(m_duplicateList, i, 1, f.getArtist());
  addData(m_duplicateList, i, 2, f.getAlbum());
  addData(m_duplicateList, i, 3, f.getDurationString());
  addData(m_duplicateList, i, 4, boolToStr(f.getProtected()));
  addData(m_duplicateList, i, 5, format(_T("%8d"), f.getFileSize()));
  addData(m_duplicateList, i, 6, f.getSourceURL());
}

class MediaDuplicateComparator : public Comparator<MediaFile> {
private:
  bool &m_stop;
public:
  MediaDuplicateComparator(bool &stop) : m_stop(stop) {
  }
  int compare(const MediaFile &f1, const MediaFile &f2);
  AbstractComparator *clone() const {
    return new MediaDuplicateComparator(m_stop);
  }
};

int MediaDuplicateComparator::compare(const MediaFile &f1, const MediaFile &f2) {
  if(m_stop) {
    throw true;
  }
  int c = _tcsicmp(f1.getTitle(), f2.getTitle());
  if(c) {
    return c;
  }
  if(c = _tcsicmp(f1.getArtist(), f2.getArtist())) {
    return c;
  }
  if(c = sign(f1.getDuration() - f2.getDuration())) {
    return c;
  }
  if(c = (int)f1.getFileSize() - (int)f2.getFileSize()) {
    return c;
  }
  return _tcsicmp(f1.getSourceURL(), f2.getSourceURL());
}

FindDuplicatesThread::FindDuplicatesThread(CShowDuplicatesDlg &dlg) : m_dlg(dlg) {
  m_stop = false;
  setDemon(true);
}

void FindDuplicatesThread::showMessage(_In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr,format);
  m_dlg.m_stateMessageQueue.put(vformat(format, argptr));
  va_end(argptr);

  m_dlg.SendMessage(ID_SHOW_STATEMESSAGE);
}

UINT FindDuplicatesThread::run() {
  try {
    MediaArray &mediaArray     = m_dlg.m_mediaArray;
    MediaArray &duplicateArray = m_dlg.m_duplicateArray;

    MediaDuplicateComparator comparator(m_stop);

    showMessage(_T("Sorterer filer..."));
    mediaArray.sort(comparator);

    const size_t n = mediaArray.size();
    if(n > 1) {
      const MediaFile *last = &mediaArray[0];
      bool lastAdded = false;
      for(size_t i = 1; i < n; i++) {
        if(m_stop) {
          break;
        }
        if(i % 200 == 0) {
          showMessage(_T("Filtering... %.1lf%%"), PERCENT(i,n));
        }
        const MediaFile *f = &mediaArray[i];
        if(_tcsicmp(f->getTitle(), last->getTitle()) == 0
        && _tcsicmp(f->getArtist(), last->getArtist()) == 0) {
          if(!lastAdded) {
            duplicateArray.add(*last);
          }
          duplicateArray.add(*f);
          lastAdded = true;
        } else {
          lastAdded = false;
        }
        last = f;
      }
    }
  } catch(...) {
    // ignore
  }
  showMessage(EMPTYSTRING);
  return 0;
}

void CShowDuplicatesDlg::OnCheckFilterBySize() {
  UpdateData();
  showDuplicateList();
}

void CShowDuplicatesDlg::OnCheckFilterByDuration() {
  UpdateData();
  showDuplicateList();
}

void CShowDuplicatesDlg::OnGotoList() {
  m_duplicateList.SetFocus();
}

LRESULT CShowDuplicatesDlg::OnShowStateMessage(WPARAM wp, LPARAM lp) {
  setWindowText(this, IDC_STATETEXT, m_stateMessageQueue.get());
  return 0;
}

void CShowDuplicatesDlg::OnEditDelete() {
  const int selected = getSelectedIndex();
  if(selected < 0) {
    return;
  }
  PasswordDlg dlg;
  if(dlg.DoModal() != IDOK) {
    return;
  }

  const String sourceURL = (LPCTSTR)m_duplicateList.GetItemText(selected, 6);

  if(moveFileToTrashCan(m_hWnd, sourceURL)) {
    removeMediaFile(selected, sourceURL);
    setSelectedIndex(m_duplicateList, selected);
    m_changed = true;
  }
}

void CShowDuplicatesDlg::removeMediaFile(int listIndex, const String &sourceURL) {
  m_duplicateList.DeleteItem(listIndex);
  const int arrayIndex = findBySourceURL(sourceURL);
  if(arrayIndex >= 0) {
    m_duplicateArray.removeIndex(arrayIndex);
  }
}

int CShowDuplicatesDlg::findBySourceURL(const String &sourceURL) const {
  for(size_t i = 0; i < m_duplicateArray.size(); i++) {
    const MediaFile &f = m_duplicateArray[i];
    if(sourceURL.equalsIgnoreCase(f.getSourceURL())) {
      return (int)i;
    }
  }
  return -1;
}

int CShowDuplicatesDlg::getSelectedIndex() {
  return m_duplicateList.GetSelectionMark();
}

void CShowDuplicatesDlg::OnClose() {
  stopTimer();
  __super::OnClose();
}

void CShowDuplicatesDlg::OnSize(UINT nType, int cx, int cy) {
  m_layoutManager.OnSize(nType,cx,cy);
  __super::OnSize(nType, cx, cy);
}

void CShowDuplicatesDlg::startTimer() {
  if(!m_timerIsRunning && SetTimer(1,1000,NULL)) {
    m_timerIsRunning = true;
  }
}

void CShowDuplicatesDlg::stopTimer() {
  if(m_timerIsRunning) {
    KillTimer(1);
    m_timerIsRunning = false;
  }
}

BOOL CShowDuplicatesDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}
