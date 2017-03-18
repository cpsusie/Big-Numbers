#include "stdafx.h"
#include <io.h>
#include "RegexDemo.h"
#include "TestRegexDlg.h"
#include "..\Test\UnitTest\TestRegex\TestRegex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTestRegexDlg::CTestRegexDlg(CWnd* pParent /*=NULL*/) : CDialog(CTestRegexDlg::IDD, pParent) {
  m_text = EMPTYSTRING;
}


void CTestRegexDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDITOUTPUT, m_text);
}


BEGIN_MESSAGE_MAP(CTestRegexDlg, CDialog)
    ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CTestRegexDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  m_layoutManager.OnInitDialog(this);

  m_layoutManager.addControl(IDOK          , RELATIVE_POSITION);
  m_layoutManager.addControl(IDC_EDITOUTPUT, RELATIVE_SIZE    );

  FILE *output = NULL;
  try {
    const String fileName = _T("c:\\temp\\testRegex.txt");

    output = FOPEN(fileName, _T("w"));
    testRegex(output);
    fclose(output); output = NULL;

    String s = readTextFile(_T("c:\\temp\\DebugTrace_testRE.txt"));
    s.replace('\n',_T("\r\n"));

    UNLINK(fileName);

    m_text = s.cstr();

    UpdateData(FALSE);
  } catch(Exception e) {
    if(output) {
      fclose(output);
    }
    MessageBox(e.what(), _T("Error"), MB_ICONWARNING);
  }

  GetDlgItem(IDC_EDITOUTPUT)->SetFocus();

  return FALSE;
}

void CTestRegexDlg::OnSize(UINT nType, int cx, int cy) {
  CDialog::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);    
}
