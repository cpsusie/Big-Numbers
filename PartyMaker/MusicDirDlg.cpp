#include "stdafx.h"
#include <MFCUtil/SelectDirDlg.h>
#include "partymaker.h"
#include "MusicDirDlg.h"
#include <Tokenizer.h>

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CMusicDirDlg::CMusicDirDlg(Options &options, CWnd *pParent)
: m_options(options)
, CDialog(CMusicDirDlg::IDD, pParent) {
}

void CMusicDirDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMusicDirDlg, CDialog)
    ON_LBN_DBLCLK(IDC_DIRLIST, OnDblclkDirlist)
    ON_BN_CLICKED(IDC_NEWBUTTON, OnNewButton)
    ON_BN_CLICKED(IDC_DELETEBUTTON, OnDeleteButton)
END_MESSAGE_MAP()

BOOL CMusicDirDlg::OnInitDialog() {
  __super::OnInitDialog();

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_MUSICDIR_ACCELERATOR));
  CListBox *lb = getListBox();

  MusicDirList tmp(m_options.getDirList());
  for(size_t i = 0; i < tmp.size(); i++) {
    lb->AddString(tmp[i].getDir().cstr());
    lb->SetSel((int)i,tmp[i].isActive());
  }
  lb->SetFocus();
  return FALSE;
}

void CMusicDirDlg::OnOK() {
  CListBox *lb = getListBox();
  MusicDirList tmp;
  for(int i = 0; i < lb->GetCount(); i++) {
    CString dir;
    bool active;
    lb->GetText(i,dir);
    active = lb->GetSel(i) ? true : false;
    tmp.add(MusicDir(active,(LPCTSTR)dir));
  }
  m_options.setDirList(tmp.toString());
  __super::OnOK();
}

void CMusicDirDlg::OnDblclkDirlist() {
  CListBox *lb = getListBox();
  int selected = lb->GetCurSel();
  CRect rect;
  lb->GetItemRect(selected,&rect);

  const MSG *msg = GetCurrentMessage();
}

void CMusicDirDlg::OnNewButton() {
  CSelectDirDlg dlg(m_options.getStartSelectDir());
  if(dlg.DoModal() == IDOK) {
    CListBox *lb = getListBox();
    lb->AddString(dlg.getSelectedDir().cstr());
    lb->SetSel(lb->GetCount()-1);
  }
}

void CMusicDirDlg::OnDeleteButton() {
  CListBox *lb = getListBox();
  int selected = lb->GetCurSel();
  if(selected >= 0 && selected < lb->GetCount()) {
    lb->DeleteString(selected);
    if(selected < lb->GetCount()) {
      lb->SetCurSel(selected);
    } else {
      lb->SetCurSel(lb->GetCount()-1);
    }
  }
}

BOOL CMusicDirDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

CListBox *CMusicDirDlg::getListBox() {
  return (CListBox*)GetDlgItem(IDC_DIRLIST);
}
