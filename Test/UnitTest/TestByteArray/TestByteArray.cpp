#include "stdafx.h"
#include <MyUtil.h>
#include <Random.h>
#include <ByteArray.h>
#include <Bytefile.h>

static void testSaveLoad(const ByteArray &a, const TCHAR *fileName) {
  const static TCHAR *objectToTest = _T("Save/Load/ByteFileArray");

  _tprintf(_T("  Testing %s%s"),objectToTest,spaceString(15-_tcslen(objectToTest),_T('.')).cstr());

  a.save(ByteOutputFile(fileName));

  ByteArray loaded;
  loaded.load(ByteInputFile(fileName));


  verify(loaded == a);
  loaded.load(ByteInputFile(fileName));
  verify(loaded == a);

  ByteFileArray bfArray(fileName, 0);
  verify(bfArray.size() == loaded.size());

  for(int i = 0; i < 1000; i++) {
    const int index = randInt(bfArray.size());
    const BYTE bfElement = bfArray[index];
    const BYTE element   = loaded[index];
    verify(bfElement == element);
  }
}


void testByteArray() {
  const static TCHAR *objectToTest = _T("ByteArray");

  _tprintf(_T("Testing %s%s"),objectToTest,spaceString(15-_tcslen(objectToTest),_T('.')).cstr());

  const double startTime = getProcessTime();

  ByteArray a;
  
  BYTE data[200000];
  for(unsigned int i = 0; i < ARRAYSIZE(data); i++) {
    data[i] = i % 256;
  }

  a.setData(data, ARRAYSIZE(data));
  verify(a.size() == ARRAYSIZE(data));

  for(unsigned int i = 0; i < a.size(); i++) {
    verify(a[i] == data[i]);
  }

  ByteArray b = a;
  verify(b.size() == ARRAYSIZE(data));

  for(unsigned int i = 0; i < a.size(); i++) {
    verify(b[i] == data[i]);
  }

  const TCHAR *fileName = _T("c:\\temp\\TestByteArray.tmp");
  testSaveLoad(a, fileName);
  unlink(fileName);

  b = b;

  verify(b.size() == ARRAYSIZE(data));
  for(unsigned int i = 0; i < a.size(); i++) {
    verify(b[i] == data[i]);
  }

  verify(a == b);

  b[0] = 1;

  verify(a != b);

  a = b;

  verify(a == b);

  a += b;

  ByteArray c = a;

  verify(a.size() == 2*b.size());

  a = b;

  verify(a == b);

  a += a;

  verify(a == c);

  a = b + b;

  verify(a == c);

  c.clear();
  verify(c.size() == 0);
  c.clear();
  verify(c.size() == 0);

  try {
    BYTE byte = c[0];
    verify(false);
  } catch(Exception e) {
    // ignore
  }
  _tprintf(_T("%s ok!\n"),objectToTest);
}
