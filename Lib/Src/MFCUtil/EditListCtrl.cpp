#include "pch.h"
#include <MyUtil.h>
#include <DebugLog.h>
#include <MFCUtil/EditListNumericEditor.h>
#include <MFCUtil/EditListStringEditor.h>
#include <MFCUtil/EditListStringCombo.h>
#include <MFCUtil/EditListBooleanEditor.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CEditListCtrl::CEditListCtrl() {
  m_currentEditor   = NULL;
  m_currentCell     = CPoint(-1,-1);
  m_sortColumn      = -1;
  m_ascending       = true;
  createBitmaps();
}

CEditListCtrl::~CEditListCtrl() {
}

#define FIRST_CTRL_ID    EDITLIST_CONTROL_ID_COLUMN0
#define LAST_CTRL_ID     MAKE_EDITLIST_CONTROL_ID(999)

BEGIN_MESSAGE_MAP(CEditListCtrl, CListCtrl)
  ON_WM_PAINT()
  ON_WM_LBUTTONDOWN()
  ON_WM_SETFOCUS()
  ON_WM_KILLFOCUS()
  ON_WM_SIZE()
  ON_NOTIFY_REFLECT( LVN_ITEMCHANGED, CEditListCtrl::OnItemchanged)
  ON_NOTIFY_REFLECT( LVN_COLUMNCLICK, CEditListCtrl::OnColumnclick)
  ON_CONTROL_RANGE(  EN_SETFOCUS , FIRST_CTRL_ID, LAST_CTRL_ID, OnEditSetFocus )
  ON_CONTROL_RANGE(  EN_KILLFOCUS, FIRST_CTRL_ID, LAST_CTRL_ID, OnEditKillFocus)
  ON_CONTROL_RANGE(  BN_SETFOCUS , FIRST_CTRL_ID, LAST_CTRL_ID, OnEditSetFocus )
  ON_CONTROL_RANGE(  BN_KILLFOCUS, FIRST_CTRL_ID, LAST_CTRL_ID, OnEditKillFocus)
  ON_WM_MEASUREITEM_REFLECT()
END_MESSAGE_MAP()

#ifdef __SAVE__MESSAGE_MAP
    ON_CONTROL_RANGE(  EN_SETFOCUS , FIRST_CTRL_ID, LAST_CTRL_ID, OnEditSetFocus )
    ON_CONTROL_RANGE(  EN_KILLFOCUS, FIRST_CTRL_ID, LAST_CTRL_ID, OnEditKillFocus)
    ON_CONTROL_RANGE(  BN_SETFOCUS , FIRST_CTRL_ID, LAST_CTRL_ID, OnEditSetFocus )
    ON_CONTROL_RANGE(  BN_KILLFOCUS, FIRST_CTRL_ID, LAST_CTRL_ID, OnEditKillFocus)
#endif

#define HANDLE_KILLSET_FOCUS 0
#define HANDLE_ITEM_CHANGED  1
#define INIT_DONE            2
#define GOTERROR             3
#define COMMITACTIVE         4
#define MSGWASMOUSECLICK     5

#define SETCONTROLFLAG(  flag) m_controlFlags.add(     flag)
#define CLRCONTROLFLAG(  flag) m_controlFlags.remove(  flag)
#define ISCONTROLFLAGSET(flag) m_controlFlags.contains(flag)

#define V(f) { if(!f) showError(_T("%s failed. %s"), _T(#f), getLastErrorText().cstr()); }

static LRESULT CALLBACK privateHeaderCtrlWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  CEditListCtrl *listCtrl = (CEditListCtrl*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

  const LRESULT result = CallWindowProc(listCtrl->m_headerCtrlWinProc, hwnd, msg, wParam, lParam);

  if(msg == WM_PAINT) {
    listCtrl->paintHeaderSortMark();
  }
  return result;
}

void CEditListCtrl::substituteControl(CWnd *wnd, int id, CTableModel &model) {
  DEFINEMETHODNAME;
  m_model = &model;
  CWnd *ctrl = wnd->GetDlgItem(id);
  if(ctrl == NULL) {
    showError(_T("%s:Control %d not found"), method, id);
    return;
  }

  const CompactIntArray origTabOrder = getTabOrder(wnd);

  DWORD       style   = ctrl->GetStyle();
  DWORD       exStyle = ctrl->GetExStyle();
  const CRect rect    = getWindowRect(ctrl);
  ctrl->DestroyWindow();

  style &= ~(LVS_TYPEMASK | LVS_SORTASCENDING | LVS_SORTDESCENDING | LVS_NOSCROLL | LVS_EDITLABELS | LVS_NOSORTHEADER);
  style |= LVS_REPORT | LVS_OWNERDRAWFIXED;

  m_itemHeight = getItemHeight();

  if(!Create(style, rect, wnd, id)) {
    showError(_T("%s::Create failed"), method);
    return;
  }

  setTabOrder(wnd, origTabOrder);

  ModifyStyleEx(0, exStyle);
  SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP);

  // set up our own private window procedure for the header ctrl.
  CHeaderCtrl *headerCtrl = GetHeaderCtrl();
  m_headerCtrlWinProc = (WNDPROC)GetWindowLongPtr(*headerCtrl, GWLP_WNDPROC  );
  SetWindowLongPtr(*headerCtrl, GWLP_USERDATA, (LONG_PTR)this                    );
  SetWindowLongPtr(*headerCtrl, GWLP_WNDPROC , (LONG_PTR)privateHeaderCtrlWinProc);

  const int columnCount = model.getColumnCount();
  for(int c = 0; c < columnCount; c++) {
    InsertColumn(c, model.getColumnName(c).cstr(), LVCFMT_LEFT, model.getColumnWidth(c));
  }
  const int rowCount = model.getRowCount();
  for(int r = 0; r < rowCount; r++) {
    insertRow(r);
  }
  SETCONTROLFLAG(HANDLE_KILLSET_FOCUS);
  SETCONTROLFLAG(HANDLE_ITEM_CHANGED );
  SETCONTROLFLAG(INIT_DONE           );
}

