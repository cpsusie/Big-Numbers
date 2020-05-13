#pragma once

#include "SaveLoadExprWithAnimationDialog.h"
#include <D3DGraphics/ExprParametricR2R3SurfaceParameters.h>

class CParametricR2R3SurfaceParametersDlg : public SaveLoadExprWithAnimationDialog<ExprParametricR2R3SurfaceParameters> {
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
  BOOL    m_doubleSided;

  bool   validate();
  void   paramToWin(const ExprParametricR2R3SurfaceParameters &param);
  bool   winToParam(      ExprParametricR2R3SurfaceParameters &param);
  void   enableTimeFields();

  void gotoExprX() {
    gotoExpr(IDC_EDIT_EXPRX);
  }
  void gotoExprY() {
    gotoExpr(IDC_EDIT_EXPRY);
  }
  void gotoExprZ() {
    gotoExpr(IDC_EDIT_EXPRZ);
  }

public:
  CParametricR2R3SurfaceParametersDlg(const ExprParametricR2R3SurfaceParameters &param, CWnd *pParent = NULL);
  virtual ~CParametricR2R3SurfaceParametersDlg();

  enum { IDD = IDR_PARAMETRICR2R3SURFACE };

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  afx_msg BOOL OnInitDialog();
  afx_msg void OnEditFindMatchingParentesis();
  afx_msg void OnGotoCommon();
  afx_msg void OnGotoExprX();
  afx_msg void OnGotoExprY();
  afx_msg void OnGotoExprZ();
  afx_msg void OnGotoTInterval();
  afx_msg void OnGotoSInterval();
  afx_msg void OnGotoTStepCount();
  afx_msg void OnGotoSStepCount();
  afx_msg void OnButtonHelpX();
  afx_msg void OnButtonHelpY();
  afx_msg void OnButtonHelpZ();
  DECLARE_MESSAGE_MAP()
};
