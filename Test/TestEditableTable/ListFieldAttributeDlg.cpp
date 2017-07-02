#include "stdafx.h"
#include "testeditabletable.h"
#include "ListFieldAttributeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CListFieldAttributeDlg::CListFieldAttributeDlg(const ListFieldAttribute &attr, CWnd *pParent /*=NULL*/) : CDialog(CListFieldAttributeDlg::IDD, pParent) {
    m_attr = attr;
    const ListFieldType type = LF_GETTYPE(attr.m_flags);

    m_header              = attr.m_header.cstr();
    m_fieldType           = fieldTypeToComboValue(type);
    m_enabled             = attr.m_enabled ? TRUE : FALSE;
    m_hasLowerLimit       = isNumericType(type) && (attr.m_flags & LF_NUM_HAS_LOWERLIMIT      );
    m_hasUpperLimit       = isNumericType(type) && (attr.m_flags & LF_NUM_HAS_UPPERLIMIT      );
    m_lowerLimitExclusive = isNumericType(type) && (attr.m_flags & LF_NUM_LOWERLIMIT_EXCLUSIVE);
    m_upperLimitExclusive = isNumericType(type) && (attr.m_flags & LF_NUM_UPPERLIMIT_EXCLUSIVE);
    m_lowerLimit          = isNumericType(type) ? attr.m_numberInterval.getMin() : 0;
    m_upperLimit          = isNumericType(type) ? attr.m_numberInterval.getMax() : 0;
    m_showZero            = isNumericType(type) && (attr.m_flags & LF_NUM_SHOW_ZERO           );
    m_decimalCount        = LF_ISINTEGERTYPE(type) ? attr.getDecimalCount() : 0;
    m_dropdownStyle       = (type == LFT_STRCOMBO) ? ((attr.m_flags & LF_STRCOMBO_DROPLIST) ? 0 : 1) : -1;
    m_width               = 0;
}


void CListFieldAttributeDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
    DDX_Text(   pDX, IDC_EDITHEADER              , m_header             );
    DDX_CBIndex(pDX, IDC_COMBOFIELDTYPE          , m_fieldType          );
    DDX_Check(  pDX, IDC_CHECKENABLED            , m_enabled            );
    DDX_Check(  pDX, IDC_CHECKHASLOWERLIMIT      , m_hasLowerLimit      );
    DDX_Check(  pDX, IDC_CHECKHASUPPERLIMIT      , m_hasUpperLimit      );
    DDX_Check(  pDX, IDC_CHECKLOWERLIMITEXCLUSIVE, m_lowerLimitExclusive);
    DDX_Check(  pDX, IDC_CHECKUPPERLIMITEXCLUSIVE, m_upperLimitExclusive);
    DDX_Text(   pDX, IDC_EDITLOWERLIMIT          , m_lowerLimit         );
    DDX_Text(   pDX, IDC_EDITUPPERLIMIT          , m_upperLimit         );
    DDX_Check(  pDX, IDC_CHECKSHOWZERO           , m_showZero           );
    DDX_Text(   pDX, IDC_EDITDECIMALS            , m_decimalCount       );
    DDV_MinMaxUInt(pDX, m_decimalCount, 0, 15);
    DDX_Radio(  pDX, IDC_RADIODROPDOWN           , m_dropdownStyle      );
    DDX_Text(   pDX, IDC_EDITWIDTH               , m_width              );
}


BEGIN_MESSAGE_MAP(CListFieldAttributeDlg, CDialog)
    ON_CBN_SELCHANGE(IDC_COMBOFIELDTYPE, OnSelChangeComboFieldType)
    ON_COMMAND(ID_GOTOHEADER  , OnGotoHeader    )
    ON_COMMAND(ID_GOTODECIMALS, OnGotoDecimals  )
    ON_COMMAND(ID_GOTOWIDTH   , OnGotoWidth     )
END_MESSAGE_MAP()

BOOL CListFieldAttributeDlg::OnInitDialog() {
  __super::OnInitDialog();

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_ACCELERATOR_LISTFIELDATTRIBUTE));

  setFieldType(comboValueToFieldType(m_fieldType));
  gotoEditBox(this, IDC_STATICHEADER);

  return FALSE;
}

void CListFieldAttributeDlg::OnOK() {
  UpdateData();
  paramFromWindow(m_attr);
  __super::OnOK();
}

void CListFieldAttributeDlg::OnGotoHeader() {
  gotoEditBox(this, IDC_EDITHEADER  );
}

void CListFieldAttributeDlg::OnGotoDecimals() {
  gotoEditBox(this, IDC_EDITDECIMALS);
}

void CListFieldAttributeDlg::OnGotoWidth() {
  gotoEditBox(this, IDC_EDITWIDTH   );
}

void CListFieldAttributeDlg::paramFromWindow(ListFieldAttribute &attr) {
  const ListFieldType type = getFieldType();

  attr.m_header  = m_header;
  attr.m_enabled = m_enabled ? true : false;
  attr.m_flags   = type;
  attr.m_width;
  switch(type) {
  case LFT_SHORT    :
  case LFT_INT      :
  case LFT_FLOAT    :
  case LFT_DOUBLE   :
    { if(m_hasLowerLimit      ) attr.m_flags |= LF_NUM_HAS_LOWERLIMIT;
      if(m_hasUpperLimit      ) attr.m_flags |= LF_NUM_HAS_UPPERLIMIT;
      if(m_lowerLimitExclusive) attr.m_flags |= LF_NUM_LOWERLIMIT_EXCLUSIVE;
      if(m_upperLimitExclusive) attr.m_flags |= LF_NUM_UPPERLIMIT_EXCLUSIVE;
      if(m_showZero           ) attr.m_flags |= LF_NUM_SHOW_ZERO;
      if(!LF_ISINTEGERTYPE(type)) {
        attr.m_flags |= LF_NUM_DECIMALS(m_decimalCount);
      }
      const DoubleInterval interval(m_lowerLimit, m_upperLimit);
      const DoubleInterval maxInterval = CTableModel::getMaxInterval(type);
      if(!maxInterval.contains(interval)) {
        throwException(_T("NumberInterval(%lf,%lf) not contained in max interval for specified type")
                      ,m_lowerLimit, m_upperLimit);
      }
      if(LF_NUM_HASINTERVAL(attr.m_flags)) {
        attr.m_numberInterval = interval;
      }
    }
    break;

  case LFT_STRING   :
    break;
  case LFT_STRCOMBO :

  case LFT_BOOL     :
    break;
  }
}