void CEditListCtrl::setSortColumn(int column, bool asc) {
  m_sortColumn = column;
  m_ascending  = asc;
  if(ISCONTROLFLAGSET(INIT_DONE)) {
    GetHeaderCtrl()->Invalidate();
    PostMessage(WM_PAINT);
  }
}

void CEditListCtrl::insertNewItem(int index /* = -1*/ ) { // if index == -1 then append item at the end
  endEdit();
  if((index < 0) || index > getRowCount()) {
    index = getRowCount();
  }
  insertRow(index);
}

void CEditListCtrl::removeItem(UINT index) {
  if(index >= (UINT)getRowCount()) {
    return;
  }
  endEdit();
  removeRow(index);
  beginEdit();
}

bool CEditListCtrl::lastMessageWasMouseClick() const {
  return ISCONTROLFLAGSET(MSGWASMOUSECLICK);
}

void CEditListCtrl::setItem(LVITEM &lv, bool newItem) {
  if(newItem) {
    if(InsertItem(&lv) < 0) {
      showWarning(_T("InsertItem(%d,%d) failed"), lv.iItem, lv.iSubItem);
    }
  } else {
    if(!SetItem(&lv)) {
      showWarning(_T("SetItem(%d,%d) failed"), lv.iItem, lv.iSubItem);
    }
  }
}

void CEditListCtrl::insertRow(UINT row) {
  const int columnCount = getColumnCount();
  CPoint cell(0, row);
  for(cell.x = 0; cell.x < columnCount; cell.x++) {
    modelValueToList(cell, cell.x == 0);
  }
}

void CEditListCtrl::removeRow(UINT row) {
  DeleteItem(row);
  if(m_currentCell.y >= getRowCount()) {
    m_currentCell.y = getRowCount() - 1;
  }
}

void CEditListCtrl::setCurrentCell(int r, int c) {
  const ListCell current = getCurrentCell();

//  trace("setCurrentCell(%d,%d)", r,c);

  if((r != current.y) || (c != current.x)) {
    CLRCONTROLFLAG(HANDLE_ITEM_CHANGED);

    if(hasCurrentEditor()) {
      if(!endEdit()) {
        SETCONTROLFLAG(HANDLE_ITEM_CHANGED);
        return;
      }
    }

    SetSelectionMark(r);
    SetItemState(r, 0, LVIS_SELECTED|LVIS_FOCUSED);

    if(r >= 0) {
      m_currentCell = ListCell(c, r);
      beginEdit();
    } else {
      m_currentCell = ListCell(-1,-1);
    }

    SETCONTROLFLAG(HANDLE_ITEM_CHANGED);
  }
}

void CEditListCtrl::beginEdit() {
  DEFINEMETHODNAME;
  if(!hasCurrentCell()) {
    return;
  }
  const ListCell cell = getCurrentCell();

  if(hasCurrentEditor()) {
    showError(_T("%s(%d,%d) called with existing editor!!\n"), method, cell.y, cell.x);
    return;
  }
  EnsureVisible(cell.y, FALSE);
  CWnd *editor = getEditor(cell);
  const ListFieldType type = getListFieldType(cell);
  switch(type) {
  case LFT_SHORT   :
  case LFT_INT     :
  case LFT_FLOAT   :
  case LFT_DOUBLE  :
    { CEditListNumericEditor  *ctrl = (CEditListNumericEditor*)editor;
      setWindowRect(ctrl, getCurrentEditorRect());
      ctrl->setEditable(isCurrentCellEditable());
      modelValueToEditor(cell);
    }
    break;
  case LFT_STRING  :
    { CEditListStringEditor   *ctrl = (CEditListStringEditor*)editor;
      setWindowRect(ctrl, getCurrentEditorRect());
      ctrl->setEditable(isCurrentCellEditable());
      modelValueToEditor(cell);
    }
    break;
  case LFT_STRCOMBO:
    { CEditListStringCombo    *ctrl = (CEditListStringCombo*)editor;
      setWindowRect(ctrl, getCurrentEditorRect());
      ctrl->setEditable(isCurrentCellEditable());
      modelValueToEditor(cell);
    }
    break;
  case LFT_BOOL    :
    { CEditListBooleanEditor  *ctrl = (CEditListBooleanEditor*)editor;
      setWindowRect(ctrl, getCurrentEditorRect());
      ctrl->setEditable(isCurrentCellEditable());
      modelValueToEditor(cell);
    }
    break;
  }

// trace("beginEdit %s-Mode", isCurrentCellEditable() ? "Edit":"Read-only");

  editor->ShowWindow(SW_SHOW);

  CLRCONTROLFLAG(HANDLE_KILLSET_FOCUS);
  editor->SetFocus();
  m_currentEditor = editor;
  m_lastCellRect  = getCurrentCellRect();
}

