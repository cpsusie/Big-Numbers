#include "stdafx.h"
#include <MFCUtil/TreeCtrlWalker.h>
#include "TreeDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

TreeDlg::TreeDlg(SyntaxNodep tree, CWnd *pParent) : m_tree(tree), CDialog(TreeDlg::IDD, pParent) {
}

BEGIN_MESSAGE_MAP(TreeDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTONEXPAND   , OnButtonExpand    )
	ON_BN_CLICKED(IDC_BUTTONEXPANDALL, OnButtonExpandAll )
	ON_COMMAND(ID_COLLAPSE           , OnCollapse        )
	ON_COMMAND(ID_EXPAND             , OnExpand          )
	ON_COMMAND(ID_EXPANDORCOLLAPSE   , OnExpandOrCollapse)
END_MESSAGE_MAP()

void TreeDlg::traverse(CTreeCtrl *ctrl, SyntaxNodep tree, HTREEITEM p) {
  HTREEITEM q;
  if(tree->isTerminal()) {
    q = ctrl->InsertItem(tree->getSymbol(), 0, 0, p);
  } else {
    q = ctrl->InsertItem(tree->getSymbol(), 1, 1, p);
  }
  for(UINT i = 0; i < tree->getChildCount(); i++) {
    traverse(ctrl, tree->getChild(i), q);
  }
}

BOOL TreeDlg::OnInitDialog() {
  __super::OnInitDialog();
  m_accelTable = LoadAccelerators(theApp.m_hInstance, MAKEINTRESOURCE(IDR_ACCELERATORTREE));

  CTreeCtrl *ctrl = getTreeCtrl();
  m_images.Create(IDB_BITMAPTERMINAL, 13, 1, RGB(255, 255, 255));
  ctrl->SetImageList(&m_images, TVSIL_NORMAL);
  traverse(ctrl, m_tree, TVI_ROOT);
  ctrl->SetFocus();
  ctrl->SelectItem(ctrl->GetRootItem());
  return false;
}

void TreeDlg::OnButtonExpand() {
  CTreeCtrl *ctrl = getTreeCtrl();
  TreeItemExpander(true).visitAllItems(ctrl, ctrl->GetSelectedItem());
  ctrl->SetFocus();
}

void TreeDlg::OnCollapse() {
  CTreeCtrl *ctrl = getTreeCtrl();
  ctrl->Expand(ctrl->GetSelectedItem(), TVE_COLLAPSE);
  ctrl->SetFocus();
}

void TreeDlg::OnExpand() {
  CTreeCtrl *ctrl = getTreeCtrl();
  ctrl->Expand(ctrl->GetSelectedItem(), TVE_EXPAND);
  ctrl->SetFocus();
}

void TreeDlg::OnButtonExpandAll() {
  CTreeCtrl *ctrl = getTreeCtrl();
  int hp = ctrl->GetScrollPos(SB_HORZ);
  int vp = ctrl->GetScrollPos(SB_VERT);
  TreeItemExpander(true).visitAllItems(ctrl);
  ctrl->SetFocus();
  ctrl->SetScrollPos(SB_HORZ, hp);
  ctrl->SetScrollPos(SB_VERT, vp);
}

CTreeCtrl *TreeDlg::getTreeCtrl() {
  return (CTreeCtrl*)GetDlgItem(IDC_DERIVATIONTREE);
}

BOOL TreeDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }

  switch(pMsg->message) {
  case WM_RBUTTONDOWN:
    { CTreeCtrl *ctrl = getTreeCtrl();
      CPoint     p    = pMsg->pt;
      ctrl->ScreenToClient(&p);
      HTREEITEM item = findTreeItemByPoint(ctrl, p);
      if(item != nullptr) {
        const String derivation = getDerivation(ctrl, item, 160);
        showInformation(derivation);
        return TRUE;
      }
    }
    break;
  }
  return __super::PreTranslateMessage(pMsg);
}

void TreeDlg::OnOK() {
  __super::OnCancel();
}

void TreeDlg::OnExpandOrCollapse() {
  CTreeCtrl *ctrl = getTreeCtrl();
  HTREEITEM item = ctrl->GetSelectedItem();
  UINT state;
  if((state = ctrl->GetItemState(item, TVIS_EXPANDED)) & TVIS_EXPANDED) {
    OnCollapse();
  } else {
    OnExpand();
  }
}

void TreeDlg::getDerivation(CTreeCtrl *ctrl, HTREEITEM item, String &derivation, int maxlen) {
  int image, selectedimage;
  ctrl->GetItemImage(item, image, selectedimage);
  if(image == 0) { // item is a terminal
    CString s = ctrl->GetItemText(item);
    maxlen -= (int)derivation.length();
    if(maxlen < 0) return;
    derivation += s.GetBuffer(maxlen);
    return;
  }
  for(HTREEITEM child = ctrl->GetChildItem(item); child != nullptr; child = ctrl->GetNextSiblingItem(child)) { // item is a nonterminal
    String tmp;
    getDerivation(ctrl, child, tmp, maxlen);
    if(derivation.length() > 0) derivation += _T(" ");
    derivation += tmp;
    if((int)derivation.length() > maxlen) {
      derivation = left(derivation, maxlen);
      return;
    }
  }
}

String TreeDlg::getDerivation(CTreeCtrl *ctrl, HTREEITEM item, int maxlen) {
  String result;
  getDerivation(ctrl, item, result, maxlen+10);
  if((int)result.length() > maxlen) {
    result = left(result, maxlen-3) + _T("...");
  }
  return result;
}
