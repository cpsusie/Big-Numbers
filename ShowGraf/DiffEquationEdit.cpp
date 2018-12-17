#include "stdafx.h"
#include "DiffEquationEdit.h"

CDiffEquationEdit::CDiffEquationEdit(CFont &font) : m_font(font) {
  m_subWndArray.add(&m_editName    );
  m_subWndArray.add(&m_label       );
  m_subWndArray.add(&m_editStartV  );
  m_subWndArray.add(&m_checkVisible);
  m_subWndArray.add(&m_colorButton );
  m_subWndArray.add(&m_buttonDelete);
  m_subWndArray.add(this           );
}

CompactArray<CRect> CDiffEquationEdit::calculateSubWinRect(const CRect &r) const {
  const int MARGIN = 15;
  const int SPACE  = 8;

  const int nameWidth    = 40;
  const int nameHeight   = 22;
  const int nameLeft     = r.left + MARGIN;
  const int nameRight    = nameLeft + nameWidth;
  const int nameTop      = r.CenterPoint().y - nameHeight/2;

  const int lblWidth     = 22;
  const int lblHeight    = nameHeight;
  const int lblLeft      = nameRight + 2;
  const int lblRight     = lblLeft  + lblWidth;
  const int lblTop       = nameTop;

  const int delWidth     = 28;
  const int delHeight    = 23;
  const int delRight     = r.right  - MARGIN;
  const int delLeft      = delRight - delWidth;
  const int delTop       = r.CenterPoint().y - delHeight/2;

  const int colorWidth   = 40;
  const int colorHeight  = nameHeight;
  const int colorRight   = delLeft    - SPACE;
  const int colorLeft    = colorRight - colorWidth;
  const int colorTop     = r.CenterPoint().y - colorHeight/2;

  const int visWidth     = 80;
  const int visHeight    = nameHeight;
  const int visRight     = colorLeft - SPACE;
  const int visLeft      = visRight  - visWidth;
  const int visTop       = r.CenterPoint().y - visHeight/2;

  const int startVWidth  = 90;
  const int startVHeight = nameHeight;
  const int startVRight  = visLeft - SPACE;
  const int startVLeft   = startVRight - startVWidth;
  const int startVTop    = r.CenterPoint().y - startVHeight/2;

  const int exprLeft     = lblRight + SPACE;
  const int exprHeight   = r.Height();
  const int exprRight    = max(exprLeft+SPACE, startVLeft-SPACE);
  const int exprTop      = r.top;

  const CRect nameRect(  nameLeft  , nameTop  , nameRight    , nameTop   + nameHeight  );
  const CRect lblRect(   lblLeft   , lblTop   , lblRight     , lblTop    + lblHeight   );
  const CRect startVRect(startVLeft, startVTop, startVRight  , startVTop + startVHeight);
  const CRect visRect (  visLeft   , visTop   , visRight     , visTop    + visHeight   );
  const CRect colorRect (colorLeft , colorTop , colorRight   , colorTop  + visHeight   );
  const CRect delBtnRect(delLeft   , delTop   , delRight     , delTop    + delHeight   );
  const CRect exprRect(  exprLeft  , exprTop  , exprRight    , exprTop   + exprHeight  );

  CompactArray<CRect> result(m_subWndArray.size());
  result.add(nameRect  );
  result.add(lblRect   );
  result.add(startVRect);
  result.add(visRect   );
  result.add(colorRect );
  result.add(delBtnRect);
  result.add(exprRect  );
  return result;
}

void CDiffEquationEdit::Create(CWnd *parent, UINT eqIndex) {
  const CRect dummyRect(10,10,30,30);

#define STD_STYLES    WS_VISIBLE | WS_TABSTOP      | WS_CHILD
#define NAME_STYLE    STD_STYLES | ES_RIGHT        | WS_BORDER
#define LABEL_STYLE   WS_VISIBLE | SS_CENTERIMAGE
#define EXPR_STYLE    STD_STYLES | WS_VSCROLL      | WS_BORDER      | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN
#define STARTV_STYLE  STD_STYLES | ES_RIGHT        | ES_AUTOHSCROLL | WS_BORDER
#define VISIBLE_STYLE STD_STYLES | BS_AUTOCHECKBOX | BS_LEFTTEXT

  m_exprId = DIFFEUQUATIONFIELDID(eqIndex);
  m_editName.Create(                    NAME_STYLE   , dummyRect, parent  , getNameId()       );
  m_editName.SetFont(&m_font, FALSE);

  m_label.Create(  _T("' =")          , LABEL_STYLE  , dummyRect, parent  , getLabelId()      );
  m_label.SetFont(&m_font, FALSE);

  CEdit::Create(                        EXPR_STYLE   , dummyRect, parent  , getExprId()       );
  ModifyStyleEx(0, WS_EX_CLIENTEDGE);
  SetFont(&m_font, FALSE);

  m_editStartV.Create(                  STARTV_STYLE , dummyRect, parent  , getStartVId()     );
  m_editStartV.SetFont(&m_font, FALSE);
  m_checkVisible.Create(_T("Visible:"), VISIBLE_STYLE, dummyRect, parent  , getVisibleId()    );
  m_checkVisible.SetFont(&m_font, FALSE);

  m_colorButton.Create(_T("Color")    , STD_STYLES   , dummyRect, parent  , getColorId()      );
  m_colorButton.EnableOtherButton(_T("Other"));
  m_buttonDelete.Create(parent                       , dummyRect.TopLeft(), getDeleteId(),true);
}

