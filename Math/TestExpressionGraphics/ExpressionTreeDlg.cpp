#include "stdafx.h"
#include <MFCUtil/TreeCtrlWalker.h>
#include "ExpressionTreeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CExpressionTreeDlg::CExpressionTreeDlg(const Expression &expr, CWnd *pParent)
: m_expr(&expr)
, CDialog(CExpressionTreeDlg::IDD, pParent)
, m_extendedInfo(FALSE)
{
  m_node         = expr.getRoot();
  m_extendedInfo = TRUE;
  m_selectedNode = NULL;
}

CExpressionTreeDlg::CExpressionTreeDlg(const ExpressionNode *n, CWnd *pParent)
: m_node(n)
, CDialog(CExpressionTreeDlg::IDD, pParent)
{
  m_expr = (Expression*)&m_node->getTree();
}

void CExpressionTreeDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Check(pDX, IDC_CHECKEXTENDEDINFO, m_extendedInfo);
}

BEGIN_MESSAGE_MAP(CExpressionTreeDlg, CDialog)
    ON_BN_CLICKED(IDCLOSE, OnClose)
    ON_WM_SIZE()
    ON_WM_CONTEXTMENU()
    ON_COMMAND(ID_CLEARBREAKPOINT                , OnClearBreakPoint           )
    ON_COMMAND(ID_SETBREAKPOINT                  , OnSetBreakPoint             )
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_EXPRESSION, OnSelchangedTreeExpression  )
    ON_BN_CLICKED(IDC_CHECKEXTENDEDINFO          , OnBnClickedCheckExtendedInfo)
END_MESSAGE_MAP()


BOOL CExpressionTreeDlg::OnInitDialog() {
  __super::OnInitDialog();

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_TREE_EXPRESSION  , RELATIVE_SIZE );
  m_layoutManager.addControl(IDCLOSE              , RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_CHECKEXTENDEDINFO, RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_LIST_SYMBOLTABLE , RELATIVE_Y_POS | RELATIVE_WIDTH);

  buildTree();

  const String treeFormName = m_expr->getTreeFormName();
  const String stateName    = m_expr->getStateName();
  const int    nodeCount    = m_expr->getNodeCount();

  String title = getWindowText(this);
  title += format(_T(" - %s form - state %s. %d nodes")
                 ,treeFormName.cstr()
                 ,stateName.cstr()
                 ,nodeCount
                 );
  setWindowText(this, title);

  CListBox *lb = (CListBox*)GetDlgItem(IDC_LIST_SYMBOLTABLE);
  const ExpressionVariableArray variables = m_expr->getSymbolTable().getAllVariables();
  for(size_t i = 0; i < variables.size(); i++) {
    const ExpressionVariableWithValue &v = variables[i];
    lb->InsertString(-1, v.toString().cstr());
  }
  return TRUE;
}

void CExpressionTreeDlg::OnClose() {
  OnOK();
}

void CExpressionTreeDlg::buildTree() {
  CTreeCtrl *treeCtrl = getTreeCtrl();
  treeCtrl->DeleteAllItems();
  traverse( treeCtrl, TVI_ROOT, m_node);
  expandAllNodes();
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

static String getSimpleString(const ExpressionNode *n) {
  switch(n->getSymbol()) {
  case NUMBER  :
  case NAME    :
  case TYPEBOOL:
    return n->toString();
  case ADDENT  :
    return format(_T("%c%s"), n->isPositive()?'+':'-',n->getSymbolName().cstr());
  default      :
    return n->getSymbolName();
  }
}

static String getExtendedString(const ExpressionNode *n) {
  switch(n->getSymbol()) {
  case NAME    :
    return format(_T("%s - (%-8s %s)")
                 ,n->getVariable().toString(false).cstr()
                 ,n->getNodeTypeName().cstr()
                 ,n->getInfo().toString().cstr());

  case NUMBER  :
    { const Number &number = n->getNumber();
      return format(_T("%s (%s) - (%-8s %s)")
                   ,number.getTypeName().cstr()
                   ,number.toString().cstr()
                   ,n->getNodeTypeName().cstr()
                   ,n->getInfo().toString().cstr());
    }
  default:
    return format(_T("%s - (%-8s %s)")
                 ,getSimpleString(n).cstr()
                 ,n->getNodeTypeName().cstr()
                 ,n->getInfo().toString().cstr());
  }
}

String CExpressionTreeDlg::getString(const ExpressionNode *n) const {
  return m_extendedInfo ? getExtendedString(n)
                        : getSimpleString(n);
}

#define ADDITEM(n,p) ctrl->InsertItem(getString(n).cstr(),p)

void CExpressionTreeDlg::traverse(CTreeCtrl *ctrl, HTREEITEM p, const ExpressionNode *n) {
  HTREEITEM q;
  switch(n->getSymbol()) {
  case NAME    :
  case NUMBER  :
  case TYPEBOOL:
    q = ADDITEM(n,p);
    break;

  case POLY    :
    { q = ADDITEM(n,p);
      const SNodeArray &coef = n->getCoefArray();
      for(size_t i = 0; i < coef.size(); i++) {
        traverse(ctrl, q, coef[i].node());
      }
      traverse(ctrl, q, n->getArgument().node());
    }
    break;

  default      :
    { q = ADDITEM(n,p);
      const SNodeArray &a = n->getChildArray();
      for(size_t i = 0; i < a.size(); i++) {
        traverse(ctrl, q, a[i].node());
      }
    }
    break;
  }
  ctrl->SetItemData(q, (DWORD_PTR)n);
}

void CExpressionTreeDlg::updateNodeText(const ExpressionNode *n) {
  HTREEITEM item = findItemFromNode(n);
  if(item) {
    getTreeCtrl()->SetItemText(item, getString(n).cstr());
  }
}

void CExpressionTreeDlg::expandAllNodes() {
  TreeItemExpander(true).visitAllItems(getTreeCtrl());
}

void CExpressionTreeDlg::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
}

