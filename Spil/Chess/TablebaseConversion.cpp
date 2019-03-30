#include "stdafx.h"
#include <io.h>
#include <ByteFile.h>
#include <CompressFilter.h>

#ifdef TABLEBASE_BUILDER

void EndGameTablebase::compressNew(ByteOutputStream &s) {
  throwException(_T("%s:%s:Not supported"), getName().cstr(), __TFUNCTION__);
}

void EndGameTablebase::convert() {
  throwException(_T("%s:%s:Not supported"), getName().cstr(), __TFUNCTION__);
#ifdef __NEVER__
  String /*f1Src,f1Dst, f2Src,f2Dst, f3Src,f3Dst, */f4Src,f4Dst/*, f5Src,f5Dst*/ ,f6Src,f6Dst;
  try {
/*
    f1Src = loadAllForwardPositions();
    f1Dst = saveAllForwardPositions(true);
    unload();

    f2Src = oldtb.loadAllRetroPositions();
    m_info = oldtb.getInfo();
    f2Dst = saveAllRetroPositions(true);

    unload(); oldtb.unload();
*/
    EndGameKeyDefinition::setMetric(DEPTH_TO_CONVERSION);
    load();
//    f3Dst = save(true);

    f4Src = getTbFileName(COMPRESSEDTABLEBASE);
    f4Dst = compress(true);
    unload();

    EndGameKeyDefinition::setMetric(DEPTH_TO_MATE);
    load();
//    f5Dst = save(true);

    f6Src = getTbFileName(COMPRESSEDTABLEBASE);
    f6Dst = compress(true);
    unload();

    EndGameKeyDefinition::setMetric(DEPTH_TO_CONVERSION);

/*
    unlink(f1Src);    rename(f1Dst, f1Src);
    unlink(f2Src);    rename(f2Dst, f2Src);
    unlink(f3Src);    rename(f3Dst, f3Src);
    unlink(f4Src);    rename(f4Dst, f4Src);
    unlink(f5Src);    rename(f5Dst, f5Src);
    unlink(f6Src);    rename(f6Dst, f6Src);
*/
  } catch(...) {
    unload();

//    if(f1Dst != EMPTYSTRING) unlink(f1Dst);
//    if(f2Dst != EMPTYSTRING) unlink(f2Dst);

//    if(f3Dst != EMPTYSTRING) unlink(f3Dst);
    if(f4Dst != EMPTYSTRING) unlink(f4Dst);
//    if(f5Dst != EMPTYSTRING) unlink(f5Dst);
    if(f6Dst != EMPTYSTRING) unlink(f6Dst);
    EndGameKeyDefinition::setMetric(DEPTH_TO_CONVERSION);
    throw;
  }
#endif // __NEVER__

}

#endif
