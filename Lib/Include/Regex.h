#pragma once

#include "MyUtil.h"
#include "ByteArray.h"
#include "BitSet.h"
#include <Stack.h>
#include "CompactStack.h"

#define RE_NREGS 10

class RegexRegisters {
public:
  int start[RE_NREGS];
  int end[RE_NREGS];
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
  unsigned short m_minRepeat;
  unsigned short m_maxRepeat;
  BYTE           m_regno; // index of associated StateRegister (not counterRegister)
  _RegexCounterRange()
    : m_minRepeat(-1)
    , m_maxRepeat(-1)
  {
  }

  _RegexCounterRange(unsigned short minRepeat, unsigned short maxRepeat, BYTE regno)
    : m_minRepeat(minRepeat)
    , m_maxRepeat(maxRepeat)
    , m_regno(regno)
  {
  }
  inline bool contains(unsigned int n) const {
    return (m_minRepeat <= n) && ((m_maxRepeat==0) || (n <= m_maxRepeat));
  }
  inline bool isGEMin(unsigned int n) const {
    return n >= m_minRepeat;
  }
  inline bool isBelowMax(unsigned int n) const {
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
  unsigned int m_value;
  inline bool isValueInRange() const {
    return isGEMin(m_value);
  }
};

#define SIZE_JUMP         (1 + sizeof(short))
#define SIZE_COUNTINGJUMP (1 + SIZE_JUMP)
#define RE_MAXCOUNTER     20

typedef _RegexCounterRegister _RegexCounterTable[RE_MAXCOUNTER];

class _RegexMatchStackElement {
public:
  const BYTE                        *m_ip;
  const TCHAR                       *m_sp;
  BYTE                               m_counterIndex;
  unsigned int                       m_counterValue; // only set when hasCounter() = true
//  const TCHAR                       *m_regEnd;       // only set when hasCounter() = true
  inline _RegexMatchStackElement() {
  }
  inline _RegexMatchStackElement(const BYTE *ip, const TCHAR *sp)
    : m_ip(ip)
    , m_sp(sp)
    , m_counterIndex(-1)
  {
  }
  inline _RegexMatchStackElement(const BYTE *ip, const TCHAR *sp, BYTE counterIndex, unsigned int counterValue/*, const TCHAR *regEnd*/)
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
  const int                             m_pos;                  // Index of first character in targetstring we try to match

  inline void push(const BYTE *ip, const TCHAR *sp) {
    m_stack.push(_RegexMatchStackElement(ip,sp));
  }

  inline void pushZeroes() {
    push(NULL,NULL);
  }

  inline void resetCounter(BYTE counterIndex) {
    m_counterTable[counterIndex].m_value = 0;
  }

