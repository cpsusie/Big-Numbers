#include "stdafx.h"
#include "PartyMaker.h"
#include "ChangeOrderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CChangeOrderDlg::CChangeOrderDlg(MediaQueue &mediaQueue, CWnd *pParent) : CDialog(IDD, pParent), m_mediaQueue(mediaQueue) {
}

void CChangeOrderDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CChangeOrderDlg, CDialog)
  ON_COMMAND(ID_EDIT_CUT, OnEditCut)
  ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
  ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
  ON_LBN_SELCHANGE(IDC_EDITLIST, OnSelchangeEditlist)
  ON_WM_KEYDOWN()
  ON_COMMAND(ID_EDIT_RANDOM, OnEditRandom)
END_MESSAGE_MAP()

BOOL CChangeOrderDlg::OnInitDialog() {
  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_CHANGEORDER_ACCELERATOR));

  for(size_t i = 0; i < m_mediaQueue.size(); i++) {
    insertMediaFile(i,m_mediaQueue[i]);
  }
  CListBox *lb = getListBox();
  lb->InsertString((int)m_mediaQueue.size(),EMPTYSTRING);
  __super::OnInitDialog();

  gotoToListBox();
  if(!m_mediaQueue.isEmpty()) {
    lb->SetSel(0);
  }
  return false;
}

void CChangeOrderDlg::gotoToListBox() {
  getListBox()->SetFocus();
}

CListBox *CChangeOrderDlg::getListBox() {
  return (CListBox*)GetDlgItem(IDC_EDITLIST);
}

void CChangeOrderDlg::OnOK() {
  m_mediaQueue.clear();
  for(size_t i = 0; i < m_editList.size(); i++) {
    m_mediaQueue.put(m_editList[i]);
  }
  __super::OnOK();
}

void CChangeOrderDlg::getSelected(CompactIntArray &selected) {
  CListBox *lb = getListBox();
  int count =  lb->GetSelCount();
  int *items = new int[count];
  lb->GetSelItems(count,items);
  selected.clear();
  for(int i = 0; i < count; i++) {
    if(items[i] == lb->GetCount() - 1) {
      continue; // never select the last empty String
    }
    selected.add(items[i]);
  }
  delete[] items;
}

void CChangeOrderDlg::OnEditCut() {
  CListBox *lb = getListBox();
  int pos = lb->GetCurSel();
  CompactIntArray selected;
  getSelected(selected);
  m_buffer.clear();
  for(size_t i = 0; i < selected.size(); i++) {
    m_buffer.add(m_editList[selected[i]]);
  }
  removeSelected(selected);
  if(pos < lb->GetCount()) {
    lb->SetSel(pos);
  } else {
    lb->SetSel(lb->GetCount()-1);
  }
  Invalidate();
}

void CChangeOrderDlg::OnEditPaste() {
  CListBox *lb = getListBox();
  int pos = lb->GetCurSel();
  for(size_t i = 0; i < m_buffer.size(); i++) {
    insertMediaFile(pos,m_buffer[i]);
  }
  for(int i = 0; i < lb->GetCount(); i++) {
    lb->SetSel(i,false);
  }
  if(m_buffer.size() > 0) {
    lb->SetSel(pos);
  }
  Invalidate();
}

void CChangeOrderDlg::OnEditDelete() {
  OnEditCut();
}

void CChangeOrderDlg::removeSelected(const CompactIntArray &selected) {
  CListBox *lb = getListBox();
  for(intptr_t i = selected.size() - 1; i >= 0; i--) {
    m_editList.removeIndex(selected[i]);
    lb->DeleteString(selected[i]);
  }
}

void CChangeOrderDlg::insertMediaFileIntoListBox(size_t pos, const MediaFile &f) {
  CListBox *lb = getListBox();
  String tmp = format(_T("%-38.38s%-36.36s%-36.36s")
                     ,f.getTitle()
                     ,f.getArtist()
                     ,f.getAlbum()
                     );
  int ret = lb->InsertString((int)pos, tmp.cstr());
}

void CChangeOrderDlg::insertMediaFile(size_t pos, const MediaFile &f) {
  m_editList.add(pos,f);
  insertMediaFileIntoListBox(pos,f);
}

BOOL CChangeOrderDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

void CChangeOrderDlg::OnSelchangeEditlist() {
  CListBox *lb = getListBox();
  int i = lb->GetCurSel();
  if(i == lb->GetCount()-1) { // never select the last empty String
    lb->SetSel(lb->GetCount() - 1, false);
  }
}

void CChangeOrderDlg::OnEditRandom() {
  m_editList.shuffle();
  CListBox *lb = getListBox();
  lb->ResetContent();
  for(size_t i = 0; i < m_editList.size(); i++) {
    insertMediaFileIntoListBox(i,m_editList[i]);
  }
  lb->InsertString((int)m_mediaQueue.size(),EMPTYSTRING);
}
