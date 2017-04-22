#include "stdafx.h"
#include <Process.h>
#include <Random.h>
#include <MFCUtil/Clipboard.h>
#include "PartyMaker.h"
#include "PartyMakerDlg.h"
#include "PasswordDlg.h"
#include "SearchDlg.h"
#include "SelectDirAndListNameDlg.h"
#include "ChangePasswordDlg.h"
#include "ChangeOrderDlg.h"
#include "MusicDirDlg.h"
#include "PlayerSliderControlDlg.h"
#include "MaxChoiseDlg.h"
#include "EditTagDlg.h"
#include "InfoDlg.h"

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

CPartyMakerDlg::CPartyMakerDlg(CWnd *pParent) : CDialog(CPartyMakerDlg::IDD, pParent) {
    m_hIcon = AfxGetApp()->LoadIcon(IDI_MAINFRAME);
}

void CPartyMakerDlg::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_ALLMEDIA, m_allMedia);
    DDX_Control(pDX, IDC_MEDIAPLAYER, m_player);
}

BEGIN_MESSAGE_MAP(CPartyMakerDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_QUERYDRAGICON()
    ON_WM_TIMER()
    ON_WM_PAINT()
    ON_WM_CONTEXTMENU()
    ON_WM_MOVE()
    ON_WM_CLOSE()
    ON_WM_HSCROLL()
    ON_COMMAND(ID_FILE_NEXTNUMBER              , OnFileNextNumber              )
    ON_COMMAND(ID_FILE_EDITORDER               , OnFileEditOrder               )
    ON_COMMAND(ID_FILE_PAUSE                   , OnFilePause                   )
    ON_COMMAND(ID_FILE_PASSWORDPROTECT         , OnFilePasswordProtect         )
    ON_COMMAND(ID_FILE_GENERATEPLAYLISTFROMDIR , OnFileGeneratePlaylistFromDir )
    ON_COMMAND(ID_FILE_EXIT                    , OnFileExit                    )
    ON_COMMAND(ID_EDIT_COPY                    , OnEditCopy                    )
    ON_COMMAND(ID_EDIT_DELETE                  , OnEditDelete                  )
    ON_COMMAND(ID_EDIT_MODIFYINFO              , OnEditModifyInfo              )
    ON_COMMAND(ID_EDIT_SHOWINFO                , OnEditShowInfo                )
    ON_COMMAND(ID_EDIT_SHOWLOG                 , OnEditShowLog                 )
    ON_COMMAND(ID_EDIT_SHOWERRORS              , OnEditShowErrors              )
    ON_COMMAND(ID_EDIT_FIND                    , OnEditFind                    )
    ON_COMMAND(ID_OPTIONS_BALANCE              , OnOptionsBalance              )
    ON_COMMAND(ID_OPTIONS_SPEED                , OnOptionsSpeed                )
    ON_COMMAND(ID_OPTIONS_KATALOG              , OnOptionsKatalog              )
    ON_COMMAND(ID_OPTIONS_RESCANCATALOG        , OnOptionsRescanCatalog        )
    ON_COMMAND(ID_OPTIONS_AUTOSELECT           , OnOptionsAutoSelect           )
    ON_COMMAND(ID_OPTIONS_ALLOWDUPLICATES      , OnOptionsAllowDuplicates      )
    ON_COMMAND(ID_OPTIONS_CONFIRMCHOISE        , OnOptionsConfirmChoise        )
    ON_COMMAND(ID_OPTIONS_MAXCHOISE            , OnOptionsMaxChoise            )
    ON_COMMAND(ID_OPTIONS_COLORS_CURRENTTRACK  , OnOptionsColorsCurrentTrack   )
    ON_COMMAND(ID_OPTIONS_COLORS_MEDIAQUEUE    , OnOptionsColorsMediaQueue     )
    ON_COMMAND(ID_OPTIONS_COLORS_BACKGROUND    , OnOptionsColorsBackground     )
    ON_COMMAND(ID_OPTIONS_CHANGEPASSWORD       , OnOptionsChangePassword       )
    ON_COMMAND(ID_HELP_ABOUTPARTYMAKER         , OnHelpAboutPartymaker         )
    ON_BN_CLICKED(IDC_SEARCHBUTTON             , OnButtonSearch                )
    ON_BN_CLICKED(IDC_PAUSEBUTTON              , OnButtonPause                 )
    ON_BN_CLICKED(IDC_NEXTBUTTON               , OnButtonNext                  )
    ON_COMMAND(ID_SHOWCOUNT                    , OnShowCount                   )
    ON_NOTIFY( NM_DBLCLK        , IDC_ALLMEDIA , OnDblClkAllMedia              )
    ON_NOTIFY(LVN_COLUMNCLICK   , IDC_ALLMEDIA , OnColumnClickAllMedia         )
    ON_NOTIFY(LVN_ITEMCHANGED   , IDC_ALLMEDIA , OnItemChangedAllMedia         )
    ON_COMMAND(ID_CONTEXTMENU_REMOVE           , OnContextMenuRemove           )
    ON_COMMAND(ID_CONTEXTMENU_INFO             , OnContextMenuInfo             )
    ON_COMMAND(ID_SORT_BY_TITLE                , OnSortByTitle                 )
    ON_COMMAND(ID_SORT_BY_ARTIST               , OnSortByArtist                )
    ON_COMMAND(ID_SORT_BY_ALBUM                , OnSortByAlbum                 )
END_MESSAGE_MAP()

static void showFile(const String &fileName) {
  USES_CONVERSION;
  _spawnlp(_P_NOWAITO, "notepad", "notepad", T2A(fileName.cstr()), NULL);
}

void log(const MediaFile &mediaFile) {
  FILE *f = fopen(makeLogFileName(), _T("a"));
  if(f == NULL) {
    return;
  }
  time_t tt;
  time(&tt);
  struct tm *tm = localtime(&tt);
  _ftprintf(f,_T("%02d-%02d-%d %2d:%02d %s - %s\n")
           ,tm->tm_mday,tm->tm_mon+1,tm->tm_year+1900,tm->tm_hour,tm->tm_min
           ,mediaFile.getTitle(),mediaFile.getArtist());
  fclose(f);
}

BOOL CPartyMakerDlg::OnInitDialog() {
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

  try {
    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);         // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon

    m_allMedia.InsertColumn(0,_T("Titel")      , LVCFMT_LEFT, 350);
    m_allMedia.InsertColumn(1,_T("Medvirkende"), LVCFMT_LEFT, 200);
    m_allMedia.InsertColumn(2,_T("Album")      , LVCFMT_LEFT, 160);
    m_allMedia.SetExtendedStyle(LVS_EX_FULLROWSELECT);

    m_options.load();
    checkMenuItem(this, ID_OPTIONS_AUTOSELECT     , m_options.getAutoSelect());
    checkMenuItem(this, ID_OPTIONS_ALLOWDUPLICATES, m_options.getAllowDuplicates());
    checkMenuItem(this, ID_OPTIONS_CONFIRMCHOISE  , m_options.getConfirmChoise());

    getVolumeControl()->SetRange(0,100);
    setVolume(m_options.getVolume());

    MediaDatabase::open(m_player);

    m_mediaQueue    = new MediaQueue();
    m_newMediaArray = new LoadableMediaArray();

    m_newMediaArray->rescan(m_options.getDirList());

    m_queueFont.CreateFont(8, 8, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                          CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                          DEFAULT_PITCH | FF_MODERN,
                          _T("Courier") );

    CClientDC dc(this);
    m_currentTrackRect = getClientRect(getCurrentTrackWnd());
    m_currentTrackBitmap.CreateCompatibleBitmap(&dc, m_currentTrackRect.Width(), m_currentTrackRect.Height());
    m_currentTrackDC.CreateCompatibleDC(NULL);
    m_currentTrackDC.SelectObject(&m_currentTrackBitmap);

/*    m_currentFont.CreateFont( 16, 12, 0, 0, 700, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                              CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                              DEFAULT_PITCH | FF_MODERN,
                              _T("Courier") );

*/

    randomize();
    m_lastRefresh            = 0;
    setFlag(PAUSEBUTTONISPAUSE);
    m_prefixTime             = 0;
    m_selectedFromMediaQueue = -1;
    if(isMenuItemChecked(this, ID_FILE_EDITORDER)) {
      setFlag(EDITORDERENABLED);
    }
    SetPriorityClass(GetCurrentProcess(),HIGH_PRIORITY_CLASS);
    m_accelTable = LoadAccelerators(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));
    gotoToListBox();
  } catch(Exception e) {
    Message(_T("Fatal error:%s"), e.what());
    exit(-1);
  }
  startTimer();
  return false;
}

void CPartyMakerDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
  } else {
    CDialog::OnSysCommand(nID, lParam);
  }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPartyMakerDlg::OnPaint() {
  if(IsIconic()) {
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
    CPaintDC dlgDC(this);
    COLORREF pixelColor = dlgDC.GetPixel(10,10);
    CBrush backgroundBrush;
    backgroundBrush.CreateSolidBrush(m_options.getBackgroundColor());
    CBrush *oldBrush = dlgDC.SelectObject(&backgroundBrush);
    dlgDC.ExtFloodFill( 10,10,pixelColor,FLOODFILLSURFACE);
    dlgDC.SelectObject(oldBrush);

    showMediaQueue();
    showHeaderSortMark();
    showCurrentTrack(getPlayerState());
  }
}

HCURSOR CPartyMakerDlg::OnQueryDragIcon() {
  return (HCURSOR) m_hIcon;
}

#define TIMERUPDATERATE 1000

String mediaQueueFormat(MediaFile &mf) {
  return format(_T("%-50.50s %-38.38s")
               ,mf.getTitle()
               ,mf.getArtist()
               );
}

int CPartyMakerDlg::insertMediaFile(MediaFile &f) {
  int index = (int)m_mediaArray.binaryInsert(f,m_mediaComparator);
  m_allMedia.InsertItem( index, f.getTitle());
  m_allMedia.SetItemText(index, 1, f.getArtist());
  m_allMedia.SetItemText(index, 2, f.getAlbum());
  return index;
}

void CPartyMakerDlg::removeMediaFile(intptr_t index) {
  m_mediaArray.removeIndex(index);
  m_allMedia.DeleteItem((int)index);
}

void CPartyMakerDlg::resetMediaList() {
  m_allMedia.DeleteAllItems();
  m_mediaArray.clear();
}

int CPartyMakerDlg::findMediaFileInAllMedia(const CPoint &point) { // point in screen-coordinates
  const int n = getMediaArraySize();
  if(n == 0) {
    return -1;
  }
  CPoint p = point;
  ScreenToClient(&p);

  CRect lbrect = getRelativeWindowRect(this, IDC_ALLMEDIA);
  if(!lbrect.PtInRect(p)) {
    return -1;
  }
  p = point;
  m_allMedia.ScreenToClient(&p);
  for(int i = m_allMedia.GetTopIndex();; i++) {
    if(i >= n) {
      return -1;
    }
    CRect rect;
    m_allMedia.GetItemRect(i,&rect, LVIR_BOUNDS);
    if(rect.top > lbrect.Height()) {
      return -1;
    }
    if(rect.PtInRect(p)) {
      return i;
    }
  }
  return -1;
}

int CPartyMakerDlg::getSelectedIndex() {
  return m_allMedia.GetSelectionMark();
}

int CPartyMakerDlg::getMediaArraySize() {
  return m_allMedia.GetItemCount();
}

