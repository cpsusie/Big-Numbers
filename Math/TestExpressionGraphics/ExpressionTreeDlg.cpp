#include "stdafx.h"
#include "ExpressionTreeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CExpressionTreeDlg::CExpressionTreeDlg(const Expression &expr, CWnd *pParent) : m_expr(&expr), CDialog(CExpressionTreeDlg::IDD, pParent) {
  m_node = expr.getRoot();
  m_selectedNode = NULL;
}

CExpressionTreeDlg::CExpressionTreeDlg(const ExpressionNode *n, CWnd *pParent) : m_node(n), CDialog(CExpressionTreeDlg::IDD, pParent) {
  m_expr = m_node->getExpr();
}

void CExpressionTreeDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CExpressionTreeDlg, CDialog)
    ON_BN_CLICKED(IDCLOSE, OnClose)
    ON_WM_SIZE()
    ON_WM_CONTEXTMENU()
    ON_COMMAND(ID_CLEARBREAKPOINT, OnClearBreakPoint)
    ON_COMMAND(ID_SETBREAKPOINT, OnSetBreakPoint)
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_EXPRESSION, OnSelchangedTreeExpression)
END_MESSAGE_MAP()


BOOL CExpressionTreeDlg::OnInitDialog() {
  __super::OnInitDialog();

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_TREE_EXPRESSION , RELATIVE_SIZE );
  m_layoutManager.addControl(IDCLOSE             , RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_LIST_SYMBOLTABLE, RELATIVE_Y_POS | RELATIVE_WIDTH);

  CTreeCtrl *treeCtrl = getTreeCtrl();
  traverse( treeCtrl, TVI_ROOT, m_node);

  const String treeFormName = m_expr->getTreeFormName();
  String title = getWindowText(this);
  title += format(_T(" - %s form"), treeFormName.cstr());
  setWindowText(this, title);

  expandAll(treeCtrl, TVI_ROOT);

  CListBox *lb = (CListBox*)GetDlgItem(IDC_LIST_SYMBOLTABLE);
  const ExpressionVariableArray variables = m_expr->getAllVariables();
  for(size_t i = 0; i < variables.size(); i++) {
    const ExpressionVariableWithValue &v = variables[i];
    lb->InsertString(-1, v.toString().cstr());
  }
  return TRUE;
}

void CExpressionTreeDlg::OnClose() {
  OnOK();
}

CTreeCtrl *CExpressionTreeDlg::getTreeCtrl() {
  return (CTreeCtrl*)GetDlgItem(IDC_TREE_EXPRESSION);
}

void CExpressionTreeDlg::setSelectedNode(const ExpressionNode *selectedNode) { // property
  const ExpressionNode *oldSelected = m_selectedNode;
  if(selectedNode != oldSelected) {
    m_selectedNode = (ExpressionNode*)selectedNode;
    notifyPropertyChanged(TREE_SELECTEDNODE, oldSelected, m_selectedNode);
  }
}

void CExpressionTreeDlg::traverse(CTreeCtrl *ctrl, HTREEITEM p, const ExpressionNode *n) {
  HTREEITEM q;
  switch(n->getSymbol()) {
  case NAME    :
  case NUMBER  :
    q = ctrl->InsertItem(n->toString().cstr(), p);
    break;

  case SUM     :
    { q = ctrl->InsertItem(_T("SUM"), p);
      const AddentArray &a = n->getAddentArray();
      for(size_t i = 0; i < a.size(); i++) {
        const SumElement *e = a[i];
        TCHAR *signStr = e->isPositive() ? _T("+") : _T("-");
        HTREEITEM q1 = ctrl->InsertItem(signStr, q);
        traverse(ctrl, q1, e->getNode());
      }
    }
    break;

  case PRODUCT :
    { q = ctrl->InsertItem(_T("PRODUCT"), p);
      const FactorArray &a = n->getFactorArray();
      for(size_t i = 0; i < a.size(); i++) {
        traverse(ctrl, q, a[i]);
      }
    }
    break;
  case POLY    :
    { q = ctrl->InsertItem(n->getSymbolName().cstr(), p);
      const ExpressionNodeArray &coef = n->getCoefficientArray();
      for(size_t i = 0; i < coef.size(); i++) {
        traverse(ctrl, q, coef[i]);
      }
      traverse(ctrl, q, n->getArgument());
    }
    break;
  default      :
    { q = ctrl->InsertItem(n->getSymbolName().cstr(), p);
      const ExpressionNodeArray &a = n->getChildArray();
      for(size_t i = 0; i < a.size(); i++) {
        traverse(ctrl, q, a[i]);
      }
    }
    break;
  }
  ctrl->SetItemData(q, (DWORD_PTR)n);
}

void CExpressionTreeDlg::expandAll(CTreeCtrl *ctrl, HTREEITEM p) {
  ctrl->Expand(p, TVE_EXPAND);
  for(HTREEITEM child = ctrl->GetChildItem(p); child != NULL; child = ctrl->GetNextSiblingItem(child)) {
    ctrl->Expand(child,TVE_EXPAND);
    expandAll(ctrl,child);
  }
}
void CExpressionTreeDlg::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
}

void CExpressionTreeDlg::OnContextMenu(CWnd *pWnd, CPoint point) {
  CPoint treePoint = point;
  CTreeCtrl *treeCtrl = getTreeCtrl();
  treeCtrl->ScreenToClient(&treePoint);
  setSelectedNode(getNodeFromPoint(treePoint));
  if(m_selectedNode == NULL) return;
  CMenu menu;
  if(!menu.LoadMenu(IDR_CONTEXTMENU_TREE)) {
    MessageBox(_T("LoadMenu failed"), _T("Error"), MB_ICONWARNING);
    return;
  }
  if(m_selectedNode->isBreakPoint()) {
    removeMenuItem(menu, ID_SETBREAKPOINT);
  } else {
    removeMenuItem(menu, ID_CLEARBREAKPOINT);
  }
  menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x,point.y, this);
}

const ExpressionNode *CExpressionTreeDlg::getNodeFromPoint(CPoint p) {
  CTreeCtrl *treeCtrl = getTreeCtrl();
  for(HTREEITEM item = treeCtrl->GetFirstVisibleItem(); item; item = treeCtrl->GetNextVisibleItem(item)) {
    CRect rect;
    treeCtrl->GetItemRect(item, &rect, true);
    if(rect.PtInRect(p)) {
      treeCtrl->SelectItem(item);
      const ExpressionNode *n = (const ExpressionNode*)treeCtrl->GetItemData(item);
      return n;
    }
  }
  return NULL;
}

void CExpressionTreeDlg::OnSetBreakPoint() {
  m_selectedNode->setBreakPoint();
}

void CExpressionTreeDlg::OnClearBreakPoint() {
  m_selectedNode->clearBreakPoint();
}


void CExpressionTreeDlg::OnSelchangedTreeExpression(NMHDR *pNMHDR, LRESULT *pResult) {
  NM_TREEVIEW *pNMTreeView = (NM_TREEVIEW*)pNMHDR;
  TVITEM &tvItem = ((NMTREEVIEW*)pNMTreeView)->itemNew;
  if(tvItem.hItem) {
    setSelectedNode((const ExpressionNode*)getTreeCtrl()->GetItemData(tvItem.hItem));
  }
  *pResult = 0;
}
