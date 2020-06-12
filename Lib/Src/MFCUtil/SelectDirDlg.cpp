#include "pch.h"
#include <MyUtil.h>
#include <FileNameSplitter.h>
#include <Scandir.h>
#include <Direct.h>
#include <Tokenizer.h>
#include <MFCUtil/TreeCtrlWalker.h>
#include <MFCUtil/SelectDirDlg.h>

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CSelectDirDlg::CSelectDirDlg(const String &startDir, CWnd *pParent) : CDialog(IDD, pParent) {
    m_dir = startDir.cstr();
    m_drive = EMPTYSTRING;
}

void CSelectDirDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
    DDX_Text(pDX    , _IDC_EDIT_DIR, m_dir);
    DDX_CBString(pDX, _IDC_COMBO_DRIVE, m_drive);
}

BEGIN_MESSAGE_MAP(CSelectDirDlg, CDialog)
    ON_CBN_EDITCHANGE(_IDC_COMBO_DRIVE, OnEditChangeDriveCombo)
    ON_COMMAND(_ID_GOTO_DIR           , OnGotoDir             )
    ON_COMMAND(_ID_GOTO_DRIVE         , OnGotoDrive           )
    ON_CBN_SELCHANGE(_IDC_COMBO_DRIVE , OnSelChangeDriveCombo )
    ON_NOTIFY(NM_DBLCLK, _IDC_TREE_DIR, OnDblclkDirTree       )
END_MESSAGE_MAP()

BOOL CSelectDirDlg::OnInitDialog() {
  __super::OnInitDialog();
  unsigned long drives = _getdrives();

  setControlText(IDD, this);
  m_accelTable = LoadAccelerators(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(_IDR_SELECTDIR_ACCELERATOR));
  m_images.Create(_IDR_FOLDERIMAGES, 18, 1, RGB(255,255,255));
  CTreeCtrl *ctrl = getTreeCtrl();
  ctrl->SetImageList(&m_images, TVSIL_NORMAL);
  CComboBox *combo = (CComboBox*)GetDlgItem(_IDC_COMBO_DRIVE);
  for(int i = 0; drives; i++) {
    if(drives & (1<<i)) {
      TCHAR drive[10];
      _stprintf(drive, _T("%c:"), _T('A')+i);
      combo->AddString(drive);
      drives &= ~(1<<i);
    }
  }
  if(m_dir.GetLength() == 0) {
    m_dir = _T("C:\\");
  }
  FileNameSplitter dirInfo((LPCTSTR)m_dir);
  m_drive = dirInfo.getDrive().cstr();
  fillTree(m_dir);
  return true;
}

BOOL CSelectDirDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

void CSelectDirDlg::OnEditChangeDriveCombo() {
  CComboBox *combo = (CComboBox*)GetDlgItem(_IDC_COMBO_DRIVE);
  UpdateData();
  const TCHAR *d = m_drive;
  TCHAR path[10];
  _stprintf(path, _T("%s\\"), d);
  fillTree(path);
}

void CSelectDirDlg::OnSelChangeDriveCombo() {
  OnEditChangeDriveCombo();
}

void CSelectDirDlg::fillTree(const TCHAR *path) {
  DirList    list = scandir(FileNameSplitter::getChildName(path, _T("*.*")), SELECTSUBDIR);
  CTreeCtrl *ctrl = getTreeCtrl();
  ctrl->DeleteAllItems();

  HTREEITEM p = TVI_ROOT;
  TCHAR tmp[256];
  if(path[1] == ':' && path[2] == '\\') { /* start with C:\ */
    TCHAR first[10];
    _tcsncpy(first, path, 3);
    first[3] = '\0';
    p = ctrl->InsertItem(first, 1, 0, p);
    _tcscpy(tmp, path+3);
  } else {
    _tcscpy(tmp, path);
  }

  for(Tokenizer tok(tmp, _T("\\")); tok.hasNext();) {
    p = ctrl->InsertItem(tok.next().cstr(), 1, 0, p);
  }
  HTREEITEM selecteditem = p;
  for(size_t i = 0; i < list.size(); i++) {
    ctrl->InsertItem(list[i].name, 0, 0, p);
  }

  TreeItemExpander(true).visitAllItems(ctrl);

  m_dir = path;
  UpdateData(false);
  ctrl->SelectItem(selecteditem);
  Invalidate();
}

void CSelectDirDlg::OnGotoDir() {
  gotoEditBox(this, _IDC_EDIT_DIR);
}

void CSelectDirDlg::OnGotoDrive() {
  GetDlgItem(_IDC_COMBO_DRIVE)->SetFocus();
}

CString CSelectDirDlg::getSelectedPath() const {
  CTreeCtrl *ctrl = getTreeCtrl();
  String path;
  for(HTREEITEM item = ctrl->GetSelectedItem(); item; item = ctrl->GetParentItem(item)) {
    CString f = ctrl->GetItemText(item);
    path = FileNameSplitter::getChildName((LPCTSTR)f, path);
  }
  return path.cstr();
}

CTreeCtrl *CSelectDirDlg::getTreeCtrl() const {
  return (CTreeCtrl*)GetDlgItem(_IDC_TREE_DIR);
}

static int getChildCount(CTreeCtrl *ctrl, HTREEITEM item) {
  int count = 0;
  for(HTREEITEM child = ctrl->GetChildItem(item); child; child = ctrl->GetNextSiblingItem(child)) {
    count++;
  }
  return count;
}

void CSelectDirDlg::OnOK() {
  switch(getFocusCtrlId(this)) {
  case _IDC_TREE_DIR:
    { CString path = getSelectedPath();
      DirList list = scandir(FileNameSplitter::getChildName((LPCTSTR)path, _T("*.*")), SELECTSUBDIR);
      CTreeCtrl *ctrl = getTreeCtrl();
      if(getChildCount(ctrl, ctrl->GetSelectedItem()) > 0 || list.size() == 0) {
        m_dir = path;
        UpdateData(false);
        __super::OnOK();
      } else {
        fillTree((LPCTSTR)path);
      }
    }
    break;
  case _IDC_COMBO_DRIVE:
    break;
  default:
    UpdateData();
    __super::OnOK();
    break;
  }
}

void CSelectDirDlg::OnDblclkDirTree(NMHDR *pNMHDR, LRESULT *pResult) {
  CString path = getSelectedPath();
  fillTree(path);
  *pResult = 1;
}
