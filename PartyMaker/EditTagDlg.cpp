#include "stdafx.h"
#include <AfxCView.h>
#include "PartyMaker.h"
#include "EditTagDlg.h"
#include "SelectAttributeDlg.h"
#include "mp3tag.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CEditTagDlg::CEditTagDlg(MediaFile &mediaFile, CWnd *pParent)
: CDialog(CEditTagDlg::IDD, pParent)
, m_mediaArray(mediaFile)
, m_origAttributes(    mediaFile.getAttributes(SELECT_READWRITE | SELECT_EMPTY))
, m_readOnlyAttributes(mediaFile.getAttributes(SELECT_READONLY  | SELECT_EMPTY))
{
  initData();
}

CEditTagDlg::CEditTagDlg(MediaArray &mediaArray, CWnd *pParent)
: CDialog(CEditTagDlg::IDD, pParent)
, m_mediaArray(mediaArray)
, m_origAttributes(    mediaArray.getAttributes(SELECT_READWRITE | SELECT_EMPTY))
, m_readOnlyAttributes(mediaArray.getAttributes(SELECT_READONLY  | SELECT_EMPTY))
{
  initData();
}

void CEditTagDlg::initData() {
    m_attributes = m_origAttributes;
    m_changed    = false;
}

void CEditTagDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_ATTRIBUTELIST        , m_readWriteListCtrl);
    DDX_Control(pDX, IDC_READONLYATTRIBUTELIST, m_readOnlyListCtrl );
}

BEGIN_MESSAGE_MAP(CEditTagDlg, CDialog)
    ON_WM_SIZE()
    ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_ATTRIBUTELIST         , OnBeginLabelEditAttributeList   )
    ON_NOTIFY(LVN_ENDLABELEDIT  , IDC_ATTRIBUTELIST         , OnEndLabelEditAttributeList     )
    ON_COMMAND(ID_EDIT_ADDATTRIBUTE                         , OnEditAddAttribute              )
    ON_COMMAND(ID_EDIT_EDIT                                 , OnEditEdit                      )
    ON_CBN_CLOSEUP(IDC_GENRECOMBO                           , OnCloseupGenreCombo             )
    ON_CBN_SELENDOK(IDC_GENRECOMBO                          , OnSelendOkGenreCombo            )
    ON_COMMAND(ID_GOTO_READWRITELIST                        , OnGotoReadWriteList             )
    ON_COMMAND(ID_GOTO_READONLYLIST                         , OnGotoReadOnlyList              )
    ON_NOTIFY(HDN_TRACK, IDC_ATTRIBUTELIST                  , OnTrackAttributelist            )
    ON_NOTIFY(LVN_COLUMNCLICK, IDC_ATTRIBUTELIST            , OnColumnclickAttributelist      )
END_MESSAGE_MAP()

void CEditTagDlg::initListControl(CListCtrl &ctrl, const AttributeArray &attributes, int extendedStyles) {
  CClientDC dc(&ctrl);
  int maxNameWidth = 30, maxValueWidth = 30;
  const int n = (int)attributes.size();
  for(int i = 0; i < n; i++) {
    const MediaAttribute &attr = attributes[i];
    int width     = dc.GetTextExtent(attr.getName() ).cx;
    maxNameWidth  = max(maxNameWidth, width);
    width         = dc.GetTextExtent(attr.getValue()).cx;
    maxValueWidth = max(maxValueWidth, width);
  }
  maxValueWidth = min(maxValueWidth,500);
  maxNameWidth  = min(maxNameWidth ,150);

  ctrl.InsertColumn(0,_T("Værdi")    , LVCFMT_LEFT, maxValueWidth);
  ctrl.InsertColumn(1,_T("Attribute"), LVCFMT_LEFT, maxNameWidth );
  ctrl.SetExtendedStyle(LVS_EX_TRACKSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP | extendedStyles);
  int columnOrder[2] =  { 1, 0 };
  ctrl.SetColumnOrderArray(2, columnOrder);
  for(int i = 0; i < n; i++) {
    const MediaAttribute &attr = attributes[i];
    addData(ctrl, i, 0, attr.getValue(),true);
    addData(ctrl, i, 1, attr.getName());
  }
}

