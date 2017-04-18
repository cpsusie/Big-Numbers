#pragma once

#include "ExprDialog.h"
#include <D3DGraphics/Function2DSurface.h>

class CFunction2DSurfaceDlg : public CExprDialog {
private:
  HACCEL m_accelTable;

  bool validate();
  void saveAs(Function2DSurfaceParameters &param);
  void save(const String &fileName, Function2DSurfaceParameters &param);
  void enableTimeFields();
  void paramToWin(const Function2DSurfaceParameters &param);
  void winToParam(Function2DSurfaceParameters &param) const;
public:
  CFunction2DSurfaceDlg(const Function2DSurfaceParameters &param, CWnd* pParent = NULL);

  enum { IDD = IDD_FUNC2DSURFACE_DIALOG };
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

    Function2DSurfaceParameters m_param;
    CString                     m_name;

protected:
    virtual BOOL PreTranslateMessage(MSG *pMsg);
    virtual void DoDataExchange(CDataExchange *pDX);
    afx_msg BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    virtual void OnOK();
    afx_msg void OnFileOpen();
    afx_msg void OnFileSave();
    afx_msg void OnFileSaveAs();
    afx_msg void OnEditFindMatchingParentesis();
    afx_msg void OnGotoExpr();
    afx_msg void OnGotoXInterval();
    afx_msg void OnGotoYInterval();
    afx_msg void OnGotoTimeInterval();
    afx_msg void OnGotoPointCount();
    afx_msg void OnGotoFrameCount();
    afx_msg void OnCheckIncludeTime();
    afx_msg void OnButtonHelp();
    afx_msg void OnExprHelp(UINT id);
    DECLARE_MESSAGE_MAP()
};
