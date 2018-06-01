#include "pch.h"
#include <Opcode.h>
#include "FPUEmulator.h"

namespace Expr {

#define addKey(code) put(code.getMnemonic().cstr(), _##code)

FPUOpcodeHashMap::FPUOpcodeHashMap(size_t capacity) : CompactStrHashMap(capacity) {
  addKey(FLD     ) ;addKey(FSTP    ) ;addKey(FST     ) ;addKey(FADD    ) ;addKey(FMUL    );
  addKey(FSUB    ) ;addKey(FDIV    ) ;addKey(FSUBR   ) ;addKey(FDIVR   ) ;addKey(FADDP   );
  addKey(FMULP   ) ;addKey(FSUBP   ) ;addKey(FDIVP   ) ;addKey(FSUBRP  ) ;addKey(FDIVRP  );
  addKey(FCOM    ) ;addKey(FCOMI   ) ;addKey(FUCOM   ) ;addKey(FUCOMI  ) ;addKey(FCOMP   );
  addKey(FCOMIP  ) ;addKey(FUCOMP  ) ;addKey(FUCOMIP ) ;addKey(FCOMPP  ) ;addKey(FUCOMPP );
  addKey(FILD    ) ;addKey(FISTP   ) ;addKey(FISTTP  ) ;addKey(FIST    ) ;addKey(FIADD   );
  addKey(FIMUL   ) ;addKey(FISUB   ) ;addKey(FIDIV   ) ;addKey(FISUBR  ) ;addKey(FIDIVR  );
  addKey(FICOM   ) ;addKey(FICOMP  ) ;addKey(FCMOVB  ) ;addKey(FCMOVAE ) ;addKey(FCMOVE  );
  addKey(FCMOVNE ) ;addKey(FCMOVBE ) ;addKey(FCMOVA  ) ;addKey(FCMOVU  ) ;addKey(FCMOVNU );
  addKey(FFREE   ) ;addKey(FXCH    ) ;addKey(FWAIT   ) ;addKey(FNOP    ) ;addKey(FCHS    );
  addKey(FABS    ) ;addKey(FTST    ) ;addKey(FXAM    ) ;addKey(FLD1    ) ;addKey(FLDL2T  );
  addKey(FLDL2E  ) ;addKey(FLDPI   ) ;addKey(FLDLG2  ) ;addKey(FLDLN2  ) ;addKey(FLDZ    );
  addKey(F2XM1   ) ;addKey(FYL2X   ) ;addKey(FPTAN   ) ;addKey(FPATAN  ) ;addKey(FXTRACT );
  addKey(FPREM1  ) ;addKey(FDECSTP ) ;addKey(FINCSTP ) ;addKey(FPREM   ) ;addKey(FYL2XP1 );
  addKey(FSQRT   ) ;addKey(FSINCOS ) ;addKey(FRNDINT ) ;addKey(FSCALE  ) ;addKey(FSIN    );
  addKey(FCOS    );
#ifdef IS32BIT
#ifndef LONGDOUBLE
  addKey(CALL);
#endif
#endif
};

#ifdef FINDBESTCAPACITY
class HashmapCapcityFinder {
public:
  HashmapCapcityFinder();
};

HashmapCapcityFinder::HashmapCapcityFinder() {
  int  bestCapacity    = -1;
  UINT bestChainLength = 0;
  for(int capacity = 20; capacity < 2000; capacity++) {
    const FPUOpcodeHashMap hm(capacity);
    const UINT chainLength = hm.getMaxChainLength();
    debugLog(_T("Capacity:%4d, maxChainLength:%d\n"), capacity, chainLength);
    if((bestCapacity < 0) || (chainLength < bestChainLength)) {
      bestCapacity    = capacity;
      bestChainLength = chainLength;
    }
    if(bestChainLength == 1) {
      break;
    }
  }
  debugLog(_T("\nCapacity=%d gives best hashmap (maxchainLength=%u\n")
          ,bestCapacity, bestChainLength);
}

HashmapCapcityFinder s_hashmapCapcityFinder;

#endif // FINDBESTCAPACITY

}; // namespace Expr
