#include "stdafx.h"
#include "EndGameUtil.h"
#include "EndGameKeyCodec.h"

EndGameKeyDefinition6Men::EndGameKeyDefinition6Men(PieceKey pk2, PieceKey pk3, PieceKey pk4, PieceKey pk5)
: EndGameKeyDefinitionDupletsNotAllowed(pk2, pk3, pk4, pk5)
{
  assert((GET_TYPE_FROMKEY(pk2) != Pawn) 
      && (GET_TYPE_FROMKEY(pk3) != Pawn) 
      && (GET_TYPE_FROMKEY(pk4) != Pawn) 
      && (GET_TYPE_FROMKEY(pk5) != Pawn));
}

#define MININDEX 0

EndGamePosIndex EndGameKeyDefinition6Men::keyToIndex(const EndGameKey &key) const {
  if(!key.kingsOnMainDiag1()) {
    return KK_OFFDIAG_6MEN_INDEX(   key) + OFFSET_KK_OFFDIAG_6MEN     - MININDEX;
  }
  if(!key.p2OnMainDiag1()) {
    return KK_ONDIAG_6MEN_INDEX(    key) + OFFSET_KK_ONDIAG_6MEN      - MININDEX;
  }
  if(!key.p3OnMainDiag1()) {
    return KKP2_ONDIAG_6MEN_INDEX(  key) + OFFSET_KKP2_ONDIAG_6MEN    - MININDEX;
  }
  if(!key.p4OnMainDiag1()) {
    return KKP23_ONDIAG_6MEN_INDEX( key) + OFFSET_KKP23_ONDIAG_6MEN   - MININDEX;
  }
  if(!key.p5OnMainDiag1()) {
    return KKP234_ONDIAG_6MEN_INDEX(key) + OFFSET_KKP234_ONDIAG_6MEN  - MININDEX;
  }
  return KKP2345_ONDIAG_6MEN_INDEX( key) + OFFSET_KKP2345_ONDIAG_6MEN - MININDEX;
}

EndGameKey EndGameKeyDefinition6Men::indexToKey(EndGamePosIndex index) const {
  index += MININDEX;
  EndGameKey result;

  if(index < START_RANGE_KK_ONDIAG_6MEN) {            // king(s) off maindiag => p2,p3,p4,p5 anywhere
    index -= OFFSET_KK_OFFDIAG_6MEN;
    SETPIT(              result, index   );
    SETP5_INDEX(         result, index   );
    SETP4_INDEX(         result, index   );
    SETP3_INDEX(         result, index   );
    SETP2_INDEX(         result, index   );
    SETKK_OFFDIAG(       result, index   );
    result.p2345IndexToOffDiagPos();
  } else if(index < START_RANGE_KKP2_ONDIAG_6MEN) {   // kings on maindiag => p2 below, p3,p4,p5 anywhere
    index -= OFFSET_KK_ONDIAG_6MEN;
    SETPIT(              result, index   );
    SETP5_INDEX(         result, index   );
    SETP4_INDEX(         result, index   );
    SETP3_INDEX(         result, index   );
    SETPOS_BELOWDIAG(    result, index, 2);
    SETKK_ONDIAG(        result, index   );
    result.p345IndexToOffDiagPos();
  } else if(index < START_RANGE_KKP23_ONDIAG_6MEN) {  // kings,p2 on maindiag => p3 below, p4,p5 anywhere
    index -= OFFSET_KKP2_ONDIAG_6MEN;
    SETPIT(              result, index   );
    SETP5_INDEX(         result, index   );
    SETP4_INDEX(         result, index   );
    SETPOS_BELOWDIAG(    result, index, 3);
    SETP2_ONDIAG(        result, index   );
    SETKK_ONDIAG(        result, index   );
    result.p2IndexToDiagPos();
    result.p45IndexToOffDiagPos();
  } else if(index < START_RANGE_KKP234_ONDIAG_6MEN) { // kings,p2,p3 on maindiag => p4 below, p5 anywhere
    index -= OFFSET_KKP23_ONDIAG_6MEN;
    SETPIT(              result, index   );
    SETP5_INDEX(         result, index   );
    SETPOS_BELOWDIAG(    result, index, 4);
    SETP3_ONDIAG(        result, index   );
    SETP2_ONDIAG(        result, index   );
    SETKK_ONDIAG(        result, index   );
    result.p23IndexToDiagPos();
    result.p5IndexToOffDiagPos();
  } else if(index < START_RANGE_KKP2345_ONDIAG_6MEN) { // kings,p2,p3,p4 on maindiag => p5 below
    index -= OFFSET_KKP234_ONDIAG_6MEN;
    SETPIT(              result, index   );
    SETPOS_BELOWDIAG(    result, index, 5);
    SETP4_ONDIAG(        result, index   );
    SETP3_ONDIAG(        result, index   );
    SETP2_ONDIAG(        result, index   );
    SETKK_ONDIAG(        result, index   );
    result.p234IndexToDiagPos();
  } else {                                             // kings,p2,p3,p4,p5 on maindiag
    index -= OFFSET_KKP2345_ONDIAG_6MEN;
    SETPIT(              result, index   );
    SETP5_ONDIAG(        result, index   );
    SETP4_ONDIAG(        result, index   );
    SETP3_ONDIAG(        result, index   );
    SETP2_ONDIAG(        result, index   );
    SETKK_ONDIAG(        result, index   );
    result.p2345IndexToDiagPos();
  }
  return result;
}

SymmetricTransformation EndGameKeyDefinition6Men::getSymTransformation(const EndGameKey &key) const {
  return getSym8Transformation6Men(key);
}
