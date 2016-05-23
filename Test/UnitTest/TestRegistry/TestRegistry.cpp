#include "stdafx.h"
#include <MyUtil.h>
#include <Registry.h>
#include <MyAssert.h>

static const TCHAR *regEntry = _T("Software\\TestRegistry");

class InitRegistry {
public:
  InitRegistry();
 ~InitRegistry();
};

InitRegistry::InitRegistry() {
  try {
    RegistryKey key0(HKEY_CURRENT_USER,"Software");
    RegistryKey key1 = key0.createOrOpenPath(_T("TestRegistry\\Test"));
    for(int i = 0; i < 3; i++) {
      for(int j = 0; j < 3; j++) {
        key1.createOrOpenPath(format(_T("subdirI%02d\\subdirJ%02d"),i,j));
      }
    }
    _tprintf(_T("key1.fullName:<%s>\n"),key1.getName().cstr());
  } catch(Exception e) {
    _tprintf(_T("Exception in InitRegistry:%s\n"),e.what());
    abort();
  }
}

InitRegistry::~InitRegistry() {
  RegistryKey key0(HKEY_CURRENT_USER,_T("Software"));
  try {
    key0.deleteKey(_T("TestRegistry"));
    verify(false);
  } catch(Exception e) {
    // ignore
  }
  try {
    key0.deleteKeyAndSubkeys(_T("TestRegistry"));
  } catch(Exception e) {
    _tprintf(_T("Exception in ~InitRegistry:%s\n"),e.what()); fflush(stdout);
    abort();
  }
}

static InitRegistry dummy;

class RegTest : public Array<RegistryValue> {
public:
  RegTest();
  ~RegTest();
  void load();
  void save();
  void listValues() const;
  void removeValues();
};

RegTest::RegTest() {
  load();
}

RegTest::~RegTest() {
  save();
}

void RegTest::load() {
  try {
    RegistryKey thisKey(HKEY_CURRENT_USER,format(_T("%s\\Test"),regEntry));
    clear();
    for(Iterator<RegistryValue> it = thisKey.getValueIterator(); it.hasNext();) {
      add(it.next());
    }    
  } catch(Exception e) {
    _tprintf(_T("Exception:%s\n"),e.what());
  }
}

void RegTest::save() {
  try {
    RegistryKey key(HKEY_CURRENT_USER,regEntry);
    RegistryKey thisKey = key.createOrOpenKey(_T("Test"));

    thisKey.deleteValues();
    for(int i = 0; i < size(); i++) {
      thisKey.setValue((*this)[i]);
    }
  } catch(Exception e) {
    _tprintf(_T("Exception:%s\n"),e.what());
  }
}

void RegTest::listValues() const {
  RegistryKey thisKey(HKEY_CURRENT_USER,format(_T("%s\\Test"),regEntry));
  for(Iterator<RegistryValue> it = thisKey.getValueIterator(); it.hasNext();) {
    const RegistryValue &value = it.next();
    _tprintf(_T("type:%2d name=%-20s. value=%s\n"),value.getType(), value.getName().cstr(), value.toString().cstr());
  }
}

void RegTest::removeValues() {
  RegistryKey thisKey(HKEY_CURRENT_USER,format(_T("%s\\Test"),regEntry));
  for(Iterator<RegistryValue> it = thisKey.getValueIterator(); it.hasNext();) {
    const RegistryValue value = it.next();
    _tprintf(_T("removing value %-20s\n"),value.getName().cstr());
    it.remove();
  }
}

static Array<String> registryErrors;

void traverseRegistryKey(RegistryKey &key, int level=0) {
  _tprintf(_T("%*.*sKey:%s\n"),level,level,_T(""),key.getName().cstr());
  bool headerDone = false;
  for(Iterator<RegistryValue> itv = key.getValueIterator(); itv.hasNext();) {
    if(!headerDone) {
      _tprintf(_T("  %*.*s%-20s %-12.12s %s\n"),level,level,_T(""),_T("Name"),_T("Type"),_T("Data"));
      headerDone = true;
    }
    const RegistryValue &value = itv.next();
    _tprintf(_T("  %*.*s%-20.20s %-12.12s %-70.70s\n"),level,level,_T(""),value.getName().cstr(),value.getTypeString().cstr(),value.toString().cstr());
  }
  for(Iterator<String> it = key.getSubKeyIterator(); it.hasNext();) {
    String child = it.next();
    try {
      traverseRegistryKey(key.openKey(child),level+2);
    } catch(Exception e) {
      registryErrors.add(format(_T("%s:%s\\%s"),e.what(),key.getName().cstr(),child.cstr()));
    } 
  }
}

static void usage() {
  _ftprintf(stderr,_T("Usage:testregistry [root]\n")
                   _T("     root = {root, currentuser, localmachine, config, performance, dyndata}\n")
         );
  exit(-1);
}

int _tmain(int argc, TCHAR **argv) {
  argv++;
  if(!*argv) {
    unsigned char bytes[] = { 0,1,2,3,4,5,6,7,8,9,10,0,0};
    RegistryValue binaryValue(_T("binaryFelt"), bytes, ARRAYSIZE(bytes));
    StringArray strings;
    for(int i = 0; i < 10; i++) {
      strings.add(format(_T("string %d"),i));
    }
    RegistryValue multiStringValue(_T("multiStringFelt"), strings);

    RegTest test;
    test.add(RegistryValue(_T("stringFelt"), _T("fisk")));
    test.add(RegistryValue(_T("longFelt")  , 0x12345678));
    test.add(binaryValue);
    test.add(multiStringValue);
    test.save();
    test.load();
    test.listValues();
    test.removeValues();
    _tprintf(_T("RegTest:<%s>\n"),test.toString().cstr());
  } else {
    TCHAR *cp = *argv;
    HKEY root;
    if(_tcsicmp(cp,_T("root")) == 0) {
      root = HKEY_CLASSES_ROOT;
    } else if(_tcsicmp(cp,_T("currentuser")) == 0) {
      root = HKEY_CURRENT_USER;
    } else if(_tcsicmp(cp,_T("localmachine")) == 0) {
      root = HKEY_LOCAL_MACHINE;
    } else if(_tcsicmp(cp,_T("config")) == 0) {
      root = HKEY_CURRENT_CONFIG;
    } else if(_tcsicmp(cp,_T("performance")) == 0) {
      root = HKEY_PERFORMANCE_DATA;
    } else if(_tcsicmp(cp,_T("dyndata")) == 0) {
      root = HKEY_DYN_DATA;
    } else {
      usage();
    }
    traverseRegistryKey(RegistryKey(root));
    for(int i = 0; i < registryErrors.size(); i++) {
      _tprintf(_T("Exception:%s\n"),registryErrors[i].cstr());
    }
  }
  return 0;
}
