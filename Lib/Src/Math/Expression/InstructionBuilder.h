#pragma once

#include <Math/Expression/NewOpcode.h>
#include "RexByte.h"

// Encoding of various addressing modes is described here:
// http://www.c-jump.com/CIS77/CPU/x86/index.html
//
//                                     7 6 5 4 3 2 1 0
// MOD-REG-R/M Byte encoding. Layout: [m m r r r / / /]  (m:MOD field, r:REG, /:R/M
// MOD field values {0..3]
#define DP_0BYTE 0 // R/M is memory-operand (no displacement) or if R/M=5, then displacement only
#define DP_1BYTE 1 // R/M is memory-operand is followed by 1 byte  signed displacement
#define DP_4BYTE 2 // R/M is memory-operand is followed by 4 bytes signed displacement
#define REG_MODE 3 // R/M-field is index of a GP-register (8-,16-,32-bit (or 64 in 64-mode)

// For MOD = [0..2] && (R/M == 4) => SIB-byte added. See below
// For MOD = [0..2] && (R/M != 4), then R/M is 3-bit index of 32-bit register
//
//  General Purpose Registers: 0   1   2   3   4   5   6   7
//  8-bit registers          : al  cl  dl  bl  ah  ch  dh  bh
// 16-bit registers          : ax  cx  dx  bx  sp  bp  si  di
// 32-bit registers          :eax ecx edx ebx esp ebp esi edi
//
//
// REG field indicates source (or destination) field, in 2 operand instructions
// For 1 operand instructions, REG-field contains opcode-extension.
// If d-bit=0, (direction-bit,bit 1 in opcode), then REG is source and R/M is destination
// If d-bit=1, then REG is destination, R/M is source
// s-bit=0, (size-bit, bit 0 in opcode) indicates 8-bit operands
// s-bit=1, indicates 32-bit operands (or 16, if prefix 0x66 is used)
// In x64-mode, REX-byte prefix will extend this further. See RexByte.h
#define ENC_MODREG_BYTE(mod,reg,rm)    (((mod)<<6) | (((reg)&7)<<3) | ((rm)&7))

#define MR_DP1BYTE(index  ) ENC_MODREG_BYTE(DP_1BYTE,0,index)
#define MR_DP4BYTE(index  ) ENC_MODREG_BYTE(DP_4BYTE,0,index)
#define MR_REGREG( dst,src) ENC_MODREG_BYTE(REG_MODE,dst,src)
#define MR_SIB(    disp   ) ENC_MODREG_BYTE(disp,0,4)
#define MR_DISPONLY         ENC_MODREG_BYTE(0   ,0,5)
#define MR_REG(    reg    ) MR_REGREG(0,reg)
//                                  7 6 5 4 3 2 1 0
// SIB (Scaled Index Byte) Layout: [s s i i i b b b]  (s:shift, i:inx, b:base)
// inx != 4, shift=[0..3]. Address calculated as reg[base]+(reg[inx]<<shift) (+ displacement if any)
// Where reg is 1 of 32-bit registers (or 64-bit in x64-mode)
// Displacement only if(MOD==0 && R/M==5), inx==5 only if MOD = {1,2}
#define SIB_BYTE(base,inx,shift)   (((shift)<<6)|(((inx)&7)<<3)|((base)&7))