bool CEditListCtrl::endEdit() {
  if(hasCurrentEditor()) {
//    trace("endEdit");
    if(isCurrentCellEditable()) {
      return commitEdit();
    } else {
      cancelEdit();
    }
  }
  return true;
}

bool CEditListCtrl::commitEdit() {
  if(ISCONTROLFLAGSET(COMMITACTIVE)) {
    return true;
  }

//  trace("enter commitEdit");

  SETCONTROLFLAG(COMMITACTIVE);
  bool ret;
  const ListCell &cell = getCurrentCell();
  if(hasCurrentEditor()) {
    try {
      editorValueToModel(cell);
      modelValueToList(cell, false);
      hideEditCtrl();
      ret = true;
    } catch(Exception e) {
      SETCONTROLFLAG(GOTERROR);
//      trace("commitEdit:enter Messagebox(%s)", e.what());
      showException(e);
      ret = false;
    }
  }
  CLRCONTROLFLAG(COMMITACTIVE);

//  trace("leave commitEdit returning %s", boolToStr(ret));

  return ret;
}

void CEditListCtrl::cancelEdit() {
//  trace("cancelEdit");

  hideEditCtrl();
}

void CEditListCtrl::hideEditCtrl() {
  if(hasCurrentEditor()) {

//    trace("hideEditCtrl");

    CWnd *editCtrl = m_currentEditor;
    m_currentEditor = NULL; // to prevent recursive call from OnEditKillFocus
    editCtrl->ShowWindow(SW_HIDE); // and now we can hide it
    SETCONTROLFLAG(HANDLE_KILLSET_FOCUS);
  }
}

void CEditListCtrl::modelValueToEditor(const ListCell &cell) {
  const ListFieldType type = getListFieldType(cell);
  switch(type) {
  case LFT_SHORT :
  case LFT_INT   :
  case LFT_FLOAT :
  case LFT_DOUBLE:
    { const double v = getNumericValueAt(cell);
      CEditListNumericEditor *editCtrl  = (CEditListNumericEditor*)getEditor(cell);
      editCtrl->setValue(v);
    }
    break;
  case LFT_STRING:
    { const String v = *(String*)m_model->getValueAt(cell.y, cell.x);
      CEditListStringEditor *editCtrl  = (CEditListStringEditor*)getEditor(cell);
      editCtrl->setValue(v);
    }
    break;
  case LFT_STRCOMBO:
    modelValueToComboEditor(cell);
    break;
  case LFT_BOOL  :
    { const bool v = *(bool*)m_model->getValueAt(cell.y, cell.x);
      CEditListBooleanEditor *button = (CEditListBooleanEditor*)getEditor(cell);
      button->setValue(v);
    }
    break;
  }
}

void CEditListCtrl::editorValueToModel(const ListCell &cell) {
  const ListFieldType type = getListFieldType(cell);
  switch(type) {
  case LFT_SHORT :
    { CEditListNumericEditor *editCtrl = (CEditListNumericEditor*)getEditor(cell);
      *(short*)m_model->getValueAt(cell.y, cell.x) = editCtrl->getShortValue();
    }
    break;
  case LFT_INT   :
    { CEditListNumericEditor *editCtrl = (CEditListNumericEditor*)getEditor(cell);
      *(int*)m_model->getValueAt(cell.y, cell.x) = editCtrl->getIntValue();
    }
    break;
  case LFT_FLOAT :
    { CEditListNumericEditor *editCtrl = (CEditListNumericEditor*)getEditor(cell);
      *(float*)m_model->getValueAt(cell.y, cell.x) = editCtrl->getFloatValue();
    }
    break;
  case LFT_DOUBLE:
    { CEditListNumericEditor *editCtrl = (CEditListNumericEditor*)getEditor(cell);
      *(double*)m_model->getValueAt(cell.y, cell.x) = editCtrl->getDoubleValue();
    }
    break;
  case LFT_STRING:
    { CEditListStringEditor  *editCtrl = (CEditListStringEditor*)getEditor(cell);
      *(String*)m_model->getValueAt(cell.y, cell.x) = editCtrl->getValue();;
    }
    break;
  case LFT_STRCOMBO:
    comboEditorValueToModel(cell);
    break;
  case LFT_BOOL :
    { CEditListBooleanEditor *button   = (CEditListBooleanEditor*)getEditor(cell);
      *(bool*)m_model->getValueAt(cell.y, cell.x) = button->getValue();
    }
    break;
  }
}

