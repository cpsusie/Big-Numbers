#include "stdafx.h"
#include <StringPool.h>
#include <CompactArray.h>
#include <ByteFile.h>

static String generateTestString(int i) {
  return format(_T("Dette er en teststring med nr %08x"), i);
}

static int strCompare(const TCHAR * const &s1, const TCHAR * const &s2) {
  return _tcscmp(s1,s2);
}

void testStringPool() {
  const static TCHAR *objectToTest = _T("StringPool");

  _tprintf(_T("Testing %s%s"),objectToTest,spaceString(15-_tcslen(objectToTest),'.').cstr());

  StringPool sp;
  
  CompactIntArray offsets;
  for(int i = 0; i < 10000; i++) {
    offsets.add(sp.addString(generateTestString(i).cstr()));
  }
  for(int i = 0; i < offsets.size(); i++) {
    const TCHAR *str = sp.getString(offsets[i]);
    verify(generateTestString(i) == str);
  }
  sp.clearIndex();
  verify(!sp.isEmpty());
  verify(sp.getIndexCapacity() == 0);
  verify(sp.getTextCapacity() >= (unsigned int)offsets.last());

  for(int i = 0; i < offsets.size(); i++) {
    const TCHAR *str = sp.getString(offsets[i]);
    verify(generateTestString(i) == str);
  }

  CompactIntArray offsets1;
  for(int i = 0; i < 10000; i++) {
    offsets1.add(sp.addString(generateTestString(i).cstr()));
  }
  verify(offsets1 == offsets);
  
  StringPool sp1(sp);
  verify(sp1 == sp);

  sp1.clear();
  verify(sp1.isEmpty());

  sp1 = sp;
  verify(sp1 == sp);

  const TCHAR *fileName = _T("c:\\temp\\TestStringPool.tmp");
  sp.save(ByteOutputFile(fileName));

  StringPool loaded;
  loaded.load(ByteInputFile(fileName));

  verify(loaded == sp);
  loaded.load(ByteInputFile(fileName));
  verify(loaded == sp);

  unlink(fileName);

  CompactArray<const TCHAR*> str1Array, str2Array;
  for(Iterator<const TCHAR*> it = loaded.getIterator(); it.hasNext();) {
    const TCHAR *s = it.next();
    str1Array.add(s);
  }

  for(int i = 0; i < offsets.size(); i++) {
    str2Array.add(loaded.getString(offsets[i]));
  }
  str1Array.sort(strCompare);
  str2Array.sort(strCompare);
  verify(str1Array == str2Array);

  _tprintf(_T("%s ok!\n"),objectToTest);
}
