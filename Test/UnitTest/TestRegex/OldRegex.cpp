#include "stdafx.h"
#include <MyUtil.h>
#include <String.h>
#include <Stack.h>
#include "OldRegex.h"

class FastStack {
private:
  int m_top;
  int m_bufSize;
  const char **m_buffer;
public:
  FastStack();
  ~FastStack();
  const char *pop()   { return m_buffer[--m_top]; }
  void push(const char *p);
  bool isEmpty() const  {
    return m_top == 0;
  };
  void clear() {
    m_top = 0;
  }
};

void FastStack::push(const char *p) {
  if(m_top == m_bufSize) {
    m_bufSize *= 2;
    m_buffer = REALLOC(m_buffer, const char*, m_bufSize);
  }
  m_buffer[m_top++] = p;
}

FastStack::FastStack() {
  m_top     = 0;
  m_bufSize = 10;
  m_buffer  = MALLOC(const char*, m_bufSize);
}

FastStack::~FastStack() {
  FREE(m_buffer);
}

#define bzero(d,n) memset(d,0,n)

static bool wordLetterTable[256];

class OldWordLetterInitializer {
public:
  OldWordLetterInitializer();
};

OldWordLetterInitializer::OldWordLetterInitializer() {
  for(int c = 0; c < ARRAYSIZE(wordLetterTable); c++) {
    wordLetterTable[c] = false;
  }

  for(c = 'a'; c <= 'z'; c++) {
    wordLetterTable[c] = true;
  }

  for(c = 'A'; c <= 'Z'; c++) {
    wordLetterTable[c] = true;
  }

  for(c = '0'; c <= '9'; c++) {
    wordLetterTable[c] = true;
  }
}

static OldWordLetterInitializer dummyInitializer;
#define isWordLetter(c) wordLetterTable[(unsigned char)(c)]

int OldRegex::compareBytes(const char *s1, const char *s2, register int length) const {
  if(m_translateTable == NULL) {
    return memcmp(s1, s2, length);
  }

  register unsigned char *p1 = (unsigned char*)s1;
  register unsigned char *p2 = (unsigned char*)s2;
  while(length--) {
    if(m_translateTable[*p1++] != m_translateTable[*p2++]) {
      return 1;
    }
  }
  return 0;
}

static unsigned short getUShort(const char *&p) {
  unsigned short amount;
  memcpy(&amount,p,sizeof(amount));
  p += sizeof(amount);
  return amount;
}

static int getJumpAddress(const char *&p) {
  short amount;
  memcpy(&amount,p,sizeof(amount));
  p += sizeof(amount);
  return amount;
}

static char *storeShort(char *dst, short s) {
  memcpy(dst,&s,sizeof(short));
  return dst + 2;
}

// Store where from points a jump operation to jump to where to points.
// opcode is the opcode to store.
static void storeJump(char opcode, char *dst, char *to) {
  dst[0] = opcode;
  storeShort(dst+1,(short)(to-(dst + 3)));
}

// Open up space at char from, and insert there a jump to to.
// currentEnd gives the end of storage so we know how many bytes to move
// opcode is the jump-instruction to insert.
// If you call this function, you must set pendingExact = 0.
static void insertJump(char opcode, char *dst, char *to, char *currentEnd) {
  memmove(dst+3,dst,currentEnd - dst);
  storeJump(opcode, dst, to);
}

#define BITSPERBYTE 8
#define MAXBITSETSIZE ((1<<BITSPERBYTE)/BITSPERBYTE)
#define FASTMAPSIZE ARRAYSIZE(m_fastMap)

#define ADDBIT(set,i)       set[(i)/BITSPERBYTE] |= (1<<((i)%BITSPERBYTE))
#define CONTAINSBIT(set,i) (set[(i)/BITSPERBYTE] &  (1<<((i)%BITSPERBYTE)))

#define TRANSLATE(ch)      (m_translateTable?m_translateTable[(unsigned char)(ch)]:(unsigned char)(ch))

// These are the command codes that appear in compiled regular expressions, one per byte.
// Some command codes are followed by argument bytes.
// A command code can specify any interpretation whatever for its arguments.
// Zero-bytes may appear in the compiled regular expression.

