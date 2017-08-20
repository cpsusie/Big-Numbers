#pragma once

#include "MyUtil.h"
#include "ByteArray.h"
#include "BitSet.h"
#include <Stack.h>
#include "CompactStack.h"

#define RE_NREGS 10

class RegexRegisters {
public:
  intptr_t start[RE_NREGS];
  intptr_t end[RE_NREGS];
  void clear() {
    memset(this, 0, sizeof(RegexRegisters));
  }
  String toString(const String &text) const;
};

class _RegexMatchState;

class _RegexStateRegister {
public:
  const TCHAR *m_regStart;
  const TCHAR *m_regStartSegEnd;
  const TCHAR *m_regEnd;
  inline _RegexStateRegister() {
    m_regStart = (TCHAR*)-1; // Initialize \( and \) text positions to -1 to indicate, that no \( or \) has been seen
#ifdef _DEBUG
    m_regStartSegEnd = m_regEnd = NULL;
#endif
  }
  inline bool isSet() const {
    return m_regStart != (TCHAR*)-1;
  }
  inline void clear() {
    m_regStart = (TCHAR*)-1;
#ifdef _DEBUG
    m_regStartSegEnd = m_regEnd = NULL;
#endif
  }

#ifdef _DEBUG
  String toString(const _RegexMatchState *state) const;
#endif
};

class _RegexCounterRange {
public:
  USHORT m_minRepeat;
  USHORT m_maxRepeat;
  // index of associated StateRegister (not counterRegister)
  BYTE   m_regno;
  _RegexCounterRange()
    : m_minRepeat(-1)
    , m_maxRepeat(-1)
  {
  }

  _RegexCounterRange(USHORT minRepeat, USHORT maxRepeat, BYTE regno)
    : m_minRepeat(minRepeat)
    , m_maxRepeat(maxRepeat)
    , m_regno(regno)
  {
  }
  inline bool contains(UINT n) const {
    return (m_minRepeat <= n) && ((m_maxRepeat==0) || (n <= m_maxRepeat));
  }
  inline bool isGEMin(UINT n) const {
    return n >= m_minRepeat;
  }
  inline bool isBelowMax(UINT n) const {
    return m_maxRepeat && (n < m_maxRepeat);
  }
  inline bool hasStateRegister() const {
    return m_regno < RE_NREGS;
  }
#ifdef _DEBUG
  String toString() const;
#endif
};

class _RegexCounterRegister : public _RegexCounterRange {
public:
  UINT m_value;
  inline bool isValueInRange() const {
    return isGEMin(m_value);
  }
};

#define SIZE_JUMP         (1 + (BYTE)sizeof(short))
#define SIZE_COUNTINGJUMP (1 + SIZE_JUMP)
#define RE_MAXCOUNTER     20

typedef _RegexCounterRegister _RegexCounterTable[RE_MAXCOUNTER];

class _RegexMatchStackElement {
public:
  const BYTE                        *m_ip;
  const TCHAR                       *m_sp;
  BYTE                               m_counterIndex;
  // only set when hasCounter() = true
  UINT                               m_counterValue;
//  const TCHAR                       *m_regEnd;       // only set when hasCounter() = true
  inline _RegexMatchStackElement() {
  }
  inline _RegexMatchStackElement(const BYTE *ip, const TCHAR *sp)
    : m_ip(ip)
    , m_sp(sp)
    , m_counterIndex(-1)
  {
  }
  inline _RegexMatchStackElement(const BYTE *ip, const TCHAR *sp, BYTE counterIndex, UINT counterValue/*, const TCHAR *regEnd*/)
    : m_ip(ip)
    , m_sp(sp)
    , m_counterIndex(counterIndex)
    , m_counterValue(counterValue)
//    , m_regEnd(regEnd)
  {
  }
  inline bool hasCounter() const {
    return m_counterIndex < RE_MAXCOUNTER;
  }
#ifdef _DEBUG
  String toString(const BYTE *codeStart) const;
#endif
};

class Regex;

inline int getJumpAddress(const BYTE *&p) {
  const short result = *(short*)p;
  p += sizeof(result);
  return result;
}

