#include "stdafx.h"
#include "EndGameUtil.h"
#include "EndGameKeyCodec.h"

#define MININDEX 0

#define KK_OFFDIAG_POSCOUNT                    (MAXINDEX_KK_OFFDIAG_2MEN     - MININDEX_KK_OFFDIAG_2MEN    )
#define KK_ONDIAG_POSCOUNT                     (MAXINDEX_KK_ONDIAG_2MEN      - MININDEX_KK_ONDIAG_2MEN     )
#define KKP23_ONDIAG_POSCOUNT                  (MAXINDEX_KKP23_ONDIAG_4MEN   - MININDEX_KKP23_ONDIAG_4MEN  )
#define KKP234_ONDIAG_POSCOUNT                ((MAXINDEX_KKP234_ONDIAG_5MEN  - MININDEX_KKP234_ONDIAG_5MEN )*14)
#define KKP2345_ONDIAG_POSCOUNT                (MAXINDEX_KKP2345_ONDIAG_6MEN - MININDEX_KKP2345_ONDIAG_6MEN)

#define KK_OFFDIAG_POSCOUNT_23                 GET_RANGESTART2EQUAL(KK_OFFDIAG_POSCOUNT               , 61 )
#define KK_ONDIAG_P23_BELOW_POSCOUNT_23        GET_RANGESTART2EQUAL(KK_ONDIAG_POSCOUNT                , 27 )
#define KK_ONDIAG_P2_BELOW_POSCOUNT_23         GET_RANGESTART2EQUAL(KK_ONDIAG_POSCOUNT                , 28 )
#define KKP2_ONDIAG_POSCOUNT_23                (MAXINDEX_KKP2_ONDIAG_4MEN - MININDEX_KKP2_ONDIAG_4MEN      )
#define KKP23_ONDIAG_4MEN2EQUAL_POSCOUNT       GET_RANGESTART2EQUAL(KK_ONDIAG_POSCOUNT                , 5  )
#define KKP2345_ONDIAG_POSCOUNT_23             GET_RANGESTART2EQUAL(KKP2345_ONDIAG_POSCOUNT           , 4  )

#define KK_OFFDIAG_POSCOUNT_45                 GET_RANGESTART2EQUAL(KK_OFFDIAG_POSCOUNT_23            , 59 )
#define KK_ONDIAG_P23_BELOW_POSCOUNT_45        GET_RANGESTART2EQUAL(KK_ONDIAG_P23_BELOW_POSCOUNT_23   , 59 )
#define KK_ONDIAG_P2_BELOW_POSCOUNT_45         GET_RANGESTART2EQUAL(KK_ONDIAG_P2_BELOW_POSCOUNT_23    , 59 )
#define KKP2_ONDIAG_POSCOUNT_45                GET_RANGESTART2EQUAL(KKP2_ONDIAG_POSCOUNT_23           , 59 )
#define KKP23_ONDIAG_P45_BELOW_POSCOUNT_45     GET_RANGESTART2EQUAL(KKP23_ONDIAG_4MEN2EQUAL_POSCOUNT  , 27 )
#define KKP23_ONDIAG_P4_BELOW_POSCOUNT_45      GET_RANGESTART2EQUAL(KKP23_ONDIAG_4MEN2EQUAL_POSCOUNT  , 28 )

#define START_RANGE_KK_ONDIAG_P23_BELOWDIAG    (                                         KK_OFFDIAG_POSCOUNT_45            )
#define START_RANGE_KK_ONDIAG_P2_BELOWDIAG     (START_RANGE_KK_ONDIAG_P23_BELOWDIAG    + KK_ONDIAG_P23_BELOW_POSCOUNT_45   )
#define START_RANGE_KKP2_ONDIAG                (START_RANGE_KK_ONDIAG_P2_BELOWDIAG     + KK_ONDIAG_P2_BELOW_POSCOUNT_45    )
#define START_RANGE_KKP23_ONDIAG_P45_BELOWDIAG (START_RANGE_KKP2_ONDIAG                + KKP2_ONDIAG_POSCOUNT_45           )
#define START_RANGE_KKP23_ONDIAG_P4_BELOWDIAG  (START_RANGE_KKP23_ONDIAG_P45_BELOWDIAG + KKP23_ONDIAG_P45_BELOW_POSCOUNT_45)
#define START_RANGE_KKP234_ONDIAG              (START_RANGE_KKP23_ONDIAG_P4_BELOWDIAG  + KKP23_ONDIAG_P4_BELOW_POSCOUNT_45 )
#define START_RANGE_KKP2345_ONDIAG             (START_RANGE_KKP234_ONDIAG              + KKP234_ONDIAG_POSCOUNT            )

