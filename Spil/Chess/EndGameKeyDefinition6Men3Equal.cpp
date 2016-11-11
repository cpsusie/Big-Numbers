#include "stdafx.h"
#include "EndGameUtil.h"
#include "EndGameKeyCodec.h"

#define MININDEX 0

static EndGamePosIndex rangeStartKKOffDiag[59];
static EndGamePosIndex rangeStartKKOnDiag[59];
static EndGamePosIndex rangeStartKKP2OnDiag[27];

#define KK_OFFDIAG_POSCOUNT                      (MAXINDEX_KK_OFFDIAG_3MEN  - MININDEX_KK_OFFDIAG_3MEN )
#define KK_ONDIAG_POSCOUNT                       (MAXINDEX_KK_ONDIAG_3MEN   - MININDEX_KK_ONDIAG_3MEN  )
#define KKP2_ONDIAG_POSCOUNT                     (MAXINDEX_KKP2_ONDIAG_3MEN - MININDEX_KKP2_ONDIAG_3MEN)

#define KKP2_ONDIAG_1_BELOWDIAG_POSCOUNT          GET_RANGESTART3EQUAL(KKP2_ONDIAG_POSCOUNT, 26)
#define KKP2_ONDIAG_2_BELOWDIAG_POSCOUNT          GET_RANGESTART3EQUAL(KKP2_ONDIAG_POSCOUNT, 27)
#define KKP2_ONDIAG_3_BELOWDIAG_POSCOUNT          KKP2_ONDIAG_1_BELOWDIAG_POSCOUNT

#define KKP23_ONDIAG_POSCOUNT                    (MAXINDEX_KKP23_ONDIAG_4MEN  - MININDEX_KKP23_ONDIAG_4MEN)
#define KKP23_ONDIAG_P45_BELOWDIAG_POSCOUNT       GET_RANGESTART2EQUAL(KKP23_ONDIAG_POSCOUNT, 27)
#define KKP23_ONDIAG_P4_BELOWDIAG_POSCOUNT        GET_RANGESTART2EQUAL(KKP23_ONDIAG_POSCOUNT, 28)
#define KKP245_ONDIAG_POSCOUNT                   (MAXINDEX_KKP2_ONDIAG_4MEN  - MININDEX_KKP2_ONDIAG_4MEN  )

#define START_RANGE_KK_ONDIAG1                   (                                           GET_RANGESTART3EQUAL(KK_OFFDIAG_POSCOUNT ,59) )
#define START_RANGE_KKP2_ONDIAG_P5_BELOWDIAG     (START_RANGE_KK_ONDIAG1                   + GET_RANGESTART3EQUAL(KK_ONDIAG_POSCOUNT  ,59) )
#define START_RANGE_KKP2_ONDIAG_P35_BELOWDIAG    (START_RANGE_KKP2_ONDIAG_P5_BELOWDIAG     + KKP2_ONDIAG_1_BELOWDIAG_POSCOUNT              )
#define START_RANGE_KKP2_ONDIAG_P45_BELOWDIAG    (START_RANGE_KKP2_ONDIAG_P35_BELOWDIAG    + KKP2_ONDIAG_2_BELOWDIAG_POSCOUNT              )
#define START_RANGE_KKP2_ONDIAG_P345_BELOWDIAG   (START_RANGE_KKP2_ONDIAG_P45_BELOWDIAG    + KKP2_ONDIAG_2_BELOWDIAG_POSCOUNT              )
#define START_RANGE_KKP23_ONDIAG_P45_BELOWDIAG   (START_RANGE_KKP2_ONDIAG_P345_BELOWDIAG   + KKP2_ONDIAG_3_BELOWDIAG_POSCOUNT              )
#define START_RANGE_KKP23_ONDIAG_P4_BELOWDIAG    (START_RANGE_KKP23_ONDIAG_P45_BELOWDIAG   + KKP23_ONDIAG_P45_BELOWDIAG_POSCOUNT           )
#define START_RANGE_KKP245_ONDIAG                (START_RANGE_KKP23_ONDIAG_P4_BELOWDIAG    + KKP23_ONDIAG_P4_BELOWDIAG_POSCOUNT            )
#define START_RANGE_KKP2345_ONDIAG               (START_RANGE_KKP245_ONDIAG                + GET_RANGESTART2EQUAL(KKP245_ONDIAG_POSCOUNT,4))

EndGameKeyDefinition6Men3Equal::EndGameKeyDefinition6Men3Equal(PieceKey pk2, PieceKey pk345)
: EndGameKeyDefinitionDupletsAllowed(pk2, pk345, pk345)
{
  assert((GET_TYPE_FROMKEY(pk2  ) != Pawn)
      && (GET_TYPE_FROMKEY(pk345) != Pawn)
      && (pk2 != pk345));

  static bool initDone = false;
  if(initDone) return;

  initDone = true;

  INIT_RANGETABLE3EQUAL(rangeStartKKOffDiag , KK_OFFDIAG_POSCOUNT  );
  INIT_RANGETABLE3EQUAL(rangeStartKKOnDiag  , KK_ONDIAG_POSCOUNT   );
  INIT_RANGETABLE3EQUAL(rangeStartKKP2OnDiag, KKP2_ONDIAG_POSCOUNT );
/*
  _tprintf(_T("Constants for %s\n"), getCodecName().cstr());
  DUMP_MACRO( KK_OFFDIAG_POSCOUNT                     );
  DUMP_MACRO( KK_ONDIAG_POSCOUNT                      );
  DUMP_MACRO( KKP2_ONDIAG_POSCOUNT                    );
  DUMP_MACRO( KKP2_ONDIAG_1_BELOWDIAG_POSCOUNT        );
  DUMP_MACRO( KKP2_ONDIAG_2_BELOWDIAG_POSCOUNT        );
  DUMP_MACRO( KKP2_ONDIAG_3_BELOWDIAG_POSCOUNT        );
  DUMP_MACRO( KKP23_ONDIAG_P45_BELOWDIAG_POSCOUNT     );
  DUMP_MACRO( KKP23_ONDIAG_P4_BELOWDIAG_POSCOUNT      );
  DUMP_MACRO( KKP245_ONDIAG_POSCOUNT                  );

  DUMP_MACRO(START_RANGE_KK_ONDIAG1                   );
  DUMP_MACRO(START_RANGE_KKP2_ONDIAG_P5_BELOWDIAG     );
  DUMP_MACRO(START_RANGE_KKP2_ONDIAG_P35_BELOWDIAG    );
  DUMP_MACRO(START_RANGE_KKP2_ONDIAG_P45_BELOWDIAG    );
  DUMP_MACRO(START_RANGE_KKP2_ONDIAG_P345_BELOWDIAG   );
  DUMP_MACRO(START_RANGE_KKP23_ONDIAG_P45_BELOWDIAG   );
  DUMP_MACRO(START_RANGE_KKP23_ONDIAG_P4_BELOWDIAG    );
  DUMP_MACRO(START_RANGE_KKP245_ONDIAG                );
  DUMP_MACRO(START_RANGE_KKP2345_ONDIAG               );

  DUMP_RANGETABLE(rangeStartKKOffDiag   );
  DUMP_RANGETABLE(rangeStartKKOnDiag    );
  DUMP_RANGETABLE(rangeStartKKP2OnDiag  );
*/
}