class _RegexMatchState {
private:
  CompactStack<_RegexMatchStackElement> m_stack;
  _RegexStateRegister                   m_register[RE_NREGS];
  _RegexCounterTable                   &m_counterTable;
  const Regex                          &m_regex;
  // Index of first character in targetstring we try to match
  const intptr_t                        m_pos;

  inline void push(const BYTE *ip, const TCHAR *sp) {
    m_stack.push(_RegexMatchStackElement(ip,sp));
  }

  inline void pushZeroes() {
    push(NULL,NULL);
  }

  inline void resetCounter(BYTE counterIndex) {
    m_counterTable[counterIndex].m_value = 0;
  }

  // Returns true if jump should be done
  inline bool incrCounter(BYTE counterIndex) {
    _RegexCounterRegister &counter = m_counterTable[counterIndex];
    return counter.isBelowMax(++counter.m_value);
  }

  inline bool isCounterInRange(BYTE counterIndex) const {
    return m_counterTable[counterIndex].isValueInRange();
  }

  inline void startMemory(BYTE regno, const TCHAR *sp) {
    _RegexStateRegister &reg = m_register[regno];
    reg.m_regStart       = sp;
    reg.m_regStartSegEnd = m_spEnd;
  }

  inline void stopMemory(BYTE regno, const TCHAR *sp) {
    _RegexStateRegister &reg = m_register[regno];
    reg.m_regEnd = sp;
    if(reg.m_regStartSegEnd == m_spEnd) {
      reg.m_regStartSegEnd = sp;
    }
  }

public:
  // Instruction pointer
  const BYTE  *m_ip;
  // End of instructions
  const BYTE  *m_ipEnd;
  // End of first and second String
  const TCHAR *m_end1,      *m_end2;
  const TCHAR *m_endMatch1, *m_endMatch2;
  const TCHAR *m_sp, *m_spEnd;

  _RegexMatchState(const Regex *regex, _RegexCounterTable &counterTable, intptr_t pos);

  inline intptr_t getPos() const {
    return m_pos;
  }

  void doMaybePopAndJump();

  inline void popNoRestore() {
    m_stack.pop();
  }

  inline void popAndRestore() {
    const _RegexMatchStackElement e = m_stack.pop();
    m_ip = e.m_ip;
    m_sp = e.m_sp;
  }

  inline bool isStackEmpty() const {
    return m_stack.isEmpty();
  }

  inline void doOnFailureJump() {
    const int jumpCount = getJumpAddress(m_ip);
    push(m_ip + jumpCount, m_sp);
  }

  inline void doOnFailureJumpPushCounter() {
    const BYTE                   counterIndex = *(m_ip++);
    const int                    jumpCount    = getJumpAddress(m_ip);
    const _RegexCounterRegister &counter      = m_counterTable[counterIndex];
    m_stack.push(_RegexMatchStackElement(m_ip + jumpCount, m_sp, counterIndex, counter.m_value
                                        /*,counter.hasStateRegister()?m_register[counter.m_regno].m_regEnd : NULL*/));
  }

  inline void doPopAndJump() {
    popNoRestore();
    doJump();
  }

  inline void doJump() {
    m_ip += getJumpAddress(m_ip);
  }

  inline void doDummyFailureJump() {
    pushZeroes();
    doJump();
  }

  inline void doPopCountAndJump() {
    popNoRestore();
    doCountAndJump();
  }

  inline void doCountAndJump() {
    if(incrCounter(*m_ip)) {
      m_ip++;
      doJump();
    } else {
      doOnFailureJumpPushCounter(); // !! The jump address will not be used. This will be popped only on a failure later in the pattern,
                                    // and the stackelement just below this, will restore the backtracking with the counter = maxrepeat-1.
    }
  }

  inline void doResetCounter() {
    resetCounter(*(m_ip++));
  }

  inline void doStartMemory() {
    startMemory(*(m_ip++), m_sp);
  }

  inline void doStopMemory() {
    stopMemory(*(m_ip++), m_sp);
  }

  inline bool topHasCounter() const {
    return m_stack.top().hasCounter();
  }

  // Return true if handled. false if another pop is required
  bool handleCounterFailure();

  void convertRegisters(const TCHAR    *string1
                       ,intptr_t        size1
                       ,const TCHAR    *string2
                       ,intptr_t        size2
                       ,RegexRegisters *registers);

