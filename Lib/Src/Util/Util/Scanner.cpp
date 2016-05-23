#include "pch.h"
#include <Scanner.h>

#define NEWLINE _T('\n')

Scanner::Scanner() {

  static LexFileStream filestream_STDIN;

  m_debug = false;
  init(&filestream_STDIN, SourcePosition());
}

Scanner::Scanner(LexStream *stream) {
  m_debug = false;
  init(stream, SourcePosition());
}

Scanner::~Scanner() {
  if(m_inputStream) {
    m_inputStream->close();
  }
}

void Scanner::init(LexStream *stream, const SourcePosition &pos) {
  m_endBuf         = end();
  m_previousMark   = NULL;
  m_previousLength = 0;

  m_inputStream    = stream;
  m_termchar       = 0;
  m_eofRead        = false;

  //  if pos.m_col == 0, we have to simulate an empty line before 
  //  the first input line, so that lex start-of-line anchor 
  //  will work on the first input line.
  //  use the same trick on eoi, see fillBuf

  m_pos = pos;
  if(pos.m_column == 0) {
    m_nextChar  = m_startMark = m_endMark = end() - 1;
    *m_nextChar = NEWLINE;
    m_pos.m_lineNumber--; // -1 to cancel the "\n" we just added
  } else {
    m_nextChar  = m_startMark = m_endMark = end();
    flushBuf();
  }

  m_previousPos = m_pos;

  if(m_debug) {
    debug(_T("Initialize Scanner"));
  }
}

void Scanner::newStream(LexStream *stream, const SourcePosition &pos) {
  init(stream, pos);
}

void Scanner::newStream(LexStream *stream, int lineno) {
  init(stream, lineno);
}

int Scanner::setLineNumber(int lineNumber) {
  return setPos(lineNumber).getLineNumber();
}

SourcePosition Scanner::setPos(const SourcePosition &pos) {
  SourcePosition old = m_pos;
  m_pos = pos;
  return old;
}

_TUCHAR *Scanner::markStart() {
  m_markPos = m_pos;
  return m_endMark = m_startMark = m_nextChar;
}

_TUCHAR *Scanner::markEnd() {
  m_markPos = m_pos;
  return m_endMark = m_nextChar;
}

_TUCHAR *Scanner::moveStart() {
  if(m_startMark >= m_endMark) {
    return NULL;
  } else {
    return ++m_startMark;
  }
}

_TUCHAR *Scanner::markPrevious() {
  // Set the previousMark. A call to flush() won't go past m_previousMark so,
  // once set, we must move it every time we move startMark.
  // This should not be done automatically, because we might want to remember the
  // token before last rather than the last one.
  // If markPrevious() is never called, m_previousMark is just ignored.

  m_previousPos         = m_pos;
  m_previousLength      = m_endMark - m_startMark;
  return m_previousMark = m_startMark;
}

_TUCHAR *Scanner::gotoMark() {
  m_pos = m_markPos;
  return m_nextChar = m_endMark;
}

int Scanner::look(int n) { // protected
  if(n == 1 && m_termchar) {
    return m_termchar;
  }

  // Return the n'th TCHAR of lookahead, EOF if you try to look past
  // end of file, or 0 if you try to look past end of the buffer.

  if(n > (m_endBuf - m_nextChar)) { // (m_endBuf - m_nextChar) is the # of unread
    return m_eofRead ? EOF : 0;     // chars in the buffer (including the one pointed to by m_nextChar).
  }

  // The current lookahead TCHAR is at m_nextChar[0]. The last TCHAR
  // read is at m_nextChar[-1]. The --n in the following if statement adjusts
  // n so that m_nextChar[n] will reference the correct TCHAR.

  if(--n < -(m_nextChar - m_inputBuffer)) { // (m_nextChar - m_inputBuffer) is the # of buffered
    return 0;                               // characters that have been read.
  }

  return m_nextChar[n];
}

int Scanner::advance() { // protected
  // advance() is the real input function. It returns the next TCHAR
  // from input and advances past it. The buffer is flushed if the current
  // TCHAR is within SCANNERMAXLOOK characters of the end of the buffer. 0 is
  // returned at end of file. -1 is returned if the buffer can't be flushed
  // because it's too full. In this case you can call flush(1) to do a
  // buffer flush but you'll loose the current lexeme as a consequence.


  if(noMoreChars()) {
    return 0;
  }

  if(!m_eofRead && flush(false) < 0) {
    return -1;
  }

  if(*m_nextChar == NEWLINE) {
    m_pos.m_lineNumber++;
    m_pos.m_column = 0;
  } else {
    m_pos.m_column++;
  }

  return *m_nextChar++;
}

