#include "stdafx.h"
#include "EndGameUtil.h"
#include "EndGameKeyCodec.h"

EndGameKeyDefinition5Men::EndGameKeyDefinition5Men(PieceKey pk2, PieceKey pk3, PieceKey pk4)
: EndGameKeyDefinitionDupletsNotAllowed(pk2, pk3, pk4)
{
  assert((GET_TYPE_FROMKEY(pk2) != Pawn)
      && (GET_TYPE_FROMKEY(pk3) != Pawn)
      && (GET_TYPE_FROMKEY(pk4) != Pawn));
}

#define MININDEX 0

EndGamePosIndex EndGameKeyDefinition5Men::keyToIndex(EndGameKey key) const {
  if(!key.kingsOnMainDiag1()) {
    return KK_OFFDIAG_5MEN_INDEX(  key) + OFFSET_KK_OFFDIAG_5MEN    - MININDEX;
  }
  if(!key.p2OnMainDiag1()) {
    return KK_ONDIAG_5MEN_INDEX(   key) + OFFSET_KK_ONDIAG_5MEN     - MININDEX;
  }
  if(!key.p3OnMainDiag1()) {
    return KKP2_ONDIAG_5MEN_INDEX( key) + OFFSET_KKP2_ONDIAG_5MEN   - MININDEX;
  }
  if(!key.p4OnMainDiag1()) {
    return KKP23_ONDIAG_5MEN_INDEX(key) + OFFSET_KKP23_ONDIAG_5MEN  - MININDEX;
  }
  return KKP234_ONDIAG_5MEN_INDEX( key) + OFFSET_KKP234_ONDIAG_5MEN - MININDEX;
}

EndGameKey EndGameKeyDefinition5Men::indexToKey(EndGamePosIndex index) const {
  index += MININDEX;
  EndGameKey result;

  if(index < START_RANGE_KK_ONDIAG_5MEN) {            // king(s) off maindiag => p2,p3,p4 anywhere
    index -= OFFSET_KK_OFFDIAG_5MEN;
    SETPIT(              result, index   );
    SETP4_INDEX(         result, index   );
    SETP3_INDEX(         result, index   );
    SETP2_INDEX(         result, index   );
    SETKK_OFFDIAG(       result, index   );
    result.p234IndexToOffDiagPos();
  } else if(index < START_RANGE_KKP2_ONDIAG_5MEN) {   // kings on maindiag => p2 below, p3,p4 anywhere
    index -= OFFSET_KK_ONDIAG_5MEN;
    SETPIT(              result, index   );
    SETP4_INDEX(         result, index   );
    SETP3_INDEX(         result, index   );
    SETPOS_BELOWDIAG(    result, index, 2);
    SETKK_ONDIAG(        result, index   );
    result.p34IndexToOffDiagPos();
  } else if(index < START_RANGE_KKP23_ONDIAG_5MEN) {  // kings,p2 on maindiag => p3 below, p4 anywhere
    index -= OFFSET_KKP2_ONDIAG_5MEN;
    SETPIT(              result, index   );
    SETP4_INDEX(         result, index   );
    SETPOS_BELOWDIAG(    result, index, 3);
    SETP2_ONDIAG(        result, index   );
    SETKK_ONDIAG(        result, index   );
    result.p2IndexToDiagPos();
    result.p4IndexToOffDiagPos();
  } else if(index < START_RANGE_KKP234_ONDIAG_5MEN) { // kings,p2,p3 on maindiag => p4 below
    index -= OFFSET_KKP23_ONDIAG_5MEN;
    SETPIT(              result, index   );
    SETPOS_BELOWDIAG(    result, index, 4);
    SETP3_ONDIAG(        result, index   );
    SETP2_ONDIAG(        result, index   );
    SETKK_ONDIAG(        result, index   );
    result.p23IndexToDiagPos();
  } else {                                            // kings,p2,p3,p4 on maindiag
    index -= OFFSET_KKP234_ONDIAG_5MEN;
    SETPIT(              result, index   );
    SETP4_ONDIAG(        result, index   );
    SETP3_ONDIAG(        result, index   );
    SETP2_ONDIAG(        result, index   );
    SETKK_ONDIAG(        result, index   );
    result.p234IndexToDiagPos();
  }
  return result;
}

SymmetricTransformation EndGameKeyDefinition5Men::getSymTransformation(EndGameKey key) const {
  return getSym8Transformation5Men(key);
}