void CPartyMakerDlg::syncMediaList() {
  MediaArray playlist = *m_newMediaArray;
  playlist.sort(m_mediaComparator);

  m_mediaArray.wait();
  for(intptr_t i = m_mediaArray.size() - 1; i >= 0; i--) { // first remove from list
    if(playlist.binarySearch(m_mediaArray[i],m_mediaComparator) < 0) {
      removeMediaFile(i);
    }
  }
  MediaComparator cmp(CMP_FNAME);
  playlist.sort(cmp);
  for(intptr_t i = m_mediaArray.size() - 1; i >= 0; i--) { // remove by filename. title,artist and album could have changed
    if(playlist.binarySearch(m_mediaArray[i],cmp) < 0) {
      removeMediaFile(i);
    }
  }

  playlist.sort(m_mediaComparator);
  for(size_t i = 0; i < playlist.size(); i++) { // then insert into list
    if(m_mediaArray.binarySearch(playlist[i],m_mediaComparator) < 0) {
      insertMediaFile(playlist[i]);
    }
  }
  m_mediaArray.notify();
}

void CPartyMakerDlg::showMediaList() {
  if(m_newMediaArray->size() != m_mediaArray.size()) {
    time_t now;
    time(&now);
    if(m_lastRefresh != 0 && now < m_lastRefresh + 8) {
      return;
    }
    const int selectedIndex = getSelectedIndex();
    String selectedSourceURL;
    if(selectedIndex >= 0) {
      selectedSourceURL = m_mediaArray[selectedIndex].getSourceURL();
    }
    stopTimer();
    syncMediaList();
    m_lastRefresh = now;

    if(selectedIndex >= 0) {
      const int newIndex = m_mediaArray.findBySourceURL(selectedSourceURL);
      if(newIndex >= 0 && newIndex < getMediaArraySize()) {
        setSelectedIndex(m_allMedia, newIndex);
      }
    }
    startTimer();
  }
}

#define CHARHEIGHT 15

void CPartyMakerDlg::showMediaQueue() {
  CWnd *wnd = getQueueWnd();
  CClientDC dc(wnd);
  const CRect cr = getClientRect(wnd);
  dc.SetBkColor(m_options.getMediaQueueColor());
  dc.SelectObject(&m_queueFont);
  Iterator<MediaFileWithQueueId> it = m_mediaQueue->getIterator();
//  dc.getBoundsRect(&cr, DCB_ENABLE);
  for(int i = 0; i < 20; i++) {
    CRect textRect(0,i*CHARHEIGHT, cr.Width(), (i+1)*CHARHEIGHT-2);
    dc.FillSolidRect(&textRect, m_options.getMediaQueueColor());
    if(it.hasNext()) {
      dc.TextOut(0,i*CHARHEIGHT,mediaQueueFormat(it.next()).cstr());
    }
  }
  if((m_mediaQueue->size() > 1) != isFlagSet(EDITORDERENABLED)) {
    enableEditOrderItem(m_mediaQueue->size() > 1);
  }
}

void CPartyMakerDlg::enableEditOrderItem(bool enable) {
  if(enable && isPasswordProtected()) {
    return;
  }
  setFlag(EDITORDERENABLED, enable);
  enableMenuItem(this, ID_FILE_EDITORDER, enable);
}

void CPartyMakerDlg::syncMediaQueue(bool shiftToNext) {
  if(m_mediaQueue->syncPlayList(shiftToNext)) {
    showMediaQueue();
  }
}

void CPartyMakerDlg::showCurrentTrack(WMPPlayState state) {
  static String idString, durationString;
  String timeString;

  switch(state) {
  case wmppsPaused :
  case wmppsPlaying:
    break;
  default:
    clearCurrentTrack();
    return;
  }

  CWMPMedia currentMedia = m_player.GetCurrentMedia();
  if(currentMedia != NULL) {
    if(currentMedia.GetSourceURL() != m_currentMediaFile.getSourceURL()) {
      m_currentMediaFile = MediaFile(currentMedia);
      log(m_currentMediaFile);
      const TCHAR *title  = m_currentMediaFile.getTitle();
      const TCHAR *artist = m_currentMediaFile.getArtist();

      idString = format(_T("%s"), title);
      if(artist[0]) {
        idString += format(_T(" med %s"), artist);
      }
      durationString = m_currentMediaFile.getDurationString();
      clearCurrentTrack();
    }

    timeString = format(_T("%s/%s") ,(LPCTSTR)getPlayerPositionString(),durationString.cstr());

    syncMediaQueue();

    clearCurrentTrack(&m_currentTrackDC);
    m_currentTrackDC.SetBkColor(m_options.getCurrentTrackColor());
    m_currentTrackDC.TextOut(10 ,6, idString.cstr()  );
    m_currentTrackDC.TextOut(640,6, timeString.cstr());
    CClientDC dc(getCurrentTrackWnd());
    dc.BitBlt(0,0,m_currentTrackRect.Width(), m_currentTrackRect.Height(), &m_currentTrackDC,0,0,SRCCOPY);
  } else if(m_currentMediaFile.isDefined()) {
    m_currentMediaFile.clear();
    clearCurrentTrack();
  }
}

void CPartyMakerDlg::clearCurrentTrack(CDC *pdc) {
  if(pdc == NULL) {
    CClientDC dc(getCurrentTrackWnd());
    dc.FillSolidRect(&m_currentTrackRect, m_options.getCurrentTrackColor());
  } else {
    pdc->FillSolidRect(&m_currentTrackRect, m_options.getCurrentTrackColor());
  }
}

void CPartyMakerDlg::OnDblClkAllMedia(NMHDR *pNMHDR, LRESULT *pResult) {
  OnOK();
}

void CPartyMakerDlg::OnColumnClickAllMedia(NMHDR *pNMHDR, LRESULT *pResult) {
  NM_LISTVIEW *pNMListView = (NM_LISTVIEW*)pNMHDR;
  sortMediaList(pNMListView->iSubItem);
  *pResult = TRUE;
}

void CPartyMakerDlg::OnItemChangedAllMedia(NMHDR *pNMHDR, LRESULT *pResult) {
  NM_LISTVIEW *pNMListView = (NM_LISTVIEW*)pNMHDR;

  if((pNMListView->uChanged & LVIF_STATE) && (pNMListView->uNewState & LVNI_SELECTED)) {
    if(!isPasswordProtected()) {
      enableMenuItem(this,ID_EDIT_COPY                  , true);
      enableMenuItem(this,ID_EDIT_DELETE                , true);
      enableMenuItem(this,ID_EDIT_MODIFYINFO            , true);
      enableMenuItem(this,ID_EDIT_SHOWINFO              , true);
    }
  }
  *pResult = 0;
}

