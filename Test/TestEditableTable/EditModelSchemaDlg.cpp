#include "stdafx.h"
#include "testeditabletable.h"
#include "EditModelSchemaDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CEditModelSchemaDlg::CEditModelSchemaDlg(const ListCtrlModelSchema &schema, CWnd *pParent /*=NULL*/) : CDialog(CEditModelSchemaDlg::IDD, pParent) {
  m_schema = schema;
}

void CEditModelSchemaDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEditModelSchemaDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTONUP  , OnButtonUp  )
    ON_BN_CLICKED(IDC_BUTTONDOWN, OnButtonDown)
    ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CEditModelSchemaDlg::OnInitDialog() {
  __super::OnInitDialog();

  m_model.setModelSchema(m_schema);
  m_list.substituteControl(this, IDC_LISTBASESCHEMA, m_model);

  m_layoutManager.OnInitDialog(this);

  m_layoutManager.addControl(IDC_LISTBASESCHEMA , RELATIVE_SIZE     | RESIZE_LISTHEADERS | INIT_LISTHEADERS );
  m_layoutManager.addControl(IDC_BUTTONUP       , RELATIVE_X_POS    | PCT_RELATIVE_Y_POS                    );
  m_layoutManager.addControl(IDC_BUTTONDOWN     , RELATIVE_X_POS    | PCT_RELATIVE_Y_POS                    );
  m_layoutManager.addControl(IDOK               , RELATIVE_POSITION                                         );
  m_layoutManager.addControl(IDCANCEL           , RELATIVE_POSITION                                         );

  return TRUE;
}

void CEditModelSchemaDlg::OnOK() {
  __super::OnOK();
}

void CEditModelSchemaDlg::OnButtonUp() {
}

void CEditModelSchemaDlg::OnButtonDown() {
}

void CEditModelSchemaDlg::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
}

BaseTableModel::BaseTableModel(const ListCtrlModelSchema &modelSchema) {
  setModelSchema(modelSchema);
}

void BaseTableModel::setModelSchema(const ListCtrlModelSchema &modelSchema) {
  m_elementArray.clear();
  for(size_t i = 0; i < modelSchema.size(); i++) {
    m_elementArray.add(ListFieldDefinition(modelSchema[i]));
  }
}

ListCtrlModelSchema BaseTableModel::getModelSchema() const {
  ListCtrlModelSchema result;
  for(size_t i = 0; i < m_elementArray.size(); i++) {
    result.add(m_elementArray[i]);
  }
  return result;
}

UINT BaseTableModel::getRowCount() {
  return (UINT)m_elementArray.size();
}

UINT BaseTableModel::getColumnCount() {
  return 13;
}

String BaseTableModel::getColumnName(UINT column) {
  switch(column) {
  case  0: return "Header";
  case  1: return "Type";
  case  2: return "Enabled";
  case  3: return "Width";
  case  4: return "HasL.Lim";
  case  5: return "L.Lim.Excl";
  case  6: return "L.Lim";
  case  7: return "HasU.Lim";
  case  8: return "U.Lim.Excl";
  case  9: return "U.Lim";
  case 10: return "Show zero";
  case 11: return "Decimals";
  case 12: return "Dropdown style";
  default: return "?";
  }
}

UINT BaseTableModel::getColumnWidth(UINT column) {
  switch(column) {
  case  0: return 50;                                    // "Header"
  case  1: return 50;                                    // "Type"
  case  2: return 35;                                    // "Enabled"
  case  3: return 35;                                    // "Width"
  case  4: return 60;                                    // "HasLowerLimit"
  case  5: return 60;                                    // "LowerLimitExcl"
  case  6: return 60;                                    // "LowerLimit"
  case  7: return 60;                                    // "HasUpperLimit"
  case  8: return 60;                                    // "UpperLimitExcl"
  case  9: return 60;                                    // "UpperLimit"
  case 10: return 35;                                    // "Show zero"
  case 11: return 35;                                    // "Decimals"
  case 12: return 35;                                    // "Dropdown style"
  default: return 10;                                    // "?"
  }
}

