#include "stdafx.h"
#include "partymaker.h"
#include "SelectAttributeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSelectAttributeDlg::CSelectAttributeDlg(CWnd *pParent /*=NULL*/)
    : CDialog(CSelectAttributeDlg::IDD, pParent)
{
}


void CSelectAttributeDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSelectAttributeDlg, CDialog)
END_MESSAGE_MAP()
