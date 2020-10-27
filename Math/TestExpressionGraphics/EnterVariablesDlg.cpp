#include "stdafx.h"
#include "EnterVariablesDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CEnterVariablesDlg::CEnterVariablesDlg(Expression &expr, CWnd *pParent /*=nullptr*/)
: m_expr(expr)
, CDialog(CEnterVariablesDlg::IDD, pParent)
{
}

void CEnterVariablesDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CEnterVariablesDlg, CDialog)
END_MESSAGE_MAP()

BOOL CEnterVariablesDlg::OnInitDialog() {
  __super::OnInitDialog();

  m_fieldArray.Create(this, m_expr);
  m_fieldArray.putValues(m_expr);
  m_fieldArray.gotoFirstField();

  return FALSE;
}

void CEnterVariablesDlg::OnOK() {
  try {
    m_fieldArray.validate();
  } catch(Exception e) {
    showException(e);
    return;
  }
  const ExpressionVariableArray values = m_fieldArray.getValues();
  for(size_t i = 0; i < values.size(); i++) {
    const ExpressionVariableWithValue &v = values[i];
    m_expr.setValue(v.getName(), v.getValue());
  }
  __super::OnOK();
}
