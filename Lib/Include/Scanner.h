#pragma once

#include "MyUtil.h"
#include <ctype.h>

class LexStream { // abstract class for inputstreams
public:
  virtual bool open(const String &s)                  = NULL; // Opens a new inputstream. return true on success, false on error
  virtual void close()                                = NULL; // Close stream
  virtual bool eof()                                  = NULL;
  virtual bool ok()                                   = NULL;
  virtual intptr_t getChars(_TUCHAR *dst, size_t n)   = NULL;
};

#ifdef UNICODE

#include "TextFormatDetecter.h"

class ByteQueue : private ByteArray {
private:
  DECLARECLASSNAME;
  bool         m_firstRead;
  TextFormat   m_textFormat;
  bool hasFullLine() const;
  const BYTE *findLastNewLine() const;
public:
  inline ByteQueue() {
    clear();
  }
  inline void clear() {
    __super::clear();
    m_firstRead = true;
  }
  inline size_t size() const {
    return __super::size();
  }
  inline bool isEmpty() const {
    return size() == 0;
  }

  String getConvertedString(size_t count); // count is the number of bytes to convert
  size_t readUntilHasNewLine(FILE *f);
};

class CharQueue : private String {
private:
  DECLARECLASSNAME;
public:
  CharQueue() {
  }
  inline void clear() {
    (String&)(*this) = EMPTYSTRING;
  };
  inline size_t size() const {
    return length();
  }
  inline bool isEmpty() const {
    return size() == 0;
  }

  inline void put(const String &s) {
    *this += s;
  }
  intptr_t get(_TUCHAR *dst, size_t n);
};

#endif

class LexFileStream : public LexStream {
private:
  DECLARECLASSNAME;
  FILE        *m_f;
  int          m_oldMode;

#ifdef UNICODE
  ByteQueue    m_rawQueue;
  CharQueue    m_convertedQueue;
#endif
  void initQueues();

public:

  LexFileStream();
  LexFileStream(const String &name);

  ~LexFileStream() {
    close();
  }

  bool open(const String &s);
  void close();
  intptr_t getChars(_TUCHAR *dst, size_t n);
  bool eof();

  bool ok() {
    return m_f != NULL;
  }
};

class LexStringStream : public LexStream {
private:
  String   m_string;
  intptr_t m_pos;
public:
  LexStringStream() {
    open(EMPTYSTRING);
  }
  LexStringStream(const String &str) {
    open(str);
  }
  bool open(const String &str);
  void close() {
    open(EMPTYSTRING);
  }
  intptr_t getChars(_TUCHAR *dst, size_t n);

  bool eof() {
    return m_pos >= (intptr_t)m_string.length();
  }

  bool ok() {
    return true;
  }
};

class SourcePosition {
private:
  String m_fileName;
  int    m_lineNumber;
  int    m_column;
public:
  inline SourcePosition(const String &fileName, int lineNumber, int column)
    : m_fileName(fileName)
    , m_lineNumber(lineNumber)
    , m_column(column)
  {
  }

  inline SourcePosition(int lineNumber = 0, int column = 0)
    : m_lineNumber(lineNumber)
    , m_column(column)
  {
  }
  SourcePosition(const String &s, UINT index);

  void setLocation(int lineNumber, int column);

  // SourcePosition(s, index).findCharIndex(s) == index; index = [0..s.length()[
  int findCharIndex(const String &s) const;

  inline const String &getFileName() const {
    return m_fileName;
  }

  inline int getLineNumber() const {
    return m_lineNumber;
  }

  inline int getColumn() const {
    return m_column;
  }

  inline void incrLineNumber() {
    m_lineNumber++;
    m_column = 0;
  }

  inline void incrColumn(int amount = 1) {
    m_column += amount;
  }

  inline int compare(const SourcePosition &pos) const { // assume same fileName
    const int c = m_lineNumber - pos.m_lineNumber;
    if(c) return c;
    return m_column - pos.m_column;
  }

  inline bool operator<(const SourcePosition &pos) const {
    return compare(pos) < 0;
  }
  inline bool operator>(const SourcePosition &pos) const {
    return compare(pos) > 0;
  }
  inline bool operator<=(const SourcePosition &pos) const {
    return compare(pos) <= 0;
  }
  inline bool operator>=(const SourcePosition &pos) const {
    return compare(pos) >= 0;
  }
  bool operator==(const SourcePosition &pos) const {
    return compare(pos) == 0;
  }
  bool operator!=(const SourcePosition &pos) const {
    return !(*this == pos);
  }
  String toString() const;
  friend class Scanner;
};

class Scanner {

#define SCANNERMAXLOOK    16                     // Maximum amount of lookahead
#define SCANNERMAXLEX     1024                   // Maximum lexeme sizes.
#define SCANNERBUFSIZE    ((8*SCANNERMAXLEX)+(2*SCANNERMAXLOOK))

