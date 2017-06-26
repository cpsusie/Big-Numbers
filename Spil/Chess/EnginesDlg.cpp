#include "stdafx.h"
#include <MFCUtil/WinTools.h>
#include "ChessGraphics.h"
#include "ExternEngine.h"
#include "EnginesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CEnginesDlg::CEnginesDlg(CWnd* pParent /*=NULL*/) : CDialog(CEnginesDlg::IDD, pParent) {
}

void CEnginesDlg::DoDataExchange(CDataExchange* pDX) {
    __super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_ENGINES, m_listCtrl);
}

BEGIN_MESSAGE_MAP(CEnginesDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_ADD     , OnButtonAdd)
    ON_BN_CLICKED(IDC_BUTTON_DELETE  , OnButtonDelete)
    ON_BN_CLICKED(IDC_BUTTON_CHECK   , OnButtonCheck)
    ON_BN_CLICKED(IDC_BUTTON_MOVEUP  , OnButtonMoveUp)
    ON_BN_CLICKED(IDC_BUTTON_MOVEDOWN, OnButtonMoveDown)
    ON_NOTIFY(LVN_ITEMCHANGED        , IDC_LIST_ENGINES, OnItemChangedList )
    ON_WM_CREATE()
END_MESSAGE_MAP()

int CEnginesDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if(__super::OnCreate(lpCreateStruct) == -1) {
     return -1;
  }

  m_moveUpButton.Create(  this, OBMIMAGE(UPARROW), CPoint(0 ,0), IDC_BUTTON_MOVEUP);
  m_moveDownButton.Create(this, OBMIMAGE(DNARROW), CPoint(20,0), IDC_BUTTON_MOVEDOWN);
  return 0;
}

BOOL CEnginesDlg::OnInitDialog() {
  __super::OnInitDialog();
  setControlText(IDD, this);

  m_engineList = Options::getEngineRegister();
  m_listCtrl.SetExtendedStyle(LVS_EX_TRACKSELECT | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
  m_listCtrl.InsertColumn( 0,loadString(IDS_NAME  ).cstr(),LVCFMT_LEFT, 200);
  m_listCtrl.InsertColumn( 1,loadString(IDS_AUTHOR).cstr(),LVCFMT_LEFT, 206);
  m_listCtrl.InsertColumn( 2,loadString(IDS_PATH  ).cstr(),LVCFMT_LEFT, 300);
  const CRect lr = getWindowRect(&m_listCtrl);
  setWindowPosition(this, IDC_BUTTON_MOVEUP  , CPoint(lr.right + 10, (lr.top*2 + lr.bottom  ) / 3));
  setWindowPosition(this, IDC_BUTTON_MOVEDOWN, CPoint(lr.right + 10, (lr.top   + lr.bottom*2) / 3));

  updateListCtrl();
  GetDlgItem(IDC_LIST_ENGINES)->SetFocus();
  setSelectedIndex(m_listCtrl, 0);
  ajourButtons();

  m_origList = m_engineList;

  return FALSE;
}

bool CEnginesDlg::isChanged() const {
  return m_engineList != m_origList;
}

void CEnginesDlg::OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult) {
  NM_LISTVIEW &listView = *(NM_LISTVIEW*)pNMHDR;
  if ((listView.uChanged & LVIF_STATE) && (listView.uNewState & LVNI_SELECTED)) {
    ajourButtons(listView.iItem);
  }
}

void CEnginesDlg::updateListCtrl() {
  m_listCtrl.DeleteAllItems();
  for(UINT i = 0; i < m_engineList.size(); i++) {
    const EngineDescription &desc = m_engineList[i];
    addData(m_listCtrl, i, 0, desc.getName(), true);
    addData(m_listCtrl, i, 1, desc.getAuthor());
    addData(m_listCtrl, i, 2, desc.getPath());
  }
  Invalidate(FALSE);
}

int CEnginesDlg::getSelectedIndex() {
  return m_listCtrl.GetSelectionMark();
}

int CEnginesDlg::getListSize() {
  return m_listCtrl.GetItemCount();
}

void CEnginesDlg::ajourButtons(int selected) {
  if(selected < 0) {
    selected = getSelectedIndex();
  }
  GetDlgItem(IDC_BUTTON_DELETE  )->EnableWindow((selected >= 0)?TRUE:FALSE);
  GetDlgItem(IDC_BUTTON_MOVEUP  )->EnableWindow((selected >  0)?TRUE:FALSE);
  GetDlgItem(IDC_BUTTON_MOVEDOWN)->EnableWindow(((selected >= 0) && (selected < getListSize()-1))?TRUE:FALSE);
}

