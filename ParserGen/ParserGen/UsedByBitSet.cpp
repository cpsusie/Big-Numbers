#include "stdafx.h"
#include "UsedByBitSet.h"

String UsedByBitSet::toString(const AbstractSymbolNameContainer *nameContainer) const {
  const UINT n = (UINT)size();
  String elementString;
  switch(getElementType()) {
  case ETYPE_SYMBOL:
  case ETYPE_TERM:
    if((nameContainer != nullptr) && (n <= 3)) {
      elementString = nameContainer->symbolSetToString(*this);
    }
    break;
  case ETYPE_NTINDEX:
    if((nameContainer != nullptr) && (n <= 3)) {
      elementString = nameContainer->NTIndexSetToString(*this);
    }
    break;
  }
  if(elementString.length() == 0) {
    elementString = toRangeString(SizeTStringifier(), _T(","), BT_BRACKETS);
  }
  return format(_T("Used by %s %s")
               ,ElementName::createElementCountText(m_type, n).cstr()
               ,elementString.cstr());
}
