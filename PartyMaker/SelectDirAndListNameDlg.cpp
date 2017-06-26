#include "stdafx.h"
#include <MFCUtil/SelectDirDlg.h>
#include "PartyMaker.h"
#include <Scandir.h>
#include <Direct.h>
#include <Tokenizer.h>
#include "SelectDirAndListNameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSelectDirAndListNameDlg::CSelectDirAndListNameDlg(Options &options, CWnd *pParent)
: m_options(options)
, CDialog(CSelectDirAndListNameDlg::IDD, pParent)
{
  m_timerIsRunning = false;

    m_listName = EMPTYSTRING;
    m_dir = EMPTYSTRING;
    m_drive = EMPTYSTRING;
    m_recurseSubDir = FALSE;
}


void CSelectDirAndListNameDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_NAMEEDIT, m_listName);
    DDX_Text(pDX, IDC_DIREDIT, m_dir);
    DDX_CBString(pDX, IDC_DRIVECOMBO, m_drive);
    DDX_Check(pDX, IDC_CHECKRECURSIVE, m_recurseSubDir);
}


BEGIN_MESSAGE_MAP(CSelectDirAndListNameDlg, CDialog)
    ON_CBN_EDITCHANGE(IDC_DRIVECOMBO, OnEditChangeDriveCombo)
    ON_COMMAND(ID_GOTO_LISTNAME     , OnGotoListName        )
    ON_COMMAND(ID_GOTO_DIR          , OnGotoDir             )
    ON_COMMAND(ID_GOTO_DRIVE        , OnGotoDrive           )
    ON_CBN_SELCHANGE(IDC_DRIVECOMBO , OnSelChangeDriveCombo )
    ON_NOTIFY(NM_DBLCLK, IDC_DIRTREE, OnDblclkDirTree       )
    ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CSelectDirAndListNameDlg::OnInitDialog() {
  __super::OnInitDialog();
  ULONG drives = _getdrives();

  m_accelTable = LoadAccelerators(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDR_SELECTDIRANDLISTNAME_ACCELERATOR));
  m_images.Create(IDR_FOLDERIMAGES,18,1,RGB(255,255,255));
  CTreeCtrl *ctrl = (CTreeCtrl*)GetDlgItem(IDC_DIRTREE);
  ctrl->SetImageList(&m_images,TVSIL_NORMAL);
  CComboBox *combo = (CComboBox *)GetDlgItem(IDC_DRIVECOMBO);
  for(int i = 0; i < 32; i++) {
    if(drives & (1<<i)) {
      combo->AddString(format(_T("%c:"),'A'+i).cstr());
    }
  }
  const String startDir = m_options.getStartSelectDir();
  FileNameSplitter dirInfo(startDir);
  m_drive = dirInfo.getDrive().cstr();
  fillTree(startDir.cstr());
  OnGotoListName();
  return true;
}

BOOL CSelectDirAndListNameDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

void CSelectDirAndListNameDlg::OnEditChangeDriveCombo() {
  CComboBox *combo = (CComboBox *)GetDlgItem(IDC_DRIVECOMBO);
  UpdateData();
  const TCHAR *d = m_drive;
  fillTree(format(_T("%s\\"),d).cstr());
}

void CSelectDirAndListNameDlg::OnSelChangeDriveCombo() {
  OnEditChangeDriveCombo();
}

static void expandAll(CTreeCtrl *ctrl, HTREEITEM p) {
  ctrl->Expand(p,TVE_EXPAND);
  for(HTREEITEM child = ctrl->GetChildItem(p); child != NULL; child = ctrl->GetNextSiblingItem(child)) {
    ctrl->Expand(child,TVE_EXPAND);
    expandAll(ctrl,child);
  }
}

void CSelectDirAndListNameDlg::fillTree(const TCHAR *path) {
  m_options.setStartSelectDir(path);
  DirList list = scandir(FileNameSplitter::getChildName(path,_T("*.*")),SELECTSUBDIR);
  CTreeCtrl *ctrl = (CTreeCtrl*)GetDlgItem(IDC_DIRTREE);
  ctrl->DeleteAllItems();

  HTREEITEM p = TVI_ROOT;
  TCHAR tmp[256];
  if(path[1] == _T(':') && path[2] == _T('\\')) { // start with "C:\"
    TCHAR first[10];
    _tcsnccpy(first,path,3);
    first[3] = _T('\0');
    p = ctrl->InsertItem( first, 1, 0, p);
    _tcscpy(tmp,path+3);
  } else {
    _tcscpy(tmp,path);
  }

  for(Tokenizer tok(tmp,_T("\\")); tok.hasNext();) {
    p = ctrl->InsertItem(tok.next().cstr(), 1, 0, p);
  }
  HTREEITEM selecteditem = p;
  for(size_t i = 0; i < list.size(); i++) {
    ctrl->InsertItem(list[i].name, 0, 0, p);
  }

  p = ctrl->GetRootItem();
  if(p != NULL) {
    expandAll(ctrl,p);
  }
  m_dir = path;
  UpdateData(false);
  ctrl->SelectItem(selecteditem);
  Invalidate();
}

