#pragma once

#include <MFCUtil/LayoutManager.h>
#include <D3DGraphics/Function2DSurface.h>
#include <D3DGraphics/ParametricSurface.h>
#include <D3DGraphics/IsoSurface.h>
#include "D3DGraphics/D3CoordinateSystem.h"
#include "D3DGraphics/D3SceneEditor.h"

#ifdef LOGMEMORY
#include "MemoryLogThread.h"
#endif // LOGMEMORY
#ifdef DEBUG_POLYGONIZER
#include "DebugThread.h"
#endif // DEBUG_POLYGONIZER


class CD3FunctionPlotterDlg : public CDialog
                            , public D3SceneContainer
#ifdef DEBUG_POLYGONIZER
                            , public PropertyChangeListener
#endif // DEBUG_POLYGONIZER
{
private:
    HICON                       m_hIcon;
    HACCEL                      m_accelTable;
    SimpleLayoutManager         m_layoutManager;
    D3Scene                     m_scene;
    D3SceneEditor               m_editor;
#ifdef DEBUG_POLYGONIZER
    DebugThread                *m_debugThread;
#endif // DEBUG_POLYGONIZER
#ifdef LOGMEMORY
    MemoryLogThread             m_memlogThread;
#endif // LOGMEMORY
    Function2DSurfaceParameters m_function2DSurfaceParam;
    ParametricSurfaceParameters m_parametricSurfaceParam;
    IsoSurfaceParameters        m_isoSurfaceParam;
    bool                        m_infoVisible;
    D3CoordinateSystem         *m_coordinateSystem;

    CWnd *getInfoPanel() {
      return GetDlgItem(IDC_STATIC_INFOPANEL);
    }

    void setInfoVisible(bool visible);
    void showInfo(_In_z_ _Printf_format_string_ TCHAR const * const format, ...);
    void show3DInfo();
    void createInitialObject();
    CPoint get3DPanelPoint(CPoint point, bool screenRelative) const;
    D3SceneObject *createRotatedProfile();
    void createSaddle();
    void deleteCalculatedObject();
    void setCalculatedObject(D3SceneObject *obj, PersistentData *param = NULL);
    void setCalculatedObject(Function2DSurfaceParameters &param);
    void setCalculatedObject(ParametricSurfaceParameters &param);
    void setCalculatedObject(IsoSurfaceParameters        &param);
    D3SceneObject *getCalculatedObject() const;

  void startDebugging();
  void stopDebugging();

#ifdef DEBUG_POLYGONIZER
  void startDebugThread(bool singleStep=false);
  void killDebugThread(bool showCreateSurface);
  void asyncKillDebugThread();
  void ajourDebugMenuItems();
  inline bool hasDebugThread() const {
    return m_debugThread != NULL;
  }
  inline bool isDebugThreadRunning() const {
    return hasDebugThread() && m_debugThread->isRunning();
  }
  inline bool isDebugThreadStopped() const {
    return hasDebugThread() && !m_debugThread->isRunning() && !m_debugThread->isFinished();
  }
  inline bool isDebugThreadFinished() const {
    return hasDebugThread() && !m_debugThread->isRunning() && m_debugThread->isFinished();
  }
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);

#endif

public:
    CD3FunctionPlotterDlg(CWnd *pParent = NULL);
    D3Scene &getScene() {
      return m_scene;
    }
    CWnd    *getMessageWindow() {
      return this;
    }
    CWnd    *get3DWindow() {
      return GetDlgItem(IDC_STATIC_3DPANEL);
    }
    void render(BYTE renderFlags) {
      SendMessage(ID_MSG_RENDER, renderFlags, 0);
    }
    void modifyContextMenu(CMenu &menu) {
      appendMenuItem(menu, _T("Add box"), ID_ADDBOXOBJECT);
    }
    bool isBreakOnNextLevelChecked() const;

    const Function2DSurfaceParameters &get2DSurfaceParameters() const {
      return m_function2DSurfaceParam;
    }
    const ParametricSurfaceParameters &getParametricSurfaceParameters() const {
      return m_parametricSurfaceParam;
    }
    const IsoSurfaceParameters        &getIsoSurfaceParameters() const {
      return m_isoSurfaceParam;
    }
    enum { IDD = IDR_MAINFRAME };

protected:
    virtual BOOL PreTranslateMessage(MSG *pMsg);
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    virtual void OnCancel();
    virtual void OnOK();
    afx_msg void OnClose();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnFileSaveState();
    afx_msg void OnFileLoadState();
    afx_msg void OnFileFunctionSurface();
    afx_msg void OnFileParametricSurface();
    afx_msg void OnFileIsoSurface();
    afx_msg void OnFileProfileSurface();
    afx_msg void OnFileRead3DPointsFromFile();
    afx_msg void OnFileReadObjFile();
    afx_msg void OnFileExit();
    afx_msg void OnViewShow3dinfo();
    afx_msg void OnDebugGo();
    afx_msg void OnDebugSinglestep();
    afx_msg void OnDebugStepCube();
    afx_msg void OnDebugBreakOnNextLevel();
    afx_msg void OnResetPositions();
    afx_msg void OnObjectEditFunction();
    afx_msg void OnAddBoxObject();
    afx_msg LRESULT OnMsgRender(WPARAM wp, LPARAM lp);
    afx_msg LRESULT OnMsgDebugThreadRunning(WPARAM wp, LPARAM lp);
    afx_msg LRESULT OnMsgKillDebugThread(WPARAM wp, LPARAM lp);

    DECLARE_MESSAGE_MAP()
};
