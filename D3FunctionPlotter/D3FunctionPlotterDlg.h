#pragma once

#include <MFCUtil/LayoutManager.h>
#include <D3DGraphics/Function2DSurface.h>
#include <D3DGraphics/ParametricSurface.h>
#include <D3DGraphics/IsoSurface.h>
#include "D3DGraphics/D3CoordinateSystem.h"
#include "D3DGraphics/D3SceneEditor.h"
#include "MemoryLogThread.h"

class CD3FunctionPlotterDlg : public CDialog, public D3SceneContainer {
private:
    HICON                       m_hIcon;
    HACCEL                      m_accelTable;
    SimpleLayoutManager         m_layoutManager;
    D3Scene                     m_scene;
    D3SceneEditor               m_editor;
    MemoryLogThread             m_memlogThread;
    Function2DSurfaceParameters m_function2DSurfaceParam;
    ParametricSurfaceParameters m_parametricSurfaceParam;
    IsoSurfaceParameters        m_isoSurfaceParam;
    bool                        m_infoVisible;
    D3SceneObject              *m_calculatedObject;
    D3CoordinateSystem         *m_coordinateSystem;

    void setFillMode( D3DFILLMODE  fillMode );
    void setShadeMode(D3DSHADEMODE shadeMode);
    CWnd *getInfoPanel() {
      return GetDlgItem(IDC_STATIC_INFOPANEL);
    }

    void setInfoVisible(bool visible);
    void showInfo(const TCHAR *format, ...);
    void show3DInfo();
    CMenu &loadMenu(CMenu &menu, int id);
    void showContextMenu(CMenu &menu, CPoint point);
    void createInitialObject();
    CPoint get3DPanelPoint(CPoint point, bool screenRelative) const;
    D3SceneObject *createRotatedProfile();
    void createSaddle();

    void setCalculatedObject(D3SceneObject *obj, PersistentParameter *param = NULL);
    void setCalculatedObject(Function2DSurfaceParameters *param);
    void setCalculatedObject(ParametricSurfaceParameters *param);
    void setCalculatedObject(IsoSurfaceParameters        *param);

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
      PostMessage(ID_MSG_RENDER, renderFlags, 0);
    }
    enum { IDD = IDR_MAINFRAME };

    virtual BOOL PreTranslateMessage(MSG *pMsg);
    protected:
    virtual void DoDataExchange(CDataExchange *pDX);

protected:
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
    afx_msg void OnFileNexttry();
    afx_msg void OnViewShow3dinfo();
    afx_msg void OnResetPositions();
    afx_msg void OnObjectEditFunction();
    afx_msg LRESULT OnMsgRender(WPARAM wp, LPARAM lp);
    DECLARE_MESSAGE_MAP()
};

