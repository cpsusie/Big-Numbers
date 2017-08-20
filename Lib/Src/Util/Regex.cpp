#include "pch.h"
#include <String.h>
#include <TreeMap.h>
#include <Regex.h>

//NOTRACEFILE
//TRACEFILE

// These are the command codes that appear in compiled regular expressions, one per byte.
// Some command codes are followed by argument bytes.
// A command code can specify any interpretation whatever for its arguments.
// Zero-bytes may appear in the compiled regular expression.

typedef enum {
    unused
   ,onFailureJump            // Followed by two bytes giving relative address of place
                             // to resume at in case of failure.
                             //
   ,maybePopAndJump          // Like jump but finalize if safe to do so.
                             // This is used to jump back to the beginning of a repeat. If the command that follows
                             // this jump is clearly incompatible with the one at the beginning of the repeat, such that
                             // we can be sure that there is no use backtracking out of repetitions already completed,
                             // then we finalize.
                             //
   ,popAndJump               // Throw away latest failure point and then jump to address.
                             //
   ,jump                     // Followed by 2 bytes giving relative address to jump to
   ,dummyFailureJump         // Jump, and push a dummy failure point.
                             // This failure point will be thrown away
                             // if an attempt is made to use it for a failure.
                             // A '+' closure makes this before the first repeat.
                             //
   ,onFailureJumpPushCounter // Followed by 1 byte indicating counterIndex, and 2 bytes giving relative addres to jump to (like onFailureJump)

   ,maybePopCountAndJump     // Almost like maybyPopAndJump.
                             // Followed by 1 byte indicating counterIndex, and 2 bytes giving relative address to jump to
   ,popCountAndJump          // Almost like popAndJump. Parameters like maybePopCounterAndJump
   ,countAndJump             // Almost like like jump, Parameters like maybePopCounterAndJump. Increments the specified counterregister
   ,resetCounter             // Followed by 1 byte indicating counterIndex

   ,startMemory              // Starts remembering the text that is matched
                             // and stores it in a memory register.
                             // Followed by one byte containing the register number.
                             // Register numbers must be in the range 0 through RE_NREGS.
                             //
   ,stopMemory               // Stops remembering the text that is matched
                             // and stores it in a memory register.
                             // Followed by one byte containing the register number.
                             // Register numbers must be in the range 0 through RE_NREGS.
                             //
   ,duplicate                // Match a duplicate of something remembered.
                             // Followed by one byte containing the index of
                             // the memory register.
   ,exactMatch               // Followed by one byte giving n, and then by n literal TCHAR
   ,anychar                  // Matches any TCHAR
   ,charSet                  // Matches any char belonging to specified set.
                             // Followed by 2 UShort, specifying range of Bitset
                             // Then come bytes for a bit-map saying which TCHAR are in.bitset
                             // A TCHAR is in the set if its bit is 1.
                             // A TCHAR outside range is automatically excluded from the set
                             //
   ,charSetNot               // Similar but match any TCHAR that is NOT one of those specified
                             //
   ,beginBuf                 // Succeeds if at beginning of buffer
   ,endBuf                   // Succeeds if at end of buffer
   ,beginLine                // Succeeds if at beginning of line
   ,endLine                  // Succeeds if at end of line
   ,beginWord                // Succeeds if at word beginning
   ,endWord                  // Succeeds if at word end
   ,wordBound                // Succeeds if at a word boundary
   ,notWordBound             // Succeeds if not at a word boundary
   ,wordchar                 // Matches any word-constituent TCHAR
   ,notWordchar              // Matches any char that is not a word-constituent
} RegexOPCode;


#ifdef UNICODE
#define MAXCHARSETSIZE 0x10000
#else
#define MAXCHARSETSIZE 0x100
#endif

static const TCHAR *noRegExpressionMsg  = _T("No regular expression specified");
static const TCHAR *unexpectedEndMsg    = _T("Unexpected end of regular expression");
static const TCHAR *minMaxRepeatMsg     = _T("Max repeatcount must be >= min repeatcount");
static const TCHAR *maxRepeatIsZeroMsg  = _T("Max repeatcount is zero");
static const TCHAR *invalidOpcodeMsg    = _T("Invalid opcode:%d");
static const TCHAR *numberToBigMsg      = _T("Number too big");
static const TCHAR *regnoNotDefined     = _T("Register %d not defined at the specified point of the pattern. %s");
static const TCHAR *expectedNumberMsg   = _T("Expected number");
static const TCHAR *expectedRCurlMsg    = _T("Expected '}'");
static const TCHAR *unmatchedLPMsg      = _T("Unmatched \\(");
static const TCHAR *unmatchedRPMsg      = _T("Unmatched \\)");
static const TCHAR *unexpectedOpcodeMsg = _T("doMaybePopAndJump:opcode must be maybePopAndJump or maybePopCounterAndJump. (=%d)");
static const TCHAR *tooManyCountersMsg  = _T("Too many counters in use. Max=%d");
static const TCHAR *illegalJumpMsg      = _T("Regex::first(%d,%d):Jump to a previous address (=%d) not legal");
static const TCHAR *invalidCharSetMsg   = _T("CharSet:Invalid opcode:%d. Must be charSet or charSetNot");

#define NEWLINE _T('\n')

// --------------------------------- WordLetterSet ---------------------------------------------------

class WordLetterSet : public BitSet {
public:
  WordLetterSet();
};

WordLetterSet::WordLetterSet() : BitSet(256) {
  add('a','z');
  add('A','Z');
  add('0','9');
}

static const WordLetterSet wordLetterSet;

// --------------------------------- Helper functions ---------------------------------------------------

static inline bool isWordLetter(_TUCHAR ch) {
  return (ch < 256) && wordLetterSet.contains(ch);
}

static inline _TUCHAR translate(_TUCHAR ch, const TCHAR *table) {
  return (ch < 256)? table[ch] : ch;
}

#define TRANSLATE(ch) (m_translateTable ? translate(ch, m_translateTable) : ((_TUCHAR)(ch)))

static bool isClosureCharacter(_TUCHAR ch) {
  switch(ch) {
  case _T('*'):
  case _T('+'):
  case _T('?'):
  case _T('{'):
    return true;
  default:
    return false;
  }
}

static bool isCharacterSet(BYTE p) {
  switch(p) {
  case charSet      :
  case charSetNot   :
    return true;
  default:
    return false;
  }
}

static inline unsigned short getUShort(const BYTE *&p) {
  const unsigned short result = *(unsigned short*)p;
  p += sizeof(result);
  return result;
}

static inline short getShort(const BYTE *&p) {
  const short result = *(short*)p;
  p += sizeof(result);
  return result;
}

static inline _TUCHAR getCharacter(const BYTE *&p) {
  const _TUCHAR result = *(_TUCHAR*)p;
  p += sizeof(result);
  return result;
}

// --------------------------------- CharSet ---------------------------------------------------

class _CharSet {
private:
  unsigned short m_minChar;               // multiplum of 8
  unsigned short m_maxChar;
  BYTE m_bitSet[1];                       // the real length depends on min and maxChar. We just nead a pointer
                                          // to this structure, save in Regex.m_buffer with ByteBitSet.append
public:
  inline bool contains(_TUCHAR ch) const {
    if((ch < m_minChar) || (ch > m_maxChar)) {
      return false;
    }
    const UINT index = ch - m_minChar;
    return (m_bitSet[index/8] & (1<<(index%8))) != 0;
  }
#ifdef _DEBUG
  operator BitSet() const;
#endif
};

#ifdef _DEBUG
_CharSet::operator BitSet() const {
  BitSet result(m_maxChar+1);
  for(int i = m_minChar; i <= m_maxChar; i++) {
    if(contains(i)) {
      result.add(i);
    }
  }
  return result;
}
#endif

class CharSet {
private:
  bool            m_invertingSet;                 // true if opcode if charSetNot
  const _CharSet *m_charSet;
public:
  CharSet(RegexOPCode opcode, const BYTE *&p);
  inline bool contains(_TUCHAR ch) const {        // return true if c is in set, Dont care about m_invertingSet
    return m_charSet->contains(ch);
  }
  inline bool isLegalChar(_TUCHAR ch) const {
    return contains(ch) ^ m_invertingSet;
  }

#ifdef _DEBUG
  inline String toString() const {
    return charBitSetToString(*m_charSet);
  }
#endif
};

CharSet::CharSet(RegexOPCode opcode, const BYTE *&p) {
  m_invertingSet = false;
  switch(opcode) {
  case charSetNot:
    m_invertingSet = true;
    // continue case
  case charSet   :
    { const short offset = getShort(p);
      m_charSet          = (_CharSet*)(p + offset);
    }
    break;
  default:
    throwException(invalidCharSetMsg, opcode);
  }
}

// --------------------------------- ByteBitSet ---------------------------------------------------

#define BYTEINDEX(i)     ((i)/8)
#define GETBITADDRESS(i) (m_bitSet + BYTEINDEX(i))
#define MASKBYTE(i)      (((i)==8)?(0xff):((1<<(i))-1))

class ByteBitSet {
private:
  unsigned short m_min, m_max;
  BYTE m_bitSet[(MAXCHARSETSIZE-1) / 8 + 1];          // pointer into Regex.m_buffer.data, that contains bitsst.
public:
  ByteBitSet();
  ByteBitSet(const ByteBitSet &src);
  ByteBitSet &operator=(const ByteBitSet &src);
  void stopAdding();
  void add(_TUCHAR ch);
  void add(_TUCHAR from, _TUCHAR b);
  inline bool isEmpty() const {
    return m_min > m_max;
  }
  inline bool contains(_TUCHAR ch) const {
    return (m_bitSet[ch/8] & (1<<(ch%8))) != 0;
  }
  void append(ByteArray &buffer) const;
  static int compare(const ByteBitSet &s1, const ByteBitSet &s2);

#ifdef _DEBUG
  operator BitSet() const;
#endif
};

ByteBitSet::ByteBitSet() {
  memset(m_bitSet, 0, sizeof(m_bitSet));
  m_min = -1;
  m_max =  0;
}

ByteBitSet::ByteBitSet(const ByteBitSet &src) {
  memcpy(m_bitSet, src.m_bitSet, sizeof(m_bitSet));
  m_min = src.m_min;
  m_max = src.m_max;
}

