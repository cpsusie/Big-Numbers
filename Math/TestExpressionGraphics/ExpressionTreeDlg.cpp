#include "stdafx.h"
#include <MFCUtil/TreeCtrlWalker.h>
#include "ExpressionTreeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CExpressionTreeDlg::CExpressionTreeDlg(const Expression &expr, CWnd *pParent)
: CDialog(IDD, pParent)
, m_expr(expr)
, m_node(expr.getRoot())
, m_extendedInfo(FALSE)
, m_selectedNode(NULL)
{
}

CExpressionTreeDlg::CExpressionTreeDlg(const ExpressionNode *n, CWnd *pParent)
: CDialog(IDD, pParent)
, m_expr(*(Expression*)&n->getTree())
, m_node(n)
, m_extendedInfo(FALSE)
, m_selectedNode(NULL)
{
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

  m_treeCtrl.substituteControl(this, IDC_TREE_EXPRESSION);

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_TREE_EXPRESSION  , RELATIVE_SIZE );
  m_layoutManager.addControl(IDCLOSE              , RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_CHECKEXTENDEDINFO, RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_LIST_SYMBOLTABLE , RELATIVE_Y_POS | RELATIVE_WIDTH);

  m_treeCtrl.showTree(m_node, m_extendedInfo);
  ParserTree *tree = m_expr.getTree();
  const String treeFormName = tree->getTreeFormName();
  const String stateName    = tree->getStateName();
  const UINT   nodeCount    = tree->getNodeCount();

  String title = getWindowText(this);
  title += format(_T(" - %s form - state %s. %u nodes")
                 ,treeFormName.cstr()
                 ,stateName.cstr()
                 ,nodeCount
                 );
  setWindowText(this, title);

  CListBox *lb = (CListBox*)GetDlgItem(IDC_LIST_SYMBOLTABLE);
  const ExpressionVariableArray variables = tree->getSymbolTable().getAllVariables();
  for(size_t i = 0; i < variables.size(); i++) {
    const ExpressionVariableWithValue &v = variables[i];
    lb->InsertString(-1, v.toString().cstr());
  }
  return TRUE;
}

void CExpressionTreeDlg::OnClose() {
  OnOK();
}

void CExpressionTreeDlg::setSelectedNode(const ExpressionNode *selectedNode) { // property
  const ExpressionNode *oldSelected = m_selectedNode;
  if(selectedNode != oldSelected) {
    m_selectedNode = (ExpressionNode*)selectedNode;
    notifyPropertyChanged(TREE_SELECTEDNODE, oldSelected, m_selectedNode);
  }
}

void CExpressionTreeDlg::updateNodeText(const ExpressionNode *n) {
  HTREEITEM item = findItemFromNode(n);
  if(item) {
    m_treeCtrl.updateItemText(item);
  }
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
  m_treeCtrl.ScreenToClient(&p);
  HTREEITEM item = findTreeItemByPoint(&m_treeCtrl,p);
  if(item) {
    m_treeCtrl.SelectItem(item);
    const ExpressionNode *n = (const ExpressionNode*)m_treeCtrl.GetItemData(item);
    return n;
  }
  return NULL;
}

HTREEITEM CExpressionTreeDlg::findItemFromNode(const ExpressionNode *n) {
  for(HTREEITEM item = m_treeCtrl.GetFirstVisibleItem(); item; item = m_treeCtrl.GetNextVisibleItem(item)) {
    const ExpressionNode *itemNode = (const ExpressionNode*)m_treeCtrl.GetItemData(item);
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
    setSelectedNode((const ExpressionNode*)m_treeCtrl.GetItemData(tvItem.hItem));
  }
  *pResult = 0;
}


void CExpressionTreeDlg::OnBnClickedCheckExtendedInfo() {
  UpdateData();
  m_treeCtrl.setExtendedText(m_extendedInfo);
}