BOOL CEditTagDlg::OnInitDialog() {
  __super::OnInitDialog();
  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_EDITTAG_ACCELERATOR));

  if(m_mediaArray.size() == 1) {
    SetWindowText(m_mediaArray[0].getSourceURL());
  } else {
    SetWindowText(format(_T("Redigerer %d sange"), m_mediaArray.size()).cstr());
  }

  initListControl(m_readWriteListCtrl, m_attributes        );
  initListControl(m_readOnlyListCtrl , m_readOnlyAttributes, LVS_EX_FULLROWSELECT);

  m_genreAttribute = m_attributes.findByName(_T("WM/Genre"));

  CComboBox *genreCombo = getGenreCombo();

#if defined(HAS_ID3TAGS)
  for(int i = 0; i < ID3_NR_OF_V1_GENRES; i++) {
    genreCombo->AddString(ID3_V1GENRE2DESCRIPTION(i));
  }
#endif

  if(m_genreAttribute >= 0) {
    CString currentGenre = m_attributes[m_genreAttribute].getValue();
    if(currentGenre.GetLength() > 0 && (getGenreIndex(currentGenre) < 0)) { // current genre is not present in the ID3-library's list og common genres
      genreCombo->AddString(currentGenre);
    }
  }

  m_layoutManager.OnInitDialog(this);

  m_layoutManager.addControl(IDC_ATTRIBUTELIST        , RELATIVE_SIZE | INIT_LISTHEADERS| RESIZE_LISTHEADERS);
  m_layoutManager.addControl(IDC_STATICREADONLYLABEL  , RELATIVE_Y_POS   );
  m_layoutManager.addControl(IDC_READONLYATTRIBUTELIST, RELATIVE_Y_POS  | RELATIVE_WIDTH | INIT_LISTHEADERS | RESIZE_LISTHEADERS);
  m_layoutManager.addControl(IDOK                     , RELATIVE_POSITION);
  m_layoutManager.addControl(IDCANCEL                 , RELATIVE_POSITION);
  OnGotoReadWriteList();

  return FALSE;
}

void CEditTagDlg::OnOK() {
  UpdateData();
  if(m_attributes != m_origAttributes) {
    theApp.BeginWaitCursor();

    try {
      for(size_t i = 0; i < m_mediaArray.size(); i++) {
        m_mediaArray[i].update(m_attributes);
        m_changed = true;
      }
    } catch(Exception e) {
      showException(e);
      return;
    }

    theApp.EndWaitCursor();
  }
  __super::OnOK();
}

void CEditTagDlg::OnSize(UINT nType, int cx, int cy) {
  m_layoutManager.OnSize(nType,cx,cy);
  __super::OnSize(nType, cx, cy);
}

int CEditTagDlg::getGenreIndex(const CString &genre) {
  CComboBox *genreCombo = getGenreCombo();
  const int count = genreCombo->GetCount();
  for(int i = 0; i < count; i++) {
    CString g;
    genreCombo->GetLBText(i,g);
    if(genre.CompareNoCase(g) == 0) {
      return i;
    }
  }
  return -1;
}

BOOL CEditTagDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  int fisk;
  int msg = pMsg->message;
  int unknownMessage = 0;
  switch(pMsg->message) {
  case 15:
  case 160:
  case 161:
  case 256:
  case 257:
  case 260:
  case 273:
  case 275:
  case 280:
  case 512:
  case 513:
  case 514:
  case 515:
  case 522:
  case 673:
  case 674:
  case 675:
  case 799:
  case 1024:
     break;
  default:
    unknownMessage = msg;
    fisk = 1;
    break;
  }
  return __super::PreTranslateMessage(pMsg);
}

CComboBox *CEditTagDlg::getGenreCombo() {
  return (CComboBox*)GetDlgItem(IDC_GENRECOMBO);
}

void CEditTagDlg::OnGotoReadWriteList() {
  m_readWriteListCtrl.SetFocus();
}

void CEditTagDlg::OnGotoReadOnlyList() {
  m_readOnlyListCtrl.SetFocus();
}

