#pragma once

// General purpose registers

// 32 bit registers
#define EAX 0
#define ECX 1
#define EDX 2
#define EBX 3
#define ESP 4
#define EBP 5
#define ESI 6
#define EDI 7

#ifdef IS64BIT
// 64 bit registers
#define RAX  0
#define RCX  1
#define RDX  2
#define RBX  3
#define RSP  4
#define RBP  5
#define RSI  6
#define RDI  7
#define R8   8
#define R9   9
#define R10 10
#define R11 11
#define R12 12
#define R13 13
#define R14 14
#define R15 15

#define XMM0   0
#define XMM1   1
#define XMM2   2
#define XMM3   3
#define XMM4   4
#define XMM5   5
#define XMM6   6
#define XMM7   7
#define XMM8   8
#define XMM9   9
#define XMM10 10
#define XMM11 11
#define XMM12 12
#define XMM13 13
#define XMM14 14
#define XMM15 15

#endif // IS53BIT

// 16 bit registers
#define AX 0
#define CX 1
#define DX 2
#define BX 3
#define SP 4
#define BP 5
#define SI 6
#define DI 7

// 8 bit registers
#define AL 0
#define CL 1
#define DL 2
#define BL 3
#define AH 4
#define CH 5
#define DH 6
#define BH 7

// 16 bit segment registers
#define ES 0
#define CS 1
#define SS 2
#define DS 3
#define FS 4
#define GS 5

#define _SWAP2(op)     ((((op)&0xff)<< 8) | (((op)>> 8)&0xff))
#define _SWAP3(op)     ((_SWAP2(op) << 8) | (((op)>>16)&0xff))
#define _SWAP4(op)     ((_SWAP2(op) <<16) | (_SWAP2((op)>>16)))
#define _SWAP5(op)     ((_SWAP4(op) << 8) | (((op)>>32)&0xff))
#define _SWAP6(op)     ((_SWAP4(op) <<16) | (_SWAP2((op)>>32)))

#define I64(n)        ((unsigned __int64)(n))


class IntelOpcode {
public:
  static inline unsigned __int64 swapBytes(unsigned __int64 bytes, int sz) {
    switch(sz) {
    case 1 : return bytes;
    case 2 : return _SWAP2(bytes);
    case 3 : return _SWAP3(bytes);
    case 4 : return _SWAP4(bytes);
    case 5 : return _SWAP5(bytes);
    case 6 : return _SWAP6(bytes);
    default: throwMethodInvalidArgumentException(_T("IntelOpcode"), _T("swapBytes"), _T("sz=%d"), sz);
             return bytes;
    }
  }
public:
  union {
    unsigned __int64 m_bytes;
    unsigned char    m_byte[8];
  };
  unsigned int     m_size        : 6; // = [0..15]. We can take only 6
  unsigned int     m_memAddrMode : 1;
  unsigned int     m_regSrcMode  : 1;
  inline IntelOpcode(BYTE size, unsigned __int64 bytes, bool memAddrMode, bool regSrcMode)
    : m_bytes(swapBytes(bytes,size))
    , m_size(size)
    , m_memAddrMode(memAddrMode)
    , m_regSrcMode(regSrcMode)
  {
  }
  IntelOpcode &prefix48() {
    m_bytes <<= 8;
    m_bytes |= 0x48;
    m_size++;
    return *this;
  }
};

class IntelInstruction : public IntelOpcode {
private:
  inline IntelInstruction &or(BYTE b) { // always |= the last byte in instruction
    m_byte[m_size - 1] |= b;
    return *this;
  }
  // size=#bytes of opcode, ofSize=#bytes of offset
  inline IntelInstruction &add(__int64 bytesToAdd, BYTE count) {
    if (count == 1) {
      m_byte[m_size++] = (BYTE)bytesToAdd;
    }
    else {
      m_bytes |= (bytesToAdd) << (8 * m_size);
      m_size += count;
    }
    return *this;
  }
public:
  inline IntelInstruction(BYTE size, unsigned __int64 bytes) : IntelOpcode(size, bytes, false, false) {
  }
  inline IntelInstruction(const IntelOpcode &op) : IntelOpcode(op) {
  }
  inline IntelInstruction &memAddrPtr(BYTE reg) {
    assert(m_memAddrMode);
    return or (reg);
  }
  inline IntelInstruction &memAddrPtr1(BYTE reg, BYTE offset) {
    assert(m_memAddrMode);
    return or(0x40 | reg).add(offset, 1);
  }
  inline IntelInstruction &memAddrPtr4(BYTE reg, int offset) {
    assert(m_memAddrMode);
    return or(0x80 | reg).add(offset, 4);
  }
  inline IntelInstruction &memAddrMp2Ptr4(BYTE reg, BYTE p2, int offset) {
    assert(m_memAddrMode);
    return or(0x04).add(0x05 | (p2 << 6) | (reg << 3), 1).add(offset, 4);
  }
  inline IntelInstruction &memAddrMp2Reg(BYTE reg, BYTE p2, BYTE addReg) {
    assert(m_memAddrMode);
    return or(0x04).add((p2 << 6) | (addReg << 3) | reg, 1);
  }
  inline IntelInstruction &memAddrMp2Reg1(BYTE reg, BYTE p2, BYTE addReg, BYTE offs1) {
    assert(m_memAddrMode);
    return or(0x44).add((p2 << 6) | (addReg << 3) | reg, 1).add(offs1, 1);
  }
  inline IntelInstruction &memAddrMp2Reg4(BYTE reg, BYTE p2, BYTE addReg, int offs4) {
    assert(m_memAddrMode);
    return or(0x84).add((p2 << 6) | (addReg << 3) | reg, 1).add(offs4, 4);
  }
  inline IntelInstruction &memAddrEsp() {
    assert(m_memAddrMode);
    return or(0x04).add(0x24, 1);
  }
  inline IntelInstruction &memAddrEsp1(BYTE offs1) {
    assert(m_memAddrMode);
    return or(0x44).add(0x24, 1).add(offs1, 1);
  }
  inline IntelInstruction &memAddrEsp4(int offs4) {
    assert(m_memAddrMode);
    return or(0x84).add(0x24, 1).add(offs4, 4);
  }
  inline IntelInstruction &memImmDword() { // add 4 bytes address
    assert(m_memAddrMode);
    return or(0x05);
  }
  inline IntelInstruction &regSrc(BYTE reg) {
    assert(m_regSrcMode);
    return or(0xc0 | reg);
  }
};

