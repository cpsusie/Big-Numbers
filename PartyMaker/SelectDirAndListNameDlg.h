#pragma once

class CSelectDirAndListNameDlg : public CDialog {
private:
  HACCEL             m_accelTable;
  Options           &m_options;
  CImageList         m_images;
  bool               m_timerIsRunning;
  LoadableMediaArray m_mediaArray;
  CWMPPlaylist       m_playList;

  void       fillTree(const TCHAR *path);
  CString    getSelectedPath();
  void       gotoEditBox(int id);
  bool       validate();
  void       startTimer();
  void       stopTimer();
public:
    CSelectDirAndListNameDlg(Options &options, CWnd *pParent = NULL);
    const MediaArray &getMediaArray() const {
      return m_mediaArray;
    }

    CWMPPlaylist &getPlayList() {
      return m_playList;
    }

    enum { IDD = IDD_SELECTDIRANDLISTNAMEDIALOG };
    CString m_listName;
    CString m_dir;
    CString m_drive;
    BOOL    m_recurseSubDir;


public:
    virtual BOOL PreTranslateMessage(MSG *pMsg);
protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnEditChangeDriveCombo();
    afx_msg void OnGotoListName();
    afx_msg void OnGotoDir();
    afx_msg void OnGotoDrive();
    afx_msg void OnSelChangeDriveCombo();
    afx_msg void OnDblclkDirTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    DECLARE_MESSAGE_MAP()
};