typedef enum {
    unused 
   ,exactn               // Followed by one byte giving n, and then by n literal bytes       
   ,begline              // Fails unless at beginning of line
   ,endline              // Fails unless at end of line
                         //
   ,jump                 // Followed by two bytes giving relative address to jump to
   ,onFailureJump        // Followed by two bytes giving relative address of place
                         // to resume at in case of failure.
                         //
   ,repeat               // Followed by 2x2bytes giving minimum and maximum repeatitions
                         //
   ,finalizeJump         // Throw away latest failure point and then jump to address. 
                         //
   ,maybeFinalizeJump    // Like jump but finalize if safe to do so.
                         // This is used to jump back to the beginning
                         // of a repeat. If the command that follows
                         // this jump is clearly incompatible with the
                         // one at the beginning of the repeat, such that
                         // we can be sure that there is no use backtracking
                         // out of repetitions already completed,
                         // then we finalize.
                         //
   ,dummyFailureJump     // Jump, and push a dummy failure point.
                         // This failure point will be thrown away
                         // if an attempt is made to use it for a failure.
                         // A + construct makes this before the first repeat.
                         //
   ,anychar              // Matches any character 
   ,charset              // Matches any char belonging to specified set.
                         // First following byte is number of bytes in set
                         // Then come bytes for a bit-map saying which chars are in.
                         // Bits in each byte are ordered low-bit-first.
                         // A character is in the set if its bit is 1.
                         // A character too large to have a bit in the map
                         // is automatically not in the set
                         //
   ,charsetNot           // Similar but match any character 
                         // that is NOT one of those specified
                         //
   ,startMemory          // Starts remembering the text that is matched
                         // and stores it in a memory register.
                         // Followed by one byte containing the register number.
                         // Register numbers must be in the range 0 through RE_NREGS.
                         //
   ,stopMemory           // Stops remembering the text that is matched
                         // and stores it in a memory register.
                         // Followed by one byte containing the register number.
                         // Register numbers must be in the range 0 through RE_NREGS.
                         //
   ,duplicate            // Match a duplicate of something remembered.
                         // Followed by one byte containing the index of
                         // the memory register.
                         //
   ,beforeDot            // Succeeds if before dot
   ,atDot                // Succeeds if at dot
   ,afterDot             // Succeeds if after dot
   ,begbuf               // Succeeds if at beginning of buffer
   ,endbuf               // Succeeds if at end of buffer
   ,wordchar             // Matches any word-constituent character
   ,notWordchar          // Matches any char that is not a word-constituent
   ,wordbeg              // Succeeds if at word beginning
   ,wordend              // Succeeds if at word end
   ,wordBound            // Succeeds if at a word boundary
   ,notWordBound         // Succeeds if not at a word boundary
} RegexPCode;

#define MAXBUFSIZE 0x10000

#define EXTEND_BUFFER                                         \
  { char *oldBuffer = m_buffer;                               \
    if(m_allocated == MAXBUFSIZE) {                           \
      throwException("Regular expression too big");           \
    }                                                         \
    m_allocated *= 2;                                         \
    if(m_allocated > MAXBUFSIZE) m_allocated = MAXBUFSIZE;    \
    if((m_buffer = REALLOC(m_buffer,char,m_allocated))==NULL) \
      throwException("Memory exhausted");                     \
    int change = m_buffer - oldBuffer;                        \
    pc += change;                                             \
    if(fixupJump)    fixupJump    += change;                  \
    if(lastStart)    lastStart    += change;                  \
    if(pendingExact) pendingExact += change;                  \
    begalt += change;                                         \
  }

class OldRegexScanner {
private:
  const unsigned char *m_source,*m_current,*m_end;
  const unsigned char *m_translateTable;
public:
  OldRegexScanner(const char *source, const unsigned char *translateTable);
  unsigned char fetchRaw();
  unsigned char fetch();
  void          unFetch() { m_current--; }
  unsigned char look(int lookahead = 0) const;
  bool eos() const { return m_current == m_end; }
  int getIndex() const { return m_current - m_source; }
  unsigned short fetchShort();
};

OldRegexScanner::OldRegexScanner(const char *source, const unsigned char *translateTable) {
  m_source         = (const unsigned char*)source;
  m_current        = m_source;
  m_end            = m_source + strlen(source);
  m_translateTable = translateTable;
}

unsigned char OldRegexScanner::look(int lookahead) const {
  const unsigned char *result = m_current + lookahead;
  if(result > m_end) {
    throwException("Unexpected end of regular expression");
  }
  return *result;

}

unsigned char OldRegexScanner::fetchRaw() {
  if(eos()) {
    throwException("Unexpected end of regular expression");
  }
  return *m_current++;
}

unsigned char OldRegexScanner::fetch() {
  unsigned char c = fetchRaw();
  return TRANSLATE(c);
}

unsigned short OldRegexScanner::fetchShort() {
  if(eos()) {
    throwException("Unexpected end of regular expression");
  }
  char ch = look();
  if(!isdigit(ch)) {
    throwException("Expected number");
  }
  unsigned int result = 0;
  do {
    result = result * 10 + (ch - '0');
    fetch();
    ch = look();
    if(result > 0xffff) {
      throwException("Number too big");
    }
  } while(isdigit(ch));
  return (unsigned short)result;
}

#define appendByte(ch) (*pc++ = (char) (ch))