#define PREFIX48(ins) ((ins).prefix48())

#define MEM_ADDR_PTR(       op,reg                ) IntelInstruction(op).memAddrPtr(     reg)                   //  reg!=ESP,EBP                                     ex:fld word ptr[eax]
#define MEM_ADDR_PTR1(      op,reg          ,offs1) IntelInstruction(op).memAddrPtr1(    reg,         offs1)    //  reg!=ESP                    offs1=1 byte signed  ex.fld word ptr[eax+127]
#define MEM_ADDR_PTR4(      op,reg          ,offs4) IntelInstruction(op).memAddrPtr4(    reg,         offs4)    //  reg!=ESP                    offs4=4 bytes signed ex fld word ptr[eax+0x12345678]
#define MEM_ADDR_MP2PTR4(   op,reg       ,p2,offs4) IntelInstruction(op).memAddrMp2Ptr4( reg,p2,      offs4)    //  reg!=ESP             p2=0-3 offs4=4 bytes signed ex fld word ptr[2*eax+0x12345678]
#define MEM_ADDR_PTRMP2REG( op,reg,addReg,p2      ) IntelInstruction(op).memAddrMp2Reg(  reg,p2,addReg)         //  reg!=EBP addReg!=ESP p2=0-3                      ex fld word ptr[esp+2*ecx]
#define MEM_ADDR_PTRMP2REG1(op,reg,addReg,p2,offs1) IntelInstruction(op).memAddrMp2Reg1( reg,p2,addReg,offs1)   //           addReg!=ESP p2=0-3                      ex fld word ptr[ebp+2*ecx+127]
#define MEM_ADDR_PTRMP2REG4(op,reg,addReg,p2,offs4) IntelInstruction(op).memAddrMp2Reg4( reg,p2,addReg,offs4)   //           addReg!=ESP p2=0-3 offs4=4 bytes signed ex fld word ptr[esp+2*eax+0x12345678]
#define MEM_ADDR_ESP(       op                    ) IntelInstruction(op).memAddrEsp()                           //                                                   ex fld word ptr[esp}
#define MEM_ADDR_ESP1(      op              ,offs1) IntelInstruction(op).memAddrEsp1(offs1)                     //                              offst=1 byte signed  ex fld word ptr[esp+128}
#define MEM_ADDR_ESP4(      op              ,offs4) IntelInstruction(op).memAddrEsp4(offs4)                     //                              offst=4 bytes signed ex fld word ptr[esp+0x12345678]
#define MEM_ADDR_DS(        op                    ) IntelInstruction(op).memImmDword()                          //  + 4 byte address. In x64 mode PC-relative offset
#define REG_SRC(            op,reg)                 IntelInstruction(op).regSrc(reg)                            //                                                   ex add eax, ecx

#define MEM_ADDR_PTRREG(    op,reg,addReg         ) MEM_ADDR_PTRMP2REG( op,reg,addReg,0      )                                                                          // reg!=EBP addReg!=ESP                              ex fld word ptr[esp+  ecx]
#define MEM_ADDR_PTRREG1(   op,reg,addReg   ,offs1) MEM_ADDR_PTRMP2REG1(op,reg,addReg,0,offs1)                                                                          //          addReg!=ESP         offs1=1 byte signed  ex fld word ptr[ebp+  ecx+127]
#define MEM_ADDR_PTRREG4(   op,reg,addReg   ,offs4) MEM_ADDR_PTRMP2REG4(op,reg,addReg,0,offs4)                                                                          //          addReg!=ESP         offs1=4 bytes signed ex fld word ptr[esp+  eax+0x12345678]

// Instructions defined with these macroes, cannot be combined with the various addressing-nmodes
#define B1INS(op)        IntelInstruction(1, op)
#define B2INS(op)        IntelInstruction(2, op)
#define B3INS(op)        IntelInstruction(3, op)
#define B4INS(op)        IntelInstruction(4, op)

// Instrcution defined with these macroes, must be combined with macroes MEM_ADDR_* (and evt. REG_SRC)
#define B1INSA(op)       IntelOpcode(1, op, true ,true)
#define B2INSA(op)       IntelOpcode(2, op, true ,true)
#define B3INSA(op)       IntelOpcode(3, op, true ,true)
#define B4INSA(op)       IntelOpcode(4, op, true ,true)
#define B2INSANOREG(op)  IntelOpcode(2, op, true,false)

#define FPUINS(op)       B2INS(      op)
#define FPUINSA(op)      B2INSANOREG(op)

// Use less/greater opcode for signed comparison. below/above for unsigned.

// 1 byte PC relative offset
#define JOSHORT                                B1INS(0x70)                              // Jump short if overflow
#define JNOSHORT                               B1INS(0x71)                              // Jump short if not overflow
#define JBSHORT                                B1INS(0x72)                              // Jump short if below                (unsigned)
#define JAESHORT                               B1INS(0x73)                              // Jump short if above or equal       (unsigned)
#define JESHORT                                B1INS(0x74)                              // Jump short if equal                (signed/unsigned)
#define JNESHORT                               B1INS(0x75)                              // Jump short if not equal            (signed/unsigned)
#define JBESHORT                               B1INS(0x76)                              // Jump short if below or equal       (unsigned)
#define JASHORT                                B1INS(0x77)                              // Jump short if above                (unsigned)
#define JSSHORT                                B1INS(0x78)                              // Jump short if sign
#define JNSSHORT                               B1INS(0x79)                              // Jump short if not sign
#define JPESHORT                               B1INS(0x7A)                              // Jump short if parity even
#define JPOSHORT                               B1INS(0x7B)                              // Jump short if parity odd
#define JLSHORT                                B1INS(0x7C)                              // Jump short if less                 (signed)
#define JGESHORT                               B1INS(0x7D)                              // Jump short if greater or equal     (signed)
#define JLESHORT                               B1INS(0x7E)                              // Jump short if less or equal        (signed)
#define JGSHORT                                B1INS(0x7F)                              // Jump short if greater              (signed)

