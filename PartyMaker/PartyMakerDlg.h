#include "wmpplayer4.h"
#pragma once

typedef enum {
  wmppsUndefined      = 0
 ,wmppsStopped        = 1
 ,wmppsPaused         = 2
 ,wmppsPlaying        = 3
 ,wmppsScanForward    = 4
 ,wmppsScanReverse    = 5
 ,wmppsBuffering      = 6
 ,wmppsWaiting        = 7
 ,wmppsMediaEnded     = 8
 ,wmppsTransitioning  = 9
 ,wmppsReady          = 10
 ,wmppsReconnecting   = 11
 ,wmppsLast           = 12
} WMPPlayState;

class CPartyMakerDlg : public CDialog {
private:
  HACCEL              m_accelTable;
  HICON               m_hIcon;
  Options             m_options;
  LoadableMediaArray *m_newMediaArray;
  MediaArray          m_mediaArray;
  MediaQueue         *m_mediaQueue;
  CFont               m_queueFont;
  CBitmap             m_currentTrackBitmap;
  CDC                 m_currentTrackDC;
  CRect               m_currentTrackRect;
  MediaComparator     m_mediaComparator;
  friend int CALLBACK compareMediaItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

  String              m_currentPrefix;
  time_t              m_prefixTime;
  time_t              m_lastRefresh;
  int                 m_selectedFromMediaQueue;
  MediaFile           m_currentMediaFile;
  bool                m_moveCursorOn;
  bool                m_pauseButtonIsPause;
  bool                m_timerIsRunning;
  bool                m_editOrderEnabled;

  void         startTimer();
  void         stopTimer();
  int          insertMediaFile(MediaFile &f);
  void         removeMediaFile(intptr_t index);
  void         resetMediaList();
  int          getSelectedIndex();
  int          getMediaArraySize();
  WMPPlayState getPlayerState();
  double       getPlayerPosition();
  CString      getPlayerPositionString();
  void         startPlayer();
  void         pausePlayer();
  void         stopPlayer();
  void         ajourPauseButton(WMPPlayState state);

  int          findMediaFileInAllMedia(  const CPoint &point);
  int          findMediaFileInMediaQueue(const CPoint &point);
  bool         pointInCurrentTrackWindow(CPoint point);
  void         findStringInMediaList(char ch);
  int          searchPrefix(const String &prefix) const;
  void         syncMediaList();
  void         showMediaList();
  void         showHeaderSortMark();
  void         sortMediaList(int headerIndex);
  void         showMediaQueue();
  void         enableEditOrderItem(bool enable);
  void         syncMediaQueue(bool shiftToNext = false);
  void         showCurrentTrack(WMPPlayState state);
  void         clearCurrentTrack(CDC *pdc = NULL);
  void         startNextNumber();
  bool         appendToMediaQueue(const MediaFile &f, bool silence = false);
  void         showInfo(const MediaFile &f);
  void         setPasswordProtection(bool on);
  bool         isPasswordProtected();
  void         setAutoSelect(     bool newValue);
  void         setAllowDuplicates(bool newValue);
  void         setConfirmChoise(  bool newValue);
  CSliderCtrl *getVolumeControl() {
    return (CSliderCtrl*)GetDlgItem(IDC_VOLUMESLIDER);
  }
  void         setVolume(         int  newValue); // 0..100
  int          getVolume();
  void         gotoToListBox();
  CWnd        *getCurrentTrackWnd();
  CWnd        *getQueueWnd();
  bool         colorEdit(COLORREF &c);
  void         setMoveCursor(bool on);
  bool         OnLButtonDown(UINT nFlags, CPoint point);
  bool         OnLButtonUp(  UINT nFlags, CPoint point);
  bool         OnMouseMove(  UINT nFlags, CPoint point);

  enum { IDD = IDD_MAINDIALOG };
  CListCtrl   m_allMedia;
  CWMPPlayer4 m_player;

public:
  CPartyMakerDlg(CWnd *pParent = NULL);
  const MediaArray &getMediaArray() const {
    return m_mediaArray;
  }

public:
    virtual BOOL PreTranslateMessage(MSG *pMsg);
protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnPaint();
    afx_msg void OnContextMenu(CWnd *pWnd, CPoint point);
    afx_msg void OnMove(int x, int y);
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnClose();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
    afx_msg void OnFileNextNumber();
    afx_msg void OnFileEditOrder();
    afx_msg void OnFilePause();
    afx_msg void OnFilePasswordProtect();
    afx_msg void OnFileGeneratePlaylistFromDir();
    afx_msg void OnFileExit();
    afx_msg void OnEditCopy();
    afx_msg void OnEditDelete();
    afx_msg void OnEditModifyInfo();
    afx_msg void OnEditShowInfo();
    afx_msg void OnEditShowLog();
    afx_msg void OnEditShowErrors();
    afx_msg void OnEditFind();
    afx_msg void OnOptionsBalance();
    afx_msg void OnOptionsSpeed();
    afx_msg void OnOptionsKatalog();
    afx_msg void OnOptionsRescanCatalog();
    afx_msg void OnOptionsAutoSelect();
    afx_msg void OnOptionsAllowDuplicates();
    afx_msg void OnOptionsConfirmChoise();
    afx_msg void OnOptionsMaxChoise();
    afx_msg void OnOptionsColorsCurrentTrack();
    afx_msg void OnOptionsColorsMediaQueue();
    afx_msg void OnOptionsColorsBackground();
    afx_msg void OnOptionsChangePassword();
    afx_msg void OnHelpAboutPartymaker();
    afx_msg void OnButtonSearch();
    afx_msg void OnButtonPause();
    afx_msg void OnButtonNext();
    afx_msg void OnShowCount();
    afx_msg void OnDblClkAllMedia(     NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnColumnClickAllMedia(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnItemChangedAllMedia(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnContextMenuRemove();
    afx_msg void OnContextMenuInfo();
    afx_msg void OnSortByTitle();
    afx_msg void OnSortByArtist();
    afx_msg void OnSortByAlbum();
    DECLARE_MESSAGE_MAP()
};

