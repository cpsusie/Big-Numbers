#pragma once

#include "wordbpn.h"

class ParamDlgParameters : public TrainingParameters {
public:
  int   m_priority;
  ParamDlgParameters(const TrainingParameters &src, int priority = THREAD_PRIORITY_NORMAL);
};

class ParamDlg : public CDialog {
private:
  ParamDlgParameters &m_params;
public:
  ParamDlg(ParamDlgParameters &params, CWnd *pParent = nullptr);

  enum { IDD = IDD_DIALOGTRAININGPARAMDIALOG };
  float   m_learningrate;
  float   m_momentum;
  int     m_updatealways;
  CString m_threadpriority;

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual void OnOK();
  virtual BOOL OnInitDialog();
  DECLARE_MESSAGE_MAP()
};