  inline const _RegexStateRegister &getRegister(int regno) const {
    return m_register[regno];
  }

  inline const Regex &getRegex() const {
    return m_regex;
  }

#ifdef _DEBUG
  UINT getDBGIpElement() const;
  UINT getDBGLineNumber() const;
  UINT getDBGPatternCharIndex() const;
  String stackToString() const;
  String registersToString() const;
  String countersToString() const;
#endif
};

#ifdef _DEBUG
class _RegexSearchState {
public:
  const Regex   &m_regex;
  const intptr_t m_startPos;
  const intptr_t m_charIndex;
  _RegexSearchState(const Regex *regex, intptr_t startPos, intptr_t charIndex)
    : m_regex(*regex)
    , m_startPos(startPos)
    , m_charIndex(charIndex)
  {
  }
};

class _RegexCompilerState {
public:
  const Regex   &m_regex;
  const String  &m_codeText;
  const String  &m_compilerStack;
  const String  &m_currentState;
  const String  &m_usedCharSets;
  const String  &m_registerInfo;
  const String  &m_fastMap;
  const intptr_t m_scannerIndex;
  const intptr_t m_pendingExact;
  _RegexCompilerState(const Regex  *regex
                     ,const String &codeText
                     ,const String &compilerStack
                     ,const String &currentState
                     ,const String &usedCharSets
                     ,const String &registerInfo
                     ,const String &fastMap
                     ,intptr_t      scannerIndex
                     ,intptr_t      pendingExact
                     )
    : m_regex(       *regex        )
    , m_codeText(     codeText     )
    , m_compilerStack(compilerStack)
    , m_currentState( currentState )
    , m_usedCharSets( usedCharSets )
    , m_registerInfo( registerInfo )
    , m_fastMap(      fastMap      )
    , m_scannerIndex( scannerIndex )
    , m_pendingExact( pendingExact )
  {
  }
};

class RegexStepHandler {
public:
  virtual void handleCompileStep(const _RegexCompilerState &state) {
  }
  virtual void handleSearchStep( const _RegexSearchState   &state) {
  }
  virtual void handleMatchStep(  const _RegexMatchState    &state) {
  }
};
#endif // _DEBUG

class _RegexByteInsertHandler {
public:
  virtual void insertBytes(UINT addr, UINT incr) = 0;
};

// This data structure is used to represent a compiled pattern.
class Regex {
private:
  // Space holding the compiled pattern commands.
  ByteArray                  m_buffer;
  // Number of bytes in code-segment. m_buffer may contain extra bytes holding the used charsets
  UINT                       m_codeSize;
  // search uses the fastmap, to skip quickly over totally implausible characters
  BitSet                     m_fastMap;
  // Counters used in ...{m,n} constructs
  mutable _RegexCounterTable m_counterTable;
  // Number of counters in use.
  UINT                       m_counterTableSize;
  // Translate table to apply to all characters before comparing.
  // Or NULL for no translation.
  // The translation is applied to a pattern when it is compiled
  // and to data when it is matched.
  const TCHAR               *m_translateTable;
  // Set to true in compilePattern, if the compiled pattern match the empty string
  bool                       m_matchEmpty;
  bool                       m_hasCompiled;
  // Length of the text-segment that match the pattern
  mutable intptr_t           m_resultLength;
  // Call this each time some bytes are inserted (not appended) to adjust references to bytes into m_buffer
  _RegexByteInsertHandler   *m_insertHandler;

#ifdef _DEBUG
  RegexStepHandler          *m_stepHandler;
    // Built by compilePattern/toString(). Indexed by byte-index of each command in m_buffer.
    // Highorder 16 bits of each elemenet holds the line into code text returned by toString()
    // Loworder 16 bits holds the index into pattern-string
  mutable CompactIntArray    m_PCToLineArray;
    // Set to true when anything in m_buffer has changed, and false when toString() is called.
  mutable bool               m_codeDirty;
    // Counts the number of cycles from the search/match begin til return
  mutable UINT               m_cycleCount;
#endif

  void compilePattern1(   const TCHAR    *pattern);

  intptr_t  search1(      const TCHAR    *string
                         ,size_t          size
                         ,intptr_t        startPos
                         ,intptr_t        range
                         ,RegexRegisters *registers) const;

