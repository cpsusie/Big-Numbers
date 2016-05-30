#include "stdafx.h"
#include <MyUtil.h>
#include <Scandir.h>
#include <Direct.h>
#include <Tokenizer.h>
#include <MFCUtil/WinTools.h>
#include <MFCUtil/SelectDirDlg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSelectDirDlg::CSelectDirDlg(const String &startDir, CWnd* pParent) : CDialog(CSelectDirDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSelectDirDlg)
	m_dir = startDir.cstr();
	m_drive = _T("");
	//}}AFX_DATA_INIT
}

void CSelectDirDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectDirDlg)
	DDX_Text(pDX    , _IDC_EDIT_DIR, m_dir);
	DDX_CBString(pDX, _IDC_COMBO_DRIVE, m_drive);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSelectDirDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectDirDlg)
	ON_CBN_EDITCHANGE(_IDC_COMBO_DRIVE, OnEditChangeDriveCombo)
	ON_COMMAND(_ID_GOTO_DIR           , OnGotoDir             )
	ON_COMMAND(_ID_GOTO_DRIVE         , OnGotoDrive           )
	ON_CBN_SELCHANGE(_IDC_COMBO_DRIVE , OnSelChangeDriveCombo )
	ON_NOTIFY(NM_DBLCLK, _IDC_TREE_DIR, OnDblclkDirTree       )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CSelectDirDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  unsigned long drives = _getdrives();

  setControlText(IDD, this);
  m_accelTable = LoadAccelerators(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(_IDR_SELECTDIR_ACCELERATOR));
  m_images.Create(_IDR_FOLDERIMAGES, 18, 1, RGB(255,255,255));
  CTreeCtrl *ctrl = (CTreeCtrl*)GetDlgItem(_IDC_TREE_DIR);
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

BOOL CSelectDirDlg::PreTranslateMessage(MSG* pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }
  return CDialog::PreTranslateMessage(pMsg);
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

static void expandAll(CTreeCtrl *ctrl, HTREEITEM p) {
  ctrl->Expand(p, TVE_EXPAND);
  for(HTREEITEM child = ctrl->GetChildItem(p); child; child = ctrl->GetNextSiblingItem(child)) {
    ctrl->Expand(child, TVE_EXPAND);
    expandAll(ctrl, child);
  }
}

void CSelectDirDlg::fillTree(const TCHAR *path) {
  DirList list = scandir(FileNameSplitter::getChildName(path, _T("*.*")), SELECTSUBDIR);
  CTreeCtrl *ctrl = (CTreeCtrl*)GetDlgItem(_IDC_TREE_DIR);
  ctrl->DeleteAllItems();

  HTREEITEM p = TVI_ROOT;
  TCHAR tmp[256];
  if(path[1] == ':' && path[2] == '\\') { // start with C:\ 
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
  for(int i = 0; i < list.size(); i++) {
    ctrl->InsertItem(list[i].name, 0, 0, p);
  }

  p = ctrl->GetRootItem();
  if(p != NULL) {
    expandAll(ctrl, p);
  }
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
  CTreeCtrl *ctrl = (CTreeCtrl*)GetDlgItem(_IDC_TREE_DIR);
  String path;
  for(HTREEITEM item = ctrl->GetSelectedItem(); item; item = ctrl->GetParentItem(item)) {
    CString f = ctrl->GetItemText(item);
    path = FileNameSplitter::getChildName((LPCTSTR)f, path);
  }
  return path.cstr();
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
      CTreeCtrl *ctrl = (CTreeCtrl*)GetDlgItem(_IDC_TREE_DIR);
      if(getChildCount(ctrl, ctrl->GetSelectedItem()) > 0 || list.size() == 0) {
        m_dir = path;
        UpdateData(false);
        CDialog::OnOK();
      } else {
        fillTree((LPCTSTR)path);
      }
    }
    break;
  case _IDC_COMBO_DRIVE:
    break;
  default:
    UpdateData();
    CDialog::OnOK();
    break;
  }
}

void CSelectDirDlg::OnDblclkDirTree(NMHDR *pNMHDR, LRESULT *pResult) {
  CString path = getSelectedPath();
  fillTree(path);
  *pResult = 1;
}
