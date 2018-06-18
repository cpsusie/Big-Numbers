#include "pch.h"
#include <MFCUtil/TreeCtrlWalker.h>

bool TreeCtrlWalker::visitItem(HTREEITEM p) {
  if(!handleItem(p)) {
    return false;
  }
  for(HTREEITEM child = m_treeCtrl->GetChildItem(p); child != NULL; child = m_treeCtrl->GetNextSiblingItem(child)) {
    if(!visitItem(child)) {
      return false;
    }
  }
  return true;
}

bool TreeCtrlWalker::visitAllItems(CTreeCtrl *treeCtrl, HTREEITEM startItem) {
  if(startItem==NULL) {
    return true;
  }
  m_treeCtrl = treeCtrl;
  return visitItem(startItem);
}