ByteBitSet &ByteBitSet::operator=(const ByteBitSet &src) {
  memcpy(m_bitSet, src.m_bitSet, sizeof(m_bitSet));
  m_min = src.m_min;
  m_max = src.m_max;
  return *this;
}

void ByteBitSet::add(_TUCHAR ch) {
  if(ch < m_min) m_min = ch;
  if(ch > m_max) m_max = ch;
  m_bitSet[BYTEINDEX(ch)] |= (1<<(ch%8));
}

void ByteBitSet::add(_TUCHAR from, _TUCHAR to) {
  if(from > to) {
    return;
  }
  if(from < m_min) m_min = from;
  if(to   > m_max) m_max = to;
  UINT fromIndex = BYTEINDEX(from);
  UINT toIndex   = BYTEINDEX(to  );

  if(fromIndex < toIndex) {
    if(from % 8) {
      m_bitSet[fromIndex] |= ~MASKBYTE(from%8);
      fromIndex++;
    }
    if((to+1) % 8) {
      m_bitSet[toIndex] |= MASKBYTE(to%8 + 1);
      toIndex--;
    }

    if(fromIndex <= toIndex) {
      memset(m_bitSet+fromIndex, ~0, toIndex-fromIndex+1);
    }
  } else {
    m_bitSet[fromIndex] |= ~MASKBYTE(from%8) & MASKBYTE(to%8 + 1);
  }
}

void ByteBitSet::stopAdding() {
  if(isEmpty()) {
    m_min = m_max = 0;
  }
  m_min &= ~7;                                 // m_min must be a multipla of 8
}

void ByteBitSet::append(ByteArray &buffer) const {
  const int size0 = (int)buffer.size();
  buffer.append((BYTE*)&m_min, sizeof(m_min)); // unpacked by CharSet
  buffer.append((BYTE*)&m_max, sizeof(m_max));
  const BYTE *firstByte = GETBITADDRESS(m_min);
  const BYTE *lastByte  = GETBITADDRESS(m_max);
  const unsigned short byteCount = (unsigned short)(lastByte - firstByte + 1);
  buffer.append(firstByte, byteCount);
}

int ByteBitSet::compare(const ByteBitSet &s1, const ByteBitSet &s2) { // static
  int c = s1.m_min - s2.m_min;
  if(c) return c;
  c = s1.m_max - s2.m_max;
  if(c) return c;
  return memcmp(s1.m_bitSet, s2.m_bitSet, sizeof(s1.m_bitSet));
}

#ifdef _DEBUG
ByteBitSet::operator BitSet() const {
  if(isEmpty()) {
    return BitSet(8);
  }
  BitSet result(MAXCHARSETSIZE);
  for(int i = 0; i < MAXCHARSETSIZE; i++) {
    if(contains(i)) {
      result.add(i);
    }
  }
  return result;
}
#endif

class CharSetMap : public TreeMap<ByteBitSet, CompactIntArray> {
public:
  CharSetMap() : TreeMap<ByteBitSet, CompactIntArray>(ByteBitSet::compare) {
  }
  void incrAddresses(UINT addr, UINT incr);
#ifdef _DEBUG
  String toString() const;
#endif
};

void CharSetMap::incrAddresses(UINT addr, UINT incr) { // called when instructions are inserted before the charset
  for(Iterator<CompactIntArray> it = values().getIterator(); it.hasNext();) {
    CompactIntArray &fixupArray = it.next();
    for(int i = 0; i < (int)fixupArray.size(); i++) {
      int &fixupAddr = fixupArray[i];
      if(fixupAddr > (int)addr) {
        fixupAddr += incr;
      }
    }
  }
}

#ifdef _DEBUG
String CharSetMap::toString() const {
  String result;
  int count = 0;
  for(Iterator<Entry<ByteBitSet, CompactIntArray> > it = ((CharSetMap*)this)->entrySet().getIterator(); it.hasNext();) {
    const Entry<ByteBitSet, CompactIntArray> &entry = it.next();
    result += format(_T("%s : %s\n"),  charBitSetToString(entry.getKey()).cstr(), entry.getValue().toStringBasicType().cstr());
  }
  return result;
}
#endif

class RegisterInfo {
public:
  BYTE           m_regno;
  BYTE           m_level;
  unsigned short m_addressStartMemory;
  unsigned short m_addressStopMemory;
  RegisterInfo() {}
  RegisterInfo(BYTE regno, BYTE level, unsigned short addressStartMemory)
    : m_regno(regno)
    , m_level(level)
    , m_addressStartMemory(addressStartMemory) {
#ifdef _DEBUG
     m_addressStopMemory = 0;
#endif
  }
  void incrAddresses(UINT addr, UINT incr);
#ifdef _DEBUG
  String toString() const {
    return format(_T("R:%d L:%d, [%d-%d]\n"), m_regno, m_level, m_addressStartMemory, m_addressStopMemory);
  }
#endif
};

void RegisterInfo::incrAddresses(UINT addr, UINT incr) {
  if(m_addressStartMemory >= addr) m_addressStartMemory += incr;
  if(m_addressStopMemory  >= addr) m_addressStopMemory  += incr;
}

class RegisterInfoTable : public CompactArray<RegisterInfo> {
public:
  void incrAddresses(UINT addr, UINT incr);
  RegisterInfo &findByRegno(BYTE regno);
  const RegisterInfo *findLastRegisterInCountingLoop(BYTE level, unsigned short loopStart) const;
};

void RegisterInfoTable::incrAddresses(UINT addr, UINT incr) {
  for(int i = 0; i < (int)size(); i++) {
    (*this)[i].incrAddresses(addr, incr);
  }
}

RegisterInfo &RegisterInfoTable::findByRegno(BYTE regno) {
  for(int i = 0; i < (int)size(); i++) {
    RegisterInfo &ri = (*this)[i];
    if(ri.m_regno == regno) {
      return ri;
    }
  }
  throwException(_T("RegisterInfo(%d) not found"), regno);
  return (*this)[0];
}

const RegisterInfo *RegisterInfoTable::findLastRegisterInCountingLoop(BYTE level, unsigned short loopStart) const {
  for(int i = (int)size(); i--;) {
    const RegisterInfo &ri = (*this)[i];
    if((ri.m_level == level)
    && (ri.m_addressStartMemory < loopStart) && (ri.m_addressStopMemory > loopStart)) {
      return &ri;
    }
  }
  return NULL;
}

class ByteInsertHandler : public _RegexByteInsertHandler {
private:
  CharSetMap        &m_charSetMap;
  RegisterInfoTable &m_registerTable;
public:
  ByteInsertHandler(CharSetMap &charSetMap, RegisterInfoTable &registerTable)
    : m_charSetMap(charSetMap)
    , m_registerTable(registerTable)
  {
  }
  void insertBytes(UINT addr, UINT incr);
};

void ByteInsertHandler::insertBytes(UINT addr, UINT incr) {
  m_charSetMap.incrAddresses(addr, incr);
  m_registerTable.incrAddresses(addr, incr);
}

// --------------------------------- Regex Scanner ---------------------------------------------------

class RegexScanner {
private:
  const _TUCHAR *m_source, *m_current, *m_end;
  const TCHAR *m_translateTable;
public:
  RegexScanner(const TCHAR *source, const TCHAR *translateTable);
  _TUCHAR        fetchRaw();
  _TUCHAR        fetch();
  _TUCHAR        look(UINT lookahead = 0) const;
  inline bool    eos() const { return m_current == m_end; }
  inline int     getIndex() const { return (int)(m_current - m_source); }
  unsigned short fetchShort();
  void           error(const TCHAR *format,...);
  static void    error(int index, const TCHAR *format,...);
  static void    verror(int index, const TCHAR *format, va_list argptr);
};

RegexScanner::RegexScanner(const TCHAR *source, const TCHAR *translateTable) {
  m_source         = (_TUCHAR*)source;
  m_current        = m_source;
  m_end            = m_source + _tcsclen(source);
  m_translateTable = translateTable;
}

_TUCHAR RegexScanner::look(UINT lookahead) const {
  const _TUCHAR *result = m_current + lookahead;
  if(result > m_end) {
    throwException(unexpectedEndMsg);
  }
  return *result;
}

_TUCHAR RegexScanner::fetchRaw() {
  if(eos()) {
    throwException(unexpectedEndMsg);
  }
  return *m_current++;
}

_TUCHAR RegexScanner::fetch() {
  const _TUCHAR c = fetchRaw();
  return TRANSLATE(c);
}

unsigned short RegexScanner::fetchShort() {
  if(eos()) {
    error(unexpectedEndMsg);
  }
  _TUCHAR ch = look();
  if(!_istascii(ch) || !_istdigit(ch)) {
    error(expectedNumberMsg);
  }
  const int startIndex = getIndex();
  UINT result = 0;
  do {
    result = result * 10 + (ch - '0');
    fetch();
    ch = look();
    if(result > 0xffff) {
      error(startIndex, numberToBigMsg);
    }
  } while(_istascii(ch) && _istdigit(ch));
  return (unsigned short)result;
}

void RegexScanner::error(const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr,format);
  verror(getIndex(), format, argptr);
  va_end(argptr);
}

void RegexScanner::error(int index, const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr,format);
  verror(index, format, argptr);
  va_end(argptr);
}

void RegexScanner::verror(int index, const TCHAR *format, va_list argptr) {
  const String errMsg = vformat(format, argptr);
  throwException(_T("(%d):%s"), index, errMsg.cstr());
}

// --------------------------------- Regex ---------------------------------------------------

Regex::Regex() : m_fastMap(MAXCHARSETSIZE) {
  init();
}

Regex::Regex(const String &pattern, const TCHAR *translateTable) : m_fastMap(MAXCHARSETSIZE) {
  init();
  compilePattern(pattern, translateTable);
}

Regex::Regex(const TCHAR *pattern, const TCHAR *translateTable) : m_fastMap(MAXCHARSETSIZE) {
  init();
  compilePattern(pattern, translateTable);
}

#ifdef _DEBUG

