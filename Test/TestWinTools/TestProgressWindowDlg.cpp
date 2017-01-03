#include "stdafx.h"
#include <MyUtil.h>
#include <Math.h>
#include <MFCUtil/WinTools.h>
#include <MFCUtil/ProgressWindow.h>
#include "TestProgressWindowDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTestProgressWindowDlg::CTestProgressWindowDlg(CWnd* pParent) : CDialog(CTestProgressWindowDlg::IDD, pParent), m_jobCount(0)
{
    m_hasMessageBox         = TRUE;
    m_hasTimeEstimate       = TRUE;
    m_autoCorrelateEstimate = FALSE;
    m_interruptable         = TRUE;
    m_hasProgressBar        = TRUE;
    m_showPercent           = FALSE;
    m_hasSubProgressBar     = FALSE;
    m_suspendable           = FALSE;
    m_delayMSec             = 600;
    m_title                 = _T("Some job");
    m_jobTime               = 10;
    m_updateRate            = 200;
    m_jobCount              = 1;
}

void CTestProgressWindowDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Check(pDX, IDC_CHECKHASMESSAGEBOX, m_hasMessageBox);
  DDX_Check(pDX, IDC_CHECKHASTIMEESTIMATE, m_hasTimeEstimate);
  DDX_Check(pDX, IDC_CHECKAUTOCORRELATEESTIMATE, m_autoCorrelateEstimate);
  DDX_Check(pDX, IDC_CHECKINTERRUPTABLE, m_interruptable);
  DDX_Check(pDX, IDC_CHECKSUPPORTPROGRESS, m_hasProgressBar);
  DDX_Check(pDX, IDC_CHECKSUPPORTPERCENT, m_showPercent);
  DDX_Check(pDX, IDC_CHECKSUPPORTSUBPROGRESS, m_hasSubProgressBar);
  DDX_Check(pDX, IDC_CHECKSUSPENDABLE, m_suspendable);
  DDX_Text(pDX, IDC_EDITDELAY, m_delayMSec);
  DDX_Text(pDX, IDC_EDITTITLE, m_title);
  DDX_Text(pDX, IDC_EDITJOBTIME, m_jobTime);
  DDX_Text(pDX, IDC_EDITUPDATERATE, m_updateRate);
  DDV_MinMaxUInt(pDX, m_updateRate, 10, 2000);
  DDX_Text(pDX, IDC_EDITJOBCOUNT, m_jobCount);
	DDV_MinMaxUInt(pDX, m_jobCount, 1, 4);
}

BEGIN_MESSAGE_MAP(CTestProgressWindowDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTONSTARTJOB              , OnButtonStartJob )
END_MESSAGE_MAP()

BOOL CTestProgressWindowDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  return TRUE;
}

class SomeJob : public InteractiveRunnable {
private:
  const String       m_title;
  const unsigned int m_supportedFeatures;
  double             m_totalMsec, m_threadMsec;
  USHORT             m_jobCount;
public:
  SomeJob(const TCHAR *title, int seconds, UINT supportedFeatures, USHORT m_jobCount);

  int getSupportedFeatures() {
    return m_supportedFeatures;
  }

  double getMaxProgress() const {
    return m_totalMsec;
  }
  double getProgress() const;
  USHORT getSubProgressPercent(UINT index);
  USHORT getJobCount() const {
    return m_jobCount;
  }
  String getTitle() {
    return m_title;
  }

  String getProgressMessage(UINT index) {  
    return format(_T("Job %d:%.1lf/%.0lf sec"), index, m_threadMsec / 1000000, m_totalMsec / 1000000);
  }
  
  UINT run();
};

SomeJob::SomeJob(const TCHAR *title, int seconds, UINT supportedFeatures, USHORT jobCount) 
: m_title(title)
, m_supportedFeatures(supportedFeatures)
, m_jobCount(jobCount)
{
  m_totalMsec    = (double)seconds * 1000000;
  m_threadMsec   = 0;
}

double SomeJob::getProgress() const {
  return m_threadMsec;
}

USHORT SomeJob::getSubProgressPercent(UINT i) {
  return (short)fmod(m_threadMsec / 10000.0,100);
}

UINT SomeJob::run() {
  while(m_threadMsec < m_totalMsec) {
    if(isInterrupted()) {
      break;
    }
    if(isSuspended()) {
      suspend();
    }
    for(int i = 0; i < 1000000; i++);
    m_threadMsec = getThreadTime();
  }
  return 0;
}

void CTestProgressWindowDlg::OnButtonStartJob() {
  UpdateData();
  UINT supportedFeatures = 0;
  if(m_hasProgressBar       ) supportedFeatures |= IR_PROGRESSBAR;
  if(m_showPercent          ) supportedFeatures |= IR_SHOWPERCENT;
  if(m_hasSubProgressBar    ) supportedFeatures |= IR_SUBPROGRESSBAR;
  if(m_interruptable        ) supportedFeatures |= IR_INTERRUPTABLE;
  if(m_suspendable          ) supportedFeatures |= IR_SUSPENDABLE;
  if(m_hasTimeEstimate      ) supportedFeatures |= IR_SHOWTIMEESTIMATE;
  if(m_autoCorrelateEstimate) supportedFeatures |= IR_AUTOCORRELATETIME;
  if(m_hasMessageBox        ) supportedFeatures |= IR_SHOWPROGRESSMSG;

  SomeJob job(m_title, m_jobTime , supportedFeatures, m_jobCount);
  ProgressWindow wnd(this, job, m_delayMSec, m_updateRate);
}