EndGameKeyDefinition6Men2Pairs::EndGameKeyDefinition6Men2Pairs(PieceKey pk23, PieceKey pk45)
: EndGameKeyDefinitionDupletsAllowed(pk23, pk23, pk45)
{
  assert((GET_PLAYER_FROMKEY(pk23) != GET_PLAYER_FROMKEY(pk45))
      && (GET_TYPE_FROMKEY(  pk23) != Pawn  )
      && (GET_TYPE_FROMKEY(  pk45) != Pawn  )
        );

/*
  DUMP_MACRO(KK_OFFDIAG_POSCOUNT                    );
  DUMP_MACRO(KK_ONDIAG_POSCOUNT                     );
  DUMP_MACRO(KKP23_ONDIAG_POSCOUNT                  );
  DUMP_MACRO(KKP234_ONDIAG_POSCOUNT                 );
  DUMP_MACRO(KKP2345_ONDIAG_POSCOUNT                );

  DUMP_MACRO(KK_OFFDIAG_POSCOUNT_23                 );
  DUMP_MACRO(KK_ONDIAG_P23_BELOW_POSCOUNT_23        );
  DUMP_MACRO(KK_ONDIAG_P2_BELOW_POSCOUNT_23         );
  DUMP_MACRO(KKP2_ONDIAG_POSCOUNT_23                );
  DUMP_MACRO(KKP23_ONDIAG_4MEN2EQUAL_POSCOUNT       );
  DUMP_MACRO(KKP2345_ONDIAG_POSCOUNT_23             );

  DUMP_MACRO(KK_OFFDIAG_POSCOUNT_45                 );
  DUMP_MACRO(KK_ONDIAG_P23_BELOW_POSCOUNT_45        );
  DUMP_MACRO(KK_ONDIAG_P2_BELOW_POSCOUNT_45         );
  DUMP_MACRO(KKP2_ONDIAG_POSCOUNT_45                );
  DUMP_MACRO(KKP23_ONDIAG_P45_BELOW_POSCOUNT_45     );
  DUMP_MACRO(KKP23_ONDIAG_P4_BELOW_POSCOUNT_45      );

  DUMP_MACRO(START_RANGE_KK_ONDIAG_P23_BELOWDIAG    );
  DUMP_MACRO(START_RANGE_KK_ONDIAG_P2_BELOWDIAG     );
  DUMP_MACRO(START_RANGE_KKP2_ONDIAG                );
  DUMP_MACRO(START_RANGE_KKP23_ONDIAG_P45_BELOWDIAG );
  DUMP_MACRO(START_RANGE_KKP23_ONDIAG_P4_BELOWDIAG  );
  DUMP_MACRO(START_RANGE_KKP234_ONDIAG              );
  DUMP_MACRO(START_RANGE_KKP2345_ONDIAG             );
*/
}

#ifdef _DEBUG

static EndGamePosIndex checkAndReturn(EndGamePosIndex index, EndGamePosIndex max) {
  max--;
  if (index > max) {
    int fisk = 1;
  }
  return index;
}

#define RETURN(index, max) return checkAndReturn(index, max)

static EndGamePosIndex encodeKKOffDiag(EndGameKey key) {
  key.sort2Pos(2, 3);
  key.sort2Pos(4, 5);
  const UINT pi2 = key.getP2OffDiagIndex();
  const UINT pi3 = key.getP3OffDiagIndexEqualP23();
  const UINT pi4 = key.getP4OffDiagIndex();
  const UINT pi5 = key.getP5OffDiagIndexEqualP45();
  RETURN( ADDPIT(key, ADD2EQUAL(ADD2EQUAL(KK_OFFDIAG_2MEN(key), KK_OFFDIAG_POSCOUNT   , pi2, pi3)
                                                              , KK_OFFDIAG_POSCOUNT_23, pi4, pi5))
        - MININDEX
         ,START_RANGE_KK_ONDIAG_P23_BELOWDIAG);
}

static void decodeKKOffDiag(EndGameKey &key, EndGamePosIndex index) {
  SETPIT(              key, index   );
  SET2POS2EQUAL(       key, index, KK_OFFDIAG_POSCOUNT_23, 4, 5);
  SET2POS2EQUAL(       key, index, KK_OFFDIAG_POSCOUNT   , 2, 3);
  SETKK_OFFDIAG(       key, index   );
  key.p23IndexToOffDiagPosEqualP23();
  key.p45IndexToOffDiagPosEqualP45();
}

static EndGamePosIndex encodeKKOnDiagNoFlip(EndGameKey key) {
  UINT pi2 = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(2)];
  UINT pi3 = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(3)];
  SORT2(pi2, pi3); /* pi2 < pi3 */

  key.sort2Pos(4, 5);
  const UINT pi4 = key.getP4OffDiagIndex();
  const UINT pi5 = key.getP5OffDiagIndexEqualP45();
  RETURN(ADDPIT(key, ADD2EQUAL(ADD2EQUAL(KK_ONDIAG_2MEN(key), KK_ONDIAG_POSCOUNT             , pi2, pi3)
                                                            , KK_ONDIAG_P23_BELOW_POSCOUNT_23, pi4, pi5))
       + START_RANGE_KK_ONDIAG_P23_BELOWDIAG
       - MININDEX
      ,START_RANGE_KK_ONDIAG_P2_BELOWDIAG);
}

static void decodeKKOnDiagNoFlip(EndGameKey &key, EndGamePosIndex index) {
  index -= START_RANGE_KK_ONDIAG_P23_BELOWDIAG;
  SETPIT(              key, index   );
  SET2POS2EQUAL(       key, index, KK_ONDIAG_P23_BELOW_POSCOUNT_23, 4, 5);
  SET2OFFDIAGPOSNOFLIP(key, index, KK_ONDIAG_POSCOUNT             , 2, 3);
  SETKK_ONDIAG(        key, index   );
  key.p45IndexToOffDiagPosEqualP45();
}