#define DBG_clear()                               m_PCToLineArray.clear()
#define DBG_INSERT(                addr, index)   m_PCToLineArray.add(addr, index)
#define DBG_STORE(                 addr, index)   m_PCToLineArray[addr] = index
#define DBG_APPEND(                      index)   m_PCToLineArray.add(index)
#define DBG_INSERTZEROES(          addr, count) { for(int i = 0; i < (count); i++) m_PCToLineArray.add(addr, 0); }
#define DBG_appendZeroes(          count      ) { for(int i = 0; i < (count); i++) m_PCToLineArray.add(0);       }
#define DBG_appendOpcode(                index)   DBG_APPEND(index)
#define DBG_appendByte()                          DBG_APPEND(0)
#define DBG_appendCharacter()                     DBG_appendZeroes(sizeof(TCHAR))
#define DBG_appendShort()                         DBG_appendZeroes(sizeof(short))
#define DBG_insertOpcode(          addr, index)   DBG_INSERT(addr, index)
#define DBG_insertJump(            addr, index) { DBG_INSERT(addr,index);  DBG_INSERTZEROES(addr+1, 2);          }
#define DBG_storeJump(             addr, index)   DBG_STORE(addr, index)
#define DBG_insertResetCounter(    addr, index) { DBG_INSERT(addr, index); DBG_INSERTZEROES(addr+1, 1);          }
#define DBG_insertCountingJump(    addr, index) { DBG_INSERT(addr, index); DBG_INSERTZEROES(addr+1, 3);          }

#define DBG_appendJump(                  index) { DBG_APPEND(index);       DBG_appendZeroes(2);                  }
#define DBG_appendCountingJump(          index) { DBG_APPEND(index);       DBG_appendZeroes(3);                  }
#define DBG_saveIndex(             name       )   int dbg_##name = scanner.getIndex()
#define DBG_setIndex(              name       )   dbg_##name     = scanner.getIndex()
#define DBG_getIndex(              name       )   dbg_##name
#define DBG_beginAlternative()                    stack.top().m_dbgStartAlternative = scanner.getIndex()

#define DBG_setHandler(handler)     setHandler(handler)
#define DBG_setCodeDirty(dirty)     m_codeDirty = dirty;

#define DBG_callCompilerHandler(fastMapStr)                                                         \
{ if(m_stepHandler) {                                                                               \
    m_stepHandler->handleCompileStep(_RegexCompilerState(this                                       \
                                                        ,toString()                                 \
                                                        ,stack.toString(), state.toString()         \
                                                        ,usedCharSetMap.toString()                  \
                                                        ,registerTable.toString()                   \
                                                        ,fastMapStr                                 \
                                                        ,scanner.getIndex(), pendingExact           \
                                                        )                                           \
                                     );                                                             \
  }                                                                                                 \
}

#define DBG_callSearchHandler(startPos, charIndex)                                                  \
{ if(m_stepHandler) { m_stepHandler->handleSearchStep(_RegexSearchState(this, startPos, charIndex)); } }

#define DBG_callMatchHandler()                                                                      \
{ if(m_stepHandler) { m_stepHandler->handleMatchStep(state); } }


#else


#define DBG_clear()
#define DBG_INSERT(                addr, index)
#define DBG_STORE(                 addr, index)
#define DBG_APPEND(                      index)
#define DBG_appendZeroes(          count      )
#define DBG_appendOpcode(                index)
#define DBG_appendByte()
#define DBG_appendCharacter()
#define DBG_appendShort()
#define DBG_insertOpcode(          addr, index)
#define DBG_insertJump(            addr, index)
#define DBG_storeJump(             addr, index)
#define DBG_insertResetCounter(    addr, index)
#define DBG_insertCountingJump(    addr, index)
#define DBG_appendJump(                  index)
#define DBG_appendCountingJump(          index)
#define DBG_saveIndex(             name       )
#define DBG_setIndex(              name       )
#define DBG_beginAlternative()

#define DBG_setHandler(handler)
#define DBG_setCodeDirty(dirty)

#define DBG_callCompilerHandler(fastMapStr)
#define DBG_callSearchHandler(startPos, charIndex)
#define DBG_callMatchHandler()

#endif


void Regex::init() {
  m_counterTableSize = 0;
  m_codeSize         = 0;
  m_translateTable   = NULL;
  m_hasCompiled      = false;
  m_matchEmpty       = false;
  m_resultLength     = 0;
  DBG_setHandler(NULL);
  DBG_setCodeDirty(false);
}

int Regex::compareStrings(const TCHAR *s1, const TCHAR *s2, size_t length) const {
  return m_translateTable ? strntabcmp(s1,s2,length, m_translateTable) : _tcsnccmp(s1, s2, length);
}

// --------------------------------- Code generation -------------------------------------

void Regex::insertZeroes(UINT addr, UINT count) {
  m_buffer.insertZeroes(addr, count);
  m_insertHandler->insertBytes(addr, count);
}

void Regex::insertOpcode(BYTE opcode, UINT addr) {
  insertZeroes(addr, 1);
  storeOpcode(opcode, addr);
}

void Regex::insertJump(BYTE opcode, UINT addr, int to) {
  insertZeroes(addr, SIZE_JUMP);
  storeJump(opcode, addr, to);
}

void Regex::insertResetCounter(UINT addr, BYTE counterIndex) {
  insertZeroes(addr, 2);
  m_buffer[addr]   = resetCounter;
  m_buffer[addr+1] = counterIndex;
}

void Regex::insertCountingJump(BYTE opcode, UINT addr, int to, BYTE counterIndex) {
  insertZeroes(addr, SIZE_COUNTINGJUMP);
  storeCountingJump(opcode, addr, to, counterIndex);
}

void Regex::storeOpcode(BYTE opcode, UINT addr) {
  assertHasSpace(addr, 1);
  m_buffer[addr] = opcode;
}

// Store a jump(to) operation at address addr in m_buffer
// opcode is the opcode to store.
void Regex::storeJump(BYTE opcode, UINT addr, int to) {
  assertHasSpace(addr, SIZE_JUMP);
  m_buffer[addr] = opcode;
  storeShort(addr+1, to-(addr+SIZE_JUMP));
}

void Regex::storeCountingJump(BYTE opcode, UINT addr, int to, BYTE counterIndex) {
  assertHasSpace(addr, SIZE_COUNTINGJUMP);
  m_buffer[addr]   = opcode;
  m_buffer[addr+1] = counterIndex;
  storeShort(addr+2, to-(addr+SIZE_COUNTINGJUMP));
}

void Regex::storeShort(UINT addr, short s) {
  storeData(addr, &s, sizeof(s));
}

void Regex::appendUShort(unsigned short s) {
  m_buffer.append((BYTE*)&s, sizeof(s));
}

void Regex::appendCharacter(TCHAR ch) {
  m_buffer.append((BYTE*)&ch, sizeof(ch));
}

void Regex::storeData(UINT addr, const void *data, UINT size) {
  assertHasSpace(addr, size);
  memcpy(&m_buffer[addr], data, size);
}

void Regex::assertHasSpace(UINT addr, UINT count) { // extend m_buffer if needed
  const int needed = (int)addr + (int)count - (int)m_buffer.size();
  if(needed > 0) {
    m_buffer.appendZeroes(needed);
  }
}

#define appendByte(        b         )               m_buffer.append(b)
#define appendShort(       s         )               storeShort((int)m_buffer.size(), s)
#define appendOpcode(      opcode    )               appendByte((BYTE)(opcode))
#define appendJump(        opcode, to)               storeJump(        opcode, (int)m_buffer.size(), to)
#define appendCountingJump(opcode, to, counterIndex) storeCountingJump(opcode, (int)m_buffer.size(), to, counterIndex)

// --------------------------------- Compile Pattern ---------------------------------------------------

class ParStackElement {
public:
  BYTE              m_regno;
  int               m_lastStart;
  int               m_beginAlternative;
  int               m_LCCharIndex;
#ifdef _DEBUG
  int               m_dbgStartAlternative;
#endif
  CompactShortArray m_jumpFixups;
  inline ParStackElement() : m_regno(1), m_lastStart(-1), m_beginAlternative(0)
  {
#ifdef _DEBUG
    m_dbgStartAlternative = 0;
#endif
  }

  inline bool hasLastStart() const {
    return m_lastStart >= 0;
  }

#ifdef _DEBUG
  inline String toString() const {
    return format(_T("%d,%d,%d,dbgAlt:%d, Fixups:%s")
          ,m_regno, m_lastStart, m_beginAlternative
          ,m_dbgStartAlternative
          ,m_jumpFixups.toStringBasicType().cstr());
  }
#endif
};

class CompilerStack : public Stack<ParStackElement> { // not CompactStack here, We have a CompactArray in the elements
public:
  bool   regnoIsOnStack(UINT regno) const;
  String getDefinedRegStr(UINT regnum) const;
};

bool CompilerStack::regnoIsOnStack(UINT regno) const {
  for(UINT h = 0; h < getHeight(); h++) {
    if(top(h).m_regno == regno) {
      return true;
    }
  }
  return false;
}

String CompilerStack::getDefinedRegStr(UINT regnum) const { // return a string with regno < regnum that is not on the stack (excluding top element
  BitSet regSet(10);
  regSet.add(1, min(9,regnum-1));
  for(UINT h = 0; h < getHeight(); h++) {
    regSet.remove(top(h).m_regno);
  }
  if(regSet.isEmpty()) {
    return EMPTYSTRING;
  } else {
    return format(_T("Defined %s %s"), (regSet.size()==1)?_T("is"):_T("are"), regSet.toString().cstr());
  }
}

void Regex::compilePattern(const String &s, const TCHAR *translateTable) {
  compilePattern(s.cstr(), translateTable);
}

void Regex::compilePattern(const TCHAR *s, const TCHAR *translateTable) {
  if(!s) {
    RegexScanner::error(0,noRegExpressionMsg);
  }
  m_translateTable = translateTable;
  compilePattern1(s);
}