UINT BaseTableModel::getFieldFlags(UINT column) {
  switch(column) {
  case  0: return LFT_STRING;                            // "Header"
  case  1: return LFT_STRCOMBO | LF_STRCOMBO_DROPLIST;   // "Type"
  case  2: return LFT_BOOL;                              // "Enabled"
  case  3: return LFT_SHORT;                             // "Width"
  case  4: return LFT_BOOL;                              // "HasLowerLimit"
  case  5: return LFT_BOOL;                              // "LowerLimitExcl"
  case  6: return LFT_DOUBLE;                            // "LowerLimit"
  case  7: return LFT_BOOL;                              // "HasUpperLimit"
  case  8: return LFT_BOOL;                              // "UpperLimitExcl"
  case  9: return LFT_DOUBLE;                            // "UpperLimit"
  case 10: return LFT_BOOL;                              // "Show zero"
  case 11: return LFT_SHORT;                             // "Decimals"
  case 12: return LFT_STRCOMBO | LF_STRCOMBO_DROPLIST;   // "Dropdown style"
  default: return LFT_BOOL;                              // "?"
  }
}

void *BaseTableModel::getValueAt(UINT row, UINT column) {
  ListFieldDefinition &e = m_elementArray[row];
  switch(column) {
  case  0: return &e.m_header;                           // "Header"
  case  1: return &e.m_type;                             // "Type"
  case  2: return &e.m_enabled;                          // "Enabled"
  case  3: return &e.m_width;                            // "Width"
  case  4: return &e.m_hasLowerLimit;                    // "HasLowerLimit"
  case  5: return &e.m_lowerLimitExclusive;              // "LowerLimitExcl"
  case  6: return &e.m_lowerLimit;                       // "LowerLimit"
  case  7: return &e.m_hasUpperLimit;                    // "HasUpperLimit"
  case  8: return &e.m_upperLimitExclusive;              // "UpperLimitExcl"
  case  9: return &e.m_upperLimit;                       // "UpperLimit"
  case 10: return &e.m_showZero;                         // "Show zero"
  case 11: return &e.m_decimalCount;                     // "Decimals"
  case 12: return &e.m_dropDownStyle;                    // "Dropdown style"
  default: return NULL;                                  // "?"
  }
}

bool BaseTableModel::isEditableCell(UINT row, UINT column) {
  return true;
}

static const char *typeNames[] = {
  ""
 ,"short"
 ,"int"
 ,"float"
 ,"double"
 ,"string"
 ,"combobox"
 ,"checkbox"
 ,NULL
};

static const char *dropdownStyle[] = {
  ""
 ,"drop down"
 ,"drop list"
 ,NULL
};

typedef enum {
  DD_STYLE_NONE
 ,DD_STYLE_DROPDOWN
 ,DD_STYLE_DROPLIST
} DropDownStyle;

const StringArray BaseTableModel::getStrComboStringArray(UINT column) {
  switch(column) {
  case 1 :
    return StringArray(typeNames);
  case 12:
    return StringArray(dropdownStyle);
  default:
    return StringArray();
  }
}

void BaseTableModel::clearTable() {
  m_elementArray.clear();
}

void BaseTableModel::addElements(UINT count) {
  m_elementArray.add(ListFieldDefinition(ListFieldAttribute()));
}

void BaseTableModel::removeElement(UINT index) {
  m_elementArray.removeIndex(index);
}

ListFieldDefinition::ListFieldDefinition(const ListFieldAttribute &attr) {
  m_header              = attr.m_header;
  m_type                = attr.getType();
  m_enabled             = attr.m_enabled;
  m_width               = attr.m_width;

  if(CTableModel::isNumericType(m_type)) {
    m_hasLowerLimit       = (attr.m_flags & LF_NUM_HAS_LOWERLIMIT      ) != 0;
    m_lowerLimitExclusive = (attr.m_flags & LF_NUM_LOWERLIMIT_EXCLUSIVE) != 0;
    m_lowerLimit          = attr.m_numberInterval.getFrom();
    m_hasUpperLimit       = (attr.m_flags & LF_NUM_HAS_UPPERLIMIT      ) != 0;
    m_upperLimitExclusive = (attr.m_flags & LF_NUM_UPPERLIMIT_EXCLUSIVE) != 0;
    m_upperLimit          = attr.m_numberInterval.getFrom();
    m_showZero            = (attr.m_flags & LF_NUM_SHOW_ZERO           ) != 0;
    m_decimalCount        = attr.getDecimalCount();
  } else {
    m_hasLowerLimit       = false;
    m_lowerLimitExclusive = false;
    m_lowerLimit          = 0;
    m_hasUpperLimit       = false;
    m_upperLimitExclusive = false;
    m_upperLimit          = 0;
    m_showZero            = false;
    m_decimalCount        = 0;
  }
  if(m_type == LFT_STRCOMBO) {
    m_dropDownStyle  = (attr.m_flags & LF_STRCOMBO_DROPDOWN) ? DD_STYLE_DROPDOWN : DD_STYLE_DROPLIST;
    m_comboStrings = attr.m_comboStrings;
  } else {
    m_dropDownStyle  = DD_STYLE_NONE;
  }
}