void CPartyMakerDlg::OnSortByTitle() {
  sortMediaList(0);
}

void CPartyMakerDlg::OnSortByArtist() {
  sortMediaList(1);
}

void CPartyMakerDlg::OnSortByAlbum() {
  sortMediaList(2);
}

bool CPartyMakerDlg::appendToMediaQueue(const MediaFile &f, bool silence) {
  if((int)m_mediaQueue->size() >= m_options.getMaxChoise()) {
    if(!silence) {
      Message(_T("Jeg kan desværre ikke huske mere end %d numre"), m_options.getMaxChoise());
    }
    return false;
  }
  if(!m_options.getAllowDuplicates() && m_mediaQueue->contains(f)) {
    if(!silence) {
      Message(_T("%s er allerede valgt"),f.getTitle());
    }
    return false;
  }
  m_mediaQueue->put(f);
  syncMediaQueue();
  bool showMessage = !silence && ((m_mediaQueue->size() >= 20) || m_options.getConfirmChoise() && ((getPlayerState() == wmppsPlaying) ? true : false));
  if(showMessage) {
    int secondsUntilPlayed = (int)(m_mediaQueue->getDuration() + m_currentMediaFile.getDuration() - getPlayerPosition() - f.getDuration());
    CTime now(CTime::GetCurrentTime());
    now = now + CTimeSpan(0,0,0,secondsUntilPlayed);
    const String klstr = format(_T("%d:%02d"),now.GetHour(),now.GetMinute());
    Message(_T("%s vil blive spillet ca. kl. %s"), f.getTitle(),klstr.cstr());
  }
  return true;
}

void CPartyMakerDlg::OnOK() {
  int selectedIndex = getSelectedIndex();
  if(selectedIndex >= 0) {
    appendToMediaQueue(m_mediaArray[selectedIndex]);
  }
}

void CPartyMakerDlg::ajourPauseButton(WMPPlayState state) {
  switch(state) {
  case wmppsPlaying:
    if(!isFlagSet(PAUSEBUTTONISPAUSE)) {
      checkMenuItem(this,ID_FILE_PAUSE,false);
      GetDlgItem(IDC_PAUSEBUTTON)->SetWindowText(_T("&Pause = Ctrl+P"));
      setFlag(PAUSEBUTTONISPAUSE);
    }
    break;
  case wmppsPaused :
    if(isFlagSet(PAUSEBUTTONISPAUSE)) {
      checkMenuItem(this,ID_FILE_PAUSE,true);
      GetDlgItem(IDC_PAUSEBUTTON)->SetWindowText(_T("Fortsæt = Ctrl+P"));
      clrFlag(PAUSEBUTTONISPAUSE);
    }
  }
}

void CPartyMakerDlg::setPasswordProtection(bool on) {
  if(on) {
    checkMenuItem( this,ID_FILE_PASSWORDPROTECT       ,true );
    enableMenuItem(this,ID_FILE_NEXTNUMBER            ,false);
    enableMenuItem(this,ID_FILE_EDITORDER             ,false);
    enableMenuItem(this,ID_FILE_PAUSE                 ,false);
    enableMenuItem(this,ID_FILE_EXIT                  ,false);
    enableMenuItem(this,ID_EDIT_COPY                  ,false);
    enableMenuItem(this,ID_EDIT_DELETE                ,false);
    enableMenuItem(this,ID_EDIT_MODIFYINFO            ,false);
    enableMenuItem(this,ID_EDIT_SHOWLOG               ,false);
    enableMenuItem(this,ID_EDIT_SHOWERRORS            ,false);
    enableMenuItem(this,_T("2")                       ,false); // options

    GetDlgItem(IDC_PAUSEBUTTON )->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_NEXTBUTTON  )->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_VOLUMESLIDER)->ShowWindow(SW_HIDE);
#define PROTFLAGS WS_MINIMIZEBOX|WS_SYSMENU

    DWORD style = GetStyle();
    DWORD protFlags = PROTFLAGS;
    ModifyStyle(PROTFLAGS,0);
    RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE|RDW_ERASE);
  } else {
    checkMenuItem( this,ID_FILE_PASSWORDPROTECT       ,false);
    enableMenuItem(this,ID_FILE_NEXTNUMBER            ,true);
    enableMenuItem(this,ID_FILE_EDITORDER             ,true);
    enableMenuItem(this,ID_FILE_PAUSE                 ,true);
    enableMenuItem(this,ID_FILE_EXIT                  ,true);
    enableMenuItem(this,ID_EDIT_COPY                  ,true);
    enableMenuItem(this,ID_EDIT_DELETE                ,true);
    enableMenuItem(this,ID_EDIT_MODIFYINFO            ,true);
    enableMenuItem(this,ID_EDIT_SHOWLOG               ,true);
    enableMenuItem(this,ID_EDIT_SHOWERRORS            ,true);
    enableMenuItem(this,_T("2")                       ,true); // options

    GetDlgItem(IDC_PAUSEBUTTON )->ShowWindow(SW_SHOW);
    GetDlgItem(IDC_NEXTBUTTON  )->ShowWindow(SW_SHOW);
    GetDlgItem(IDC_VOLUMESLIDER)->ShowWindow(SW_SHOW);

    ModifyStyle(0,PROTFLAGS);
    RedrawWindow(NULL,NULL,RDW_FRAME|RDW_INVALIDATE|RDW_ERASE);
  }
}

bool CPartyMakerDlg::isPasswordProtected() {
  return isMenuItemChecked(this,ID_FILE_PASSWORDPROTECT);
}

