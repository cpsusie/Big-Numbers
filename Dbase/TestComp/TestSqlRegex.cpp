#include "stdafx.h"
#include "SqlRegex.h"

#ifdef _DEBUG
void SqlRegex::testSqlRegex() {
  for(;;) {
    String pattern = inputString(_T("Enter pattern:"));
    SqlRegex reg(pattern);
    reg.dump();
    for(;;) {
      String text = inputString(_T("Enter text (terminate with empty line):"));
      if(text.length() == 0) break;
      if(reg.match(text)) {
        _tprintf(_T("Match!\n"));
      } else {
        _tprintf(_T("No match!\n"));
      }

    }
  }
}
#endif // _DEBUG