static EndGamePosIndex encodeKKOnDiagFlipi(EndGameKey key, int i, int j) {
  UINT pi = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(i)] - 28;
  UINT pj = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(j)];
  SORT2(pi, pj);
  key.sort2Pos(4, 5);
  const UINT pi4 = key.getP4OffDiagIndex();
  const UINT pi5 = key.getP5OffDiagIndexEqualP45();
  RETURN( ADDPIT(key, ADD2EQUAL(ADD2EQUALALLOWEQUALLH(KK_ONDIAG_2MEN(key), KK_ONDIAG_POSCOUNT            , pi , pj )
                                                                         , KK_ONDIAG_P2_BELOW_POSCOUNT_23, pi4, pi5))
        + START_RANGE_KK_ONDIAG_P2_BELOWDIAG
        - MININDEX
        , START_RANGE_KKP2_ONDIAG);
}

static void decodeKKOnDiagFlipi(EndGameKey &key, EndGamePosIndex index) {
  index -= START_RANGE_KK_ONDIAG_P2_BELOWDIAG;
  SETPIT(              key, index   );
  SET2POS2EQUAL(       key, index, KK_ONDIAG_P2_BELOW_POSCOUNT_23, 4, 5);
  SET2OFFDIAGPOSFLIPj( key, index, KK_ONDIAG_POSCOUNT            , 2, 3);
  SETKK_ONDIAG(        key, index   );
  key.p45IndexToOffDiagPosEqualP45();
}

static EndGamePosIndex encodeKKP2OnDiag(EndGameKey key) {
  key.sort2Pos(4, 5);
  const UINT pi4 = key.getP4OffDiagIndex();
  const UINT pi5 = key.getP5OffDiagIndexEqualP45();
  RETURN(ADDPIT(key, ADD2EQUAL(KKP2_ONDIAG_4MEN(key), KKP2_ONDIAG_POSCOUNT_23, pi4, pi5))
       + START_RANGE_KKP2_ONDIAG
       - MININDEX
       , START_RANGE_KKP23_ONDIAG_P45_BELOWDIAG );
}

static EndGamePosIndex encodeKKP3OnDiag(EndGameKey key) {
  key.swapPos(2, 3);
  return encodeKKP2OnDiag(key);
}

static void decodeKKP2OnDiag(EndGameKey &key, EndGamePosIndex index) {
  index -= START_RANGE_KKP2_ONDIAG;
  SETPIT(              key, index   );
  SET2POS2EQUAL(       key, index, KKP2_ONDIAG_POSCOUNT_23, 4, 5);
  SETPOS_BELOWDIAG(    key, index, 3);
  SETP2_ONDIAG(        key, index   );
  SETKK_ONDIAG(        key, index   );
  key.p2IndexToDiagPos();
  key.p45IndexToOffDiagPosEqualP45();
}

static EndGamePosIndex encodeKKP23OnDiagNoFlip(EndGameKey key) {
  key.sort2Pos(2, 3);
  const UINT pi2 = key.getP2DiagIndex();
  const UINT pi3 = key.getP3DiagIndexEqualP23();
  UINT pi4 = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(4)];
  UINT pi5 = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(5)];
  SORT2(pi4, pi5); /* pi4 < pi5 */
  RETURN(ADDPIT(key, ADD2EQUAL(ADD2EQUAL(KK_ONDIAG_2MEN(key), KK_ONDIAG_POSCOUNT                , pi2, pi3)
                                                            , KKP23_ONDIAG_4MEN2EQUAL_POSCOUNT, pi4, pi5))
       + START_RANGE_KKP23_ONDIAG_P45_BELOWDIAG
       - MININDEX
       , START_RANGE_KKP23_ONDIAG_P4_BELOWDIAG);
}

static void decodeKKP23OnDiagNoFlip(EndGameKey &key, EndGamePosIndex index) {
  index -= START_RANGE_KKP23_ONDIAG_P45_BELOWDIAG;
  SETPIT(              key, index   );
  SET2OFFDIAGPOSNOFLIP(key, index, KKP23_ONDIAG_4MEN2EQUAL_POSCOUNT, 4, 5);
  SET2POS2EQUAL(       key, index, KK_ONDIAG_POSCOUNT                , 2, 3);
  SETKK_ONDIAG(        key, index   );
  key.p23IndexToDiagPosEqualP23();
}

static EndGamePosIndex encodeKKP23OnDiagFlipi(EndGameKey key, int i, int j) {
  key.sort2Pos(2, 3);
  const UINT pi2 = key.getP2DiagIndex();
  const UINT pi3 = key.getP3DiagIndexEqualP23();
  UINT pi = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(i)] - 28;
  UINT pj = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(j)];
  SORT2(pi, pj);
  RETURN( ADDPIT(key, ADD2EQUALALLOWEQUALLH(ADD2EQUAL(KK_ONDIAG_2MEN(key), KK_ONDIAG_POSCOUNT               , pi2, pi3)
                                                                         , KKP23_ONDIAG_4MEN2EQUAL_POSCOUNT, pi , pj))
        + START_RANGE_KKP23_ONDIAG_P4_BELOWDIAG
        - MININDEX
        ,START_RANGE_KKP234_ONDIAG);
}

static void decodeKKP23OnDiagFlipi(EndGameKey &key, EndGamePosIndex index) {
  index -= START_RANGE_KKP23_ONDIAG_P4_BELOWDIAG;
  SETPIT(              key, index   );
  SET2OFFDIAGPOSFLIPj( key, index, KKP23_ONDIAG_4MEN2EQUAL_POSCOUNT, 4, 5);
  SET2POS2EQUAL(       key, index, KK_ONDIAG_POSCOUNT               , 2, 3);
  SETKK_ONDIAG(        key, index   );
  key.p23IndexToDiagPosEqualP23();
}

