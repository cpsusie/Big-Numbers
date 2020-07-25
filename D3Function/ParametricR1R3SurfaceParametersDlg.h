#pragma once

#include "SaveLoadExprWithCommonParametersDlg.h"
#include <D3DGraphics/ExprParametricR1R3SurfaceParameters.h>

class CParametricR1R3SurfaceParametersDlg : public SaveLoadExprWithCommonParametersDlg<ExprParametricR1R3SurfaceParameters> {
private:
  CString m_commonText;
  CString m_exprX;
  CString m_exprY;
  CString m_exprZ;
  double  m_tfrom;
  double  m_tto;
  UINT    m_tStepCount;
  CString m_profileFileName;

  bool   validate();
  void   paramToWin(const ExprParametricR1R3SurfaceParameters &param);
  bool   winToParam(      ExprParametricR1R3SurfaceParameters &param);
  void   enableTimeFields();

public:
  CParametricR1R3SurfaceParametersDlg(const ExprParametricR1R3SurfaceParameters &param, AbstractTextureFactory &atf, CWnd *pParent = NULL);
  virtual ~CParametricR1R3SurfaceParametersDlg();

  enum { IDD = IDR_PARAMETRICR1R3SURFACE };

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  afx_msg BOOL OnInitDialog();
  afx_msg void OnEditFindMatchingParentesis();
  afx_msg void OnButtonHelpX();
  afx_msg void OnButtonHelpY();
  afx_msg void OnButtonHelpZ();
  DECLARE_MESSAGE_MAP()
};