void CPartyMakerDlg::startNextNumber() {
  if(getPlayerState() != wmppsStopped) {
    stopPlayer();
  }
  if(m_mediaQueue->isEmpty()) {
    if(!m_options.getAutoSelect()) {
      return;
    }
    const MediaFile *mf = m_mediaArray.selectRandom();;
    if(mf == NULL) {
      return;
    } else {
      m_mediaQueue->put(*mf);
    }
  }

  syncMediaQueue(true);
  if(isMenuItemChecked(this,ID_FILE_PAUSE)) {
    pausePlayer();
  } else if(m_options.getAutoSelect()) {
    startPlayer();
  }
}

void CPartyMakerDlg::startPlayer() {
  m_player.GetControls().play();
}

void CPartyMakerDlg::stopPlayer() {
  m_player.GetControls().stop();
}

void CPartyMakerDlg::pausePlayer() {
  m_player.GetControls().pause();
}

WMPPlayState CPartyMakerDlg::getPlayerState() {
  long state = m_player.GetPlayState();
//  CString str = m_player.GetStatus();
  return (WMPPlayState)state;
}

double CPartyMakerDlg::getPlayerPosition() {
  return m_player.GetControls().GetCurrentPosition();
}

CString CPartyMakerDlg::getPlayerPositionString() {
  return m_player.GetControls().GetCurrentPositionString();
}

void CPartyMakerDlg::startTimer(int delayMsec) {
  if (isFlagSet(TIMER1_RUNNING) || isFlagSet(TIMER2_RUNNING)) {
    return;
  }
  if (delayMsec) {
    SetTimer(2, delayMsec, NULL);
    setFlag(TIMER2_RUNNING);
  } else {
    SetTimer(1,TIMERUPDATERATE,NULL);
    setFlag(TIMER1_RUNNING);
  }
}

void CPartyMakerDlg::stopTimer() {
  if(isFlagSet(TIMER2_RUNNING)) {
    KillTimer(2);
    clrFlag(TIMER2_RUNNING);
  }
  if(isFlagSet(TIMER1_RUNNING)) {
    KillTimer(1);
    clrFlag(TIMER1_RUNNING);
  }
}

void CPartyMakerDlg::OnTimer(UINT_PTR nIDEvent) {
  switch(nIDEvent) {
  case 1: onTimer1(); break;
  case 2: onTimer2(); break;
  }
  CDialog::OnTimer(nIDEvent);
}

void CPartyMakerDlg::onTimer1() {
  showMediaList();
  showMediaQueue();
  const WMPPlayState state = getPlayerState();

  switch(state) {
  case wmppsUndefined :
  case wmppsStopped   :
  case wmppsMediaEnded:
  case wmppsLast      :
    startNextNumber();
    break;
  case wmppsPaused    :
  case wmppsPlaying   :
    break;
  case wmppsReady     :
    startPlayer();
    break;
  }

  showCurrentTrack(state);
  ajourPauseButton(state);
}

void CPartyMakerDlg::onTimer2() {
  KillTimer(2);
  clrFlag(TIMER2_RUNNING);
  startTimer();
}

void CPartyMakerDlg::gotoToListBox() {
  m_allMedia.SetFocus();
}

CWnd *CPartyMakerDlg::getCurrentTrackWnd() {
  return GetDlgItem(IDC_CURRENT);
}

CWnd *CPartyMakerDlg::getQueueWnd() {
  return GetDlgItem(IDC_QUEUE);
}

static int CALLBACK compareMediaItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
  CPartyMakerDlg &dlg = *(CPartyMakerDlg*)lParamSort;
  return dlg.m_mediaComparator.compare(dlg.m_mediaArray[lParam1], dlg.m_mediaArray[lParam2]);
}

void CPartyMakerDlg::sortMediaList(int headerIndex) {
  const ComparatorCriteria &sortCrit = m_mediaComparator.getComparatorCriteria();
  if(headerIndex == sortCrit.m_headerIndex) {
    m_mediaComparator.setHeaderIndex(sortCrit.m_headerIndex, !sortCrit.m_asc);
  } else {
    m_mediaComparator.setHeaderIndex(headerIndex);
  }
  const int selectedIndex = getSelectedIndex();
  String selectedSourceURL;
  if(selectedIndex >= 0) {
    selectedSourceURL = m_mediaArray[selectedIndex].getSourceURL();
  }
  m_allMedia.SortItemsEx(compareMediaItems, (DWORD_PTR)this);
  m_mediaArray.sort(m_mediaComparator);

  if(selectedIndex >= 0) {
    const int newIndex = m_mediaArray.findBySourceURL(selectedSourceURL);
    if((newIndex >= 0) && (newIndex < getMediaArraySize())) {
      setSelectedIndex(m_allMedia, newIndex);
    }
  }
  showHeaderSortMark();
  gotoToListBox();
}

void CPartyMakerDlg::showHeaderSortMark() {
  CHeaderCtrl              *headerCtrl = m_allMedia.GetHeaderCtrl();
  const int                 n          = headerCtrl->GetItemCount();
  HDITEM                    hd;
  memset(&hd, 0, sizeof(hd));
  hd.mask = HDI_FORMAT;
  for(int i = 0; i < n; i++) {
    int ret = Header_GetItem(headerCtrl->m_hWnd, i, &hd);
    if(i != m_mediaComparator.getHeaderIndex()) {
      hd.fmt &= ~(HDF_SORTDOWN | HDF_SORTUP);
    } else if(m_mediaComparator.getAscending()) {
      hd.fmt |=  HDF_SORTUP;
      hd.fmt &= ~HDF_SORTDOWN;
    } else {
      hd.fmt |=  HDF_SORTDOWN;
      hd.fmt &= ~HDF_SORTUP;
    }
    ret = Header_SetItem(headerCtrl->m_hWnd, i, &hd);
  }
}

void CPartyMakerDlg::OnButtonSearch() {
  SearchDlg dlg(m_mediaArray, !isPasswordProtected(), this);
  if(dlg.DoModal() == IDOK) {
    bool silence = false;
    const MediaArray &selected = dlg.getSelected();
    for(size_t i = 0; i < selected.size(); i++) {
      appendToMediaQueue(selected[i], silence);
      silence = true;
    }
  }

  gotoToListBox();
}