static EndGamePosIndex encodeKKP234OnDiag(EndGameKey key) {
  key.sort2Pos(2, 3);

  const UINT pi2 = key.getP2DiagIndex();
  const UINT pi3 = key.getP3DiagIndexEqualP23();
  const UINT pi4 = key.getP4DiagIndex();
  const UINT pi5 = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(5)];

  RETURN( ADDPIT(key, ADDPOS_BELOWDIAG(ADDP4_ONDIAG(
                         ADD2EQUAL(KK_ONDIAG_2MEN(key), KK_ONDIAG_POSCOUNT, pi2, pi3)
                        ,pi4
                       )
                      ,pi5
                     )
               )
         + START_RANGE_KKP234_ONDIAG
         - MININDEX
         , START_RANGE_KKP2345_ONDIAG);
}

static EndGamePosIndex encodeKKP235OnDiag(EndGameKey key) {
  key.swapPos(4, 5);
  return encodeKKP234OnDiag(key);
}

static void decodeKKP234OnDiag(EndGameKey &key, EndGamePosIndex index) {
  index -= START_RANGE_KKP234_ONDIAG;
  SETPIT(              key, index   );
  SETPOS_BELOWDIAG(    key, index, 5);
  SETP4_ONDIAG(        key, index   );
  SET2POS2EQUAL(       key, index, KK_ONDIAG_POSCOUNT                , 2, 3);
  SETKK_ONDIAG(        key, index   );
  key.p23IndexToDiagPosEqualP23();
  key.p4IndexToDiagPos();
}

static EndGamePosIndex encodeKKP2345OnDiag(EndGameKey key) {
  key.sort2Pos(2, 3);
  key.sort2Pos(4, 5);

  const UINT pi2 = key.getP2DiagIndex();
  const UINT pi3 = key.getP3DiagIndexEqualP23();
  const UINT pi4 = key.getP4DiagIndex();
  const UINT pi5 = key.getP5DiagIndexEqualP45();
  return ADDPIT(key, ADD2EQUAL(ADD2EQUAL(KK_ONDIAG_2MEN(key), KK_ONDIAG_POSCOUNT              , pi2, pi3)
                                                            , KKP23_ONDIAG_4MEN2EQUAL_POSCOUNT, pi4, pi5))
       + START_RANGE_KKP2345_ONDIAG
       - MININDEX;
}

static void decodeKKP2345OnDiag(EndGameKey &key, EndGamePosIndex index) {
  index -= START_RANGE_KKP2345_ONDIAG;
  SETPIT(              key, index   );
  SET2POS2EQUAL(       key, index, KKP23_ONDIAG_4MEN2EQUAL_POSCOUNT, 4, 5);
  SET2POS2EQUAL(       key, index, KK_ONDIAG_POSCOUNT              , 2, 3);
  SETKK_ONDIAG(        key, index   );
  key.p23IndexToDiagPosEqualP23();
  key.p45IndexToDiagPosEqualP45();
}

#define ENCODE_KK_OFFDIAG(         key       )  return encodeKKOffDiag(        key       )
#define DECODE_KK_OFFDIAG(         key, index)  decodeKKOffDiag(               key, index);
#define ENCODE_KK_ONDIAG_NOFLIP(   key       )  return encodeKKOnDiagNoFlip(   key       )
#define DECODE_KK_ONDIAG_NOFLIP(   key, index)  decodeKKOnDiagNoFlip(          key, index);
#define ENCODE_KK_ONDIAG_FLIPi(    key, i, j )  return encodeKKOnDiagFlipi(    key, i, j )
#define DECODE_KK_ONDIAG_FLIPi(    key, index)  decodeKKOnDiagFlipi(           key, index);
#define ENCODE_KKP2_ONDIAG(        key       )  return encodeKKP2OnDiag(       key       )
#define ENCODE_KKP3_ONDIAG(        key       )  return encodeKKP3OnDiag(       key       )
#define DECODE_KKP2_ONDIAG(        key, index)  decodeKKP2OnDiag(              key, index);
#define ENCODE_KKP23_ONDIAG_NOFLIP(key       )  return encodeKKP23OnDiagNoFlip(key       )
#define DECODE_KKP23_ONDIAG_NOFLIP(key, index)  decodeKKP23OnDiagNoFlip(       key, index);
#define ENCODE_KKP23_ONDIAG_FLIPi( key, i, j )  return encodeKKP23OnDiagFlipi( key, i, j )
#define DECODE_KKP23_ONDIAG_FLIPi( key, index)  decodeKKP23OnDiagFlipi(        key, index);
#define ENCODE_KKP234_ONDIAG(      key       )  return encodeKKP234OnDiag(     key       )
#define ENCODE_KKP235_ONDIAG(      key       )  return encodeKKP235OnDiag(     key       )
#define DECODE_KKP234_ONDIAG(      key, index)  decodeKKP234OnDiag(            key, index);
#define ENCODE_KKP2345_ONDIAG(     key       )  return encodeKKP2345OnDiag(    key       )
#define DECODE_KKP2345_ONDIAG(     key, index)  decodeKKP2345OnDiag(           key, index);

#else