#define JNAESHORT                              B1INS(0x72)                              // Jump short if not above or equal   (unsigned)
#define JCSHORT                                B1INS(0x72)                              // Jump short if carry                (unsigned)
#define JNCSHORT                               B1INS(0x73)                              // Jump short if not carry            (unsigned)
#define JNBSHORT                               B1INS(0x73)                              // Jump short if not below            (unsigned)
#define JZSHORT                                B1INS(0x74)                              // Jump short if zero                 (signed/unsigned)
#define JNZSHORT                               B1INS(0x75)                              // Jump short if not zero             (signed/unsigned)
#define JNASHORT                               B1INS(0x76)                              // Jump short if not above            (unsigned)
#define JNBESHORT                              B1INS(0x77)                              // Jump short if not below or equal   (unsigned)
#define JNGESHORT                              B1INS(0x7C)                              // Jump short if not greater or equal (signed)
#define JNLSHORT                               B1INS(0x7D)                              // Jump short if not less             (signed)
#define JNGSHORT                               B1INS(0x7E)                              // Jump short if not greater          (signed)
#define JNLESHORT                              B1INS(0x7F)                              // Jump short if not less or equal    (signed)

// 4 byte PC relative offset
#define JONEAR                                 B2INS(0x0F80)                            // Jump near if overflow
#define JNONEAR                                B2INS(0x0F81)                            // Jump near if not overflow
#define JBNEAR                                 B2INS(0x0F82)                            // Jump near if below                 (unsigned)
#define JAENEAR                                B2INS(0x0F83)                            // Jump near if above or equal        (unsigned)
#define JENEAR                                 B2INS(0x0F84)                            // Jump near if equal                 (signed/unsigned)
#define JNENEAR                                B2INS(0x0F85)                            // Jump near if not equal             (signed/unsigned)
#define JBENEAR                                B2INS(0x0F86)                            // Jump near if below or equal        (unsigned)
#define JANEAR                                 B2INS(0x0F87)                            // Jump near if above                 (unsigned)
#define JSNEAR                                 B2INS(0x0F88)                            // Jump near if sign
#define JNSNEAR                                B2INS(0x0F89)                            // Jump near if not sign
#define JPENEAR                                B2INS(0x0F8A)                            // Jump near if parity even
#define JPONEAR                                B2INS(0x0F8B)                            // Jump near if parity odd
#define JLNEAR                                 B2INS(0x0F8C)                            // Jump near if less                  (signed)
#define JGENEAR                                B2INS(0x0F8D)                            // Jump near if greater or equal      (signed)
#define JLENEAR                                B2INS(0x0F8E)                            // Jump near if less or equal         (signed)
#define JGNEAR                                 B2INS(0x0F8F)                            // Jump near if greater               (signed)

#define JNAENEAR                               B2INS(0x0F82)                            // Jump near if not above or equal    (unsigned)
#define JCNEAR                                 B2INS(0x0F82)                            // Jump near if carry                 (unsigned)
#define JNCNEAR                                B2INS(0x0F83)                            // Jump near if not carry             (unsigned)
#define JNBNEAR                                B2INS(0x0F83)                            // Jump near if not below             (unsigned)
#define JZNEAR                                 B2INS(0x0F84)                            // Jump near if 0                     (signed/unsigned)
#define JNZNEAR                                B2INS(0x0F85)                            // Jump near if not zero              (signed/unsigned)
#define JNANEAR                                B2INS(0x0F86)                            // Jump near if not above             (unsigned)
#define JNBENEAR                               B2INS(0x0F87)                            // Jump near if not below or equal    (unsigned)
#define JNGENEAR                               B2INS(0x0F8C)                            // Jump near if not greater or equal  (signed)
#define JNLNEAR                                B2INS(0x0F8D)                            // Jump near if not less              (signed)
#define JNGNEAR                                B2INS(0x0F8E)                            // Jump near if not greater           (signed)
#define JNLENEAR                               B2INS(0x0F8F)                            // Jump near if not less or equal     (signed)


#define JMPSHORT                               B1INS(0xEB)                              // Unconditional short jump. 1 byte PC relative offset
#define JMPNEAR                                B1INS(0xE9)                              // Unconditional near jump. 4 byte PC relative offset
#define JCXZSHORT                              B2INS(0x67E3)                            // Jump short if CX  register is 0. 1 byte PC relative offset
#define JECXSHORT                              B1INS(0xE3)                              // Jump short if ECX register is 0  1 byte PC relative offset

#define CALL                                   B1INS(0xE8)                              // Call near, 4 byte PC relative, displacement
#define CALLABSOLUTE                           B2INSA(0xFFD0)                            // Call far, absolute address given by operand
#define RET                                    B1INS(0xC3)                              // Near return to calling procedure

#define TEST_AL_IMM_BYTE                       B1INS(0xA8)                              // 1 byte operand
#define TEST_AX_IMM_WORD                       B2INS(0x66A9)                            // 2 byte operand
#define TEST_EAX_IMM_DWORD                     B1INS(0xA9)                              // 4 byte operand

#define TEST_IMM_BYTE                          B2INSA(0xF600)                           // Build src with MEM_ADDR-*,REG_SRC-macroes. 1 byte operand
#define TEST_IMM_WORD                          B3INSA(0x66F700)                         // Build src with MEM_ADDR-*,REG_SRC-macroes. 2 byte operand
#define TEST_IMM_DWORD                         B2INSA(0xF700)                           // Build src with MEM_ADDR-*,REG_SRC-macroes. 4 byte operand

#define TEST_R8_BYTE(        r8 )              B2INSA(0x8400    | ((r8 )<<3))           // Build op2 with MEM_ADDR-*,REG_SRC-macroes
#define TEST_R16_WORD(       r16)              B3INSA(0x668500  | ((r16)<<3))           // Build op2 with MEM_ADDR-*,REG_SRC-macroes
#define TEST_R32_DWORD(      r32)              B2INSA(0x8500    | ((r32)<<3))           // Build op2 with MEM_ADDR-*,REG_SRC-macroes


#define NOOP                                   B1INS(0x90)

#define XCHG_AX_R16(         r16)              B2INS(0x6690     |  (r16))               // r16=ax-di
#define XCHG_EAX_R32(        r32)              B1INS(0x90       |  (r32))               // r32=eax-edi
#define XCHG_R8_BYTE(        r8 )              B1INSA(0x8600    | ((r8 )<<3))           // Build op2 with MEM_ADDR-*,REG_SRC-macroes
#define XCHG_R16_WORD(       r16)              B3INSA(0x668700  | ((r32)<<3)            // Build op2 with MEM_ADDR-*,REG_SRC-macroes
#define XCHG_R32_DWORD(      r32)              B1INSA(0x8700    | ((r32)<<3))           // Build op2 with MEM_ADDR-*,REG_SRC-macroes

