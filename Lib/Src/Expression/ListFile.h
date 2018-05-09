#pragma once

#include <MyUtil.h>
#include <Opcode.h>
#include "FunctionCall.h"

typedef int CodeLabel;

inline String labelToString(CodeLabel label) {
  return format(_T("L%d"), label);
}

#define LF_MARGIN  2
#define LF_POSLEN  4
#define LF_INSLEN 32
#define LF_MNELEN  6
#define LF_OPSLEN 40

class ListLine {
protected:
  static String formatIns(const InstructionBase &ins) {
    return format(_T("%-*s"),LF_INSLEN, ins.toString().cstr());
  }
  static String formatOp(const OpcodeBase &opcode) {
    return format(_T("%-*s")
                 ,LF_MNELEN, opcode.getMnemonic().cstr()
                 );
  }
  static String formatOp(const OpcodeBase &opcode, const InstructionOperand *arg) {
    return format(_T("%-*s %s")
                 ,LF_MNELEN, opcode.getMnemonic().cstr()
                 ,arg->toString().cstr()
                 );
  }
  static String formatOp(const OpcodeBase &opcode, const InstructionOperand *arg1, const InstructionOperand *arg2) {
    return format(_T("%-*s %s, %s")
                 ,LF_MNELEN, opcode.getMnemonic().cstr()
                 ,arg1->toString().cstr()
                 ,arg2->toString().cstr()
                 );
  }
  static String formatOp(const StringPrefix &prefix, const StringInstruction &strins) {
    return format(_T("%-*s %s")
                 ,LF_MNELEN, prefix.getMnemonic().cstr()
                 ,strins.getMnemonic().cstr()
                 );
  }
  static String formatOpAndComment(const String &opstr, const TCHAR *comment=NULL) {
    return comment ? format(_T("%-*s; %s"),LF_OPSLEN, opstr.cstr(), comment)
                   : format(_T("%s"), opstr.cstr());
  }
public:
  UINT m_pos;
  const bool m_isLabel; // used by sort, to put labels BEFORE the ListLine with same m_pos
  ListLine(UINT pos, bool isLabel=false) : m_pos(pos), m_isLabel(isLabel)  {
  }
  virtual ~ListLine() {
  }
  virtual String toString() const {
    return format(_T("%*s%-*d: ")
                 ,LF_MARGIN,_T("")
                 ,LF_POSLEN, m_pos);
  }
  virtual void setIPrelativeOffset(int iprel) {
    throwUnsupportedOperationException(__TFUNCTION__);
  }
};

class ListLine0Arg : public ListLine {
private:
  const Opcode0Arg &m_opcode0;
public:
  ListLine0Arg(UINT pos, const Opcode0Arg &opcode)
    : ListLine(pos)
    , m_opcode0(opcode)
  {
  }
  String toString() const {
    return __super::toString()
         + formatIns(m_opcode0)
         + formatOpAndComment(formatOp(m_opcode0));
  }
};

class ListLine1Arg : public ListLine {
private:
  const OpcodeBase         &m_opcode;
  const InstructionOperand *m_arg;
  const TCHAR              *m_nameComment;
public:
  ListLine1Arg(UINT pos, const OpcodeBase &opcode, const InstructionOperand &arg, const TCHAR *nameComment)
    : ListLine(pos)
    , m_opcode(opcode)
    , m_arg(arg.clone())
    , m_nameComment(nameComment)
  {
    TRACE_NEW(m_arg);
  }
  ~ListLine1Arg() {
    SAFEDELETE(m_arg);
  }
  String toString() const {
    return __super::toString()
         + formatIns(m_opcode(*m_arg))
         + formatOpAndComment(formatOp(m_opcode,m_arg), m_nameComment);
  }
};

class ListLine2Arg : public ListLine {
private:
  const OpcodeBase         &m_opcode;
  const InstructionOperand *m_arg1, *m_arg2;
  const TCHAR              *m_nameComment;
public:
  ListLine2Arg(UINT pos, const OpcodeBase &opcode, const InstructionOperand &arg1, const InstructionOperand &arg2, const TCHAR *nameComment)
    : ListLine(pos)
    , m_opcode(opcode)
    , m_arg1(arg1.clone())
    , m_arg2(arg2.clone())
    , m_nameComment(nameComment)
  {
    TRACE_NEW(m_arg1);
    TRACE_NEW(m_arg2);
  }

  ~ListLine2Arg() {
    SAFEDELETE(m_arg1);
    SAFEDELETE(m_arg2);
  }
  String toString() const {
    return __super::toString()
         + formatIns(m_opcode(*m_arg1,*m_arg2))
         + formatOpAndComment(formatOp(m_opcode,m_arg1,m_arg2), m_nameComment);
  }
};