#define FOREACHSUBWIN(i) for(Iterator<CWnd*> i = m_subWndArray.getIterator(); i.hasNext();)

void CDiffEquationEdit::DoDataExchange(CDataExchange *pDX) {
  DDX_Text(    pDX, getNameId()   , m_name      );
  DDX_Text(    pDX, getExprId()   , m_expr      );
  DDX_Text(    pDX, getStartVId() , m_startValue);
  DDX_Check(   pDX, getVisibleId(), m_visible   );
}

void CDiffEquationEdit::paramToWin(const DiffEquationDescription &desc, const EquationAttributes &attr) {
  m_name         = desc.getName().cstr();
  m_expr         = desc.getExprText().cstr();
  m_startValue   = attr.getStartValue();
  m_visible      = attr.isVisible();
  m_colorButton.SetColor(attr.getColor());
}

void CDiffEquationEdit::winToParam(DiffEquationDescription &desc, EquationAttributes &attr) {
  desc.setName(     (LPCTSTR)m_name          );
  desc.setExprText( (LPCTSTR)m_expr          );
  attr.setStartValue(m_startValue            );
  attr.setVisible(   m_visible ? true : false);
  attr.setColor(     m_colorButton.GetColor());
}

bool CDiffEquationEdit::getVisibleChecked() {
  return (m_checkVisible.GetCheck() == BST_CHECKED);
}

void CDiffEquationEdit::setVisibleChecked(bool checked) {
  m_checkVisible.SetCheck(checked ? BST_CHECKED : BST_UNCHECKED);
  m_colorButton.EnableWindow(checked ? 1 : 0);
}

CRect CDiffEquationEdit::getWindowRect() {
  CRect r = ::getWindowRect(this);
  FOREACHSUBWIN(it) r.UnionRect(r, ::getWindowRect(it.next()));
  return r;
}

void CDiffEquationEdit::setWindowRect(const CRect &r) {
  const CompactArray<CRect> ra = calculateSubWinRect(r);
  for(size_t i = 0; i < m_subWndArray.size(); i++) {
    ::setWindowRect(m_subWndArray[i], ra[i]);
  }
}

DiffEquationField CDiffEquationEdit::findFieldByCtrlId(UINT id) const {
  if(!containsCtrlId(id)) return (DiffEquationField)-1;
  switch (id - getExprId()) {
  case 0 : return EQ_EXPR_EDIT;
  case 1 : return EQ_NAME_EDIT;
  case 2 : return EQ_LABEL_STATIC;
  case 3 : return EQ_STARTV_EDIT;
  case 4 : return EQ_VISIBLE_BUTTON;
  case 5 : return EQ_COLOR_BUTTON;
  case 6 : return EQ_DELETE_BUTTON;
  default: return (DiffEquationField)-1;
  }
}

CompactIntArray CDiffEquationEdit::getTabOrderArray() const {
  CompactIntArray a;
  a.add(getNameId());
  a.add(getExprId());
  a.add(getStartVId());
  a.add(getVisibleId());
  a.add(getColorId());
  a.add(getDeleteId());
  return a;
}

int CDiffEquationEdit::getExprId() const {
  return m_exprId;
}

int CDiffEquationEdit::getNameId() const {
  return m_exprId + 1;
}

int CDiffEquationEdit::getLabelId() const {
  return m_exprId + 2;
}

int CDiffEquationEdit::getStartVId() const {
  return m_exprId + 3;
}

int CDiffEquationEdit::getVisibleId() const {
  return m_exprId + 4;
}

int CDiffEquationEdit::getColorId() const {
  return m_exprId + 5;
}

int CDiffEquationEdit::getDeleteId() const {
  return m_exprId + 6;
}

CDiffEquationEdit::~CDiffEquationEdit() {
  FOREACHSUBWIN(it) it.next()->DestroyWindow();
}

