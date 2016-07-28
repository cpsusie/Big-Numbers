#include "stdafx.h"
#include "ParserDemo.h"
#include "TreeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

TreeDlg::TreeDlg(SyntaxNodep tree, CWnd *pParent) : m_tree(tree), CDialog(TreeDlg::IDD, pParent) {
}

void TreeDlg::DoDataExchange(CDataExchange *pDX) {
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(TreeDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTONEXPAND, OnButtonExpand)
	ON_BN_CLICKED(IDC_BUTTONEXPANDALL, OnButtonExpandAll)
	ON_WM_SIZE()
	ON_COMMAND(ID_COLLAPSE, OnCollapse)
	ON_COMMAND(ID_EXPAND, OnExpand)
	ON_COMMAND(ID_EXPANDORCOLLAPSE, OnExpandOrCollapse)
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
  CDialog::OnInitDialog();
  m_accelTable = LoadAccelerators(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_ACCELERATORTREE));

  CTreeCtrl *ctrl = (CTreeCtrl*)GetDlgItem(IDC_DERIVATIONTREE);
  m_images.Create(IDB_BITMAPTERMINAL, 13, 1, RGB(255, 255, 255));
  ctrl->SetImageList(&m_images, TVSIL_NORMAL);
  traverse(ctrl, m_tree, TVI_ROOT);
  ctrl->SetFocus();
  ctrl->SelectItem(ctrl->GetRootItem());

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_DERIVATIONTREE , RELATIVE_SIZE );
  m_layoutManager.addControl(IDOK               , RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_BUTTONEXPAND   , RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_BUTTONEXPANDALL, RELATIVE_X_POS);

  return false;
}

void TreeDlg::expandAll(CTreeCtrl *ctrl, HTREEITEM p) {
  ctrl->Expand(p, TVE_EXPAND);
  for(HTREEITEM child = ctrl->GetChildItem(p); child != NULL; child = ctrl->GetNextSiblingItem(child)) {
    ctrl->Expand(child, TVE_EXPAND);
    expandAll(ctrl, child);
  }
}

void TreeDlg::OnButtonExpand() {
  CTreeCtrl *ctrl = (CTreeCtrl*)GetDlgItem(IDC_DERIVATIONTREE);
  expandAll(ctrl, ctrl->GetSelectedItem());
  ctrl->SetFocus();
}

void TreeDlg::OnCollapse() {
  CTreeCtrl *ctrl = (CTreeCtrl*)GetDlgItem(IDC_DERIVATIONTREE);
  ctrl->Expand(ctrl->GetSelectedItem(), TVE_COLLAPSE);
  ctrl->SetFocus();
}

void TreeDlg::OnExpand() {
  CTreeCtrl *ctrl = (CTreeCtrl*)GetDlgItem(IDC_DERIVATIONTREE);
  ctrl->Expand(ctrl->GetSelectedItem(), TVE_EXPAND);
  ctrl->SetFocus();
}

void TreeDlg::OnButtonExpandAll() {
  CTreeCtrl *ctrl = (CTreeCtrl*)GetDlgItem(IDC_DERIVATIONTREE);
  int hp = ctrl->GetScrollPos(SB_HORZ);
  int vp = ctrl->GetScrollPos(SB_VERT);
  expandAll(ctrl, ctrl->GetRootItem());
  ctrl->SetFocus();
  ctrl->SetScrollPos(SB_HORZ, hp);
  ctrl->SetScrollPos(SB_VERT, vp);
}

HTREEITEM TreeDlg::findTreeItem(CTreeCtrl *ctrl, const CPoint &pt) {
  CPoint p = pt;
  ctrl->ScreenToClient(&p);
  for(HTREEITEM item = ctrl->GetFirstVisibleItem(); item; item = ctrl->GetNextVisibleItem(item)) {
    CRect r;
    ctrl->GetItemRect(item, &r, false);
    if(r.PtInRect(p)) {
      return item;
    }
  }
  return NULL;
}

BOOL TreeDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg))
    return true;

  switch(pMsg->message) {
  case WM_RBUTTONDOWN:
    { CTreeCtrl *ctrl = (CTreeCtrl*)GetDlgItem(IDC_DERIVATIONTREE);
      HTREEITEM item = findTreeItem(ctrl, pMsg->pt);
      if(item != NULL) {
        String derivation = getDerivation(ctrl, item, 160);
        MessageBox(derivation.cstr(), _T("Derived String"));
        break;
      }
    }
    break;
  }
  return CDialog::PreTranslateMessage(pMsg);
}

void TreeDlg::OnSize(UINT nType, int cx, int cy) {
  CDialog::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
}

void TreeDlg::OnOK() {
  CDialog::OnCancel();
}

void TreeDlg::OnExpandOrCollapse() {
  CTreeCtrl *ctrl = (CTreeCtrl*)GetDlgItem(IDC_DERIVATIONTREE);
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
  for(HTREEITEM child = ctrl->GetChildItem(item); child != NULL; child = ctrl->GetNextSiblingItem(child)) { // item is a nonterminal
    String tmp;
    getDerivation(ctrl, child, tmp, maxlen);
    if(derivation.length() > 0) derivation += _T(" ");
    derivation += tmp;
    if(derivation.length() > maxlen) {
      derivation = left(derivation, maxlen);
      return;
    }
  }
}

String TreeDlg::getDerivation(CTreeCtrl *ctrl, HTREEITEM item, int maxlen) {
  String result;
  getDerivation(ctrl, item, result, maxlen+10);
  if(result.length() > maxlen) {
    result = left(result, maxlen-3) + _T("...");
  }
  return result;
}