void CEditListCtrl::modelValueToList(const ListCell &cell, bool newItem) {
  const ListFieldType type = getListFieldType(cell);
  LVITEM lv;
  ZeroMemory(&lv, sizeof(lv));
  lv.iItem    = cell.y;
  lv.iSubItem = cell.x;

  switch(type) {
  case LFT_SHORT :
  case LFT_INT   :
  case LFT_FLOAT :
  case LFT_DOUBLE:
    { CEditListNumericEditor *editor = (CEditListNumericEditor*)getEditor(cell);
      const String str = editor->valueToString(getNumericValueAt(cell));
      lv.mask          = LVIF_TEXT;
      lv.pszText       = (LPTSTR)str.cstr();
      setItem(lv, newItem);
    }
    break;
  case LFT_STRING:
    { CEditListStringEditor *editor = (CEditListStringEditor*)getEditor(cell);
      const String v   = *(String*)m_model->getValueAt(cell.y, cell.x);
      const String str = editor->valueToString(v);
      lv.mask          = LVIF_TEXT;
      lv.pszText       = (LPTSTR)str.cstr();
      setItem(lv, newItem);
    }
    break;
  case LFT_STRCOMBO:
    { const String str = getModelComboStringAt(cell);
      lv.mask          = LVIF_TEXT;
      lv.pszText       = (LPTSTR)str.cstr();
      setItem(lv, newItem);
    }
    break;
  case LFT_BOOL :
    { const bool v     = *(bool*)m_model->getValueAt(cell.y, cell.x);
      lv.mask          = LVIF_TEXT;
      lv.pszText       = EMPTYSTRING;
      setItem(lv, newItem);
    }
    break;
  default:
    return;
  }
}

/*
static String ctrlTypeToString(int type) {
#define CASETYPE(t) case ODT_##t: return #t
  switch(type) {
  CASETYPE(BUTTON  );
  CASETYPE(COMBOBOX);
  CASETYPE(LISTBOX );
  CASETYPE(MENU    );
  CASETYPE(LISTVIEW);
  CASETYPE(STATIC  );
  CASETYPE(TAB     );
  default: return format("Unknown controltype:%d", type);
  }
}
*/

void CEditListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
//  String typeStr = ctrlTypeToString(lpDrawItemStruct->CtlType);

  CPoint       cell(0, lpDrawItemStruct->itemID);
  const bool   hasVisibleEditor = isEditorVisible();
  const CRect &rect             = lpDrawItemStruct->rcItem;
  const int    colCount         = getColumnCount();
  for(cell.x = 0; cell.x < colCount; cell.x++) {
    const CRect cr = getCellRect(cell);
    if((cr.right < rect.left) || (cr.left > rect.right)) {
      continue;
    }
    if(hasVisibleEditor && (cell == m_currentCell)) {
      continue;
    }
    clearItemRect(lpDrawItemStruct->hDC, cr);
    switch(getListFieldType(cell)) {
    case LFT_SHORT :
    case LFT_INT   :
    case LFT_FLOAT :
    case LFT_DOUBLE:
      { CRect           tr     = getTextRect(cell);
        CEditListNumericEditor *editor = (CEditListNumericEditor*)getEditor(cell);
        const String    str    = editor->valueToString(getNumericValueAt(cell));
        DrawText(lpDrawItemStruct->hDC, str.cstr(), (int)str.length(), &tr, DT_END_ELLIPSIS | DT_RIGHT /*| DT_VCENTER | DT_SINGLELINE*/);
      }
      break;
    case LFT_STRING:
      { CRect           tr     = getTextRect(cell);
        CEditListStringEditor  *editor = (CEditListStringEditor*)getEditor(cell);
        const String    str    = editor->valueToString(*(String*)m_model->getValueAt(cell.y,cell.x));
        DrawText(lpDrawItemStruct->hDC, str.cstr(), (int)str.length(), &tr, DT_END_ELLIPSIS | DT_LEFT /*| DT_VCENTER | DT_SINGLELINE*/);
      }
      break;
    case LFT_STRCOMBO:
      { CRect           tr     = getComboBoxTextRect(cell);
        const String    str    = getModelComboStringAt(cell);
        DrawText(lpDrawItemStruct->hDC, str.cstr(), (int)str.length(), &tr, DT_END_ELLIPSIS | DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        CEditListStringCombo::paintComboBoxArrow(lpDrawItemStruct->hDC, cr, isCellEditable(cell));
      }
      break;
    case LFT_BOOL:
      { const bool value = *(bool*)m_model->getValueAt(cell.y,cell.x);
        CEditListBooleanEditor::paintCheckBox(lpDrawItemStruct->hDC, getCheckBoxPosition(cell), value, isCellEditable(cell));
      }
      break;
    default:
      break;
    }
  }
}

