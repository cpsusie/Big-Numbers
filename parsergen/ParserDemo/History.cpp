#include "stdafx.h"
#include "history.h"
#include <Registry.h>

static RegistryKey getKey() {
  return RegistryKey(HKEY_CURRENT_USER, _T("Software")).createOrOpenPath(_T("JGMData\\ParserDemo\\FindHistory"));
}

History::History() {
  try {
    RegistryKey key = getKey();

    clear();
    for(int i = 0; i < 15; i++) {
      String str;
      key.getValue(format(_T("s%02d"), i), str);
      add(str);
    }    
  } catch(Exception) {
    // ignore
  }
}

History::~History() {
  try {
    RegistryKey key = getKey();
    const int n = min(15, (int)size());
    for(int i = 0; i < n; i++) {
      key.setValue(format(_T("s%02d"), i), (*this)[i]);
    }
  } catch(Exception) {
    // ignore
  }
}

bool History::add(const String &s) {
  for(size_t i = 0; i < size(); i++) {
    if((*this)[i] == s) {
      removeIndex(i);
      break;
    }
  }
  Array<String>::add(0, s);
  return true;
}
