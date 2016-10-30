#include "stdafx.h"
#include "EndGameUtil.h"
#include "EndGameKeyCodec.h"

EndGameKeyDefinition4Men::EndGameKeyDefinition4Men(PieceKey pk2, PieceKey pk3) : EndGameKeyDefinitionDupletsNotAllowed(pk2, pk3) {
  assert(GET_TYPE_FROMKEY(pk2) != Pawn);
  assert(GET_TYPE_FROMKEY(pk3) != Pawn);
  assert(pk2 != pk3);
}

#define MININDEX 0

EndGamePosIndex EndGameKeyDefinition4Men::keyToIndex(const EndGameKey &key) const {
  if(!key.kingsOnMainDiag1()) {
    return KK_OFFDIAG_4MEN_INDEX( key) + OFFSET_KK_OFFDIAG_4MEN   - MININDEX;
  }
  if(!key.p2OnMainDiag1()) {
    return KK_ONDIAG_4MEN_INDEX(  key) + OFFSET_KK_ONDIAG_4MEN    - MININDEX;
  }
  if(!key.p3OnMainDiag1()) {
    return KKP2_ONDIAG_4MEN_INDEX(key) + OFFSET_KKP2_ONDIAG_4MEN  - MININDEX;
  }
  return KKP23_ONDIAG_4MEN_INDEX( key) + OFFSET_KKP23_ONDIAG_4MEN - MININDEX;
}

EndGameKey EndGameKeyDefinition4Men::indexToKey(EndGamePosIndex index) const {
  index += MININDEX;
  EndGameKey result;

  if(index < START_RANGE_KK_ONDIAG_4MEN) {           // king(s) off maindiag => p2,p3 anywhere
    index -= OFFSET_KK_OFFDIAG_4MEN;
    SETPIT(              result, index   );
    SETP3_INDEX(         result, index   );
    SETP2_INDEX(         result, index   );
    SETKK_OFFDIAG(       result, index   );
    result.p23IndexToOffDiagPos();
  } else if(index < START_RANGE_KKP2_ONDIAG_4MEN) {  // kings on maindiag => p2 below, p3 anywhere
    index -= OFFSET_KK_ONDIAG_4MEN;
    SETPIT(              result, index   );
    SETP3_INDEX(         result, index   );
    SETPOS_BELOWDIAG(    result, index, 2);
    SETKK_ONDIAG(        result, index   );
    result.p3IndexToOffDiagPos();
  } else if(index < START_RANGE_KKP23_ONDIAG_4MEN) { // kings,p2 on maindiag => p3 below
    index -= OFFSET_KKP2_ONDIAG_4MEN;
    SETPIT(              result, index   );
    SETPOS_BELOWDIAG(    result, index, 3);
    SETP2_ONDIAG(        result, index   );
    SETKK_ONDIAG(        result, index   );
    result.p2IndexToDiagPos();
  } else {                                           // kings,p2,p3 on maindiag
    index -= OFFSET_KKP23_ONDIAG_4MEN;
    SETPIT(              result, index   );
    SETP3_ONDIAG(        result, index   );
    SETP2_ONDIAG(        result, index   );
    SETKK_ONDIAG(        result, index   );
    result.p23IndexToDiagPos();
  }
  return result;
}

SymmetricTransformation EndGameKeyDefinition4Men::getSymTransformation(const EndGameKey &key) const {
  return getSym8Transformation4Men(key);
}