double CEditListCtrl::getNumericValueAt(const ListCell &cell) {
  const ListFieldType type = getListFieldType(cell);
  switch(type) {
  case LFT_SHORT : return *(short *)m_model->getValueAt(cell.y, cell.x);
  case LFT_INT   : return *(int   *)m_model->getValueAt(cell.y, cell.x);
  case LFT_FLOAT : return *(float *)m_model->getValueAt(cell.y, cell.x);
  case LFT_DOUBLE: return *(double*)m_model->getValueAt(cell.y, cell.x);
  default        :
    throwException(_T("Call(%d,%d) not numeric. type=%s"), cell.y, cell.x, CTableModel::getListFieldTypeName(type).cstr());
    return 0;
  }
}

String CEditListCtrl::getModelComboStringAt(const ListCell &cell) {
  CEditListStringCombo *editCtrl = (CEditListStringCombo*)getEditor(cell);
  if(editCtrl->useStringValue()) {
    return *(String*)m_model->getValueAt(cell.y, cell.x);
  } else {
    const int index = *(int*)m_model->getValueAt(cell.y, cell.x);
    return editCtrl->getListString(index);
  }
}

void CEditListCtrl::modelValueToComboEditor(const ListCell &cell) {
  CEditListStringCombo *cbCtrl = (CEditListStringCombo*)getEditor(cell);
  if(cbCtrl->useStringValue()) {
    const String v = *(String*)m_model->getValueAt(cell.y, cell.x);
    cbCtrl->setStringValue(v);
  } else {
    const int v = *(int*)m_model->getValueAt(cell.y, cell.x);
    cbCtrl->setIntValue(v);
  }
}

void CEditListCtrl::comboEditorValueToModel(const ListCell &cell) {
  CEditListStringCombo *editCtrl = (CEditListStringCombo*)getEditor(cell);
  if(editCtrl->useStringValue()) {
    *(String*)m_model->getValueAt(cell.y, cell.x) = editCtrl->getStringValue();
  } else {
    *(int*   )m_model->getValueAt(cell.y, cell.x) = editCtrl->getIntValue();
  }
}

void CEditListCtrl::clearItemRect(HDC hdc, const CRect &r) {
  BitBlt(hdc, r.left,r.top, r.Width(), r.Height(), NULL, 0,0, WHITENESS);
}

int CEditListCtrl::findRowFromPoint(const CPoint &p) const {
  const int top         = GetTopIndex();
  const int lastVisible = top + GetCountPerPage();
  for(int i = top; i <= lastVisible; i++) {
    CRect rect;
    GetItemRect(i, rect, LVIR_BOUNDS);
    if(rect.PtInRect(p)) {
      return i;
    }
  }
  return -1;
}

int CEditListCtrl::findColumnFromPoint(int itemIndex, const CPoint &p) {
  const int columnCount = getColumnCount();
  for(int c = 1; c < columnCount; c++) {
    CRect rect;
    GetSubItemRect(itemIndex, c, LVIR_BOUNDS, rect);
    if(rect.PtInRect(p)) {
      return c;
    }
  }
  return -1;
}

ListCell CEditListCtrl::findCellFromPoint(const CPoint &p) {
  const int r = findRowFromPoint(p);
  if(r < 0) {
    return ListCell(-1,-1);
  }
  const int c = findColumnFromPoint(r, p);
  return (c < 0) ? ListCell(0,r) : ListCell(c,r);
}

int CEditListCtrl::getItemHeight() const { // in pixels
  const int n = m_model->getColumnCount();
  int h = 16;
  for(int i = 0; i < n; i++) {
    const ListFieldType type = m_model->getListFieldType(i);
    switch(type) {
    case LFT_STRCOMBO:
      h = max(h, 23);
      break;
    }
  }
  return h;
}

void CEditListCtrl::createBitmaps() {
  m_descBitmap.LoadOEMBitmap(OBM_COMBO);
  m_ascBitmap.LoadOEMBitmap(OBM_COMBO);
  const CSize bmSize = getBitmapSize(m_descBitmap);

  HDC screenDC  = getScreenDC();
  HDC srcHDC    = CreateCompatibleDC(screenDC);
  HDC dstHDC    = CreateCompatibleDC(screenDC);
  DeleteDC(screenDC);

  CDC        *srcDC     = CDC::FromHandle(srcHDC);
  CDC        *dstDC     = CDC::FromHandle(dstHDC);
  CGdiObject *oldSrcGDI = srcDC->SelectObject(&m_descBitmap);
  CGdiObject *oldDstGDI = dstDC->SelectObject(&m_ascBitmap);

  for(int y = 0, y1 = bmSize.cy-1; y < bmSize.cy; y++, y1--) {
    for(int x = 0; x < bmSize.cx; x++) {
      dstDC->SetPixel(x,y1, srcDC->GetPixel(x,y));
    }
  }
  dstDC->SelectObject(oldDstGDI);
  srcDC->SelectObject(oldSrcGDI);
  DeleteDC(dstHDC);
  DeleteDC(srcHDC);
}

