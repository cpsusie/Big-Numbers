#pragma once

#include <MFCUtil/ExprDialog.h>
#include <D3DGraphics/ExprFunction2DSurfaceParameters.h>

class CFunction2DSurfaceDlg : public SaveLoadExprDialog<ExprFunction2DSurfaceParameters> {
private:
  CString m_expr;
  double  m_xfrom;
  double  m_xto;
  double  m_yfrom;
  double  m_yto;
  int     m_pointCount;
  BOOL    m_machineCode;
  BOOL    m_createListFile;
  UINT    m_frameCount;
  double  m_timefrom;
  double  m_timeto;
  BOOL    m_includeTime;
  BOOL    m_doubleSided;

  String getListFileName() const;
  bool   validate();
  void   paramToWin(const ExprFunction2DSurfaceParameters &param);
  bool   winToParam(      ExprFunction2DSurfaceParameters &param);
  void   enableTimeFields();
public:
  CFunction2DSurfaceDlg(const ExprFunction2DSurfaceParameters &param, CWnd *pParent = NULL);

  enum { IDD = IDR_FUNC2DSURFACE };

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
    afx_msg void OnCheckMachineCode();
    afx_msg void OnButtonHelp();
    DECLARE_MESSAGE_MAP()
};
