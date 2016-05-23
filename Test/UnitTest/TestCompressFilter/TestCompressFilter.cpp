#include "stdafx.h"
#include <MyUtil.h>
#include <ByteFile.h>
#include <CompressFilter.h>
#include <Random.h>
#include "TestCompressFilter.h"

static ByteArray randomByteArray(unsigned int size) {
  BYTE *buf = new BYTE[size];
  for(unsigned int i = 0; i < size; i++) {
    buf[i] = randInt() % 256;
  }
  ByteArray result;
  result.setData(buf, size);
  delete[] buf;
  return result;
}

static ByteArray niceByteArray(unsigned int size) {
  BYTE *buf = new BYTE[size];
  int k = 0;
  for(unsigned int i = 0, j = 0, dj=1; i < size;) {
    buf[i++] = j % 256;
    j += dj;
    if(j == 0) {
      dj = ++k;
    } else if(j==256) {
      dj = -(++k);
    }
    if(k == 128) {
      k = 0;
    }
  }
  ByteArray result;
  result.setData(buf, size);
  delete[] buf;
  return result;
}

void testCompressFilter() {
  _tprintf(_T("Testing CompressFilter\n"));

  const double startTime = getProcessTime();
  bool ok = true;
  const String fileName = _T("c:\\temp\\compressedData.dat");
  try {
    for(int i = 0; i < 10; i++) {
      ByteArray a = niceByteArray(100000) + randomByteArray(100000);
      a.save(CompressFilter(ByteOutputFile(fileName)));
      const long fileSize = STAT(fileName).st_size;

      ByteArray b;
      b.load(DecompressFilter(ByteInputFile(fileName)));
      if(b != a) {
        throwException(_T("Decompressed ByteArray differs from original ByteArray"));
      }
      _tprintf(_T("Raw size:%8ld. Compressed size:%8ld CompressionRate:%8.2lf%%\n"), a.size(), fileSize, (double)fileSize / a.size() * 100.0);
    }
    unlink(fileName);
  } catch(Exception e) {
    _ftprintf(stderr,_T("Exception:%s\n"),e.what());
    throw;
  }
  _tprintf(_T("CompressFilter  ok!. Timeusage:%.3lf sec\n"), (getProcessTime()-startTime)/1000000);
}