#ifdef _DEBUG

#define ENCODE_KK_OFFDIAG(           key)          return encode_kk_offdiag6m3e(            key)
#define DECODE_KK_OFFDIAG(           key, index  )        decode_kk_offdiag6m3e(            key, index)
#define ENCODE_KK_ONDIAG(            key)          return encode_kk_ondiag6m3e(             key)
#define DECODE_KK_ONDIAG(            key, index  )        decode_kk_ondiag6m3e(             key, index)
#define ENCODE_KKP2_ONDIAG_NOFLIP(   key)          return encode_kkp2_ondiag_noflip6m3e(    key)
#define DECODE_KKP2_ONDIAG_NOFLIP(   key, index  )        decode_kkp2_ondiag_noflip6m3e(    key, index)
#define ENCODE_KKP2_ONDIAG_FLIPi(    key, i, j, k) return encode_kkp2_ondiag_flipi6m3e(     key, i, j, k)
#define DECODE_KKP2_ONDIAG_FLIPi(    key, index  )        decode_kkp2_ondiag_flipi6m3e(     key, index)
#define DECODE_KKP2_ONDIAG_FLIPj(    key, index  )        decode_kkp2_ondiag_flipj6m3e(     key, index)
#define ENCODE_KKP2_ONDIAG_FLIPij(   key, i, j, k) return encode_kkp2_ondiag_flipij6m3e(    key, i, j, k)
#define DECODE_KKP2_ONDIAG_FLIPij(   key, index  )        decode_kkp2_ondiag_flipij6m3e(    key, index)
#define ENCODE_KKP23_ONDIAG_NOFLIP(  key, i, j)    return encode_kkp23_ondiag_noflip6m3e(   key, i, j)
#define DECODE_KKP23_ONDIAG_NOFLIP(  key, index  )        decode_kkp23_ondiag_noflip6m3e(   key, index)
#define ENCODE_KKP23_ONDIAG_FLIPi(   key, i, j)    return encode_kkp23_ondiag_flipi6m3e(    key, i, j)
#define DECODE_KKP23_ONDIAG_FLIPi(   key, index  )        decode_kkp23_ondiag_flipi6m3e(    key, index)
#define ENCODE_KKP23_ONDIAG_NOSWAP(  key, i, j)    return encode_kkp23_ondiag_noswap6m3e(   key, i, j)
#define DECODE_KKP23_ONDIAG_NOSWAP(  key, index  )        decode_kkp23_ondiag_noswap6m3e(   key, index)
#define ENCODE_KKP23_ONDIAG_PRESWAPi(key, i, j)    return encode_kkp23_ondiag_preswapi6m3e( key, i, j)
#define ENCODE_KKP245_ONDIAG_NOSWAP( key         )        encode_kkp245_ondiag_noswap6m3e(  key)
#define DECODE_KKP245_ONDIAG_NOSWAP( key, index  )        decode_kkp245_ondiag_noswap6m3e(  key, index)
#define ENCODE_KKP234_ONDIAG_PRESWAP(key)          return encode_kkp234_ondiag_preswap6m3e( key)
#define ENCODE_KKP235_ONDIAG_PRESWAP(key)          return encode_kkp235_ondiag_preswap6m3e( key)
#define ENCODE_KKP245_ONDIAG_PRESWAP(key)          return encode_kkp245_ondiag_preswap6m3e( key)
#define ENCODE_KKP2345_ONDIAG(       key)          return encode_kkp2345_ondiag6m3e(        key)
#define DECODE_KKP2345_ONDIAG(       key, index)          decode_kkp2345_ondiag6m3e(        key, index)

static EndGamePosIndex encode_kk_offdiag6m3e(EndGameKey key) {
  key.sort3Pos(3,4,5);
  const UINT pi3 = key.getP3OffDiagIndex();
  const UINT pi4 = key.getP4OffDiagIndexEqualP34();
  const UINT pi5 = key.getP5OffDiagIndexEqualP345();
  const EndGamePosIndex maxAddr = KK_OFFDIAG_POSCOUNT;
  return ADDPIT(key, ADD3EQUAL(KK_OFFDIAG_3MEN(key), maxAddr, pi3, pi4, pi5))
       - MININDEX;
}

static void decode_kk_offdiag6m3e(EndGameKey &key, EndGamePosIndex index) {
  const EndGamePosIndex maxAddr = KK_OFFDIAG_POSCOUNT;
  SETPIT(              key, index  );
  SET3POS3EQUAL(       key, index, rangeStartKKOffDiag, maxAddr, 3, 4, 5);
  SETP2_INDEX(         key, index   );
  SETKK_OFFDIAG(       key, index   );
  key.p2IndexToOffDiagPos();
  key.p345IndexToOffDiagPosEqualP345();
}

static EndGamePosIndex encode_kk_ondiag6m3e(EndGameKey key) {
  const EndGamePosIndex maxAddr = KK_ONDIAG_POSCOUNT;
  key.sort3Pos(3,4,5);
  const UINT pi3 = key.getP3OffDiagIndex();
  const UINT pi4 = key.getP4OffDiagIndexEqualP34();
  const UINT pi5 = key.getP5OffDiagIndexEqualP345();
  EndGamePosIndex addr = ADD3EQUAL(KK_ONDIAG_3MEN(key), maxAddr, pi3, pi4, pi5);
  return ADDPIT(key, addr)
       + START_RANGE_KK_ONDIAG1
       - MININDEX;
}