void CPartyMakerDlg::setAutoSelect(bool newValue) {
  m_options.setAutoSelect(newValue);
  checkMenuItem(this,ID_OPTIONS_AUTOSELECT,newValue);
}

void CPartyMakerDlg::setAllowDuplicates(bool newValue) {
  m_options.setAllowDuplicates(newValue);
  checkMenuItem(this,ID_OPTIONS_ALLOWDUPLICATES,newValue);
}

void CPartyMakerDlg::setConfirmChoise(bool newValue) {
  m_options.setConfirmChoise(newValue);
  checkMenuItem(this,ID_OPTIONS_CONFIRMCHOISE,newValue);
}

void CPartyMakerDlg::setVolume(int newValue) {
  CSliderCtrl *slider = getVolumeControl();
  int minVolume, maxVolume;
  slider->GetRange(minVolume, maxVolume);
  newValue = minMax(newValue, minVolume, maxVolume);
  m_player.GetSettings().SetVolume(newValue);
  m_options.setVolume(newValue);
  slider->SetPos(newValue);
}

int CPartyMakerDlg::getVolume() {
  return getVolumeControl()->GetPos();
}

void CPartyMakerDlg::OnFileNextNumber() {
  if(isPasswordProtected()) {
    return;
  }
  startNextNumber();
}

void CPartyMakerDlg::OnFilePause() {
  if(isPasswordProtected()) {
    return;
  }

  if(isMenuItemChecked(this,ID_FILE_PAUSE)) {
    if(getPlayerState() == wmppsPaused) {
      startPlayer();
    }
    ajourPauseButton(wmppsPlaying);
  } else {
    if(getPlayerState() == wmppsPlaying) {
      pausePlayer();
    }
    ajourPauseButton(wmppsPaused);
  }
}

void CPartyMakerDlg::OnFileEditOrder() {
  CChangeOrderDlg dlg(*m_mediaQueue);
  if(dlg.DoModal() == IDOK) {
    syncMediaQueue();
  }
}

void CPartyMakerDlg::OnFilePasswordProtect() {
  if(isPasswordProtected()) {
    PasswordDlg dlg;
    if(dlg.DoModal() == IDOK) {
      setPasswordProtection(false);
    }
  } else {
    setPasswordProtection(true);
  }
}

void CPartyMakerDlg::OnFileGeneratePlaylistFromDir() {
  CSelectDirAndListNameDlg dirDlg(m_options);
  if(dirDlg.DoModal() != IDOK) {
    return;
  }
  CWMPPlaylist &playList = dirDlg.getPlayList();
  const MediaArray &mediaArray = dirDlg.getMediaArray();
  const size_t n = mediaArray.size();
  for(size_t i = 0; i < n; i++) {
    playList.appendItem(mediaArray[i].getMedia());
  }
}

void CPartyMakerDlg::OnFileExit() {
  stopTimer();
  if(getPlayerState() != wmppsStopped) {
    stopPlayer();
  }
  m_mediaArray.clear();
  m_allMedia.DeleteAllItems();
  MediaDatabase::close();
  PostMessage(WM_QUIT);
}

void CPartyMakerDlg::OnClose() {
  OnFileExit();
}

void CPartyMakerDlg::OnCancel() {
}

void CPartyMakerDlg::OnEditCopy() {
  const int selected = getSelectedIndex();
  if(selected >= 0) {
    try {
      clipboardDropFile(m_hWnd,m_mediaArray[selected].getSourceURL());
    } catch(Exception e) {
      Message(_T("clipboardDropFiles failed:%s"),e.what());
    }
  }
}

void CPartyMakerDlg::OnEditDelete() {
  const int selected = getSelectedIndex();
  if(selected < 0) {
    return;
  }
  PasswordDlg dlg;
  if(dlg.DoModal() != IDOK) {
    return;
  }

  MediaFile &mediaFile = m_mediaArray[selected];

  if(moveFileToTrashCan(m_hWnd, mediaFile.getSourceURL())) {
    m_mediaArray.wait();

    removeMediaFile(selected);
    MediaFile &ff = (*m_newMediaArray)[selected];
    m_newMediaArray->removeIndex(selected);
    setSelectedIndex(m_allMedia, selected);

    m_mediaArray.notify();
  }
}

void CPartyMakerDlg::OnEditModifyInfo() {
  const int selectedIndex = getSelectedIndex();
  if(selectedIndex < 0) {
    return;
  }
  MediaFile tmp = m_mediaArray[selectedIndex];
  m_player.GetSettings().SetEnableErrorDialogs(TRUE);
  CEditTagDlg dlg(tmp, this);
  if(dlg.DoModal() == IDOK) {
    stopTimer();

    m_mediaArray.wait();

    removeMediaFile(selectedIndex);
    const int newIndex = m_newMediaArray->findBySourceURL(tmp.getSourceURL());
    if(newIndex >= 0) {
      (*m_newMediaArray)[newIndex] = tmp;
    }
    setSelectedIndex(m_allMedia, insertMediaFile(tmp));

    m_mediaArray.notify();

    startTimer();
  }
}

void CPartyMakerDlg::OnEditShowInfo() {
  const int selected = getSelectedIndex();
  if(selected < 0) {
    return;
  }
  MediaFile tmp = m_mediaArray[selected];
  showInfo(tmp);
}

void CPartyMakerDlg::OnEditShowLog() {
  showFile(makeLogFileName());
}

void CPartyMakerDlg::OnEditShowErrors() {
  showFile(makeErrorFileName());
}

void CPartyMakerDlg::OnEditFind() {
  OnButtonSearch();
}

void CPartyMakerDlg::OnOptionsBalance() {
  CPlayerSliderControlDlg dlg(m_player, CONTROL_BALANCE);
  dlg.DoModal();
}

void CPartyMakerDlg::OnOptionsSpeed() {
  CPlayerSliderControlDlg dlg(m_player, CONTROL_SPEED);
  dlg.DoModal();
}

void CPartyMakerDlg::OnOptionsKatalog() {
  const String old = m_options.getDirList();
  CMusicDirDlg dlg(m_options);
  if((dlg.DoModal() == IDOK) && (m_options.getDirList() != old)) {
    OnOptionsRescanCatalog();
  }
}

