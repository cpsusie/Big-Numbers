#include "stdafx.h"
#include <SHA256.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestSHA256 {		

#include <UnitTestTraits.h>

  TEST_CLASS(TesSHA256) {

    void TestHashCodes(const TCHAR *method, const char **inputLines, const char **expectedOutput, size_t n) {
      for(int i = 0; i < n; i++) {
        const String line = inputLines[i];
        SHA256HashCode code;
        const String hashCode = SHA256().getHashCode(code, line).toString(false);
        String expected = expectedOutput[i];
        if(hashCode != expected) {
          OUTPUT(_T("Error in %s. line:\"%s\". Gives hashcode:\"%s\". Expected \"%s\"."), method, line.cstr(), hashCode.cstr(), expected.cstr());
          verify(false);
        }
      }
    }

#define DOTEST() TestHashCodes(__TFUNCTION__, input, expectedOutput, ARRAYSIZE(input))

    public:

    TEST_METHOD(Test512) {
      const char *input[] = {
        "11223344555577889911223344555577889911223344555577889911223344555577889911223344555577889911223344555577812121212494949494121212"
      };
      const char *expectedOutput[] = {
        "65cf88ca7e7ed71474116f4695c916691481eda44b377e630be4a21cc06ef924"
      };
      DOTEST();
    } // Test512

    TEST_METHOD(TestEmpty) {
      const char *input[] = {
        ""
      };
      const char *expectedOutput[] = {
        "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"
      };
      DOTEST();
    } // TestEmpty

    TEST_METHOD(TestLarge) {
      const char *input[] = {
        "3bf0e8c1157b2be9a4c8e57bebb3ab44be2466084976b325a5953dd7418ee284e62fcdfae01a5ef57d16d645b6e6c24f69b36e12a11f5829acc7ea5082dbd69de25a7b2ba78aafe6ec37036e7f33537a31935627a29a9011866275a6b1eedfb3036dc91104b374617f565d1a6b9c9abab9e2e062fc061f37c8bb8c7138ffae3097ef5c0d1754fd4b13eac8e805a31c8c6657554ed2c60054361e7c6180ec17f4b6b439fe626ec4bf79a02885af9318b79"
      };
      const char *expectedOutput[] = {
        "c7f2aa0438d7933b938c9940fcf67e8d3d37f1c0a34a9aa4852964fa6db4dab6"
      };
      DOTEST();
    } // TestLarge

    TEST_METHOD(TestLeadingZero) {
      const char *input[] = {
        "de188941a3375d3a8a061e67576e926d"
      };
      const char *expectedOutput[] = {
        "067c531269735ca7f541fdaca8f0dc76305d3cada140f89372a410fe5eff6e4d"
      };
      DOTEST();
    } // TestLeadingZero

    TEST_METHOD(TestLess512) {
      const char *input[] = {
        "e5"
      };
      const char *expectedOutput[] = {
        "ab61ba11a38b007ff98baa3ab20e2a584e15269fd428db3c857e2a2d568b5725"
      };
      DOTEST();
    } // TestLess512

    TEST_METHOD(TestMore512) {
      const char *input[] = {
        "11223344555577889911223344555577889911223344555577889911223344555577889911223344555577889911223344555577812121212494949494121212129685746375869786abe4a6f30200"
      };
      const char *expectedOutput[] = {
        "56435d749581c4ff41a33cbf39138cb4b9b8184d74627d36ebb09332823fa039"
      };
      DOTEST();
    } // TestMore512

    TEST_METHOD(TestSample) {
      const char *input[] = {
        "e5"
       ,"4719"
       ,"93006a"
       ,"889468b1"
      };
      const char *expectedOutput[] = {
        "ab61ba11a38b007ff98baa3ab20e2a584e15269fd428db3c857e2a2d568b5725"
       ,"60acb27227ef97fb8ad343bf910cf5f451d8af50498e4b7d293bbd97e6c50c9b"
       ,"1dc0b9dd1a9633d9c967c18ca23a68457850bd5e67194d3fc977931d50f455ea"
       ,"855b2244b875ed9ae089fb10d84c85257f30c65ea1325c2f76727a582ba4c801"
      };
      DOTEST();
    } // TestSample

    TEST_METHOD(TestVectors) {
      const char *input[] = {
        "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"
       ,"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"
       ,"cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0"
       ,"88b4b6f3b3c7e6a8519477a71c605fcfcdf636718d9bb55363cbd15a0eac58ca"
       ,"5d52a73def39b2e4eb98725300b36c7f26100152b25633301e61cfd2e3c129ce"
       ,"02eaeaeba71b64a97cc41c83625e497e64d991e0966773131b143689e50bd87d"
      };
      const char *expectedOutput[] = {
        "4f8b42c22dd3729b519ba6f68d2da7cc5b2d606d05daed5ad5128cc03e6c6358"
       ,"5df6e0e2761359d30a8275058e299fcc0381534545f55cf43e41983f5d4c9456"
       ,"80d1189477563e1b5206b2749f1afe4807e5705e8bd77887a60187a712156688"
       ,"6d001b0645dd880f4f2d3375ddae9e5198f3890e33207c165e3c738b5c972417"
       ,"3671a0b14821ae99ba2d2908008d981eb99f1d56e29204696e65b5c69a0f108c"
       ,"f84bef74588a23683db45304c4fa973b09a6045b46a0be5eb0b28c4dbb2a21be"
      };
      DOTEST();
    } // TestVectors

    TEST_METHOD(TestMassive) {
      const char *input[] = {
        "3bf0e8c1157b2be9a4c8e57bebb3ab44be2466084976b325a5953dd7418ee284"
        "a1f0e8c1157b2be9a4c8e57bebb3ab44be2466084976b325a5953dd7418ee284"
        "bef0e8c1157b2be9a4c8e57bebb3ab44be2466084976b325a5953dd7418ee284"
        "a1f0e8c1157b2be9a4c8e57bebb3ab44be2466084976b325a5953dd7418ee284"
        "003bf0e8c1157b2be9a4c8e57bebb3ab44be2466084976b325a5953dd7418ee2"
        "84a1f0e8c1157b2be9a4c8e57bebb3ab44be2466084976b325a5953dd7418ee2"
        "84bef0e8c1157b2be9a4c8e57bebb3ab44be2466084976b325a5953dd7418ee2"
        "84a1f0e8c1157b2be9a4c8e57bebb3ab44be2466084976b325a5953dd7418ee2"
        "84003bf0e8c1157b2be9a4c8e57bebb3ab44be2466084976b325a5953dd7418e"
        "e284a1f0e8c1157b2be9a4c8e57bebb3ab44be2466084976b325a5953dd7418e"
        "e284bef0e8c1157b2be9a4c8e57bebb3ab44be2466084976b325a5953dd7418e"
        "e284a1f0e8c1157b2be9a4c8e57bebb3ab44be2466084976b325a5953dd7418e"
        "e284003bf0e8c1157b2be9a4c8e57bebb3ab44be2466084976b325a5953dd741"
        "8ee284a1f0e8c1157b2be9a4c8e57bebb3ab44be2466084976b325a5953dd741"
        "8ee284bef0e8c1157b2be9a4c8e57bebb3ab44be2466084976b325a5953dd741"
        "8ee284a1f0e8c1157b2be9a4c8e57bebb3ab44be2466084976b325a5953dd741"
        "8ee284003bf0e8c1157b2be9a4c8e57bebb3ab44be2466084976b325a5953dd7"
        "418ee284a1f0e8c1157b2be9a4c8e57bebb3ab44be2466084976b325a5953dd7"
        "418ee284bef0e8c1157b2be9a4c8e57bebb3ab44be2466084976b325a5953dd7"
        "418ee284a1f0e8c1157b2be9a4c8e57bebb3ab44be2466084976b325a5953dd7"
        "418ee284013bf0e8c1157b2be9a4c8e57bebb3ab44be2466084976e284a1f0e8"
        "c1157b2be9a4c8e57bebb3ab44be2466084976b325a5953dd7418ee284bef0e8"
        "c1157b2be9a4c8e57bebb3ab44be2466084976b325a5953dd7418ee284a1f0e8"
        "c1157b2be9a4c8e57bebb3ab44be2466084976b325a5953dd7418ee284003bf0"
        "e8c1157b2be9a4c8e57bebb3ab44be2466084976b325a5953dd7418ee284a1f0"
        "e8c1157b2be9a4c8e57bebb3ab44be2466084976b325a5953dd7418ee284bef0"
        "e8c1157b2be9a4c8e57bebb3ab44be2466084976b325a5953dd7418ee284a1f0"
        "e8c1157b2be9a4c8e57bebb3ab44be2466084976b325a5953dd7418ee284013b"
        "f0e8c1157b2be9a4c8e57bebb3ab44be2466084976a2a2a2a2a2a2a2a2a2a280"
        "ffefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefef"
        "efefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefef"
        "efefefefefefefefefefefefefefefefee939393939393939393939392101010"
      };
      const char *expectedOutput[] = {
        "8a5f820a95f645158839bfc6582871a0c9cd900c9a0a645a01ddfee778a8f3d2"
      };
      DOTEST();
    } // TestMassive
  };
}
