#include "stdafx.h"
#include <InputValue.h>
#include "SqlRegex.h"

#ifdef _DEBUG
void SqlRegex::testSqlRegex() {
  for(;;) {
    String pattern = inputValue<String>(_T("Enter pattern:"));
    SqlRegex reg(pattern);
    reg.dump();
    for(;;) {
      String text = inputValue<String>(_T("Enter text (terminate with empty line):"));
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