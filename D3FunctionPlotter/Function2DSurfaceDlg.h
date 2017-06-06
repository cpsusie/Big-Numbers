#pragma once

#include <MFCUtil/ExprDialog.h>
#include <D3DGraphics/Function2DSurface.h>

class CFunction2DSurfaceDlg : public SaveLoadExprDialog<Function2DSurfaceParameters> {
private:
  bool validate();
  void paramToWin(const Function2DSurfaceParameters &param);
  void winToParam(      Function2DSurfaceParameters &param) const;
  void enableTimeFields();
public:
  CFunction2DSurfaceDlg(const Function2DSurfaceParameters &param, CWnd *pParent = NULL);

  enum { IDD = IDR_FUNC2DSURFACE };
    CString m_expr;
    double  m_xfrom;
    double  m_xto;
    double  m_yfrom;
    double  m_yto;
    int     m_pointCount;
    BOOL    m_machineCode;
    UINT	  m_frameCount;
    double	m_timeFrom;
    double	m_timeTo;
    BOOL	  m_includeTime;
    BOOL	  m_doubleSided;

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    afx_msg BOOL OnInitDialog();
    afx_msg void OnEditFindMatchingParentesis();
    afx_msg void OnGotoExpr();
    afx_msg void OnGotoXInterval();
    afx_msg void OnGotoYInterval();
    afx_msg void OnGotoTimeInterval();
    afx_msg void OnGotoPointCount();
    afx_msg void OnGotoFrameCount();
    afx_msg void OnCheckIncludeTime();
    afx_msg void OnButtonHelp();
    DECLARE_MESSAGE_MAP()
};