void CEnginesDlg::OnButtonMoveUp() {
  const int index = getSelectedIndex();
  if(index > 0) {
    m_engineList.swap(index, index-1);
    updateListCtrl();
    setSelectedIndex(m_listCtrl, index - 1);
  }
}

void CEnginesDlg::OnButtonMoveDown() {
  const int index = getSelectedIndex();
  if(index < getListSize() - 1) {
    m_engineList.swap(index, index+1);
    updateListCtrl();
    setSelectedIndex(m_listCtrl, index + 1);
  }
}

void CEnginesDlg::OnButtonAdd() {
#ifndef TABLEBASE_BUILDER
  try {
    const String title      = loadString(IDS_SELECTEXEFILETITLE);
    const String extensions = format(_T("%s%c*.exe%c%s%c*.*%c%c")
                                    ,loadString(IDS_EXEFILEEXTENSION).cstr() , 0, 0
                                    ,loadString(IDS_ALLFILESEXTENSION).cstr(), 0, 0
                                    ,0);

    CFileDialog dlg(TRUE);
    dlg.m_ofn.lpstrTitle  = title.cstr();
    dlg.m_ofn.lpstrFilter = extensions.cstr();
  //  Options &options = getOptions();
  //  const FileNameSplitter info(options.getExternEnginePath(player));
  //  TCHAR dir[256], fileName[256];

  //  _tcscpy(dir     , info.getDir().cstr());
  //  _tcscpy(fileName, info.getFullPath().cstr());
    dlg.m_ofn.Flags |= OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_ENABLESIZING;
  //  dlg.m_ofn.lpstrInitialDir = dir;
  //  dlg.m_ofn.lpstrFile       = fileName;

    if(dlg.DoModal() == IDOK && (_tcslen(dlg.m_ofn.lpstrFile) > 0)) {
      m_engineList.add(ExternEngine::getUCIReply(dlg.m_ofn.lpstrFile));
      updateListCtrl();
      setSelectedIndex(m_listCtrl, (int)m_engineList.size() - 1);
    }
  } catch(Exception e) {
    MessageBox(e.what(), _T("Error"), MB_ICONWARNING);
  }
#else
  MessageBox(_T("Cannot add extern engines in BUILDER_MODE"), _T("Error"), MB_ICONEXCLAMATION);
#endif
}

void CEnginesDlg::OnButtonDelete() {
  const int selected = getSelectedIndex();
  if(selected >= 0) {
    const bool wasLast = selected == getListSize()-1;
    m_engineList.removeIndex(selected);
    updateListCtrl();

    if(wasLast && (selected > 0)) {
      setSelectedIndex(m_listCtrl, selected-1);
    }
  }
}

void CEnginesDlg::OnButtonCheck() {
#ifndef TABLEBASE_BUILDER
  StringArray errors;
  String      selectedEngineName;
  const int   selected = getSelectedIndex();
  if(selected >= 0) {
    selectedEngineName = m_engineList[selected].getName();
  }
  bool changed = false;
  for(UINT i = (UINT)m_engineList.size(); i--;) {
    EngineDescription &desc = m_engineList[i];
    try {
      const EngineDescription newDesc = ExternEngine::getUCIReply(desc.getPath());
      if(newDesc != desc) {
        desc = newDesc;
        changed = true;
      }
    } catch(Exception e) {
      errors.add(format(_T("%s:%s: %s\n"), desc.getPath().cstr(), e.what(), loadString(IDS_DELETED).cstr()));
      m_engineList.removeIndex(i);
      changed = true;
    }
  }
  if(changed) {
    updateListCtrl();
    if(selectedEngineName.length() > 0) {
      int newIndex = m_engineList.getIndexByName(selectedEngineName);
      if(newIndex < 0) {
        newIndex = 0;
      }
      setSelectedIndex(m_listCtrl, newIndex);
    }
    if(errors.size() > 0) {
      String msg;
      for(size_t i = errors.size(); i--;) {
        msg += errors[i];
      }
      MessageBox(msg.cstr(), loadString(IDS_WARNING).cstr());
    }
  }
#else
  MessageBox(_T("Cannot check extern engines in BUILDER_MODE"), _T("Error"), MB_ICONEXCLAMATION);
#endif
}

void CEnginesDlg::OnOK() {
  Options::saveEngineRegister(m_engineList);
  __super::OnOK();
}

void CEnginesDlg::OnCancel() {
  if(isChanged()) {
    if(!confirmCancel(this)) {
      return;
    }
  }
  __super::OnCancel();
}