int Scanner::flushBuf() { // protected
  if(m_termchar) {
    unTerminateLexeme();
  }

  return flush(true);
}

#define SCANNERDANGER   (m_endBuf - SCANNERMAXLOOK)
                                                    // Flush buffer when Next passes this address
// Flush the input buffer. Do nothing if the current input TCHAR isn't
// in the SCANNERDANGER zone, otherwise move all unread characters to the left end
// of the buffer and fill the remainder of the buffer. Note that input()
// flushes the buffer willy-nilly if you read past the end of buffer.
// Similarly, input_line() flushes the buffer at the beginning of each line.
//
//                                 pMark    SCANNERDANGER
//                                  |          |
//      Start_buf                  sMark eMark |Next  End_buf
//          |                       ||     |   ||     |
//          V                       VV     V   VV     V
//          +-----------------------+----------------+-------+
//          | this is already read  | to be done yet | waste |
//          +-----------------------+----------------+-------+
//          |                       |                |       |
//          |<---- shiftAmount ---->|<- copyAmount ->|       |
//          |                                                |
//          |<------------------ SCANNERBUFSIZE ------------>|
//
// Either the pMark or sMark (whichever is smaller) is used as the leftmost
// edge of the buffer. None of the text to the right of the mark will be
// lost. Return 1 if everything's ok, -1 if the buffer is so full that it
// can't be flushed. 0 if we're at end of file. If "force" is true, a buffer
// flush is forced and the characters already in it are discarded. Don't
// call this function on a buffer that's been terminated by terminateLexeme().
int Scanner::flush(bool force) { // protected
  if(noMoreChars()) {
    return 0;
  }

  if(m_eofRead) {            // nothing more to read
    return 1;
  }

  if(m_nextChar >= SCANNERDANGER || force) {
    _TUCHAR *leftEdge = m_previousMark ? min(m_startMark, m_previousMark) : m_startMark;
    int shiftAmount = leftEdge - m_inputBuffer ;

    if(shiftAmount < SCANNERMAXLEX) {               // if(not enough room)
      if(!force) {
        return -1;
      }

      leftEdge = markStart();  // Reset start to current TCHAR
      markPrevious();

      shiftAmount = leftEdge - m_inputBuffer ;
    }

    const int copyAmount = m_endBuf - leftEdge;
    MEMMOVE(m_inputBuffer, leftEdge, copyAmount);

    if(!fillBuf(m_inputBuffer + copyAmount)) {
      throwException(_T("Scanner::flush():INTERNAL ERROR: Buffer full, can't read."));
    }

    if(m_previousMark) {
      m_previousMark -= shiftAmount;
    }

    m_startMark -= shiftAmount;
    m_endMark   -= shiftAmount;
    m_nextChar  -= shiftAmount;
  }

  return 1;
}

int Scanner::input() { // protected
  int ch;
  if(m_termchar) {
    unTerminateLexeme();
    ch = advance();
    markEnd();
    terminateLexeme();
  } else {
    ch = advance();
    markEnd();
  }

  return ch;
}

void Scanner::unput(int ch) {
  if(m_termchar) {
    unTerminateLexeme();
    if(pushback(1)) {
      *m_nextChar = ch;
    }
    terminateLexeme();
  } else {
    if(pushback(1)) {
      *m_nextChar = ch;
    }
  }
}

void Scanner::less(int count) { // protected
  unTerminateLexeme();
  pushback(count);
  terminateLexeme();
}

static unsigned short findColumn(const TCHAR *s, const TCHAR *first) { // search backwards for the the first '\n'
  for(unsigned short count = 0; s >= first && *s != NEWLINE; s--) {
    count++;
  }
  return count - 1;
}

// Push count characters back into the input. You can't push past the current
// startMark. You can, however, push back characters after end of file has been encountered.
bool Scanner::pushback(int count) {  // protected
  bool lineChanged = false;

  while(--count >= 0 && m_nextChar > m_startMark) {
    if(*--m_nextChar == NEWLINE || !*m_nextChar) {
      m_pos.m_lineNumber--;
      lineChanged = true;
    } else {
      m_pos.m_column--;
    }
  }

  if(lineChanged) {
    m_pos.m_column = findColumn((TCHAR*)m_nextChar,(TCHAR*)m_startMark);
  }

  if(m_nextChar < m_endMark) {
    m_endMark = m_nextChar;
    m_markPos = m_pos;
  }

  return m_nextChar > m_startMark;
}

