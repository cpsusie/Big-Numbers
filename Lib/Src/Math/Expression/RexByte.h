#pragma once

#ifdef IS32BIT
#define SETREXBITS(          bits      )
#define SETREXBITONHIGHREG(  reg,bit   )
#define SETREXBITSONHIGHREG2(reg,addReg)
#else // IS64BIT
#define SETREXBITS(          bits      ) { if(bits) setRexBits(bits); }
#define SETREXBITONHIGHREG(  reg,bit   )                                          \
{ if((reg.getIndex()) > 7) setRexBits(1<<(bit));                                  \
}
#define SETREXBITSONHIGHREG2(reg,addReg)                                          \
{ const BYTE _rexBits = (((reg.getIndex())>>3)&1) | (((addReg.getIndex())>>2)&2); \
  SETREXBITS(_rexBits)                                                            \
}
#endif // IS64BIT