// compilePattern1 takes a string containing a regular-expression in the user's format
// and converts it into a buffer full of byte commands for matching.
//
void Regex::compilePattern1(const TCHAR *pattern) {
  RegexScanner                     scanner(pattern, m_translateTable);
  CompilerStack                    stack;
  CharSetMap                       usedCharSetMap;
  RegisterInfoTable                registerTable;
  ParStackElement                  state;
  ByteInsertHandler                insertHandler(usedCharSetMap, registerTable);
  int                              pendingExact     = -1;

  m_insertHandler    = &insertHandler;
  m_buffer.clear();
  m_codeSize         = 0;
  m_counterTableSize = 0;
  m_hasCompiled = false;

  DBG_setCodeDirty(true);

#define pc               ((unsigned short)m_buffer.size())
#define LASTSTART        state.m_lastStart
#define REGNUM           state.m_regno
#define BEGINALTERNATIVE state.m_beginAlternative

  DBG_clear();

  while(!scanner.eos()) {

    DBG_callCompilerHandler(EMPTYSTRING);

    DBG_saveIndex(commandChar);
    const int commandStartIndex = scanner.getIndex();
    _TUCHAR c = scanner.fetch();

    switch(c) {
    case _T('*'):
    case _T('+'):
    case _T('?'):

      { if(!state.hasLastStart()) { // If there is no previous pattern, char not special.
          goto NormalChar;
        }
        const bool zeroTimesOk = (c != _T('+'));
        const bool manyTimesOk = (c != _T('?'));
        pendingExact = -1;

        if(manyTimesOk) {
          appendJump(maybePopAndJump, LASTSTART - SIZE_JUMP);
          DBG_appendJump(scanner.getIndex()-1);
        }

        insertJump(onFailureJump, LASTSTART, pc + SIZE_JUMP);
        DBG_insertJump(LASTSTART, DBG_getIndex(commandChar));

        if(!zeroTimesOk) { // At least one repetition required: insert before the loop
                           // a skip over the initial on-failure-jump instruction
          insertJump(dummyFailureJump, LASTSTART, LASTSTART + 2*SIZE_JUMP);
          DBG_insertJump(LASTSTART, DBG_getIndex(commandChar));
        }
      }
      break;

    case _T('{'):
      { if(!state.hasLastStart()) {
          goto NormalChar;
        }
        const int LCIndex = scanner.getIndex();
        DBG_saveIndex(startRepeat);
        DBG_saveIndex(endRepeat);

        const unsigned short minRepeat = scanner.fetchShort();
        unsigned short maxRepeat = minRepeat;
        if(scanner.look() == _T(',')) {
          scanner.fetch();
          DBG_setIndex(endRepeat);
          maxRepeat = 0;         // Meaning no upper limit of repeat count
          if(_istdigit(scanner.look())) {
            const int numberIndex = scanner.getIndex();
            maxRepeat = scanner.fetchShort();
            if(maxRepeat < minRepeat) {
              scanner.error(numberIndex, minMaxRepeatMsg);
            }
            if(maxRepeat == 0) { // If specified, it must be > 0
              scanner.error(numberIndex, maxRepeatIsZeroMsg);
            }
          }
        }

        if(scanner.look() != _T('}')) {
          scanner.error(expectedRCurlMsg);
        } else {
          scanner.fetch();
        }
        pendingExact = -1;

        if((maxRepeat == 0) && (minRepeat <= 1)) { // Like '+' or '*' closure
          appendJump(maybePopAndJump, LASTSTART - SIZE_JUMP);
          DBG_appendJump(DBG_getIndex(startRepeat));
          insertJump(onFailureJump, LASTSTART, pc + SIZE_JUMP);
          DBG_insertJump(LASTSTART, DBG_getIndex(commandChar));
          if(minRepeat == 1) {                     // Like '+' closure
            insertJump(dummyFailureJump, LASTSTART, LASTSTART + 2*SIZE_JUMP);
            DBG_insertJump(LASTSTART, DBG_getIndex(startRepeat));
          }
        } else if(maxRepeat == 1) {
          if(minRepeat == 1) {                     // Just match once. => No jumps
            // do nothing
          } else if(minRepeat == 0) {              // Like '?' closure.
            insertJump(onFailureJump, LASTSTART, pc + SIZE_JUMP);
            DBG_insertJump(LASTSTART, DBG_getIndex(startRepeat));
          }
        } else {                                   // Can loop up to maxRepeat or forever if this is 0

          if(m_counterTableSize >= RE_MAXCOUNTER) {
            scanner.error(LCIndex, tooManyCountersMsg, RE_MAXCOUNTER);
          }

          const _RegexCounterRange counterRange(minRepeat, maxRepeat, -1); //<-- TODO
          (_RegexCounterRange&)(m_counterTable[m_counterTableSize]) = counterRange;

          insertResetCounter(LASTSTART, m_counterTableSize);
          DBG_insertResetCounter(LASTSTART, DBG_getIndex(commandChar));
          LASTSTART+=2;

          appendCountingJump(countAndJump /*maybePopCountAndJump*/, LASTSTART - SIZE_COUNTINGJUMP, m_counterTableSize);
          DBG_appendCountingJump(DBG_getIndex(endRepeat));

          insertCountingJump(onFailureJumpPushCounter, LASTSTART, pc, m_counterTableSize);
          DBG_insertCountingJump(LASTSTART, DBG_getIndex(startRepeat));

          const RegisterInfo *ri = registerTable.findLastRegisterInCountingLoop(stack.getHeight(), LASTSTART);
          if(ri != NULL) {
            m_counterTable[m_counterTableSize].m_regno = ri->m_regno;
          }
          m_counterTableSize++;
        }
      }
      break;

    case _T('.'):
      LASTSTART = pc;
      appendOpcode(anychar);
      DBG_appendOpcode(DBG_getIndex(commandChar));
      break;

    case _T('['):
      { LASTSTART = pc;
        bool inclusiveCharSet = true;
        if(scanner.look() == _T('^')) {
          inclusiveCharSet = false;
          scanner.fetch();
        }

        ByteBitSet legalCharSet;

        const int startIndex = scanner.getIndex();
        for(bool done = false; !done;) { // Read in characters and ranges, setting legal bits
          _TUCHAR c = scanner.fetch();
          switch(c) {
          case _T(']'):
            done = true;
            break;
          case _T('\\'):
            c = scanner.fetch();
            legalCharSet.add(c);
            break;
          default:
            if(scanner.look() == _T('-')) {
              scanner.fetch(); // eat '-'
              const _TUCHAR c1 = scanner.fetch();
              legalCharSet.add(c, c1);
            } else {
              legalCharSet.add(c);
            }
            break;
          }
        }
        legalCharSet.stopAdding();

        CompactIntArray *fixupArray = usedCharSetMap.get(legalCharSet);
        if(fixupArray == NULL) {
          usedCharSetMap.put(legalCharSet, CompactIntArray());
          fixupArray = usedCharSetMap.get(legalCharSet);
        }
        appendOpcode(inclusiveCharSet ? charSet : charSetNot);
        DBG_appendOpcode(DBG_getIndex(commandChar));
        fixupArray->add(pc);
        appendShort(-1); // will be fixed after this loop
        DBG_appendShort();
      }
      break;

    case _T('^'):
      if(state.hasLastStart()) { // ^ means succeed if at start of line, but only if no preceding pattern.
        goto NormalChar;
      }
      appendOpcode(beginLine);
      DBG_appendOpcode(DBG_getIndex(commandChar));
      break;

    case _T('$'): // $ means succeed if at end of line, but only in special contexts.
                  // If in the middle of a pattern, it is a normal TCHAR.
      if(scanner.eos() || (scanner.look() == _T('\\') && (scanner.look(1) == _T(')') || scanner.look(1) == _T('|')))) {
        appendOpcode(endLine);
        DBG_appendOpcode(DBG_getIndex(commandChar));
        break;
      }
      goto NormalChar;

    case _T('\\'):
      { if(scanner.eos()) {
          scanner.error(commandStartIndex, unexpectedEndMsg);
        }
        DBG_saveIndex(escCommandChar);
        c = scanner.fetchRaw();
        switch(c) {
        case _T('('):
          if(REGNUM < RE_NREGS) {
            registerTable.add(RegisterInfo(REGNUM, stack.getHeight(), pc));
            appendOpcode(startMemory);
            appendByte(REGNUM);
            DBG_appendOpcode(DBG_getIndex(escCommandChar));  DBG_appendByte();
          }
          LASTSTART           = pc;
          state.m_LCCharIndex = commandStartIndex;
          stack.push(state);
          REGNUM++;
          LASTSTART        = -1;
          BEGINALTERNATIVE = pc;

          DBG_beginAlternative();
          break;

        case _T(')'):
          { if(stack.isEmpty()) {
              scanner.error(commandStartIndex, unmatchedRPMsg);
            }
            const ParStackElement e = stack.pop();
            for(int i = 0; i < (int)e.m_jumpFixups.size(); i++) {
              const short jumpIns = e.m_jumpFixups[i];
              storeJump(jump, jumpIns, pc);
            }
            if(e.m_regno < RE_NREGS) {
              registerTable.findByRegno(e.m_regno).m_addressStopMemory = pc;
              appendOpcode(stopMemory);
              appendByte(e.m_regno);
              DBG_appendOpcode(DBG_getIndex(escCommandChar)); DBG_appendByte();
            }
            LASTSTART        = e.m_lastStart;
            BEGINALTERNATIVE = e.m_beginAlternative;
          }
          break;

        case _T('|'):
          insertJump(onFailureJump, BEGINALTERNATIVE, pc + 2*SIZE_JUMP);
          DBG_insertJump(BEGINALTERNATIVE, stack.top().m_dbgStartAlternative);

          stack.top().m_jumpFixups.add(pc);
          appendJump(jump, 0); // will be fixed when \) is parsed
          DBG_appendJump(DBG_getIndex(escCommandChar)-2);

          LASTSTART        = -1;
          BEGINALTERNATIVE = pc;

          pendingExact     = -1;
          DBG_beginAlternative();
          break;

        case _T('`'):
          appendOpcode(beginBuf);
          DBG_appendOpcode(DBG_getIndex(escCommandChar));
          break;

        case _T('\''):
          appendOpcode(endBuf);
          DBG_appendOpcode(DBG_getIndex(escCommandChar));
          break;

        case _T('<'):
          appendOpcode(beginWord);
          DBG_appendOpcode(DBG_getIndex(escCommandChar));
          break;

        case _T('>'):
          appendOpcode(endWord);
          DBG_appendOpcode(DBG_getIndex(escCommandChar));
          break;

        case _T('b'):
          appendOpcode(wordBound);
          DBG_appendOpcode(DBG_getIndex(escCommandChar));
          break;

        case _T('B'):
          appendOpcode(notWordBound);
          DBG_appendOpcode(DBG_getIndex(escCommandChar));
          break;

        case _T('w'):
          LASTSTART = pc;
          appendOpcode(wordchar);
          DBG_appendOpcode(DBG_getIndex(escCommandChar));
          break;

        case _T('W'):
          LASTSTART = pc;
          appendOpcode(notWordchar);
          DBG_appendOpcode(DBG_getIndex(escCommandChar));
          break;

        case _T('1'):
        case _T('2'):
        case _T('3'):
        case _T('4'):
        case _T('5'):
        case _T('6'):
        case _T('7'):
        case _T('8'):
        case _T('9'):
          { const UINT c1 = c - _T('0');
            if((c1 >= REGNUM) || stack.regnoIsOnStack(c1)) {
              scanner.error(commandStartIndex, regnoNotDefined, c1, stack.getDefinedRegStr(REGNUM).cstr());
            }
            LASTSTART = pc;
            appendOpcode(duplicate);
            appendByte(c1);
            DBG_appendOpcode(DBG_getIndex(escCommandChar)); DBG_appendByte();
          }
          break;

        default:
          goto NormalChar;
        } /* end switch */
      } // end case '\\'
      break;

    default   :
    NormalChar:
      { // the expression (pendingExact + 1 + m_buffer[pendingExact] * sizeof(TCHAR))
        // is the expected size of the buffer if wde are appending character at the end
        // pendingExact points to the counter byte following the exactMatch opcode
        // and then followed by the string itself

        const _TUCHAR c1 = scanner.look();
        if((pendingExact < 0)
        ||  pendingExact + 1 + m_buffer[pendingExact] * sizeof(TCHAR) != (int)pc
        ||  m_buffer[pendingExact] == 255
        ||  isClosureCharacter(c1) || c1 == _T('^')) {
          LASTSTART = pc;
          appendOpcode(exactMatch);

          DBG_appendOpcode(DBG_getIndex(commandChar)); DBG_appendByte();

          pendingExact = pc;
          appendByte(0); // byte following exactMatch counts the length of the string
        }
        appendCharacter(c);
        m_buffer[pendingExact]++;
        DBG_appendCharacter();
      }
    } // end switch
  } // end while

  DBG_appendOpcode(scanner.getIndex()-1); // end - not existing byte, but pc will point here when match succeeded

  if(!stack.isEmpty()) {
    scanner.error(stack.top().m_LCCharIndex, unmatchedLPMsg);
  }

  m_codeSize = pc;

  // Save the charsets used in the code, and fixup the references
  for(Iterator<Entry<ByteBitSet, CompactIntArray> > it = usedCharSetMap.entrySet().getIterator(); it.hasNext();) {
    const Entry<ByteBitSet, CompactIntArray> &e = it.next();
    const short addr = pc;
    e.getKey().append(m_buffer);
    const CompactIntArray &fixupArray = e.getValue();
    for(int i = 0; i < (int)fixupArray.size(); i++) {
      const short setEntry = fixupArray[i];
      storeShort(setEntry, addr - setEntry - sizeof(short));
    }
  }

  DBG_callCompilerHandler(EMPTYSTRING);

  m_fastMap = first(0, m_codeSize, &m_matchEmpty);
  m_hasCompiled = true;

  DBG_callCompilerHandler(fastMapToString());
}

