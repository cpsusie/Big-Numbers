#pragma once

#include <MFCUtil/LayoutManager.h>
#include "Image.h"

typedef enum {
  PFF_BMP
 ,PFF_JPG
 ,PFF_PNG
 ,PFF_TIFF
} PictureFileFormat;

class CTestPictureDlg : public CDialog {
private:
  HICON               m_hIcon;
  HACCEL              m_accelTable;
  SimpleLayoutManager m_layoutManager;
  CFont               m_infoFont;
  String              m_origTitle;
  Image               m_image[2];
  int                 m_currentImageIndex;
  int                 m_extensionIndex;
  bool                m_isDragging;
  CPoint              m_lastMouse, m_mouseDown, m_mouseDownOffset;
  int                 m_infoWindowHeight;
  HDC                 m_workDC;
  HBITMAP             m_workBitmap;
  void setCurrentImageIndex(int index);
  int getCurrentImageIndex() const {
    return m_currentImageIndex;
  }
  Image &getCurrentImage() {
    return m_image[getCurrentImageIndex()];
  }
  CPoint getImagePoint(const CPoint &p) const;
  void setDragging(bool on);
  void ajourMenuItems();
  void showInfo();
  void showTitle();
  void savePicture(PictureFileFormat fileFormat);
  void setInfoWindowVisible(bool visible);
  HDC getWorkDC();
  CWnd *getImageWin() {
    return GetDlgItem(IDC_STATICIMAGEFRAME);
  }
  const CWnd *getImageWin() const {
    return GetDlgItem(IDC_STATICIMAGEFRAME);
  }
  CWnd *getInfoWin() {
    return GetDlgItem(IDC_EDITINFO);
  }
public:
    CTestPictureDlg(CWnd *pParent = NULL);

    enum { IDD = IDD_TESTPICTURE_DIALOG };

protected:
    virtual BOOL PreTranslateMessage(MSG *pMsg);
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnClose();
    virtual void OnCancel();
    virtual void OnOK();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(  UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(  UINT nFlags, CPoint point);
    afx_msg BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnFileLoad();
    afx_msg void OnFileUnload();
    afx_msg void OnFileSaveBmp();
    afx_msg void OnFileSaveJpg();
    afx_msg void OnFileSavePng();
    afx_msg void OnFileSaveTiff();
    afx_msg void OnFileExit();
    afx_msg void OnEditCurrent1();
    afx_msg void OnEditCurrent2();
    afx_msg void OnEditPic1pic2();
    afx_msg void OnEditPic2pic1();
    afx_msg void OnEditSwapRedAndBlue();
    afx_msg void OnEditClearLog();
    afx_msg void OnViewKeepAspectRatio();
    afx_msg void OnViewShowDebugInfo();
    afx_msg void OnDropFiles(HDROP hDropInfo);
    DECLARE_MESSAGE_MAP()
};

