#pragma once

#include <MyUtil.h>
#include "FunctionCall.h"

namespace IntelCPU {
  class IndexRegister;
  class OpcodeBase;
  class Opcode0Arg;
  class OpcodeCall;
  class InstructionBase;
  class StringPrefix;
  class StringInstruction;
};

namespace Expr {

using namespace IntelCPU;

typedef int CodeLabel;
class FPUState;

inline String labelToString(CodeLabel label) {
  return format(_T("L%d"), label);
}

#define LF_MARGIN  2
#define LF_POSLEN  4
#define LF_INSLEN 32
#define LF_MNELEN  6
#define LF_OPSLEN 40
#define LF_COMLEN 20

class ListLine {
private:
  String m_FPUComment;
  bool   m_FPURegOptimized;
protected:
  static String formatIns(const InstructionBase &ins);
  static String formatOp(const OpcodeBase &opcode);
  static String formatOp(const OpcodeBase &opcode, const InstructionOperand *arg);
  static String formatOp(const OpcodeBase &opcode, const InstructionOperand *arg1, const InstructionOperand *arg2);
  static String formatOp(const StringPrefix &prefix, const StringInstruction &strins);
  static String formatOpAndComment(const String &opstr, const TCHAR *comment=NULL, bool FPURegOptimized=false);
  inline const String &getFPUComment() const {
    return m_FPUComment;
  }
  inline bool getFPURegOptimized() const {
    return m_FPURegOptimized;
  }
public:
  UINT m_pos;
  const bool m_isLabel; // used by sort, to put labels BEFORE the ListLine with same m_pos
  ListLine(UINT pos, bool isLabel=false) : m_pos(pos), m_isLabel(isLabel), m_FPURegOptimized(false)  {
  }
  inline void setFPUComment(const String &comment, bool FPURegOptimized) {
    m_FPUComment      = comment;
    m_FPURegOptimized = FPURegOptimized;
  }
  inline bool hasFPUComment() const {
    return m_FPUComment.length() > 0;
  }
  virtual ~ListLine() {
  }
  virtual String toString() const;
  virtual void setIPrelativeOffset(int iprel) {
    throwUnsupportedOperationException(__TFUNCTION__);
  }
};

class ListLine0Arg : public ListLine {
private:
  const Opcode0Arg &m_opcode0;
public:
  ListLine0Arg(UINT pos, const Opcode0Arg &opcode);
  String toString() const;
};

class ListLine1Arg : public ListLine {
private:
  const OpcodeBase         &m_opcode;
  const InstructionOperand *m_arg;
  const TCHAR              *m_nameComment;
public:
  ListLine1Arg(UINT pos, const OpcodeBase &opcode, const InstructionOperand &arg, const TCHAR *nameComment);
  ~ListLine1Arg();
  String toString() const;
};

class ListLine2Arg : public ListLine {
private:
  const OpcodeBase         &m_opcode;
  const InstructionOperand *m_arg1, *m_arg2;
  const TCHAR              *m_nameComment;
public:
  ListLine2Arg(UINT pos, const OpcodeBase &opcode, const InstructionOperand &arg1, const InstructionOperand &arg2, const TCHAR *nameComment);
  ~ListLine2Arg();
  String toString() const;
};

class ListLineStringOp : public ListLine {
private:
  const StringPrefix      &m_prefix;
  const StringInstruction &m_strins;
public:
  ListLineStringOp(UINT pos, const StringPrefix &prefix, const StringInstruction &strins);
  String toString() const;
};

class ListLineJump : public ListLine {
private:
  const OpcodeBase         &m_opcode;
  const CodeLabel           m_label;
  int                       m_iprel;
public:
  ListLineJump(UINT pos, const OpcodeBase &opcode, int iprel, CodeLabel label, const FPUState &state);
  void setIPrelativeOffset(int iprel) {
    m_iprel = iprel;
  }
  String toString() const;
};

class ListLineCall : public ListLine {
private:
  const OpcodeBase         &m_opcode;
  const InstructionOperand *m_arg;
  const FunctionCall        m_fc;
public:
  ListLineCall(UINT pos, const OpcodeCall &opcode, const InstructionOperand &arg, const FunctionCall &fc);
  ~ListLineCall();
  String toString() const;
};

class ListLineLabel : public ListLine {
private:
  const CodeLabel m_label;
public:
  ListLineLabel(UINT pos, CodeLabel label, const FPUState &state);
  String toString() const;
};

class ListLineFuncAddr : public ListLine {
private:
  const UINT         m_rbxOffset;
  const FunctionCall m_fc;
public:
  ListLineFuncAddr(UINT pos, UINT rbxOffset, const FunctionCall &fc);
  String toString() const;
};

class ListFile {
private:
  FILE                          *m_f;
  CompactArray<ListLine*>        m_lineArray;
  const ValueAddressCalculation &m_addressTable;
  const StringArray              m_nameCommentArray;   // For comments
  String                         m_FPUComment;
  bool                           m_FPURegOptimized;
  const IndexRegister           &m_tableRefRegister;

