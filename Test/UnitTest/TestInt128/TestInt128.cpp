#include "stdafx.h"
#include <limits.h>
#include <CompactLineArray.h>
#include <StrStream.h>
#include <Math/Int128.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace std;

namespace TestInt128 {

#include <UnitTestTraits.h>

  TEST_CLASS(TestInt128) {
  public:

    TEST_METHOD(Int128Comparators) {
      int              minI32    = _I32_MIN;
      int              maxI32    = _I32_MAX;
      UINT             maxUI32   = _UI32_MAX;
      INT64            minI64    = _I64_MIN;
      INT64            maxI64    = _I64_MAX;
      UINT64           maxUI64   = _UI64_MAX;
      _int128          minI128   = _I128_MIN;
      _int128          maxI128   = _I128_MAX;
      _uint128         maxUI128  = _UI128_MAX;
      INT64            i64Zero   = 0;
      UINT64           ui64Zero  = 0;
      _int128          i128Zero  = 0;
      _uint128         ui128Zero = 0;

      _int128 i1 = minI32;
      verify(i1 == minI32);
      _int128 i2 = maxI32;
      verify(i2 == maxI32);

      _int128 i3 = minI64;
      verify(i3 == minI64);
      _int128 i4 = maxI64;
      verify(i4 == maxI64);

      _int128 i5 = minI64;
      verify(i5 == minI64);
      i5--;
      verify(i5 <  minI64);
      i5++;
      verify(i5 == minI64);
      verify(i5 <= minI64);
      verify(i5 >= minI64);
      i5++;
      verify(i5 >  minI64);

      _int128 i6 = maxI64;
      verify(i6 == maxI64);
      i6++;
      verify(i6 >  maxI64);
      i6--;
      verify(i6 == maxI64);
      verify(i6 <= maxI64);
      verify(i6 >= maxI64);
      i6--;
      verify(i6 < maxI64);

      _uint128 ui7 = maxUI64;
      verify(ui7 == maxUI64);
      ui7++;
      verify(ui7 >  maxUI64);
      ui7--;
      verify(ui7 == maxUI64);
      verify(ui7 <= maxUI64);
      verify(ui7 >= maxUI64);
      ui7--;
      verify(ui7 <  maxUI64);

      _int128 i8 =  0;
      verify(i8 ==  0);
      verify(i8 >=  0);
      verify(i8 <=  0);
      i8--;
      verify(i8 <   0);
      verify(i8 == -1);
      verify(i8 >= -1);
      verify(i8 <= -1);
      i8++;
      verify(i8 ==  0);
      verify(i8 >=  0);
      verify(i8 <=  0);

      i8++;
      verify(i8 >   0);
      verify(i8 ==  1);
      verify(i8 >=  1);
      verify(i8 <=  1);

      _int128 i9 =  maxI128;
      verify(i9 ==  maxI128);
      i9--;
      verify(i9 <   maxI128);
      verify(i9 <=  maxI128);
      i9++;
      verify(i9 ==  maxI128);
      verify(i9 <=  maxI128);
      verify(i9 >=  maxI128);
      i9++;
      verify(i9 ==  minI128);
      verify(i9 <   0);
      i9--;
      verify(i9 ==  maxI128);

      _int128 i10 = minI128;
      verify(i10 == minI128);
      i10++;
      verify(i10 >  minI128);
      verify(i10 >= minI128);
      i10--;
      verify(i10 == minI128);
      verify(i10 <= minI128);
      verify(i10 >= minI128);
      i10--;
      verify(i10 == maxI128);
      verify(i10 >  0);
      i10++;
      verify(i10 == minI128);

      _uint128 ui11 = maxUI128;
      verify(ui11 == maxUI128);
      ui11--;
      verify(ui11 <  maxUI128);
      verify(ui11 <= maxUI128);
      ui11++;
      verify(ui11 == maxUI128);
      verify(ui11 <= maxUI128);
      verify(ui11 >= maxUI128);
      ui11++;
      verify(ui11 == 0);
      verify(ui11 <= 0);
      verify(ui11 >= 0);
      ui11--;
      verify(ui11 == maxUI128);

      _uint128 ui12 = ui11--; // post decrement
      verify(ui12 == ui11 + 1);
      ui12 = ui11;
      _uint128 ui13 = --ui11; // pre-decrement
      verify((ui13 == ui12 - 1) && (ui13 == ui11));

      _uint128 ui14 = ui11++; // post increment;
      verify(ui14 == ui11 - 1);
      ui14 = ui11;
      _uint128 ui15 = ++ui11; // pre-increment
      verify((ui15 == ui14 + 1) && (ui15 == ui11));

      _int128 i15 = maxUI64;
      i15++;
      _int128 ei15 = _strtoi128("0x10000000000000000", NULL, 0);
      verify(i15 == ei15);
      i15--;
      verify(i15 == maxUI64);

    } // Int128Comparators

    TEST_METHOD(TestMultiplication) {
      _uint128 x(0x1111111122222222);
      _uint128 y(0x3333333344444444);
      _uint128 z = x * y;
      const _uint128 expected1(0x0369d036a1907f6e, 0x530eca863b2a1908);

      verify(z == expected1);

      x = _uint128(0x123, 0xffffffffffffffff);
      y = 0x000fffffffffffff;
      z = x * y;
      const _uint128 expected2(0x123ffffffffffedb, 0xfff0000000000001);
      verify(z == expected2);

    }
    /*
        TEST_METHOD(TestShortDivisor) {
          _uint128 x(0);
          for (int i = 1; i < 128; i++) {
            int expo2;
            unsigned int q = ShortDivisor::getFirst32(x, expo2);
            x <<= 1;
            x |= (i & 1);
          }
        }
    */
    TEST_METHOD(TestDivision1) {
      _uint128 x(0x00000000ffffffff);
      _uint128 y(0x12345);
      const _uint128 expectedQ(0xE100);
      _uint128 d = x / y;
      verify(d == expectedQ);

      const _uint128 expectedR(0x5AFF);
      _uint128 r = x % y;
      verify(r == expectedR);
    }

    TEST_METHOD(TestDivision2) {
      _uint128 x(0x00ffffffffffffff);
      _uint128 y(0x12345);

      const _uint128 expectedQ(0xE1004FFB1C);
      _uint128 d = x / y;
      verify(d == expectedQ);

      const _uint128 expectedR(0x7D73);
      _uint128 r = x % y;
      verify(r == expectedR);
    }

    TEST_METHOD(TestUnsignedDivision2a) {
      _uint128 xmask = 1;
      for(UINT xbits = 1; xbits <= 128; xbits++, xmask = (xmask<<1)|1) {
        for(int i = 0; i < 30; i++) {
          const _uint128 x = _uint128(randInt64(), randInt64()) & xmask;
          _uint128 ymask = 1;
          _uint128 y,q,r;
          for(UINT ybits = 1; ybits <= 128; ybits++, ymask = (ymask<<1)|1) {
            for(int j = 0; j < 30; j++) {
              do {
                y = _uint128(randInt64(), randInt64()) & ymask;
              } while(y.isZero());
              q = x / y;
              r = x % y;
              verify(y*q + r == x);
              verify(((q==0) && (x<y) && (r==x)) || (r < y));
            }
          }
        }
      }
    }

    TEST_METHOD(TestDivision2a) {
      for (int i = 0; i < 30; i++) {
        int ystep = randInt(2, 20);
        //        OUTPUT(_T("i:%d, ystep:%d"), i, ystep);
        _uint128 x = _uint128(randInt64(), randInt64());
        _uint128 y = randInt(1, 5);
        for (int j = 0; y < 0x8000; j++, y += ystep) {
          _uint128 d = x / y;
          _uint128 r = x % y;
          verify(y * d + r == x);
          verify((d >= 0) && (r >= 0) && (r < y));
        }
      }
    }

    TEST_METHOD(TestDivison3) {
      _uint128 x(0xfffffffffffffffui64, 0xffffffffffffffffui64); //   21267647932558653966460912964485513215
      _uint128 y(0xB94, 0x1F8E8E281519E475);                     //                  54678423345639783523445
      const _uint128 expectedQ(0x161C180916AC8);                 //                          388958690306760
      const _uint128 expectedR(0x713, 0x25F8E0D8E4E81297);       //                  33409789701483783525015

      _uint128 Q = x / y;
      _uint128 R = x % y;
      verify(Q == expectedQ);
      verify(R == expectedR);
    }

    TEST_METHOD(TestDivison00a) {
      const _uint128 x(0xec9e5bd9b3970be, 0x5d905a30d4d95138);   //   19657503303515891713170780434776805688
      const _uint128 y(0, 0x5196e4fb4f1df03a);                   //                      5879138131594047546
      const _uint128 expectedQ(0, 0x2e66dd1f38416c4b);           //                      3343602899526708299
      const _uint128 expectedR(0, 0x208573035701783a);           //                      2343405639298021434

      const _uint128 Q = x / y;
      const _uint128 R = x % y;
      verify(Q == expectedQ);
      verify(R == expectedR);
    }

    TEST_METHOD(TestDivison01a) {
      const _uint128 x(0xaa6e695c0b5f04b, 0x4c7c5d1fc812a76d);   //   14158878055212066840979520053222483821
      const _uint128 y(0, 0x314de1121cefe94d);                   //                      3552743148976335181
      const _uint128 expectedQ(0, 0x374ec2b71dd06a0a);           //                      3985336812004469258
      const _uint128 expectedR(0, 0x157f42090889a86b);           //                      1549029403404118123

      const _uint128 Q = x / y;
      const _uint128 R = x % y;
      verify(Q == expectedQ);
      verify(R == expectedR);
    }

    TEST_METHOD(TestDivison02a) {
      const _uint128 x(0x87ada792d0cd0f1, 0x70890d8405e8b107);   //   11271715348864155501847136111667228935
      const _uint128 y(0, 0x2331b107d01c9270);                   //                      2536002712300720752
      const _uint128 expectedQ(0, 0x3daeab0d114917f8);           //                      4444677954874185720
      const _uint128 expectedR(0, 0x020d5cb0a722c487);           //                       147876276561167495

      const _uint128 Q = x / y;
      const _uint128 R = x % y;
      verify(Q == expectedQ);
      verify(R == expectedR);
    }

    TEST_METHOD(TestDivison03a) {
      const _uint128 x(0x2f9778f805ae0b4, 0x12c3d7ccb4865538);   //    3953762885439023626648176384310400312
      const _uint128 y(0, 0x45fb9184025c8f5f);                   //                      5042784203932733279
      const _uint128 expectedQ(0, 0x0ae17b8977997151);           //                       784043640486457681
      const _uint128 expectedR(0, 0x1906b92f412d0929);           //                      1803332313416534313

      const _uint128 Q = x / y;
      const _uint128 R = x % y;
      verify(Q == expectedQ);
      verify(R == expectedR);
    }

    TEST_METHOD(TestDivison04) {
      const _uint128 x(0x4cbd249310e6c, 0x0d5ff365c0bcf2e7);     //      24903177149519783583261367323128551
      const _uint128 y(0, 0x3494f84d83d242ca);                   //                      3788926198333194954
      const _uint128 expectedQ(0, 0x001759c3a7a06908);           //                         6572621330147592
      const _uint128 expectedR(0, 0x0ae8bf0ee0860297);           //                       786088205593477783

      const _uint128 Q = x / y;
      const _uint128 R = x % y;
      verify(Q == expectedQ);
      verify(R == expectedR);
    }

    TEST_METHOD(TestDivison05) {
      const _uint128 x(0xe5da54bde203044, 0x7ed001013030232e);   //   19095428157273938029225758709062312750
      const _uint128 y(0, 0x5f84d68efb1f38d6);                   //                      6882862040115787990
      const _uint128 expectedQ(0, 0x268073524176c0b6);           //                      2774344167583039670
      const _uint128 expectedR(0, 0x1c4136a4b4b93b0a);           //                      2035968587582749450

      const _uint128 Q = x / y;
      const _uint128 R = x % y;
      verify(Q == expectedQ);
      verify(R == expectedR);
    }

    TEST_METHOD(TestDivison06) {
      const _uint128 x(0xb60c257e9871a3b, 0x689d30d003474bdc);   //   15123910204640001370786487648114920412
      const _uint128 y(0, 0x436ae31139cc619d);                   //                      4857944811196146077
      const _uint128 expectedQ(0, 0x2b346c309d8753dc);           //                      3113232198476977116
      const _uint128 expectedR(0, 0x40371c30513281f0);           //                      4627198135992746480

      const _uint128 Q = x / y;
      const _uint128 R = x % y;
      verify(Q == expectedQ);
      verify(R == expectedR);
    }

    TEST_METHOD(TestDivison07) {
      const _uint128 x(0xaa1c9e7af9d04a6, 0x1124f0c90bcf01f3);   //   14132334872458840783324386483684442611
      const _uint128 y(0, 0x7d41ec83ce8f1ba9);                   //                      9025755179077540777
      const _uint128 expectedQ(0, 0x15bac365cfed740e);           //                      1565778662512227342
      const _uint128 expectedR(0, 0x5800b173a4fdeab5);           //                      6341263385585117877

      const _uint128 Q = x / y;
      const _uint128 R = x % y;
      verify(Q == expectedQ);
      verify(R == expectedR);
    }

    TEST_METHOD(TestDivison08) {
      const _uint128 x(0x2550bbbdbf242ac, 0x5b8622e090743c2b);   //    3100039214787414484882657158882409515
      const _uint128 y(0, 0x3b4233b646350e7d);                   //                      4270032254655598205
      const _uint128 expectedQ(0, 0x0a1344422aa5bfb3);           //                       725999015910817715
      const _uint128 expectedR(0, 0x24d9173f0afad7c4);           //                      2655179014846207940

      const _uint128 Q = x / y;
      const _uint128 R = x % y;
      verify(Q == expectedQ);
      verify(R == expectedR);
    }

    TEST_METHOD(TestDivison09) {
      const _uint128 x(0xc9908a6d0e148c9, 0x0926db51b083556e);   //   16745332844571859754349129596536051054
      const _uint128 y(0, 0x2b5ccdda1b40d50c);                   //                      3124598578132079884
      const _uint128 expectedQ(0, 0x4a5faf64008d3c37);           //                      5359194925635025975
      const _uint128 expectedR(0, 0x186c13ad4c0abfda);           //                      1759803189421064154

      const _uint128 Q = x / y;
      const _uint128 R = x % y;
      verify(Q == expectedQ);
      verify(R == expectedR);
    }

    TEST_METHOD(TestDivison10) {
      const _uint128 x(0xed0c2cd88aa034b, 0x7c5ba6d43e94df5a);   //   19693140759095984500242704281181151066
      const _uint128 y(0xd4fb68066, 0x4e690ec18b817840);         //          1054636059310620277920032192576
      const _uint128 expectedQ(0, 0x00000000011ced1d);           //                                 18672925
      const _uint128 expectedR(0x91a9fc490, 0xf915091f2d97001a); //           721293220347162787151623946266

      const _uint128 Q = x / y;
      const _uint128 R = x % y;
      verify(Q == expectedQ);
      verify(R == expectedR);
    }

    TEST_METHOD(TestDivison11) {
      const _uint128 x(0xd5fd5e715317cd8, 0x1bd958e5b41244c2);   //   17777570608274845704678865690709607618
      const _uint128 y(0xcc0730ba6, 0x013cfb246a2eb974);         //          1010298153534406036701118904692
      const _uint128 expectedQ(0, 0x00000000010c7fc8);           //                                 17596360
      const _uint128 expectedR(0x776bfbcb2, 0xf97eeadb6c20d622); //           591348164696712765040943486498

      const _uint128 Q = x / y;
      const _uint128 R = x % y;
      verify(Q == expectedQ);
      verify(R == expectedR);
    }

    TEST_METHOD(TestDivison12) {
      const _uint128 x(0xb39577e309fd4e6, 0x62ba620672bc3c2d);   //   14919243442003059337704437148018293805
      const _uint128 y(0xfe52c49db, 0x1b09c2d88c542cfa);         //          1259348046473654632500451093754
      const _uint128 expectedQ(0, 0x0000000000b4c48f);           //                                 11846799
      const _uint128 expectedR(0x35647cff1, 0xced6146ffd7fb487); //           264387014111052725630984500359

      const _uint128 Q = x / y;
      const _uint128 R = x % y;
      verify(Q == expectedQ);
      verify(R == expectedR);
    }

    TEST_METHOD(TestDivison13) {
      const _uint128 x(0x1d27f2b9e568642, 0x7900adf4449d0a56);   //    2422189657911114745977921620772981334
      const _uint128 y(0x2064c72eb, 0x327543189c952497);         //           160405656138147247925170087063
      const _uint128 expectedQ(0, 0x0000000000e669f0);           //                                 15100400
      const _uint128 expectedR(0x11c38fcb6, 0xca7000a96d1ecdc6); //            87962636043408683238086856134

      const _uint128 Q = x / y;
      const _uint128 R = x % y;
      verify(Q == expectedQ);
      verify(R == expectedR);
    }

    TEST_METHOD(TestDivison14) {
      const _uint128 x(0xf2a16dd50f8fd8a, 0x649d09f36e1094aa);   //   20156960152327208543716165404505183402
      const _uint128 y(0x224cc79ce, 0x710990befa936a02);         //           169844981463817388975056972290
      const _uint128 expectedQ(0, 0x000000000712e42d);           //                                118678573
      const _uint128 expectedR(0x186f081f2, 0x9abcdaf295992a50); //           120989909687570471339427441232

      const _uint128 Q = x / y;
      const _uint128 R = x % y;
      verify(Q == expectedQ);
      verify(R == expectedR);
    }

    TEST_METHOD(TestDivison15) {
      const _uint128 x(0xcb5b31367817fb3, 0x7b25130730cc43cd);   //   16894173769501446189350867837817471949
      const _uint128 y(0xa820db14, 0x56b38a8a4982c968);          //            52033201849655759040483084648
      const _uint128 expectedQ(0, 0x00000000135a3bc9);           //                                324680649
      const _uint128 expectedR(0x4ba1fe59, 0x0f2ce094ce212925);  //            23407213917499202640782895397

      const _uint128 Q = x / y;
      const _uint128 R = x % y;
      verify(Q == expectedQ);
      verify(R == expectedR);
    }

    TEST_METHOD(TestDivison16) {
      const _uint128 x(0xb25b31c989c6083, 0x3880ac385210b615);   //   14817255754338236961086450364826301973
      const _uint128 y(0xe3681bb5c, 0x6d29735978b36beb);         //          1126063301948282343641517878251
      const _uint128 expectedQ(0, 0x0000000000c8c839);           //                                 13158457
      const _uint128 expectedR(0x2bb242b44, 0x6a41cad3f63616c2); //           216373747518420313949129283266

      const _uint128 Q = x / y;
      const _uint128 R = x % y;
      verify(Q == expectedQ);
      verify(R == expectedR);
    }

    TEST_METHOD(TestDivison17) {
      const _uint128 x(0xc11095e90c38917, 0x7ef7fccd916f420d);   //   16039195029950021097143121029368857101
      const _uint128 y(0xfc9a855de, 0x2d32946de01d021e);         //          1250832429725335737381729141278
      const _uint128 expectedQ(0, 0x0000000000c3a920);           //                                 12822816
      const _uint128 expectedR(0xbd2ccb3f4, 0x64f73005ca8f304d); //           936749110398472886488923058253

      const _uint128 Q = x / y;
      const _uint128 R = x % y;
      verify(Q == expectedQ);
      verify(R == expectedR);
    }

    TEST_METHOD(TestDivison18) {
      const _uint128 x(0x3b9cb8c1cd44e5f, 0x092af69180cd48f4);   //    4952387336198226662535812934131337460
      const _uint128 y(0xafb5bbae2, 0x3de8e7ab8234be8b);         //           870073257390976964007346617995
      const _uint128 expectedQ(0, 0x000000000056da0f);           //                                  5691919
      const _uint128 expectedR(0xa7d4ee627, 0x3f9f98ebf2c1c0cf); //           831062634452540080579579855055

      const _uint128 Q = x / y;
      const _uint128 R = x % y;
      verify(Q == expectedQ);
      verify(R == expectedR);
    }

    TEST_METHOD(TestDivison19) {
      const _uint128 x(0x23b8aeeb819979d, 0x7e9b6411d98f593b);   //    2967619392027549836540352149672909115
      const _uint128 y(0x85666de47, 0x402dc9d92398202d);         //           660565371072053669455940034605
      const _uint128 expectedQ(0, 0x0000000000448d00);           //                                  4492544
      const _uint128 expectedR(0x504bf4d1a, 0xa8c1570e83e2903b); //           397610021556148085482848423995

      const _uint128 Q = x / y;
      const _uint128 R = x % y;
      verify(Q == expectedQ);
      verify(R == expectedR);
    }

    TEST_METHOD(TestDivision20) {
      const _int128 x(_I128_MAX);
      const _int128 y(_I128_MIN);
      const _int128 Q = x / y;
      const _int128 R = x % y;
      const _uint128 expectedQ(0);
      const _uint128 expectedR(x);
      verify(Q == expectedQ);
      verify(R == expectedR);
    }

    TEST_METHOD(Int128_ui128div) {
      JavaRandom rnd;
      _uint128 n = 5;
      for(int p = 5; p < 128; p++) {
        for(int i = 0; i < 1000; i++) {
          _uint128 numer = randInt128(rnd);
          _uint128 denom;
          do {
            denom = randInt128(n, rnd);
          } while(denom == 0);

          _uint128 quot = numer / denom;
          _uint128 rem  = numer % denom;

          const _ui128div_t div = _ui128div(numer, denom);
          verify((quot == div.quot) && (rem == div.rem));
          verify(div.quot * denom + div.rem == numer);
        }
        n <<= 1;
        if(rnd.nextBool()) n |= 1;
      }
    }


    void checkSignCombinations(_int128 x, _int128 y, _int128 expQ, _int128 expR) {
      _int128 d = x / y;             // + / +
      verify(d == expQ);
      _int128 r = x % y;
      verify(r == expR);

      x = -x;
      d = x / y;
      verify(d == -expQ);            // - / +
      r = x % y;
      verify(r == -expR);

      x = -x;
      y = -y;
      d = x / y;
      verify(d == -expQ);            // + / -
      r = x % y;
      verify(r == expR);

      x = -x;
      d = x / y;
      verify(d == expQ);            // - / -
      r = x % y;
      verify(r == -expR);
    }

    TEST_METHOD(TestSignedDivision1) {
      _int128 x(0x00000000ffffffff);
      _int128 y(0x12345);
      _int128 expectedQ(0xE100);
      _int128 expectedR(0x5AFF);
      checkSignCombinations(x, y, expectedQ, expectedR);
    }

    TEST_METHOD(TestSignedDivision2) {
      _int128 x(0x00ffffffffffffff);
      _int128 y(0x12345);
      _int128 expectedQ(0xE1004FFB1C);
      _int128 expectedR(0x7D73);

      checkSignCombinations(x, y, expectedQ, expectedR);
    }

    TEST_METHOD(TestSignedDivision2a) {
      _uint128 xmask = 1;
      for(UINT xbits = 1; xbits <= 128; xbits++, xmask = (xmask<<1)|1) {
        for(int i = 0; i < 30; i++) {
          const _int128 x = _int128(randInt64(), randInt64()) & xmask;
          _uint128 ymask = 1;
          _int128 y,q,r;
          for(UINT ybits = 1; ybits <= 128; ybits++, ymask = (ymask<<1)|1) {
            for(int j = 0; j < 30; j++) {
              do {
                y = _int128(randInt64(), randInt64()) & ymask;
              } while(y.isZero());
              q = x / y;
              r = x % y;
              verify(y*q + r == x);
              verify((r == 0) || (sign(r) == sign(x)));
              verify(((q==0) && (abs(x)<abs(y)) && (r==x)) || (sign(q) == sign(x)*sign(y)));
            }
          }
        }
      }
    }

    TEST_METHOD(TestSignedDivison3) {
      _int128 x(0xfffffffffffffffui64, 0xffffffffffffffffui64); //   21267647932558653966460912964485513215
      _int128 y(0xB94, 0x1F8E8E281519E475);                     //                  54678423345639783523445
      const _int128 expectedQ(0x161C180916AC8);                 //                          388958690306760
      const _int128 expectedR(0x713, 0x25F8E0D8E4E81297);       //                  33409789701483783525015

      checkSignCombinations(x, y, expectedQ, expectedR);
    }


    TEST_METHOD(Int128ArithmethicOperators) {
      _uint128 x1 = _strtoui128("0xffffffffffffffffffffffffffffffff", NULL, 0);
      _uint128 res1 = x1 + 1;
      verify(res1 == 0);

      _uint128 x2 = _strtoui128("12345678901234567890123456789012345", NULL, 10);
      _uint128 y2 = _strtoui128("23456789012345678901234567890123456", NULL, 10);
      _uint128 z2 = x2 + y2;
      _uint128 res2 = _strtoui128("35802467913580246791358024679135801", NULL, 10);
      verify(z2 == res2);

      _uint128 z3 = y2 - x2;
      const char *str3 = "11111110111111111011111111101111111";
      _uint128 res3 = _strtoui128(str3, NULL, 10);

      verify(z3 == res3);
      char buf3[300];
      _ui128toa(z3, buf3, 10);
      verify(strcmp(str3, buf3) == 0);

      _uint128 x4 = _strtoui128("340282366920938463463374607431768211455", NULL, 10);
      verify(x4 == _UI128_MAX);
      _uint128 y4 = _strtoui128("54678423345639783523445", NULL, 10);
      _uint128 q4 = x4 / y4;
      _uint128 r4 = x4 % y4;
      _uint128 z4 = q4 * y4 + r4;
      verify(r4 < y4);
      _uint128 x4a = (q4 + 1) * y4;
      _int128 d4 = x4 - x4a;
      verify((d4 < 0) && (-d4 < r4));
      verify(z4 == x4);

      // ---------------------------------------------------------------

      _int128 x5p = _strtoi128("170141183460469231731687303715884105727", NULL, 10);
      _int128 x5n = _strtoi128("-170141183460469231731687303715884105728", NULL, 10);
      verify(x5p == _I128_MAX);
      verify(x5n == _I128_MIN);
      _int128 y5p = _strtoi128("54678423345639783523445", NULL, 10);
      _int128 y5n = _strtoi128("-54678423345639783523445", NULL, 10);

      _int128 x5pcopy(x5p);
      _int128 y5pcopy(y5p);

      _int128 x5ncopy(x5n);
      _int128 y5ncopy(y5n);

      _int128 s5np = x5n + y5p;
      _int128 s5npcopy(s5np);
      verify((x5n == x5ncopy) && (y5p == y5pcopy) && (s5np <= 0) && (s5np > x5n));

      _int128 d5nn = x5n - y5n;
      _int128 d5nncopy(d5nn);
      verify((x5n == x5ncopy) && (y5n == y5ncopy) && (d5nn <= 0) && (d5nn > x5n));

      _int128 s5pn = x5p + y5n;
      _int128 s5pncopy(s5pn);
      verify((x5p == x5pcopy) && (y5n == y5ncopy) && (s5pn >= 0) && (s5pn < x5p));

      _int128 d5pp = x5p - y5p;
      _int128 d5ppcopy(d5pp);
      verify((x5p == x5pcopy) && (y5p == y5pcopy) && (d5pp >= 0) && (d5pp < x5p));

      s5np -= y5p;
      verify((s5np == x5ncopy) && (y5p == y5pcopy));

      d5nn += y5n;
      verify((d5nn == x5ncopy) && (y5n == y5ncopy));

      s5pn -= y5n;
      verify((s5pn == x5pcopy) && (y5n == y5ncopy));

      d5pp += y5p;
      verify((d5pp == x5pcopy) && (y5p == y5pcopy));

      // ---------------------------------------------------------------

      _int128 q5pp = x5p / y5p;
      _int128 q5ppcopy(q5pp);
      verify((x5p == x5pcopy) && (y5p == y5pcopy) && (q5pp >= 0));

      _int128 q5pn = x5p / y5n;
      _int128 q5pncopy(q5pn);
      verify((x5p == x5pcopy) && (y5n == y5ncopy) && (q5pn <= 0));

      _int128 q5np = x5n / y5p;
      _int128 q5npcopy(q5np);
      verify((x5n == x5ncopy) && (y5p == y5pcopy) && (q5np <= 0));

      _int128 q5nn = x5n / y5n;
      _int128 q5nncopy(q5nn);
      verify((x5n == x5ncopy) && (y5n == y5ncopy) && (q5nn >= 0));

      // ---------------------------------------------------------------

      _int128 r5pp = x5p % y5p;
      _int128 r5ppcopy(r5pp);
      verify((x5p == x5pcopy) && (y5p == y5pcopy) && (r5pp >= 0) && (r5pp < y5p));

      _int128 r5pn = x5p % y5n;
      _int128 r5pncopy(r5pn);
      verify((x5p == x5pcopy) && (y5n == y5ncopy) && (r5pn >= 0) && (r5pn < y5p));

      _int128 r5np = x5n % y5p;
      _int128 r5npcopy(r5np);
      verify((x5n == x5ncopy) && (y5p == y5pcopy) && (r5np <= 0) && (r5np > y5n));

      _int128 r5nn = x5n % y5n;
      _int128 r5nncopy(r5nn);
      verify((x5n == x5ncopy) && (y5n == y5ncopy) && (r5nn <= 0) && (r5nn > y5n));

      // ---------------------------------------------------------------

      _int128 z5pp = q5pp * y5p + r5pp;
      verify((z5pp == x5pcopy) && (y5p == y5pcopy) && (q5pp == q5ppcopy) && (r5pp == r5ppcopy));

      _int128 z5pn = q5pn * y5n + r5pn;
      verify((z5pn == x5pcopy) && (y5n == y5ncopy) && (q5pn == q5pncopy) && (r5pn == r5pncopy));

      _int128 z5np = q5np * y5p + r5np;
      verify((z5np == x5ncopy) && (y5p == y5pcopy) && (q5np == q5npcopy) && (r5np == r5npcopy));

      _int128 z5nn = q5nn * y5n + r5nn;
      verify((z5nn == x5ncopy) && (y5n == y5ncopy) && (q5nn == q5nncopy) && (r5nn == r5nncopy));

      // ---------------------------------------------------------------

      _int128 x5qp = x5p;
      _int128 x5qn = x5n;

      x5qp /= y5p;
      verify((x5qp == q5pp) && (y5p == y5pcopy));

      x5qn /= y5p;
      verify((x5qn == q5np) && (y5p == y5pcopy));

      x5qp *= y5p;
      verify((x5p - x5qp == r5pp) && (y5p == y5pcopy));

      x5qn *= y5p;
      verify((x5n - x5qn == r5np) && (y5p == y5pcopy));

      x5qp = x5p;
      x5qn = x5n;

      x5qp /= y5n;
      verify((x5qp == q5pn) && (y5n == y5ncopy));

      x5qn /= y5n;
      verify((x5qn == q5nn) && (y5n == y5ncopy));

      x5qp *= y5n;
      verify((x5p - x5qp == r5pn) && (y5n == y5ncopy));

      x5qn *= y5n;
      verify((x5n - x5qn == r5nn) && (y5n == y5ncopy));

      // ---------------------------------------------------------------

      _int128 x5rp = x5p;
      _int128 x5rn = x5n;

      x5rp %= y5p;
      verify((x5rp == r5pp) && (y5p == y5pcopy));

      x5rn %= y5p;
      verify((x5rn == r5np) && (y5p == y5pcopy));

      x5rp = x5p;
      x5rn = x5n;

      x5rp %= y5n;
      verify((x5rp == r5pn) && (y5n == y5ncopy));

      x5rn %= y5n;
      verify((x5rn == r5nn) && (y5n == y5ncopy));

      // ---------------------------------------------------------------

      _int128  x6 = _strtoi128("-0x80000000000000000000000000000000", NULL, 0);
      _uint128 ux6 = _strtoui128("0x80000000000000000000000000000000", NULL, 0);
      /*
            TCHAR bx6[1200], bimin[200];
            _i128tow(x6, bx6, 16);
            _i128tow(_I128_MIN, bimin, 16);
            OUTPUT(_T("x6:%s"), bx6);
            OUTPUT(_T("Imin:%s"), bimin);
      */
      verify(x6 == _I128_MIN);
      _int128 y6 = _strtoi128("-54678423345639783523445", NULL, 0);
    } // Int128ArithmethicOperators

    TEST_METHOD(Int128BitOperators) {
      _int128 x1 = 0;
      x1 = ~x1;
      verify(x1 == -1);
      x1 = x1 >> 2;
      verify(x1 == -1);
      x1 = x1 << 3;
      verify(x1 == -8);

      _uint128 x2 = 0;
      x2 = ~x2;
      verify(x2 == _UI128_MAX);

      x2 = x2 >> 3;
      verify(x2 == _UI128_MAX / 8);
      x2 = x2 << 3;
      verify(x2 == _UI128_MAX - 7);
      x2 = x2 | 7;
      verify(x2 == _UI128_MAX);

      _int128 b1 = _strtoi128("0x123456789abcdef0123456789abcdef", NULL, 0);
      _int128 b2 = _strtoi128("0x7777777777777777777777777777777", NULL, 0);
      _int128 b3 = _strtoi128("0x46328ab5cdf43a89b3c819bf6483219", NULL, 0);
      _int128 ea = _strtoi128("0x1234567012345670123456701234567", NULL, 0);
      verify((b1 & b2) == ea);
      _int128 eo = _strtoi128("0x7777777ffffffff77777777ffffffff", NULL, 0);
      verify((b1 | b2) == eo);

      _int128 x13 = b1 ^ b3;
      _int128 b1mb3 = b1 & ~b3;
      _int128 b3mb1 = b3 & ~b1;
      _int128 symDif13 = b1mb3 | b3mb1;
      verify(x13 == symDif13);

    } // Int128BitOperators

    template<class I128Type> void testAllShifts(I128Type v0, I128Type(*op)(const I128Type&, int)) {
      CompactArray<I128Type> singleShiftValue(129);
      singleShiftValue.add(v0);
      for (int i = 1; i <= 128; i++) {
        singleShiftValue.add(op(singleShiftValue.last(), 1));
      }
      for (int i = 0; i <= 128; i++) {
        const I128Type shftValue = op(v0, i);
        verify(shftValue == singleShiftValue[i]);
      }
    }

    TEST_METHOD(Int128ShiftOperators) {
      const _int128  si0 = randInt128() & _I128_MAX;
      const _int128  si1 = randInt128() | _I128_MIN;
      const _uint128 ui0 = si0;
      const _uint128 ui1 = si1;

      testAllShifts(si0, operator<<);
      testAllShifts(si0, operator>>);
      testAllShifts(si1, operator>>);
      testAllShifts(ui0, operator>>);
      testAllShifts(ui1, operator>>);
    } // Int128ShiftOperators

    static inline UINT64 getRandInt64(UINT bits) {
      return (bits >= 64) ? randInt64() : (randInt64() & ((UINT64(1) << bits) - 1));
    }

    static inline _uint128 getRandInt128(UINT bits) {
      return (bits <= 64) ? getRandInt64(bits) : _uint128(getRandInt64(bits - 64), randInt64());
    }

    static inline bool isPow2(int n) {
      return (n & -n) == n;
    }

#define USE_INT64_AS_INTTYPE
#if defined(USE_INT64_AS_INTTYPE)

    typedef INT64           _inttype;
    typedef UINT64          _uinttype;
#define MINIVALUE       _I64_MIN
#define MAXIVALUE       _I64_MAX
#define MAXUIVALUE      _UI64_MAX
#define BITCOUNT        64
#define _itypetoa       _i64toa
#define _uitypetoa      _ui64toa
#define _itypetow       _i64tow
#define _uitypetow      _ui64tow
#define _atoitype       _atoi64
#define _atouitype      _atoui64
#define _wcstoitype     _wcstoi64
#define _wcstouitype    _wcstoui64
#define randuitype      getRandInt64

#else // USE_INT64_AS_INTTYPE

    typedef _int128         _inttype;
    typedef _uint128        _uinttype;
#define MINIVALUE       _I128_MIN
#define MAXIVALUE       _I128_MAX
#define MAXUIVALUE      _UI128_MAX
#define BITCOUNT        128
#define _itypetoa       _i128toa
#define _uitypetoa      _ui128toa
#define _itypetow       _i128tow
#define _uitypetow      _ui128tow
#define _atoitype       _atoi128
#define _atouitype      _atoui128
#define _wcstoitype     _wcstoi128
#define _wcstouitype    _wcstoui128
#define randuitype      getRandInt128

#endif // USE_INT64_AS_INTTYPE

#if defined(_UNICODE)

#define _itypetot       _itypetow
#define _uitypetot      _uitypetow
#define _tcstoitype     _wcstoitype
#define _tcstouitype    _wcstouitype

#else

#define _itypetot       _itypetoa
#define _uitypetot      _uitypetoa
#define _tcstoitype     _atoitype
#define _tcstouitype    _atouitype

#endif // _UNICODE

#define MINRADIX 2
#define MAXRADIX 36

    static inline TCHAR radixLetter(UINT c) {
      return (c < 10) ? ('0' + c) : ('a' + (c - 10));
    }

    static String uitypeToString(_uinttype v, int radix) {
      if (v == 0) {
        return _T("0");
      }
      String result;
      do {
        const UINT c = v % radix;
        result += radixLetter(c);
        v /= radix;
      } while (v != 0);
      return rev(result);
    }

    static String itypeToString(_inttype v, int radix) {
      if ((radix != 10) || (v >= 0)) {
        return uitypeToString((_uinttype)v, radix);
      }
      else {
        return String(_T("-")) + uitypeToString((_uinttype)(-v), radix);
      }
    }

    static String incr(const String &num, UINT radix) {
      String result = num;
      const TCHAR maxDigit = radixLetter(radix - 1);
      for (TCHAR *dp = result.cstr() + result.length() - 1;; dp--) {
        if (*dp < maxDigit) {
          (*dp)++;
          break;
        }
        (*dp) = '0';
        if (dp == result.cstr()) {
          result.insert(0, '1');
          break;
        }
      }
      return result;
    }

#pragma warning(disable : 4996)

    TEST_METHOD(Int128Test_ui128tot) {
      for (int i = 0; i < 1000; i++) {
        const int       bits = randInt(BITCOUNT) + 1;
        const _uinttype x = randuitype(bits);
        for (int radix = MINRADIX; radix <= MAXRADIX; radix++) {
          const String wanted = uitypeToString(x, radix);
          TCHAR str[200], *endp;
          const String numStr = _uitypetot(x, str, radix);
          verify(numStr == wanted);

          const String numStr1 = numStr + _T("!");
          endp = NULL; errno = 0;
          const _uinttype x1 = _tcstouitype(numStr1.cstr(), &endp, radix);
          verify(errno == 0);
          verify(endp && (*endp == '!'));
          verify(x1 == x);

          const String nstr = _T("-") + wanted + _T("!");
          endp = NULL; errno = 0;
          const _uinttype x2 = _tcstouitype(nstr.cstr(), &endp, radix);
          verify(errno == 0);
          verify(endp && (*endp == '!'));
          verify(x2 == (~x) + 1);

          String prefix;
          bool tryRadix0 = false;
          switch (radix) {
          case  8: tryRadix0 = true; prefix = _T("0");  break;
          case 10: tryRadix0 = true;                    break;
          case 16: tryRadix0 = true; prefix = _T("0x"); break;
          }
          if (tryRadix0) {
            const String sp = prefix + numStr1;
            endp = NULL; errno = 0;
            const  _uinttype xp = _tcstouitype(sp.cstr(), &endp, 0);
            verify(errno == 0);
            verify(endp && (*endp == '!'));
            verify(xp == x);
          }
        }
      }
    }

    TEST_METHOD(Int128Test_i128tot) {
      for (int i = 0; i < 1000; i++) {
        const int      bits = randInt(BITCOUNT) + 1;
        const _inttype x = randuitype(bits);
        for (int radix = MINRADIX; radix <= MAXRADIX; radix++) {
          const String wanted = itypeToString(x, radix);
          TCHAR str[200], *endp;
          const String numStr = _itypetot(x, str, radix);
          verify(numStr == wanted);

          const String numStr1 = numStr + _T("!");
          endp = NULL; errno = 0;
          const  _inttype x1 = _tcstoitype(numStr1.cstr(), &endp, radix);
          if ((radix != 10) && (x < 0)) {
            verify(errno == ERANGE);
            verify(endp && (*endp == '!'));
            verify(x1 == MAXIVALUE);
          } else {
            verify(errno == 0);
            verify(endp && (*endp == '!'));
            verify(x1 == x);
          }

          if (radix != 10) {
            const String nstr = _T("-") + wanted + _T("!");
            endp = NULL; errno = 0;
            const _inttype x2 = _tcstoitype(nstr.cstr(), &endp, radix);
            if (x < 0) {
              verify(errno == ERANGE);
              verify(endp && (*endp == '!'));
              verify(x2 == MINIVALUE);
            }
            else {
              verify(errno == 0);
              verify(endp && (*endp == '!'));
              verify(x2 == (~x) + 1);
            }
          }

          String prefix;
          bool tryRadix0 = false;
          switch (radix) {
          case  8: tryRadix0 = x >= 0; prefix = _T("0");  break;
          case 10: tryRadix0 = true;                    break;
          case 16: tryRadix0 = x >= 0; prefix = _T("0x"); break;
          }
          if (tryRadix0) {
            const String sp = prefix + numStr1;
            endp = NULL; errno = 0;
            const  _inttype xp = _tcstoitype(sp.cstr(), &endp, 0);
            verify(errno == 0);
            verify(endp && (*endp == '!'));
            verify(xp == x);
          }
        }
      }
    }

    TEST_METHOD(Int128TestOverflowUnsigned) {
      const _uinttype x = MAXUIVALUE;

      for (int radix = MINRADIX; radix <= MAXRADIX; radix++) {
        TCHAR str[200], *endp;
        const String numStr = _uitypetot(x, str, radix);

        const String numStr1 = numStr + _T("!");
        endp = NULL; errno = 0;
        const _uinttype x1 = _tcstouitype(numStr1.cstr(), &endp, radix);
        verify(errno == 0);
        verify(endp && (endp[0] == '!'));
        verify(x1 == x);

        endp = NULL; errno = 0;
        const String    numStr2 = incr(numStr, radix) + _T("!");
        const _uinttype x2 = _tcstouitype(numStr2.cstr(), &endp, radix);
        verify(errno == ERANGE);
        verify(endp && (endp[0] == '!'));
        verify(x2 == x);
      }
    }

#if defined(USE_INT64_AS_INTTYPE)
#pragma warning(disable : 4307) // integral constant overflow on __int64
#endif

    TEST_METHOD(Int128TestOverflowSigned) {
      const _inttype x = MAXIVALUE;
      const _inttype nx = x + 1;

      for (int radix = MINRADIX; radix <= MAXRADIX; radix++) {
        TCHAR strx[200], strnx[200], *endp;
        const String numStrx = _itypetot(x, strx, radix);
        String       numStrnx = _itypetot(nx, strnx, radix);
        if (radix != 10) {
          numStrnx = String(_T("-")) + numStrnx;
        }
        numStrnx += _T("!");

        const String numStr1 = numStrx + _T("!");
        endp = NULL; errno = 0;
        _inttype x1 = _tcstoitype(numStr1.cstr(), &endp, radix);
        verify(errno == 0);
        verify(endp && (endp[0] == '!'));
        verify(x1 == x);

        endp = NULL; errno = 0;
        x1 = _tcstoitype(numStrnx.cstr(), &endp, radix);
        verify(errno == 0);
        verify(endp && (endp[0] == '!'));
        verify(x1 == nx);

        endp = NULL; errno = 0;
        const String    numStr2 = incr(numStrx, radix) + _T("!");
        const _inttype  x2 = _tcstoitype(numStr2.cstr(), &endp, radix);
        verify(errno == ERANGE);
        verify(endp && (endp[0] == '!'));
        verify(x2 == x);
      }
    }

#if defined(USE_INT64_AS_INTTYPE)
#define OPENERRORLOG()                                                                                \
  FILE *_errorLog = MKFOPEN(getTestFileName(__TFUNCTION__,_T("txt")),_T("w"));                        \
  UINT _lastFormatCounterDumped = -1

#define PRINTPARAM()                                                                                  \
{ if(formatCounter != _lastFormatCounterDumped) {                                                     \
    _lastFormatCounterDumped = formatCounter;                                                         \
    _ftprintf(_errorLog, _T("formatCounter:%d format:%s\n"), formatCounter, param.toString().cstr()); \
  }                                                                                                   \
}

#define PRINTERRORLOG(...) { PRINTPARAM();_ftprintf(_errorLog, __VA_ARGS__); }
#define CLOSEERRORLOG()    fclose(_errorLog)
#else
#define OPENERRORLOG()
#define PRINTERRORLOG(...)
#define CLOSEERRORLOG()
#endif

    template<class INTTYPE> void testToFromStream(const CompactArray<INTTYPE> &a, INTTYPE maxValue) {
      try {
        OPENERRORLOG();
        StreamParametersIterator it = StreamParameters::getIntParamIterator(20, 0, ITERATOR_INTFORMATMASK & ~ios::internal);
        const UINT               totalFormatCount = (UINT)it.getMaxIterationCount(), quatil = totalFormatCount/4;
        UINT                     formatCounter    = 0;
        while(it.hasNext()) {
          const StreamParameters &param = it.next();
          if(++formatCounter % quatil == 0) {
            INFO(_T("%s progress:%.2lf%%"), __TFUNCTION__, PERCENT(formatCounter, totalFormatCount));
          }
          const UINT     radix = param.radix();
          ostringstream  costr;
          wostringstream wostr;

//          OUTPUT(_T("formatCounter:%d format:%s"), formatCounter, param.toString().cstr());

          setFormat(costr, param);
          setFormat(wostr, param);
          const StreamSize w = param.width();
          for(size_t i = 0; i < a.size(); i++) {
            const INTTYPE &x = a[i];
            costr.width(w);
            wostr.width(w);
            costr << x << endl;
            wostr << x << endl;
          }

          const string  cstr = costr.str();
          const wstring wstr = wostr.str();
          verify(String(cstr.c_str()) == String(wstr.c_str()));

          CompactLineArray lineArray(cstr);
          verify((StreamSize)lineArray.minLength() >= param.width());

          istringstream    cistr(cstr);
          wistringstream   wistr(wstr);
          StreamParameters ip(param);
          ip.flags(param.flags() | ios::skipws);
          setFormat(cistr, ip);
          setFormat(wistr, ip);
          for(size_t i = 0; i < a.size(); i++) {
            const INTTYPE &expected = a[i];

            if(!iswspace(ip.fill())) {
              skipspace(cistr);
              skipfill( cistr);
              skipspace(wistr);
              skipfill( wistr);
            }
            INTTYPE cx = 0x1234567890abcdef;
            INTTYPE wx = 0x1234567890abcdef;
            cistr >> cx;
            if(!cistr) {
              const int err = errno;
              PRINTERRORLOG(_T("input failed(state:[%s], errno=%3d). line[%3zu]:<%s> cx:%38s, expected:%s\n")
                            ,streamStateToString(cistr).cstr(),err
                            ,i, lineArray[i]
                            ,toString(cx      ,0,0,param.flags()).cstr()
                            ,toString(expected,0,0,param.flags()).cstr());

              verify(err == ERANGE);
              verify(cx == maxValue);
              cistr.clear();

              wistr >> wx;
              verify(!wistr);
              verify(errno == ERANGE);
              verify(wx == maxValue);
              wistr.clear();
            } else {
              wistr >> wx;
              verify(wistr.good());
              verify(cx == expected);
              verify(wx == expected);
            }
          }
        }

        CLOSEERRORLOG();

      } catch (Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        verify(false);
      }
    }

    TEST_METHOD(Int128Test_i128StreamIO) {
      CompactArray<_inttype>  sa;
      for(_inttype x = 0; x <= MAXIVALUE/4; x = (x + 1) * 3 ) { // add some positive test-numbers
        sa.add(x);
      }
      sa.add(MAXIVALUE);
      for(_inttype x = 0; x >= MINIVALUE/4; x = (x - 1) * 3 ) { // add some negative test-numbers
        sa.add((_inttype)x);
      }
      sa.add(MINIVALUE);
      testToFromStream(sa, MAXIVALUE);
    }

    TEST_METHOD(Int128Test_ui128StreamIO) {
      CompactArray<_uinttype> ua;
      for(_uinttype x = 0; x <= MAXUIVALUE/16; x = (x + 1) * 11) { // add unsigned test-numbers
        ua.add(x);
      }
      ua.add(MAXUIVALUE);
      testToFromStream(ua, MAXUIVALUE);
    }

    template<class INT64TYPE, class INT128TYPE> void testToStream(CompactArray<INT64TYPE> &a) {
      try {
        StreamParametersIterator it               = StreamParameters::getIntParamIterator(30);
        const UINT               totalFormatCount = (UINT)it.getMaxIterationCount(), quatil = totalFormatCount/4;
        UINT                     formatCounter    = 0;
        while(it.hasNext()) {
          const StreamParameters &param = it.next();
          if(++formatCounter % quatil == 0) {
            INFO(_T("%s progress:%.2lf%%"), __TFUNCTION__, PERCENT(formatCounter, totalFormatCount));
          }
          const UINT     radix = param.radix();
          ostringstream  ostr64, costr128;
          wostringstream wostr128;

          for(size_t i = 0; i < a.size(); i++) {
            const INT64TYPE &x64  = a[i];
            INT128TYPE       x128 = x64;

            if((radix != 10) && (x64 < 0)) {
              x128 &= 0xffffffffffffffffui64;
            }
            setFormat( ostr64 , param);
            setFormat(costr128, param);
            setFormat(wostr128, param);
            ostr64   << x64 << endl;
            costr128 << x128 << endl;
            wostr128 << x128 << endl;
          }

          string  str64   =  ostr64.str();
          string  cstr128 = costr128.str();

          if(cstr128 != str64) {
            const CompactLineArray tmp64(  str64  );
            const CompactLineArray ctmp128(cstr128);
            const size_t n64 = tmp64.size(), n218 = ctmp128.size();
            for(size_t i = 0; i < n64; i++) {
              if(_tcscmp(tmp64[i], ctmp128[i]) != 0) {
                const size_t     errIndex = i;
                const TCHAR     *s64      = tmp64[errIndex], *s128 = ctmp128[errIndex];
                const INT64TYPE  x1       = a[errIndex];
                const INT128TYPE x2       = x1;
                ostringstream  o64, o128;
                setFormat(o64 , param);
                setFormat(o128, param);
                o64 << x1 << endl;
                o128 << x2 << endl;
              }
            }
            verify(cstr128 == str64);
          }
          verify(String(wostr128.str().c_str()) == String(str64.c_str()));

        } // for(iterator
      } catch (Exception e) {
        throwException(_T("Exception:%s"), e.what());
      }
    }


    TEST_METHOD(Int128Test_i128StreamO) {
      CompactArray<INT64> a;
      for(INT64 x = 0; x <= _I64_MAX/16; x = (x + 1) * 11) { // add some positive test-numbers
        a.add(x);
      }
      a.add(_I64_MAX);
      for(INT64 x = 0; x >= _I64_MIN/16; x = (x - 1) * 11) { // add some negative test-numbers
        a.add(x);
      }
      a.add(_I64_MIN);
      testToStream<INT64, _int128>(a);
    }

    TEST_METHOD(Int128Test_ui128StreamO) {
      CompactArray<UINT64> a;
      for(UINT64 x = 0; x <= _UI64_MAX/16; x = (x + 1) * 11) { // add unsigned test-numbers
        a.add(x);
      }
      a.add(_UI64_MAX);
      testToStream<UINT64, _uint128>(a);
    }
  };
}
