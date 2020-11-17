#include "pch.h"

const TCHAR *String::s_bracketChars[5] = {
  _T("  ") // BT_NOBRACKET         No enclosing brackets
 ,_T("()") // BT_ROUNDBRACKETS     (...)
 ,_T("{}") // BT_BRACES            {...}
 ,_T("[]") // BT_BRACKETS          [...]
 ,_T("<>") // BT_CHEVRONS          <...>
};