void CEditListCtrl::paintHeaderSortMark() {
  if((m_sortColumn >= 0) && (m_sortColumn < getColumnCount())) {
    CHeaderCtrl *headerCtrl = GetHeaderCtrl();
    CRect headerRect;

    headerCtrl->GetItemRect(m_sortColumn, &headerRect);
    CClientDC dc(headerCtrl);
    CDC srcDC;
    srcDC.CreateCompatibleDC(NULL);
    CBitmap      *bitmap     = m_ascending ? &m_ascBitmap : &m_descBitmap;
    const CSize   bmSize     = getBitmapSize(*bitmap);
    CBitmap      *oldBitmap  = srcDC.SelectObject(bitmap);
    const String  columnName = m_model->getColumnName(m_sortColumn);

    dc.SelectObject(GetFont());
    CSize textSize = getTextExtent(dc, columnName);

    const CPoint arrowPos(headerRect.left + textSize.cx + bmSize.cx,(headerRect.Height() - bmSize.cy)/2);
    dc.BitBlt(arrowPos.x,arrowPos.y, bmSize.cx,bmSize.cy, &srcDC, 0, 0, SRCCOPY);
    srcDC.SelectObject(oldBitmap);
  }
}

CRect CEditListCtrl::getEditorRect(const ListCell &cell) const {
  switch(getListFieldType(cell)) {
  case LFT_SHORT    :
  case LFT_INT      :
  case LFT_FLOAT    :
  case LFT_DOUBLE   :
  case LFT_STRING   :
    return getTextRect(cell);
  case LFT_STRCOMBO :
    return getComboBoxWinRect(cell);
  case LFT_BOOL     :
    return getCheckBoxWinRect(cell);
  default:
    throwException(_T("Unknown fieldType for cell(%d,%d):%d"), cell.y,cell.x, m_model->getListFieldType(cell.x));
    return CRect(0,0,0,0);
  }
}

CRect CEditListCtrl::getTextRect(const ListCell &cell) const {
  CRect r = getCellRect(cell);
  if(cell.x == 0) {
//    r.left -= 1;
    r.left += 3;
  } else {
    r.left += 3;
  }
  if(!isCellEditable(cell)) {
    const CSize sz = getTextSize(cell);
    if(sz.cx+3 < r.Width()) {
      r.right = r.left + sz.cx + 6;
    }
  }
  return r;
}

CSize CEditListCtrl::getTextSize(const ListCell &cell) const {
  CClientDC dc((CWnd*)this);
  CFont *oldFont = dc.SelectObject(GetFont());
  const CSize result = getTextExtent(dc, getItemString(*this, cell.y, cell.x));
  dc.SelectObject(oldFont);
  return result;
}

CRect CEditListCtrl::getCellRect(const ListCell &cell) const {
  CRect rect;
  if(cell.x == 0) {
    GetItemRect(cell.y, rect, LVIR_LABEL);
  } else {
    ((CListCtrl*)this)->GetSubItemRect(cell.y, cell.x, LVIR_LABEL, rect);
  }
  return rect;
}

CRect CEditListCtrl::getComboBoxWinRect( const ListCell &cell) const {
  CRect r = getCellRect(cell);
  r.bottom += 70;
  return r;
}

CRect CEditListCtrl::getComboBoxTextRect(const ListCell &cell) const {
  CRect r = getCellRect(cell);
  if(cell.x == 0) {
    r.left += 4;
  } else {
    r.left += 4;
  }
  const CSize arrowSize = CEditListStringCombo::getComboArrowSize();
  if(arrowSize.cx < r.Width()) {
    r.right -= arrowSize.cx;
  } else {
    r.right = r.left + 1;
  }
  return r;
}

CRect CEditListCtrl::getCheckBoxWinRect(const ListCell &cell) const {
  const CPoint p  = getCheckBoxPosition(cell);
  const CSize  sz = CEditListBooleanEditor::getCheckBoxSize();
  return CRect(p.x-2,p.y-1, p.x + sz.cx, p.y + sz.cy+1);
}

CPoint CEditListCtrl::getCheckBoxPosition(const ListCell &cell) const {
  return CEditListBooleanEditor::getCheckBoxPosition(getCellRect(cell));
}

bool CEditListCtrl::isChildWindow(CWnd *wnd) {
  for(CWnd *child = GetWindow(GW_CHILD); child; child = child->GetNextWindow()) {
    if(child == wnd) {
      return true;
    }
  }
  return false;
}

void CEditListCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) {
  lpMeasureItemStruct->itemHeight = m_itemHeight;
}

void CEditListCtrl::OnLButtonDown(UINT nFlags, CPoint point) {
  const CPoint cell = findCellFromPoint(point);
  if((cell.x >= 0) && (cell.y >= 0)) {
    setCurrentCell(cell.y,cell.x);
  }
}

void CEditListCtrl::OnSetFocus(CWnd *pOldWnd) {
//  trace("OnSetFocus");

  if(!isChildWindow(pOldWnd)) {
    CListCtrl::OnSetFocus(pOldWnd);
  }
  if(ISCONTROLFLAGSET(HANDLE_KILLSET_FOCUS)) {
    if(hasCurrentCell()) {
      beginEdit();
    } else {
      setCurrentCell(0,0);
    }
  }
}