void CEditTagDlg::OnBeginLabelEditAttributeList(NMHDR *pNMHDR, LRESULT *pResult) {
  LV_DISPINFO *pDispInfo = (LV_DISPINFO*)pNMHDR;

  LVITEM &lv = pDispInfo->item;
  int attrIndex = lv.iItem;

  if(m_attributes[attrIndex].isReadOnly()) {
    *pResult = TRUE;
    return;
  } else {
    *pResult = FALSE;
  }

  if(attrIndex == m_genreAttribute) {
    CRect r;
    m_readWriteListCtrl.GetItemRect(attrIndex, &r, LVIR_LABEL);
    m_readWriteListCtrl.ClientToScreen(&r);
    CString genre = m_readWriteListCtrl.GetItemText(attrIndex,0);
    ScreenToClient(&r);
    CComboBox *genreCombo = getGenreCombo();
    WINDOWPLACEMENT wp;
    genreCombo->GetWindowPlacement(&wp);
    wp.rcNormalPosition = r;
    genreCombo->SetWindowPlacement(&wp);

    genreCombo->ShowWindow(SW_SHOW);
    genreCombo->ShowDropDown();
    const int genreIndex = getGenreIndex(genre);
    if(genreIndex >= 0) {
      genreCombo->SetCurSel(genreIndex);
    }
    *pResult = TRUE;
    genreCombo->SetFocus();
  }
}

void CEditTagDlg::OnEndLabelEditAttributeList(NMHDR *pNMHDR, LRESULT *pResult) {
  LV_DISPINFO *pDispInfo = (LV_DISPINFO*)pNMHDR;

  LVITEM &lv = pDispInfo->item;
  if(lv.pszText != NULL) {
    m_attributes[lv.iItem].setValue(lv.pszText);
    *pResult = TRUE;
  }
}

void CEditTagDlg::OnEditAddAttribute() {
  CSelectAttributeDlg dlg;
  dlg.DoModal();
}

void CEditTagDlg::OnEditEdit() {
  if(GetFocus() == GetDlgItem(IDC_ATTRIBUTELIST)) {
    const int selected = m_readWriteListCtrl.GetSelectionMark();
    if(selected >= 0) {
      m_readWriteListCtrl.EditLabel(selected);
    }
  }
}

void CEditTagDlg::OnCloseupGenreCombo() {
  getGenreCombo()->ShowWindow(SW_HIDE);
  m_readWriteListCtrl.SetFocus();
}

void CEditTagDlg::OnSelendOkGenreCombo() {
  CComboBox *genreCombo = getGenreCombo();
  int selected = genreCombo->GetCurSel();
  if(selected >= 0) {
    CString selectedGenreText;
    genreCombo->GetLBText(selected, selectedGenreText);
    m_readWriteListCtrl.SetItemText(m_genreAttribute,0,selectedGenreText);
  }
}


void CEditTagDlg::OnTrackAttributelist(NMHDR *pNMHDR, LRESULT *pResult) {
    HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
    *pResult = 0;
}

void CEditTagDlg::OnColumnclickAttributelist(NMHDR *pNMHDR, LRESULT *pResult) {
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    *pResult = 0;
}

BOOL CEditTagDlg::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pLResult) {
  int ctrlId = (int)wParam; // TODO
  if(ctrlId == IDC_ATTRIBUTELIST) {
    int heureka = 1;
  }
  switch(message) {
  case WM_NOTIFY:
    { NMHEADER *pnmhdr = (NMHEADER*)lParam;
      int code = pnmhdr->hdr.code;
      switch(code) {
      case HDN_TRACK:
        { int heureka = 1;
          break;
        }
      case HDN_ENDTRACK:
        { int heureka = 1;
          break;
        }
      case HDN_BEGINTRACK:
        { int heureka = 1;
          break;
        }
      }
    }
    break;
  case WM_NOTIFYFORMAT:
    { int heureka = 1;
      break;
    }
  }
  return __super::OnChildNotify(message, wParam, lParam, pLResult);
}

BOOL CEditTagDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *pResult) {
  const int hdnTrack      = HDN_TRACK;
  const int hdnEndtrack   = HDN_ENDTRACK;
  const int hdnBeginTrack = HDN_BEGINTRACK;

  int ctrlId = (int)wParam; // TODO
  if(ctrlId == IDC_ATTRIBUTELIST) {
    NMHEADER *pnmhdr = (NMHEADER*)lParam;
    int code = pnmhdr->hdr.code;
#if defined(_DEBUG)
    CClientDC dc(this);
    dc.TextOut(80,10,format(_T("code:%d     "), code).cstr());
#endif
    switch(code) {
    case HDN_TRACK:
      { int heureka = 1;
        break;
      }
    case HDN_ENDTRACK:
      { int heureka = 1;
        break;
      }
    case HDN_BEGINTRACK:
      { int heureka = 1;
        break;
      }
    }
  }

  return __super::OnNotify(wParam, lParam, pResult);
}
