#include "stdafx.h"
#include <MFCUtil/TreeCtrlWalker.h>
#include "TestExpressionGraphics.h"
#include "ExpressionTreeCtrl.h"

IMPLEMENT_DYNAMIC(CExpressionTreeCtrl, CTreeCtrl)

CExpressionTreeCtrl::CExpressionTreeCtrl() {
  m_extendedText = false;
  m_tree         = NULL;
}

CExpressionTreeCtrl::~CExpressionTreeCtrl() {
}

BEGIN_MESSAGE_MAP(CExpressionTreeCtrl, CTreeCtrl)
END_MESSAGE_MAP()

void CExpressionTreeCtrl::substituteControl(CWnd *parent, int id) {
  DEFINEMETHODNAME;

  CTreeCtrl *ctrl = (CTreeCtrl*)parent->GetDlgItem(id);
  if(ctrl == NULL) {
    showError(_T("%s:Control with id=%d does not exist"), method, id);
    return;
  }

  const CompactIntArray origTabOrder = getTabOrder(parent);

  DWORD       style   = ctrl->GetStyle();
  DWORD       exStyle = ctrl->GetExStyle();
  const CRect rect    = getWindowRect(ctrl);
  CFont      *font    = ctrl->GetFont();
  if(font == NULL) {
    font = parent->GetFont();
  }
  ctrl->DestroyWindow();

  if(!Create(style, rect, parent, id)) {
    showError(_T("%s::Create failed"), method);
    return;
  }
  setTabOrder(parent, origTabOrder);
  ModifyStyleEx(0, exStyle);
  SetFont(font);
}

void CExpressionTreeCtrl::showTree(const ExpressionNode *n, bool extendedText) {
  DeleteAllItems();
  m_tree = NULL;
  if(n != NULL) {
    m_extendedText = extendedText;
    traverse(TVI_ROOT, n);
    expandAllNodes();
    m_tree = &n->getTree();
  }
}

void CExpressionTreeCtrl::expandAllNodes() {
  TreeItemExpander(true).visitAllItems(this);
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

static String getNodeString(const ExpressionNode *n, bool extended) {
  return extended ? getExtendedString(n) : getSimpleString(n);
}

#define ADDITEM(n,p) InsertItem(getNodeString(n,m_extendedText).cstr(),p)

void CExpressionTreeCtrl::traverse(HTREEITEM p, const ExpressionNode *n) {
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
        traverse(q, coef[i].node());
      }
      traverse(q, n->getArgument().node());
    }
    break;

  default      :
    { q = ADDITEM(n,p);
      const SNodeArray &a = n->getChildArray();
      for(size_t i = 0; i < a.size(); i++) {
        traverse(q, a[i].node());
      }
    }
    break;
  }
  SetItemData(q, (DWORD_PTR)n);
}

void CExpressionTreeCtrl::updateItemText(HTREEITEM item) {
  if(item) {
    const ExpressionNode *n = (ExpressionNode*)GetItemData(item);
    SetItemText(item, getNodeString(n,m_extendedText).cstr());
  }
}

class ItemTextUpdater : public TreeCtrlWalker {
private:
  const bool m_extendedText;
public:
  ItemTextUpdater(bool extendedText) : m_extendedText(extendedText) {
  }
  bool handleItem(HTREEITEM item);
};

bool ItemTextUpdater::handleItem(HTREEITEM item) {
  if(item == TVI_ROOT) return true;
  CTreeCtrl &treeCtrl = getTreeCtrl();
  const ExpressionNode *n = (const ExpressionNode*)treeCtrl.GetItemData(item);
  treeCtrl.SetItemText(item, getNodeString(n,m_extendedText).cstr());
  return true;
}

void CExpressionTreeCtrl::setExtendedText(bool extendedText) {
  if(extendedText != m_extendedText) {
    m_extendedText = extendedText;
    ItemTextUpdater(m_extendedText).visitAllItems(this);
  }
}

