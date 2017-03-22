#pragma once

class CMainFrame : public CFrameWnd {
protected: // create from serialization only
    CMainFrame();
    DECLARE_DYNCREATE(CMainFrame)

public:

    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
    virtual ~CMainFrame();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext &dc) const;
#endif

protected:  // control bar embedded members
    CStatusBar  m_wndStatusBar;
    CToolBar    m_wndToolBar;

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    DECLARE_MESSAGE_MAP()
};