void Scanner::terminateLexeme() { // protected
  m_termchar  = *m_nextChar ;
  *m_nextChar = 0 ;
}

void Scanner::unTerminateLexeme() { // protected
  if(m_termchar) {
    *m_nextChar = m_termchar;
    m_termchar  = 0;
  }
}

void Scanner::terminateInput() { // protected. when called, next call to getNextLexeme() will return EOS (0)
  m_eofRead  = true;
  m_nextChar = m_endBuf;
}

void Scanner::setMore() {
  m_moreFlag = true;
}

void Scanner::initMore() {
  m_moreFlag = false;
}

bool Scanner::isMore() const {
  return m_moreFlag;
}

bool Scanner::isWrap() {
  return true;
}

//----------------------------------------------------------------------------------------------

int Scanner::fillBuf(_TUCHAR *start) { // private
  // Fill the input buffer from start to the end of the buffer.
  // The input file is not closed when EOF is reached. Buffers are read
  // in units of SCANNERMAXLEX characters; it's an error if that many characters
  // cannot be read (0 is returned in this case). For example, if SCANNERMAXLEX
  // is 1024, then 1024 characters will be read at a time. The number of
  // characters read is returned. m_eofRead is set to true as soon as the last buffer is read.

  const int need = ((end() - start) / SCANNERMAXLEX) * SCANNERMAXLEX; // Number of bytes required from input.

  if(need == 0) {
    return 0;
  }

  if(need < 0) {
    throwException(_T("Scanner::fillBuf():INTERNAL ERROR: Bad starting address. need=%d, start=%p, end=%p"), need, start, end());
  }

  int got; // Number of bytes actually read.
  if( (got = m_inputStream->getChars(start, need)) == -1 ) {
    error(getPos(),_T("Can't read input stream"));
    m_endBuf = start;
  } else {
    m_endBuf = start + got;
  }

  if(got < need && m_inputStream->eof()) {
    // make sure that the last input line is terminated with '\n' so 
    // that lex end-of-line anchor will work for the last input line.

    if(m_endBuf[-1] != NEWLINE) { 
      *(m_endBuf++) = NEWLINE;
      got++;
    } else if(got == 0) {
      got = 1; // to catch the extreme case where the inputstream is empty
               // the buffer is initialized to \n, so there is always 1 TCHAR
    }
    m_eofRead = true; // At end of file
  }
  return got;
}

void Scanner::error(const SourcePosition &pos, const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr,format);
  verror(pos,format,argptr);
  va_end(argptr);
}

void Scanner::verror(const SourcePosition &pos, const TCHAR *format, va_list argptr) {
  _tprintf(_T("error in line %d:"),pos.getLineNumber());
  _vtprintf(format,argptr);
  _tprintf(_T("\n")); // we default append a newline.
}

void Scanner::debug(const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr,format);
  vdebug(format,argptr);
  va_end(argptr);
}

void Scanner::vdebug(const TCHAR *format, va_list argptr) {
  _vtprintf(format,argptr);
  _tprintf(_T("\n"));
}

void Scanner::debugState(const TCHAR *label, int state, int lookahead) {
  if(isprint(lookahead)) {
    debug(_T("%sstate:%3d. lookahead:'%c' text:[%*.*s]")
         ,label,state,lookahead,getLength(),getLength(),getText());
  } else {
    debug(_T("%sstate:%3d. lookahead:%#x text:[%*.*s]")
         ,label,state,lookahead,getLength(),getLength(),getText());
  }
}

SourcePosition Scanner::getPos() const {
  int length = getLength();
  if(length <= m_pos.getColumn()) {
    return SourcePosition(m_pos.getFileName(),m_pos.getLineNumber(),m_pos.getColumn() - length);
  } else { // we have one or more newlines in the lexeme
    SourcePosition p(m_pos);
    for(const TCHAR *s = (const TCHAR*)getText() + length; length > 0; s--, length--) {
      if(*s == NEWLINE) {
        p.m_lineNumber--;
      }
    }
    p.m_column = findColumn(s,(TCHAR*)m_inputBuffer);
    return p;
  }
}

int Scanner::getNextLexeme() { // virtual. default inputfunction. Will be generated in derived class by lexgen
  return 0;
}
