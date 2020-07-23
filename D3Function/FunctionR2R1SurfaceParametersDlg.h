#pragma once

#include "SaveLoadExprWithCommonParametersDlg.h"
#include <D3DGraphics/D3AbstractTextureFactory.h>
#include <D3DGraphics/ExprFunctionR2R1SurfaceParameters.h>

class CExprFunctionR2R1SurfaceParametersDlg : public SaveLoadExprWithCommonParametersDlg<ExprFunctionR2R1SurfaceParameters> {
private:
  CString m_expr;
  double  m_xfrom;
  double  m_xto;
  double  m_yfrom;
  double  m_yto;
  int     m_pointCount;

  bool   validate();
  void   paramToWin(const ExprFunctionR2R1SurfaceParameters &param);
  bool   winToParam(      ExprFunctionR2R1SurfaceParameters &param);
  void   enableTimeFields();
public:
  CExprFunctionR2R1SurfaceParametersDlg(const ExprFunctionR2R1SurfaceParameters &param, AbstractTextureFactory &atf, CWnd *pParent = NULL);

  enum { IDD = IDR_FUNCTIONR2R1SURFACE };

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  afx_msg BOOL OnInitDialog();
  afx_msg void OnEditFindMatchingParentesis();
  afx_msg void OnButtonHelp();
  DECLARE_MESSAGE_MAP()
};