void CEditListCtrl::OnKillFocus(CWnd *pNewWnd) {
//  trace("OnKillFocus");

  if(!isChildWindow(pNewWnd)) {
    CListCtrl::OnKillFocus(pNewWnd);
  }
  if(ISCONTROLFLAGSET(HANDLE_KILLSET_FOCUS)) {
    if(hasCurrentEditor()) {
      endEdit();
    }
  }
}

void CEditListCtrl::OnEditSetFocus(UINT id) {
//  trace("Enter EditSetFocus(%d)", id);

  if(ISCONTROLFLAGSET(HANDLE_KILLSET_FOCUS)) {
    beginEdit();
  }
//  trace("Leave EditSetFocus(%d)", id);
}

void CEditListCtrl::OnEditKillFocus(UINT id) {
//  trace("Enter OnEditKillFocus(%d)", id);

  const bool ok = endEdit();
  if(!ok) m_currentEditor->PostMessage(WM_SETFOCUS);
//  trace("Leave OnEditKillFocus. endEdit=%s", boolToStr(ok));
}

void CEditListCtrl::OnSize(UINT nType, int cx, int cy) {
  CListCtrl::OnSize(nType, cx, cy);
  if(hasCurrentCell() && hasCurrentEditor()) {
    setWindowRect(m_currentEditor, getCurrentTextRect());
  }
}

void CEditListCtrl::OnItemchanged(NMHDR *pNMHDR, LRESULT *pResult) {
  if(ISCONTROLFLAGSET(HANDLE_ITEM_CHANGED)) {

//    trace("OnItemchanged");

    NMLISTVIEW *pNMListView = (NMLISTVIEW*)pNMHDR;
    if((pNMListView->uChanged & LVIF_STATE) && (pNMListView->uNewState & LVNI_SELECTED)) {
      const int index = GetSelectionMark();
      setCurrentCell(index, m_currentCell.x);
    }
  }
  *pResult = 0;
}

ColumnOrderArray::ColumnOrderArray(CListCtrl *listCtrl) {
  const int n = listCtrl->GetHeaderCtrl()->GetItemCount();
  int *indexArray = new int[n]; TRACE_NEW(indexArray);
  listCtrl->GetColumnOrderArray(indexArray);
  for(int i = 0; i < n; i++) {
    add(indexArray[i]);
  }
  SAFEDELETEARRAY(indexArray);
}

int ColumnOrderArray::findColumnIndex(UINT column) const {
  for(UINT i = 0; i < size(); i++) {
    if((*this)[i] == column) {
      return i;
    }
  }
  return -1;
}

static bool traceMessage(const MSG *pMsg) {
  switch(pMsg->message) {
  case WM_MOUSEMOVE  :
  case WM_NCMOUSEMOVE:
  case WM_MOUSEWHEEL :
  case WM_KEYUP      :
  case WM_SYSKEYDOWN :
  case WM_TIMER      :
  case 0x118         :
  case WM_PAINT      :
    return false;
  default            :
    return true;
  }
}


BOOL CEditListCtrl::PreTranslateMessage(MSG *pMsg) {

/*
 if(traceMessage(pMsg)) {
    trace("Enter PreTranslateMessage. MSG:%s", getMessageName(pMsg->message).cstr());
  }
*/
  const BOOL ret = PreTranslateMessage1(pMsg);
  if(ISCONTROLFLAGSET(GOTERROR) && hasCurrentEditor()) {
    CLRCONTROLFLAG(GOTERROR);
    m_currentEditor->SetFocus();
  }

/*
  if(traceMessage(pMsg)) {
    trace("Leave PreTranslateMessage returning %s", boolToStr(ret));
  }
*/
  return ret;
}

