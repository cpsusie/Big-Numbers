#include "stdafx.h"
#include "EndGameUtil.h"
#include "EndGameKeyCodec.h"

#define MININDEX 0

static EndGamePosIndex rangeStartKKOffDiag[60];
static EndGamePosIndex rangeStartKKOnDiag[27];

#define KK_OFFDIAG_POSCOUNT    (2*KK_OFFDIAG_COUNT)
#define KK_ONDIAG_POSCOUNT     (2*KK_ONDIAG_COUNT )

#define KK_ONDIAG_1_BELOWDIAG_POSCOUNT           GET_RANGESTART3EQUAL(KK_ONDIAG_POSCOUNT, 26)
#define KK_ONDIAG_2_BELOWDIAG_POSCOUNT           GET_RANGESTART3EQUAL(KK_ONDIAG_POSCOUNT, 27)
#define KK_ONDIAG_3_BELOWDIAG_POSCOUNT           KK_ONDIAG_1_BELOWDIAG_POSCOUNT

#define KKP2_ONDIAG_POSCOUNT                     (MAXINDEX_KKP2_ONDIAG_3MEN  - MININDEX_KKP2_ONDIAG_3MEN)
#define KKP2_ONDIAG_P34_BELOWDIAG_POSCOUNT        GET_RANGESTART2EQUAL(KKP2_ONDIAG_POSCOUNT, 27)
#define KKP2_ONDIAG_P3_BELOWDIAG_POSCOUNT         GET_RANGESTART2EQUAL(KKP2_ONDIAG_POSCOUNT, 28)
#define KKP34_ONDIAG_POSCOUNT                    (MAXINDEX_KK_ONDIAG_3MEN    - MININDEX_KK_ONDIAG_3MEN  )

#define START_RANGE_KK_ONDIAG_P4_BELOWDIAG       (KK_OFFDIAG_POSCOUNT * LENGTH_RANGETABLE3EQUAL(rangeStartKKOffDiag)                      )
#define START_RANGE_KK_ONDIAG_P24_BELOWDIAG      (START_RANGE_KK_ONDIAG_P4_BELOWDIAG       + KK_ONDIAG_1_BELOWDIAG_POSCOUNT               )
#define START_RANGE_KK_ONDIAG_P34_BELOWDIAG      (START_RANGE_KK_ONDIAG_P24_BELOWDIAG      + KK_ONDIAG_2_BELOWDIAG_POSCOUNT               )
#define START_RANGE_KK_ONDIAG_P234_BELOWDIAG     (START_RANGE_KK_ONDIAG_P34_BELOWDIAG      + KK_ONDIAG_2_BELOWDIAG_POSCOUNT               )
#define START_RANGE_KKP2_ONDIAG_P34_BELOWDIAG    (START_RANGE_KK_ONDIAG_P234_BELOWDIAG     + KK_ONDIAG_3_BELOWDIAG_POSCOUNT               )
#define START_RANGE_KKP2_ONDIAG_P3_BELOWDIAG     (START_RANGE_KKP2_ONDIAG_P34_BELOWDIAG    + KKP2_ONDIAG_P34_BELOWDIAG_POSCOUNT           )
#define START_RANGE_KKP34_ONDIAG                 (START_RANGE_KKP2_ONDIAG_P3_BELOWDIAG     + KKP2_ONDIAG_P3_BELOWDIAG_POSCOUNT            )
#define START_RANGE_KKP234_ONDIAG                (START_RANGE_KKP34_ONDIAG                 + GET_RANGESTART2EQUAL(KKP34_ONDIAG_POSCOUNT,5))

EndGameKeyDefinition5Men3Equal::EndGameKeyDefinition5Men3Equal(PieceKey pk234)
: EndGameKeyDefinitionDupletsAllowed(pk234, pk234)
{
  assert(GET_TYPE_FROMKEY(pk234) != Pawn);  // for pk234 = Pawn  : use EndGameKeyDefinition5Men3EqualPawns

  static bool initDone = false;
  if(initDone) return;

  initDone = true;

  INIT_RANGETABLE3EQUAL(rangeStartKKOffDiag , KK_OFFDIAG_POSCOUNT);
  INIT_RANGETABLE3EQUAL(rangeStartKKOnDiag  , KK_ONDIAG_POSCOUNT );

/*
  _tprintf(_T("Constants for %s\n"), getCodecName().cstr());
  DUMP_MACRO( KK_OFFDIAG_POSCOUNT                      );
  DUMP_MACRO( KK_ONDIAG_POSCOUNT                       );
  DUMP_MACRO( KK_ONDIAG_1_BELOWDIAG_POSCOUNT           );
  DUMP_MACRO( KK_ONDIAG_2_BELOWDIAG_POSCOUNT           );
  DUMP_MACRO( KK_ONDIAG_3_BELOWDIAG_POSCOUNT           );
  DUMP_MACRO( KKP2_ONDIAG_P34_BELOWDIAG_POSCOUNT       );
  DUMP_MACRO( KKP2_ONDIAG_P3_BELOWDIAG_POSCOUNT        );
  DUMP_MACRO( KKP34_ONDIAG_POSCOUNT                    );

  DUMP_MACRO( START_RANGE_KK_ONDIAG_P4_BELOWDIAG       );
  DUMP_MACRO( START_RANGE_KK_ONDIAG_P24_BELOWDIAG      );
  DUMP_MACRO( START_RANGE_KK_ONDIAG_P34_BELOWDIAG      );
  DUMP_MACRO( START_RANGE_KK_ONDIAG_P234_BELOWDIAG     );
  DUMP_MACRO( START_RANGE_KKP2_ONDIAG_P34_BELOWDIAG    );
  DUMP_MACRO( START_RANGE_KKP2_ONDIAG_P3_BELOWDIAG     );
  DUMP_MACRO( START_RANGE_KKP34_ONDIAG                 );
  DUMP_MACRO( START_RANGE_KKP234_ONDIAG                );

  DUMP_RANGETABLE(rangeStartKKOffDiag   );
  DUMP_RANGETABLE(rangeStartKKOnDiag    );
*/
}