void CListFieldAttributeDlg::OnSelChangeComboFieldType() {
  CComboBox *cb = (CComboBox*)GetDlgItem(IDC_COMBOFIELDTYPE);
  int v = cb->GetCurSel();
  setFieldType(comboValueToFieldType(v));
}

static const int numTypeFieldTable[] = {
   IDC_CHECKHASLOWERLIMIT
  ,IDC_CHECKHASUPPERLIMIT
  ,IDC_CHECKLOWERLIMITEXCLUSIVE
  ,IDC_CHECKUPPERLIMITEXCLUSIVE
  ,IDC_CHECKSHOWZERO
  ,IDC_EDITDECIMALS
  ,IDC_EDITLOWERLIMIT
  ,IDC_EDITUPPERLIMIT
  ,IDC_STATICNUMERICFRAME
};

static const int comboTypeFieldTable[] = {
  IDC_RADIODROPDOWN
 ,IDC_RADIODROPLIST
 ,IDC_STATICCOMBOSTYLEFRAME
};

void CListFieldAttributeDlg::setFieldsEnabled(const int *idArray, int n, bool enabled) {
  for(int i = 0; i < n; i++) {
    GetDlgItem(idArray[i])->EnableWindow(enabled?TRUE:FALSE);
  }
}

#define SET_NUMERICENABLED( enabled) setFieldsEnabled(numTypeFieldTable  , ARRAYSIZE(numTypeFieldTable  ), enabled)
#define SET_COMBOBOXENABLED(enabled) setFieldsEnabled(comboTypeFieldTable, ARRAYSIZE(comboTypeFieldTable), enabled)

void CListFieldAttributeDlg::setFieldType(ListFieldType type) {
  m_fieldType = fieldTypeToComboValue(type);
  if(isNumericType(type)) {
    SET_NUMERICENABLED( true );
    SET_COMBOBOXENABLED(false);
  } else if(type == LFT_STRCOMBO) {
    SET_NUMERICENABLED( false);
    SET_COMBOBOXENABLED(true );
  } else {
    SET_NUMERICENABLED( false);
    SET_COMBOBOXENABLED(false);
  }
}

typedef struct {
  int           m_comboValue;
  ListFieldType m_type;
} ComboTypeElement;

static const ComboTypeElement comboTypeTable[] = {
  0, LFT_SHORT
 ,1, LFT_INT
 ,2, LFT_FLOAT
 ,3, LFT_DOUBLE
 ,4, LFT_STRING
 ,5, LFT_STRCOMBO
 ,6, LFT_BOOL
};

ListFieldType CListFieldAttributeDlg::comboValueToFieldType(int i) { // static
  for(int k = 0; k < ARRAYSIZE(comboTypeTable); k++) {
    if(comboTypeTable[k].m_comboValue == i) {
      return comboTypeTable[k].m_type;
    }
  }
  return LFT_INT;
}

int CListFieldAttributeDlg::fieldTypeToComboValue(ListFieldType type) { // static
  for(int k = 0; k < ARRAYSIZE(comboTypeTable); k++) {
    if(comboTypeTable[k].m_type == type) {
      return comboTypeTable[k].m_comboValue;
    }
  }
  return 1;
}

ListFieldAttribute::ListFieldAttribute() {
  m_header         = _T("New field");
  m_flags          = LFT_INT;
  m_enabled        = true;
  m_width          = 40;
  m_numberInterval = CTableModel::getMaxInterval(LFT_INT);
}

String ListFieldAttribute::toString() const {
  const ListFieldType type = getType();
  String result = format(_T("Header:<%-20s>, Type:%-12s, Enabled:%-5s, Width:%3d")
                        ,m_header.cstr()
                        ,CTableModel::getListFieldTypeName(type).cstr()
                        ,boolToStr(m_enabled)
                        ,m_width
                        );

  if(CTableModel::isNumericType(type)) {

#define IFNUMFLAG(f) if(m_flags&(LF_NUM_##f)) { result += " "; result += #f; }

    IFNUMFLAG(SHOW_ZERO           );
    IFNUMFLAG(HAS_LOWERLIMIT      );
    IFNUMFLAG(LOWERLIMIT_EXCLUSIVE);
    IFNUMFLAG(HAS_UPPERLIMIT      );
    IFNUMFLAG(UPPERLIMIT_EXCLUSIVE);
    if(!LF_ISINTEGERTYPE(type)) {
      result += format(_T(" Decimals:%d"), getDecimalCount());
    }
    if(LF_NUM_HASINTERVAL(m_flags)) {
      result += format(_T(" Interval:(%lg..%lg)"), m_numberInterval.getMin(), m_numberInterval.getMax());
    }
  } else if(type == LFT_STRCOMBO) {
    if(m_flags & LF_STRCOMBO_DROPDOWN) {
      result += _T(", Style:Drop down");
    } else {
      result += _T(", Style:Drop list");
    }
  }
  return result;
};

BOOL CListFieldAttributeDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}