#undef pc

// --------------------------------- createFastMap ---------------------------------------------------

// Given a compiled pattern in m_buffer, compute a fastmap from it.
// The fastmap records which of the MAXCHARSETSIZE possible characters
// that can possibly start a string that matches the pattern.
// This is used by search to skip quickly over totally implausible text.

#define DUMPSTACK { int n = stack.getHeight(); for(int i=0;i<n;i++){debugLog(_T("%d "),stack.top(n-i-1)-m_buffer.getData());} debugLog(_T("\n")); }

BitSet Regex::first(intptr_t pcStart,intptr_t pcEnd, bool *matchEmpty) const {  // in range [pcstart;pcend[. pcstart inclusive, pcend exclusive
  CompactStack<const BYTE*> stack;
  const BYTE               *codeStart = m_buffer.getData();
  const BYTE               *pc0       = codeStart + pcStart;
  const BYTE               *pcend     = codeStart + pcEnd;
  const BYTE               *pc        = pc0;
  const int                 maxIteration = 10000;

  if(matchEmpty) {
    *matchEmpty = false;
  }
  BitSet result(MAXCHARSETSIZE);
  int iter;
  for(iter = 0; iter < maxIteration; iter++) {
    if(pc < pc0) {
      throwException(illegalJumpMsg, pcStart,pcEnd, pc - pc0);
    }
//    debugLog("pc:%3d. stack:", pc-m_buffer.getData()); DUMPSTACK

    if(pc == pcend) {
      if(matchEmpty) {
        *matchEmpty = true;
      }
    }
    if(pc < pcend) {
      const RegexOPCode opcode = (RegexOPCode)*pc++;
      switch(opcode) {
      case onFailureJump            :
        { const int jumpCount = getJumpAddress(pc);
          stack.push(pc + jumpCount);
        }
        continue;

      case maybePopAndJump          :
      case popAndJump               :
      case jump                     :
      case dummyFailureJump         :
        { int jumpCount = getJumpAddress(pc);
          pc += jumpCount;
          if(jumpCount > 0) {
            continue;
          }

        // Jump backward reached implies we just went through the body of a loop and matched nothing.
        // Opcode jumped to should be an onFailureJump. Just treat it like an ordinary jump.
        // For a * loop, it has pushed its failure point already; If so, discard it as redundant.

          if(*pc != onFailureJump) {
            continue;
          }
          pc++;
          jumpCount = getJumpAddress(pc);
          pc += jumpCount;
          if(!stack.isEmpty() && stack.top() == pc) {
            stack.pop();
          }
          continue;
        }

      case onFailureJumpPushCounter :
        { const BYTE counterIndex = *(pc++);
          const int  jumpCount    = getJumpAddress(pc);
          if((m_counterTable[counterIndex].m_minRepeat == 0)) { // In this case the commands following the loop should also be added to fastmap
            stack.push(pc + jumpCount);
          }
        }
        continue;

      case maybePopCountAndJump     :
      case popCountAndJump          :
      case countAndJump             : // always backwards jump, because these are the end of the loop. Se comment above at case maybePopAndJump
        { pc++;                       // skip counterIndex;
          int jumpCount = getJumpAddress(pc);
          if(pc == codeStart + m_codeSize) {
            break;
          }
          pc += jumpCount;
          assert(*pc == onFailureJumpPushCounter);
          pc++;                       // skip opcode
          pc++;                       // skip counterIndex
          pc += getJumpAddress(pc);
          if(!stack.isEmpty() && stack.top() == pc) {
            stack.pop();
          }
        }
        continue;

      case resetCounter             :
        pc++;
        continue;

      case startMemory              :
      case stopMemory               :
        pc++;
        continue;

      case exactMatch               :
        result.add(TRANSLATE(*(_TUCHAR*)(pc+1)));
        break;

      case anychar                  :
        { for(int j = 0; j < MAXCHARSETSIZE; j++) {
            if(TRANSLATE(j) != NEWLINE) {
              result.add(j);
            }
          }
        }
        goto Return;

      case charSet                  :
      case charSetNot               :
        { const CharSet set(opcode, pc);
          for(UINT j = 0; j < MAXCHARSETSIZE; j++) {
            if(set.isLegalChar(j)) {
              result.add(TRANSLATE(j));
            }
          }
        }
        break;

      case beginBuf                 :
      case endBuf                   :
      case beginLine                :
      case beginWord                :
      case endWord                  :
      case wordBound                :
      case notWordBound             :
        continue;

      case endLine           :
        result.add(TRANSLATE(NEWLINE));
        break;

      case wordchar                 :
        { for(int j = 0; j < MAXCHARSETSIZE; j++) {
            if(isWordLetter(j)) {
              result.add(j);
            }
          }
        }
        break;

      case notWordchar              :
        { for(int j = 0; j < MAXCHARSETSIZE; j++) {
            if(!isWordLetter(j)) {
              result.add(j);
            }
          }
        }
        break;

      default                       :
        throwException(invalidOpcodeMsg, opcode);
        break;

      } // end switch
    } // end if

// Get here means we have successfully found the possible starting characters
// of one path of the pattern. We need not follow this path any farther.
// Instead, look at the next alternative remembered in the stack.
    bool found = false;
    while(!stack.isEmpty()) {
      if((pc = stack.pop()) <= pcend) {
        found = true;
        break;
      }
    }
    if(!found) {
      break;;
    }
  } // end for

  if(iter == maxIteration) {
    throwException(_T("Regex::first(%d,%d):Max iteration reached"), pcStart,pcEnd);
  }

Return:
  return result;
}

// --------------------------------- search ---------------------------------------------------

#ifdef _DEBUG
#define DBG_resetCycleCount()   m_cycleCount = 0
#define DBG_incrCycleCount()    m_cycleCount++
#else
#define DBG_resetCycleCount()
#define DBG_incrCycleCount()
#endif

intptr_t Regex::search(const String &text, bool forward, intptr_t startPos, RegexRegisters *registers) const {
  return search(text.cstr(), forward, startPos, registers);
}

intptr_t Regex::search(const TCHAR *text, bool forward, intptr_t startPos, RegexRegisters *registers) const {
  if(!m_hasCompiled) {
    throwException(noRegExpressionMsg);
  }
  DBG_resetCycleCount();
  const int length = (int)_tcsclen(text);
  if(startPos < 0) {
    startPos = forward ? 0 : length;
  } else if(startPos > length) {
    startPos = length;
  }
  if(forward) {
    return search1(text, length, startPos, length - startPos, registers);
  } else {
    return search1(text, length, startPos, -startPos, registers);
  }
}

