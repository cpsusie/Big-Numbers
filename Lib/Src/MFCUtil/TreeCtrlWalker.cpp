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

// pt is assumed to be relative to wnd
HTREEITEM findTreeItemByPoint(const CWnd *wnd, int id, const CPoint &pt) {
  CTreeCtrl *treeCtrl = (CTreeCtrl*)(wnd->GetDlgItem(id));
  if(treeCtrl == NULL) return NULL;
  CPoint p = pt;
  wnd->ClientToScreen(&p);
  treeCtrl->ScreenToClient(&p);
  return findTreeItemByPoint(treeCtrl,p);
}

HTREEITEM findTreeItemByPoint(const CTreeCtrl *ctrl, const CPoint &pt) {
  for(HTREEITEM item = ctrl->GetFirstVisibleItem(); item; item = ctrl->GetNextVisibleItem(item)) {
    CRect r;
    ctrl->GetItemRect(item, &r, false);
    if(r.PtInRect(pt)) {
      return item;
    }
  }
  return NULL;
}
