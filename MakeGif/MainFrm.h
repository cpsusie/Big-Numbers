#pragma once

#include <PropertyContainer.h>
#include "MakeGifdoc.h"
#include "MakeGifView.h"

class CMainFrame : public CFrameWnd, public PropertyChangeListener {
private:
  CStatusBar  m_wndStatusBar;
  CToolBar    m_wndToolBar;
  ShowFormat  m_currentShowFormat;
  bool        m_registeredAsListener;
  bool        m_closePending;
  void onFileMruFile(int index);
  bool load(const String &name);
  void save(const String &name);
  void setTitle();
  void setShowFormat(ShowFormat format);
  void showAllImages();
  bool confirmDiscard();
  void registerAsListener();
  void unregisterAsListener();
protected:
  CMainFrame();
  DECLARE_DYNCREATE(CMainFrame)

public:
    virtual ~CMainFrame();

    CMakeGifDoc *getDoc() {
      return (CMakeGifDoc*)GetActiveDocument();
    }
    CMakeGifView *getView() {
      return (CMakeGifView*)GetActiveView();
    }

    void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
    void ajourMenuItems();
    inline ShowFormat getShowFormat() const {
      return m_currentShowFormat;
    }
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnClose();
    afx_msg void OnAppExit();
    afx_msg void OnFileNew();
    afx_msg void OnFileOpen();
    afx_msg void OnFileCloseGiffile();
    afx_msg void OnFileMruFile1();
    afx_msg void OnFileMruFile2();
    afx_msg void OnFileMruFile3();
    afx_msg void OnFileMruFile4();
    afx_msg void OnFileMruFile5();
    afx_msg void OnFileMruFile6();
    afx_msg void OnFileMruFile7();
    afx_msg void OnFileMruFile8();
    afx_msg void OnFileMruFile9();
    afx_msg void OnFileMruFile10();
    afx_msg void OnFileSave();
    afx_msg void OnFileSaveAs();
    afx_msg void OnEditInserIimages();
    afx_msg void OnEditRemoveAllImages();
    afx_msg void OnEditAddToGif();
    afx_msg void OnEditSettinsgs();
    afx_msg void OnViewPlay();
    afx_msg void OnViewStop();
    afx_msg void OnViewShowAllImages();
    afx_msg void OnViewShowRawPictures();
    afx_msg void OnViewShowScaledPictures();
    afx_msg void OnViewShowQuantizedPictures();
    DECLARE_MESSAGE_MAP()
};