// Like search2, below, but only one String is specified.

intptr_t Regex::search1(const TCHAR     *string
                       ,size_t           size
                       ,intptr_t         startPos
                       ,intptr_t         range
                       ,RegexRegisters  *registers) const {

  return search2(0, 0, string, size, startPos, range, registers, size);
}

// Like match2 but tries first a match starting at index `startPos',
// then at startPos + 1, and so on.
// `range' is the number of places to try before giving up.
// If `range' is negative, the starting positions tried are
// startPos, startPos - 1, etc.
// It is up to the caller to make sure that range is not so large
// as to take the starting position outside of the input strings.
//
// The value returned is the position at which the match was found,
// or -1 if no match was found.

intptr_t Regex::search2(const TCHAR     *string1
                       ,size_t           size1
                       ,const TCHAR     *string2
                       ,size_t           size2
                       ,intptr_t         startPos
                       ,intptr_t         range
                       ,RegexRegisters  *registers
                       ,size_t           mstop) const {
  const intptr_t total = size1 + size2;

#define GETCP(pos) (((pos) >= (intptr_t)size1) ? (string2 + (pos) - size1) : (string1 + (pos)))

  for(;;) {
    // skip quickly over characters that cannot possibly be the start of a match.
    // Note, however, that if the pattern can possibly match
    // the null String, we must test it at each starting point
    // so that we take the first null String we get.

    if((startPos < total) && !m_matchEmpty) {
      if(range > 0) {
        intptr_t lim    = 0;
        intptr_t irange = range;
        if((startPos < (intptr_t)size1) && (startPos + range >= (intptr_t)size1)) {
          lim = range - ((intptr_t)size1 - startPos);
        }

        const TCHAR *p = GETCP(startPos);

        while((range > lim) && !m_fastMap.contains(TRANSLATE(*p++))) {
          DBG_incrCycleCount();
          DBG_callSearchHandler(startPos, (p-1 - string2));
          range--;
        }
        startPos += irange - range;
      } else {
        TCHAR c = *GETCP(startPos);

        if(!m_fastMap.contains(TRANSLATE(c))) {
          DBG_incrCycleCount();
          DBG_callSearchHandler(startPos, startPos);
          goto advance;
        }
      }
    } // end if

    if((range >= 0) && (startPos == total) && !m_matchEmpty) {
      return -1;
    }

    if((m_resultLength = match2(string1, size1, string2, size2, startPos, registers, mstop)) >= 0) {
      return startPos;
    }

  advance:
    if(!range) {
      break;
    }
    if(range > 0) {
      range--;
      startPos++;
    } else {
      range++;
      startPos--;
    }
  } // end for
  return -1;
}

// --------------------------------- match ---------------------------------------------------

bool Regex::match(const String &text, RegexRegisters *registers) const {
  if(!m_hasCompiled) {
    throwException(noRegExpressionMsg);
  }
  return (match(text.cstr(), text.length(), 0, registers) >= 0) && (m_resultLength == text.length());
}

bool Regex::match(const TCHAR *text, RegexRegisters *registers) const {
  if(!m_hasCompiled) {
    throwException(noRegExpressionMsg);
  }
  const size_t len = _tcsclen(text);
  return (match(text, len, 0, registers) >= 0) && (m_resultLength == len);
}

intptr_t Regex::match(const TCHAR     *string
                     ,size_t           size
                     ,intptr_t         pos
                     ,RegexRegisters  *registers) const {
  DBG_resetCycleCount();
  m_resultLength = match2(0, 0, string, size, pos, registers, size);
  return m_resultLength;
}

intptr_t Regex::match2(const TCHAR    *string1
                      ,size_t          size1
                      ,const TCHAR    *string2
                      ,size_t          size2
                      ,intptr_t        pos
                      ,RegexRegisters *registers
                      ,size_t          mstop) const {

  _RegexMatchState state(this, m_counterTable, pos);

  if(size2 == 0) {
    string2 = string1;
    size2   = size1;
    string1 = NULL;
    size1   = 0;
  }
  state.m_end1 = string1 + size1;
  state.m_end2 = string2 + size2;

  if(mstop <= size1) { // Compute where to stop matching, within the two strings
    state.m_endMatch1 = string1 + mstop;
    state.m_endMatch2 = string2;
  } else {
    state.m_endMatch1 = state.m_end1;
    state.m_endMatch2 = string2 + mstop - size1;
  }

// state.m_ip scans through the pattern as state.m_sp scans through the data.
// spend is the end of the input string that sp scans
// d is advanced into the following input string whenever necessary,
// but this happens before fetching; therefore, at the beginning of the loop,
// d can be pointing at the end of a String, but it cannot equal string2.

  if(pos <= (intptr_t)size1) {
    state.m_sp    = string1 + pos;
    state.m_spEnd = state.m_endMatch1;
  } else {
    state.m_sp    = string2 + pos - size1;
    state.m_spEnd = state.m_endMatch2;
  }

// Use PREFETCH just before fetching a TCHAR with *d.
#define PREFETCH                           \
while(state.m_sp == state.m_spEnd) {       \
  if(state.m_spEnd == state.m_endMatch2) { \
    goto Fail;                             \
  }                                        \
  state.m_sp    = string2;                 \
  state.m_spEnd = state.m_endMatch2;       \
}

  // This loop loops over pattern commands. It exits by returning from the function if match is complete,
  // or it drops through if match fails at this starting point in the input data

  for(;;) {
    DBG_incrCycleCount();
    DBG_callMatchHandler();
    if(state.m_ip == state.m_ipEnd) { // End of pattern means we have succeeded!
      if(registers) {                 // If caller wants register contents data back, convert it to indices
        state.convertRegisters(string1, size1, string2, size2, registers);
      }
      if((state.m_sp - string1) >= 0 && (state.m_sp - string1 <= (intptr_t)size1)) {
        return state.m_sp - string1 - pos;
      } else {
        return state.m_sp - string2 + size1 - pos;
      }
    } // end if

// Otherwise match next pattern command

    const RegexOPCode opcode = (RegexOPCode)*(state.m_ip++);
    switch(opcode) {

// "or" constructs ("|") are handled by starting each alternative
// with an onFailureJump that points to the start of the next alternative.
// Each alternative except the last ends with a jump to the joining point.
// (Actually, each jump except the last one really jumps
// to the following jump, because tensioning the jumps is a hassle.)
//
// The start of a stupid repeat has an onFailureJump that points
// past the end of the repeat text.
// This makes a failure point so that, on failure to match a repetition,
// matching restarts past as many repetitions have been found
// with no way to fail and look for another one.
//
// A smart repeat is similar but loops back to the onFailureJump
// so that each repetition makes another failure point.

    case onFailureJump:
      state.doOnFailureJump();
      break;

    case maybePopAndJump:   // The end of a smart repeat has a maybePopAndJump back.
                            // Change it either to a popAndJump or an ordinary jump.
      state.doMaybePopAndJump();
      DBG_setCodeDirty(true);
      break;

    case popAndJump     :
      state.doPopAndJump();
      break;

    case jump      :
      state.doJump();
      break;

    case dummyFailureJump      :
      state.doDummyFailureJump();
      break;

    case onFailureJumpPushCounter:
      state.doOnFailureJumpPushCounter();
      break;

    case maybePopCountAndJump:
      state.doMaybePopAndJump();
      DBG_setCodeDirty(true);
      break;

    case popCountAndJump:
      state.doPopCountAndJump();
      break;

    case countAndJump:
      state.doCountAndJump();
      break;

    case resetCounter:
      state.doResetCounter();
      break;

// \( is represented by a startMemory, \) by a stopMemory.
// Both of those commands contain a "register number" argument.
// The text matched within the \( and \) is recorded under that number.
// Then, \<digit> turns into a `duplicate' command which
// is followed by the numeric value of <digit> as the register number.

    case startMemory:
      state.doStartMemory();
      break;

    case stopMemory:
      state.doStopMemory();
      break;

    case duplicate:
      { const _RegexStateRegister &reg = state.getRegister(*state.m_ip++);
        const TCHAR *d2    = reg.m_regStart;
        const TCHAR *dend2 = reg.m_regStartSegEnd;
        for(;;) {                                    // Advance to next segment in register contents, if necessary
          while(d2 == dend2) {
            if(dend2 == state.m_endMatch2) {
              break;
            }
            if(dend2 == reg.m_regEnd) {
              break;
            }
            d2    = string2;
            dend2 = reg.m_regEnd;                                               // end of string1 => advance to string2.
          }

          if(d2 == dend2) {                                                     // At end of register contents => success
            break;
          }

          PREFETCH;                                                             // Advance to next segment in data being matched, if necessary

          intptr_t count = state.m_spEnd - state.m_sp;                          // count gets # consecutive chars to compare
          if(count > dend2 - d2) {
            count = dend2 - d2;
          }

          if(compareStrings(state.m_sp, d2, count)) {                           // Compare count characters;
            goto Fail;                                                          // Failure if mismatch, else skip them.
          }
          state.m_sp += count;
          d2 += count;
        }
      }
      break;

    case exactMatch:                                                            // Match the next few pattern characters exactly.
      { for(int count = *state.m_ip++; count--; state.m_ip += sizeof(TCHAR)) {
          PREFETCH;
          if(TRANSLATE(*state.m_sp++) != *(_TUCHAR*)state.m_ip) {
            goto Fail;
          }
        }
      }
      break;

    case anychar:
      PREFETCH;                                                                 // fetch a data TCHAR
      if(TRANSLATE(*state.m_sp++) == NEWLINE) {                                 // Match anything but a newline.
        goto Fail;
      }
      break;

    case charSet   :
    case charSetNot:
      { const CharSet set(opcode, state.m_ip);
        PREFETCH;                                                               // fetch a data TCHAR
        const _TUCHAR c = TRANSLATE(*state.m_sp);
        if(!set.isLegalChar(c)) {
          goto Fail;
        }
        state.m_sp++;
      }
      break;

    case beginBuf:
      if(state.m_sp == string1) {                   // Note, d cannot equal string2, unless string1 == string2.
        break;
      }
      goto Fail;

    case endBuf:
      if((state.m_sp == state.m_end2) || ((state.m_sp == state.m_end1) && (size2 == 0))) {
        break;
      }
      goto Fail;

    case beginLine:
      if((state.m_sp == string1) || (state.m_sp[-1] == NEWLINE)) {
        break;
      }
      goto Fail;

    case endLine:
      if((state.m_sp == state.m_end2) || (state.m_sp == state.m_end1 ? (size2 == 0 || *string2 == NEWLINE) : *state.m_sp == NEWLINE)) {
        break;
      }
      goto Fail;

    case beginWord:
      if((state.m_sp == state.m_end2) || ((state.m_sp == state.m_end1) && (size2 == 0)) || !isWordLetter(state.m_sp == state.m_end1 ? *string2 : *state.m_sp)) { // Next char not a letter
        goto Fail;
      }
      if((state.m_sp == string1) || !isWordLetter(state.m_sp[-1])) {
        break;
      }
      goto Fail;

    case endWord:
      if((state.m_sp == string1) || !isWordLetter(state.m_sp[-1])) {            // Prev char not letter
        goto Fail;
      }
      if(  state.m_sp == state.m_end2                                           // Points to end
       || (state.m_sp == state.m_end1 && size2 == 0)                            // Points to end
       || !isWordLetter(state.m_sp == state.m_end1 ? *string2 : *state.m_sp)) { // Next char not a letter
        break;
      }
      goto Fail;

    case wordBound:
      if((state.m_sp == string1) || (state.m_sp == state.m_end2) || (state.m_sp == state.m_end1 && size2 == 0)) {
        break;
      }
      if(isWordLetter(state.m_sp[-1]) != isWordLetter(state.m_sp == state.m_end1 ? *string2 : *state.m_sp)) {
        break;
      }
      goto Fail;

    case notWordBound:
      if((state.m_sp == string1) || (state.m_sp == state.m_end2) || ((state.m_sp == state.m_end1) && (size2 == 0))) {
        goto Fail;
      }
      if(isWordLetter(state.m_sp[-1]) != isWordLetter(state.m_sp == state.m_end1 ? *string2 : *state.m_sp)) {
        goto Fail;
      }
      break;

    case wordchar:
      PREFETCH;
      if(!isWordLetter(*state.m_sp++)) {
        goto Fail;
      }
      break;

    case notWordchar:
      PREFETCH;
      if(isWordLetter(*state.m_sp++)) {
        goto Fail;
      }
      break;

    default:
      throwException(invalidOpcodeMsg, opcode);
      break;

    } // end switch

    continue; // Successfully matched one pattern command; keep matching


    Fail:                                                                       // Jump here if any matching operation fails.
      if(state.isStackEmpty()) {                                                // A restart point is known.
        break;                                                                  // No restartpoint => no Match
      } else {                                                                  // Restart there and pop it.
        if(state.topHasCounter()) {
          if(!state.handleCounterFailure()) {
            goto Fail;
          }
        } else {
          state.popAndRestore();
        }
        if(state.m_ip == NULL) {                                                // If innermost failure point is dormant,
          goto Fail;
        }
                                                                                // flush it and keep looking
        if(state.m_sp >= string1 && state.m_sp <= state.m_end1) {
          state.m_spEnd = state.m_endMatch1;
        }
      }
  } // end for
  return -1;                                                                    // Failure to match
}

