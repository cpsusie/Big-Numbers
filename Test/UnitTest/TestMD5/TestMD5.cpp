#include "stdafx.h"
#include <MD5.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestMD5 {

#include <UnitTestTraits.h>

  static String MD5String(const char *s) {
    MD5HashCode code;
    String codeString = MD5::getHashCode(code, ByteArray((BYTE*)s, strlen(s))).toString(false);
    return codeString;
  }

#define VERIFYMD5(s, expected) verify(MD5String(s) == _T(expected))

  TEST_CLASS(TesMD5Hash) {
  public:
    TEST_METHOD(Test512) {
      VERIFYMD5(""                                                                                , "d41d8cd98f00b204e9800998ecf8427e");
      VERIFYMD5("a"                                                                               , "0cc175b9c0f1b6a831c399e269772661");
      VERIFYMD5("abc"                                                                             , "900150983cd24fb0d6963f7d28e17f72");
      VERIFYMD5("message digest"                                                                  , "f96b697d7cb7938d525a2f31aaf161d0");
      VERIFYMD5("abcdefghijklmnopqrstuvwxyz"                                                      , "c3fcd3d76192e4007dfb496cca67e13b");
      VERIFYMD5("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"                  , "d174ab98d277d9f5a5611c2c9f419d9f");
      VERIFYMD5("12345678901234567890123456789012345678901234567890123456789012345678901234567890", "57edf4a22be3c955ac49da2e2107b67a");
    }
  };
}
