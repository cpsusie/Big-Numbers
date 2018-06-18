#pragma once

#include "WinTools.h"

class TreeCtrlWalker {
private:
  CTreeCtrl *m_treeCtrl;
  bool visitItem(HTREEITEM p);
protected:
  CTreeCtrl &getTreeCtrl() const {
    return *m_treeCtrl;
  }
public:
  // Should return false, to terminate tree-scan. true to continue to next node
  virtual bool handleItem(HTREEITEM item) = 0;
  // Visit startItem + childItems (recursive). If startItem is specified, it is assumed to be in treeCtrl.
  // Returns true, if all item's were visited, false if recusive scan was terminated
  bool visitAllItems(CTreeCtrl *treeCtrl, HTREEITEM startItem=TVI_ROOT);
};

class TreeItemExpander : public TreeCtrlWalker {
private:
  const bool m_expand;
public:
  // if expand all visited nodes will be expanded, else they will be collapsed
  TreeItemExpander(bool expand) : m_expand(expand) {
  }
  bool handleItem(HTREEITEM item) {
    if(m_expand) getTreeCtrl().Expand(item, TVE_EXPAND);
    else getTreeCtrl().Expand(item,TVE_COLLAPSE);
    return true;
  }
};