void CPartyMakerDlg::OnOptionsRescanCatalog() {
  m_lastRefresh = 0;
  m_newMediaArray->rescan(m_options.getDirList());
}

void CPartyMakerDlg::OnOptionsAutoSelect() {
  setAutoSelect(!m_options.getAutoSelect());
}

void CPartyMakerDlg::OnOptionsAllowDuplicates() {
  setAllowDuplicates(!m_options.getAllowDuplicates());
}

void CPartyMakerDlg::OnOptionsConfirmChoise() {
  setConfirmChoise(!m_options.getConfirmChoise());
}

void CPartyMakerDlg::OnOptionsMaxChoise() {
  MaxChoiseDlg dlg(m_options.getMaxChoise());
  if(dlg.DoModal() == IDOK) {
    m_options.setMaxChoise(dlg.m_maxChoise);
  }
}

void CPartyMakerDlg::OnOptionsColorsBackground() {
  COLORREF c = m_options.getBackgroundColor();
  if(colorEdit(c)) {
    m_options.setBackgroundColor(c);
    Invalidate();
  }
}

void CPartyMakerDlg::OnOptionsColorsCurrentTrack() {
  COLORREF c = m_options.getCurrentTrackColor();
  if(colorEdit(c)) {
    m_options.setCurrentTrackColor(c);
    Invalidate();
  }
}

void CPartyMakerDlg::OnOptionsColorsMediaQueue() {
  COLORREF c = m_options.getMediaQueueColor();
  if(colorEdit(c)) {
    m_options.setMediaQueueColor(c);
    Invalidate();
  }
}

void CPartyMakerDlg::OnOptionsChangePassword() {
  CChangePasswordDlg dlg;
  dlg.DoModal();
}

void CPartyMakerDlg::OnHelpAboutPartymaker() {
  CAboutDlg dlg;
  dlg.DoModal();
}

void CPartyMakerDlg::OnContextMenu(CWnd *pWnd, CPoint point) {
  if(isPasswordProtected()) {
    return;
  }
  if(pWnd == getCurrentTrackWnd()) {
    if(m_currentMediaFile.isDefined()) {;
      showInfo(m_currentMediaFile);
    }
    return;
  }
  int index = findMediaFileInAllMedia(point);
  if(index < 0) {
    CPoint wp = point;
    ScreenToClient(&wp);
    if((m_selectedFromMediaQueue = findMediaFileInMediaQueue(wp)) < 0) {
      return;
    }
    if(m_selectedFromMediaQueue >= (int)m_mediaQueue->size()) {
      return;
    }
    CMenu menu;
    int ret = menu.LoadMenu(IDR_CONTEXTMENU);
    if(!ret) {
      Message(_T("Loadmenu failed"));
      return;
    }
    menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x,point.y, this);
    return;
  }
  showInfo(m_mediaArray[index]);
}

void CPartyMakerDlg::OnContextMenuInfo() {
  showInfo(m_mediaQueue->peek(m_selectedFromMediaQueue));
}

void CPartyMakerDlg::showInfo(const MediaFile &f) {
  CInfoDlg dlg(f);
  dlg.DoModal();
}

void CPartyMakerDlg::OnContextMenuRemove() {
  m_mediaQueue->get(m_selectedFromMediaQueue);
  showMediaQueue();
}

void CPartyMakerDlg::OnMove(int x, int y) {
  if(isPasswordProtected()) {
    return;
  }
  CDialog::OnMove(x, y);
}

int CPartyMakerDlg::searchPrefix(const String &prefix) const {
  const size_t l  = prefix.length();
  const TCHAR *ps = prefix.cstr();
  for(size_t i = 0; i < m_mediaArray.size(); i++) {
    if(strneaicmp(m_mediaComparator.getCompareString(m_mediaArray[i]),ps,l) == 0) {
      return (int)i;
    }
  }
  return -1;
}

void CPartyMakerDlg::findStringInMediaList(char ch) {
  time_t now;
  time(&now);
  String newPrefix;
  if((now - m_prefixTime < 5) || (m_prefixTime == 0)) {
    if(ch == VK_BACK) {
      const size_t len = m_currentPrefix.length();
      if(len > 0) {
        newPrefix = left(m_currentPrefix, len-1);
      }
    } else {
      newPrefix = m_currentPrefix + String(ch);
    }
  } else {
    newPrefix = String(ch);
  }
  if(newPrefix.length() == 0) {
    m_currentPrefix = newPrefix;
  } else {
    const int index = searchPrefix(newPrefix);
    if(index >= 0) {
      setSelectedIndex(m_allMedia, index);
      m_currentPrefix = newPrefix;
    }
  }
  m_prefixTime = now;
}

bool isPrintable(BYTE ch) {
  _TUCHAR tch = String::upperCaseAccentTranslate[ch];
  return _istprint(tch) || (ch == _T(' '));
}

BOOL CPartyMakerDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    m_currentPrefix = EMPTYSTRING;
    return true;
  }

  switch(pMsg->message) {
  case WM_KEYDOWN:
    switch(pMsg->wParam) {
    case VK_DOWN :
    case VK_UP   :
    case VK_HOME :
    case VK_END  :
    case VK_NEXT :
    case VK_PRIOR:
      m_currentPrefix = EMPTYSTRING;
      if(GetFocus() != &m_allMedia) {
        gotoToListBox();
        return true;
      }
      break;

    default:
      { const BYTE ch = toAscii((UINT)pMsg->wParam);
        if(isPrintable(ch) || (ch == VK_BACK)) {
          findStringInMediaList(ch);
          return true;
        } else {
          m_currentPrefix = EMPTYSTRING;
        }
      }
      break;
    }
    break;

  case WM_LBUTTONDOWN:
    { CPoint pt = pMsg->pt;
      ScreenToClient(&pt);
      if(OnLButtonDown((UINT)(pMsg->wParam), pt)) {
        return true;
      }
    }
    break;

  case WM_LBUTTONUP:
    { CPoint pt = pMsg->pt;
      ScreenToClient(&pt);
      if(OnLButtonUp((UINT)(pMsg->wParam), pt)) {
        return true;
      }
    }
    break;

  case WM_MOUSEMOVE:
    { CPoint pt = pMsg->pt;
      ScreenToClient(&pt);
      if(OnMouseMove((UINT)(pMsg->wParam), pt)) {
        return true;
      }
    }
    break;

  case WM_MOUSEWHEEL:
    if(pMsg->wParam & MK_CONTROL) {
      short zDelta = (short)(pMsg->wParam >> 16);
      setVolume(getVolume() + ((zDelta>0) ? 2 : -2));
      return true;
    }
    setSelectedIndex(m_allMedia, -1);
    break;
  }
  return CDialog::PreTranslateMessage(pMsg);
}