BOOL CEditListCtrl::PreTranslateMessage1(MSG *pMsg) {
  CLRCONTROLFLAG(MSGWASMOUSECLICK);

  switch(pMsg->message) {
  case WM_LBUTTONDOWN:
  case WM_MBUTTONDOWN:
  case WM_RBUTTONDOWN:
    SETCONTROLFLAG(MSGWASMOUSECLICK);
    m_lastMouseClick = pMsg->pt;
    ScreenToClient(&m_lastMouseClick);
    break;
  case WM_KEYDOWN:
//    trace("MSG:%s", getMessageName(pMsg->message).cstr());
    switch(pMsg->wParam) {
    case VK_TAB  :
      { const ColumnOrderArray coArray(this);
        const int colCount = (int)coArray.size();
        CPoint    cell     = getCurrentCell();
        int coIndex = coArray.findColumnIndex(cell.x);
        if(shiftKeyPressed()) { // prev. cell
          coIndex--;
          if(coIndex < 0) {
            coIndex = colCount - 1;
            cell.y--;
          }
          if(cell.y >= 0) {
            setCurrentCell(cell.y, coArray[coIndex]);
            return true;
          }
        } else {                // next cell
          coIndex++;
          if(coIndex >= colCount) {
            coIndex = 0;
            cell.y++;
          }
          if(cell.y < getRowCount()) {
            setCurrentCell(cell.y, coArray[coIndex]);
            return true;
          }
        }
      }
      break;
    case VK_DOWN :
      { CPoint cell = getCurrentCell();
        cell.y++;
        if(cell.y < getRowCount()) {
          setCurrentCell(cell.y, cell.x);
        }
      }
      return true;
    case VK_UP   :
      { CPoint cell = getCurrentCell();
        cell.y--;
        if(cell.y >= 0) {
          setCurrentCell(cell.y, cell.x);
        }
      }
      return true;
    case VK_NEXT :
      { CPoint cell = getCurrentCell();
        cell.y += GetCountPerPage();
        cell.y = min(cell.y, GetItemCount()-1);
        setCurrentCell(cell.y, cell.x);
      }
      return true;
    case VK_PRIOR:
      { CPoint cell = getCurrentCell();
        cell.y -= GetCountPerPage();
        cell.y = max(cell.y, 0);
        setCurrentCell(cell.y, cell.x);
      }
      return true;
    case VK_HOME:
      { if(ctrlKeyPressed()) {
          const ColumnOrderArray coArray(this);
          setCurrentCell(0, coArray[0]);
          return true;
        }
      }
      break;
    case VK_END:
      { if(ctrlKeyPressed()) {
          const ColumnOrderArray coArray(this);
          setCurrentCell(getRowCount()-1, coArray.last());
          return true;
        }
      }
      break;
    case VK_ESCAPE:
      { if(hasCurrentEditor() && isCurrentCellEditable()) {
          cancelEdit();
          beginEdit();
          return true;
        }
      }
      break;
    case VK_RETURN:
      { if(hasCurrentEditor() && isCurrentCellEditable()) {
          if(commitEdit()) {
            beginEdit();
          }
          return true;
        }
      }
      break;
    } // switch(pMsg->wParam)
    break;
  } // switch(pMsg->message)
  return CListCtrl::PreTranslateMessage(pMsg);
}

void CEditListCtrl::OnPaint() {
  CListCtrl::OnPaint();
  if(!hasCurrentEditor()) {
    return;
  }

  const CRect cr = getCurrentCellRect();
  if(cr != m_lastCellRect) {
    const CRect er = getCurrentEditorRect();
    const int headerBottom = getWindowRect(GetHeaderCtrl()).bottom;
    setWindowRect(m_currentEditor, er);
    if(er.top < headerBottom) {
      if(isEditorVisible()) {
        m_currentEditor->ShowWindow(SW_HIDE);
      }
    } else if(!isEditorVisible()) {
      m_currentEditor->ShowWindow(SW_SHOW);
      m_currentEditor->SetFocus();
    }
    m_lastCellRect = cr;
  }
}

void CEditListCtrl::OnColumnclick(NMHDR *pNMHDR, LRESULT *pResult) {
  NMLISTVIEW *pNMListView = (NMLISTVIEW*)pNMHDR;
  const int selectedColumn = pNMListView->iSubItem;

//  trace("ColumnClick %d", selectedColumn);

  if(selectedColumn == m_sortColumn) {
    setSortColumn(selectedColumn, !m_ascending);
  } else {
    setSortColumn(selectedColumn, m_ascending);
  }
  *pResult = 0;
}

void CEditListCtrl::trace(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  String msg = vformat(format,argptr);
  va_end(argptr);
  debugLog(_T("List:%-35s:%s\n"), msg.cstr(), infoToString().cstr());
}

/*
static String itemStateToString(UINT state) {
  String       result = _T("(");
  const TCHAR *delim  = EMPTYSTRING;
#define IFLIVS(n) if(state&(LVIS_##n)) { result += delim; result += #n; delim = _T(","); }
  IFLIVS(ACTIVATING    );
  IFLIVS(CUT           );
  IFLIVS(DROPHILITED   );
  IFLIVS(FOCUSED       );
  IFLIVS(OVERLAYMASK   );
  IFLIVS(SELECTED      );
  IFLIVS(STATEIMAGEMASK);
  result += ")";
  return result;
}
*/
String CEditListCtrl::infoToString() const {
  const int selMark = ((CEditListCtrl*)this)->GetSelectionMark();
  String    infoStr = format(_T("Sel.Mark:%2d, CurrentCell:(%2d,%2d) hasEdit:%s, flags:%s")
                            ,selMark
                            ,m_currentCell.y, m_currentCell.x
                            ,boolToStr(hasCurrentEditor())
                            ,m_controlFlags.toString().cstr()
                            );
/*
  if(selMark >= 0) {
    const UINT state = GetItemState(selMark, -1);
    infoStr += _T(", state:") + itemStateToString(state);
  }
*/
  return infoStr;
}

String EditListCtrlFlags::toString() const {
  String        result = _T("(");
  const  TCHAR *delim  = EMPTYSTRING;

#define IFSET(f) if(contains(f)) { result += delim; result += _T(#f); delim = _T(","); }

  IFSET(INIT_DONE           )
  IFSET(HANDLE_ITEM_CHANGED )
  IFSET(HANDLE_KILLSET_FOCUS)
  IFSET(COMMITACTIVE        )
  IFSET(GOTERROR            )
  IFSET(MSGWASMOUSECLICK    );
  result += _T(")");
  return result;
}
