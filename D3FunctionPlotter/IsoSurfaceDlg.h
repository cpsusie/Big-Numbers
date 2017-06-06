#pragma once

#include <MFCUtil/ExprDialog.h>
#include <D3DGraphics/IsoSurface.h>

class CIsoSurfaceDlg : public SaveLoadExprDialog<IsoSurfaceParameters> {
private:
    bool validate();
    void paramToWin(const IsoSurfaceParameters &param);
    void winToParam(      IsoSurfaceParameters &param) const;
    void enableCheckBox();
    void enableTimeFields();
public:
    CIsoSurfaceDlg(const IsoSurfaceParameters &param, CWnd *pParent = NULL);

  enum { IDD = IDR_ISOSURFACE };
    CString m_expr;
    double  m_cellSize;
    UINT    m_bounds;
    BOOL    m_tetrahedral;
    BOOL	  m_adaptiveCellSize;
    BOOL    m_originOutside;
    BOOL    m_machineCode;
    BOOL	  m_doubleSided;
    BOOL	  m_includeTime;
    UINT	  m_frameCount;
    double	m_timeFrom;
    double	m_timeTo;
    double	m_xfrom;
    double	m_xto;
    double	m_yfrom;
    double	m_yto;
    double	m_zfrom;
    double	m_zto;

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    afx_msg BOOL OnInitDialog();
    afx_msg void OnEditFindMatchingParentesis();
    afx_msg void OnGotoExpr();
    afx_msg void OnGotoCellSize();
    afx_msg void OnGotoXInterval();
    afx_msg void OnGotoYInterval();
    afx_msg void OnGotoZInterval();
    afx_msg void OnGotoTimeInterval();
    afx_msg void OnGotoFrameCount();
    afx_msg void OnCheckDoubleSided();
    afx_msg void OnCheckIncludeTime();
    afx_msg void OnButtonHelp();
    DECLARE_MESSAGE_MAP()
};