  inline bool hasFPUComment() const {
    return (m_FPUComment.length() > 0) || m_FPURegOptimized;
  }
  inline bool isTracing() const {
#if defined(_DEBUG)
    return isOpen() && isatty(m_f) && IsDebuggerPresent();
#else
    return false;
#endif // _DEBUG
  }
  void addLine(ListLine *l);
  const TCHAR *findArgComment(const InstructionOperand &arg) const;
public:
  ListFile(FILE *f, const ValueAddressCalculation &addressTable, const StringArray &commentArray, const IndexRegister &tableRefRegister);
  ~ListFile();
  void clear();
  inline void add(UINT pos, const Opcode0Arg &opcode) {
    addLine(new ListLine0Arg(pos,opcode));
  }
  inline void add(UINT pos, const OpcodeBase &opcode, const InstructionOperand &arg) {
    addLine(new ListLine1Arg(pos,opcode,arg,findArgComment(arg)));
  }
  inline void add(UINT pos, const OpcodeBase &opcode, const InstructionOperand &arg1, const InstructionOperand &arg2) {
    const TCHAR *comment1 = findArgComment(arg1);
    addLine(new ListLine2Arg(pos,opcode,arg1,arg2, comment1?comment1:findArgComment(arg2)));
  }
  inline void add(UINT pos, const StringPrefix &prefix, const StringInstruction &strins) {
    addLine(new ListLineStringOp(pos,prefix,strins));
  }
  inline void add(UINT pos, const OpcodeBase &opcode, int iprel, CodeLabel label, const FPUState &state) {
    addLine(new ListLineJump(pos,opcode,iprel,label,state));
  }
  inline void add(UINT pos, const OpcodeCall &opcode, const InstructionOperand &arg, const FunctionCall &fc) {
    addLine(new ListLineCall(pos,opcode,arg,fc));
  }
  inline void add(UINT pos, CodeLabel label, const FPUState &state) {
    addLine(new ListLineLabel(pos,label, state));
  }
  inline void add(UINT pos, UINT rbxOffset, const FunctionCall &fc) {
    addLine(new ListLineFuncAddr(pos,rbxOffset,fc));
  }
  ListLine *findLineByPos(UINT pos);
  inline bool isOpen() const {
    return m_f != NULL;
  }
  inline void setFPUComment(const String &comment, bool FPURegOptimized) {
    m_FPUComment      = comment;
    m_FPURegOptimized = FPURegOptimized;

  }
  void adjustPositions(UINT pos, UINT bytesAdded);
  void vprintf(const TCHAR *format, va_list argptr) const;
  void flush();
  static void printComment(FILE *file, const TCHAR *format,...); // prefixes every line with ';'
  static String makeSkillLineString(UINT length = 60) {
    return spaceString(length, '-');
  }
};

}; // namespace Expr
