#include "stdafx.h"
#include "FindHistory.h"
#include <Registry.h>

static RegistryKey getKey() {
  return Settings::getSubKey(_T("FindHistory"));
}

FindHistory::FindHistory() {
  try {
    RegistryKey key = getKey();
    clear();
    for(int i = 0; i < 15; i++) {
      String value;
      key.getValue(format(_T("s%02d"), i), value);
      add(value);
    }
  } catch(Exception) {
    // ignore
  }
}

FindHistory::~FindHistory() {
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

bool FindHistory::add(const String &s) {
  for(size_t i = 0; i < size(); i++) {
    if((*this)[i] == s) {
      removeIndex(i);
      break;
    }
  }
  StringArray::add(0, s);
  return true;
}