                                                 //  Values of the anchor field:
#define ANCHOR_NONE  0                           //  Not anchored
#define ANCHOR_START 1                           //  Lexeme Anchored at start of line
#define ANCHOR_END   2                           //  At end of line
#define ANCHOR_BOTH  (ANCHOR_START | ANCHOR_END) //  Both start and end of line.

private:
  LexStream      *m_inputStream;                 // Input source
  _TUCHAR         m_inputBuffer[SCANNERBUFSIZE]; // Input buffer
  _TUCHAR        *m_endBuf;                      // Just past last TCHAR
  _TUCHAR        *m_nextChar;                    // Next input TCHAR
  _TUCHAR        *m_startMark;                   // Start of current lexeme
  _TUCHAR        *m_endMark;                     // End of current lexeme
  _TUCHAR        *m_previousMark;                // Start of previous lexeme
  intptr_t        m_previousLength;              // Length of previous lexeme
  SourcePosition  m_pos;                         // Current scanner position
  SourcePosition  m_previousPos;                 // Position of previous accepted lexeme
  SourcePosition  m_startPos;                    // Position of last accepted lexeme
  SourcePosition  m_markPos;                     // Position when markEnd() called
  _TUCHAR         m_termchar;                    // Holds the TCHAR that was overwritten by a 0.
                                                 // See terminateLexeme()/unTerminateLexeme()
  bool            m_eofRead;                     // true if end of file has been read.
                                                 // It's possible for this to be true,
                                                 // and for characters to still be in the input buffer.
  bool            m_moreFlag;

  void init(LexStream *stream, const SourcePosition &pos);

  intptr_t fillBuf(_TUCHAR *start);

  bool noMoreChars() const {
    return m_eofRead && (m_nextChar >= m_endBuf);
  }

  // Just past last char in buf
  inline _TUCHAR *end() {
    return m_inputBuffer + SCANNERBUFSIZE;
  }

protected:
  // If true, call debug on every cycle
  bool m_debug;

  // Return the n'th TCHAR of lookahead.
  // Return EOF if you try to look past end of file,
  // 0 if you try to look past end of the buffer..
  int  look(int n);

  // Returns the next TCHAR from input and advances past it
  int  advance();
  int  flushBuf();
  int  flush(bool force);
  int  input();
  void unput(int ch);
  void less(int n);

  // Push count characters back into the input.
  bool pushback(int count);

  // when called, next call to getNextLexeme() will return EOS (0)
  void terminateLexeme();
  void unTerminateLexeme();

  // when called, next call to getNextLexeme() will return EOS
  void terminateInput();
  virtual void setMore();
  virtual void initMore();
  virtual bool isMore() const;
  virtual bool isWrap();
  void debugState(const TCHAR *label, int state, int lookahead);

public:
  Scanner();
  Scanner(LexStream *stream);
  ~Scanner();
  void newStream(LexStream *stream, const SourcePosition &pos);
  void newStream(LexStream *stream, int lineno = 1);
  int setLineNumber(int lineNumber);
  SourcePosition setPos(const SourcePosition &pos);

  inline _TUCHAR *getText() const {
    return m_startMark;
  }

  inline intptr_t getLength() const {
    return m_endMark - m_startMark;
  }

  SourcePosition getPos() const;

  inline int getLineNumber() const {
    return getPos().getLineNumber();
  }

  inline _TUCHAR *getPreviousText() const {
    return m_previousMark;
  }

  inline intptr_t getPreviousLength() const {
    return m_previousLength;
  }

  inline const SourcePosition &getPreviousPos() const {
    return m_previousPos;
  }

  inline const SourcePosition &getStartPos() const {
    return m_startPos;
  }

  inline int getPreviousLineNumber() const {
    return m_previousPos.getLineNumber();
  }

  _TUCHAR *markStart();    // Set start- and endMark to current position. Returns next TCHAR
  _TUCHAR *markEnd();      // Set endMark to current position. Returns next TCHAR
  _TUCHAR *moveStart();    // If startMark is set, advance it one TCHAR
  _TUCHAR *markPrevious(); // Set previousMark and Length = position remembered by markStart/markEnd
  _TUCHAR *gotoMark();     // Set current position = position remembered by markEnd()

  inline void setDebug(bool newvalue) {
    m_debug = newvalue;
  }
  void error(const SourcePosition &pos, const TCHAR *format, ...);
  void debug(const TCHAR *format, ...);
  virtual void verror(const SourcePosition &pos, const TCHAR *format, va_list argptr);
  virtual void vdebug(const TCHAR *format, va_list argptr);
  // virtual. default inputfunction. Will be generated in derived class by lexgen
  virtual int getNextLexeme() {
    return 0;
  }
};
