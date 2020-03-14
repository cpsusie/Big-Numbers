#pragma once

#include <CompactHashMap.h>

class OpcodeBase;
class InstructionOperand;

namespace Expr {

typedef enum {
  _FLD       ,_FSTP      ,_FST       ,_FADD      ,_FMUL
 ,_FSUB      ,_FDIV      ,_FSUBR     ,_FDIVR     ,_FADDP
 ,_FMULP     ,_FSUBP     ,_FDIVP     ,_FSUBRP    ,_FDIVRP
 ,_FCOM      ,_FCOMI     ,_FUCOM     ,_FUCOMI    ,_FCOMP
 ,_FCOMIP    ,_FUCOMP    ,_FUCOMIP   ,_FCOMPP    ,_FUCOMPP
 ,_FILD      ,_FISTP     ,_FISTTP    ,_FIST      ,_FIADD
 ,_FIMUL     ,_FISUB     ,_FIDIV     ,_FISUBR    ,_FIDIVR
 ,_FICOM     ,_FICOMP    ,_FCMOVB    ,_FCMOVAE   ,_FCMOVE
 ,_FCMOVNE   ,_FCMOVBE   ,_FCMOVA    ,_FCMOVU    ,_FCMOVNU
 ,_FFREE     ,_FXCH      ,_FWAIT     ,_FNOP      ,_FCHS
 ,_FABS      ,_FTST      ,_FXAM      ,_FLD1      ,_FLDL2T
 ,_FLDL2E    ,_FLDPI     ,_FLDLG2    ,_FLDLN2    ,_FLDZ
 ,_F2XM1     ,_FYL2X     ,_FPTAN     ,_FPATAN    ,_FXTRACT
 ,_FPREM1    ,_FDECSTP   ,_FINCSTP   ,_FPREM     ,_FYL2XP1
 ,_FSQRT     ,_FSINCOS   ,_FRNDINT   ,_FSCALE    ,_FSIN
 ,_FCOS      ,_CALL      ,_NOTFPU=-1
} FPUOpcodeKey;

class FPUOpcodeHashMap : public CompactStrHashMap<FPUOpcodeKey> {
public:
  FPUOpcodeHashMap(size_t capacity);
  // Returns the corresponding FPUOpcodeKey if opcode is for FPU. _NOTFPU if not
  FPUOpcodeKey getOpcodeKey(const OpcodeBase &opcode) const;
};

typedef enum { // all must be negative. saved in an int, where a value>=0 will be interpreted as valueIndex
  SLOT_EMPTY = -1
 ,SLOT_MIXED = -2
 ,TEMPVAR    = -3
 ,CONST_0    = -4
 ,CONST_1    = -5
 ,CONST_PI   = -6
} FPUSlotSymbol;

class FPUSlot {
private:
  int  m_value;
public:
  inline FPUSlot() {
    setSymbol(SLOT_EMPTY);
  }
  inline bool isEmpty() const {
    return m_value == SLOT_EMPTY;
  }
  inline bool isMixed() const {
    return m_value == SLOT_MIXED;
  }
  inline void setSymbol(FPUSlotSymbol symbol) {
    m_value = symbol;
  }
  inline void setValueIndex(UINT index) {
    m_value = index;
  }
  inline bool isIndex() const {
    return m_value >= 0;
  }
  inline int getValueIndex() const {
    assert(isIndex());
    return m_value;
  }
  String toString() const;
};

class FPUState {
  char          m_height; // Number of registers pushed.               m_height=[0..8]
  BYTE          m_bottom; // Index of slot at the bottom of the stack. m_bottom=[0..7]
  FPUSlot       m_slot[8];
  bool          m_changed;

#define CHECKCONDITION(c) { if(!(c)) throwException(_T("%s(%d):%s:%s"), __TFILE__,__LINE__,__TFUNCTION__,_T(#c)); }

  inline BYTE getSlotIndex(BYTE index) const {
    return (m_bottom + index + 1 - m_height) & 7;
  }