#ifdef _DEBUG

#define ENCODE_KK_OFFDIAG(          key)          return encode_kk_offdiag5m3e(          key)
#define DECODE_KK_OFFDIAG(          key, index  )        decode_kk_offdiag5m3e(          key, index)
#define ENCODE_KK_ONDIAG_NOFLIP(    key)          return encode_kk_ondiag_noflip5m3e(    key)
#define DECODE_KK_ONDIAG_NOFLIP(    key, index  )        decode_kk_ondiag_noflip5m3e(    key, index)
#define ENCODE_KK_ONDIAG_FLIPi(     key, i, j, k) return encode_kk_ondiag_flipi5m3e(     key, i, j, k)
#define DECODE_KK_ONDIAG_FLIPi(     key, index  )        decode_kk_ondiag_flipi5m3e(     key, index)
#define DECODE_KK_ONDIAG_FLIPj(     key, index  )        decode_kk_ondiag_flipj5m3e(     key, index)
#define ENCODE_KK_ONDIAG_FLIPij(    key, i, j, k) return encode_kk_ondiag_flipij5m3e(    key, i, j, k)
#define DECODE_KK_ONDIAG_FLIPij(    key, index  )        decode_kk_ondiag_flipij5m3e(    key, index)
#define ENCODE_KKP2_ONDIAG_NOFLIP(  key, i, j)    return encode_kkp2_ondiag_noflip5m3e(  key, i, j)
#define DECODE_KKP2_ONDIAG_NOFLIP(  key, index  )        decode_kkp2_ondiag_noflip5m3e(  key, index)
#define ENCODE_KKP2_ONDIAG_FLIPi(   key, i, j)    return encode_kkp2_ondiag_flipi5m3e(   key, i, j)
#define DECODE_KKP2_ONDIAG_FLIPi(   key, index  )        decode_kkp2_ondiag_flipi5m3e(   key, index)
#define ENCODE_KKP2_ONDIAG_NOSWAP(  key, i, j)    return encode_kkp2_ondiag_noswap5m3e(  key, i, j)
#define DECODE_KKP2_ONDIAG_NOSWAP(  key, index  )        decode_kkp2_ondiag_noswap5m3e(  key, index)
#define ENCODE_KKP2_ONDIAG_PRESWAPi(key, i, j)    return encode_kkp2_ondiag_preswapi5m3e(key, i, j)
#define ENCODE_KKP34_ONDIAG_NOSWAP( key)          return encode_kkp34_ondiag_noswap5m3e( key)
#define DECODE_KKP34_ONDIAG_NOSWAP( key, index  )        decode_kkp34_ondiag_noswap5m3e( key, index)
#define ENCODE_KKP23_ONDIAG_PRESWAP(key)          return encode_kkp23_ondiag_preswap5m3e(key)
#define ENCODE_KKP24_ONDIAG_PRESWAP(key)          return encode_kkp24_ondiag_preswap5m3e(key)
#define ENCODE_KKP34_ONDIAG_PRESWAP(key)          return encode_kkp34_ondiag_preswap5m3e(key)
#define ENCODE_KKP234_ONDIAG(       key)          return encode_kkp234_ondiag5m3e(       key)
#define DECODE_KKP234_ONDIAG(       key, index)          decode_kkp234_ondiag5m3e(       key, index)

static EndGamePosIndex encode_kk_offdiag5m3e(EndGameKey key) {
  key.sort3Pos(2,3,4);
  const UINT pi2 = key.getP2OffDiagIndex();
  const UINT pi3 = key.getP3OffDiagIndexEqualP23();
  const UINT pi4 = key.getP4OffDiagIndexEqualP234();
  return ADDPIT(key, ADD3EQUAL(KK_OFFDIAG_2MEN(key), KK_OFFDIAG_POSCOUNT, pi2, pi3, pi4))         // Use SET3POS3EQUAL to decode
       - MININDEX;
}

static void decode_kk_offdiag5m3e(EndGameKey &key, EndGamePosIndex index) {
  SETPIT(              key, index  );
  SET3POS3EQUAL(       key, index, rangeStartKKOffDiag, KK_OFFDIAG_POSCOUNT, 2, 3, 4);
  SETKK_OFFDIAG(       key, index  );
  key.p234IndexToOffDiagPosEqualP234();
}

static EndGamePosIndex encode_kk_ondiag_noflip5m3e(EndGameKey key) {
  UINT pi2 = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(2)];
  UINT pi3 = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(3)];
  UINT pi4 = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(4)];
  SORT3(pi2, pi3, pi4); /* pi2 < pi3 < pi4 */
  return ADDPIT(key, ADD3EQUAL(KK_ONDIAG_2MEN(key), KK_ONDIAG_POSCOUNT, pi2, pi3, pi4))
       + START_RANGE_KK_ONDIAG_P234_BELOWDIAG
       - MININDEX;
}