  inline bool incrCounter(BYTE counterIndex) { // Returns true if jump should be done
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
  const BYTE  *m_ip;                    // Instruction pointer
  const BYTE  *m_ipEnd;                 // end of instructions
  const TCHAR *m_end1,      *m_end2;    // End of first and second String
  const TCHAR *m_endMatch1, *m_endMatch2;
  const TCHAR *m_sp, *m_spEnd;

  _RegexMatchState(const Regex *regex, _RegexCounterTable &counterTable, int pos);
  
  inline int getPos() const {
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

  bool handleCounterFailure(); // return true if handled. false if another pop is required

  void convertRegisters(const TCHAR    *string1
                       ,int             size1
                       ,const TCHAR    *string2
                       ,int             size2
                       ,RegexRegisters *registers);

  inline const _RegexStateRegister &getRegister(int regno) const {
    return m_register[regno];
  }

  inline const Regex &getRegex() const {
    return m_regex;
  }

#ifdef _DEBUG
  unsigned int getDBGIpElement() const;
  unsigned int getDBGLineNumber() const;
  unsigned int getDBGPatternCharIndex() const;
  String stackToString() const;
  String registersToString() const;
  String countersToString() const;
#endif
};

#ifdef _DEBUG
class _RegexSearchState {
public:
  const Regex &m_regex;
  const int    m_startPos;
  const int    m_charIndex;
  _RegexSearchState(const Regex *regex, int startPos, int charIndex) 
    : m_regex(*regex)
    , m_startPos(startPos)
    , m_charIndex(charIndex)
  {
  }
};

class _RegexCompilerState {
public:
  const Regex  &m_regex;
  const String &m_codeText;
  const String &m_compilerStack;
  const String &m_currentState;
  const String &m_usedCharSets;
  const String &m_registerInfo;
  const String &m_fastMap;
  const int     m_scannerIndex;
  const int     m_pendingExact;
  _RegexCompilerState(const Regex  *regex 
                     ,const String &codeText
                     ,const String &compilerStack
                     ,const String &currentState
                     ,const String &usedCharSets
                     ,const String &registerInfo
                     ,const String &fastMap
                     ,int           scannerIndex
                     ,int           pendingExact
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
#endif

// This data structure is used to represent a compiled pattern.

class _RegexByteInsertHandler {
public:
  virtual void insertBytes(unsigned int addr, unsigned int incr) = 0;
};

class Regex {
private:
  ByteArray                  m_buffer;           // Space holding the compiled pattern commands.
  unsigned int               m_codeSize;         // Number of bytes in code-segment. m_buffer may contain extra bytes holding the used charsets
  BitSet                     m_fastMap;          // search uses the fastmap, to skip quickly over totally implausible characters
  mutable _RegexCounterTable m_counterTable;     // Counters used in ...{m,n} constructs
  unsigned int               m_counterTableSize; // Number of counters in use.
  const unsigned char       *m_translateTable;   // Translate table to apply to all characters before comparing.
                                                 // Or NULL for no translation.
                                                 // The translation is applied to a pattern when it is compiled
                                                 // and to data when it is matched.
  bool                       m_matchEmpty;       // Set to true in compilePattern, if the compiled pattern match the empty string
  bool                       m_hasCompiled;
  mutable int                m_resultLength;     // Length of tha text-segment that match the pattern
  _RegexByteInsertHandler   *m_insertHandler;    // call this each time bytes are inserted (not appended) to adjust references to bytes into m_buffer

#ifdef _DEBUG
  RegexStepHandler          *m_stepHandler;
  mutable CompactIntArray    m_PCToLineArray;    // Built by compilePattern/toString(). Indexed by byte-index of each command in m_buffer.
                                                 // Highorder 16 bits of each elemenet holds the line into code text returned by toString()
                                                 // Loworder 16 bits holds the index into pattern-string
  mutable bool               m_codeDirty;        // Set to true when anything in m_buffer has changed, and false when toString() is called.
  mutable unsigned int       m_cycleCount;       // Counts the number of cycles from the search/match begin til return
#endif

  void compilePattern1(   const TCHAR    *pattern);

  int  search1(           const TCHAR    *string
                         ,int             size
                         ,int             startPos
                         ,int             range
                         ,RegexRegisters *registers) const;

  int  search2(           const TCHAR    *string1
                         ,int             size1
                         ,const TCHAR    *string2
                         ,int             size2
                         ,int             startPos
                         ,int             range
                         ,RegexRegisters *registers
                         ,int             mstop) const;

  int  match(             const TCHAR    *string
                         ,int             size
                         ,int             pos
                         ,RegexRegisters *registers) const;

  int  match2(            const TCHAR    *string1
                         ,int             size1
                         ,const TCHAR    *string2
                         ,int             size2
                         ,int             pos
                         ,RegexRegisters *registers
                         ,int             mstop) const;
  int  compareStrings(const TCHAR *s1, const TCHAR *s2, register int length) const;
  void init();
  void insertZeroes(unsigned int addr,   unsigned int count); // Always call this. instead of m_buffer.insertZeroes
  void storeData(                        unsigned int addr, const void *data, unsigned int size);
  void insertOpcode(      BYTE opcode,   unsigned int addr                           );
  void insertJump(        BYTE opcode,   unsigned int addr, int to                   );
  void insertResetCounter(               unsigned int addr,         BYTE counterIndex);
  void insertCountingJump(BYTE opcode,   unsigned int addr, int to, BYTE counterIndex);
  void storeOpcode(       BYTE opcode,   unsigned int addr                           );
  void storeJump(         BYTE opcode,   unsigned int addr, int to                   );
  void storeResetCounter(                unsigned int addr,         BYTE counterIndex);
  void storeCountingJump( BYTE opcode,   unsigned int addr, int to, BYTE counterIndex);
  void storeShort(                       unsigned int addr, short s                  );
  void appendUShort(                     unsigned short s                             );
  void appendCharacter(TCHAR ch);
  void assertHasSpace(unsigned int addr, unsigned int count); // extend m_buffer if needed
public:
  Regex();
  Regex(const String &pattern, const unsigned char *translateTable = NULL);
  Regex(const TCHAR  *pattern, const unsigned char *translateTable = NULL);
  void compilePattern(const String &pattern, const unsigned char *translateTable = NULL);
  void compilePattern(const TCHAR  *pattern, const unsigned char *translateTable = NULL);
  int  search(        const String &text, bool forward = true, int startPos = -1, RegexRegisters *registers = NULL) const; // search for the compiled expression in text
  int  search(        const TCHAR  *text, bool forward = true, int startPos = -1, RegexRegisters *registers = NULL) const;
  bool match(         const String &text, RegexRegisters *registers = NULL) const;  // check for exact match
  bool match(         const TCHAR  *text, RegexRegisters *registers = NULL) const;

  inline int getResultLength() const {
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

  inline unsigned int getCodeSize() const {
    return m_codeSize;
  }

  BitSet first(int pcStart, int pcEnd, bool *matchEmpty = NULL) const;
  // Return set of characters that can possibly begin a string matching 
  // the commands in range [pcStart;pcEnd[. pcStart inclusive, pcEnd exclusive

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
  unsigned int getCycleCount() const {
    return m_cycleCount;
  }
#endif
};

extern TCHAR *regexDescription[]; // help text,syntax-description terminated with NULL-pointer