// --------------------------------------------- _RegexMatchState --------------------------------------------

_RegexMatchState::_RegexMatchState(const Regex *regex, _RegexCounterTable &counterTable, intptr_t pos)
: m_regex(*regex)
, m_pos(pos)
, m_ip(regex->getCodeStart())
, m_ipEnd(regex->getCodeStart() + regex->getCodeSize())
, m_counterTable(counterTable)
{
#ifdef _DEBUG
  for(BYTE i = 0; i < RE_MAXCOUNTER; i++) { // no need to do this when not debugging. instruction resetCounter will do it anyway
    resetCounter(i);
  }
#endif
}

void _RegexMatchState::doMaybePopAndJump() {
// The end of a smart repeat has a maybePopAndJump/maybePopCountAndJump back.
// Change it either to a popAndJump/popCountAndJump or jump/countAndJump,
// depending on wether intersection of first(loop) and first(what follows loop) is empty or not

  BYTE       &opcodeRef    = ((BYTE*)m_ip)[-1];
  BYTE        counterIndex = -1;
  int         jumpCount;
  int         paramSize;
  RegexOPCode popAndJumpOpcode;

  switch(opcodeRef) {
  case maybePopAndJump     :
    jumpCount        = getJumpAddress(m_ip);
    paramSize        = SIZE_JUMP;
    popAndJumpOpcode = popAndJump;
    break;
  case maybePopCountAndJump:
    counterIndex     = *(m_ip++);
    jumpCount        = getJumpAddress(m_ip);
    paramSize        = SIZE_COUNTINGJUMP;
    popAndJumpOpcode = popCountAndJump;
    break;
  default                  :
    throwException(unexpectedOpcodeMsg, opcodeRef);
  }

// Compare what follows with the begining of the repeat.
// If we can establish that there is nothing that they would both match, we can change to popAndJump/popCountAndJump

  const BYTE    *codeStart      = m_regex.getCodeStart();
  const intptr_t startLoop      = (m_ip + jumpCount) - codeStart;
  const intptr_t endLoop        =  m_ip - codeStart;
  const intptr_t startFollow    = endLoop;
  const int      endFollow      = m_regex.getCodeSize();
  const BitSet   loopFirstSet   = m_regex.first(startLoop  , endLoop  );
  const BitSet   followSet      = m_regex.first(startFollow, endFollow);

#ifdef _DEBUG
  debugLog(_T("loopFirstSet(%d-%d):%s\nfollowSet(%d,%d):%s\n")
   ,startLoop  , endLoop  , charBitSetToString(loopFirstSet  ).cstr()
   ,startFollow, endFollow, charBitSetToString(followSet     ).cstr()
  );
#endif

  if((loopFirstSet * followSet).isEmpty()) {
    opcodeRef = popAndJumpOpcode;
  }
  switch(opcodeRef) {
  case popAndJump     :
    popNoRestore();
    break;
  case maybePopAndJump     :
    opcodeRef = jump;
    break;
  case popCountAndJump     :
    popNoRestore();
    if(!incrCounter(counterIndex)) {
      return;
    }
    break;
  case maybePopCountAndJump:
    opcodeRef = countAndJump;
    if(!incrCounter(counterIndex)) {
      return; // Dont do the jump. can only happen if we have minRepeat=maxRepeat=0
    }
    break;
  default                  :
    throwException(unexpectedOpcodeMsg, opcodeRef);
  }
  m_ip += jumpCount; // do the jump
}

bool _RegexMatchState::handleCounterFailure() { // Called when failure and stacktop has a counter attached
  const _RegexMatchStackElement &top = m_stack.top();
  if((m_ip < top.m_ip) && isCounterInRange(top.m_counterIndex)) { // still in the loop
    m_ip = top.m_ip + SIZE_COUNTINGJUMP; // skip jump back to loopstart and continue matching after loop
    m_sp = top.m_sp;
    return true;
  }

  const _RegexMatchStackElement e = m_stack.pop();
  if(e.m_counterValue > 0) {
    _RegexCounterRegister &counter = m_counterTable[e.m_counterIndex];
    counter.m_value = e.m_counterValue-1;
    if(counter.hasStateRegister()) {
      stopMemory(counter.m_regno, m_stack.top().m_sp); // stack NOT empty
    }
    counter.m_value = e.m_counterValue-1;
    m_ip = m_stack.top().m_ip;
  }
  return false;
}

void _RegexMatchState::convertRegisters(const TCHAR   *string1
                                      ,intptr_t        size1
                                      ,const TCHAR    *string2
                                      ,intptr_t        size2
                                      ,RegexRegisters *registers) {
  registers->clear();

  _RegexStateRegister &reg0 = m_register[0];
  reg0.m_regEnd   = m_sp;
  reg0.m_regStart = string1;
  for(int i = 0; i < RE_NREGS; i++) {
    if(i && !m_register[i].isSet()) {
      continue;
    }
    _RegexStateRegister &reg = m_register[i];
    if(reg.m_regStart - string1 < 0 || reg.m_regStart - string1 > size1) {
      registers->start[i] = reg.m_regStart - string2 + size1;
    } else {
      registers->start[i] = reg.m_regStart - string1;
    }
    if(reg.m_regEnd - string1 < 0 || reg.m_regEnd - string1 > size1) {
      registers->end[i] = reg.m_regEnd - string2 + size1;
    } else {
      registers->end[i] = reg.m_regEnd - string1;
    }
  }
  registers->start[0] = m_pos;
}

String RegexRegisters::toString(const String &text) const {
  const intptr_t length = text.length();
  String result;
  for(int i = 0; i < RE_NREGS; i++) {
    const intptr_t textstart = start[i];
    const intptr_t textend   = end[i];
    const intptr_t len       = textend - textstart;
    if((0 <= textstart) && (len > 0) && (textend <= length)) {
      result += format(_T("%d:\"%-*.*s\"\n"), i, len,len, text.cstr()+textstart);
    }
  }
  return result;
}

#ifdef _DEBUG

RegexStepHandler *Regex::setHandler(RegexStepHandler *handler) {
  RegexStepHandler *old = m_stepHandler;
  m_stepHandler = handler;
  return old;
}

#define ADD(s)                 result += s;
#define ADDOPCODE(s)           ADD(format(_T("%-12s "), _T(#s)))
#define ADDINT(n)              ADD(format(_T("%d"),n))
#define ADDJUMP(count)         ADD(format(_T(" to %d"), (p-start)+(count)))
#define ADDCOUNTERINDEX(index) ADD(format(_T("C[%d]"), index));

#define ADDNEWLINE     ADD(_T("\n"))

#define SETLINENUMBERS       { for(;lastp < p; lastp++) m_PCToLineArray[lastp - start] |= (lineCount<<16); lineCount++; }
#define CLEARLINENUMBERS()   { for(int n = (int)m_PCToLineArray.size(), *lp = n?(&m_PCToLineArray.first()):NULL; n--;) *(lp++) &= 0x0000ffff; }
#define GETDBGLINENO(   dbg) ((dbg)>>16   )
#define GETDBGCHARINDEX(dbg) ((dbg)&0xffff)