class InstructionBuilder : public InstructionBase {
private:
  // Contains operand-sizes, that requires sizeBit=1 (if it exist)
  static const RegSizeSet s_sizeBitSet;
  const UINT   m_flags;
  const BYTE   m_extension;
  BYTE         m_opcodePos;
  const BYTE   m_opcodeSize;
  // Number of operands
  const BYTE   m_opCount;
  // is MOD-REG-R/M byte added
  bool         m_hasModeByte;
#ifdef IS64BIT
  // See RexByte.h
  bool         m_hasRexByte;
  BYTE         m_rexByteIndex;
#endif
  inline void init() {
    m_opcodePos    = 0;
    m_hasModeByte  = false;
#ifdef IS64BIT
    m_hasRexByte   = false;
    m_rexByteIndex = 0;
#endif
  }
  // Use with Imm-addressing. reg = { ES,CS,SS,DS,FS,GS }
  // ptr[(reg<<shift)+offset], (reg&7) != 4, shift<=3, offset=[INT_MIN;INT_MAX]
  InstructionBuilder &addrShiftInx(    const IndexRegister &inx, BYTE shift, int offset);
  InstructionBuilder &addrBase(        const IndexRegister &base, int offset);
  InstructionBuilder &addrBaseShiftInx(const IndexRegister &base, const IndexRegister &inx, BYTE shift, int offset);
  inline InstructionBuilder &addrDisplaceOnly(int displacement) {
    return setModeBits(MR_DISPONLY).add((BYTE*)&displacement, 4);
  }
  inline InstructionBuilder &addImmAddr(size_t addr) {
    return add((BYTE*)&addr, sizeof(addr));
  }
  inline void addExtension() {
    setModeBits(m_extension << 3);
  }
  InstructionBuilder &prefixSegReg(    const SegmentRegister &reg);
  inline bool isWordPtrOnly() const {
    return ISWORDPTR_ONLY(m_flags);
  }
  inline bool isWordGPROnly() const {
    return ISWORDGPR_ONLY(m_flags);
  }
  inline bool isWordSizeOnly(bool isRegister) const {
    return isRegister ? isWordGPROnly() : isWordPtrOnly();
  }
  InstructionBuilder &prefixImm(BYTE op, OperandSize size, bool isRegister, bool immIsByte);
protected:
  static void throwImmSizeException(const TCHAR *method, const String        &dst, INT64 immv) {
    throwInvalidArgumentException(method,_T("%s"),getImmSizeErrorString(dst,immv).cstr());
  }
  static inline void sizeError(     const TCHAR *method, const Register      &reg, INT64 immv) {
    throwImmSizeException(method,reg.getName(),immv);
  }
  static inline void sizeError(     const TCHAR *method, const MemoryOperand &mem, INT64 immv) {
    throwImmSizeException(method,mem.toString(),immv);
  }
  static inline void sizeError(     const TCHAR *method, OperandSize         size, INT64 immv) {
    throwImmSizeException(method,::toString(size),immv);
  }
  inline InstructionBuilder &prefixImm(BYTE op, const Register      &reg, bool immIsByte) {
    return prefixImm(op,reg.getSize(),true, immIsByte);
  }
  inline InstructionBuilder &prefixImm(BYTE op, const MemoryOperand &mem, bool immIsByte) {
    return prefixImm(op,mem.getSize(),false, immIsByte);
  }
public:
  InstructionBuilder(const OpcodeBase      &opcode);
  InstructionBuilder(const InstructionBase &ins   , UINT flags = 0);

  inline BYTE getOpcodePos() const {
    return m_opcodePos;
  }
  inline BYTE getOpcodeSize() const {
    return m_opcodeSize;
  }
  inline BYTE getLastOpcodeByteIndex() const {
    return getOpcodePos() + getOpcodeSize() - 1;
  }
  // Return index of MOD-REG-R/M byte. Assume it exist
  inline BYTE getModeByteIndex() const {
    return getOpcodePos() + getOpcodeSize();
  }
  inline bool hasSizeBit() const {
    return (m_flags & HAS_SIZEBIT) != 0;
  }
  inline bool hasDirectionBit() const {
    return (m_flags & HAS_DIRECTIONBIT) != 0;
  }
  inline bool hasModeByte() const {
    return m_hasModeByte;
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
    assert(index < m_size);
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
    assert(index < m_size);
    m_bytes[index] ^= b;
    return *this;
  }
  inline InstructionBuilder &add(BYTE b) {
    assert(m_size < MAX_INSTRUCTIONSIZE);
    m_bytes[m_size++] = b;
    return *this;
  }
  InstructionBuilder &add(const BYTE *src, BYTE count);
  inline InstructionBuilder &add(const InstructionBase &ins) {
    return add(ins.getBytes(),ins.size());
  }
  inline bool hasRexByte() const {
#ifdef IS32BIT
    return false;
#else
    return m_hasRexByte ? true : false;
#endif
  }
#ifdef IS64BIT
  // See RexByte.h
  InstructionBuilder &setRexBits(BYTE bits);
#endif // IS64BIT

  inline InstructionBuilder &wordIns(bool isRegister) {
    if(isWordSizeOnly(isRegister)) return *this;
#ifdef IS64BIT
    m_rexByteIndex++;
#endif
    return prefix(0x66);
  }

  static inline bool needSizeBit(OperandSize size) {
    return s_sizeBitSet.contains(size);
  }
  // Set bit 0 in opcode to 1 for for 16-, 32- (and 64- bit operands)
  inline InstructionBuilder &setSizeBit() {
    return hasSizeBit() ? or(m_opcodePos,1) : *this;
  }
  InstructionBuilder &setOperandSize(OperandSize size, bool isRegister);
  // Set bit 1 in opcode to 1 if destination is a register
  inline InstructionBuilder &setDirectionBit() {
    return hasDirectionBit() ? or(m_opcodePos,2) : *this;
  }
  // add MOD-REG-R/M byte if not there yet, else modeByte |= bits
  InstructionBuilder &setModeBits(BYTE bits);
  InstructionBuilder &setRegisterOperand(  const GPRegister    &reg);
  InstructionBuilder &setMemoryOperand(    const MemoryOperand &mem);
  InstructionBuilder &setMemoryRegOperands(const MemoryOperand &mem, const  Register &reg);
  InstructionBuilder &setRegRegOperands(   const Register      &reg1, const Register &reg2);
};
