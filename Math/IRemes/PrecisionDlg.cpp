#include "stdafx.h"
#include "IRemes.h"
#include "PrecisionDlg.h"

IMPLEMENT_DYNAMIC(CPrecisionDlg, CDialog)

CPrecisionDlg::CPrecisionDlg(const VisiblePrecisions &values, CWnd *pParent /*=nullptr*/)
  : CDialog(IDD_PRECISION_DIALOG, pParent)
  , m_coefDigits(   values.m_coefDigits   )
  , m_extremaDigits(values.m_extremaDigits)
  , m_errorDigits(  values.m_errorDigits  )
{
}

CPrecisionDlg::~CPrecisionDlg() {
}

void CPrecisionDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDIT_COEFDIGITS   , m_coefDigits   );
  DDX_Text(pDX, IDC_EDIT_EXTREMADIGITS, m_extremaDigits);
  DDX_Text(pDX, IDC_EDIT_ERRORDIGITS  , m_errorDigits  );
}

BEGIN_MESSAGE_MAP(CPrecisionDlg, CDialog)
END_MESSAGE_MAP()

BOOL CPrecisionDlg::OnInitDialog() {
  __super::OnInitDialog();

  UpdateData(FALSE);
  gotoEditBox(this, IDC_EDIT_COEFDIGITS);
  return FALSE;
}

#define CHECKINVALIERANGE(v, id)          \
if(((v) < 1)||((v)>40)) {                 \
  gotoEditBox(this,  id);                 \
  showWarning(_T("Valid range:[1..40]")); \
  return;                                 \
}

void CPrecisionDlg::OnOK() {
  UpdateData();
  CHECKINVALIERANGE(m_coefDigits   , IDC_EDIT_COEFDIGITS   )
  CHECKINVALIERANGE(m_extremaDigits, IDC_EDIT_EXTREMADIGITS)
  CHECKINVALIERANGE(m_errorDigits  , IDC_EDIT_ERRORDIGITS  )
  __super::OnOK();
}