String Regex::toString() const {
  String result;
  UINT lineCount = 0;

  CLEARLINENUMBERS();

  const BYTE *start = m_buffer.getData();
  const BYTE *lastp = start;
  const int maxAddress = m_codeSize ? m_codeSize : (int)m_buffer.size();
  for(const BYTE *p = start;;) {
    const UINT address = (UINT)(p - start);
    ADD(format(_T("%4u: "), address));
    if((int)address >= maxAddress) {
      ADDOPCODE(end);
      p++;
      if(m_codeSize) {
        SETLINENUMBERS
      }
      ADDNEWLINE;
      break;
    }
    const RegexOPCode opcode = (RegexOPCode)*p++;
    switch(opcode) {
    case unused:
      ADDOPCODE(unused);
      break;

    case onFailureJump:       // followed by two bytes giving relative address of place
                              // to resume in case of failure.
      { const int jumpCount = getJumpAddress(p);
        ADDOPCODE(onFailureJump)
        ADDJUMP(jumpCount);
      }
      break;

    case maybePopAndJump:   // Like jump but finalize if safe to do so.
      { const int jumpCount = getJumpAddress(p);
        ADDOPCODE(maybePopAndJump);
        ADDJUMP(jumpCount);
      }
      break;

    case popAndJump:        // Throw away latest failure point and then jump to address.
      { const int jumpCount = getJumpAddress(p);
        ADDOPCODE(popAndJump);
        ADDJUMP(jumpCount);
      }
      break;

    case jump:                // followed by two bytes giving relative address to jump to
      { const int jumpCount = getJumpAddress(p);
        ADDOPCODE(jump)
        ADDJUMP(jumpCount);
      }
      break;

    case dummyFailureJump:    // jump, and push a dummy failure point.
      { const int jumpCount = getJumpAddress(p);
        ADDOPCODE(dummyFailureJump);
        ADDJUMP(jumpCount);
      }
      break;

    case onFailureJumpPushCounter:
      { const BYTE counterIndex = *(p++);
        const int  jumpCount    = getJumpAddress(p);
        ADDOPCODE(onFailureJumpPushCounter)
        ADDCOUNTERINDEX(counterIndex);
        ADD(format(_T(" %s"), m_counterTable[counterIndex].toString().cstr()));
        ADDJUMP(jumpCount);
      }
      break;

    case maybePopCountAndJump:
      { const BYTE counterIndex = *(p++);
        const int  jumpCount    = getJumpAddress(p);
        ADDOPCODE(maybePopCountAndJump)
        ADDCOUNTERINDEX(counterIndex);
        ADDJUMP(jumpCount);
      }
      break;

    case popCountAndJump:
      { const BYTE counterIndex = *(p++);
        const int  jumpCount    = getJumpAddress(p);
        ADDOPCODE(popCountAndJump)
        ADDCOUNTERINDEX(counterIndex);
        ADDJUMP(jumpCount);
      }
      break;

    case countAndJump:
      { const BYTE counterIndex = *(p++);
        const int jumpCount     = getJumpAddress(p);
        ADDOPCODE(countAndJump)
        ADDCOUNTERINDEX(counterIndex);
        ADDJUMP(jumpCount);
      }
      break;

    case resetCounter:
      { const BYTE counterIndex = *(p++);
        ADDOPCODE(resetCounter)
        ADDCOUNTERINDEX(counterIndex);
      }
      break;

    case startMemory:         // Starts remembering the text that is matched
      { const BYTE regno = *p++;
        ADDOPCODE(startMemory);
        ADDINT(regno);
      }
      break;

    case stopMemory:          // Stops remembering the text that is matched
      { const BYTE regno = *p++;
        ADDOPCODE(stopMemory);
        ADDINT(regno);
      }
      break;

    case duplicate:           // Match a duplicate of something remembered.
                              // Followed by one byte containing the index of the memory register.
      { const BYTE regno = *p++;
        ADDOPCODE(duplicate);
        ADDINT(regno);
      }
      break;

    case exactMatch:          // Followed by one byte giving n, and then by n literal TCHARS
      { const BYTE n = *p++;
        ADDOPCODE(exactMatch);
        ADD(format(_T("%u <%*.*s>"),n,n,n,(TCHAR*)p));
        p += n * sizeof(TCHAR);
      }
      break;

    case anychar:             // matches any one TCHAR
      ADDOPCODE(anychar);
      break;

    case charSet      :       // Matches any one character belonging to specified set.
    case charSetNot   :
      { String setStr;
        if(*(USHORT*)(p) > m_buffer.size()) {
          setStr = _T("--");
          getShort(p);
        } else {
          const CharSet set(opcode, p);
          setStr = set.toString();
        }
        switch(opcode) {
        case charSet    : ADDOPCODE(charSet   ); break;
        case charSetNot : ADDOPCODE(charSetNot); break;
        }
        ADD(setStr);
      }
      break;

    case beginBuf:            // Succeeds if at beginning of buffer
      ADDOPCODE(beginBuf);
      break;

    case endBuf:              // Succeeds if at end of buffer
      ADDOPCODE(endBuf);
      break;

    case beginLine:           // fails unless at beginning of line
      ADDOPCODE(beginLine);
      break;

    case endLine:             // fails unless at end of line
      ADDOPCODE(endLine);
      break;

    case beginWord:           // Succeeds if at word beginning
      ADDOPCODE(beginWord);
      break;

    case endWord:             // Succeeds if at word end
      ADDOPCODE(endWord);
      break;

    case wordBound:           // Succeeds if at a word boundary
      ADDOPCODE(wordBound);
      break;

    case notWordBound:        // Succeeds if not at a word boundary
      ADDOPCODE(notWordBound);
      break;

    case wordchar:            // Matches any word-constituent TCHAR
      ADDOPCODE(wordchar);
      break;

    case notWordchar:         // Matches any char that is not a word-constituent
      ADDOPCODE(notWordchar);
      break;

    default:
      ADD(format(_T("Unknown opcode:%d"), opcode));
      break;
    }

    SETLINENUMBERS

    ADDNEWLINE;
  }
  DBG_setCodeDirty(false);
  return result;
}

String Regex::fastMapToString() const {
  return charBitSetToString(m_fastMap);
}

String Regex::countersToString() const {
  String result;
  for(int i = 0; i < (int)m_counterTableSize; i++) {
    if(i > 0) result += _T(" ");
    result += format(_T("C[%d]=%d"), i, m_counterTable[i].m_value);
  }
  return result;
}

void Regex::dump(FILE *f, bool includeFastmap) const {
  _ftprintf(f, _T("%s"), toString().cstr());
  if(includeFastmap) {
    _ftprintf(f, _T("Fastmap:%s\n"), fastMapToString().cstr());
  }
}

BitSet Regex::getPossibleBreakPointLines() const {
  if(!isCompiled() ) {
    return BitSet(10); // empty set
  }
  int maxLine = 0;
  for(size_t i = 0; i < m_PCToLineArray.size(); i++) {
    const int l = GETDBGLINENO(m_PCToLineArray[i]);
    if(l > maxLine) {
      maxLine = l;
    }
  }
  BitSet result(maxLine+1);
  for(size_t i = 0; i < m_PCToLineArray.size(); i++) {
    result.add(GETDBGLINENO(m_PCToLineArray[i]));
  }
  return result;
}

int Regex::getLastCodeLine() const {
  return m_PCToLineArray.isEmpty() ? -1 : GETDBGLINENO(m_PCToLineArray.last());
}

UINT _RegexMatchState::getDBGIpElement() const {
  const UINT             ipIndex = (UINT)(m_ip - m_regex.getCodeStart());
  const CompactIntArray &dbg     = m_regex.getDebugInfo();
  return ((int)ipIndex >= dbg.size()) ? 0 : dbg[ipIndex];
}

UINT _RegexMatchState::getDBGPatternCharIndex() const {
  return GETDBGCHARINDEX(getDBGIpElement());
}

UINT _RegexMatchState::getDBGLineNumber() const {
  return GETDBGLINENO(getDBGIpElement());
}

String _RegexMatchState::stackToString() const {
  String result;
  const BYTE *codeStart = m_regex.getCodeStart();
  const int n = m_stack.getHeight();
  for(int i = 0; i < n; i++) {
    result += format(_T("%2d : %s\n"),  n-i, m_stack.top(i).toString(codeStart).cstr());
  }
  return result;
}

String _RegexMatchState::registersToString() const {
  String result = _T("0: \"\"\n");
  for(int regno = 1; regno < RE_NREGS; regno++) {
    const _RegexStateRegister &reg = m_register[regno];
    if(reg.isSet()) {
      result += format(_T("%d:%s\n"), regno, reg.toString(this).cstr());
    }
  }
  return result;
}

String _RegexMatchState::countersToString() const {
  return m_regex.countersToString();
}

String _RegexMatchStackElement::toString(const BYTE *codeStart) const {
  String result;
  if(m_ip) {
    result = format(_T("<%-5.5s>, OnError %3d")
                   ,m_sp?m_sp:_T("null")
                   ,m_ip - codeStart
                   );
  } else {
    result = format(_T("%7d, 0"),0);
  }
  if(hasCounter()) {
    result += format(_T(" C[%d]=%-3d"), m_counterIndex, m_counterValue);
  }
  return result;
}


String _RegexStateRegister::toString(const _RegexMatchState *state) const {
  int len;
  if(!isSet() || (m_regStart == NULL)) {
    return _T("\"\"");
  }
  if(m_regStartSegEnd == state->m_spEnd) {
    len = (int)(m_regStartSegEnd - m_regStart);
  } else {
    len = (int)(m_regEnd - m_regStart);
  }
  return format(_T("\"%-*.*s\""), len, len, m_regStart);
}

String _RegexCounterRange::toString() const {
  String result = m_maxRepeat ? format(_T("{%u;%u}"), m_minRepeat, m_maxRepeat) : format(_T("{%u;inf}"), m_minRepeat);
  if(hasStateRegister()) {
    result += format(_T("R[%d]"), m_regno);
  }
  return result;
}

#endif
