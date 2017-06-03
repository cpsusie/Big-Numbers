#pragma once

#include <D3DGraphics/ParametricSurface.h>
#include "ExprDialog.h"

class CParametricSurfaceDlg : public SaveLoadExprDialog<ParametricSurfaceParameters> {
private:
  int    m_selectedExprId;
  bool validate();
  void paramToWin(const ParametricSurfaceParameters &param);
  void winToParam(      ParametricSurfaceParameters &param) const;

  void enableTimeFields();

  void gotoExprX() {
    GetDlgItem(IDC_EDIT_EXPRX)->SetFocus();
  }
  void gotoExprY() {
    GetDlgItem(IDC_EDIT_EXPRY)->SetFocus();
  }
  void gotoExprZ() {
    GetDlgItem(IDC_EDIT_EXPRZ)->SetFocus();
  }

public:
	CParametricSurfaceDlg(const ParametricSurfaceParameters &param, CWnd *pParent = NULL);
	virtual ~CParametricSurfaceDlg();

	enum { IDD = IDR_PARAMETRICSURFACE };

private:
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
  double  m_timeFrom;
  double  m_timeTo;
  UINT    m_frameCount;
  BOOL    m_machineCode;

protected:
	  virtual void DoDataExchange(CDataExchange *pDX);
    afx_msg BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnEditFindMatchingParentesis();
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
    afx_msg void OnExprHelp(UINT id);
    DECLARE_MESSAGE_MAP()
};