static void decode_kk_ondiag6m3e(EndGameKey &key, EndGamePosIndex index) {
  const EndGamePosIndex maxAddr = KK_ONDIAG_POSCOUNT;
  index -= START_RANGE_KK_ONDIAG1;
  SETPIT(              key, index   );
  SET3POS3EQUAL(       key, index, rangeStartKKOnDiag, maxAddr, 3, 4, 5);
  SETPOS_BELOWDIAG(    key, index, 2);
  SETKK_ONDIAG(        key, index   );
  key.p345IndexToOffDiagPosEqualP345();
}

static EndGamePosIndex encode_kkp2_ondiag_noflip6m3e(EndGameKey key) {
  const EndGamePosIndex maxAddr = KKP2_ONDIAG_POSCOUNT;

  UINT pi3 = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(3)];
  UINT pi4 = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(4)];
  UINT pi5 = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(5)];
  SORT3(pi3, pi4, pi5); /* pi3 < pi4 < pi5 */
  return ADDPIT(key, ADD3EQUAL(KKP2_ONDIAG_3MEN(key), maxAddr, pi3, pi4, pi5))
       + START_RANGE_KKP2_ONDIAG_P345_BELOWDIAG
       - MININDEX;
}

static void decode_kkp2_ondiag_noflip6m3e(EndGameKey &key, EndGamePosIndex index) {
  const EndGamePosIndex maxAddr = KKP2_ONDIAG_POSCOUNT;
  index -= START_RANGE_KKP2_ONDIAG_P345_BELOWDIAG;
  SETPIT(              key, index   );
  SET3OFFDIAGPOSNOFLIP(key, index, rangeStartKKP2OnDiag, maxAddr, 3, 4, 5);
  SETP2_ONDIAG(        key, index   );
  SETKK_ONDIAG(        key, index   );
  key.p2IndexToDiagPos();
}

static EndGamePosIndex encode_kkp2_ondiag_flipi6m3e(EndGameKey key, UINT i, UINT j, UINT k) {
  const EndGamePosIndex maxAddr = KKP2_ONDIAG_POSCOUNT;
  UINT pi = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(i)] - 28;
  UINT pj = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(j)];
  UINT pk = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(k)];
  SORT2(pj, pk);
  if(pi <= pj) {
    return ADDPIT(key, ADD3EQUALALLOWEQUALLM(KKP2_ONDIAG_3MEN(key), maxAddr, pi, pj, pk))
         + START_RANGE_KKP2_ONDIAG_P45_BELOWDIAG
         - MININDEX;
  } else {
    return ADDPIT(key, ADD3EQUALALLOWEQUALHM(KKP2_ONDIAG_3MEN(key), maxAddr, pj, pi, pk))
         + START_RANGE_KKP2_ONDIAG_P35_BELOWDIAG
         - MININDEX;
  }
}

static void decode_kkp2_ondiag_flipi6m3e(EndGameKey &key, EndGamePosIndex index) {
  const EndGamePosIndex maxAddr = KKP2_ONDIAG_POSCOUNT;
  index -= START_RANGE_KKP2_ONDIAG_P45_BELOWDIAG;
  SETPIT(              key, index  );
  SET3OFFDIAGPOSFLIPi( key, index, rangeStartKKP2OnDiag, maxAddr, 3, 4, 5);
  SETP2_ONDIAG(        key, index   );
  SETKK_ONDIAG(        key, index   );
  key.p2IndexToDiagPos();
}

static void decode_kkp2_ondiag_flipj6m3e(EndGameKey &key, EndGamePosIndex index) {
  const EndGamePosIndex maxAddr = KKP2_ONDIAG_POSCOUNT;
  index -= START_RANGE_KKP2_ONDIAG_P35_BELOWDIAG;
  SETPIT(              key, index  );
  SET3OFFDIAGPOSFLIPj( key, index, rangeStartKKP2OnDiag, maxAddr, 3, 4, 5);
  SETP2_ONDIAG(        key, index   );
  SETKK_ONDIAG(        key, index   );
  key.p2IndexToDiagPos();
}

static EndGamePosIndex encode_kkp2_ondiag_flipij6m3e(EndGameKey key, int i, int j, int k) {
  const EndGamePosIndex maxAddr = KKP2_ONDIAG_POSCOUNT;
  UINT pi = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(i)] - 28;
  UINT pj = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(j)] - 28;
  UINT pk = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(k)];
  SORT2(pi, pj);
  return ADDPIT(key, ADD3EQUAL(KKP2_ONDIAG_3MEN(key), maxAddr, pi, pj, pk))
       + START_RANGE_KKP2_ONDIAG_P5_BELOWDIAG
       - MININDEX;
}

static void decode_kkp2_ondiag_flipij6m3e(EndGameKey &key, EndGamePosIndex index) {
  const EndGamePosIndex maxAddr = KKP2_ONDIAG_POSCOUNT;
  index -= START_RANGE_KKP2_ONDIAG_P5_BELOWDIAG;
  SETPIT(              key, index  );
  SET3OFFDIAGPOSFLIPij(key, index, rangeStartKKP2OnDiag, maxAddr, 3, 4, 5);
  SETP2_ONDIAG(        key, index   );
  SETKK_ONDIAG(        key, index   );
  key.p2IndexToDiagPos();
}

static EndGamePosIndex encode_kkp23_ondiag_noflip6m3e(EndGameKey key, int i, int j) {
  const EndGamePosIndex maxAddr = KKP23_ONDIAG_POSCOUNT;
  UINT pi = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(i)];
  UINT pj = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(j)];
  SORT2(pi, pj);
  return ADDPIT(key, ADD2EQUAL(KKP23_ONDIAG_4MEN(key), maxAddr, pi, pj))
       + START_RANGE_KKP23_ONDIAG_P45_BELOWDIAG
       - MININDEX;
}

static void decode_kkp23_ondiag_noflip6m3e(EndGameKey &key, EndGamePosIndex index) {
  const EndGamePosIndex maxAddr = KKP23_ONDIAG_POSCOUNT;
  index -= START_RANGE_KKP23_ONDIAG_P45_BELOWDIAG;
  SETPIT(              key, index   );
  SET2OFFDIAGPOSNOFLIP(key, index, maxAddr, 4, 5);
  SETP3_ONDIAG(        key, index   );
  SETP2_ONDIAG(        key, index   );
  SETKK_ONDIAG(        key, index   );
  key.p23IndexToDiagPos();
}