#define ENCODE_KK_OFFDIAG(key)                                                                                                    \
{ key.sort2Pos(2, 3);                                                                                                             \
  key.sort2Pos(4, 5);                                                                                                             \
  const UINT pi2 = key.getP2OffDiagIndex();                                                                                       \
  const UINT pi3 = key.getP3OffDiagIndexEqualP23();                                                                               \
  const UINT pi4 = key.getP4OffDiagIndex();                                                                                       \
  const UINT pi5 = key.getP5OffDiagIndexEqualP45();                                                                               \
  return ADDPIT(key, ADD2EQUAL(ADD2EQUAL(KK_OFFDIAG_2MEN(key), KK_OFFDIAG_POSCOUNT   , pi2, pi3)                                  \
                                                             , KK_OFFDIAG_POSCOUNT_23, pi4, pi5))                                                                 \
       - MININDEX;                                                                                                                \
}

#define DECODE_KK_OFFDIAG(key, index)                                                                                             \
{ SETPIT(              key, index   );                                                                                            \
  SET2POS2EQUAL(       key, index, KK_OFFDIAG_POSCOUNT_23, 4, 5);                                                                 \
  SET2POS2EQUAL(       key, index, KK_OFFDIAG_POSCOUNT   , 2, 3);                                                                 \
  SETKK_OFFDIAG(       key, index   );                                                                                            \
  key.p23IndexToOffDiagPosEqualP23();                                                                                             \
  key.p45IndexToOffDiagPosEqualP45();                                                                                             \
}

#define ENCODE_KK_ONDIAG_NOFLIP(key)                                                                                              \
{ UINT pi2 = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(2)];                                                       \
  UINT pi3 = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(3)];                                                       \
  SORT2(pi2, pi3); /* pi2 < pi3 */                                                                                                \
  key.sort2Pos(4, 5);                                                                                                             \
  const UINT pi4 = key.getP4OffDiagIndex();                                                                                       \
  const UINT pi5 = key.getP5OffDiagIndexEqualP45();                                                                               \
  return ADDPIT(key, ADD2EQUAL(ADD2EQUAL(KK_ONDIAG_2MEN(key), KK_ONDIAG_POSCOUNT             , pi2, pi3)                          \
                                                            , KK_ONDIAG_P23_BELOW_POSCOUNT_23, pi4, pi5))                         \
       + START_RANGE_KK_ONDIAG_P23_BELOWDIAG                                                                                      \
       - MININDEX;                                                                                                                \
}

#define DECODE_KK_ONDIAG_NOFLIP(key, index)                                                                                       \
{ index -= START_RANGE_KK_ONDIAG_P23_BELOWDIAG;                                                                                   \
  SETPIT(              key, index   );                                                                                            \
  SET2POS2EQUAL(       key, index, KK_ONDIAG_P23_BELOW_POSCOUNT_23, 4, 5);                                                        \
  SET2OFFDIAGPOSNOFLIP(key, index, KK_ONDIAG_POSCOUNT             , 2, 3);                                                        \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p45IndexToOffDiagPosEqualP45();                                                                                             \
}

#define ENCODE_KK_ONDIAG_FLIPi(key, i, j )                                                                                        \
{ UINT pi = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(i)] - 28;                                                   \
  UINT pj = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(j)];                                                        \
  SORT2(pi, pj);                                                                                                                  \
  key.sort2Pos(4, 5);                                                                                                             \
  const UINT pi4 = key.getP4OffDiagIndex();                                                                                       \
  const UINT pi5 = key.getP5OffDiagIndexEqualP45();                                                                               \
  return ADDPIT(key, ADD2EQUAL(ADD2EQUALALLOWEQUALLH(KK_ONDIAG_2MEN(key), KK_ONDIAG_POSCOUNT            , pi , pj )               \
                                                                        , KK_ONDIAG_P2_BELOW_POSCOUNT_23, pi4, pi5))              \
       + START_RANGE_KK_ONDIAG_P2_BELOWDIAG                                                                                       \
       - MININDEX;                                                                                                                \
}

#define DECODE_KK_ONDIAG_FLIPi(key, index)                                                                                        \
{ index -= START_RANGE_KK_ONDIAG_P2_BELOWDIAG;                                                                                    \
  SETPIT(              key, index   );                                                                                            \
  SET2POS2EQUAL(       key, index, KK_ONDIAG_P2_BELOW_POSCOUNT_23, 4, 5);                                                         \
  SET2OFFDIAGPOSFLIPj( key, index, KK_ONDIAG_POSCOUNT            , 2, 3);                                                         \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p45IndexToOffDiagPosEqualP45();                                                                                             \
}

#define ENCODE_KKP2_ONDIAG(key)                                                                                                   \
{ key.sort2Pos(4, 5);                                                                                                             \
  const UINT pi4 = key.getP4OffDiagIndex();                                                                                       \
  const UINT pi5 = key.getP5OffDiagIndexEqualP45();                                                                               \
  return ADDPIT(key, ADD2EQUAL(KKP2_ONDIAG_4MEN(key), KKP2_ONDIAG_POSCOUNT_23, pi4, pi5))                                         \
       + START_RANGE_KKP2_ONDIAG                                                                                                  \
       - MININDEX;                                                                                                                \
}

#define ENCODE_KKP3_ONDIAG(key)                                                                                                   \
{ key.swapPos(2, 3);                                                                                                              \
  ENCODE_KKP2_ONDIAG(key);                                                                                                        \
}

#define DECODE_KKP2_ONDIAG(key, index)                                                                                            \
{ index -= START_RANGE_KKP2_ONDIAG;                                                                                               \
  SETPIT(              key, index   );                                                                                            \
  SET2POS2EQUAL(       key, index, KKP2_ONDIAG_POSCOUNT_23, 4, 5);                                                                \
  SETPOS_BELOWDIAG(    key, index, 3);                                                                                            \
  SETP2_ONDIAG(        key, index   );                                                                                            \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p2IndexToDiagPos();                                                                                                         \
  key.p45IndexToOffDiagPosEqualP45();                                                                                             \
}

