#pragma once

#include <Thread.h>
#include <SynchronizedQueue.h>

class CShowDuplicatesDlg;

class FindDuplicatesThread : public Thread {
private:
  CShowDuplicatesDlg &m_dlg;
  bool                m_stop;
  void showMessage(const TCHAR *format,...);
public:
  FindDuplicatesThread(CShowDuplicatesDlg &dlg);
  UINT run();

  void stop() {
    m_stop = true;
  }
};

class CShowDuplicatesDlg : public CDialog {
private:
    HACCEL                    m_accelTable;
    SimpleLayoutManager       m_layoutManager;
    MediaArray                m_mediaArray;
    MediaArray                m_duplicateArray;
    SynchronizedQueue<String> m_stateMessageQueue;
    bool                      m_changed;
    bool                      m_timerIsRunning;
    FindDuplicatesThread     *m_workerThread;

    int  getSelectedIndex();
    void removeMediaFile(int listIndex, const String &sourceURL);
    int  findBySourceURL(const String &sourceURL) const; // find index of mediafile in m_duplicateArray
    void showDuplicateList();
    void showMediaArray(const MediaArray &mediaArray);
    void addMediaFile(int i, const MediaFile &f);
    void startTimer();
    void stopTimer();

    friend class FindDuplicatesThread;
public:
    CShowDuplicatesDlg(const MediaArray &mediaArray, CWnd *pParent = NULL);
   ~CShowDuplicatesDlg();

    bool isAnyDeleted() const {
      return m_changed;
    }

    enum { IDD = IDD_SHOWDUPLICATESDIALOG };
    CListCtrl m_duplicateList;
    BOOL      m_filterBySize;
    BOOL      m_filterByDuration;

public:
    virtual BOOL PreTranslateMessage(MSG *pMsg);
protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnClose();
    afx_msg void OnEditDelete();
    afx_msg void OnCheckFilterBySize();
    afx_msg void OnCheckFilterByDuration();
    afx_msg void OnGotoList();
    afx_msg LRESULT OnShowStateMessage(WPARAM wp, LPARAM lp);
    DECLARE_MESSAGE_MAP()
};
