#pragma once

#include <MFCUtil/GifCtrl.h>

class CTestGIFDlg : public CDialog {
private:
  SimpleLayoutManager m_layoutManager;
  CGifCtrl            m_gif;
  int                 m_frameIndex;
  void updateState();
  void loadGifResource(int resId);
public:

    CTestGIFDlg(CWnd *pParent = nullptr);

    enum { IDD = IDD_TESTGIF_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnButtonLoadGif();
    afx_msg void OnButtonLoadResource();
    afx_msg void OnButtonLoadResourceError();
    afx_msg void OnButtonStart();
    afx_msg void OnButtonStop();
    afx_msg void OnButtonUnload();
    afx_msg void OnButtonPaintAll();
    afx_msg void OnButtonPaintFirst();
    afx_msg void OnButtonPaintNext();
    afx_msg void OnButtonPaintPrev();
    afx_msg void OnButtonClear();
    afx_msg void OnButtonHide();
    DECLARE_MESSAGE_MAP()
};