  inline UINT getHeight() const {
    return m_height;
  }
  inline bool isEmpty() const {
    return getHeight() == 0;
  }
  inline FPUSlot &ST(BYTE index) {
    CHECKCONDITION(index < getHeight());
    return m_slot[getSlotIndex(index)];
  }
  inline const FPUSlot &ST(BYTE index) const {
    CHECKCONDITION(index < getHeight());
    return m_slot[getSlotIndex(index)];
  }
  inline void push(UINT valueIndex) {
    CHECKCONDITION(getHeight() < 8);
    m_height++;
    ST(0).setValueIndex(valueIndex);
    m_changed = true;
  }
  inline void push(FPUSlotSymbol symbol) {
    CHECKCONDITION(getHeight() < 8);
    m_height++;
    ST(0).setSymbol(symbol);
    m_changed = true;
  }
  inline void push(const FPUSlot &st) {
    CHECKCONDITION(getHeight() < 8);
    m_height++;
    ST(0) = st;
    m_changed = true;
  }
  void pop(BYTE count = 1) {
    CHECKCONDITION(count <= getHeight());
    m_height -= count;
    m_changed = true;
  }
  inline void pushMixed() {
    push(SLOT_MIXED);
  }
  inline void setMixed(BYTE index) { // index=0 is ST(0)., etc
    ST(index).setSymbol(SLOT_MIXED);
    m_changed = true;
  }
  inline void setEmpty(BYTE index) {
    ST(index).setSymbol(SLOT_EMPTY);
    m_changed = true;
  }
  void swapWith0(BYTE index) {
    CHECKCONDITION(index <= getHeight());
    FPUSlot &st0 = ST(0), &sti = ST(index);
    const FPUSlot tmp = st0; st0 = sti; sti = tmp;
    m_changed = true;
  }
  inline void fincstp() {
    if(m_bottom == 7) m_bottom = 0; else m_bottom++;
    m_changed = true;
  }
  inline void fdecstp() {
    if(m_bottom == 0) m_bottom = 7; else m_bottom--;
    m_changed = true;
  }
public:
  inline FPUState() {
    m_bottom    = 7;
    m_height    = 0;
    resetChanged();
  }
  inline void resetChanged() {
    m_changed = false;
  }
  inline bool isChanged() const {
    return m_changed;
  }
  int findRegisterWithValueIndex(UINT valueIndex) const;
  void execute(FPUOpcodeKey code, char stackDelta, int memIndex, int reg1, int reg2);
  bool operator==(const FPUState &state) const;
  bool operator!=(const FPUState &state) const {
    return !(*this == state);
  }
  String toString() const;
};

class FPUContainer {
public:
  virtual int  getValueIndex(const InstructionOperand &op) const = NULL;
  virtual void putFPUComment(const String &str) = NULL;
  virtual bool wantFPUComment() const = NULL;
};

class FPUEmulator {
private:
  FPUState      m_state;
  FPUContainer *m_container;

  static FPUOpcodeHashMap s_FPUOpcodeMap;

  inline bool isFPUOpcode(const OpcodeBase &opcode, FPUOpcodeKey &opcodeKey) const {
    return (opcodeKey = s_FPUOpcodeMap.getOpcodeKey(opcode)) != _NOTFPU;
  }
  FPUOpcodeKey execute(FPUOpcodeKey code, char stackDelta, int memIndex=-1, int reg1=-1, int reg2=-1);
public:
  inline FPUEmulator() {
    m_container = NULL;
  }
  void setContainer(FPUContainer *container) {
    m_container = container;
  }

  FPUOpcodeKey execute(const OpcodeBase &opcode);
  FPUOpcodeKey execute(const OpcodeBase &opcode, const InstructionOperand &arg);
  FPUOpcodeKey execute(const OpcodeBase &opcode, const InstructionOperand &arg1, const InstructionOperand &arg2);

  static inline FPUOpcodeKey codeLookup(const OpcodeBase &opcode) {
    return s_FPUOpcodeMap.getOpcodeKey(opcode);
  }
  // return index of reg with the specified value. -1 if not found
  inline int findRegisterWithValueIndex(UINT valueIndex) const {
    return m_state.findRegisterWithValueIndex(valueIndex);
  }
  inline String toString() const {
    return m_state.toString();
  }
  const FPUState &getState() const {
    return m_state;
  }
  // state must have been made by an earlier call to getState().
  inline void setState(const FPUState &state) {
    m_state = state;
  }
};

}; // namespace Expr
