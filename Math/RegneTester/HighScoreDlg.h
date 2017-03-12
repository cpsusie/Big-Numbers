#pragma once

class CHighScoreDlg : public CDialog {
private:
  int m_execiszeType;
  void showHighScore(int index);
public:
    CHighScoreDlg(int execiszeType, CWnd *pParent = NULL);

    enum { IDD = IDD_HIGHSCORE_DIALOG };
    CTabCtrl    m_execiszeTab;
    CListCtrl   m_listBestTime;
protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnSelchangeTabExecise(NMHDR* pNMHDR, LRESULT* pResult);
    DECLARE_MESSAGE_MAP()
};

