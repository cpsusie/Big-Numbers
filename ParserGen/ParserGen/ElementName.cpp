#include "stdafx.h"

const TCHAR *ElementName::s_elementName[][2] = {
  _T("symbol "    ), _T("symbols"    )
 ,_T("terminal "  ), _T("terminals"  )
 ,_T("ntIndex  "  ), _T("ntIndices"  )
 ,_T("production "), _T("productions")
 ,_T("state "     ), _T("states"     )
};

String ElementName::createElementCountText(ElementType type, UINT count) { // static
  return format(_T("%u %s"), count, getElementName(type, count > 1));
}
