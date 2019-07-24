#include "stdafx.h"
#include <Random.h>
#include <ByteArray.h>
#include <Bytefile.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestByteArray {

#include <UnitTestTraits.h>

  TEST_CLASS(TestByteArray)	{
	public:
		
    static void testSaveLoad(const ByteArray &a, const String &fileName) {
      a.save(ByteOutputFile(fileName));

      ByteArray loaded;
      loaded.load(ByteInputFile(fileName));

      verify(loaded == a);
      loaded.load(ByteInputFile(fileName));
      verify(loaded == a);

      ByteFileArray bfArray(fileName, 0);
      verify(bfArray.size() == loaded.size());

      for(int i = 0; i < 1000; i++) {
        const size_t index     = randSizet(bfArray.size());
        const BYTE   bfElement = bfArray[index];
        const BYTE   element   = loaded[index];
        verify(bfElement == element);
      }
    }

    TEST_METHOD(ByteArrayPrimitiveOperations) {
      const double startTime = getProcessTime();

      ByteArray a;

      BYTE data[200000];
      for(size_t i = 0; i < ARRAYSIZE(data); i++) {
        data[i] = i % 256;
      }

      a.setData(data, ARRAYSIZE(data));
      verify(a.size() == ARRAYSIZE(data));

      for(size_t i = 0; i < a.size(); i++) {
        verify(a[i] == data[i]);
      }

      ByteArray b = a;
      verify(b.size() == ARRAYSIZE(data));

      for(size_t i = 0; i < a.size(); i++) {
        verify(b[i] == data[i]);
      }

      const String fileName = getTestFileName(__TFUNCTION__);
      testSaveLoad(a, fileName);
      unlink(fileName);

      b = b;

      verify(b.size() == ARRAYSIZE(data));
      for(size_t i = 0; i < a.size(); i++) {
        verify(b[i] == data[i]);
      }

      verify(a == b);

      b[0] = 1;

      verify(a != b);

      a = b;

      verify(a == b);

      a += b;

      ByteArray c = a;

      verify(a.size() == 2 * b.size());

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
      }  catch(Exception e) {
        // ignore
      }
    }
  };
}