#define MOV_FROM_SEGREG_WORD(seg)              B2INSA(0x8C00    | ((seg)<<3))           // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define LEA_R32_DWORD(       dst)              B2INSANOREG(0x8D00 | ((dst)<<3))         // Build src with MEM_ADDR-macroes
#define MOV_TO_SEGREG_WORD(  seg)              B2INSA(0x8E00    | ((seg)<<3))           // Build src with MEM_ADDR-*,REG_SRC-macroes
#define POP_DWORD                              B2INSA(0x8F00)                           // Build dst with MEM_ADDR-*,REG_SRC-macroes

#define MOV_BYTE_R8(         r8 )              B2INSA(0x8800    | ((r8 )<<3))           // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define MOV_WORD_R16(        r16)              B3INSA(0x668900  | ((r16)<<3))           // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define MOV_DWORD_R32(       r32)              B2INSA(0x8900    | ((r32)<<3))           // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define MOV_R8_BYTE(         r8 )              B2INSA(0x8A00    | ((r8 )<<3))           // Build src with MEM_ADDR-*,REG_SRC-macroes
#define MOV_R16_WORD(        r16)              B3INSA(0x668B00  | ((r16)<<3))           // Build src with MEM_ADDR-*,REG_SRC-macroes
#define MOV_R32_DWORD(       r32)              B2INSA(0x8B00    | ((r32)<<3))           // Build src with MEM_ADDR-*,REG_SRC-macroes

#define MOV_R8_IMM_BYTE(     r8)               B1INS(0xB0       |  (r8 ))               // 1 byte operand
#define MOV_R16_IMM_WORD(    r16)              B2INS(0x66B8     |  (r16))               // 2 byte operand
#define MOV_R32_IMM_DWORD(   r32)              B1INS(0xB8       |  (r32))               // 4 byte operand

#define MOV_TO_AL_IMM_ADDR_BYTE                B1INS(0xA0)                              // 4/8 byte address. move byte  pointed to by 2. operand to AL
#define MOV_TO_AX_IMM_ADDR_WORD                B2INS(0x66A1)                            // 4/8 byte address. move word  pointed to by 2. operand to AX
#define MOV_TO_EAX_IMM_ADDR_DWORD              B1INS(0xA1)                              // 4/8 byte address. move dword pointed to by 2. operand to EAX
#define MOV_FROM_AL_IMM_ADDR_BYTE              B1INS(0xA2)                              // 4/8 byte address. move AL  to byte  pointed to by 2. operand
#define MOV_FROM_AX_IMM_ADDR_WORD              B2INS(0x66A3)                            // 4/8 byte address. move AX  to word  pointed to by 2. operand
#define MOV_FROM_EAX_IMM_ADDR_DWORD            B1INS(0xA3)                              // 4/8 byte address. move EAX to dword pointed to by 2. operand

#ifdef IS64BIT

#define MOV_R64_QWORD(       r64)              PREFIX48(MOV_R32_DWORD(r64))             // Build src with MEM_ADDR-*,REG_SRC-macroes
#define MOV_QWORD_R64(       r64)              PREFIX48(MOV_DWORD_R32(r64))             // Build dst with MEM_ADDR-*,REG_SRC-macroes

#define MOV_R64_IMM_QWORD(   r64)              PREFIX48(MOV_R32_IMM_DWORD(r64))         // 8 byte operand

#define MOV_TO_RAX_IMM_ADDR_QWORD              PREFIX48(MOV_TO_EAX_IMM_ADDR_DWORD)      // 8 byte address                              // 4/8 byte address. move dword pointed to by 2. operand to EAX
#define MOV_FROM_RAX_IMM_ADDR_QWORD            PREFIX48(MOV_FROM_EAX_IMM_ADDR_DWORD)    // 8 byte address                             // 4 byte address. move EAX to dword pointed to by 2. operand



#endif

#define ADD_BYTE_R8(         r8 )              B2INSA(0x0000    | ((r8 )<<3))           // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define ADD_DWORD_R32(       r32)              B2INSA(0x0100    | ((r32)<<3))           // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define ADD_WORD_R16(        r16)              B3INSA(0x660100  | ((r16)<<3))           // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define ADD_R8_BYTE(         r8 )              B2INSA(0x0200    | ((r8 )<<3))           // Build src with MEM_ADDR-*,REG_SRC-macroes
#define ADD_R32_DWORD(       r32)              B2INSA(0x0300    | ((r32)<<3))           // Build src with MEM_ADDR-*,REG_SRC-macroes
#define ADD_R16_WORD(        r16)              B3INSA(0x660300  | ((r16)<<3))           // Build src with MEM_ADDR-*,REG_SRC-macroes
#define ADD_AL_IMM_BYTE                        B1INS(0x04)                              // 1 byte operand
#define ADD_EAX_IMM_DWORD                      B1INS(0x05)                              // 4 byte operand
#define ADD_AX_IMM_WORD                        B2INS(0x6605)                            // 2 byte operand
#define ADD_R8_IMM_BYTE(     r8 )              B2INS(0x80C0     |  (r8 ))               // 1 byte operand
#define ADD_R32_IMM_DWORD(   r32)              B2INS(0x81C0     |  (r32))               // 4 byte operand
#define ADD_R16_IMM_WORD(    r16)              B3INS(0x6681C0   |  (r16))               // 2 byte operand
#define ADD_R32_IMM_BYTE(    r32)              B2INS(0x83C0     |  (r32))               // 1 byte operand
#define ADD_R16_IMM_BYTE(    r16)              B3INS(0x6683C0   |  (r16))               // 1 byte operand

#define OR_BYTE_R8(          r8 )              B2INSA(0x0800    | ((r8 )<<3))           // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define OR_DWORD_R32(        r32)              B2INSA(0x0900    | ((r32)<<3))           // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define OR_WORD_R16(         r16)              B3INSA(0x660900  | ((r16)<<3))           // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define OR_R8_BYTE(          r8 )              B2INSA(0x0A00    | ((r8 )<<3))           // Build src with MEM_ADDR-*,REG_SRC-macroes
#define OR_R32_DWORD(        r32)              B2INSA(0x0B00    | ((r32)<<3))           // Build src with MEM_ADDR-*,REG_SRC-macroes
#define OR_R16_WORD(         r16)              B3INSA(0x660B00  | ((r16)<<3))           // Build src with MEM_ADDR-*,REG_SRC-macroes
#define OR_AL_IMM_BYTE                         B1INS(0x0C)                              // 1 byte operand
#define OR_EAX_IMM_DWORD                       B1INS(0x0D)                              // 4 byte operand
#define OR_AX_IMM_WORD                         B2INS(0x660D)                            // 2 byte operand
#define OR_R8_IMM_BYTE(      r8 )              B2INS(0x80C8     |  (r8 ))               // 1 byte operand
#define OR_R32_IMM_DWORD(    r32)              B2INS(0x81C8     |  (r32))               // 4 byte operand
#define OR_R16_IMM_WORD(     r16)              B3INS(0x6681C8   |  (r16))               // 2 byte operand
#define OR_R32_IMM_BYTE(     r32)              B2INS(0x83C8     |  (r32))               // 1 byte operand
#define OR_R16_IMM_BYTE(     r16)              B3INS(0x6683C8   |  (r16))               // 1 byte operand