void CExpressionTreeDlg::OnContextMenu(CWnd *pWnd, CPoint point) {
  setSelectedNode(getNodeFromPoint(point));
  if(m_selectedNode == NULL) return;
  CMenu menu;
  if(!menu.LoadMenu(IDR_CONTEXTMENU_TREE)) {
    showWarning(_T("LoadMenu failed"));
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
  treeCtrl->ScreenToClient(&p);
  HTREEITEM item = findTreeItemByPoint(treeCtrl,p);
  if(item) {
    treeCtrl->SelectItem(item);
    const ExpressionNode *n = (const ExpressionNode*)treeCtrl->GetItemData(item);
    return n;
  }
  return NULL;
}

HTREEITEM CExpressionTreeDlg::findItemFromNode(const ExpressionNode *n) {
  CTreeCtrl *treeCtrl = getTreeCtrl();
  for(HTREEITEM item = treeCtrl->GetFirstVisibleItem(); item; item = treeCtrl->GetNextVisibleItem(item)) {
    const ExpressionNode *itemNode = (const ExpressionNode*)treeCtrl->GetItemData(item);
    if(itemNode == n) {
      return item;
    }
  }
  return NULL;
}

void CExpressionTreeDlg::OnSetBreakPoint() {
  m_selectedNode->setBreakPoint();
  if(m_extendedInfo) updateNodeText(m_selectedNode);
}

void CExpressionTreeDlg::OnClearBreakPoint() {
  m_selectedNode->clearBreakPoint();
  if(m_extendedInfo) updateNodeText(m_selectedNode);
}

void CExpressionTreeDlg::OnSelchangedTreeExpression(NMHDR *pNMHDR, LRESULT *pResult) {
  NM_TREEVIEW *pNMTreeView = (NM_TREEVIEW*)pNMHDR;
  TVITEM &tvItem = ((NMTREEVIEW*)pNMTreeView)->itemNew;
  if(tvItem.hItem) {
    setSelectedNode((const ExpressionNode*)getTreeCtrl()->GetItemData(tvItem.hItem));
  }
  *pResult = 0;
}

class ItemTextUpdater : public TreeCtrlWalker {
private:
  const bool m_extendedText;
  String getString(const ExpressionNode *n) const {
    return m_extendedText ? getExtendedString(n) : getSimpleString(n);
  }
public:
  ItemTextUpdater(bool extendedText) : m_extendedText(extendedText) {
  }
  bool handleItem(HTREEITEM item);
};

bool ItemTextUpdater::handleItem(HTREEITEM item) {
  CTreeCtrl &tree = getTreeCtrl();
  tree.SetItemText(item, getString((const ExpressionNode*)tree.GetItemData(item)).cstr());
  return true;
}

void CExpressionTreeDlg::OnBnClickedCheckExtendedInfo() {
  UpdateData();
  ItemTextUpdater(m_extendedInfo).visitAllItems(getTreeCtrl());
}
