#include "stdafx.h"
#include "ShwGraph.h"
#include "dataedit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

dataedit::dataedit(CWnd* pParent /*=NULL*/) : CDialog(dataedit::IDD, pParent) {
}

void dataedit::DoDataExchange(CDataExchange* pDX) {
    __super::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(dataedit, CDialog)
        // NOTE: the ClassWizard will add message map macros here
END_MESSAGE_MAP()

BOOL dataedit::OnInitDialog() { 
  __super::OnInitDialog();
  CListBox *lb = (CListBox*)GetDlgItem(IDC_DATALIST);

  TCHAR tmp[50];
  int i;
  strcpy(tmp,_T("jesper"));
  for(i=0;i<m_ndata;i++) {
    _stprintf(tmp,_T("%13lg %13lg"),m_data[i].x,m_data[i].f);
    lb->InsertString(-1,tmp);
  }
  return(TRUE);
}
