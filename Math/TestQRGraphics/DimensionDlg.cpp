#include "stdafx.h"
#include "DimensionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DimensionDlg::DimensionDlg(int dimension, CWnd* pParent /*=NULL*/)
    : CDialog(DimensionDlg::IDD, pParent)
{
    m_dimension = dimension;
}


void DimensionDlg::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDITDIMENSION, m_dimension);
    DDV_MinMaxUInt(pDX, m_dimension, 1, 30);
}


BEGIN_MESSAGE_MAP(DimensionDlg, CDialog)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DimensionDlg message handlers

BOOL DimensionDlg::OnInitDialog() 
{
  GetDlgItem(IDC_EDITDIMENSION)->SetFocus();
  __super::OnInitDialog();
    
  return FALSE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}