// compilePattern1 takes a string containing a regular-expression in the user's format
// and converts it into a buffer full of byte commands for matching.
//
void OldRegex::compilePattern1(const char *pattern) {
  OldRegexScanner scanner(pattern,m_translateTable);
  Stack<int>   stack;

  char *pc           = m_buffer;
  char *begalt       = pc;
  char *pendingExact = NULL;
  char *fixupJump    = NULL;
  char *lastStart    = NULL;
  bool  zeroTimesOk;
  bool  manyTimesOk;
  unsigned int regnum = 1;

  while(!scanner.eos()) {
    if(pc - m_buffer > m_allocated - 20) {
      EXTEND_BUFFER;
    }

    unsigned char c = scanner.fetch();

    switch(c) {
    case '$': // $ means succeed if at end of line, but only in special contexts.
              // If in the middle of a pattern, it is a normal character.
      if(scanner.eos() || (scanner.look() == '\\' && (scanner.look(1) == ')' || scanner.look(1) == '|'))) {
        appendByte(endline);
        break;
      }
      goto normal_char;

    case '^':
      if(lastStart) { // ^ means succeed if at start of line, but only if no preceding pattern.
        goto normal_char;
      }
      appendByte(begline);
      break;

    case '*':
    case '+':
    case '?':
       
      if(!lastStart) // If there is no previous pattern, char not special.
        goto normal_char;
       // If there is a sequence of repetition chars,
       // collapse it down to equivalent to just one.
      zeroTimesOk = false;
      manyTimesOk = false;
      for(;;) {
        zeroTimesOk |= (c != '+');
        manyTimesOk |= (c != '?');
        if(scanner.eos()) {
          break;
        }
        c = scanner.fetch();
        if(!(c == '*' || c == '+' || c == '?')) {
          scanner.unFetch();
          break;
        }
      }

      if(manyTimesOk) {
        storeJump(maybeFinalizeJump, pc, lastStart - 3);
        pc += 3;
      }
      insertJump(onFailureJump, lastStart, pc + 3, pc);
      pendingExact = 0;
      pc += 3;
      if(!zeroTimesOk) { // At least one repetition required: insert before the loop
                         // a skip over the initial on-failure-jump instruction
        insertJump(dummyFailureJump, lastStart, lastStart + 6, pc);
        pc += 3;
      }
      break;

    case '{':
      { if(!lastStart) {
          goto normal_char;
        }
        unsigned short minRepeat = scanner.fetchShort();
        unsigned short maxRepeat = minRepeat;
        if(scanner.look() == ',') {
          scanner.fetch();
          maxRepeat = 0;
          if(isdigit(scanner.look())) {
            maxRepeat = scanner.fetchShort();
            if(maxRepeat < minRepeat) {
              throwException("Max repeatcount must be >= min repeatcount");
            }
          }
        }
        if(scanner.look() != '}') {
          throwException("Expected '}'");
        } else {
          scanner.fetch();
        }
        appendByte(repeat);
        pc = storeShort(pc,minRepeat);
        pc = storeShort(pc,maxRepeat);
      }
      break;
    case '.':
      lastStart = pc;
      appendByte(anychar);
      break;

    case '[':
      { if(pc - m_buffer > m_allocated-3-MAXBITSETSIZE) {
          EXTEND_BUFFER;
        }

        lastStart = pc;
        if(scanner.look() == '^') {
          appendByte(charsetNot);
          scanner.fetch();
        }
        else {
          appendByte(charset);
        }
        appendByte(MAXBITSETSIZE);
        bzero(pc, MAXBITSETSIZE); // Clear the whole map
          
        int startIndex = scanner.getIndex();
        for(;;) { // Read in characters and ranges, setting map bits
          c = scanner.fetch();
          if(c == ']' && scanner.getIndex() != startIndex + 1) {
            break;
          }
          if(scanner.look() == '-') {
            scanner.fetch();
            unsigned char c1 = scanner.fetch();
            for(;c <= c1;c++) {
              ADDBIT(pc,c);
            }
          }
          else {
            ADDBIT(pc,c);
          }
        }
          // Discard any bitmap bytes that are all 0 at the end of the map.
          // Decrement the map-length byte too.

        char &bitsetSize = pc[-1];
        while(bitsetSize > 0 && pc[bitsetSize-1] == 0) {
          bitsetSize--;
        }
        pc += bitsetSize;
      }
      break;

    case '\\':
      if(scanner.eos()) {
        throwException("Unexpected end of regular expression");
      }
      c = scanner.fetchRaw();
      switch(c) {
      case '(':
        if(regnum < RE_NREGS) {
          appendByte(startMemory);
          appendByte(regnum);
        }
        stack.push(pc - m_buffer);
        stack.push(fixupJump ? fixupJump - m_buffer + 1 : 0);
        stack.push(regnum++);
        stack.push(begalt - m_buffer);
        fixupJump = 0;
        lastStart = 0;
        begalt = pc;
        break;

      case ')':
        { if(stack.isEmpty()) {
            throwException("Unmatched \\)");
          }
          begalt = stack.pop() + m_buffer;
          if(fixupJump) {
            storeJump(jump, fixupJump, pc);
          }
          int regnum = stack.pop();
          if(regnum < RE_NREGS) {
            appendByte(stopMemory);
            appendByte(regnum);
          }
          int index = stack.pop();
          fixupJump = index ? m_buffer - 1 + index : 0;
          lastStart = stack.pop() + m_buffer;
        }
        break;

      case '|':
        insertJump(onFailureJump, begalt, pc + 6, pc);
        pendingExact = 0;
        pc += 3;
        if(fixupJump) {
          storeJump(jump, fixupJump, pc);
        }
        fixupJump = pc;
        pc += 3;
        lastStart = 0;
        begalt = pc;
        break;

      case 'w':
        lastStart = pc;
        appendByte(wordchar);
        break;

      case 'W':
        lastStart = pc;
        appendByte(notWordchar);
        break;

      case '<':
        appendByte(wordbeg);
        break;

      case '>':
        appendByte(wordend);
        break;

      case 'b':
        appendByte(wordBound);
        break;

      case 'B':
        appendByte(notWordBound);
        break;

      case '`':
        appendByte(begbuf);
        break;

      case '\'':
        appendByte(endbuf);
        break;

      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        { int c1 = c - '0';
          if(c1 >= regnum) {
            goto normal_char;
          }
          for(int h = 1; h < stack.getHeight(); h+=4) {
            if(stack.top(h) == c1) {
              goto normal_char;
            }
          }
          lastStart = pc;
          appendByte(duplicate);
          appendByte(c1);
        }
        break;

      default:
        goto normal_char;
      } /* end switch */
      break;

    default:
    normal_char:
      { unsigned char c1 = scanner.look();
        if(!pendingExact || pendingExact + *pendingExact + 1 != pc
        || *pendingExact == 0177 
        || c1 == '*' || c1 == '^' || c1 == '+' || c1 == '?') {
          lastStart = pc;
          appendByte(exactn);
          pendingExact = pc;
          appendByte(0);
        }
        appendByte(c);
        (*pendingExact)++;
      }
    } // end switch
  } // end while

  if(fixupJump) {
    storeJump(jump, fixupJump, pc);
  }

  if(!stack.isEmpty()) {
    throwException("Unmatched \\(");
  }

  m_used = pc - m_buffer;

  createFastMap();

  m_hasCompiled = true;
}