#define AND_BYTE_R8(         r8 )              B2INSA(0x2000    | ((r8 )<<3))           // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define AND_DWORD_R32(       r32)              B2INSA(0x2100    | ((r32)<<3))           // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define AND_WORD_R16(        r16)              B3INSA(0x662100  | ((r16)<<3))           // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define AND_R8_BYTE(         r8 )              B2INSA(0x2200    | ((r8 )<<3))           // Build src with MEM_ADDR-*,REG_SRC-macroes
#define AND_R32_DWORD(       r32)              B2INSA(0x2300    | ((r32)<<3))           // Build src with MEM_ADDR-*,REG_SRC-macroes
#define AND_R16_WORD(        r16)              B3INSA(0x662300  | ((r16)<<3))           // Build src with MEM_ADDR-*,REG_SRC-macroes
#define AND_AL_IMM_BYTE                        B1INS(0x24)                              // 1 byte operand
#define AND_EAX_IMM_DWORD                      B1INS(0x25)                              // 4 byte operand
#define AND_AX_IMM_WORD                        B2INS(0x6625)                            // 2 byte operand
#define AND_R8_IMM_BYTE(     r8 )              B2INS(0x80E0     |  (r8 ))               // 1 byte operand
#define AND_R32_IMM_DWORD(   r32)              B2INS(0x81E0     |  (r32))               // 4 byte operand
#define AND_R16_IMM_WORD(    r16)              B3INS(0x6681E0   |  (r16))               // 2 byte operand
#define AND_R32_IMM_BYTE(    r32)              B2INS(0x83E0     |  (r32))               // 1 byte operand
#define AND_R16_IMM_BYTE(    r16)              B3INS(0x6683E0   |  (r16))               // 1 byte operand

#define SUB_BYTE_R8(         r8 )              B2INSA(0x2800    | ((r8 )<<3))           // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define SUB_DWORD_R32(       r32)              B2INSA(0x2900    | ((r32)<<3))           // Build src with MEM_ADDR-*,REG_SRC-macroes
#define SUB_WORD_R16(        r16)              B3INSA(0x662900  | ((r16)<<3))           // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define SUB_R8_BYTE(         r8 )              B2INSA(0x2A00    | ((r8 )<<3))           // Build src with MEM_ADDR-*,REG_SRC-macroes
#define SUB_R32_DWORD(       r32)              B2INSA(0x2B00    | ((r32)<<3))           // Build src with MEM_ADDR-*,REG_SRC-macroes
#define SUB_R16_WORD(        r16)              B3INSA(0x662B00  | ((r16)<<3))           // Build src with MEM_ADDR-*,REG_SRC-macroes
#define SUB_AL_IMM_BYTE                        B1INS(0x2C)                              // 1 byte operand
#define SUB_EAX_IMM_DWORD                      B1INS(0x2D)                              // 4 byte operand
#define SUB_AX_IMM_WORD                        B2INS(0x662D)                            // 2 byte operand
#define SUB_R8_IMM_BYTE(     r8 )              B2INS(0x80E8     |  (r8 ))               // 1 byte operand
#define SUB_R32_IMM_DWORD(   r32)              B2INS(0x81E8     |  (r32))               // 4 byte operand
#define SUB_R16_IMM_WORD(    r16)              B3INS(0x6681E8   |  (r16))               // 2 byte operand
#define SUB_R32_IMM_BYTE(    r32)              B2INS(0x83E8     |  (r32))               // 1 byte operand
#define SUB_R16_IMM_BYTE(    r16)              B3INS(0x6683E8   |  (r16))               // 1 byte operand

#define XOR_BYTE_R8(         r8 )              B2INSA(0x3000    | ((r8 )<<3))           // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define XOR_DWORD_R32(       r32)              B2INSA(0x3100    | ((r32)<<3))           // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define XOR_WORD_R16(        r16)              B3INSA(0x663100  | ((r16)<<3))           // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define XOR_R8_BYTE(         r8 )              B2INSA(0x3200    | ((r8 )<<3))           // Build src with MEM_ADDR-*,REG_SRC-macroes
#define XOR_R16_WORD(        r16)              B3INSA(0x663300  | ((r16)<<3))           // Build src with MEM_ADDR-*,REG_SRC-macroes
#define XOR_R32_DWORD(       r32)              B2INSA(0x3300    | ((r32)<<3))           // Build src with MEM_ADDR-*,REG_SRC-macroes
#define XOR_AL_IMM_BYTE                        B1INS(0x34)                              // 1 byte operand
#define XOR_EAX_IMM_DWORD                      B1INS(0x35)                              // 4 byte operand
#define XOR_R8_IMM_BYTE(     r8 )              B2INS(0x80F0     |  (r8 ))               // 1 byte operand
#define XOR_AX_IMM_WORD                        B2INS(0x6635)                            // 2 byte operand
#define XOR_R32_IMM_DWORD(   r32)              B2INS(0x81F0     |  (r32))               // 4 byte operand
#define XOR_R16_IMM_WORD(    r16)              B3INS(0x6681F0   |  (r16))               // 2 byte operand
#define XOR_R32_IMM_BYTE(    r32)              B2INS(0x83F0     |  (r32))               // 1 byte operand
#define XOR_R16_IMM_BYTE(    r16)              B3INS(0x6683F0   |  (r16))               // 1 byte operand

