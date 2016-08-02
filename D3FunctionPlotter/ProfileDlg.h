#pragma once

#include <MFCUtil/Viewport2D.h>
#include "DrawTool.h"

class CProfileDlg : public CDialog, public ViewportContainer {
private:
  HACCEL                    m_accelTable;
  LOGFONT                   m_logFont;
  CDC                       m_workDC;
  CBitmap                   m_workBitmap;
  CRect                     m_workRect;
  D3Scene                   m_scene;
  D3SceneObject            *m_3DObject;
  Viewport2D               *m_viewport;
  DrawTool                 *m_currentDrawTool;
  int                       m_currentControl;
  Profile                   m_profile;
  Profile                   m_lastRotatedProfile;
  ProfileRotationParameters m_lastRotationParameters;
  Profile                   m_lastSavedProfile;
  int                       m_last3dmode;
  CBitmap                   m_testBitmap;
  void createWorkBitmap();
  void destroyWorkBitmap();
  void initViewport();
  void destroyViewport();
  void checkToolItem(int id);
  void setCurrentDrawToolId(int id);
  void setCurrentDrawTool(DrawTool *newDrawTool);
  void refresh2d();
  void repaint2d(CDC &dc);
  void refresh3d();
  void repaint3d();
  void rotateProfile();
  void stretchProfile();
  void repaint();
  void updateAndRefresh();
  void resetView();
  void saveAs();
  void save(const String &fileName);
  void set3DObject(D3SceneObject *obj);
  void showSliderPos();
  ProfileRotationParameters getRotateParameters();
  ProfileStretchParameters  getStretchParameters();
  CPoint getRelativePoint(int id, const CPoint &p);
  int getControlAtPoint(const CPoint &point);
  void clipCursor();
  bool isDirty() const {
    return m_profile != m_lastSavedProfile;
  }
  bool dirtyCheck();
  void showMousePosition(const CPoint &p);
public:
  CProfileDlg(const Profile &profile, CWnd* pParent = NULL);
 ~CProfileDlg();
  void setProfileName(const String &name);

// ---------------------------------- ViewportContainer interface functions----------------
  Viewport2D &getViewport() {
    return *m_viewport;
  }
  Profile &getProfile() {
    return m_profile;
  }
  HWND getWindow() {
    return m_hWnd;
  }
  void repaintScreen();
  int  showNormals();
  bool showPoints();
  void setMousePosition(const Point2D &p);
// ------------------------------------------------------------

  bool validate();
  void paintAll();
  void paintPoints(Viewport2D &vp);
  void paintBox(Viewport2D &vp, const Point2D &point, CBrush &brush);

  enum { IDD = IDD_PROFILE_DIALOG };
  double    m_degree;
  BOOL      m_normalSmooth;
  BOOL      m_rotateSmooth;
  int       m_edgeCount;
  int       m_rotateAxis;
  int       m_3dmode;


  public:
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);

protected:

    virtual BOOL OnInitDialog();
    virtual void OnCancel();
    virtual void OnOK();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnPaint();
    afx_msg void OnFileNew();
    afx_msg void OnFileOpen();
    afx_msg void OnFileSave();
    afx_msg void OnFileSaveAs();
    afx_msg void OnFileSelectFromFont();
    afx_msg void OnEditCut();
    afx_msg void OnEditCopy();
    afx_msg void OnEditPaste();
    afx_msg void OnEditDelete();
    afx_msg void OnEditConnect();
    afx_msg void OnEditInvertnormals();
    afx_msg void OnEditMirrorHorizontal();
    afx_msg void OnEditMirrorVertical();
    afx_msg void OnViewShowPoints();
    afx_msg void OnViewShowNormals();
    afx_msg void OnViewAutoUpdate3D();
    afx_msg void OnToolsLine();
    afx_msg void OnToolsBezierCurve();
    afx_msg void OnToolsRectangle();
    afx_msg void OnToolsPolygon();
    afx_msg void OnToolsEllipse();
    afx_msg void OnToolsSelect();
    afx_msg void OnRadioRotate();
    afx_msg void OnRadioStretch();
    afx_msg void OnRadioRotatexaxis();
    afx_msg void OnRadioRotateyaxis();
    afx_msg void OnButtonRefresh();
    afx_msg void OnCheckRotateSmooth();
    afx_msg void OnCheckNormalSmooth();
    afx_msg void OnSetfocusEditDegrees();
    afx_msg void OnKillfocusEditDegrees();
    afx_msg void OnKillfocusEditEdgeCount();
    afx_msg void OnSetfocusEditEdgeCount();
    DECLARE_MESSAGE_MAP()
};

Profile *selectAndLoadProfile();
extern const TCHAR *profileFileExtensions;

