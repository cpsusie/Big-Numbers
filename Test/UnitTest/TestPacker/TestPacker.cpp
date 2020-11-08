#include "stdafx.h"
#include <Math/Int128.h>
#include <Math/Double80.h>
#include <Math/Double64.h>
#include <ByteMemoryStream.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestPacker {

#include <UnitTestTraits.h>

  TEST_CLASS(TestPacker) {

    static void sendReceive(Packer &dst, const Packer &src) {
      ByteArray a;
      src.write(ByteMemoryOutputStream(a));
      dst.read( ByteMemoryInputStream(a));
    }

  public:

    TEST_METHOD(PackerChar) {
      try {
        Packer s,d;

        for(int i = CHAR_MIN; i <= CHAR_MAX; i++) {
          s << (char)i;
        }
        sendReceive(d,s);
        for(int i = CHAR_MIN; i <= CHAR_MAX; i++) {
          char data;
          d >> data;
          verify(data == i);
        }
      } catch (Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        verify(false);
      }
    }

    TEST_METHOD(PackerUchar) {
      try {
        Packer s,d;

        for(int i = 0; i <= UCHAR_MAX; i++) {
          s << (UCHAR)i;
        }
        sendReceive(d,s);
        for(int i = 0; i <= UCHAR_MAX; i++) {
          UCHAR data;
          d >> data;
          verify(data == i);
        }
      } catch (Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        verify(false);
      }
    }

    TEST_METHOD(PackerShort) {
      try {
        Packer s,d;

        for(int i = SHRT_MIN; i <= SHRT_MAX; i++) {
          s << (short)i;
        }
        sendReceive(d,s);
        for(int i = SHRT_MIN; i <= SHRT_MAX; i++) {
          short data;
          d >> data;
          verify(data == i);
        }
      } catch (Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        verify(false);
      }
    }

    TEST_METHOD(PackerUshort) {
      try {
        Packer s,d;

        for(int i = 0; i <= USHRT_MAX; i++) {
          s << (USHORT)i;
        }
        sendReceive(d,s);
        for(int i = 0; i <= USHRT_MAX; i++) {
          USHORT data;
          d >> data;
          verify(data == i);
        }
      } catch (Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        verify(false);
      }
    }

    TEST_METHOD(PackerInt) {
      try {
        Packer s,d;
        CompactIntArray a;
        INT64 i;
        for(i = SHRT_MIN; i <= SHRT_MAX; i++) {
          a.add((int)i);
        }
        while(isInt(i)) {
          a.add((int)i);
          a.add((int)(-i));
          i = i*3+1;
        }
        for(size_t i = 0; i < a.size(); i++) {
          s << a[i];
        }
        sendReceive(d,s);
        for(size_t i = 0; i < a.size(); i++) {
          int data;
          d >> data;
          verify(data == a[i]);
        }
      } catch (Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        verify(false);
      }
    }

    TEST_METHOD(PackerUint) {
      try {
        Packer s,d;
        CompactUIntArray a;
        UINT64 i;
        for(i = 0; i <= USHRT_MAX; i++) {
          a.add((UINT)i);
        }
        while(isUint(i)) {
          a.add((UINT)i);
          i = i*3+1;
        }
        for(size_t i = 0; i < a.size(); i++) {
          s << a[i];
        }
        sendReceive(d,s);
        for(size_t i = 0; i < a.size(); i++) {
          UINT data;
          d >> data;
          verify(data == a[i]);
        }
      } catch (Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        verify(false);
      }
    }

    TEST_METHOD(PackerInt64) {
      try {
        Packer s,d;
        CompactInt64Array a;
        INT64 i;
        for(i = SHRT_MIN; i <= SHRT_MAX; i++) {
          a.add(i);
        }
        while(i < INT64_MAX/4) {
          a.add(i);
          a.add(-i);
          i = i*3+1;
        }
        a.add(INT64_MAX);
        a.add(INT64_MIN);

        for(size_t i = 0; i < a.size(); i++) {
          s << a[i];
        }
        sendReceive(d,s);
        for(size_t i = 0; i < a.size(); i++) {
          INT64 data;
          d >> data;
          verify(data == a[i]);
        }
      } catch (Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        verify(false);
      }
    }

    TEST_METHOD(PackerUint64) {
      try {
        Packer s,d;
        CompactUInt64Array a;
        UINT64 i;
        for(i = 0; i <= USHRT_MAX; i++) {
          a.add(i);
        }
        while(i < UINT64_MAX/4) {
          a.add(i);
          i = i*3+1;
        }
        a.add(UINT64_MAX);

        for(size_t i = 0; i < a.size(); i++) {
          s << a[i];
        }
        sendReceive(d,s);
        for(size_t i = 0; i < a.size(); i++) {
          UINT64 data;
          d >> data;
          verify(data == a[i]);
        }
      } catch (Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        verify(false);
      }
    }

    TEST_METHOD(PackerInt128) {
      try {
        Packer s,d;
        CompactArray<_int128> a;
        _int128 i;
        for(i = SHRT_MIN; i <= SHRT_MAX; i++) {
          a.add(i);
        }
        const _int128 limit = _I128_MAX/4;
        while(i < limit) {
          a.add(i);
          a.add(-i);
          i = i*3+1;
        }
        a.add(_I128_MAX);
        a.add(_I128_MIN);

        for(size_t i = 0; i < a.size(); i++) {
          s << a[i];
        }
        sendReceive(d,s);
        for(size_t i = 0; i < a.size(); i++) {
          _int128 data;
          d >> data;
          verify(data == a[i]);
        }
      } catch (Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        verify(false);
      }
    }

    TEST_METHOD(PackerUint128) {
      try {
        Packer s,d;
        CompactArray<_uint128> a;
        _uint128 i;
        for(i = 0; i <= USHRT_MAX; i++) {
          a.add(i);
        }
        const _uint128 limit = _UI128_MAX/4;
        while(i < limit) {
          a.add(i);
          i = i*3+1;
        }
        a.add(_UI128_MAX);

        for(size_t i = 0; i < a.size(); i++) {
          s << a[i];
        }
        sendReceive(d,s);
        for(size_t i = 0; i < a.size(); i++) {
          _uint128 data;
          d >> data;
          verify(data == a[i]);
        }
      } catch (Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        verify(false);
      }
    }

    TEST_METHOD(PackerFloat) {
      try {
        Packer s,d;
        CompactFloatArray a;
        float i;
        a.add(FLT_NAN);
        a.add(FLT_PINF);
        a.add(FLT_NINF);
        for(i = SHRT_MIN; i <= SHRT_MAX; i++) {
          a.add(i);
          a.add(-i);
          a.add(1.f/i);
          a.add(-1.f/i);
        }
        while(!isinf(i)) {
          a.add(i);
          a.add(-i);
          i *= 3.154329f;
        }

        for(size_t i = 0; i < a.size(); i++) {
          s << a[i];
        }
        sendReceive(d,s);
        for(size_t i = 0; i < a.size(); i++) {
          float data;
          d >> data;
          if(isnormal(data) || (data == 0)) {
            verify(data == a[i]);
          } else if(isPInfinity(data)) {
            verify(isPInfinity(a[i]));
          } else if(isNInfinity(data)) {
            verify(isNInfinity(a[i]));
          } else if(isnan(data)) {
            verify(isnan(a[i]));
          } else {
            throwException(_T("Unknown float-classification for a[%zu]:%e"),i,data);
          }
        }
      } catch (Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        verify(false);
      }
    }

    TEST_METHOD(PackerDouble) {
      try {
        Packer s,d;
        CompactDoubleArray a;
        double i;
        a.add(DBL_NAN);
        a.add(DBL_PINF);
        a.add(DBL_NINF);
        for(i = SHRT_MIN; i <= SHRT_MAX; i++) {
          a.add(i);
          a.add(-i);
          a.add(1.0/i);
          a.add(-1.0/i);
        }
        while(!isinf(i)) {
          a.add(i);
          a.add(-i);
          i *= 3.3217;
        }

        for(size_t i = 0; i < a.size(); i++) {
          s << a[i];
        }
        sendReceive(d,s);
        for(size_t i = 0; i < a.size(); i++) {
          double data;
          d >> data;
          if(isnormal(data) || (data == 0)) {
            verify(data == a[i]);
          } else if(isPInfinity(data)) {
            verify(isPInfinity(a[i]));
          } else if(isNInfinity(data)) {
            verify(isNInfinity(a[i]));
          } else if(isnan(data)) {
            verify(isnan(a[i]));
          } else {
            throwException(_T("Unknown double-classification for a[%zu]:%le"),i,data);
          }
        }
      } catch (Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        verify(false);
      }
    }

    TEST_METHOD(PackerDouble80) {
      try {
        Packer s,d;
        CompactArray<Double80> a;
        Double80 i;
        a.add(std::numeric_limits<Double80>::quiet_NaN());
        a.add(std::numeric_limits<Double80>::signaling_NaN());
        a.add(DBL80_NAN);
        a.add(DBL80_PINF);
        a.add(DBL80_NINF);
        for(i = SHRT_MIN; i <= SHRT_MAX; i++) {
          a.add(i);
          a.add(-i);
          a.add(Double80::_1/i);
          a.add(-Double80::_1/i);
        }
        while(!isinf(i)) {
          a.add(i);
          a.add(-i);
          i *= 3.3217891;
        }

        for(size_t i = 0; i < a.size(); i++) {
          s << a[i];
        }
        sendReceive(d,s);
        for(size_t i = 0; i < a.size(); i++) {
          Double80 data;
          d >> data;
          if(isnormal(data) || (data == 0)) {
            verify(data == a[i]);
          } else if(isPInfinity(data)) {
            verify(isPInfinity(a[i]));
          } else if(isNInfinity(data)) {
            verify(isNInfinity(a[i]));
          } else if(isnan(data)) {
            verify(isnan(a[i]));
          } else {
            TCHAR tmpstr[100];
            throwException(_T("Unknown Double80-classification for a[%zu]:%s"),i,d80tot(tmpstr,data));
          }
        }
      } catch (Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        verify(false);
      }
    }
  };
}