static EndGamePosIndex encode_kkp23_ondiag_flipi6m3e(EndGameKey key, int i, int j) {
  const EndGamePosIndex maxAddr = KKP23_ONDIAG_POSCOUNT;
  UINT pi = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(i)] - 28;
  UINT pj = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(j)];
  SORT2(pi, pj);
  return ADDPIT(key, ADD2EQUALALLOWEQUALLH(KKP23_ONDIAG_4MEN(key), maxAddr, pi, pj))
       + START_RANGE_KKP23_ONDIAG_P4_BELOWDIAG
       - MININDEX;
}

static void decode_kkp23_ondiag_flipi6m3e(EndGameKey &key, EndGamePosIndex index) {
  const EndGamePosIndex maxAddr = KKP23_ONDIAG_POSCOUNT;
  index -= START_RANGE_KKP23_ONDIAG_P4_BELOWDIAG;
  SETPIT(              key, index   );
  SET2OFFDIAGPOSFLIPj( key, index, maxAddr, 4, 5);
  SETP3_ONDIAG(        key, index   );
  SETP2_ONDIAG(        key, index   );
  SETKK_ONDIAG(        key, index   );
  key.p23IndexToDiagPos();
}

static EndGamePosIndex encode_kkp23_ondiag_noswap6m3e(EndGameKey key, int i, int j) {
  switch(KEYBOOL2MASK(key, IS_ABOVEMAINDIAG1, i, j)) {
  case 0: ENCODE_KKP23_ONDIAG_NOFLIP(key,i,j);
  case 1: ENCODE_KKP23_ONDIAG_FLIPi( key,i,j);
  case 2: ENCODE_KKP23_ONDIAG_FLIPi( key,j,i);
  case 3: throwException(_T("%s:Impossible encoding"), __TFUNCTION__);
  }
  return 0;
}

static EndGamePosIndex encode_kkp23_ondiag_preswapi6m3e(EndGameKey key, int i, int j) {
  key.swapPos(2, i);
  ENCODE_KKP23_ONDIAG_NOSWAP(key,2,j);
}

static EndGamePosIndex encode_kkp245_ondiag_noswap6m3e(EndGameKey key) {
  const EndGamePosIndex maxAddr = KKP245_ONDIAG_POSCOUNT;
  const UINT pi4 = key.getP4DiagIndexEqualP34();
  const UINT pi5 = key.getP5DiagIndexEqualP345();
  return ADDPIT(key, ADD2EQUAL(KKP2_ONDIAG_4MEN(key), maxAddr, pi4, pi5))
       + START_RANGE_KKP245_ONDIAG
       - MININDEX;
}

static void decode_kkp245_ondiag_noswap6m3e(EndGameKey &key, EndGamePosIndex index) {
  const EndGamePosIndex maxAddr = KKP245_ONDIAG_POSCOUNT;
  index -= START_RANGE_KKP245_ONDIAG;
  SETPIT(              key, index   );
  SET2POS2EQUAL(       key, index, maxAddr, 4, 5);
  SETPOS_BELOWDIAG(    key, index, 3);
  SETP2_ONDIAG(        key, index   );
  SETKK_ONDIAG(        key, index   );
  key.p2IndexToDiagPos();
  key.p45IndexToDiagPosEqualP345();
}

static EndGamePosIndex encode_kkp234_ondiag_preswap6m3e(EndGameKey key) {
  const int pos3 = key.getPosition(3);
  const int pos4 = key.getPosition(4);
  const int pos5 = key.getPosition(5);
  if(pos3 < pos4) {
    key.setPosition(3, pos5);
    key.setPosition(4, pos3);
    key.setPosition(5, pos4);
  } else {
    key.setPosition(3, pos5);
    key.setPosition(5, pos3);
  }
  return ENCODE_KKP245_ONDIAG_NOSWAP(key);
}

static EndGamePosIndex encode_kkp235_ondiag_preswap6m3e(EndGameKey key) {
  const int pos3 = key.getPosition(3);
  const int pos4 = key.getPosition(4);
  const int pos5 = key.getPosition(5);
  if(pos3 < pos5) {
    key.setPosition(3, pos4);
    key.setPosition(4, pos3);
  } else {
    key.setPosition(3, pos4);
    key.setPosition(4, pos5);
    key.setPosition(5, pos3);
  }
  return ENCODE_KKP245_ONDIAG_NOSWAP(key);
}

static EndGamePosIndex encode_kkp245_ondiag_preswap6m3e(EndGameKey key) {
  key.sort2Pos(4,5);
  return ENCODE_KKP245_ONDIAG_NOSWAP(key);
}

static EndGamePosIndex encode_kkp2345_ondiag6m3e(EndGameKey key) {
  const EndGamePosIndex maxAddr = KKP2_ONDIAG_POSCOUNT;
  key.sort3Pos(3,4,5);
  const UINT pi3 = key.getP3DiagIndex();
  const UINT pi4 = key.getP4DiagIndexEqualP34();
  const UINT pi5 = key.getP5DiagIndexEqualP345();
  return ADDPIT(key, ADD3EQUAL(KKP2_ONDIAG_3MEN(key), maxAddr, pi3, pi4, pi5))
       + START_RANGE_KKP2345_ONDIAG
       - MININDEX;
}

static void decode_kkp2345_ondiag6m3e(EndGameKey &key, EndGamePosIndex index) {
  const EndGamePosIndex maxAddr = KKP2_ONDIAG_POSCOUNT;
  index -= START_RANGE_KKP2345_ONDIAG;
  SETPIT(              key, index   );
  SET3POS3EQUALa(      key, index, rangeStartKKP2OnDiag, 6, maxAddr, 3, 4, 5);
  SETP2_ONDIAG(        key, index   );
  SETKK_ONDIAG(        key, index   );
  key.p2IndexToDiagPos();
  key.p345IndexToDiagPosEqualP345();
}

#else // !_DEBUG

#define ENCODE_KK_OFFDIAG(key)                                                                                                    \
{ key.sort3Pos(3,4,5);                                                                                                            \
  const UINT pi3 = key.getP3OffDiagIndex();                                                                                       \
  const UINT pi4 = key.getP4OffDiagIndexEqualP34();                                                                               \
  const UINT pi5 = key.getP5OffDiagIndexEqualP345();                                                                              \
  return ADDPIT(key, ADD3EQUAL(KK_OFFDIAG_3MEN(key), KK_OFFDIAG_POSCOUNT, pi3, pi4, pi5))                                         \
       - MININDEX;                                                                                                                \
}

