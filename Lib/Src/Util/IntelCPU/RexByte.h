#pragma once

#ifdef IS32BIT

#define SETREXBITS(          bits     )
#define SETREXBITONHIGHINX(  inx ,bit )
#define SETREXBITSONHIGHINX2(binx,iInx)
#define SETREXUNIFORMREGISTER(reg     )
#define QWORDTOREX(          size     )

#else // IS64BIT

// REX-byte format
// 0 1 0 0 W R X B
// W: 1 for QWORD-operands, 0 for default operandsize (=DWORD for most instructions)
// R: Exension to MOD-R/M reg-field
// X: Extension to SIB index-field
// B: Extension to MOD-R/M rm-field, or SIB base-field
// If a REX-byte is present, AH,CH,DX and BH are replaced by SPL,BPL,SIL,DIL.

#define SETREXBITS(          bits     )                                          \
{ const BYTE tmpBits = bits;                                                     \
  if(tmpBits) setRexBits(tmpBits);                                               \
}

#define REX_B 0
#define REX_X 1
#define REX_R 2
#define REX_W 3

#define HIGHINDEXTOREX(      inx ,bit ) (((inx)>>(3-(bit)))&(1<<(bit)))
#define SETREXBITONHIGHINX(  inx ,bit ) SETREXBITS(HIGHINDEXTOREX(inx,bit))
#define SETREXBITSONHIGHINX2(bInx,iInx) SETREXBITS(HIGHINDEXTOREX(bInx,REX_B) | HIGHINDEXTOREX(iInx,REX_X))
#define SETREXUNIFORMREGISTER(reg     )                                          \
{                                                                                \
  if((reg).isUniformByteRegister() && !hasRexByte()) setRexBits(0);              \
}

#define QWORDTOREX(          size      ) (((size)==REGSIZE_QWORD)?(1<<REX_W):0)

#endif // IS64BIT