#define ENCODE_KKP23_ONDIAG_NOFLIP(key)                                                                                           \
{ key.sort2Pos(2, 3);                                                                                                             \
  const UINT pi2 = key.getP2DiagIndex();                                                                                          \
  const UINT pi3 = key.getP3DiagIndexEqualP23();                                                                                  \
  UINT pi4 = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(4)];                                                       \
  UINT pi5 = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(5)];                                                       \
  SORT2(pi4, pi5); /* pi4 < pi5 */                                                                                                \
  return ADDPIT(key, ADD2EQUAL(ADD2EQUAL(KK_ONDIAG_2MEN(key), KK_ONDIAG_POSCOUNT              , pi2, pi3)                         \
                                                            , KKP23_ONDIAG_4MEN2EQUAL_POSCOUNT, pi4, pi5))                        \
       + START_RANGE_KKP23_ONDIAG_P45_BELOWDIAG                                                                                   \
       - MININDEX;                                                                                                                \
}

#define DECODE_KKP23_ONDIAG_NOFLIP(key, index)                                                                                    \
{ index -= START_RANGE_KKP23_ONDIAG_P45_BELOWDIAG;                                                                                \
  SETPIT(              key, index   );                                                                                            \
  SET2OFFDIAGPOSNOFLIP(key, index, KKP23_ONDIAG_4MEN2EQUAL_POSCOUNT, 4, 5);                                                       \
  SET2POS2EQUAL(       key, index, KK_ONDIAG_POSCOUNT              , 2, 3);                                                       \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p23IndexToDiagPosEqualP23();                                                                                                \
}

#define ENCODE_KKP23_ONDIAG_FLIPi(key, i, j)                                                                                      \
{ key.sort2Pos(2, 3);                                                                                                             \
  const UINT pi2 = key.getP2DiagIndex();                                                                                          \
  const UINT pi3 = key.getP3DiagIndexEqualP23();                                                                                  \
  UINT pi = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(i)] - 28;                                                   \
  UINT pj = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(j)];                                                        \
  SORT2(pi, pj);                                                                                                                  \
  return ADDPIT(key, ADD2EQUALALLOWEQUALLH(ADD2EQUAL(KK_ONDIAG_2MEN(key), KK_ONDIAG_POSCOUNT               , pi2, pi3)            \
                                                                         , KKP23_ONDIAG_4MEN2EQUAL_POSCOUNT, pi , pj))            \
       + START_RANGE_KKP23_ONDIAG_P4_BELOWDIAG                                                                                    \
       - MININDEX;                                                                                                                \
}

#define DECODE_KKP23_ONDIAG_FLIPi(key, index)                                                                                     \
{ index -= START_RANGE_KKP23_ONDIAG_P4_BELOWDIAG;                                                                                 \
  SETPIT(              key, index   );                                                                                            \
  SET2OFFDIAGPOSFLIPj( key, index, KKP23_ONDIAG_4MEN2EQUAL_POSCOUNT, 4, 5);                                                       \
  SET2POS2EQUAL(       key, index, KK_ONDIAG_POSCOUNT              , 2, 3);                                                       \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p23IndexToDiagPosEqualP23();                                                                                                \
}

#define ENCODE_KKP234_ONDIAG(key)                                                                                                 \
{ key.sort2Pos(2, 3);                                                                                                             \
  const UINT pi2 = key.getP2DiagIndex();                                                                                          \
  const UINT pi3 = key.getP3DiagIndexEqualP23();                                                                                  \
  const UINT pi4 = key.getP4DiagIndex();                                                                                          \
  const UINT pi5 = EndGameKeyDefinition::s_offDiagPosToIndex[key.getPosition(5)];                                                 \
  return ADDPIT(key, ADDPOS_BELOWDIAG(ADDP4_ONDIAG(                                                                               \
                         ADD2EQUAL(KK_ONDIAG_2MEN(key), KK_ONDIAG_POSCOUNT, pi2, pi3)                                             \
                        ,pi4                                                                                                      \
                       )                                                                                                          \
                      ,pi5                                                                                                        \
                     )                                                                                                            \
               )                                                                                                                  \
         + START_RANGE_KKP234_ONDIAG                                                                                              \
         - MININDEX;                                                                                                              \
}

#define ENCODE_KKP235_ONDIAG(key)                                                                                                 \
{ key.swapPos(4, 5);                                                                                                              \
  ENCODE_KKP234_ONDIAG(key);                                                                                                      \
}

#define DECODE_KKP234_ONDIAG(key, index)                                                                                          \
{ index -= START_RANGE_KKP234_ONDIAG;                                                                                             \
  SETPIT(              key, index   );                                                                                            \
  SETPOS_BELOWDIAG(    key, index, 5);                                                                                            \
  SETP4_ONDIAG(        key, index   );                                                                                            \
  SET2POS2EQUAL(       key, index, KK_ONDIAG_POSCOUNT                , 2, 3);                                                     \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p23IndexToDiagPosEqualP23();                                                                                                \
  key.p4IndexToDiagPos();                                                                                                         \
}