#define DECODE_KK_OFFDIAG(key, index)                                                                                             \
{ SETPIT(              key, index  );                                                                                             \
  SET3POS3EQUAL(       key, index, rangeStartKKOffDiag, KK_OFFDIAG_POSCOUNT, 3, 4, 5);                                            \
  SETP2_INDEX(         key, index   );                                                                                            \
  SETKK_OFFDIAG(       key, index   );                                                                                            \
  key.p2IndexToOffDiagPos();                                                                                                      \
  key.p345IndexToOffDiagPosEqualP345();                                                                                           \
}

#define ENCODE_KK_ONDIAG(key)                                                                                                     \
{ key.sort3Pos(3,4,5);                                                                                                            \
  const UINT pi3 = key.getP3OffDiagIndex();                                                                                       \
  const UINT pi4 = key.getP4OffDiagIndexEqualP34();                                                                               \
  const UINT pi5 = key.getP5OffDiagIndexEqualP345();                                                                              \
  return ADDPIT(key, ADD3EQUAL(KK_ONDIAG_3MEN(key), KK_ONDIAG_POSCOUNT, pi3, pi4, pi5))                                           \
       + START_RANGE_KK_ONDIAG1                                                                                                   \
       - MININDEX;                                                                                                                \
}

#define DECODE_KK_ONDIAG(key, index)                                                                                              \
{ index -= START_RANGE_KK_ONDIAG1;                                                                                                \
  SETPIT(              key, index   );                                                                                            \
  SET3POS3EQUAL(       key, index, rangeStartKKOnDiag, KK_ONDIAG_POSCOUNT, 3, 4, 5);                                              \
  SETPOS_BELOWDIAG(    key, index, 2);                                                                                            \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p345IndexToOffDiagPosEqualP345();                                                                                           \
}

#define ENCODE_KKP2_ONDIAG_NOFLIP(key)                                                                                            \
{ UINT pi3 = s_offDiagPosToIndex[pos3];                                                                                           \
  UINT pi4 = s_offDiagPosToIndex[pos4];                                                                                           \
  UINT pi5 = s_offDiagPosToIndex[pos5];                                                                                           \
  SORT3(pi3, pi4, pi5); /* pi3 < pi4 < pi5 */                                                                                     \
  return ADDPIT(key, ADD3EQUAL(KKP2_ONDIAG_3MEN(key), KKP2_ONDIAG_POSCOUNT, pi3, pi4, pi5))                                       \
       + START_RANGE_KKP2_ONDIAG_P345_BELOWDIAG                                                                                   \
       - MININDEX;                                                                                                                \
}

#define DECODE_KKP2_ONDIAG_NOFLIP(key, index)                                                                                     \
{ index -= START_RANGE_KKP2_ONDIAG_P345_BELOWDIAG;                                                                                \
  SETPIT(              key, index   );                                                                                            \
  SET3OFFDIAGPOSNOFLIP(key, index, rangeStartKKP2OnDiag, KKP2_ONDIAG_POSCOUNT, 3, 4, 5);                                          \
  SETP2_ONDIAG(        key, index   );                                                                                            \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p2IndexToDiagPos();                                                                                                         \
}

