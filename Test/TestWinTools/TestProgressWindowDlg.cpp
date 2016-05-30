#include "stdafx.h"
#include <MyUtil.h>
#include <Math.h>
#include <MFCUtil/WinTools.h>
#include <MFCUtil/ProgressWindow.h>
#include "TestProgressWindowDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTestProgressWindowDlg::CTestProgressWindowDlg(CWnd* pParent) : CDialog(CTestProgressWindowDlg::IDD, pParent) {
    //{{AFX_DATA_INIT(CTestProgressWindowDlg)
	m_hasMessageBox     = TRUE;
	m_hasTimeEstimate   = TRUE;
	m_interruptable     = TRUE;
	m_hasProgressBar    = TRUE;
	m_showPercent       = FALSE;
	m_hasSubProgressBar = FALSE;
	m_suspendable       = FALSE;
	m_delayMSec         = 600;
	m_title             = _T("Some job");
	m_jobTime           = 10;
	m_updateRate        = 200;
	//}}AFX_DATA_INIT
}

void CTestProgressWindowDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CTestProgressWindowDlg)
	DDX_Check(pDX, IDC_CHECKHASMESSAGEBOX     , m_hasMessageBox);
	DDX_Check(pDX, IDC_CHECKHASTIMEESTIMATE   , m_hasTimeEstimate  );
	DDX_Check(pDX, IDC_CHECKINTERRUPTABLE     , m_interruptable    );
	DDX_Check(pDX, IDC_CHECKSUPPORTPROGRESS   , m_hasProgressBar   );
	DDX_Check(pDX, IDC_CHECKSUPPORTPERCENT    , m_showPercent      );
	DDX_Check(pDX, IDC_CHECKSUPPORTSUBPROGRESS, m_hasSubProgressBar);
	DDX_Check(pDX, IDC_CHECKSUSPENDABLE       , m_suspendable      );
	DDX_Text( pDX, IDC_EDITDELAY              , m_delayMSec        );
	DDX_Text( pDX, IDC_EDITTITLE              , m_title            );
	DDX_Text( pDX, IDC_EDITJOBTIME            , m_jobTime          );
	DDX_Text( pDX, IDC_EDITUPDATERATE         , m_updateRate       );
	DDV_MinMaxUInt(pDX, m_updateRate, 10, 2000);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestProgressWindowDlg, CDialog)
    //{{AFX_MSG_MAP(CTestProgressWindowDlg)
	ON_BN_CLICKED(IDC_BUTTONSTARTJOB              , OnButtonStartJob )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CTestProgressWindowDlg::OnInitDialog() {
    CDialog::OnInitDialog();

    return TRUE;
}

class SomeJob : public InteractiveRunnable {
private:
  const String       m_title;
  const unsigned int m_supportedFeatures;
  double             m_timeDoneMsec;
  double             m_totalMsec;
public:
  SomeJob(const TCHAR *title, int seconds, unsigned int supportedFeatures);

  int getSupportedFeatures() {
    return m_supportedFeatures;;
  }

  unsigned short getSubProgressPercent();
  unsigned short getProgress();
  unsigned short getMaxProgress() {
    return 1000;
  }
  String getTitle() {
    return m_title;
  }

  String getProgressMessage() {  
    return format(_T("%.1lf/%.0lf sec"), m_timeDoneMsec / 1000000, m_totalMsec / 1000000);
  }
  
  unsigned int run();
};

SomeJob::SomeJob(const TCHAR *title, int seconds, unsigned int supportedFeatures) 
: m_title(title)
, m_supportedFeatures(supportedFeatures)
{
  m_timeDoneMsec = 0;
  m_totalMsec    = (double)seconds * 1000000;
}

unsigned short SomeJob::getProgress() {
  if(m_timeDoneMsec >= m_totalMsec) {
    return getMaxProgress();
  } else  {
    return (short)((m_timeDoneMsec * getMaxProgress()) / m_totalMsec);
  }
}

unsigned short SomeJob::getSubProgressPercent() {
  return (short)fmod(m_timeDoneMsec / 10000.0,100);
}

unsigned int SomeJob::run() {
  bool warningDone = false;
  HANDLE thr = GetCurrentThread();
  for(;m_timeDoneMsec < m_totalMsec;) {
    if(isInterrupted()) {
      break;
    }
    if(isSuspended()) {
      if((SuspendThread(thr) < 0) && !warningDone) {
        AfxMessageBox(getLastErrorText().cstr(), MB_ICONWARNING);
        warningDone = true;
      }
    }
    for(int i = 0; i < 1000000; i++);
    m_timeDoneMsec = getThreadTime(thr);
  }
  return 0;
}


void CTestProgressWindowDlg::OnButtonStartJob() {
  UpdateData();
  unsigned int supportedFeatures = 0;
  if(m_hasProgressBar   ) supportedFeatures |= IR_PROGRESSBAR;
  if(m_showPercent      ) supportedFeatures |= IR_SHOWPERCENT;
  if(m_hasSubProgressBar) supportedFeatures |= IR_SUBPROGRESSBAR;
  if(m_interruptable    ) supportedFeatures |= IR_INTERRUPTABLE;
  if(m_suspendable      ) supportedFeatures |= IR_SUSPENDABLE;
  if(m_hasTimeEstimate  ) supportedFeatures |= IR_SHOWTIMEESTIMATE;
  if(m_hasMessageBox    ) supportedFeatures |= IR_SHOWPROGRESSMSG;

  SomeJob job(m_title, m_jobTime , supportedFeatures);
  ProgressWindow wnd(this, job, m_delayMSec, m_updateRate);
}