#define CMP_BYTE_R8(         r8 )              B2INSA(0x3800    | ((r8 )<<3))           // Build op1 with MEM_ADDR-*,REG_SRC-macroes
#define CMP_DWORD_R32(       r32)              B2INSA(0x3900    | ((r32)<<3))           // Build op1 with MEM_ADDR-*,REG_SRC-macroes
#define CMP_WORD_R16(        r16)              B3INSA(0x663900  | ((r16)<<3))           // Build dst with MEM_ADDR-*,REG_SRC-macroes
#define CMP_R8_BYTE(         r8 )              B2INSA(0x3A00    | ((r8 )<<3))           // Build op2 with MEM_ADDR-*,REG_SRC-macroes
#define CMP_R32_DWORD(       r32)              B2INSA(0x3B00    | ((r32)<<3))           // Build op2 with MEM_ADDR-*,REG_SRC-macroes
#define CMP_R16_WORD(        r16)              B3INSA(0x663B00  | ((r16)<<3))           // Build src with MEM_ADDR-*,REG_SRC-macroes
#define CMP_AL_IMM_BYTE                        B1INS(0x3C)                              // 1 byte operand
#define CMP_EAX_IMM_DWORD                      B1INS(0x3D)                              // 4 byte operand
#define CMP_AX_IMM_WORD                        B2INS(0x663D)                            // 2 byte operand
#define CMP_R8_IMM_BYTE(     r8 )              B2INS(0x80F8     |  (r8 ))               // 1 byte operand
#define CMP_R32_IMM_DWORD(   r32)              B2INS(0x81F8     |  (r32))               // 4 byte operand
#define CMP_R16_IMM_WORD(    r16)              B3INS(0x6681F8   |  (r16))               // 2 byte operand
#define CMP_R32_IMM_BYTE(    r32)              B2INS(0x83F8     |  (r32))               // 1 byte operand
#define CMP_R16_IMM_BYTE(    r16)              B3INS(0x6683F8   |  (r16))               // 1 byte operand

#define MUL_BYTE                               B2INSA(0xF620)                           // Unsigned multiply (ax      = al  * src  )
#define MUL_DWORD                              B2INSA(0xF720)                           //                   (edx:eax = eax * src  )
#define MUL_WORD                               B3INSA(0x66F720)                         //                   (dx:ax   = ax  * src  )

#define IMUL_BYTE                              B2INSA(0xF628)                           // Signed multiply   (ax      = al  * src  )
#define IMUL_DWORD                             B2INSA(0xF728)                           //                   (edx:eax = eax * src  )
#define IMUL_WORD                              B3INSA(0x66F728)                         //                   (dx:ax   = ax  * src  )

// Additional forms of IMUL
#define IMUL2_R32_DWORD(      r32)             B3INSA(0x0FAF00    | ((r32)<<3))         // 2 arguments       (r32 *= src           )
#define IMUL2_R16_WORD(       r16)             B4INSA(0x660FAF00  | ((r16)<<3))         //                   (r16 *= src           )

#define IMUL3_DWORD_IMM_DWORD(r32)             B2INSA(0x6900      | ((r32)<<3))         // 3 args, r32,src,4 byte operand (r32 = src * imm.dword)
#define IMUL3_DWORD_IMM_BYTE( r32)             B2INSA(0x6B00      | ((r32)<<3))         // 3 args. r32.src.1 byte operand (r32 = src * imm.byte )

#define IMUL3_WORD_IMM_WORD(  r16)             B3INSA(0x666900    | ((r16)<<3))         // 2 byte operand    (r16 = src * imm word )
#define IMUL3_WORD_IMM_BYTE(  r16)             B3INSA(0x666B00    | ((r16)<<3))         // 1 byte operand    (r16 = src * imm byte )

#define DIV_BYTE                               B2INSA(0xF630)                           // Unsigned divide ax      /= src, Result:al = quot. ah = rem
#define DIV_DWORD                              B2INSA(0xF730)                           //                 dk:ax   /= src. Result:ax = quot. dx = rem
#define DIV_WORD                               B3INSA(0x66F730)                         //                 edx:eax /= src. Result:eax= quot. edx= rem

#define IDIV_BYTE                              B2INSA(0xF638)                           // Signed divide   ax      /= src, ah  must contain sign extension of al. Result:al = quot. ah = rem
#define IDIV_DWORD                             B2INSA(0xF738)                           //                 dk:ax   /= src. dx  must contain sign extension of ax. Result:ax = quot. dx = rem
#define IDIV_WORD                              B3INSA(0x66F738)                         //                 edx:eax /= src. edx must contain sign extension of ax. Result:eax= quot. edx= rem

#define NOT_BYTE                               B2INSA(0xF610)
#define NOT_DWORD                              B2INSA(0xF710)
#define NOT_WORD                               B3INSA(0x66F710)

#define NEG_BYTE                               B2INSA(0xF618)
#define NEG_DWORD                              B2INSA(0xF718)
#define NEG_WORD                               B3INSA(0x66F718)

#define INC_BYTE                               B2INSA(0xFE00)
#define INC_DWORD                              B2INSA(0xFF00)
#define INC_WORD                               B3INSA(0x66FF00)

#define DEC_BYTE                               B2INSA(0xFE08)
#define DEC_WORD                               B3INSA(0x66FF08)
#define DEC_DWORD                              B2INSA(0xFF08)

#ifdef IS32BIT

#define INC_R32(r32)                           B1INS(0x40       | (r32))
#define DEC_R32(r32)                           B1INS(0x48       | (r32))
#define INC_R16(r16)                           B2INS(0x6640     | (r16))
#define DEC_R16(r16)                           B2INS(0x6648     | (r16))

#else // IS64BIT

#define MOVSD_XMM_MMWORD(xmm)                  B4INSA(0xF20F1000 | ((xmm) << 3))        // Build src with MEM_ADDR-*,REG_SRC-macroes
#define MOVSD_MMWORD_XMM(xmm)                  B4INSA(0xF20F1100 | ((xmm) << 3))        // Build dst with MEM_ADDR-*,REG_SRC-macroes

#define MOVEAPS(xmmDst, xmmSrc)                B3INS(0x0F28C0    | ((xmmDst)<<3) | (xmmSrc))

#define ADDSD(xmm)                             B4INSA(0xF20F5800 | ((xmm) << 3))        // Build src with MEM_ADDR-*,REG_SRC-macroes
#define MULSD(xmm)                             B4INSA(0xF20F5900 | ((xmm) << 3))         
#define SUBSD(xmm)                             B4INSA(0xF20F5C00 | ((xmm) << 3))         
#define DIVSD(xmm)                             B4INSA(0xF20F5E00 | ((xmm) << 3))         


