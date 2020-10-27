#pragma once

#include <MFCUtil/Picture.h>

class CMakeAviDlg : public CDialog {
private:
    HICON               m_hIcon;
    HACCEL              m_accelTable;
    SimpleLayoutManager m_layoutManager;
    CPicture            m_currentPicture;
    StringArray         m_nameArray;
    int getSelectedIndex();
    void loadPicture(const String &fileName);
    void unloadPicture();
    void updatePreview();
    void updateWindowState();
    void scrollLines(int count);
public:
    CMakeAviDlg(CWnd *pParent = nullptr);

    enum { IDD = IDD_MAKEAVI_DIALOG };
    CListCtrl   m_nameList;
    UINT        m_framePerSecond;
    UINT        m_useEvery;

    virtual BOOL PreTranslateMessage(MSG *pMsg);
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnPaint();
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnClose();
    afx_msg void OnFileAddfFiles();
    afx_msg void OnFileExit();
    afx_msg void OnEditDeleteSelected();
    afx_msg void OnEditClearList();
    afx_msg void OnHelpAbout();
    afx_msg void OnButtonReadAVI();
    afx_msg void OnButtonMakeAVI();
    afx_msg void OnItemChangedListNames(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnChangeEditFramePerSec();
    afx_msg void OnChangeEditUseEvery();
    DECLARE_MESSAGE_MAP()
};