  intptr_t  search2(      const TCHAR    *string1
                         ,size_t          size1
                         ,const TCHAR    *string2
                         ,size_t          size2
                         ,intptr_t        startPos
                         ,intptr_t        range
                         ,RegexRegisters *registers
                         ,size_t          mstop) const;

  intptr_t match(         const TCHAR    *string
                         ,size_t          size
                         ,intptr_t        pos
                         ,RegexRegisters *registers) const;

  intptr_t  match2(       const TCHAR    *string1
                         ,size_t          size1
                         ,const TCHAR    *string2
                         ,size_t          size2
                         ,intptr_t        pos
                         ,RegexRegisters *registers
                         ,size_t          mstop) const;
  int  compareStrings(const TCHAR *s1, const TCHAR *s2, register size_t length) const;
  void init();
  // Always call this. instead of m_buffer.insertZeroes
  void insertZeroes(UINT addr,   UINT count);
  void storeData(                        UINT addr, const void *data, UINT size);
  void insertOpcode(      BYTE opcode,   UINT addr                           );
  void insertJump(        BYTE opcode,   UINT addr, int to                   );
  void insertResetCounter(               UINT addr,         BYTE counterIndex);
  void insertCountingJump(BYTE opcode,   UINT addr, int to, BYTE counterIndex);
  void storeOpcode(       BYTE opcode,   UINT addr                           );
  void storeJump(         BYTE opcode,   UINT addr, int to                   );
  void storeResetCounter(                UINT addr,         BYTE counterIndex);
  void storeCountingJump( BYTE opcode,   UINT addr, int to, BYTE counterIndex);
  void storeShort(                       UINT addr, short s                  );
  void appendUShort(                     USHORT           s                  );
  void appendCharacter(TCHAR ch);
  // Extend m_buffer if needed
  void assertHasSpace(UINT addr, UINT count);
public:
  Regex();
  Regex(const String &pattern, const TCHAR *translateTable = NULL);
  Regex(const TCHAR  *pattern, const TCHAR *translateTable = NULL);
  void compilePattern(const String &pattern, const TCHAR *translateTable = NULL);
  void compilePattern(const TCHAR  *pattern, const TCHAR *translateTable = NULL);
  // Search for the compiled expression in text
  intptr_t search(    const String &text, bool forward = true, intptr_t startPos = -1, RegexRegisters *registers = NULL) const;
  // Search for the compiled expression in text
  intptr_t search(    const TCHAR  *text, bool forward = true, intptr_t startPos = -1, RegexRegisters *registers = NULL) const;
  // Check for exact match
  bool     match(     const String &text, RegexRegisters *registers = NULL) const;
  // Check for exact match
  bool     match(     const TCHAR  *text, RegexRegisters *registers = NULL) const;

  inline intptr_t getResultLength() const {
    return m_resultLength;
  }

  const BitSet &getFastMap() const {
    return m_fastMap;
  }

  inline bool getMatchEmpty() const {
    return m_matchEmpty;
  }

  inline bool isCompiled() const {
    return m_hasCompiled;
  }

  const BYTE *getCodeStart() const {
    return m_buffer.getData();
  }

  inline UINT getCodeSize() const {
    return m_codeSize;
  }

  // Return set of characters that can possibly begin a string matching
  // the commands in range [pcStart;pcEnd[. pcStart inclusive, pcEnd exclusive
  BitSet first(intptr_t pcStart, intptr_t pcEnd, bool *matchEmpty = NULL) const;

  // Return help text,syntax-description terminated with NULL-pointer
  static const TCHAR **getHelpText();
#ifdef _DEBUG
  RegexStepHandler *setHandler(RegexStepHandler *handler);

  String toString() const;
  String fastMapToString() const;
  String countersToString() const;
  void dump(FILE *f = stdout, bool includeFastmap=false) const;
  bool isCodeDirty() const {
    return m_codeDirty;
  }
  const CompactIntArray &getDebugInfo() const {
    return m_PCToLineArray;
  }
  BitSet       getPossibleBreakPointLines() const;
  int          getLastCodeLine() const;
  UINT         getCycleCount() const {
    return m_cycleCount;
  }
#endif
};