#define TOP_EQUATION    0x1
#define BOTTOM_EQUATION 0x2

void CDiffEquationEdit::addToLayoutManager(SimpleLayoutManager &layoutManager, int flags) {
  layoutManager.addControl(getNameId()   , PCT_RELATIVE_RIGHT | PCT_RELATIVE_Y_POS);
  layoutManager.addControl(getLabelId()  , PCT_RELATIVE_X_POS | PCT_RELATIVE_Y_POS);
  int exprAttr = PCT_RELATIVE_LEFT | RELATIVE_RIGHT | PCT_RELATIVE_TOP | PCT_RELATIVE_BOTTOM;
  layoutManager.addControl(getExprId()   , exprAttr);
  layoutManager.addControl(getStartVId() , RELATIVE_X_POS     | PCT_RELATIVE_Y_POS);
  layoutManager.addControl(getVisibleId(), RELATIVE_X_POS     | PCT_RELATIVE_Y_POS);
  layoutManager.addControl(getColorId()  , RELATIVE_X_POS     | PCT_RELATIVE_Y_POS);
  layoutManager.addControl(getDeleteId() , RELATIVE_X_POS     | PCT_RELATIVE_Y_POS);
}

void CDiffEquationEdit::removeFromLayoutManager(SimpleLayoutManager &layoutManager) {
  for(size_t i = 0; i < m_subWndArray.size(); i++) {
    layoutManager.removeControl((int)(m_exprId + i));
  }
}

// ----------------------------- CDiffEquationEditArray -----------------------------

int CDiffEquationEditArray::findEquationIndexByCtrlId(UINT id) const {
  const size_t n = size();
  for(size_t i = 0; i < n; i++) {
    if((*this)[i]->containsCtrlId(id)) {
      return (int)i;
    }
  }
  return -1;
}

CDiffEquationEdit *CDiffEquationEditArray::findEquationByCtrlId(UINT id) const {
  const int index = findEquationIndexByCtrlId(id);
  return (index < 0) ? NULL : (*this)[index];
}

void CDiffEquationEditArray::addEquation(CWnd *wnd, CFont &font) {
  CDiffEquationEdit *eq = new CDiffEquationEdit(font); TRACE_NEW(eq);
  eq->Create(wnd, (UINT)size());
  eq->setVisibleChecked(true);
  add(eq);
}

void CDiffEquationEditArray::remove(size_t index) {
  CDiffEquationEdit *eq = (*this)[index];
  SAFEDELETE(eq);
  __super::remove(index);
}

void CDiffEquationEditArray::clear() {
  while (!isEmpty()) {
    remove(size()-1);
  }
  __super::clear();
}

void CDiffEquationEditArray::addAllToLayoutManager(SimpleLayoutManager &layoutManager) {
  const size_t n = size();
  for(size_t i = 0; i < n; i++) {
    addEquationToLayoutManager(layoutManager, i);
  }
}

void CDiffEquationEditArray::removeAllFromLayoutManager(SimpleLayoutManager &layoutManager) {
  const size_t n = size();
  for(size_t i = 0; i < n; i++) {
    removeEquationFromLayoutManager(layoutManager, i);
  }
}

void CDiffEquationEditArray::addEquationToLayoutManager(SimpleLayoutManager &layoutManager, size_t index) {
  int flags = 0;
  if(index == 0         ) flags |= TOP_EQUATION;
  if(index == size() - 1) flags |= BOTTOM_EQUATION;
  (*this)[index]->addToLayoutManager(layoutManager, flags);
}

void CDiffEquationEditArray::removeEquationFromLayoutManager(SimpleLayoutManager &layoutManager, size_t index) {
  (*this)[index]->removeFromLayoutManager(layoutManager);
}

void CDiffEquationEditArray::winToParam(DiffEquationGraphParameters &param) {
  param.removeAllEquations();
  const size_t n = size();
  for(size_t i = 0; i < n; i++) {
    DiffEquationDescription desc;
    EquationAttributes      attr;
    (*this)[i]->winToParam(desc, attr);
    param.addEquation(desc, attr);
  }
}

void CDiffEquationEditArray::paramToWin(const DiffEquationGraphParameters &param) {
  const size_t n = size();
  assert(n == param.getEquationCount());
  for(UINT i = 0; i < n; i++) {
    const DiffEquationDescription &desc = param.getEquationDescription(i);
    const EquationAttributes      &attr = param.getEquationAttribute(  i);
    (*this)[i]->paramToWin(desc, attr);
  }
}

void CDiffEquationEditArray::DoDataExchange(CDataExchange *pDX) {
  for(size_t i = 0; i < size(); i++) {
    (*this)[i]->DoDataExchange(pDX);
  }
}
