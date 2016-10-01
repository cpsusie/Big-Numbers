#include "stdafx.h"
#include "EndGameUtil.h"
#include "EndGameKeyCodec.h"

EndGameKeyDefinition3Men::EndGameKeyDefinition3Men(PieceKey pk2) : EndGameKeyDefinitionDupletsNotAllowed(pk2) {
  assert(GET_TYPE_FROMKEY(pk2) != Pawn);
}

#define MININDEX 0

unsigned long EndGameKeyDefinition3Men::keyToIndex(const EndGameKey &key) const {
  if(!key.kingsOnMainDiag1()) {
    return KK_OFFDIAG_3MEN_INDEX(key) + OFFSET_KK_OFFDIAG_3MEN  - MININDEX;
  }
  if(!key.p2OnMainDiag1()) {
    return KK_ONDIAG_3MEN_INDEX(key)  + OFFSET_KK_ONDIAG_3MEN   - MININDEX;
  }
  return KKP2_ONDIAG_3MEN_INDEX(key)  + OFFSET_KKP2_ONDIAG_3MEN - MININDEX;
}

EndGameKey EndGameKeyDefinition3Men::indexToKey(unsigned long index) const {
  index += MININDEX;
  EndGameKey result;

  if(index < START_RANGE_KK_ONDIAG_3MEN) {          // king(s) off maindiag => p2 anywhere
    index -= OFFSET_KK_OFFDIAG_3MEN;
    SETPIT(           result, index   );
    SETP2_INDEX(      result, index   );
    SETKK_OFFDIAG(    result, index   );
    result.p2IndexToOffDiagPos();
  } else if(index < START_RANGE_KKP2_ONDIAG_3MEN) { // kings on maindiag => p2 below
    index -= OFFSET_KK_ONDIAG_3MEN;
    SETPIT(           result, index   );
    SETPOS_BELOWDIAG( result, index, 2);
    SETKK_ONDIAG(     result, index   );
  } else {                                          // kings,p2 on maindiag
    index -= OFFSET_KKP2_ONDIAG_3MEN;
    SETPIT(           result, index   );
    SETP2_ONDIAG(     result, index   );
    SETKK_ONDIAG(     result, index   );
    result.p2IndexToDiagPos();
  }
  return result;
}

SymmetricTransformation EndGameKeyDefinition3Men::getSymTransformation(const EndGameKey &key) const {
  return getSym8Transformation3Men(key);
}
