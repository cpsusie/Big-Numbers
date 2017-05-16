#include "stdafx.h"
#include <MFCUtil/WinTools.h>
#include "Chess.h"
#include "OpeningDlg.h"
#include "SelectOpeningDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

COpeningDlg::COpeningDlg(CWnd* pParent)	: CDialog(COpeningDlg::IDD, pParent) {
}

void COpeningDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(COpeningDlg, CDialog)
  ON_WM_SIZE()
  ON_BN_CLICKED(IDC_BUTTON_EXPAND   , OnButtonExpand)
  ON_BN_CLICKED(IDC_BUTTON_EXPANDALL, OnButtonExpandAll)
  ON_COMMAND(   ID_OPENING_EXPAND   , OnOpeningExpand)
  ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_OPENING, OnSelChangedOpeningTree)
END_MESSAGE_MAP()

BOOL COpeningDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  setControlText(IDD, this);

  m_dialogName = getWindowText(this);
  m_lib.load(IDR_OPENINGLIBRARY);
  if(!m_lib.isEmpty()) {
    m_images.Create(IDB_BITMAPWHITEBLACK, 12,1,RGB(255,255,255));
    Game game;
    CTreeCtrl *ctrl = getTreeCtrl();
    traverse(ctrl, TVI_ROOT, m_lib.getState(0), game);
    ctrl->SetImageList(&m_images,TVSIL_NORMAL);
    ctrl->SetFocus();
    ctrl->SelectItem(ctrl->GetRootItem());
  }

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_TREE_OPENING    , RELATIVE_SIZE );
  m_layoutManager.addControl(IDOK                , RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_BUTTON_EXPAND   , RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_BUTTON_EXPANDALL, RELATIVE_X_POS);

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_OPENING_ACCELERATOR));
  return false;
}

BOOL COpeningDlg::PreTranslateMessage(MSG* pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }
  return CDialog::PreTranslateMessage(pMsg);
}

void COpeningDlg::traverse(CTreeCtrl *ctrl, HTREEITEM p, const LibraryState &state, Game &g) {
  for(size_t i = 0; i < state.m_transitionArray.size(); i++) {
    const LibraryTransition &tr = state.m_transitionArray[i];
    PrintableMove            m  = g.generateMove(tr.m_from,tr.m_to);
    String label = format(_T("%d. %s%s"), g.getPlyCount()/2+1, m.toString(MOVE_SHORTFORMAT).cstr(), tr.m_goodMove?EMPTYSTRING:_T("?"));
    HTREEITEM q = ctrl->InsertItem(label.cstr(), state.m_playerInTurn, state.m_playerInTurn, p);
    g.executeMove(m);
    traverse(ctrl, q, m_lib.getState(tr.m_nextState),g);
    g.unExecuteLastMove();
  }
}

void COpeningDlg::OnButtonExpand() {
  CTreeCtrl *ctrl = getTreeCtrl();
  expandAll(ctrl,ctrl->GetSelectedItem());
  ctrl->SetFocus();
}

void COpeningDlg::OnButtonExpandAll() {
  CTreeCtrl *ctrl = getTreeCtrl();
  int hp = ctrl->GetScrollPos(SB_HORZ);
  int vp = ctrl->GetScrollPos(SB_VERT);
  expandAll(ctrl,TVI_ROOT);
  ctrl->SetFocus();
  ctrl->SetScrollPos(SB_HORZ,hp);
  ctrl->SetScrollPos(SB_VERT,vp);
}

void COpeningDlg::expandAll(CTreeCtrl *ctrl, HTREEITEM p) {
  ctrl->Expand(p,TVE_EXPAND);
  for(HTREEITEM child = ctrl->GetChildItem(p); child != NULL; child = ctrl->GetNextSiblingItem(child)) {
    ctrl->Expand(child,TVE_EXPAND);
    expandAll(ctrl,child);
  }
}

void COpeningDlg::collapseAll(CTreeCtrl *ctrl, HTREEITEM p) {
  ctrl->Expand(p,TVE_COLLAPSE);
  for(HTREEITEM child = ctrl->GetChildItem(p); child != NULL; child = ctrl->GetNextSiblingItem(child)) {
    ctrl->Expand(child,TVE_COLLAPSE);
    collapseAll(ctrl,child);
  }
}

HTREEITEM getChildByIndex(CTreeCtrl *ctrl, HTREEITEM parent, int index) {
  HTREEITEM child = ctrl->GetChildItem(parent);
  for(int i = 0; i < index && child != NULL; i++) {
    child = ctrl->GetNextSiblingItem(child);
  }
  return child;
}

void COpeningDlg::OnSize(UINT nType, int cx, int cy) {
  CDialog::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
}

void COpeningDlg::OnOpeningExpand() {
  CSelectOpeningDlg dlg(m_lib.getAllNames());
  if(dlg.DoModal() == IDOK) {
    CTreeCtrl *ctrl = getTreeCtrl();
    collapseAll(ctrl,TVI_ROOT);
    int nameIndex = dlg.getSelectedIndex();
    HTREEITEM p = TVI_ROOT;
    for(int state = 0;;) {
      int trIndex = m_lib.getState(state).findTransitionIndexByNameIndex(nameIndex);
      if(trIndex < 0) {
        break;
      }
      p = getChildByIndex(ctrl,p,trIndex);
      ctrl->Expand(p,TVE_EXPAND);
      state = m_lib.nextState(state,trIndex);
    }
  }
}

void COpeningDlg::OnSelChangedOpeningTree(NMHDR *pNMHDR, LRESULT *pResult) {
  try {
    setWindowText(this, format(_T("%s:%s"), m_dialogName.cstr(), getActiveOpenings().cstr()));
  } catch(Exception e) {
    MessageBox(e.what());
  }
  *pResult = 0;
}

String COpeningDlg::getActiveOpenings() {
  Game game;
  return m_lib.getActiveOpenings(getSelectedVariant(game)).toString();
}

Game &COpeningDlg::getSelectedVariant(Game &g) {
  CTreeCtrl *ctrl = getTreeCtrl();
  StringArray moves;
  for(HTREEITEM item = ctrl->GetSelectedItem(); item; item = ctrl->GetParentItem(item)) {
    CString f = ctrl->GetItemText(item);
    int n;
    TCHAR ms[256];
    _stscanf((LPCTSTR)f,_T("%d. %s"), &n, ms);
    moves.add(ms);
  }
  moves.reverse();
  for(size_t i = 0; i < moves.size(); i++) {
    g.executeMove(g.generateMove(moves[i]));
  }
  return g;
}