class ListLineStringOp : public ListLine {
private:
  const StringPrefix      &m_prefix;
  const StringInstruction &m_strins;
public:
  ListLineStringOp(UINT pos, const StringPrefix &prefix, const StringInstruction &strins)
    : ListLine(pos)
    , m_prefix(prefix), m_strins(strins)
  {
  }
  String toString() const {
    return __super::toString()
         + formatIns(m_prefix(m_strins))
         + formatOp(m_prefix, m_strins);
  }
};

class ListLineJump : public ListLine {
private:
  const OpcodeBase         &m_opcode;
  const CodeLabel           m_label;
  int                       m_iprel;
public:
  ListLineJump(UINT pos, const OpcodeBase &opcode, int iprel, CodeLabel label)
    : ListLine(pos)
    , m_opcode(opcode)
    , m_label(label)
    , m_iprel(iprel)
  {
  }
  void setIPrelativeOffset(int iprel) {
    m_iprel = iprel;
  }
  String toString() const {
    return __super::toString()
         + formatIns(m_opcode(m_iprel))
         + formatOpAndComment(format(_T("%s %s")
                                   ,formatOp(m_opcode).cstr()
                                   ,formatHexValue(m_iprel,false).cstr()
                                   )
                             ,labelToString(m_label).cstr()
                             );
  }
};

class ListLineCall : public ListLine {
private:
  const OpcodeBase         &m_opcode;
  const InstructionOperand *m_arg;
  const FunctionCall        m_fc;
public:
  ListLineCall(UINT pos, const OpcodeCall &opcode, const InstructionOperand &arg, const FunctionCall &fc)
    : ListLine(pos)
    , m_opcode(opcode)
    , m_arg(arg.clone())
    , m_fc(fc)
  {
    TRACE_NEW(m_arg);
  }
  ~ListLineCall() {
    SAFEDELETE(m_arg);
  }
  String toString() const {
    return __super::toString()
         + formatIns(m_opcode(*m_arg))
         + formatOpAndComment(formatOp(m_opcode,m_arg)
                             ,m_fc.m_signature.cstr()
                             );
  }
};

class ListLineLabel : public ListLine {
private:
  const CodeLabel m_label;
public:
  ListLineLabel(UINT pos, CodeLabel label)
    : ListLine(pos, true)
    , m_label(label)
  {
  }
  String toString() const {
    return format(_T("%s:"), labelToString(m_label).cstr());
  }
};

class ListLineFuncAddr : public ListLine {
private:
  const UINT         m_rbxOffset;
  const FunctionCall m_fc;
public:
  ListLineFuncAddr(UINT pos, UINT rbxOffset, const FunctionCall &fc)
    : ListLine(pos)
    , m_rbxOffset(rbxOffset)
    , m_fc(fc)
  {
  }
  String toString() const {
    return __super::toString()
         + format(_T("[%-#0*x] %s (%s)")
                 ,LF_POSLEN, m_rbxOffset
                 ,formatHexValue((UINT64)m_fc.m_fp).cstr()
                 ,m_fc.m_signature.cstr());
  }
};

class ListFile {
private:
  FILE                          *m_f;
  CompactArray<ListLine*>        m_lineArray;
  const ValueAddressCalculation &m_addressTable;
  const StringArray              m_nameCommentArray;   // For comments
  const IndexRegister           &m_tableRefRegister;

  inline void addLine(ListLine *l) {
    TRACE_NEW(l); m_lineArray.add(l);
#ifdef _DEBUG
    if(isOpen() && isatty(m_f)) { _ftprintf(m_f, _T("%s\n"), l->toString().cstr()); fflush(m_f); }
#endif // _DEBUG
  }
  const TCHAR *findArgComment(const InstructionOperand &arg) const;
public:
  ListFile(FILE *f, const ValueAddressCalculation &addressTable, const StringArray &commentArray, const IndexRegister &tableRefRegister) 
    : m_f(f)
    , m_addressTable(addressTable)
    , m_nameCommentArray(commentArray)
    , m_tableRefRegister(tableRefRegister)
  {
  }
  ~ListFile() {
    flush();
    clear();
  }
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
  inline void add(UINT pos, const OpcodeBase &opcode, int iprel, CodeLabel label) {
    addLine(new ListLineJump(pos,opcode,iprel,label));
  }
  inline void add(UINT pos, const OpcodeCall &opcode, const InstructionOperand &arg, const FunctionCall &fc) {
    addLine(new ListLineCall(pos,opcode,arg,fc));
  }
  inline void add(UINT pos, CodeLabel label) {
    addLine(new ListLineLabel(pos,label));
  }
  inline void add(UINT pos, UINT rbxOffset, const FunctionCall &fc) {
    addLine(new ListLineFuncAddr(pos,rbxOffset,fc));
  }
  ListLine *findLineByPos(UINT pos);
  inline bool isOpen() const {
    return m_f != NULL;
  }
  void adjustPositions(UINT pos, UINT bytesAdded);
  void vprintf(const TCHAR *format, va_list argptr) const;
  void flush();
};
