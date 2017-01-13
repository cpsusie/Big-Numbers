#include "stdafx.h"
#include <Limits.h>
#include <float.h>
#include <MFCUtil/EditListNumericEditor.h>
#include <MFCUtil/EditListStringEditor.h>
#include <MFCUtil/EditListBooleanEditor.h>
#include <MFCUtil/EditListStringcombo.h>

CTableModel::~CTableModel() {
  for(size_t i = 0; i < m_editorArray.size(); i++) {
    CWnd *e = m_editorArray[i];
    if(e) {
      e->DestroyWindow();
      delete e;
    }
  }
}

void CTableModel::saveEditor(UINT column, CWnd *editor) {
  if(column >= m_editorArray.size()) {
    for(size_t i = m_editorArray.size(); i <= column; i++) {
      m_editorArray.add(NULL);
    }
  }
  if(m_editorArray[column] != NULL) {
    CWnd *e = m_editorArray[column];
    e->DestroyWindow();
    delete e;
  }
  m_editorArray[column] = editor;
}

CWnd *CTableModel::getCachedEditor(UINT column) {
  return (column < m_editorArray.size()) ? m_editorArray[column] : NULL;
}

CWnd *CTableModel::getEditor(CWnd *listCtrl, UINT column) {
  if(hasCachedEditor(column)) {
    return getCachedEditor(column);
  }
  CWnd *e = createEditor(listCtrl, column);
  saveEditor(column, e);
  return e;
}

CWnd *CTableModel::createEditor(CWnd *listCtrl, UINT column) {
  const UINT flags = getFieldFlags(column);
  switch(LF_GETTYPE(flags)) {
  case LFT_SHORT   :
  case LFT_INT     :
  case LFT_FLOAT   :
  case LFT_DOUBLE  :
    { CEditListNumericEditor *e = new CEditListNumericEditor;
      DoubleInterval interval, *intervalp = NULL;
      if(LF_NUM_HASINTERVAL(flags)) {
        interval = getLegalInterval(column);
        intervalp = &interval;
      }
      if(!e->Create(listCtrl, MAKE_EDITLIST_CONTROL_ID(column), flags, intervalp)) {
        throwException(_T("Cannot create CEditListNumericEditor for column %d"), column);
      }
      return e;
    }
  case LFT_STRING  :
    { CEditListStringEditor *e = new CEditListStringEditor;
      if(!e->Create(listCtrl, MAKE_EDITLIST_CONTROL_ID(column))) {
        throwException(_T("Cannot create CEditListStringEditor for column %d"), column);
      };
      return e;
    }
  case LFT_STRCOMBO:
    { CEditListStringCombo *e = new CEditListStringCombo;
      if(!e->Create(listCtrl, MAKE_EDITLIST_CONTROL_ID(column), getStrComboStringArray(column), flags)) {
        throwException(_T("Cannot create CEditListStringCombo for column %d"), column);
      };
      return e;
    }
  case LFT_BOOL    :
    { CEditListBooleanEditor *b = new CEditListBooleanEditor();
      if(!b->Create(listCtrl, MAKE_EDITLIST_CONTROL_ID(column))) {
        throwException(_T("Cannot create CEditListBooleanEditor for column %d"), column);
      }
      return b;
    }
  default          :
    throwException(_T("Unknown editorType for column %d (=%d)"), column, LF_GETTYPE(flags));
    return NULL;
  }
}

bool CTableModel::isNumericType(ListFieldType type) { // static
  switch(type) {
  case LFT_SHORT :
  case LFT_INT   :
  case LFT_FLOAT :
  case LFT_DOUBLE:
    return true;
  default        :
    return false;
  }
}

#define CASETYPE(t) case t: return #t

String CTableModel::getListFieldTypeName(ListFieldType type) { // static
  switch(type) {
  CASETYPE(LFT_SHORT   );
  CASETYPE(LFT_INT     );
  CASETYPE(LFT_FLOAT   );
  CASETYPE(LFT_DOUBLE  );
  CASETYPE(LFT_STRING  );
  CASETYPE(LFT_STRCOMBO);
  CASETYPE(LFT_BOOL    );
  default: throwInvalidArgumentException(__TFUNCTION__, _T("type=%d"), type);
           return _T("?");
  }
}

DoubleInterval CTableModel::getMaxInterval(ListFieldType type) { // static
  switch(type) {
  case LFT_SHORT : return DoubleInterval(SHRT_MIN, SHRT_MAX);
  case LFT_INT   : return DoubleInterval(INT_MIN , INT_MAX );
  case LFT_FLOAT : return DoubleInterval(-FLT_MAX, FLT_MAX );
  case LFT_DOUBLE: return DoubleInterval(-DBL_MAX, DBL_MAX );
  default: throwInvalidArgumentException(__TFUNCTION__, _T("type=%s"), getListFieldTypeName(type).cstr());
           return DoubleInterval(0,1);
  }
}
