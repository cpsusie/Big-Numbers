#pragma once

#ifdef IS32BIT
#define SETREXBITS(          bits     )
#define SETREXBITONHIGHINX(  inx ,bit )
#define SETREXBITSONHIGHINX2(binx,iInx)
#define QWORDTOREX(          size     )
#else // IS64BIT

// REX-byte format
// 0  1  0  0  b3 b2 b1 b0
// b0: When SIB-encoding, then = bit3 of index-register
// b1:
// b2:
// b3: 1 for QWORD-operands, 0 for DWORD operands
#define SETREXBITS(          bits     )                                          \
{ const BYTE tmpBits = bits;                                                     \
  if(tmpBits) setRexBits(tmpBits);                                               \
}

#define HIGHINDEXTOREX(      inx ,bit ) (((inx)>>(3-(bit)))&(1<<(bit)))
#define SETREXBITONHIGHINX(  inx ,bit ) SETREXBITS(HIGHINDEXTOREX(inx,bit))
#define SETREXBITSONHIGHINX2(bInx,iInx) SETREXBITS(HIGHINDEXTOREX(bInx,0) | HIGHINDEXTOREX(iInx,1))
#define QWORDTOREX(          size      ) (((size)==REGSIZE_QWORD)?8:0)

#endif // IS64BIT