#define ENCODE_KKP2345_ONDIAG(key)                                                                                                \
{ key.sort2Pos(2, 3);                                                                                                             \
  key.sort2Pos(4, 5);                                                                                                             \
  const UINT pi2 = key.getP2DiagIndex();                                                                                          \
  const UINT pi3 = key.getP3DiagIndexEqualP23();                                                                                  \
  const UINT pi4 = key.getP4DiagIndex();                                                                                          \
  const UINT pi5 = key.getP5DiagIndexEqualP45();                                                                                  \
  return ADDPIT(key, ADD2EQUAL(ADD2EQUAL(KK_ONDIAG_2MEN(key), KK_ONDIAG_POSCOUNT              , pi2, pi3)                         \
                                                            , KKP23_ONDIAG_4MEN2EQUAL_POSCOUNT, pi4, pi5))                        \
       + START_RANGE_KKP2345_ONDIAG                                                                                               \
       - MININDEX;                                                                                                                \
}

#define DECODE_KKP2345_ONDIAG(key, index)                                                                                         \
{ index -= START_RANGE_KKP2345_ONDIAG;                                                                                            \
  SETPIT(              key, index   );                                                                                            \
  SET2POS2EQUAL(       key, index, KKP23_ONDIAG_4MEN2EQUAL_POSCOUNT, 4, 5);                                                       \
  SET2POS2EQUAL(       key, index, KK_ONDIAG_POSCOUNT              , 2, 3);                                                       \
  SETKK_ONDIAG(        key, index   );                                                                                            \
  key.p23IndexToDiagPosEqualP23();                                                                                                \
  key.p45IndexToDiagPosEqualP45();                                                                                                \
}

#endif

EndGamePosIndex EndGameKeyDefinition6Men2Pairs::keyToIndex(EndGameKey key) const {
  if(!key.kingsOnMainDiag1()) {                                // Kings off maindiag => p2, p3, p4, p5 anywhere
    ENCODE_KK_OFFDIAG(key);
  } else {                                                     // Kings on maindiag
    switch(KEYBOOL2MASK(key, IS_OFFMAINDIAG1, 2, 3)) {
    case 0:                                                    // 2,3   on    diag
      switch(KEYBOOL2MASK(key, IS_OFFMAINDIAG1, 4, 5)) {
      case 0: ENCODE_KKP2345_ONDIAG(       key      );         // 4,5   on    diag
      case 1: ENCODE_KKP235_ONDIAG(        key      );         //   5   on    diag
      case 2: ENCODE_KKP234_ONDIAG(        key      );         // 4     on    diag
      case 3:                                                  // 4,5   off   diag
        switch(KEYBOOL2MASK(key, IS_ABOVEMAINDIAG1, 4, 5)) {
        case 0: ENCODE_KKP23_ONDIAG_NOFLIP(key      );         // 4,5   below diag
        case 1: ENCODE_KKP23_ONDIAG_FLIPi( key, 4, 5);         //   5   below diag
        case 2: ENCODE_KKP23_ONDIAG_FLIPi( key, 5, 4);         // 4     below diag
        case 3: ;                                              // none  below diag => error
        }
      }
      break;
    case 1: ENCODE_KKP3_ONDIAG(            key      );         //   3   on    diag
    case 2: ENCODE_KKP2_ONDIAG(            key      );         // 2     on    diag
    case 3:                                                    // none  on    diag
      switch(KEYBOOL2MASK(key, IS_ABOVEMAINDIAG1, 2, 3)) {
      case 0: ENCODE_KK_ONDIAG_NOFLIP(     key      );         // 2,3   below diag
      case 1: ENCODE_KK_ONDIAG_FLIPi(      key, 2, 3);         //   3   below diag
      case 2: ENCODE_KK_ONDIAG_FLIPi(      key, 3, 2);         // 2     below diag
      case 3: ;                                                // none  below diag => error
      }
    }
  }
  impossibleEncodingError(key);
  return 0;
}

EndGameKey EndGameKeyDefinition6Men2Pairs::indexToKey(EndGamePosIndex index) const {
  index += MININDEX;

  EndGameKey result;

  if(index < START_RANGE_KK_ONDIAG_P23_BELOWDIAG) {            // Kings off diag, p2, p3, p4, p5 anywhere
    DECODE_KK_OFFDIAG(result, index)
  } else if(index < START_RANGE_KK_ONDIAG_P2_BELOWDIAG) {                // Kings on diag, p2 below, p3, p4, p5 anywhere
    DECODE_KK_ONDIAG_NOFLIP(result, index);
  } else if (index < START_RANGE_KKP2_ONDIAG) {
    DECODE_KK_ONDIAG_FLIPi(result, index)
  } else if (index < START_RANGE_KKP23_ONDIAG_P45_BELOWDIAG) {
    DECODE_KKP2_ONDIAG(result, index)
  } else if( index < START_RANGE_KKP23_ONDIAG_P4_BELOWDIAG) {
    DECODE_KKP23_ONDIAG_NOFLIP(result, index)
  } else if(index < START_RANGE_KKP234_ONDIAG) {
    DECODE_KKP23_ONDIAG_FLIPi(result, index)
  } else if(index < START_RANGE_KKP2345_ONDIAG) {
    DECODE_KKP234_ONDIAG(result, index)
  } else {
    DECODE_KKP2345_ONDIAG(result, index)
  }
  return result;
}

SymmetricTransformation EndGameKeyDefinition6Men2Pairs::getSymTransformation(EndGameKey key) const {
  return getSym8Transformation6Men2Pairs(key);
}

