#pragma once

#include <D3DGraphics/D3SceneContainer.h>
#include <D3DGraphics/D3SceneEditor.h>
#include "ProfileEditor.h"
#include "DrawTool.h"
#include "Resource.h"

#define SC_RENDER2D SC_RENDERUSER(0x01)

class ProfileDialogVariables {
public:
  int                       m_3dmode;
  BOOL                      m_doubleSided;
  ProfileRotationParameters m_rotationParameters;
  Profile2D                 m_profile;
};
bool operator==(const ProfileDialogVariables &v1, const ProfileDialogVariables &v2);
inline bool operator!=(const ProfileDialogVariables &v1, const ProfileDialogVariables &v2) {
  return !(v1 == v2);
}

class ValidationException;

class CProfileDlg : public CDialog
                  , public ProfileEditor
                  , public D3SceneContainer
{
private:
  HACCEL                    m_accelTable;
  LOGFONT                   m_logFont;
  CDC                       m_workDC;
  CBitmap                   m_workBitmap;
  CRect                     m_workRect;
  D3Scene                   m_scene;
  D3SceneEditor             m_editor;
  D3SceneObjectVisual      *m_visual;
  Viewport2D               *m_viewport;
  bool                      m_exceptionRaised;
  DrawTool                 *m_currentDrawTool;
  int                       m_currentControl;
  Profile2D                 m_profile        , m_lastSavedProfile;
  ProfileDialogVariables    m_currentProfVars, m_lastProfVars;
  CBitmap                   m_testBitmap;

  int       m_3dmode;
  BOOL      m_doubleSided;
  float     m_degrees;
  BOOL      m_normalSmooth;
  BOOL      m_rotateSmooth;
  UINT      m_edgeCount;
  CString   m_rotateAxis;
  CString   m_rotateAxisAlignsTo;
  BOOL      m_useColor;

  CMFCColorButton *getColorButton() const;
  void rotateParamToWin(const ProfileRotationParameters &param);
  void rotateWinToParam(      ProfileRotationParameters &param);
  ProfileDialogVariables &getAllProfVars(ProfileDialogVariables &profVars);
  void createWorkBitmap();
  void destroyWorkBitmap();
  void initViewport();
  void destroyViewport();
  void checkToolItem(int id);
  void setCurrentDrawToolId(int id);
  void setCurrentDrawTool(DrawTool *newDrawTool);
  bool needUpdate3DObject();
  void saveCurrentProfVars();
  void create3DObject();
  void destroy3DObject();
  void stretchProfile();
  void repaintProfile();
  void repaintAll();
  bool isAutoUpdate3DChecked() const;
  void showValidateError( const ValidationException &e);
  void clearException();
  void raiseException(const Exception &e);
  bool getUintValue(    int id, UINT   &value);
  bool getUintEmptyZero(int id, UINT   &value);
  void setUintEmptyZero(int id, UINT    value);
  void validate();
  void validateAndUpdate();
  void render2D();
  void render3D();
  void renderAll();
  void enableWindowItems();
  void disableAll();
  void resetView();
  void render(BYTE renderFlags);
  void saveAs();
  void save(const String &fileName);
  void setVisual(D3SceneObjectVisual *visual);
  void showSliderPos();
  CPoint getRelativePoint(int id, const CPoint &p);
  int getControlAtPoint(const CPoint &point);
  void clipCursor();
  bool isDirty() const {
    return m_profile != m_lastSavedProfile;
  }
  bool dirtyCheck();
  void showMousePosition(const CPoint &p);
  D3Device &getDevice();
public:
  CProfileDlg(CWnd *pParent = NULL);
 ~CProfileDlg();
  void setProfileName(const String &name);

// ---------------------------------- ProfileEditor interface functions----------------
  Viewport2D &getViewport() {
    return *m_viewport;
  }
  Profile2D  &getProfile() {
    return m_profile;
  }

  void        checkWorkRectSize();
  void        repaintViewport() { render2D(); }
  NormalsMode getNormalsMode();
  bool        getShowPoints();
  void        setMousePosition(const Point2D &p);

  HWND        getWindow() {
    return *GetDlgItem(IDC_STATIC_PROFILEIMAGE2D);
  }
// ---------------------------------- SceneContainer interface functions----------------
  D3Scene    &getScene() {
    return m_scene;
  }
  // Should return how many 3D-windows (cameras) to create
  UINT        get3DWindowCount() const {
    return 1;
  }
  // Should return the window to receieve messages (from contextmenu)
  HWND        getMessageWindow() const {
    return *this;
  }
  // Assume index = [0..get3DWindowCount()-1].
  // Should return the 3D-window with the given indexed
  HWND        get3DWindow(UINT index) const {
    return *GetDlgItem(IDC_STATIC_PROFILEIMAGE3D);
  }
  void        doRender(BYTE renderFlags, CameraSet cameraSet);

  ProfileRotationParameters getRotateParameters()  const;
  ProfileStretchParameters  getStretchParameters() const;
  bool        isDoubleSided() const;
  enum { IDD = IDR_PROFILE };

public:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  virtual void OnCancel();
  virtual void OnOK();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnLButtonDown(  UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(    UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(    UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(  UINT nFlags, CPoint point);
  afx_msg void OnRButtonUp(    UINT nFlags, CPoint point);
  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
  afx_msg BOOL OnMouseWheel(   UINT nFlags, short zDelta, CPoint pt);
  afx_msg void OnHScroll(      UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
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
  afx_msg void OnButtonRefresh();
  afx_msg void OnCheckDoubleSided();
  afx_msg void OnCheckRotateSmooth();
  afx_msg void OnCheckNormalSmooth();
  afx_msg void OnSetfocusEditDegrees();
  afx_msg void OnKillfocusEditDegrees();
  afx_msg void OnKillfocusEditEdgeCount();
  afx_msg void OnSetfocusEditEdgeCount();
  afx_msg void OnEnChangeEditEdgeCount();
  afx_msg void OnEnChangeEditDegrees();
  afx_msg void OnCbnSelchangeComboRotateAxis();
  afx_msg void OnCbnSelchangeComboRotateAxisAlignsTo();
  afx_msg void OnBnClickedCheckUseColor();
  afx_msg void OnBnClickedMFCColorButton();
  afx_msg void OnDeltaposSpinEdgeCount(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg LRESULT OnMsgRender(              WPARAM wp, LPARAM lp);
  DECLARE_MESSAGE_MAP()
};

Profile2D *selectAndLoadProfile();
extern const TCHAR *profileFileExtensions;
