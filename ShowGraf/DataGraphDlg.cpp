#include "stdafx.h"
#include "showgraf.h"
#include "DataGraphDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CDataGraphDlg::CDataGraphDlg(DataGraph &g, CWnd *pParent)
: m_graph(g)
, m_model(g)
, CDialog(CDataGraphDlg::IDD, pParent)
{
  m_name  = EMPTYSTRING;
  m_style = EMPTYSTRING;
}

void CDataGraphDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITNAME, m_name);
  DDX_CBString(pDX, IDC_COMBOSTYLE, m_style);
}

BEGIN_MESSAGE_MAP(CDataGraphDlg, CDialog)
  ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CDataGraphDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  m_list.substituteControl(this, IDC_DATALIST, m_model);

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_DATALIST, RELATIVE_SIZE     | RESIZE_LISTHEADERS | INIT_LISTHEADERS);
  m_layoutManager.addControl(IDOK        , RELATIVE_POSITION );
  m_layoutManager.addControl(IDCANCEL    , RELATIVE_POSITION );

  const GraphParameters &param = m_graph.getParam();
  getColorButton()->SetColor(param.m_color);
  m_fullName = param.getName();
  m_name     = param.getDisplayName().cstr();
  m_style    = GraphParameters::graphStyleToString(param.m_style);

  UpdateData(FALSE);
  return FALSE;
}

void CDataGraphDlg::OnOK() {
  if(!UpdateData()) return;
  if(m_name.GetLength() == 0) {
    MessageBox(_T("Must specify name"));
    GetDlgItem(IDC_EDITNAME)->SetFocus();   
    return;
  }

  m_graph.getParam().setName((LPCTSTR)m_name);
  m_graph.getParam().m_style = (GraphStyle)getStyleCombo()->GetCurSel();
  m_graph.getParam().m_color = getColorButton()->GetColor();
  m_graph.setDataPoints(m_model.getData());
  CDialog::OnOK();
}

void CDataGraphDlg::OnSize(UINT nType, int cx, int cy) {
  CDialog::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
}
