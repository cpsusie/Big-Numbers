#include "stdafx.h"
#include "showgraf.h"
#include "DataGraphDlg.h"
#include <commctrl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CDataGraphDlg::CDataGraphDlg(DataGraph &g, CWnd* pParent) : m_graph(g), CDialog(CDataGraphDlg::IDD, pParent) {
  m_name  = _T("");
	m_style = _T("");
}

void CDataGraphDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATALIST, m_dataList);
  DDX_Text(pDX, IDC_EDITNAME, m_name);
	DDX_CBString(pDX, IDC_COMBOSTYLE, m_style);
}

BEGIN_MESSAGE_MAP(CDataGraphDlg, CDialog)
END_MESSAGE_MAP()

BOOL CDataGraphDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  
  const GraphParameters &param = m_graph.getParam();
  getColorButton()->SetColor(param.m_color);
  m_fullName = param.getFullName();
  m_name     = param.getPartialName().cstr();
  m_style    = GraphParameters::graphStyleToString(param.m_style);

  const int dataWidth = getWindowRect(&m_dataList).Width()/2-12;
  m_dataList.InsertColumn( 0, _T("X"), LVCFMT_LEFT, dataWidth);
  m_dataList.InsertColumn( 1, _T("Y"), LVCFMT_LEFT, dataWidth);
  m_dataList.SetExtendedStyle(LVS_EX_TRACKSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP);

  const Point2DArray &data = m_graph.getDataPoints();
  for(size_t i = 0; i < data.size(); i++) {
    const Point2D &p = data[i];
    addData(m_dataList, (int)i, 0, format(_T("%lg"), p.x),true);
    addData(m_dataList, (int)i, 1, format(_T("%lg"), p.y));
  }
  UpdateData(FALSE);
  return FALSE;
}

void CDataGraphDlg::OnOK() {
  UpdateData();
  if(m_name.GetLength() == 0) {
    MessageBox(_T("Must specify name"));
    GetDlgItem(IDC_EDITNAME)->SetFocus();   
    return;
  }

  m_graph.getParam().setName((LPCTSTR)m_name);
  m_graph.getParam().m_style = (GraphStyle)getStyleCombo()->GetCurSel();
  m_graph.getParam().m_color = getColorButton()->GetColor();

  CDialog::OnOK();
}
