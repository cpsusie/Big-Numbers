#pragma once

#include <MFCUtil/ExprDialog.h>
#include <D3DGraphics/ParametricSurface.h>

class CParametricSurfaceDlg : public SaveLoadExprDialog<ParametricSurfaceParameters> {
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
  BOOL    m_includeTime;
  BOOL    m_doubleSided;
  double  m_timefrom;
  double  m_timeto;
  UINT    m_frameCount;
  BOOL    m_machineCode;

  bool validate();
  void paramToWin(const ParametricSurfaceParameters &param);
  bool winToParam(      ParametricSurfaceParameters &param);

  void enableTimeFields();

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
  CParametricSurfaceDlg(const ParametricSurfaceParameters &param, CWnd *pParent = NULL);
  virtual ~CParametricSurfaceDlg();

  enum { IDD = IDR_PARAMETRICSURFACE };

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
    afx_msg void OnGotoTimeInterval();
    afx_msg void OnGotoFrameCount();
    afx_msg void OnCheckIncludeTime();
    afx_msg void OnButtonHelpX();
    afx_msg void OnButtonHelpY();
    afx_msg void OnButtonHelpZ();
    DECLARE_MESSAGE_MAP()
};
