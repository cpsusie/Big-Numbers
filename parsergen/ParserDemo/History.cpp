#include "stdafx.h"
#include "history.h"
#include <Registry.h>
#include <Exception.h>

static RegistryKey getKey() {
  return RegistryKey(HKEY_CURRENT_USER, "Software").createOrOpenPath("JGMData\\ParserDemo\\FindHistory");
}

History::History() {
  try {
    RegistryKey key = getKey();

    clear();
    for(int i = 0; i < 15; i++) {
      String str;
      key.getValue(format("s%02d", i), str);
      add(str);
    }    
  } catch(Exception) {
    // ignore
  }
}

History::~History() {
  try {
    RegistryKey key = getKey();
    const int n = min(15, size());
    for(int i = 0; i < n; i++) {
      key.setValue(format("s%02d", i), (*this)[i]);
    }
  } catch(Exception) {
    // ignore
  }
}

bool History::add(const String &s) {
  for(int i = 0; i < size(); i++) {
    if((*this)[i] == s) {
      removeIndex(i);
      break;
    }
  }
  Array<String>::add(0, s);
  return true;
}
