#pragma once

#include <MFCUtil/LayoutManager.h>
#include <D3DGraphics/Function2DSurface.h>
#include <D3DGraphics/ParametricSurface.h>
#include <D3DGraphics/IsoSurface.h>
#include "D3DGraphics/D3SceneObjectCoordinateSystem.h"
#include "D3DGraphics/D3SceneEditor.h"

#define INFO_EDIT  0x01
#define INFO_MEM   0x02

#define INFO_ALL  (INFO_EDIT|INFO_MEM)

class CD3FunctionPlotterDlg : public CDialog
                            , public D3SceneContainer
{
private:
    HICON                       m_hIcon;
    HACCEL                      m_accelTable;
    SimpleLayoutManager         m_layoutManager;
    D3Scene                     m_scene;
    D3SceneEditor               m_editor;
    String                      m_editorInfo, m_memoryInfo, m_debugInfo;
    int                         m_infoPanelTopLine;
    bool                        m_timerRunning;
    Function2DSurfaceParameters    m_function2DSurfaceParam;
    ParametricSurfaceParameters    m_parametricSurfaceParam;
    IsoSurfaceParameters           m_isoSurfaceParam;
    bool                           m_infoVisible;
    D3SceneObjectCoordinateSystem *m_coordinateSystem;

    CEdit *getInfoPanel() {
      return (CEdit*)GetDlgItem(IDC_EDIT_INFOPANEL);
    }
    void startTimer();
    void stopTimer();
    void setInfoVisible(bool visible);
    void updateEditorInfo();
    void updateMemoryInfo();
    void showInfo(_In_z_ _Printf_format_string_ TCHAR const * const format, ...);
    // flags any combination of INFO_MEM, INFO_EDIT, INFO_DEBUG
    void show3DInfo(BYTE flags);

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
    bool isAutoFocusCurrentCubeChecked() const;

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
    afx_msg void OnTimer(UINT_PTR nIDEvent);
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
    afx_msg void OnResetPositions();
    afx_msg void OnObjectEditFunction();
    afx_msg void OnAddBoxObject();
    afx_msg void OnVscrollEditInfoPanel();
    afx_msg LRESULT OnMsgRender(              WPARAM wp, LPARAM lp);

    DECLARE_MESSAGE_MAP()
};