static void decode_kk_ondiag_noflip5m3e(EndGameKey &key, EndGamePosIndex index) {
  index -= START_RANGE_KK_ONDIAG_P234_BELOWDIAG;
  SETPIT(              key, index   );
  SET3OFFDIAGPOSNOFLIP(key, index, rangeStartKKOnDiag, KK_ONDIAG_POSCOUNT, 2, 3, 4);
  SETKK_ONDIAG(        key, index   );
}

static EndGamePosIndex encode_kk_ondiag_flipi5m3e(EndGameKey key, UINT i, UINT j, UINT k) {
  UINT pi = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(i)] - 28;
  UINT pj = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(j)];
  UINT pk = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(k)];
  SORT2(pj, pk);

  if(pi <= pj) { // pi <= pj < pk
    return ADDPIT(key, ADD3EQUALALLOWEQUALLM(KK_ONDIAG_2MEN(key), KK_ONDIAG_POSCOUNT, pi, pj, pk)) // Use SET3OFFDIAGPOSFLIPi to decode
         + START_RANGE_KK_ONDIAG_P34_BELOWDIAG
         - MININDEX;
  } else {       // pj < pi <= pk
    return ADDPIT(key, ADD3EQUALALLOWEQUALHM(KK_ONDIAG_2MEN(key), KK_ONDIAG_POSCOUNT, pj, pi, pk)) // Use SET3OFFDIAGPOSFLIPj to decode
         + START_RANGE_KK_ONDIAG_P24_BELOWDIAG
         - MININDEX;
  }
}

static void decode_kk_ondiag_flipi5m3e(EndGameKey &key, EndGamePosIndex index) {
  index -= START_RANGE_KK_ONDIAG_P34_BELOWDIAG;
  SETPIT(              key, index  );
  SET3OFFDIAGPOSFLIPi( key, index, rangeStartKKOnDiag, KK_ONDIAG_POSCOUNT, 2, 3, 4);
  SETKK_ONDIAG(        key, index  );
}

static void decode_kk_ondiag_flipj5m3e(EndGameKey &key, EndGamePosIndex index) {
  index -= START_RANGE_KK_ONDIAG_P24_BELOWDIAG;
  SETPIT(              key, index  );
  SET3OFFDIAGPOSFLIPj( key, index, rangeStartKKOnDiag, KK_ONDIAG_POSCOUNT, 2, 3, 4);
  SETKK_ONDIAG(        key, index  );
}

static EndGamePosIndex encode_kk_ondiag_flipij5m3e(EndGameKey key, int i, int j, int k) {
  UINT pi = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(i)] - 28;
  UINT pj = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(j)] - 28;
  UINT pk = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(k)];
  SORT2(pi, pj);

  // pi < pj < pk
  return ADDPIT(key, ADD3EQUAL(KK_ONDIAG_2MEN(key), KK_ONDIAG_POSCOUNT, pi, pj, pk))               // Use SET3OFFDIAGPOSFLIPij to decode
       + START_RANGE_KK_ONDIAG_P4_BELOWDIAG
       - MININDEX;
}

static void decode_kk_ondiag_flipij5m3e(EndGameKey &key, EndGamePosIndex index) {
  index -= START_RANGE_KK_ONDIAG_P4_BELOWDIAG;
  SETPIT(              key, index  );
  SET3OFFDIAGPOSFLIPij(key, index, rangeStartKKOnDiag, KK_ONDIAG_POSCOUNT, 2, 3, 4);
  SETKK_ONDIAG(        key, index  );
}

static EndGamePosIndex encode_kkp2_ondiag_noflip5m3e(EndGameKey key, int i, int j) {
  UINT pi = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(i)];
  UINT pj = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(j)];
  SORT2(pi, pj);
  return ADDPIT(key, ADD2EQUAL(KKP2_ONDIAG_3MEN(key), KKP2_ONDIAG_POSCOUNT, pi, pj))
       + START_RANGE_KKP2_ONDIAG_P34_BELOWDIAG
       - MININDEX;
}

static void decode_kkp2_ondiag_noflip5m3e(EndGameKey &key, EndGamePosIndex index) {
  index -= START_RANGE_KKP2_ONDIAG_P34_BELOWDIAG;
  SETPIT(              key, index   );
  SET2OFFDIAGPOSNOFLIP(key, index, KKP2_ONDIAG_POSCOUNT, 3, 4);
  SETP2_ONDIAG(        key, index   );
  SETKK_ONDIAG(        key, index   );
  key.p2IndexToDiagPos();
}

static EndGamePosIndex encode_kkp2_ondiag_flipi5m3e(EndGameKey key, int i, int j) {
  UINT pi = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(i)] - 28;
  UINT pj = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(j)];
  SORT2(pi, pj);
  return ADDPIT(key, ADD2EQUALALLOWEQUALLH(KKP2_ONDIAG_3MEN(key), KKP2_ONDIAG_POSCOUNT, pi, pj))
       + START_RANGE_KKP2_ONDIAG_P3_BELOWDIAG
       - MININDEX;
}

static void decode_kkp2_ondiag_flipi5m3e(EndGameKey &key, EndGamePosIndex index) {
  index -= START_RANGE_KKP2_ONDIAG_P3_BELOWDIAG;
  SETPIT(              key, index   );
  SET2OFFDIAGPOSFLIPj( key, index, KKP2_ONDIAG_POSCOUNT, 3, 4);
  SETP2_ONDIAG(        key, index   );
  SETKK_ONDIAG(        key, index   );
  key.p2IndexToDiagPos();
}

