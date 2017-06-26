#include "stdafx.h"
#include "wordsepgraph.h"
#include "ParamDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


ParamDlg::ParamDlg(ParamDlgParameters &params, CWnd *pParent /*=NULL*/)
: CDialog(ParamDlg::IDD, pParent), m_params(params)
{
  m_learningrate = params.m_learningRate;
  m_momentum     = params.m_momentum;
  m_updatealways = params.m_updateAlways ? TRUE : FALSE;
  switch(params.m_priority) {
  case THREAD_PRIORITY_IDLE            : m_threadpriority = _T("PRIORITY_IDLE");          break;
  case THREAD_PRIORITY_LOWEST          : m_threadpriority = _T("PRIORITY_LOWEST");        break;
  case THREAD_PRIORITY_BELOW_NORMAL    : m_threadpriority = _T("PRIORITY_BELOW_NORMAL");  break;
  case THREAD_PRIORITY_NORMAL          : m_threadpriority = _T("PRIORITY_NORMAL");        break;
  case THREAD_PRIORITY_ABOVE_NORMAL    : m_threadpriority = _T("PRIORITY_ABOVE_NORMAL");  break;
  case THREAD_PRIORITY_HIGHEST         : m_threadpriority = _T("PRIORITY_HIGHEST");       break;
  case THREAD_PRIORITY_TIME_CRITICAL   : m_threadpriority = _T("PRIORITY_TIME_CRITICAL"); break;
  }
}


void ParamDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITLEARNINGRATE, m_learningrate);
  DDX_Text(pDX, IDC_EDITMOMENTUM, m_momentum);
  DDX_Radio(pDX, IDC_RADIO_UPDATE_ON_ERROR_ONLY, m_updatealways);
  DDX_CBString(pDX, IDC_COMBOPRIORITY, m_threadpriority);
}


BEGIN_MESSAGE_MAP(ParamDlg, CDialog)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ParamDlg message handlers

void ParamDlg::OnOK() {
  if(UpdateData()) {
    m_params.m_learningRate = m_learningrate;
    m_params.m_momentum     = m_momentum;
    m_params.m_updateAlways = m_updatealways ? true : false;
         if(m_threadpriority == _T("PRIORITY_IDLE")         ) m_params.m_priority = THREAD_PRIORITY_IDLE;
    else if(m_threadpriority == _T("PRIORITY_LOWEST")       ) m_params.m_priority = THREAD_PRIORITY_LOWEST;
    else if(m_threadpriority == _T("PRIORITY_BELOW_NORMAL") ) m_params.m_priority = THREAD_PRIORITY_BELOW_NORMAL;
    else if(m_threadpriority == _T("PRIORITY_NORMAL")       ) m_params.m_priority = THREAD_PRIORITY_NORMAL;
    else if(m_threadpriority == _T("PRIORITY_ABOVE_NORMAL") ) m_params.m_priority = THREAD_PRIORITY_ABOVE_NORMAL;
    else if(m_threadpriority == _T("PRIORITY_HIGHEST")      ) m_params.m_priority = THREAD_PRIORITY_HIGHEST;
    else if(m_threadpriority == _T("PRIORITY_TIME_CRITICAL")) m_params.m_priority = THREAD_PRIORITY_TIME_CRITICAL;
    __super::OnOK();
  }
}

BOOL ParamDlg::OnInitDialog() {
  __super::OnInitDialog();
  GetDlgItem(IDC_EDITLEARNINGRATE)->SetFocus();
  return false;
}

ParamDlgParameters::ParamDlgParameters(const TrainingParameters &src, int priority) : TrainingParameters(src) {
  m_priority = priority;
}
