#pragma once

#include "SaveLoadExprWithCommonParametersDlg.h"
#include <D3DGraphics/ExprParametricR2R3SurfaceParameters.h>

class CParametricR2R3SurfaceParametersDlg : public SaveLoadExprWithCommonParametersDlg<ExprParametricR2R3SurfaceParameters> {
private:
  CString m_commonText;
  CString m_exprX;
  CString m_exprY;
  CString m_exprZ;
  double  m_tfrom;
  double  m_tto;
  double  m_sfrom;
  double  m_sto;
  UINT    m_tStepCount;
  UINT    m_sStepCount;

  bool   validate();
  void   paramToWin(const ExprParametricR2R3SurfaceParameters &param);
  bool   winToParam(      ExprParametricR2R3SurfaceParameters &param);
  void   enableTimeFields();

public:
  CParametricR2R3SurfaceParametersDlg(const ExprParametricR2R3SurfaceParameters &param, AbstractTextureFactory &atf, CWnd *pParent = NULL);
  virtual ~CParametricR2R3SurfaceParametersDlg();

  enum { IDD = IDR_PARAMETRICR2R3SURFACE };

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  afx_msg BOOL OnInitDialog();
  afx_msg void OnEditFindMatchingParentesis();
  afx_msg void OnButtonHelpX();
  afx_msg void OnButtonHelpY();
  afx_msg void OnButtonHelpZ();
  DECLARE_MESSAGE_MAP()
};