static EndGamePosIndex encode_kkp2_ondiag_noswap5m3e(EndGameKey key, int i, int j) {
  const int pos3 = key.getPosition(3);
  const int pos4 = key.getPosition(4);
  switch(BOOL2MASK(IS_ABOVEMAINDIAG1, pos3, pos4)) {
  case 0: ENCODE_KKP2_ONDIAG_NOFLIP(key,3,4);
  case 1: ENCODE_KKP2_ONDIAG_FLIPi( key,3,4);
  case 2: ENCODE_KKP2_ONDIAG_FLIPi( key,4,3);
  }
  throwException(_T("%s:Impossible encoding"), __TFUNCTION__);
  return 0;
}

static EndGamePosIndex encode_kkp2_ondiag_preswapi5m3e(EndGameKey key, int i, int j) {
  key.swapPos(2, i);
  ENCODE_KKP2_ONDIAG_NOSWAP(key,2,j);
}

static EndGamePosIndex encode_kkp34_ondiag_noswap5m3e(EndGameKey key) {
  const UINT pi3 = key.getP3DiagIndexEqualP23();
  const UINT pi4 = key.getP4DiagIndexEqualP234();
  return ADDPIT(key, ADD2EQUAL(KK_ONDIAG_3MEN(key), KKP34_ONDIAG_POSCOUNT, pi3, pi4))
       + START_RANGE_KKP34_ONDIAG
       - MININDEX;
}

static void decode_kkp34_ondiag_noswap5m3e(EndGameKey &key, EndGamePosIndex index) {
  index -= START_RANGE_KKP34_ONDIAG;
  SETPIT(              key, index   );
  SET2POS2EQUAL(       key, index, KKP34_ONDIAG_POSCOUNT, 3, 4);
  SETPOS_BELOWDIAG(    key, index, 2);
  SETKK_ONDIAG(        key, index   );
  key.p34IndexToDiagPosEqualP234();
}

static EndGamePosIndex encode_kkp23_ondiag_preswap5m3e(EndGameKey key) {
  const int pos2 = key.getPosition(2);
  const int pos3 = key.getPosition(3);
  const int pos4 = key.getPosition(4);
  if(pos2 < pos3) {
    key.setPosition(2,pos4);
    key.setPosition(3,pos2);
    key.setPosition(4,pos3);
  } else {
    key.setPosition(2,pos4);
    key.setPosition(4,pos2);
  }
  ENCODE_KKP34_ONDIAG_NOSWAP(key);
}

static EndGamePosIndex encode_kkp24_ondiag_preswap5m3e(EndGameKey key) {
  const int pos2 = key.getPosition(2);
  const int pos3 = key.getPosition(3);
  const int pos4 = key.getPosition(4);
  if(pos2 < pos4) {
    key.setPosition(2,pos3);
    key.setPosition(3,pos2);
  } else {
    key.setPosition(2,pos3);
    key.setPosition(3,pos4);
    key.setPosition(4,pos2);
  }
  ENCODE_KKP34_ONDIAG_NOSWAP(key);
}

static EndGamePosIndex encode_kkp34_ondiag_preswap5m3e(EndGameKey key) {
  key.sort2Pos(3,4);
  ENCODE_KKP34_ONDIAG_NOSWAP(key);
}

static EndGamePosIndex encode_kkp234_ondiag5m3e(EndGameKey key) {
  key.sort3Pos(2,3,4);
  const UINT pi2 = key.getP2DiagIndex();
  const UINT pi3 = key.getP3DiagIndexEqualP23();
  const UINT pi4 = key.getP4DiagIndexEqualP234();
  return ADDPIT(key, ADD3EQUAL(KK_ONDIAG_2MEN(key), KK_ONDIAG_POSCOUNT, pi2, pi3, pi4))
       + START_RANGE_KKP234_ONDIAG
       - MININDEX;
}

static void decode_kkp234_ondiag5m3e(EndGameKey &key, EndGamePosIndex index) {
  index -= START_RANGE_KKP234_ONDIAG;
  SETPIT(              key, index   );
  SET3POS3EQUALa(      key, index, rangeStartKKOnDiag, 6, KK_ONDIAG_POSCOUNT, 2, 3, 4);
  SETKK_ONDIAG(        key, index   );
  key.p234IndexToDiagPosEqualP234();
}

#else // !_DEBUG

#define ENCODE_KK_OFFDIAG(key)                                                                                                    \
{ key.sort3Pos(2,3,4);                                                                                                            \
  const UINT pi2 = key.getP2OffDiagIndex();                                                                                       \
  const UINT pi3 = key.getP3OffDiagIndexEqualP23();                                                                               \
  const UINT pi4 = key.getP4OffDiagIndexEqualP234();                                                                              \
  return ADDPIT(key, ADD3EQUAL(KK_OFFDIAG_2MEN(key), KK_OFFDIAG_POSCOUNT, pi2, pi3, pi4))                                         \
       - MININDEX;                                                                                                                \
}

#define DECODE_KK_OFFDIAG(key, index)                                                                                             \
{ SETPIT(              key, index  );                                                                                             \
  SET3POS3EQUAL(       key, index, rangeStartKKOffDiag, KK_OFFDIAG_POSCOUNT, 2, 3, 4);                                            \
  SETKK_OFFDIAG(       key, index  );                                                                                             \
  key.p234IndexToOffDiagPosEqualP234();                                                                                           \
}

