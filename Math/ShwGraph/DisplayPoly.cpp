#include "stdafx.h"
#include "ShwGraph.h"
#include "DisplayPoly.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

DisplayPoly::DisplayPoly(CWnd *pParent /*=NULL*/) : CDialog(DisplayPoly::IDD, pParent) {
  m_fisk = EMPTYSTRING;
  counter = 0;
}

void DisplayPoly::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT1, m_fisk);
}

BEGIN_MESSAGE_MAP(DisplayPoly, CDialog)
    ON_WM_PAINT()
END_MESSAGE_MAP()

void DisplayPoly::OnPaint() {
    CPaintDC dc(this); // device context for painting

    TCHAR tmp[30];

    _stprintf(tmp,_T("Hello %d   "),counter++);
    dc.TextOut(10,10,tmp);

    // Do not call __super::OnPaint() for painting messages
}
