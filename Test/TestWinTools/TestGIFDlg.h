#pragma once

#include <MyUtil.h>
#include <MFCUtil/LayoutManager.h>
#include <MFCUtil/GifCtrl.h>

class CTestGIFDlg : public CDialog {
private:
  SimpleLayoutManager m_layoutManager;
  CGifCtrl            m_gif;
  int                 m_frameIndex;
  void updateState();
  void loadGifResource(int resId);
public:
   
    CTestGIFDlg(CWnd* pParent = NULL);

    //{{AFX_DATA(CTestGIFDlg)
    enum { IDD = IDD_TESTGIF_DIALOG };
    //}}AFX_DATA

    //{{AFX_VIRTUAL(CTestGIFDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    //}}AFX_VIRTUAL

protected:
    //{{AFX_MSG(CTestGIFDlg)
    virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnButtonLoadGif();
	afx_msg void OnButtonLoadResouce();
	afx_msg void OnButtonLoadResroucEerror();
	afx_msg void OnButtonStart();
	afx_msg void OnButtonStop();
	afx_msg void OnButtonUnload();
	afx_msg void OnButtonPaintAll();
	afx_msg void OnButtonPaintFirst();
	afx_msg void OnButtonPaintNext();
	afx_msg void OnButtonPaintPrev();
	afx_msg void OnButtonClear();
	afx_msg void OnButtonHide();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