#define ADD_QWORD_R64(       r64)              PREFIX48(ADD_DWORD_R32(    r64))
#define ADD_R64_QWORD(       r64)              PREFIX48(ADD_R32_DWORD(    r64))
#define ADD_R64_IMM_DWORD(   r64)              PREFIX48(ADD_R32_IMM_DWORD(r64))         // 4 byte operand
#define ADD_R64_IMM_BYTE(    r64)              PREFIX48(ADD_R32_IMM_BYTE( r64))

#define OR_QWORD_R64(        r64)              PREFIX48(OR_DWORD_R32(     r64))
#define OR_R64_QWORD(        r64)              PREFIX48(OR_R32_DWORD(     r64))
#define OR_R64_IMM_DWORD(    r64)              PREFIX48(OR_R32_IMM_DWORD( r64))         // 4 byte operand

#define AND_QWORD_R64(       r64)              PREFIX48(AND_DWORD_R32(    r64))
#define AND_R64_QWORD(       r64)              PREFIX48(AND_R32_DWORD(    r64))
#define AND_R64_IMM_DWORD(   r64)              PREFIX48(AND_R32_IMM_DWORD(r64))         // 4 byte operand

#define SUB_QWORD_R64(       r64)              PREFIX48(SUB_DWORD_R32(    r64))
#define SUB_R64_QWORD(       r64)              PREFIX48(SUB_R32_DWORD(    r64))
#define SUB_R64_IMM_DWORD(   r64)              PREFIX48(SUB_R32_IMM_DWORD(r64))         // 4 byte operand
#define SUB_R64_IMM_BYTE(    r64)              PREFIX48(SUB_R32_IMM_BYTE( r64))

#define XOR_QWORD_R64(       r64)              PREFIX48(XOR_DWORD_R32(    r64))
#define XOR_R64_QWORD(       r64)              PREFIX48(XOR_R32_DWORD(    r64))
#define XOR_R64_IMM_DWORD(   r64)              PREFIX48(XOR_R32_IMM_DWORD(r64))         // 4 byte operand

#define CMP_QWORD_R64(       r64)              PREFIX48(CMP_DWORD_R32(    r64))
#define CMP_R64_QWORD(       r64)              PREFIX48(CMP_R32_DWORD(    r64))
#define CMP_R64_IMM_DWORD(   r64)              PREFIX48(CMP_R32_IMM_DWORD(r64))         // 4 byte operand

#define MUL_QWORD                              PREFIX48(MUL_DWORD)                      // (rdx:rax = rax * src  )
#define IMUL_QWORD                             PREFIX48(IMUL_DWORD)                     // (rdx:rax = rax * src  )

#define IMUL2_R64_DWORD(      r64)             PREFIX48(IMUL2_R32_DWORD(r64)            // 2 arguments       (r64 *= src           )

#define IMUL3_QWORD_IMM_DWORD(r64)             PREFIX48(IMUL3_DWORD_IMM_DWORD(r64))     // 3 args, r64,src,4 byte operand (r64 = src * imm.dword)
#define IMUL3_QWORD_IMM_BYTE( r64)             PREFIX48(IMUL3_DWORD_IMM_BYTE(R64))      // 3 args. r64.src.1 byte operand (r64 = src * imm.byte )

#endif // IS32BIT


#define CWDE                                   B1INS(0x98)                              // Convert word to dword   Copy sign (bit 15) of AX  into higher 16 bits of EAX
#define CBW                                    B2INS(0x6698)                            // Convert byte to word    Copy sign (bit 7)  of AL  into every bit of AH
#define CDQ                                    B1INS(0x99)                              // Convert dword to qword  Copy sign (bit 31) of EAX into every bit of EDX
#define CWD                                    B2INS(0x6699)                            // Convert word to dword   Copy sign (bit 15) of AX  into every bit of DX

// PUSH/POP _R8 not available

#ifdef IS32BIT

#define PUSH_R16(r16)                          B2INS(0x6650     | (r16))                // No operand
#define POP_R16( r16)                          B2INS(0x6658     | (r16))                // No operand

#define PUSH_R32(r32)                          B1INS(0x50       | (r32))                // No operand
#define POP_R32( r32)                          B1INS(0x58       | (r32))                // No operand

#define PUSH_BYTE                              B1INS(0x6A    )                          // 1 byte value
#define PUSH_DWORD                             B1INS(0x68    )                          // 4 byte value
#define PUSH_WORD                              B2INS(0x6668  )                          // 2 byte value

#else // IS64BIT

#define PUSH_R64(r64)                          B1INS(0x50       | (r64))                // No operand
#define POP_R64( r64)                          B1INS(0x58       | (r64))                // No operand

#endif // IS32BIT

#define REP_MOVS_BYTE                          B2INS(0xF3A4  )
#define REP_MOVS_DWORD                         B2INS(0xF3A5  )
#define REP_MOVS_WORD                          B3INS(0xF366A5)


// FPU instructionss

#define FADD_0i(  i)                           FPUINS(0xD8C0     | (i))                  // st(0) += st(i)
#define FADD_i0(  i)                           FPUINS(0xDCC0     | (i))                  // st(i) += st(0)
#define FADDP_i0( i)                           FPUINS(0xDEC0     | (i))                  // st(i) += st(0); pop st(0)
#define FADD                                   FADDP_i0(1)

#define FMUL_0i(  i)                           FPUINS(0xD8C8     | (i))                  // st(0) *= st(i)
#define FMUL_i0(  i)                           FPUINS(0xDCC8     | (i))                  // st(i) *= st(0)
#define FMULP_i0( i)                           FPUINS(0xDEC8     | (i))                  // st(i) *= st(0); pop st(0)
#define FMUL                                   FMULP_i0(1)

#define FCOMP(    i)                           FPUINS(0xD8D8     | (i))                  // Compare st(0) to st(1..7)
#define FCOMPP                                 FPUINS(0xDED9)                            // Compare st(0) to st(1); pop both

#define FSUB_0i(  i)                           FPUINS(0xD8E0     | (i))                  // st(0) -= st(i)
#define FSUBR_0i( i)                           FPUINS(0xD8E8     | (i))                  // st(0) =  st(i) - st(0)
#define FSUBR_i0( i)                           FPUINS(0xDCE0     | (i))                  // st(i) =  st(0) - st(i)
#define FSUB_i0(  i)                           FPUINS(0xDCE8     | (i))                  // st(i) -= st(0)
#define FSUBRP_i0(i)                           FPUINS(0xDEE0     | (i))                  // st(i) =  st(0) - st(i); pop st(0)
#define FSUBP_i0( i)                           FPUINS(0xDEE8     | (i))                  // st(i) -= st(0); pop st(0)
#define FSUB                                   FSUBP_i0(1)