// Given a compiled pattern in m_buffer, compute a fastmap from it.
// The fastmap records which of the FASTMAPSIZE possible characters
// can start a string that matches the pattern.
// This is used by search to skip quickly over totally implausible text.

void OldRegex::createFastMap() {
  register char *p = m_buffer;
  register char *pend = m_buffer + m_used;

  Stack<char*> stack;

  for(int j = 0; j < FASTMAPSIZE; j++) {
    m_fastMap[j] = false;
  }

  m_canBeNull = false;

  while(p) {
    if(p == pend) {
      m_canBeNull = true;
      break;
    }
    RegexPCode opcode;
    switch(opcode = (RegexPCode)*p++) {
    case exactn:
      m_fastMap[TRANSLATE(p[1])] = true;
      break;

    case begline     :
    case beforeDot   :
    case atDot       : 
    case afterDot    :
    case begbuf      :
    case endbuf      :
    case wordBound   :
    case notWordBound:
    case wordbeg     :
    case wordend     :
      continue;

    case endline     :
      m_fastMap[TRANSLATE('\n')] = true;
      m_canBeNull = true;
      break;

    case finalizeJump      :
    case maybeFinalizeJump :
    case jump              :
    case dummyFailureJump  :
      { m_canBeNull = true;
        int count = getJumpAddress(p);
        p += count;
        if(count > 0) {
          continue;
        }

      // Jump backward reached implies we just went through
      // the body of a loop and matched nothing.
      // Opcode jumped to should be an onFailureJump.
      // Just treat it like an ordinary jump.
      // For a * loop, it has pushed its failure point already;
      // if so, discard that as redundant.

        if(*p != (char)onFailureJump) {
          continue;
        }
        p++;
        count = getJumpAddress(p);
        p += count;
        if(!stack.isEmpty() && stack.top() == p) {
          stack.pop();
        }
        continue;
      }

    case onFailureJump:
      stack.push(p + getJumpAddress(p));
      continue;

    case startMemory:
    case stopMemory :
      p++;
      continue;

    case duplicate:
      m_canBeNull = true;
      
    case anychar:
      { for(int j = 0; j < FASTMAPSIZE; j++) {
          if(TRANSLATE(j) != '\n') {
            m_fastMap[j] = true;
          }
        }
      }
      return;

    case wordchar:
      { for(int j = 0; j < FASTMAPSIZE; j++) {
          if(isWordLetter(j)) {
            m_fastMap[j] = true;
          }
        }
      }
      break;

    case notWordchar:
      { for(int j = 0; j < FASTMAPSIZE; j++) {
          if(!isWordLetter(j)) {
            m_fastMap[j] = true;
          }
        }
      }
      break;
  
    case charset:
      { int setSize = *(p++) * BITSPERBYTE;
        for(int j = 0; j < setSize; j++) {
          if(CONTAINSBIT(p,j)) {
            m_fastMap[TRANSLATE(j)] = true;
          }
        }
      }
      break;

    case charsetNot: // Chars beyond end of map must be allowed
      { int setSize = *(p++) * BITSPERBYTE;
        for(int j = setSize; j < FASTMAPSIZE; j++) {
          m_fastMap[TRANSLATE(j)] = true;
        }

        for(j = setSize - 1; j >= 0; j--) {
          if(!CONTAINSBIT(p,j)) {
            m_fastMap[TRANSLATE(j)] = true;
          }
        }
      }
      break;

    default:
      throwException("Invalid opcode:%d",opcode);
      break;

    } // end switch
  
// Get here means we have successfully found the possible starting characters
// of one path of the pattern. We need not follow this path any farther.
// Instead, look at the next alternative remembered in the stack.

    if(stack.isEmpty()) {
      return;
    }

    p = stack.pop();
  } // end while
}
  
  
// Like search2, below, but only one String is specified.

