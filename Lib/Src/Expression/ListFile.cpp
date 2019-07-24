#include "pch.h"
#include <CommonHashFunctions.h>
#include "FPUEmulator.h"
#include "ListFile.h"

namespace Expr {

// --------------------------------- ListFile functions ----------------------------

ListFile::ListFile(FILE *f, const ValueAddressCalculation &addressTable, const StringArray &commentArray, const IndexRegister &tableRefRegister)
  : m_f(f)
  , m_addressTable(addressTable)
  , m_nameCommentArray(commentArray)
  , m_tableRefRegister(tableRefRegister)
  , m_FPURegOptimized(false)
{
}

ListFile::~ListFile() {
  flush();
  clear();
}

void ListFile::addLine(ListLine *l) {
  TRACE_NEW(l);
  if(hasFPUComment()) {
    l->setFPUComment(m_FPUComment, m_FPURegOptimized);
    m_FPUComment      = EMPTYSTRING;
    m_FPURegOptimized = false;
  }
  m_lineArray.add(l);
  if(isTracing()) {
    _ftprintf(m_f, _T("%s\n"), l->toString().cstr()); fflush(m_f);
  }
}

void ListFile::adjustPositions(UINT pos, UINT bytesAdded) {
  const size_t n = m_lineArray.size();
  for(size_t i = 0; i < n; i++) {
    ListLine *l = m_lineArray[i];
    if(l->m_pos >= pos) l->m_pos += bytesAdded;
  }
}

void ListFile::clear() {
  const size_t n = m_lineArray.size();
  for(size_t i = 0; i < n; i++) {
    SAFEDELETE(m_lineArray[i]);
  }
  m_lineArray.clear();
}

static int listLineCmp(ListLine * const &l1, ListLine * const &l2) {
  const int c = (int)l1->m_pos - (int)l2->m_pos;
  if(c) return c;
  return ordinal(l2->m_isLabel) - ordinal(l1->m_isLabel);
}

void ListFile::flush() {
  if(!isOpen()) return;
  if(isTracing()) {
    _ftprintf(m_f,_T("%s\n"), makeSkillLineString().cstr());
  }
  m_lineArray.sort(listLineCmp);
  const size_t n = m_lineArray.size();
  for(size_t i = 0; i < n; i++) {
    const ListLine *l = m_lineArray[i];
    _ftprintf(m_f, _T("%s\n"), l->toString().cstr());
  }
}

void ListFile::vprintf(const TCHAR *format, va_list argptr) const {
  _vftprintf(m_f, format, argptr);
  fflush(m_f);
}

// prefixes every line with ';'
void ListFile::printComment(FILE *file, const TCHAR *format,...) { // static
  va_list argptr;
  va_start(argptr, format);
  const String str = vformat(format, argptr);
  va_end(argptr);
  const StringArray a(Tokenizer(str,_T("\n\r")));
  const size_t n = a.size();
  for(size_t i = 0; i < n; i++) {
    _ftprintf(file,_T(";%s\n"), trimRight(a[i]).cstr());
  }
}

const TCHAR *ListFile::findArgComment(const InstructionOperand &arg) const {
  if(arg.isMemoryRef()) {
    const MemoryRef &mr = arg.getMemoryReference();
    if(mr.hasBase() && (mr.getBase() == &m_tableRefRegister)) {
      const UINT index = m_addressTable.esiOffsetToIndex(mr.getOffset());
      if(index < m_nameCommentArray.size()) {
        return m_nameCommentArray[index].cstr();
      }
    }
  } else if(arg.isImmediateValue() && (arg.getSize() == REGSIZE_DWORD)) {
    const size_t index = m_addressTable.realRefToIndex((Real*)(size_t)arg.getImmUint32());
    if(index < m_nameCommentArray.size()) {
      return m_nameCommentArray[index].cstr();
    }
  }
  return NULL;
}

ListLine *ListFile::findLineByPos(UINT pos) {
  const size_t n = m_lineArray.size();
  for(size_t i = 0; i < n; i++) {
    ListLine *line = m_lineArray[i];
    if(line->m_pos == pos) {
      return line;
    }
  }
  return NULL;
}

// --------------------------------------------- ListLines ----------------------------

String ListLine::formatIns(const InstructionBase &ins) { // static
  return format(_T("%-*s"),LF_INSLEN, ins.toString().cstr());
}

String ListLine::formatOp(const OpcodeBase &opcode) { // static
  return format(_T("%-*s")
               ,LF_MNELEN, opcode.getMnemonic().cstr()
               );
}

String ListLine::formatOp(const OpcodeBase &opcode, const InstructionOperand *arg) { // static
  return format(_T("%-*s %s")
               ,LF_MNELEN, opcode.getMnemonic().cstr()
               ,arg->toString().cstr()
               );
}

String ListLine::formatOp(const OpcodeBase &opcode, const InstructionOperand *arg1, const InstructionOperand *arg2) { // static
  return format(_T("%-*s %s, %s")
               ,LF_MNELEN, opcode.getMnemonic().cstr()
               ,arg1->toString().cstr()
               ,arg2->toString().cstr()
               );
}

String ListLine::formatOp(const StringPrefix &prefix, const StringInstruction &strins) { // static
  return format(_T("%-*s %s")
               ,LF_MNELEN, prefix.getMnemonic().cstr()
               ,strins.getMnemonic().cstr()
               );
}

String ListLine::formatOpAndComment(const String &opstr, const TCHAR *comment, bool FPURegOptimized) { // static
  if(!comment) comment = EMPTYSTRING;
  return format(_T("%-*s;%c%-*s")
               ,LF_OPSLEN, opstr.cstr()
               ,FPURegOptimized?'+':' '
               ,LF_COMLEN, comment);
}

String ListLine::toString() const {
  return format(_T("%*s%-*d: ")
               ,LF_MARGIN, _T("")
               ,LF_POSLEN, m_pos);
}

ListLine0Arg::ListLine0Arg(UINT pos, const Opcode0Arg &opcode)
  : ListLine(pos)
  , m_opcode0(opcode)
{
}

String ListLine0Arg::toString() const {
  return __super::toString()
       + formatIns(m_opcode0)
       + formatOpAndComment(formatOp(m_opcode0))
       + getFPUComment();
}

ListLine1Arg::ListLine1Arg(UINT pos, const OpcodeBase &opcode, const InstructionOperand &arg, const TCHAR *nameComment)
  : ListLine(pos)
  , m_opcode(opcode)
  , m_arg(arg.clone())
  , m_nameComment(nameComment)
{
  TRACE_NEW(m_arg);
}

ListLine1Arg::~ListLine1Arg() {
  SAFEDELETE(m_arg);
}

String ListLine1Arg::toString() const {
  return __super::toString()
       + formatIns(m_opcode(*m_arg))
       + formatOpAndComment(formatOp(m_opcode,m_arg), m_nameComment, getFPURegOptimized())
       + getFPUComment();
}

ListLine2Arg::ListLine2Arg(UINT pos, const OpcodeBase &opcode, const InstructionOperand &arg1, const InstructionOperand &arg2, const TCHAR *nameComment)
  : ListLine(pos)
  , m_opcode(opcode)
  , m_arg1(arg1.clone())
  , m_arg2(arg2.clone())
  , m_nameComment(nameComment)
{
  TRACE_NEW(m_arg1);
  TRACE_NEW(m_arg2);
}

ListLine2Arg::~ListLine2Arg() {
  SAFEDELETE(m_arg1);
  SAFEDELETE(m_arg2);
}

String ListLine2Arg::toString() const {
  return __super::toString()
       + formatIns(m_opcode(*m_arg1,*m_arg2))
       + formatOpAndComment(formatOp(m_opcode,m_arg1,m_arg2), m_nameComment, getFPURegOptimized())
       + getFPUComment();
}

ListLineStringOp::ListLineStringOp(UINT pos, const StringPrefix &prefix, const StringInstruction &strins)
  : ListLine(pos)
  , m_prefix(prefix), m_strins(strins)
{
}

String ListLineStringOp::toString() const {
  return __super::toString()
       + formatIns(m_prefix(m_strins))
       + formatOp(m_prefix, m_strins);
}

ListLineJump::ListLineJump(UINT pos, const OpcodeBase &opcode, int iprel, CodeLabel label, const FPUState &state)
  : ListLine(pos)
  , m_opcode(opcode)
  , m_label(label)
  , m_iprel(iprel)
{
  setFPUComment(state.toString(), false);
}

String ListLineJump::toString() const {
  return __super::toString()
       + formatIns(m_opcode(m_iprel))
       + formatOpAndComment(format(_T("%s %s")
                                  ,formatOp(m_opcode).cstr()
                                  ,formatHexValue(m_iprel,false).cstr()
                                  )
                           ,labelToString(m_label).cstr()
                           )
       + getFPUComment();
}



ListLineCall::ListLineCall(UINT pos, const OpcodeCall &opcode, const InstructionOperand &arg, const FunctionCall &fc)
  : ListLine(pos)
  , m_opcode(opcode)
  , m_arg(arg.clone())
  , m_fc(fc)
{
  TRACE_NEW(m_arg);
}

ListLineCall::~ListLineCall() {
  SAFEDELETE(m_arg);
}

String ListLineCall::toString() const {
  return __super::toString()
       + formatIns(m_opcode(*m_arg))
       + formatOpAndComment(formatOp(m_opcode,m_arg)
                           ,m_fc.m_signature.cstr()
                           );
}


ListLineLabel::ListLineLabel(UINT pos, CodeLabel label, const FPUState &state)
  : ListLine(pos, true)
  , m_label(label)
{
  setFPUComment(state.toString(), false);
}

String ListLineLabel::toString() const {
  const String labelStr      = labelToString(m_label);
  const int    fillerLength1 = LF_MARGIN + LF_POSLEN + 2 + LF_INSLEN + LF_OPSLEN - (int)labelStr.length() - 1;
  const int    fillerLength2 = LF_COMLEN + 1;
  return format(_T("%s:%*s;%*s")
               , labelStr.cstr()
               , fillerLength1, _T("")
               , fillerLength2, _T("")
               )
               + getFPUComment();
}

ListLineFuncAddr::ListLineFuncAddr(UINT pos, UINT rbxOffset, const FunctionCall &fc)
  : ListLine(pos)
  , m_rbxOffset(rbxOffset)
  , m_fc(fc)
{
}

String ListLineFuncAddr::toString() const {
  return __super::toString()
       + format(_T("[%-#0*x] %s (%s)")
               ,LF_POSLEN, m_rbxOffset
               ,formatHexValue((UINT64)m_fc.m_fp).cstr()
               ,m_fc.m_signature.cstr());
}

}; // namespace Expr
