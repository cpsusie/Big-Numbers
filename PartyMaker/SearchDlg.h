#pragma once

#include <Thread.h>
#include <SynchronizedQueue.h>
#include <BMAutomate.h>

class SearchDlg;

class SearchThread : public Thread {
private:
  SearchDlg                &m_searchDlg;
  SynchronizedQueue<String> m_requestQueue;
  BMAutomate                m_BMAutomate;
  int                       m_state;
  void doSearch(const String &request);
  void addToResult(bool &firstInsert, int index);
public:
  SearchThread(SearchDlg &searchDlg);
  UINT run();
  void startSearch(const String &searchText);
  void stop();
};

class SearchDlg : public CDialog {
private:
  const MediaArray &m_mediaArray;
  HACCEL           m_accelTable;
  MediaArray       m_searchResult;
  MediaArray       m_selected;
  SearchThread    *m_workerThread;
  bool             m_timerIsRunning;
  CString          m_lastSearchText; // updated every 500 msec
  const bool       m_showMenu;
  void clearResult();
  void addToResult(const MediaFile &f);
  MediaArray &findSelected(MediaArray &selected); // return selected
  void gotoEditBox();
  void gotoListBox();
  CListBox *getListBox();
  CEdit    *getEditBox();
  void startRescan();
  void startTimer();
  void stopTimer();
  void destroyWorkerThread();
public:
  SearchDlg(const MediaArray &mediaArray, bool showMenu, CWnd *pParent = NULL);
  ~SearchDlg();

  const MediaArray &getSelected() const {
    return m_selected;
  }

  const MediaArray &getMediaArray() const {
    return m_mediaArray;
  }

  enum { IDD = IDD_SEARCHDIALOG };
  CString   m_searchText;

public:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
protected:
  virtual void DoDataExchange(CDataExchange *pDX);

protected:

    afx_msg void OnChoose();
    afx_msg void OnCancel();
    afx_msg void OnOK();
    afx_msg void OnDblclkListResult();
    virtual BOOL OnInitDialog();
    afx_msg void OnGotoSearchText();
    afx_msg void OnEditCopy();
    afx_msg LRESULT OnAddMediaFile(WPARAM wp, LPARAM lp);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnEditModifyInfo();
    afx_msg void OnSelChangeListResult();
    afx_msg void OnFindDuplicates();
  DECLARE_MESSAGE_MAP()
};