void CSelectDirAndListNameDlg::OnGotoListName() {
  gotoEditBox(IDC_NAMEEDIT);
}

void CSelectDirAndListNameDlg::OnGotoDir() {
  gotoEditBox(IDC_DIREDIT);
}

void CSelectDirAndListNameDlg::OnGotoDrive() {
  GetDlgItem(IDC_DRIVECOMBO)->SetFocus();
}

void CSelectDirAndListNameDlg::gotoEditBox(int id) {
  CEdit *e = (CEdit*)GetDlgItem(id);
  e->SetFocus();
  e->SetSel(0,100);
}

CString CSelectDirAndListNameDlg::getSelectedPath() {
  CTreeCtrl *ctrl = (CTreeCtrl*)GetDlgItem(IDC_DIRTREE);
  String path;
  for(HTREEITEM item = ctrl->GetSelectedItem(); item; item = ctrl->GetParentItem(item)) {
    CString f = ctrl->GetItemText(item);
    path = FileNameSplitter::getChildName((LPCTSTR)f,path);
  }
  return path.cstr();
}

static int getChildCount(CTreeCtrl *ctrl, HTREEITEM item) {
  int count = 0;
  for(HTREEITEM child = ctrl->GetChildItem(item); child != NULL; child = ctrl->GetNextSiblingItem(child)) {
    count++;
  }
  return count;
}


void CSelectDirAndListNameDlg::OnOK() {
  switch(getFocusCtrlId(this)) {
  case IDC_DIRTREE:
    { CString path = getSelectedPath();
      DirList list = scandir(FileNameSplitter::getChildName(path.GetBuffer(path.GetLength()),_T("*.*")),SELECTSUBDIR);
      CTreeCtrl *ctrl = (CTreeCtrl*)GetDlgItem(IDC_DIRTREE);
      if(getChildCount(ctrl,ctrl->GetSelectedItem()) > 0 || list.size() == 0) {
        m_dir = path;
        UpdateData(false);
        if(validate()) {
          __super::OnOK();
        }
      } else {
        fillTree(path.GetBuffer(path.GetLength()));
      }
    }
    break;
  case IDC_DRIVECOMBO:
    break;
  default:
    if(validate()) {
      __super::OnOK();
    }
    break;
  }
}

bool CSelectDirAndListNameDlg::validate() {
  UpdateData();
  if(m_listName.GetLength() == 0) {
    OnGotoListName();
    Message(_T("Navn skal udfyldes"));
    return false;
  }
  if(m_dir.GetLength() == 0) {
    OnGotoDir();
    Message(_T("Ingen mappe falgt"));
    return false;
  }

  m_mediaArray.rescan((LPCTSTR)m_dir, m_recurseSubDir?true:false);
  while(m_mediaArray.isBusy()) {
    Sleep(300);
  }
  if(m_mediaArray.isCancelled()) {
    return false;
  }
  if(m_mediaArray.size() == 0) {
    MessageBox(_T("Ingen media filer fundet i den angivne mappe"));
    return false;
  }
  try {
    m_playList = MediaDatabase::createPlayList((LPCTSTR)m_listName);
    if(m_playList == NULL) {
      Message(_T("Kan ikke oprette playlisten med navn %s"), (LPCTSTR)m_listName);
      return false;
    }
  } catch(Exception e) {
    Message(_T("Kan ikke oprette playlisten med navn %s\r\n%s"), (LPCTSTR)m_listName, e.what());
    return false;
  }
  return true;
}

void CSelectDirAndListNameDlg::startTimer() {
  if(!m_timerIsRunning && SetTimer(1, 300, NULL)) {
    m_timerIsRunning = true;
  }
}

void CSelectDirAndListNameDlg::stopTimer() {
  if(m_timerIsRunning) {
    KillTimer(1);
    m_timerIsRunning = false;
  }
}

void CSelectDirAndListNameDlg::OnDblclkDirTree(NMHDR *pNMHDR, LRESULT *pResult) {
  CString path = getSelectedPath();
  fillTree(path);
  *pResult = 1;
}

void CSelectDirAndListNameDlg::OnTimer(UINT_PTR nIDEvent) {
  __super::OnTimer(nIDEvent);
}
