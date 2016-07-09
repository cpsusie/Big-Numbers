#pragma once

#include "IsoSurface.h"
#include "ExprDialog.h"

class CIsoSurfaceDlg : public CExprDialog {
private:
    HACCEL m_accelTable;

    bool validate();
    void saveAs(IsoSurfaceParameters &param);
    void save(const String &fileName, IsoSurfaceParameters &param);
    void paramToWin(const IsoSurfaceParameters &param);
    void winToParam(IsoSurfaceParameters &param);
    void enableCheckBox();
    void enableTimeFields();
public:
    CIsoSurfaceDlg(const IsoSurfaceParameters &param, CWnd *pParent = NULL);

  enum { IDD = IDD_ISOSURFACE_DIALOG };
    CString m_expr;
    double  m_size;
    UINT    m_bounds;
    BOOL    m_tetrahedral;
    BOOL	m_adaptiveCellSize;
    BOOL    m_originOutside;
    BOOL    m_machineCode;
    BOOL	m_doubleSided;
    BOOL	m_includeTime;
    int		m_timeCount;
    double	m_timeFrom;
    double	m_timeTo;
    double	m_xfrom;
    double	m_xto;
    double	m_yfrom;
    double	m_yto;
    double	m_zfrom;
    double	m_zto;

    IsoSurfaceParameters m_param;
    CString              m_name;

    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:

    afx_msg BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    virtual void OnOK();
    afx_msg void OnFileOpen();
    afx_msg void OnFileSave();
    afx_msg void OnFileSaveAs();
    afx_msg void OnEditFindMatchingParentesis();
    afx_msg void OnGotoExpr();
    afx_msg void OnGotoBounds();
    afx_msg void OnGotoSize();
    afx_msg void OnGotoTInterval();
    afx_msg void OnGotoTimeCount();
    afx_msg void OnCheckDoublesided();
    afx_msg void OnCheckIncludeTime();
    afx_msg void OnButtonHelp();
    afx_msg void OnExprHelp(UINT id);
    DECLARE_MESSAGE_MAP()
};