#define ENCODE_KK_ONDIAG_NOFLIP(key)                                                                                              \
{ UINT pi2 = s_offDiagPosToIndex[pos2];                                                                                           \
  UINT pi3 = s_offDiagPosToIndex[pos3];                                                                                           \
  UINT pi4 = s_offDiagPosToIndex[pos4];                                                                                           \
  SORT3(pi2, pi3, pi4); /* pi2 < pi3 < pi4 */                                                                                     \
  return ADDPIT(key, ADD3EQUAL(KK_ONDIAG_2MEN(key), KK_ONDIAG_POSCOUNT, pi2, pi3, pi4))                                           \
       + START_RANGE_KK_ONDIAG_P234_BELOWDIAG                                                                                     \
       - MININDEX;                                                                                                                \
}

#define DECODE_KK_ONDIAG_NOFLIP(key, index)                                                                                       \
{ index -= START_RANGE_KK_ONDIAG_P234_BELOWDIAG;                                                                                  \
  SETPIT(              key, index   );                                                                                            \
  SET3OFFDIAGPOSNOFLIP(key, index, rangeStartKKOnDiag, KK_ONDIAG_POSCOUNT, 2, 3, 4);                                              \
  SETKK_ONDIAG(        key, index   );                                                                                            \
}

#define ENCODE_KK_ONDIAG_FLIPi(key,i,j,k)                                                                                         \
{ UINT pi = s_offDiagPosToIndex[pos##i] - 28;                                                                                     \
  UINT pj = s_offDiagPosToIndex[pos##j];                                                                                          \
  UINT pk = s_offDiagPosToIndex[pos##k];                                                                                          \
  SORT2(pj, pk);                                                                                                                  \
  if(pi <= pj) {                                                                                                                  \
    return ADDPIT(key, ADD3EQUALALLOWEQUALLM(KK_ONDIAG_2MEN(key), KK_ONDIAG_POSCOUNT, pi, pj, pk))                                \
         + START_RANGE_KK_ONDIAG_P34_BELOWDIAG                                                                                    \
         - MININDEX;                                                                                                              \
  } else {                                                                                                                        \
    return ADDPIT(key, ADD3EQUALALLOWEQUALHM(KK_ONDIAG_2MEN(key), KK_ONDIAG_POSCOUNT, pj, pi, pk))                                \
         + START_RANGE_KK_ONDIAG_P24_BELOWDIAG                                                                                    \
         - MININDEX;                                                                                                              \
  }                                                                                                                               \
}

#define DECODE_KK_ONDIAG_FLIPi(key, index)                                                                                        \
{ index -= START_RANGE_KK_ONDIAG_P34_BELOWDIAG;                                                                                   \
  SETPIT(              key, index  );                                                                                             \
  SET3OFFDIAGPOSFLIPi( key, index, rangeStartKKOnDiag, KK_ONDIAG_POSCOUNT, 2, 3, 4);                                              \
  SETKK_ONDIAG(        key, index  );                                                                                             \
}

#define DECODE_KK_ONDIAG_FLIPj(key, index)                                                                                        \
{ index -= START_RANGE_KK_ONDIAG_P24_BELOWDIAG;                                                                                   \
  SETPIT(              key, index  );                                                                                             \
  SET3OFFDIAGPOSFLIPj( key, index, rangeStartKKOnDiag, KK_ONDIAG_POSCOUNT, 2, 3, 4);                                              \
  SETKK_ONDIAG(        key, index  );                                                                                             \
}

#define ENCODE_KK_ONDIAG_FLIPij(key,i,j,k)                                                                                        \
{ UINT pi = s_offDiagPosToIndex[pos##i] - 28;                                                                                     \
  UINT pj = s_offDiagPosToIndex[pos##j] - 28;                                                                                     \
  UINT pk = s_offDiagPosToIndex[pos##k];                                                                                          \
  SORT2(pi, pj);                                                                                                                  \
  return ADDPIT(key, ADD3EQUAL(KK_ONDIAG_2MEN(key), KK_ONDIAG_POSCOUNT, pi, pj, pk))                                              \
       + START_RANGE_KK_ONDIAG_P4_BELOWDIAG                                                                                       \
       - MININDEX;                                                                                                                \
}

#define DECODE_KK_ONDIAG_FLIPij(key, index)                                                                                       \
{ index -= START_RANGE_KK_ONDIAG_P4_BELOWDIAG;                                                                                    \
  SETPIT(              key, index  );                                                                                             \
  SET3OFFDIAGPOSFLIPij(key, index, rangeStartKKOnDiag, KK_ONDIAG_POSCOUNT, 2, 3, 4);                                              \
  SETKK_ONDIAG(        key, index  );                                                                                             \
}

#define ENCODE_KKP2_ONDIAG_NOFLIP(key, i, j)                                                                                      \
{ UINT pi = s_offDiagPosToIndex[pos##i];                                                                                          \
  UINT pj = s_offDiagPosToIndex[pos##j];                                                                                          \
  SORT2(pi, pj);                                                                                                                  \
  return ADDPIT(key, ADD2EQUAL(KKP2_ONDIAG_3MEN(key), KKP2_ONDIAG_POSCOUNT, pi, pj))                                              \
       + START_RANGE_KKP2_ONDIAG_P34_BELOWDIAG                                                                                    \
       - MININDEX;                                                                                                                \
}

#define DECODE_KKP2_ONDIAG_NOFLIP(key, index)                                                                                     \
{ index -= START_RANGE_KKP2_ONDIAG_P34_BELOWDIAG;                                                                                 \
  SETPIT(              key, index   );                                                                                            \
  SET2OFFDIAGPOSNOFLIP(key, index, KKP2_ONDIAG_POSCOUNT, 3, 4);                                                                   \
  SETP2_ONDIAG(        key, index   );                                                                                            \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p2IndexToDiagPos();                                                                                                         \
}

#define ENCODE_KKP2_ONDIAG_FLIPi(key, i, j)                                                                                       \
{ UINT pi = s_offDiagPosToIndex[pos##i] - 28;                                                                                     \
  UINT pj = s_offDiagPosToIndex[pos##j];                                                                                          \
  SORT2(pi, pj);                                                                                                                  \
  return ADDPIT(key, ADD2EQUALALLOWEQUALLH(KKP2_ONDIAG_3MEN(key), KKP2_ONDIAG_POSCOUNT, pi, pj))                                  \
       + START_RANGE_KKP2_ONDIAG_P3_BELOWDIAG                                                                                     \
       - MININDEX;                                                                                                                \
}

#define DECODE_KKP2_ONDIAG_FLIPi(key, index)                                                                                      \
{ index -= START_RANGE_KKP2_ONDIAG_P3_BELOWDIAG;                                                                                  \
  SETPIT(              key, index   );                                                                                            \
  SET2OFFDIAGPOSFLIPj( key, index, KKP2_ONDIAG_POSCOUNT, 3, 4);                                                                   \
  SETP2_ONDIAG(        key, index   );                                                                                            \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p2IndexToDiagPos();                                                                                                         \
}

#define ENCODE_KKP2_ONDIAG_NOSWAP(key, i, j)                                                                                      \
  switch(BOOL2MASK(IS_ABOVEMAINDIAG1, pos##i, pos##j)) {                                                                          \
  case 0: ENCODE_KKP2_ONDIAG_NOFLIP(key,i,j);                                                                                     \
  case 1: ENCODE_KKP2_ONDIAG_FLIPi( key,i,j);                                                                                     \
  case 2: ENCODE_KKP2_ONDIAG_FLIPi( key,j,i);                                                                                     \
  case 3: impossibleEncodingError(key);                                                                                           \
  }

#define ENCODE_KKP2_ONDIAG_PRESWAPi(key,i,j)                                                                                      \
{ key.swapPos(2,i);                                                                                                               \
  ENCODE_KKP2_ONDIAG_NOSWAP(key,2,j);                                                                                             \
}

#define ENCODE_KKP34_ONDIAG_NOSWAP(key)                                                                                           \
{ const UINT pi3 = key.getP3DiagIndexEqualP23();                                                                                  \
  const UINT pi4 = key.getP4DiagIndexEqualP234();                                                                                 \
  return ADDPIT(key, ADD2EQUAL(KK_ONDIAG_3MEN(key), KKP34_ONDIAG_POSCOUNT, pi3, pi4))                                             \
       + START_RANGE_KKP34_ONDIAG                                                                                                 \
       - MININDEX;                                                                                                                \
}

#define DECODE_KKP34_ONDIAG_NOSWAP(key, index)                                                                                    \
{ index -= START_RANGE_KKP34_ONDIAG;                                                                                              \
  SETPIT(              key, index   );                                                                                            \
  SET2POS2EQUAL(       key, index, KKP34_ONDIAG_POSCOUNT, 3, 4);                                                                  \
  SETPOS_BELOWDIAG(    key, index, 2);                                                                                            \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p34IndexToDiagPosEqualP234();                                                                                               \
}

#define ENCODE_KKP23_ONDIAG_PRESWAP(key)                                                                                          \
{ if(pos2 < pos3) {                                                                                                               \
    key.setPosition(2,pos4);                                                                                                       \
    key.setPosition(3,pos2);                                                                                                       \
    key.setPosition(4,pos3);                                                                                                       \
  } else {                                                                                                                        \
    key.setPosition(2,pos4);                                                                                                       \
    key.setPosition(4,pos2);                                                                                                       \
  }                                                                                                                               \
  ENCODE_KKP34_ONDIAG_NOSWAP(key);                                                                                                \
}

#define ENCODE_KKP24_ONDIAG_PRESWAP(key)                                                                                          \
{ if(pos2 < pos4) {                                                                                                               \
    key.setPosition(2,pos3);                                                                                                       \
    key.setPosition(3,pos2);                                                                                                       \
  } else {                                                                                                                        \
    key.setPosition(2,pos3);                                                                                                      \
    key.setPosition(3,pos4);                                                                                                      \
    key.setPosition(4,pos2);                                                                                                      \
  }                                                                                                                               \
  ENCODE_KKP34_ONDIAG_NOSWAP(key);                                                                                                \
}

#define ENCODE_KKP34_ONDIAG_PRESWAP(key)                                                                                          \
{ key.sort2Pos(3,4);                                                                                                              \
  ENCODE_KKP34_ONDIAG_NOSWAP(key);                                                                                                \
}

#define ENCODE_KKP234_ONDIAG(key)                                                                                                 \
{ key.sort3Pos(2,3,4);                                                                                                            \
  const UINT pi2 = key.getP2DiagIndex();                                                                                          \
  const UINT pi3 = key.getP3DiagIndexEqualP23();                                                                                  \
  const UINT pi4 = key.getP4DiagIndexEqualP234();                                                                                 \
  return ADDPIT(key, ADD3EQUAL(KK_ONDIAG_2MEN(key), KK_ONDIAG_POSCOUNT, pi2, pi3, pi4))                                           \
       + START_RANGE_KKP234_ONDIAG                                                                                                \
       - MININDEX;                                                                                                                \
}

#define DECODE_KKP234_ONDIAG(key, index)                                                                                          \
{ index -= START_RANGE_KKP234_ONDIAG;                                                                                             \
  SETPIT(              key, index   );                                                                                            \
  SET3POS3EQUALa(      key, index, rangeStartKKOnDiag, 6, KK_ONDIAG_POSCOUNT, 2, 3, 4);                                           \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p234IndexToDiagPosEqualP234();                                                                                              \
}

#endif // _DEBUG

EndGamePosIndex EndGameKeyDefinition5Men3Equal::keyToIndex(EndGameKey key) const {
  UINT pos2 = key.getPosition(2);
  UINT pos3 = key.getPosition(3);
  UINT pos4 = key.getPosition(4);

  if(!key.kingsOnMainDiag1()) {                                         // kings off maindiag => p2, p3, p4 anywhere
    ENCODE_KK_OFFDIAG(key);
  } else {                                                              // kings on    diag
    switch(BOOL3MASK(IS_OFFMAINDIAG1, pos2, pos3, pos4)) {
    case 0: ENCODE_KKP234_ONDIAG(           key    );                   // 2,3,4 on    diag
    case 1: ENCODE_KKP34_ONDIAG_PRESWAP(    key    );                   //   3,4 on    diag
    case 2: ENCODE_KKP24_ONDIAG_PRESWAP(    key    );                   // 2,  4 on    diag
    case 3: ENCODE_KKP2_ONDIAG_PRESWAPi(    key,4,3);                   //     4 on    diag
    case 4: ENCODE_KKP23_ONDIAG_PRESWAP(    key    );                   // 2,3   on    diag
    case 5: ENCODE_KKP2_ONDIAG_PRESWAPi(    key,3,4);                   //   3   on    diag
    case 6: ENCODE_KKP2_ONDIAG_NOSWAP(      key,3,4);                   // 2     on    diag
    case 7:                                                             // none  on    diag
      switch(BOOL3MASK(IS_ABOVEMAINDIAG1, pos2, pos3, pos4)) {
      case 0: ENCODE_KK_ONDIAG_NOFLIP(key      );                       // 2,3,4 below diag
      case 1: ENCODE_KK_ONDIAG_FLIPi( key,2,3,4);                       //   3,4 below diag
      case 2: ENCODE_KK_ONDIAG_FLIPi( key,3,2,4);                       // 2,  4 below diag
      case 3: ENCODE_KK_ONDIAG_FLIPij(key,2,3,4);                       //     4 below diag
      case 4: ENCODE_KK_ONDIAG_FLIPi( key,4,2,3);                       // 2,3   below diag
      case 5: ENCODE_KK_ONDIAG_FLIPij(key,2,4,3);                       //    3  below diag
      case 6: ENCODE_KK_ONDIAG_FLIPij(key,3,4,2);                       //  2    below diag
      case 7: ;                                                         // none  below diag => error
      }
    }
  }
  impossibleEncodingError(key);
  return 0;
}

EndGameKey EndGameKeyDefinition5Men3Equal::indexToKey(EndGamePosIndex index) const {
  index += MININDEX;

  EndGameKey result;

  if(index < START_RANGE_KK_ONDIAG_P4_BELOWDIAG) {
    DECODE_KK_OFFDIAG(result, index);
  } else if(index < START_RANGE_KK_ONDIAG_P24_BELOWDIAG) {
    DECODE_KK_ONDIAG_FLIPij(result, index);
  } else if(index < START_RANGE_KK_ONDIAG_P34_BELOWDIAG) {
    DECODE_KK_ONDIAG_FLIPj(result, index);
  } else if(index < START_RANGE_KK_ONDIAG_P234_BELOWDIAG) {
    DECODE_KK_ONDIAG_FLIPi(result, index);
  } else if(index < START_RANGE_KKP2_ONDIAG_P34_BELOWDIAG) {
    DECODE_KK_ONDIAG_NOFLIP(result, index);
  } else if(index < START_RANGE_KKP2_ONDIAG_P3_BELOWDIAG) {
    DECODE_KKP2_ONDIAG_NOFLIP(result, index);
  } else if(index < START_RANGE_KKP34_ONDIAG) {
    DECODE_KKP2_ONDIAG_FLIPi(result, index);
  } else if(index < START_RANGE_KKP234_ONDIAG) {
    DECODE_KKP34_ONDIAG_NOSWAP(result, index);
  } else {
    DECODE_KKP234_ONDIAG(result, index);
  }
  return result;
}

SymmetricTransformation EndGameKeyDefinition5Men3Equal::getSymTransformation(EndGameKey key) const {
  return getSym8Transformation5Men3Equal(key);
}

#ifdef TABLEBASE_BUILDER

void EndGameKeyDefinition5Men3Equal::scanPositions(EndGameKeyWithOccupiedPositions &key, int pIndex, bool allPreviousOnDiag) const {
  switch(pIndex) {
  case 2:
    { for(int pos2 = 0; pos2 < 64; pos2++) { // pos2 = [0..63]
        if(key.isOccupied(pos2)) continue;
        key.setPosition(2,pos2);
        scanPositions(key, 3, allPreviousOnDiag && IS_ONMAINDIAG1(pos2));
        key.clearField(pos2);
      }
    }
    break;
  case 3:
    { const int pos2 = key.getPosition(2);
      if(allPreviousOnDiag) {                // kings, p2 on maindiag, p3 must not be above maindiag
        assert(key.kingsOnMainDiag1() && key.p2OnMainDiag1());
        for(int pi3 = 0; pi3 < ARRAYSIZE(s_subDiagIndexToPos); pi3++) {
          const int pos3 = s_subDiagIndexToPos[pi3];
          if(key.isOccupied(pos3)) continue;
          if(IS_ONMAINDIAG1(pos3) && (pos3 <= pos2)) continue;
          key.setPosition(3,pos3);
          scanPositions(key, 4, IS_ONMAINDIAG1(pos3));
          key.clearField(pos3);
        }
      } else if(!key.kingsOnMainDiag1()) {   // kings off maindiag, p3 = [pos2+1..63]
        for(int pos3 = pos2+1; pos3 < 64; pos3++) {
          if(key.isOccupied(pos3)) continue;
          key.setPosition(3,pos3);
          scanPositions(key, 4, false);
          key.clearField(pos3);
        }
      } else {                               // kings on maindiag, p2 off maindiag => p3 must be off maindiag and "above" p2
        assert(key.kingsOnMainDiag1() && !IS_ONMAINDIAG1(pos2));
        const int pi2 = s_offDiagPosToIndex[pos2];
        for(int pi3 = pi2+1; pi3 < ARRAYSIZE(s_offDiagIndexToPos); pi3++) {
          const int pos3 = s_offDiagIndexToPos[pi3];
          if(key.isOccupied(pos3)) continue;
          if((pi3 >= 28) && (pi3 - 28 >= pi2)) continue;
          key.setPosition(3,pos3);
          scanPositions(key, 4, false);
          key.clearField(pos3);
        }
      }
    }
    break;
  case 4:
    { const int pos3 = key.getPosition(3);
      if(allPreviousOnDiag) {              // kings, p2, p3 on maindag => p4 must not be above maindiag
        assert(key.kingsOnMainDiag1() && key.p2OnMainDiag1() && key.p3OnMainDiag1());
        for(int pi4 = 0; pi4 < ARRAYSIZE(s_subDiagIndexToPos); pi4++) {
          const int pos4 = s_subDiagIndexToPos[pi4];
          if(key.isOccupied(pos4)) continue;
          if(IS_ONMAINDIAG1(pos4) && (pos4 <= pos3)) continue;
          key.setPosition(4,pos4);
          checkForBothPlayers(key);
          key.clearField(pos4);
        }
      } else if(!key.kingsOnMainDiag1()) { // kings off maindiag => pos4 = [pos3+1..63]
        for(int pos4 = pos3+1; pos4 < 64; pos4++) {
          if(key.isOccupied(pos4)) continue;
          key.setPosition(4,pos4);
          checkForBothPlayers(key);
          key.clearField(pos4);
        }
      } else if(key.p2OnMainDiag1()) {     // kings,p2 on maindiag, p3 below maindiag => p4 must be off maindiag and "above" p3
        assert(IS_BELOWMAINDIAG1(pos3));
        const int pi3 = s_offDiagPosToIndex[pos3];
        for(int pi4 = pi3+1; pi4 < ARRAYSIZE(s_offDiagIndexToPos); pi4++) {
          const int pos4 = s_offDiagIndexToPos[pi4];
          if(key.isOccupied(pos4)) continue;
          if((pi4 >= 28) && (pi4 - 28 < pi3)) continue;
          key.setPosition(4,pos4);
          checkForBothPlayers(key);
          key.clearField(pos4);
        }
      } else {                             // kings on maindiag, p2,p3 off diag => p4 must be off maindiag and "above" p2 and p3
        assert(key.kingsOnMainDiag1() && !key.p2OnMainDiag1() && !key.p3OnMainDiag1());
        const int pos2 = key.getPosition(2);
        const int pi2  = s_offDiagPosToIndex[pos2];
        const int pi3  = s_offDiagPosToIndex[pos3];
        for(int pi4 = 0; pi4 < ARRAYSIZE(s_offDiagIndexToPos); pi4++) {
          const int pos4 = s_offDiagIndexToPos[pi4];
          if(key.isOccupied(pos4)) continue;
          switch(BOOL3MASK(IS_BELOWMAINDIAG1, pos2, pos3, pos4)) {
          case 0: continue; // none  below  2,3,4 above. skip
          case 1:           // 2     below    3,4 above. => pi2 > max(mirrorDiag1(pi3), mirrorDiag1(pi4)) and
            if(pos4 < pos3) { // Must have pos4 > pos3
              continue;
            }
            if(pi2 <= max(pi3, pi4) - 28) continue;
            break;
          case 2: continue; //   3   below, 2,  4 above. skip
          case 3:           // 2,3   below,     4 above. => pi3 > pi2. must have mirrorDiag1(pi4) <= pi3 (=max(pi2, pi3))
            assert(pi3 > pi2);
            if(pi4 - 28 > pi3) continue;
            break;

          case 4: continue; //     4 below, 2,3   above. skip
          case 5: continue; // 2  ,4 below,   3   above. skip
          case 6: continue; //   3,4 below, 2     above. skip
          case 7:           // 2,3,4 below. Must have pos2 < pos3 < pos4
            if((pos3 <= pos2) || (pos4 <= pos3)) continue;
            break;
          }
          key.setPosition(4,pos4);
          checkForBothPlayers(key);
          key.clearField(pos4);
        }
      }
    }
    break;

  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("pIndex=%d"), pIndex);
  }
}

void EndGameKeyDefinition5Men3Equal::selfCheck(EndGameKeyWithOccupiedPositions &key) const {
  sym8PositionScanner(key, 0, true, (PositionScanner)&EndGameKeyDefinition5Men3Equal::scanPositions);
}

bool EndGameKeyDefinition5Men3Equal::keysEqual(EndGameKey key1, EndGameKey key2) const {
  return keysMatch234Equal(key2,key1);
}

String EndGameKeyDefinition5Men3Equal::getCodecName() const {
  return _T("5Men3Equal");
}

#endif