void CPartyMakerDlg::OnButtonPause() {
  OnFilePause();
}

void CPartyMakerDlg::OnButtonNext() {
  OnFileNextNumber();
}

bool CPartyMakerDlg::colorEdit(COLORREF &c) {
  CColorDialog dlg;
  dlg.m_cc.Flags |= CC_RGBINIT;
  dlg.m_cc.rgbResult = c;
  if(dlg.DoModal() == IDOK) {
    c = dlg.m_cc.rgbResult;
    return true;
  }
  return false;
}

void CPartyMakerDlg::OnShowCount() {
  const UINT   duration       = (UINT)m_mediaArray.getDuration();
  const String durationString = format(_T("%d:%02d:%02d"), duration/3600, (duration/60)%60, duration%60);
  MessageBox(format(_T("Der er %d numre i kartoteket\r\n%s Timer")
                   ,(int)m_mediaArray.size()
                   ,durationString.cstr()
                   ).cstr()
            ,_T("Total")
            ,MB_ICONINFORMATION);
}

int CPartyMakerDlg::findMediaFileInMediaQueue(const CPoint &point) {
  CWnd *v = getQueueWnd();
  WINDOWPLACEMENT wpl;
  v->GetWindowPlacement(&wpl);
  if(!CRect(wpl.rcNormalPosition).PtInRect(point)) {
    return -1;
  }
  int index = (point.y - wpl.rcNormalPosition.top) / CHARHEIGHT;
  if(index < 0) {
    return -1;
  }
  if(index > (int)m_mediaQueue->size()) {
    return (int)m_mediaQueue->size();
  }
  return index;
}

bool CPartyMakerDlg::pointInCurrentTrackWindow(CPoint point) {
  CWnd *v = getCurrentTrackWnd();
  WINDOWPLACEMENT wpl;
  v->GetWindowPlacement(&wpl);
  return point.x >= wpl.rcNormalPosition.left && point.x <= wpl.rcNormalPosition.right
      && point.y >= wpl.rcNormalPosition.top  && point.y <= wpl.rcNormalPosition.bottom;
}

void CPartyMakerDlg::setMoveCursor(bool on) {
  if(on) {
    setWindowCursor(getQueueWnd()                , IDC_CURSORMOVE);
    setWindowCursor(getCurrentTrackWnd()         , IDC_CURSORMOVE);
    setWindowCursor(this                         , IDC_NO        );
    setWindowCursor(&m_allMedia                  , IDC_NO        );
    setWindowCursor(GetDlgItem(IDC_SEARCHBUTTON) , IDC_NO        );
    setWindowCursor(GetDlgItem(IDC_PAUSEBUTTON ) , IDC_NO        );
    setWindowCursor(GetDlgItem(IDC_NEXTBUTTON  ) , IDC_NO        );
    setWindowCursor(GetDlgItem(IDC_VOLUMESLIDER) , IDC_NO        );
    setFlag(MOVECURSORON);
  } else {
    setWindowCursor(getQueueWnd()                , IDC_ARROW     );
    setWindowCursor(getCurrentTrackWnd()         , IDC_ARROW     );
    setWindowCursor(this                         , IDC_ARROW     );
    setWindowCursor(&m_allMedia                  , IDC_ARROW     );
    setWindowCursor(GetDlgItem(IDC_SEARCHBUTTON) , IDC_ARROW     );
    setWindowCursor(GetDlgItem(IDC_PAUSEBUTTON ) , IDC_ARROW     );
    setWindowCursor(GetDlgItem(IDC_NEXTBUTTON  ) , IDC_ARROW     );
    setWindowCursor(GetDlgItem(IDC_VOLUMESLIDER) , IDC_ARROW     );
    clrFlag(MOVECURSORON);
  }
}

bool CPartyMakerDlg::OnLButtonDown(UINT nFlags, CPoint point) {
  if(isPasswordProtected()) {
    return false;
  }
  m_selectedFromMediaQueue = findMediaFileInMediaQueue(point);
  if(m_selectedFromMediaQueue >= (int)m_mediaQueue->size()) {
    m_selectedFromMediaQueue = -1;
  }
  if(m_selectedFromMediaQueue >= 0) {
    setMoveCursor(true);
    return true;
  }
  return false;
}

bool CPartyMakerDlg::OnLButtonUp(UINT nFlags, CPoint point) {
  if(isFlagSet(MOVECURSORON)) {
    setMoveCursor(false);
  }
  if(m_selectedFromMediaQueue < 0) {
    return false;
  }
  if(pointInCurrentTrackWindow(point)) {
    m_mediaQueue->move(m_selectedFromMediaQueue,0);
    startNextNumber();
    syncMediaQueue();
    return true;
  } else {
    int newPosition = findMediaFileInMediaQueue(point);
    m_mediaQueue->move(m_selectedFromMediaQueue,newPosition);
    syncMediaQueue();
    return true;
  }
}

bool CPartyMakerDlg::OnMouseMove(UINT nFlags, CPoint point) {
  if(!(nFlags & MK_LBUTTON)) {
    setMoveCursor(false);
    m_selectedFromMediaQueue = -1;
  }
  return false;
}

void CPartyMakerDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar) {
  setVolume(getVolumeControl()->GetPos());
  CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
