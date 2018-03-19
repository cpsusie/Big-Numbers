#pragma once

#include <Math/Expression/NewOpcode.h>
#include "RexByte.h"

// MOD-REG-R/M Byte encoding
// MOD field values {0..3]
#define DP_0BYTE 0 // 
#define DP_1BYTE 1 // memory-operand is followed by 1 byte  signed displacement
#define DP_4BYTE 2 // memory-operand is followed by 4 bytes signed displacement
#define REG_MODE 3 // rm-field ofthis byte, is 3-bit register index

#define ENC_MODREG_BYTE(mod,reg,rm)    (((mod)<<6) | (((reg)&7)<<3) | ((rm)&7))

#define MR_DP1BYTE(index  ) ENC_MODREG_BYTE(DP_1BYTE,0,index)
#define MR_DP4BYTE(index  ) ENC_MODREG_BYTE(DP_4BYTE,0,index)
#define MR_REGREG( dst,src) ENC_MODREG_BYTE(REG_MODE,dst,src)
#define MR_SIB(    disp   ) ENC_MODREG_BYTE(disp,0,4)
#define MR_DISPONLY         ENC_MODREG_BYTE(0   ,0,5)

// SIB (Scaled Index Byte) Layout. index != 4, shift=[0..3]
// Displacement only if MOD = 0, (inx&7)==5 only if MOD = {1,2}
#define SIB_BYTE(base,inx,shift)   (((shift)<<6)|(((inx)&7)<<3)|((base)&7))

class InstructionBuilder : public InstructionBase {
private:
  static const RegSizeSet s_sizeBitSet;
  BYTE          m_opcodePos;
  const UINT    m_opcodeSize      : 2;
  // Number of operands
  const UINT    m_opCount         : 3;
#ifdef IS64BIT
  UINT          m_hasRexByte      : 1;
  UINT          m_rexByteIndex    : 2;
#endif
  inline void init() {
    m_opcodePos    = 0;
#ifdef IS64BIT
    m_hasRexByte   = 0;
    m_rexByteIndex = 0;
#endif
  }
  // Use with Imm-addressing. reg = { ES,CS,SS,DS,FS,GS }
  // ptr[(reg<<shift)+offset], (reg&7) != 4, shift<=3, offset=[INT_MIN;INT_MAX]
  InstructionBuilder &addrShiftInx(    const IndexRegister &inx, BYTE shift, int offset);
  InstructionBuilder &addrBase(        const IndexRegister &base, int offset);
  InstructionBuilder &addrBaseShiftInx(const IndexRegister &base, const IndexRegister &inx, BYTE shift, int offset);
  inline InstructionBuilder &addrDisplaceOnly(int addr) {
    return or(MR_DISPONLY).add(addr, 4);
  }

  InstructionBuilder &prefixSegReg(    const SegmentRegister &reg);
protected:
  static void sizeError(const TCHAR *method, const GPRegister    &reg  , INT64 immv);
  static void sizeError(const TCHAR *method, const MemoryOperand &memop, INT64 immv);

public:
  InstructionBuilder(const OpcodeBase      &opcode);
  InstructionBuilder(const Instruction0Arg &ins0  );

  inline BYTE getOpcodePos() const {
    return m_opcodePos;
  }
  inline BYTE getOpcodeSize() const {
    return m_opcodeSize;
  }
  inline BYTE getLastOpcodeByteIndex() const {
    return getOpcodePos() + getOpcodeSize() - 1;
  }
  // Return index of first byte following opcode
  inline BYTE getArgIndex() const {
    return getOpcodePos() + getOpcodeSize();
  }
  InstructionBuilder &insert(BYTE index, BYTE b);
  inline InstructionBuilder &prefix(BYTE b) {
    return insert(0,b);
  }
  // m_bytes[m_size-1] |= b
  inline InstructionBuilder &or(BYTE b) {
    m_bytes[m_size - 1] |= b;
    return *this;
  }
  // m_bytes[index] |= b
  inline InstructionBuilder &or(BYTE index, BYTE b) {
    assert(m_size < MAX_INSTRUCTIONSIZE);
    m_bytes[index] |= b;
    return *this;
  }
  // m_bytes[m_size-1] ^= b
  inline InstructionBuilder &xor(BYTE b) {
    m_bytes[m_size - 1] ^= b;
    return *this;
  }
  // m_bytes[index] ^= b
  inline InstructionBuilder &xor(BYTE index, BYTE b) {
    assert(m_size < MAX_INSTRUCTIONSIZE);
    m_bytes[index] ^= b;
    return *this;
  }
  inline InstructionBuilder &add(BYTE b) {
    assert(m_size < MAX_INSTRUCTIONSIZE);
    m_bytes[m_size++] = b;
    return *this;
  }
  InstructionBuilder &add(INT64 bytesToAdd, BYTE count);

  inline bool hasRexByte() const {
#ifdef IS32BIT
    return false;
#else
    return m_hasRexByte ? true : false;
#endif
  }
#ifdef IS64BIT
  // Bits can be 0. may use bit 0..3 only.
  // bit 0: (srcreg.index()>7)?1:0
  // bit 1:
  // bit 2: (dstreg.index()>7)?1:0
  // bit 3: (operandSize is QWORD)?1:0
  InstructionBuilder &setRexBits(BYTE bits);
#endif // IS64BIT

  inline InstructionBuilder &wordIns() {
#ifdef IS64BIT
    m_rexByteIndex++;
#endif
    return prefix(0x66);
  }

  // Set bit 0 in opcode to 1 for for 16-, 32- (and 64- bit operands)
  inline InstructionBuilder &setSizeBit() {
    return or(m_opcodePos,1);
  }
  // Set bit 1 in opcode to 1 if destination is a register
  inline InstructionBuilder &setDirectionBit() {
    return or(m_opcodePos,2);
  }
  static inline bool needSizeBit(OperandSize size) {
    return s_sizeBitSet.contains(size);
  }
  InstructionBuilder &setMemoryOperand(const MemoryOperand &mop);
  InstructionBuilder &addMemoryOperand(const MemoryOperand &mop);
};