#define ENCODE_KKP2_ONDIAG_FLIPi(key,i,j,k)                                                                                       \
{ UINT pi = s_offDiagPosToIndex[pos##i] - 28;                                                                                     \
  UINT pj = s_offDiagPosToIndex[pos##j];                                                                                          \
  UINT pk = s_offDiagPosToIndex[pos##k];                                                                                          \
  SORT2(pj, pk);                                                                                                                  \
  if(pi <= pj) {                                                                                                                  \
    return ADDPIT(key, ADD3EQUALALLOWEQUALLM(KKP2_ONDIAG_3MEN(key), KKP2_ONDIAG_POSCOUNT, pi, pj, pk))                            \
         + START_RANGE_KKP2_ONDIAG_P45_BELOWDIAG                                                                                  \
         - MININDEX;                                                                                                              \
  } else {                                                                                                                        \
    return ADDPIT(key, ADD3EQUALALLOWEQUALHM(KKP2_ONDIAG_3MEN(key), KKP2_ONDIAG_POSCOUNT, pj, pi, pk))                            \
         + START_RANGE_KKP2_ONDIAG_P35_BELOWDIAG                                                                                  \
         - MININDEX;                                                                                                              \
  }                                                                                                                               \
}

#define DECODE_KKP2_ONDIAG_FLIPi(key, index)                                                                                      \
{ index -= START_RANGE_KKP2_ONDIAG_P45_BELOWDIAG;                                                                                 \
  SETPIT(              key, index  );                                                                                             \
  SET3OFFDIAGPOSFLIPi( key, index, rangeStartKKP2OnDiag, KKP2_ONDIAG_POSCOUNT, 3, 4, 5);                                          \
  SETP2_ONDIAG(        key, index   );                                                                                            \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p2IndexToDiagPos();                                                                                                         \
}

#define DECODE_KKP2_ONDIAG_FLIPj(key, index)                                                                                      \
{ index -= START_RANGE_KKP2_ONDIAG_P35_BELOWDIAG;                                                                                 \
  SETPIT(              key, index  );                                                                                             \
  SET3OFFDIAGPOSFLIPj( key, index, rangeStartKKP2OnDiag, KKP2_ONDIAG_POSCOUNT, 3, 4, 5);                                          \
  SETP2_ONDIAG(        key, index   );                                                                                            \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p2IndexToDiagPos();                                                                                                         \
}

#define ENCODE_KKP2_ONDIAG_FLIPij(key,i,j,k)                                                                                      \
{ UINT pi = s_offDiagPosToIndex[pos##i] - 28;                                                                                     \
  UINT pj = s_offDiagPosToIndex[pos##j] - 28;                                                                                     \
  UINT pk = s_offDiagPosToIndex[pos##k];                                                                                          \
  SORT2(pi, pj);                                                                                                                  \
  return ADDPIT(key, ADD3EQUAL(KKP2_ONDIAG_3MEN(key), KKP2_ONDIAG_POSCOUNT, pi, pj, pk))                                          \
       + START_RANGE_KKP2_ONDIAG_P5_BELOWDIAG                                                                                     \
       - MININDEX;                                                                                                                \
}

#define DECODE_KKP2_ONDIAG_FLIPij(key, index)                                                                                     \
{ index -= START_RANGE_KKP2_ONDIAG_P5_BELOWDIAG;                                                                                  \
  SETPIT(              key, index  );                                                                                             \
  SET3OFFDIAGPOSFLIPij(key, index, rangeStartKKP2OnDiag, KKP2_ONDIAG_POSCOUNT, 3, 4, 5);                                          \
  SETP2_ONDIAG(        key, index   );                                                                                            \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p2IndexToDiagPos();                                                                                                         \
}

#define ENCODE_KKP23_ONDIAG_NOFLIP(key, i, j)                                                                                     \
{ UINT pi = s_offDiagPosToIndex[pos##i];                                                                                          \
  UINT pj = s_offDiagPosToIndex[pos##j];                                                                                          \
  SORT2(pi, pj);                                                                                                                  \
  return ADDPIT(key, ADD2EQUAL(KKP23_ONDIAG_4MEN(key), KKP23_ONDIAG_POSCOUNT, pi, pj))                                            \
       + START_RANGE_KKP23_ONDIAG_P45_BELOWDIAG                                                                                   \
       - MININDEX;                                                                                                                \
}

#define DECODE_KKP23_ONDIAG_NOFLIP(key, index)                                                                                    \
{ index -= START_RANGE_KKP23_ONDIAG_P45_BELOWDIAG;                                                                                \
  SETPIT(              key, index   );                                                                                            \
  SET2OFFDIAGPOSNOFLIP(key, index, KKP23_ONDIAG_POSCOUNT, 4, 5);                                                                  \
  SETP3_ONDIAG(        key, index   );                                                                                            \
  SETP2_ONDIAG(        key, index   );                                                                                            \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p23IndexToDiagPos();                                                                                                        \
}

#define ENCODE_KKP23_ONDIAG_FLIPi(key, i, j)                                                                                      \
{ UINT pi = s_offDiagPosToIndex[pos##i] - 28;                                                                                     \
  UINT pj = s_offDiagPosToIndex[pos##j];                                                                                          \
  SORT2(pi, pj);                                                                                                                  \
  return ADDPIT(key, ADD2EQUALALLOWEQUALLH(KKP23_ONDIAG_4MEN(key), KKP23_ONDIAG_POSCOUNT, pi, pj))                                \
       + START_RANGE_KKP23_ONDIAG_P4_BELOWDIAG                                                                                    \
       - MININDEX;                                                                                                                \
}

#define DECODE_KKP23_ONDIAG_FLIPi(key, index)                                                                                     \
{ index -= START_RANGE_KKP23_ONDIAG_P4_BELOWDIAG;                                                                                 \
  SETPIT(              key, index   );                                                                                            \
  SET2OFFDIAGPOSFLIPj( key, index, KKP23_ONDIAG_POSCOUNT, 4, 5);                                                                  \
  SETP3_ONDIAG(        key, index   );                                                                                            \
  SETP2_ONDIAG(        key, index   );                                                                                            \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p23IndexToDiagPos();                                                                                                        \
}

#define ENCODE_KKP23_ONDIAG_NOSWAP(key, i, j)                                                                                     \
  switch(BOOL2MASK(IS_ABOVEMAINDIAG1, pos##i, pos##j)) {                                                                          \
  case 0: ENCODE_KKP23_ONDIAG_NOFLIP(key,i,j);                                                                                    \
  case 1: ENCODE_KKP23_ONDIAG_FLIPi( key,i,j);                                                                                    \
  case 2: ENCODE_KKP23_ONDIAG_FLIPi( key,j,i);                                                                                    \
  case 3: impossibleEncodingError(key);                                                                                           \
  }

#define ENCODE_KKP23_ONDIAG_PRESWAPi(key,i,j)                                                                                     \
{ key.swapPos(3,i);                                                                                                               \
  ENCODE_KKP23_ONDIAG_NOSWAP(key,3,j);                                                                                            \
}

#define ENCODE_KKP245_ONDIAG_NOSWAP(key)                                                                                          \
{ const UINT pi4 = key.getP4DiagIndexEqualP34();                                                                                  \
  const UINT pi5 = key.getP5DiagIndexEqualP345();                                                                                 \
  return ADDPIT(key, ADD2EQUAL(KKP2_ONDIAG_4MEN(key), KKP245_ONDIAG_POSCOUNT, pi4, pi5))                                          \
       + START_RANGE_KKP245_ONDIAG                                                                                                \
       - MININDEX;                                                                                                                \
}

#define DECODE_KKP245_ONDIAG_NOSWAP(key, index)                                                                                   \
{ index -= START_RANGE_KKP245_ONDIAG;                                                                                             \
  SETPIT(              key, index   );                                                                                            \
  SET2POS2EQUAL(       key, index, KKP245_ONDIAG_POSCOUNT, 4, 5);                                                                 \
  SETPOS_BELOWDIAG(    key, index, 3);                                                                                            \
  SETP2_ONDIAG(        key, index   );                                                                                            \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p2IndexToDiagPos();                                                                                                         \
  key.p45IndexToDiagPosEqualP345();                                                                                               \
}

#define ENCODE_KKP234_ONDIAG_PRESWAP(key)                                                                                         \
{ if(pos3 < pos4) {                                                                                                               \
    key.setPosition(3,pos5);                                                                                                      \
    key.setPosition(4,pos3);                                                                                                      \
    key.setPosition(5,pos4);                                                                                                      \
  } else {                                                                                                                        \
    key.setPosition(3,pos5);                                                                                                      \
    key.setPosition(5,pos3);                                                                                                      \
  }                                                                                                                               \
  ENCODE_KKP245_ONDIAG_NOSWAP(key);                                                                                               \
}

#define ENCODE_KKP235_ONDIAG_PRESWAP(key)                                                                                         \
{ if(pos3 < pos5) {                                                                                                               \
    key.setPosition(3,pos4);                                                                                                      \
    key.setPosition(4,pos3);                                                                                                      \
  } else {                                                                                                                        \
    key.setPosition(3,pos4);                                                                                                      \
    key.setPosition(4,pos5);                                                                                                      \
    key.setPosition(5,pos3);                                                                                                      \
  }                                                                                                                               \
  ENCODE_KKP245_ONDIAG_NOSWAP(key);                                                                                               \
}

#define ENCODE_KKP245_ONDIAG_PRESWAP(key)                                                                                         \
{ key.sort2Pos(4,5);                                                                                                              \
  ENCODE_KKP245_ONDIAG_NOSWAP(key);                                                                                               \
}

#define ENCODE_KKP2345_ONDIAG(key)                                                                                                \
{ key.sort3Pos(3,4,5);                                                                                                            \
  const UINT pi3 = key.getP3DiagIndex();                                                                                          \
  const UINT pi4 = key.getP4DiagIndexEqualP34();                                                                                  \
  const UINT pi5 = key.getP5DiagIndexEqualP345();                                                                                 \
  return ADDPIT(key, ADD3EQUAL(KKP2_ONDIAG_3MEN(key), KKP2_ONDIAG_POSCOUNT, pi3, pi4, pi5))                                       \
       + START_RANGE_KKP2345_ONDIAG                                                                                               \
       - MININDEX;                                                                                                                \
}

#define DECODE_KKP2345_ONDIAG(key, index)                                                                                         \
{ index -= START_RANGE_KKP2345_ONDIAG;                                                                                            \
  SETPIT(              key, index   );                                                                                            \
  SET3POS3EQUALa(      key, index, rangeStartKKP2OnDiag, 5, KKP2_ONDIAG_POSCOUNT, 3, 4, 5);                                       \
  SETP2_ONDIAG(        key, index   );                                                                                            \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p2IndexToDiagPos();                                                                                                         \
  key.p345IndexToDiagPosEqualP345();                                                                                              \
}

#endif // _DEBUG

EndGamePosIndex EndGameKeyDefinition6Men3Equal::keyToIndex(EndGameKey key) const {
  UINT pos3 = key.getPosition(3);
  UINT pos4 = key.getPosition(4);
  UINT pos5 = key.getPosition(5);

  if(!key.kingsOnMainDiag1()) {                                         // kings off maindiag => p2, p3, p4 anywhere
    ENCODE_KK_OFFDIAG(key);
  } else if(!key.p2OnMainDiag1()) {                                     // kings on    diag
    ENCODE_KK_ONDIAG(key);
  } else {                                                              // kings,p2 on diag
    switch(BOOL3MASK(IS_OFFMAINDIAG1, pos3, pos4, pos5)) {
    case 0: ENCODE_KKP2345_ONDIAG(           key    );                  // 3,4,5 on    diag
    case 1: ENCODE_KKP245_ONDIAG_PRESWAP(    key    );                  //   4,5 on    diag
    case 2: ENCODE_KKP235_ONDIAG_PRESWAP(    key    );                  // 3,  5 on    diag
    case 3: ENCODE_KKP23_ONDIAG_PRESWAPi(    key,5,4);                  //     5 on    diag
    case 4: ENCODE_KKP234_ONDIAG_PRESWAP(    key    );                  // 3,4   on    diag
    case 5: ENCODE_KKP23_ONDIAG_PRESWAPi(    key,4,5);                  //   4   on    diag
    case 6: ENCODE_KKP23_ONDIAG_NOSWAP(      key,4,5);                  // 3     on    diag
    case 7:                                                             // none  on    diag
      switch(BOOL3MASK(IS_ABOVEMAINDIAG1, pos3, pos4, pos5)) {
      case 0: ENCODE_KKP2_ONDIAG_NOFLIP(key      );                     // 3,4,5 below diag
      case 1: ENCODE_KKP2_ONDIAG_FLIPi( key,3,4,5);                     //   4,5 below diag
      case 2: ENCODE_KKP2_ONDIAG_FLIPi( key,4,3,5);                     // 3,  5 below diag
      case 3: ENCODE_KKP2_ONDIAG_FLIPij(key,3,4,5);                     //     5 below diag
      case 4: ENCODE_KKP2_ONDIAG_FLIPi( key,5,3,4);                     // 3,4   below diag
      case 5: ENCODE_KKP2_ONDIAG_FLIPij(key,3,5,4);                     //   4   below diag
      case 6: ENCODE_KKP2_ONDIAG_FLIPij(key,4,5,3);                     // 3     below diag
      case 7: ;                                                         // none  below diag => error
      }
    }
  }
  impossibleEncodingError(key);
  return 0;
}

EndGameKey EndGameKeyDefinition6Men3Equal::indexToKey(EndGamePosIndex index) const {
  index += MININDEX;

  EndGameKey result;

  if(index < START_RANGE_KK_ONDIAG1) {
    DECODE_KK_OFFDIAG(result, index);
  } else if(index < START_RANGE_KKP2_ONDIAG_P5_BELOWDIAG) {
    DECODE_KK_ONDIAG(result, index);
  } else if(index < START_RANGE_KKP2_ONDIAG_P35_BELOWDIAG) {
    DECODE_KKP2_ONDIAG_FLIPij(result, index);
  } else if(index < START_RANGE_KKP2_ONDIAG_P45_BELOWDIAG) {
    DECODE_KKP2_ONDIAG_FLIPj(result, index);
  } else if(index < START_RANGE_KKP2_ONDIAG_P345_BELOWDIAG) {
    DECODE_KKP2_ONDIAG_FLIPi(result, index);
  } else if(index < START_RANGE_KKP23_ONDIAG_P45_BELOWDIAG) {
    DECODE_KKP2_ONDIAG_NOFLIP(result, index);
  } else if(index < START_RANGE_KKP23_ONDIAG_P4_BELOWDIAG) {
    DECODE_KKP23_ONDIAG_NOFLIP(result, index);
  } else if(index < START_RANGE_KKP245_ONDIAG) {
    DECODE_KKP23_ONDIAG_FLIPi(result, index);
  } else if(index < START_RANGE_KKP2345_ONDIAG) {
    DECODE_KKP245_ONDIAG_NOSWAP(result, index);
  } else {
    DECODE_KKP2345_ONDIAG(result, index);
  }
  return result;
}

SymmetricTransformation EndGameKeyDefinition6Men3Equal::getSymTransformation(EndGameKey key) const {
  return getSym8Transformation6Men3Equal(key);
}

#ifdef TABLEBASE_BUILDER

void EndGameKeyDefinition6Men3Equal::scanPositions(EndGameKeyWithOccupiedPositions &key, int pIndex, bool allPreviousOnDiag) const {
  switch(pIndex) {
  case 2:
    sym8PositionScanner(key, 2, allPreviousOnDiag, (PositionScanner)&EndGameKeyDefinition6Men3Equal::scanPositions);
    break;
  case 3:
    { for(int pos3 = 0; pos3 < 64; pos3++) { // pos3 = [0..63]
        if(key.isOccupied(pos3)) continue;
        key.setPosition(3,pos3);
        scanPositions(key, 4, allPreviousOnDiag && IS_ONMAINDIAG1(pos3));
        key.clearField(pos3);
      }
    }
    break;
  case 4:
    { const int pos3 = key.getPosition(3);
      if(allPreviousOnDiag) {                // kings, p2,p3 on maindiag, p4 must not be above maindiag
        assert(key.kingsOnMainDiag1() && key.p2OnMainDiag1() && key.p3OnMainDiag1());
        for(int pi4 = 0; pi4 < ARRAYSIZE(s_subDiagIndexToPos); pi4++) {
          const int pos4 = s_subDiagIndexToPos[pi4];
          if(key.isOccupied(pos4)) continue;
          if(IS_ONMAINDIAG1(pos4) && (pos4 <= pos3)) continue;
          key.setPosition(4,pos4);
          scanPositions(key, 5, IS_ONMAINDIAG1(pos4));
          key.clearField(pos4);
        }
      } else if(!key.kingsOnMainDiag1() || !key.p2OnMainDiag1()) {   // kings or p2 off maindiag, p4 = [pos3+1..63]
        for(int pos4 = pos3+1; pos4 < 64; pos4++) {
          if(key.isOccupied(pos4)) continue;
          key.setPosition(4,pos4);
          scanPositions(key, 5, false);
          key.clearField(pos4);
        }
      } else {                               // kings,p2 on maindiag, p3 off maindiag => p4 must be off maindiag and "above" p3
        assert(key.kingsOnMainDiag1() && key.p2OnMainDiag1() && !IS_ONMAINDIAG1(pos3));
        const int pi3 = s_offDiagPosToIndex[pos3];
        for(int pi4 = pi3+1; pi4 < ARRAYSIZE(s_offDiagIndexToPos); pi4++) {
          const int pos4 = s_offDiagIndexToPos[pi4];
          if(key.isOccupied(pos4)) continue;
          if((pi4 >= 28) && (pi4 - 28 >= pi3)) continue;
          key.setPosition(4,pos4);
          scanPositions(key, 5, false);
          key.clearField(pos4);
        }
      }
    }
    break;
  case 5:
    { const int pos4 = key.getPosition(4);
      if(allPreviousOnDiag) {              // kings, p2, p3, p4 on maindag => p5 must not be above maindiag
        assert(key.kingsOnMainDiag1() && key.p2OnMainDiag1() && key.p3OnMainDiag1() && key.p4OnMainDiag1());
        for(int pi5 = 0; pi5 < ARRAYSIZE(s_subDiagIndexToPos); pi5++) {
          const int pos5 = s_subDiagIndexToPos[pi5];
          if(key.isOccupied(pos5)) continue;
          if(IS_ONMAINDIAG1(pos5) && (pos5 <= pos4)) continue;
          key.setPosition(5,pos5);
          checkForBothPlayers(key);
          key.clearField(pos5);
        }
      } else if(!key.kingsOnMainDiag1() || !key.p2OnMainDiag1() ) { // kings or p2 off maindiag => pos5 = [pos4+1..63]
        for(int pos5 = pos4+1; pos5 < 64; pos5++) {
          if(key.isOccupied(pos5)) continue;
          key.setPosition(5,pos5);
          checkForBothPlayers(key);
          key.clearField(pos5);
        }
      } else if(key.p3OnMainDiag1()) {     // kings,p2,p3 on maindiag, p4 below maindiag => p5 must be off maindiag and "above" p4
        assert(IS_BELOWMAINDIAG1(pos4));
        const int pi4 = s_offDiagPosToIndex[pos4];
        for(int pi5 = pi4+1; pi5 < ARRAYSIZE(s_offDiagIndexToPos); pi5++) {
          const int pos5 = s_offDiagIndexToPos[pi5];
          if(key.isOccupied(pos5)) continue;
          if((pi5 >= 28) && (pi5 - 28 < pi4)) continue;
          key.setPosition(5,pos5);
          checkForBothPlayers(key);
          key.clearField(pos5);
        }
      } else {                             // kings,p2 on maindiag, p3 off diag => p5 must be off maindiag and "above" p3 and p4
        assert(key.kingsOnMainDiag1() && key.p2OnMainDiag1() && !key.p3OnMainDiag1());
        const int pos3 = key.getPosition(3);
        const int pi3  = s_offDiagPosToIndex[pos3];
        const int pi4  = s_offDiagPosToIndex[pos4];
        for(int pi5 = 0; pi5 < ARRAYSIZE(s_offDiagIndexToPos); pi5++) {
          const int pos5 = s_offDiagIndexToPos[pi5];
          if(key.isOccupied(pos5)) continue;
          switch(BOOL3MASK(IS_BELOWMAINDIAG1, pos3, pos4, pos5)) {
          case 0: continue; // none  below  3,4,5 above. skip
          case 1:           // 3     below    4,5 above. => pi3 > max(mirrorDiag1(pi4), mirrorDiag1(pi4)) and
            if(pos5 < pos4) { // Must have pos5 > pos4
              continue;
            }
            if(pi3 <= max(pi4, pi5) - 28) continue;
            break;
          case 2: continue; //   4   below, 3,  5 above. skip
          case 3:           // 3,4   below,     5 above. => pi4 > pi3. must have mirrorDiag1(pi5) <= pi4 (=max(pi3, pi4))
            assert(pi4 > pi3);
            if(pi5 - 28 > pi4) continue;
            break;

          case 4: continue; //     5 below, 3,4   above. skip
          case 5: continue; // 3  ,5 below,   4   above. skip
          case 6: continue; //   4,5 below, 3     above. skip
          case 7:           // 3,4,5 below. Must have pos3 < pos4 < pos5
            if((pos4 <= pos3) || (pos5 <= pos4)) continue;
            break;
          }
          key.setPosition(5,pos5);
          checkForBothPlayers(key);
          key.clearField(pos5);
        }
      }
    }
    break;

  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("pIndex=%d"), pIndex);
  }
}

void EndGameKeyDefinition6Men3Equal::selfCheck(EndGameKeyWithOccupiedPositions &key) const {
  sym8PositionScanner(key, 0, true, (PositionScanner)&EndGameKeyDefinition6Men3Equal::scanPositions);
}

bool EndGameKeyDefinition6Men3Equal::keysEqual(EndGameKey key1, EndGameKey key2) const {
                     if(key2 == key1) return true;
  key2.swapPos(3,4); if(key2 == key1) return true;
  key2.swapPos(4,5); if(key2 == key1) return true;
  key2.swapPos(3,4); if(key2 == key1) return true;
  key2.swapPos(4,5); if(key2 == key1) return true;
  key2.swapPos(3,4); if(key2 == key1) return true;
  return false;
}

String EndGameKeyDefinition6Men3Equal::getCodecName() const {
  return _T("6Men3Equal");
}

#endif // TABLEBASE_BUILDER