ListFieldDefinition::operator ListFieldAttribute() const {
  ListFieldAttribute attr;
  attr.m_header  = m_header;
  attr.m_flags   = m_type;
  attr.m_enabled = m_enabled;
  attr.m_width   = m_width;
  if(CTableModel::isNumericType(m_type)) {
    const DoubleInterval maxInterval = CTableModel::getMaxInterval(m_type);
    if(m_hasLowerLimit          ) attr.m_flags |= LF_NUM_HAS_LOWERLIMIT;
    if(m_lowerLimitExclusive    ) attr.m_flags |= LF_NUM_LOWERLIMIT_EXCLUSIVE;
    attr.m_numberInterval.setFrom(m_hasLowerLimit ? m_lowerLimit : maxInterval.getMin());
    if(m_hasUpperLimit          ) attr.m_flags |= LF_NUM_HAS_UPPERLIMIT;
    if(m_upperLimitExclusive    ) attr.m_flags |= LF_NUM_UPPERLIMIT_EXCLUSIVE;
    attr.m_numberInterval.setTo(  m_hasUpperLimit ? m_upperLimit : maxInterval.getMax());
    if(m_showZero               ) attr.m_flags |= LF_NUM_SHOW_ZERO;
    if(!LF_ISINTEGERTYPE(m_type)) attr.m_flags |= LF_NUM_DECIMALS(m_decimalCount);
  } else if(m_type == LFT_STRCOMBO) {
    switch(m_dropDownStyle) {
    case DD_STYLE_NONE     : break;
    case DD_STYLE_DROPDOWN : attr.m_flags |= LF_STRCOMBO_DROPDOWN; break;
    case DD_STYLE_DROPLIST : attr.m_flags |= LF_STRCOMBO_DROPLIST; break;
    }
    attr.m_comboStrings = m_comboStrings;
  }
  return attr;
}

ListCtrlModelSchema::ListCtrlModelSchema(CTableModel &model) {
  const int n = model.getColumnCount();
  for(int i = 0; i < n; i++) {
    ListFieldAttribute attr;
    attr.m_header            = model.getColumnName(i);
    attr.m_enabled           = true;
    const ListFieldType type = model.getListFieldType(i);
    attr.m_flags             = model.getFieldFlags(i);
    if(CTableModel::isNumericType(type)) {
      if(LF_NUM_HASINTERVAL(attr.m_flags)) {
        attr.m_numberInterval = model.getLegalInterval(i);
      }
    } else if(type == LFT_STRCOMBO) {
      attr.m_comboStrings = model.getStrComboStringArray(i);
    }
    add(attr);
  }
}

ListCtrlField::ListCtrlField(const ListFieldAttribute &attr) : m_flags(attr.m_flags) {
  if(useString()) {
    m_string = new String();
  } else {
    m_int = 0;
  }
}

bool ListCtrlField::useString() const {
  switch(getType()) {
  case LFT_STRING   :
    return true;
  case LFT_STRCOMBO :
    return (m_flags & LF_STRCOMBO_DROPDOWN) != 0;
  default           :
    return false;
  }
}

ListCtrlField::~ListCtrlField() {
  if(useString()) {
    delete m_string;
  }
}

void *ListCtrlField::getValue() {
  return useString() ? (void*)m_string : (void*)&m_int;
}

String ListCtrlField::toString() const {
  switch(getType()) {
  case LFT_SHORT    : return format(_T("%d" ), m_short );
  case LFT_INT      : return format(_T("%d" ), m_int   );
  case LFT_FLOAT    : return format(_T("%g" ), m_float );
  case LFT_DOUBLE   : return format(_T("%lg"), m_double);
  case LFT_STRING   : return *m_string;
  case LFT_STRCOMBO : return useString() ? *m_string : format(_T("%d"), m_int);
  case LFT_BOOL     : return boolToStr(m_bool);
  default           : return "?";
  }
}

ListCtrlItem::ListCtrlItem(const ListCtrlModelSchema &schema) {
  for(size_t i = 0; i < schema.size(); i++) {
    m_fields.add(ListCtrlField(schema[i]));
  }
}

String ListCtrlItem::toString() const {
  String result;
  return result;
}