#define FDIV_0i(  i)                           FPUINS(0xD8F0     | (i))                  // st(0) /= st(i)
#define FDIVR_0i( i)                           FPUINS(0xD8F8     | (i))                  // st(0) =  st(i) / st(0)
#define FDIVR_i0( i)                           FPUINS(0xDCF0     | (i))                  // st(i) =  st(0) / st(i)
#define FDIV_i0(  i)                           FPUINS(0xDCF8     | (i))                  // st(i) /= st(0)
#define FDIVRP_i0(i)                           FPUINS(0xDEF0     | (i))                  // st(i) =  st(0) / st(i); pop st(0)
#define FDIVP_i0( i)                           FPUINS(0xDEF8     | (i))                  // st(i) /= st(0); pop st(0)
#define FDIV                                   FDIVP_i0(1)

#define FCOMI(    i)                           FPUINS(0xDBF0     | (i))                  // Compare st(0) to st(i) and set CPU-flags
#define FCOMIP(   i)                           FPUINS(0xDFF0     | (i))                  // Compare st(0) to st(i) and set CPU-flags; pop st(0)
#define FST(      i)                           FPUINS(0xDDD0     | (i))                  // Store st(0) into st(i)
#define FSTP(     i)                           FPUINS(0xDDD8     | (i))                  // Store st(0) into st(i) and pop st(0)

#define FLD(      i)                           FPUINS(0xD9C0     | (i))                  // Push st(i) into st(0)
#define FXCH(     i)                           FPUINS(0xD9C8     | (i))                  // Swap st(0) and st(i)
#define FCHS                                   FPUINS(0xD9E0)                            // st(0) = -st(0)
#define FTST                                   FPUINS(0xD9E4)                            // Compare st(0) to 0.0
#define FLD1                                   FPUINS(0xD9E8)                            // push 1.0
#define FLDL2T                                 FPUINS(0xD9E9)                            // push log2(10)
#define FLDL2E                                 FPUINS(0xD9EA)                            // push log2(e)
#define FLDPI                                  FPUINS(0xD9EB)                            // push pi
#define FLDLG2                                 FPUINS(0xD9EC)                            // push log10(2)
#define FLDLN2                                 FPUINS(0xD9ED)                            // push ln(2)
#define FLDZ                                   FPUINS(0xD9EE)                            // push 0.0

#define FNSTSW_AX                              FPUINS(0xDFE0)                            // Store status word into CPU register AX
#define SAHF                                   B1INS( 0x9E	)

// These opcodes should all be used with MEM_ADDR_* to get the various addressing-modes

#define FLDCW_WORD                             FPUINSA(0xD928)                           // load control word
#define FNSTCW_WORD                            FPUINSA(0xD938)                           // store control word
#define FNSTSW_WORD                            FPUINSA(0xDD38)                           // store status word

// Real4 (float)
#define FLD_DWORD                              FPUINSA(0xD900)
#define FST_DWORD                              FPUINSA(0xD910)
#define FSTP_DWORD                             FPUINSA(0xD918)

// Real8 (double)
#define FLD_QWORD                              FPUINSA(0xDD00)
#define FST_QWORD                              FPUINSA(0xDD10)
#define FSTP_QWORD                             FPUINSA(0xDD18)

// Real10 (Double80)
#define FLD_TBYTE                              FPUINSA(0xDB28)
#define FSTP_TBYTE                             FPUINSA(0xDB38)

// 16-bit integer (short)
#define FILD_WORD                              FPUINSA(0xDF00)
#define FIST_WORD                              FPUINSA(0xDF10)
#define FISTP_WORD                             FPUINSA(0xDF18)

// 32-bit integer (int)
#define FILD_DWORD                             FPUINSA(0xDB00)
#define FIST_DWORD                             FPUINSA(0xDB10)
#define FISTP_DWORD                            FPUINSA(0xDB18)

// 64-bit integer (__int64)
#define FILD_QWORD                             FPUINSA(0xDF28)
#define FISTP_QWORD                            FPUINSA(0xDF38)

// Real4 (float)
#define FADD_DWORD                             FPUINSA(0xD800)
#define FMUL_DWORD                             FPUINSA(0xD808)
#define FCOM_DWORD                             FPUINSA(0xD810)
#define FCOMP_DWORD                            FPUINSA(0xD818)
#define FSUB_DWORD                             FPUINSA(0xD820)
#define FSUBR_DWORD                            FPUINSA(0xD828)
#define FDIV_DWORD                             FPUINSA(0xD830)
#define FDIVR_DWORD                            FPUINSA(0xD838)

// Real8 (double)
#define FADD_QWORD                             FPUINSA(0xDC00)
#define FMUL_QWORD                             FPUINSA(0xDC08)
#define FCOM_QWORD                             FPUINSA(0xDC10)
#define FCOMP_QWORD                            FPUINSA(0xDC18)
#define FSUB_QWORD                             FPUINSA(0xDC20)
#define FSUBR_QWORD                            FPUINSA(0xDC28)
#define FDIV_QWORD                             FPUINSA(0xDC30)
#define FDIVR_QWORD                            FPUINSA(0xDC38)

// 16-bit integer (short)
#define FIADD_WORD                             FPUINSA(0xDE00)
#define FIMUL_WORD                             FPUINSA(0xDE08)
#define FICOM_WORD                             FPUINSA(0xDE10)
#define FICOMP_WORD                            FPUINSA(0xDE18)
#define FISUB_WORD                             FPUINSA(0xDE20)
#define FISUBR_WORD                            FPUINSA(0xDE28)
#define FIDIV_WORD                             FPUINSA(0xDE30)
#define FIDIVR_WORD                            FPUINSA(0xDE38)

// 32-bit integer (int)
#define FIADD_DWORD                            FPUINSA(0xDA00)
#define FIMUL_DWORD                            FPUINSA(0xDA08)
#define FICOM_DWORD                            FPUINSA(0xDA10)
#define FICOMP_DWORD                           FPUINSA(0xDA18)
#define FISUB_DWORD                            FPUINSA(0xDA20)
#define FISUBR_DWORD                           FPUINSA(0xDA28)
#define FIDIV_DWORD                            FPUINSA(0xDA30)
#define FIDIVR_DWORD                           FPUINSA(0xDA38)

