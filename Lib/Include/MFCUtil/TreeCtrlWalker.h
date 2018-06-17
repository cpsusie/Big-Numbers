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
  virtual bool handleItem(HTREEITEM item) = 0; // return false, to terminate tree-scan. true to continue to next node
  bool visitAllItems(CTreeCtrl *treeCtrl); // returns true, if all item's were scanned, false if scan was terminated
};