#ifdef TABLEBASE_BUILDER

void EndGameKeyDefinition6Men2Pairs::scanPositions(EndGameKeyWithOccupiedPositions &key, int pIndex, bool allPreviousOnDiag) const {
  switch(pIndex) {
  case 2:
    if(allPreviousOnDiag) {
      for(int i = 0; i < ARRAYSIZE(s_subDiagIndexToPos); i++) {
        const int pos2 = s_subDiagIndexToPos[i];

        if(key.isOccupied(pos2)) continue;
        key.setPosition(2,pos2);
        scanPositions(key, 3, IS_ONMAINDIAG1(pos2));
        key.clearField(pos2);
      }
    } else {
      for(int pos2 = 0; pos2 < 64; pos2++) {
        if(key.isOccupied(pos2)) continue;
        key.setPosition(2,pos2);
        scanPositions(key, 3, false);
        key.clearField(pos2);
      }
    }
    break;
  case 3:
    { const int pos2 = key.getPosition(2);
      if(allPreviousOnDiag) {
        for(int i = 0; i < ARRAYSIZE(s_subDiagIndexToPos); i++) {
          const int pos3 = s_subDiagIndexToPos[i];
          if(key.isOccupied(pos3)) continue;
          if(IS_ONMAINDIAG1(pos3) && (pos3 <= pos2)) continue;
          key.setPosition(3,pos3);
          scanPositions(key, 4, IS_ONMAINDIAG1(pos3));
          key.clearField(pos3);
        }
      } else if(!key.kingsOnMainDiag1()) { // kings off maindiag => pos3 = [pos2+1..63]
        for(int pos3 = pos2+1; pos3 < 64; pos3++) {
          if(key.isOccupied(pos3)) continue;
          key.setPosition(3,pos3);
          scanPositions(key, 4, false);
          key.clearField(pos3);
        }
      } else {     // kings on maindiag, p2 off maindiag => p3 must be off maindiag and "above" p2
        assert(key.kingsOnMainDiag1() && !IS_ONMAINDIAG1(pos2));
        const int pi2 = s_offDiagPosToIndex[pos2];
        for(int pi3 = pi2+1; pi3 < ARRAYSIZE(s_offDiagIndexToPos); pi3++) {
          const int pos3 = s_offDiagIndexToPos[pi3];

          if(key.isOccupied(pos3)) continue;
          if(!IS_SAMESIDEMAINDIAG1(pos2, pos3)) {
            if(pi2 > pi3) {
              if(pi2 - 28 > pi3) continue;
            } else {
              if(pi2 > pi3 - 28) continue;
            }
          }
          key.setPosition(3,pos3);
          scanPositions(key, 4, false);
          key.clearField(pos3);
        }
      }
    }
    break;
  case 4:
    if(allPreviousOnDiag) {
      for(int i = 0; i < ARRAYSIZE(s_subDiagIndexToPos); i++) {
        const int pos4 = s_subDiagIndexToPos[i];
        if(key.isOccupied(pos4)) continue;
        key.setPosition(4,pos4);
        scanPositions(key, 5, IS_ONMAINDIAG1(pos4));
        key.clearField(pos4);
      }
    } else {
      for(int pos4 = 0; pos4 < 64; pos4++) {
        if(key.isOccupied(pos4)) continue;
        key.setPosition(4,pos4);
        scanPositions(key, 5, false);
        key.clearField(pos4);
      }
    }
    break;
  case 5:
    { const int pos4 = key.getPosition(4);
      if(allPreviousOnDiag) {
        for(int i = 0; i < ARRAYSIZE(s_subDiagIndexToPos); i++) {
          const int pos5 = s_subDiagIndexToPos[i];
          if(key.isOccupied(pos5)) continue;
          if(IS_ONMAINDIAG1(pos5) && (pos5 <= pos4)) continue;
          key.setPosition(5,pos5);
          checkForBothPlayers(key);
          key.clearField(pos5);
        }
      } else if(!(key.kingsOnMainDiag1() && key.p2OnMainDiag1() && key.p3OnMainDiag1())) {
        for(int pos5 = pos4+1; pos5 < 64; pos5++) {
          if(key.isOccupied(pos5)) continue;
          key.setPosition(5,pos5);
          checkForBothPlayers(key);
          key.clearField(pos5);
        }
      } else { // kings,p2,p3 on maindiag, p4 off maindiag
        const int pi4 = s_offDiagPosToIndex[pos4];
        for(int pi5 = pi4+1; pi5 < ARRAYSIZE(s_offDiagIndexToPos); pi5++) {
          const int pos5 = s_offDiagIndexToPos[pi5];
          if(key.isOccupied(pos5)) continue;
          if(!IS_SAMESIDEMAINDIAG1(pos4, pos5)) {
            if(pi4 > pi5) {
              if(pi4 - 28 > pi5) continue;
            } else {
              if(pi4 > pi5 - 28) continue;
            }
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

void EndGameKeyDefinition6Men2Pairs::selfCheck(EndGameKeyWithOccupiedPositions &key) const {
  sym8PositionScanner(key, 0, true, (PositionScanner)&EndGameKeyDefinition6Men2Pairs::scanPositions);
}

bool EndGameKeyDefinition6Men2Pairs::keysEqual(EndGameKey key1, EndGameKey key2) const {
  return keysMatch2Pairs(key2,key1);
}

String EndGameKeyDefinition6Men2Pairs::getCodecName() const {
  return _T("6Men2Pairs");
}

#endif