int OldRegex::search1(const char     *string,
                   int             size, 
                   int             startPos,
                   int             range,
                   OldRegexRegisters *registers) const {

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

int OldRegex::search2(const char     *string1,
                   int             size1, 
                   const char     *string2,
                   int             size2,
                   int             startPos,
                   register int    range, 
                   OldRegexRegisters *registers,
                   int             mstop) const {
  int total = size1 + size2;

  for(;;) {
    // skip quickly over characters that cannot possibly be the start of a match.
    // Note, however, that if the pattern can possibly match
    // the null String, we must test it at each starting point
    // so that we take the first null String we get.

    if(startPos < total && !m_canBeNull) {
      if(range > 0) {
        register int lim = 0;
        int irange = range;
        if(startPos < size1 && startPos + range >= size1) {
          lim = range - (size1 - startPos);
        }

        const char *p = &(startPos >= size1 ? string2 - size1 : string1)[startPos];

        while(range > lim && !m_fastMap[TRANSLATE(*p++)]) {
          range--;
        }
        startPos += irange - range;
      } else {
        register char c;
        if(startPos >= size1) {
          c = string2[startPos - size1];
        } else {
          c = string1[startPos];
        }
        if(!m_fastMap[TRANSLATE(c)]) {
          goto advance;
        }
      }
    } // end if

    if(range >= 0 && startPos == total && !m_canBeNull) {
      return -1;
    }

    if(0 <= (m_resultLength = match2(string1, size1, string2, size2, startPos, registers, mstop))) {
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

int OldRegex::match(const char     *string,
                 int             size,
                 int             pos,
                 OldRegexRegisters *registers) const {
  m_resultLength = match2(0, 0, string, size, pos, registers, size);
  return m_resultLength;
}

int OldRegex::match2(const char     *string1,
                  int             size1,
                  const char     *string2,
                  int             size2,
                  int             pos,
                  OldRegexRegisters *registers,
                  int             mstop) const {

  register char *p    = m_buffer;
  register char *pend = p + m_used;
  const char *end1;           // End of first String
  const char *end2;           // End of second String
                              // Pointer just past last char to consider matching
  const char *endMatch1, *endMatch2;
  const register char *d, *dend;
  FastStack pointerStack;
  const char *regStart[RE_NREGS];
  const char *regStartSegEnd[RE_NREGS];
  const char *regEnd[RE_NREGS];

  if(size2 == 0) {
    string2 = string1;
    size2   = size1;
    string1 = NULL;
    size1   = 0;
  }
  end1 = string1 + size1;
  end2 = string2 + size2;

  if(mstop <= size1) { // Compute where to stop matching, within the two strings
    endMatch1 = string1 + mstop;
    endMatch2 = string2;
  } else {
    endMatch1 = end1;
    endMatch2 = string2 + mstop - size1;
  }

  for(int i = 0; i < RE_NREGS; i++) { // Initialize \( and \) text positions to -1 to indicate, that no \( or \) has been seen
    regStart[i] = (char*)-1;
  }

// p scans through the pattern as d scans through the data.
// dend is the end of the input string that d scans
// d is advanced into the following input string whenever necessary,
// but this happens before fetching; therefore, at the beginning of the loop,
// d can be pointing at the end of a String, but it cannot equal string2.

  if(pos <= size1) {
    d = string1 + pos;
    dend = endMatch1;
  } else {
    d = string2 + pos - size1;
    dend = endMatch2;
  }

// Use PREFETCH just before fetching a character with *d.
#define PREFETCH        \
while(d == dend) {      \
  if(dend == endMatch2) \
    goto fail;          \
  d = string2;          \
  dend = endMatch2;     \
}

                    // This loop loops over pattern commands.
                    // It exits by returning from the function if match is complete,
                    // or it drops through if match fails at this starting point in the input data

  for(;;) {
    if(p == pend) { // End of pattern means we have succeeded!
                    // If caller wants register contents data back, convert it to indices
      if(registers) {
        bzero(registers, sizeof(*registers));

        regEnd[0] = d;
        regStart[0] = string1;
        for(int i = 0; i < RE_NREGS; i++) {
          if(i && regStart[i] == (char*)-1) {
            continue;
          }
          if(regStart[i] - string1 < 0 || regStart[i] - string1 > size1) {
            registers->start[i] = regStart[i] - string2 + size1;
          } else {
            registers->start[i] = regStart[i] - string1;
          }
          if(regEnd[i] - string1 < 0 || regEnd[i] - string1 > size1) {
            registers->end[i] = regEnd[i] - string2 + size1;
          } else {
            registers->end[i] = regEnd[i] - string1;
          }
        }
        registers->start[0] = pos;
      }

      if(d - string1 >= 0 && d - string1 <= size1) {
        return d - string1 - pos;
      } else {
        return d - string2 + size1 - pos;
      }
    } // end if

// Otherwise match next pattern command 

    RegexPCode opcode;
    switch(opcode = (RegexPCode)*p++) {

// \( is represented by a startMemory, \) by a stopMemory.
// Both of those commands contain a "register number" argument.
// The text matched within the \( and \) is recorded under that number.
// Then, \<digit> turns into a `duplicate' command which
// is followed by the numeric value of <digit> as the register number.

    case startMemory:
      { int regno = *p++;
        regStart[regno] = d;
        regStartSegEnd[regno] = dend;
      }
      break;

    case stopMemory:
      { int regno = *p++;
        regEnd[regno] = d;
        if(regStartSegEnd[regno] == dend) {
          regStartSegEnd[regno] = d;
        }
      }
      break;

    case duplicate:
      { int regno = *p++;           // Get which register to match against
        const char *d2    = regStart[regno];
        const char *dend2 = regStartSegEnd[regno];
        for(;;) {                   // Advance to next segment in register contents, if necessary
          while(d2 == dend2) {
            if(dend2 == endMatch2) {
              break;
            }
            if(dend2 == regEnd[regno]) {
              break;
            }
            d2    = string2;
            dend2 = regEnd[regno];  // end of string1 => advance to string2.
          }
           
          if(d2 == dend2) {         // At end of register contents => success
            break;
          }
           
          PREFETCH;                 // Advance to next segment in data being matched, if necessary
           
          int count = dend - d;     // count gets # consecutive chars to compare
          if(count > dend2 - d2) {
            count = dend2 - d2;
          }
                                    // Compare that many; failure if mismatch, else skip them.
          if(compareBytes(d, d2, count)) {
            goto fail;
          }
          d  += count;
          d2 += count;
        }
      }
      break;

    case anychar:
      PREFETCH;                     // fetch a data character
      if(TRANSLATE(*d++) == '\n') { // Match anything but a newline.
        goto fail;
      }
      break;

    case charset:
    case charsetNot:
    { bool ok = (opcode == charsetNot);
      int bytesInSet = *(p++);
      int setSize = bytesInSet * BITSPERBYTE;
            
      PREFETCH;                     // fetch a data character

      int c = TRANSLATE(*d);

      if(c < setSize && CONTAINSBIT(p,c))
        ok = !ok;

      p += bytesInSet;

      if(!ok) {
        goto fail;
      }
      d++;
    }
    break;

    case begline:
      if(d == string1 || d[-1] == '\n') {
        break;
      }
      goto fail;

    case endline:
      if(d == end2 || (d == end1 ? (size2 == 0 || *string2 == '\n') : *d == '\n')) {
        break;
      }
      goto fail;

// "or" constructs ("|") are handled by starting each alternative
// with an onFailureJump that points to the start of the next alternative.
// Each alternative except the last ends with a jump to the joining point.
// (Actually, each jump except for the last one really jumps
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
      { int count = getJumpAddress(p);
        pointerStack.push( p + count);
        pointerStack.push( d        );
      }
      break;
                            
    case maybeFinalizeJump: // The end of a smart repeat has an maybeFinalizeJump back.
                            // Change it either to a finalizeJump or an ordinary jump. 
      { char &opcodeRef = p[-1];
        int count = getJumpAddress(p);

                            // Compare what follows with the begining of the repeat.
                            // If we can establish that there is nothing that they would
                            // both match, we can change to finalizeJump

        if(p == pend) {
          opcodeRef = (char)finalizeJump;
        } else {
          if(*p == (char)exactn || *p == (char)endline) {
            register int c = *p == (char)endline ? '\n' : p[2];
            register char *p1 = p + count;
                            // p1[0] ... p1[2] are an onFailureJump. Examine what follows that
            char *p3 = p1+3;
            if(*p3 == (char)exactn && p3[2] != c) {
              opcodeRef = (char)finalizeJump;
            } else {
              if(*p3 == (char)charset || *p3 == (char)charsetNot) {
                bool ok = *(p3++) == (char)charsetNot;
                int setSize = *(p3++) * BITSPERBYTE;
                if(c < setSize && CONTAINSBIT(p3,c)) {
                  ok = !ok;
                }
                  
                if(!ok) {    // if ok, c would match. That means it is not safe to finalize
                  opcodeRef = (char)finalizeJump;
                }
              }
            }
          }
        }
        p -= 2;
        if(opcodeRef != (char)finalizeJump) {
          opcodeRef = (char)jump;
          goto nofinalize;
        }
                            // The end of a stupid repeat has a finalize-jump
                            // back to the start, where another failure point will be made
                            // which will point after all the repetitions found so far.
      }

    case finalizeJump:
      pointerStack.pop();
      pointerStack.pop();

    case jump:
         nofinalize:
      { int count = getJumpAddress(p);
        p += count;
      }
      break;

    case dummyFailureJump:
      pointerStack.push(0);
      pointerStack.push(0);
      goto nofinalize;

    case wordBound:
      if(d == string1 || d == end2 || (d == end1 && size2 == 0)) {
        break;
      }
      if(isWordLetter(d[-1]) != isWordLetter(d == end1 ? *string2 : *d)) {
        break;
      }
      goto fail;

    case notWordBound:
      if(d == string1 || d == end2 || (d == end1 && size2 == 0)) {
        goto fail;
      }
      if(isWordLetter(d[-1]) != isWordLetter(d == end1 ? *string2 : *d)) {
        goto fail;
      }
      break;

    case wordbeg:
      if(d == end2 || (d == end1 && size2 == 0) || !isWordLetter(d == end1 ? *string2 : *d)) { // Next char not a letter
        goto fail;
      }
      if(d == string1 || !isWordLetter(d[-1])) {
        break;
      }
      goto fail;

    case wordend:
      if(d == string1 || !isWordLetter(d[-1])) {      // Prev char not letter
        goto fail;
      }
      if(d == end2                                    // Points to end
       || (d == end1 && size2 == 0)                   // Points to end
       || !isWordLetter(d == end1 ? *string2 : *d)) { // Next char not a letter
        break;
      }
      goto fail;

    case wordchar:
      PREFETCH;
      if(!isWordLetter(*d++)) {
        goto fail;
      }
      break;

    case notWordchar:
      PREFETCH;
      if(isWordLetter(*d++)) {
        goto fail;
      }
      break;

    case begbuf:
      if(d == string1) {   // Note, d cannot equal string2, unless string1 == string2.
        break;
      }
      goto fail;

    case endbuf:
      if(d == end2 || (d == end1 && size2 == 0)) {
        break;
      }
      goto fail;

    case exactn:           // Match the next few pattern characters exactly.
      { int count = *p++;  // Count is how many characters to match.
        for(int i = 0; i < count; i++) {
          PREFETCH;
          if(TRANSLATE(*d++) != (unsigned char)*p++) {
            goto fail;
          }
        }
        break;
      }
    default:
      throwException("Invalid opcode:%d",opcode);
      break;
    } // end switch
                            
    continue; // Successfully matched one pattern command; keep matching

                            
    fail:                            // Jump here if any matching operation fails.
      if(pointerStack.isEmpty()) {     // A restart point is known.
        break;                       // Matching at this starting point really fails!
      } else {                       // Restart there and pop it.
        d = pointerStack.pop();
        p = (char*)pointerStack.pop();

        if(!p) goto fail;             // If innermost failure point is dormant, 
                                      // flush it and keep looking
        if(d >= string1 && d <= end1) {
          dend = endMatch1;
        }
      }

  } // end for
  return -1; // Failure to match
}

void OldRegex::init(const OldRegex *src) {
  static const char *errmsg = "out of memory";
  bzero( this, sizeof(OldRegex) );  
  if(src == NULL) {
    m_allocated = 200;
    if((m_buffer = MALLOC(char, m_allocated)) == NULL) {
      throwException(errmsg);
    }
    m_hasCompiled = false;
  } else {
    m_allocated = src->m_allocated;
    if((m_buffer = MALLOC(char,m_allocated)) == NULL) {
      throwException(errmsg);
    }
    memcpy(m_buffer,src->m_buffer,m_allocated);
    m_used             = src->m_used;        
    memcpy(m_fastMap,src->m_fastMap,sizeof(m_fastMap));
    m_translateTable   = src->m_translateTable;
    m_canBeNull        = src->m_canBeNull;
    m_hasCompiled      = src->m_hasCompiled;
  }
}

OldRegex::OldRegex() {
  init();
}

OldRegex::OldRegex(const OldRegex &rhs) {
  init(&rhs);
}

OldRegex::OldRegex(const String &pattern, const unsigned char *translateTable) {
  init();
  compilePattern(pattern, translateTable);
}

OldRegex::OldRegex(const char *pattern, const unsigned char *translateTable) {
  init();
  compilePattern(pattern, translateTable);
}

void OldRegex::cleanup() {
  if(m_buffer != NULL) {
    FREE(m_buffer );
    m_buffer  = NULL;
  }
}

OldRegex::~OldRegex() {
  cleanup();
}

OldRegex &OldRegex::operator=(const OldRegex &rhs) {
  cleanup();
  init(&rhs);
  return *this;
}

static const char *errmsg = "No regular expression specified";

void OldRegex::compilePattern(const String &s, const unsigned char *translateTable) {
  compilePattern(s.cstr(), translateTable);
}

void OldRegex::compilePattern(const char *s, const unsigned char *translateTable) {
  if(!s) {
    throwException(errmsg);
  }
  m_translateTable = translateTable;
  compilePattern1(s);
}

int OldRegex::search(const String &text, bool forward, int startPos, OldRegexRegisters *registers) const {
  return search(text.cstr(), forward, startPos, registers);
}

int OldRegex::search(const char *text, bool forward, int startPos, OldRegexRegisters *registers) const {
  if(!m_hasCompiled) {
    throwException(errmsg);
  }
  int length = strlen(text);
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

bool OldRegex::match(const String &text, OldRegexRegisters *registers) const {
  if(!m_hasCompiled) {
    throwException(errmsg);
  }
  return match(text.cstr(), text.length(), 0, registers) >= 0 && m_resultLength == text.length();
}

bool OldRegex::match(const char *text, OldRegexRegisters *registers) const {
  if(!m_hasCompiled) {
    throwException(errmsg);
  }
  int len = strlen(text);
  return match(text, len, 0, registers) >= 0 && m_resultLength == len;
}

/* dump routines for debug */
void fprintCharSet(FILE *f,char *set,int size) {
  for(int i = 0; i < size; i++) {
    if(CONTAINSBIT(set,i)) {
      fprintf(f,"%c",i);
    }
  }
}

void OldRegex::dump(FILE *f) const {
  for(char *p = m_buffer;;) {
    int address = p - m_buffer;
    fprintf(f,"%4d:",address);
    if(address >= m_used) {
      fprintf(f,"end\n");
      break;
    }
    RegexPCode opcode;
    switch(opcode = (RegexPCode)*p++) {
    case unused:
      fprintf(f,"unused\n");
      break;
    case exactn:              // followed by one byte giving n, 
                              // and then by n literal bytes
    { unsigned char n = *p++;
      fprintf(f,"exactn %u <%*.*s>\n",n,n,n,p);
      p += n;
      break;
    }
    case begline:             // fails unless at beginning of line
      fprintf(f,"begline\n");
      break;
    case endline:             // fails unless at end of line
      fprintf(f,"endline\n");
      break;
    case jump:                // followed by two bytes giving relative address to jump to
    { int n = getJumpAddress(p);
      fprintf(f,"jump %d\n",(p-m_buffer)+n);
      break;
    }
    case onFailureJump:       // followed by two bytes giving relative address of place
                              // to resume in case of failure.
    { int n = getJumpAddress(p);
      fprintf(f,"onFailureJump %d\n",(p-m_buffer)+n);
      break;
    }
    case finalizeJump:        // Throw away latest failure point and then jump to address.
    { int n = getJumpAddress(p);
      fprintf(f,"finalizeJump %d\n",(p-m_buffer)+n);
      break;
    }
    case maybeFinalizeJump:   // Like jump but finalize if safe to do so.
                              // This is used to jump back to the beginning
                              // of a repeat. If the command that follows
                              // this jump is clearly incompatible with the
                              // one at the beginning of the repeat, such that
                              // we can be sure that there is no use backtracking
                              // out of repetitions already completed,
                              // then we finalize.

    { int n = getJumpAddress(p);
      fprintf(f,"maybeFinalizeJump %d\n",(p-m_buffer)+n);
      break;
    }
    case dummyFailureJump:    // jump, and push a dummy failure point.
                              // This failure point will be thrown away
                              // if an attempt is made to use it for a failure.
                              // A + construct makes this before the first repeat.

    { int n = getJumpAddress(p);
      fprintf(f,"dummyFailureJump %d\n",(p-m_buffer)+n);
      break;
    }

    case repeat:
    { unsigned short minRepeat = getUShort(p);
      unsigned short maxRepeat = getUShort(p);
      fprintf(f,"repeat[%hu,%hu]\n",minRepeat,maxRepeat);
      break;
    }

    case anychar:             // matches any one character
      fprintf(f,"anychar\n");
      break;
    case charset:          
    case charsetNot:          // matches any one char belonging to specified set.
                              // First following byte is # bitmap bytes.
                              // Then come bytes for a bit-map saying which chars are in.
                              // Bits in each byte are ordered low-bit-first.
                              // A character is in the set if its bit is 1.
                              // A character too large to have a bit in the map
                              // is automatically not in the set
    { char bytesInSet = *p++;
      unsigned char setSize = bytesInSet * BITSPERBYTE;
      fprintf(f,opcode==charset?"charset[":"charsetNot[");
      fprintCharSet(f,p,setSize);
      p += bytesInSet;
      fprintf(f,"]\n");
      break;
    }

    case startMemory:         // starts remembering the text that is matched
                              // and stores it in a memory register.
                              // followed by one byte containing the register number.
                              // Register numbers must be in the range 0 through NREGS.
    { unsigned char n = *p++;
      fprintf(f,"startMemory %d\n",n);
      break;
    }
    case stopMemory:          // stops remembering the text that is matched
                              // and stores it in a memory register.
                              // followed by one byte containing the register number.
                              // Register numbers must be in the range 0 through NREGS.
    { unsigned char n = *p++;
      fprintf(f,"stopMemory %d\n",n);
      break;
    }
    case duplicate:           // match a duplicate of something remembered.
                              // Followed by one byte containing the index of 
                              // the memory register.
    { unsigned char n = *p++;
      fprintf(f,"duplicate %d\n",n);
      break;
    }
    case beforeDot:           // Succeeds if before dot
      fprintf(f,"beforeDot\n");
      break;
    case atDot:               // Succeeds if at dot
      fprintf(f,"atDot\n");
      break;
    case afterDot:            // Succeeds if after dot
      fprintf(f,"afterDot\n");
      break;
    case begbuf:              // Succeeds if at beginning of buffer
      fprintf(f,"begbuf\n");
      break;
    case endbuf:              // Succeeds if at end of buffer
      fprintf(f,"endbuf\n");
      break;
    case wordchar:            // Matches any word-constituent character
      fprintf(f,"wordchar\n");
      break;
    case notWordchar:         // Matches any char that is not a word-constituent
      fprintf(f,"notWordchar\n");
      break;
    case wordbeg:             // Succeeds if at word beginning
      fprintf(f,"wordbeg\n");
      break;
    case wordend:             // Succeeds if at word end
      fprintf(f,"wordend\n");
      break;
    case wordBound:           // Succeeds if at a word boundary
      fprintf(f,"wordBound\n");
      break;
    case notWordBound:        // Succeeds if not at a word boundary
      fprintf(f,"notWordBound\n");
      break;
    default:
      fprintf(f,"unknown opcode:%d\n",opcode);
      break;
    }
  }
}
