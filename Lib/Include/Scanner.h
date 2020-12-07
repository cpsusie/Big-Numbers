#pragma once

#include <ctype.h>
#include "ByteArray.h"
#include "SourcePosition.h"

// Abstract class (interface) for inputstreams
class LexStream {
public:
  // Opens a new inputstream. return true on success, false on error
  virtual bool open(const String &s)                  = 0;
  // Close stream
  virtual void close()                                = 0;
  virtual bool eof()                                  = 0;
  virtual bool ok()                                   = 0;
  virtual intptr_t getChars(_TUCHAR *dst, size_t n)   = 0;
};

#if defined(UNICODE)

#include "TextFormatDetecter.h"

class ByteQueue : private ByteArray {
private:
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

  // Assume there are at least count bytes in ByteQueue.
  // Convert count bytes to string
  String getConvertedString(size_t count);
  size_t readUntilHasNewLine(FILE *f);
};

class CharQueue : private String {
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
  FILE        *m_f;
  int          m_oldMode;

#if defined(UNICODE)
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

  bool open(const String &s)                override;
  void close()                              override;
  intptr_t getChars(_TUCHAR *dst, size_t n) override;
  bool eof()                                override;
  bool ok()                                 override {
    return m_f != nullptr;
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
  bool open(const String &str)              override;
  void close()                              override {
    open(EMPTYSTRING);
  }
  intptr_t getChars(_TUCHAR *dst, size_t n) override;
  bool eof()                                override {
    return m_pos >= (intptr_t)m_string.length();
  }
  bool ok()                                 override {
    return true;
  }
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
  // Input source
  LexStream      *m_inputStream;
   // Input buffer
  _TUCHAR         m_inputBuffer[SCANNERBUFSIZE];
  // Just past last TCHAR
  _TUCHAR        *m_endBuf;
  // Next input TCHAR
  _TUCHAR        *m_nextChar;
  // Start of current lexeme
  _TUCHAR        *m_startMark;
  // End of current lexeme
  _TUCHAR        *m_endMark;
  // Start of previous lexeme
  _TUCHAR        *m_previousMark;
  // Length of previous lexeme
  intptr_t        m_previousLength;
  // Current scanner position
  SourcePosition  m_pos;
  // Position of previous accepted lexeme
  SourcePosition  m_previousPos;
  // Position of last accepted lexeme
  SourcePosition  m_startPos;
  // Position when markEnd() called
  SourcePosition  m_markPos;
  // Holds the TCHAR that was overwritten by a 0
  // See terminateLexeme()/unTerminateLexeme()
  _TUCHAR         m_termchar;

  // true if end of file has been read.
  // It's possible for this to be true,
  // and for characters to still be in the input buffer.
  bool            m_eofRead;
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

  // Push count characters back into the input. You can't push past the current
  // startMark. You can, however, push back characters after end of file has been encountered.
  bool pushback(int count);

  void terminateLexeme();
  void unTerminateLexeme();

  // When called, next call to getNextLexeme() will return EOS
  void terminateInput();
  virtual void setMore();
  virtual void initMore();
  virtual bool isMore() const;
  virtual bool isWrap();
  void debugState(const TCHAR *label, int state, int lookahead);

public:
  Scanner();
  Scanner(LexStream *stream);
  virtual ~Scanner();
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

  // Set start- and endMark to current position. Returns next TCHAR
  _TUCHAR *markStart();
  // Set endMark to current position. Returns next TCHAR
  _TUCHAR *markEnd();
  // If startMark is set, advance it one TCHAR
  _TUCHAR *moveStart();
  // Set previousMark and Length = position remembered by markStart/markEnd
  _TUCHAR *markPrevious();
  // Set current position = position remembered by markEnd()
  _TUCHAR *gotoMark();

  inline void setDebug(bool newvalue) {
    m_debug = newvalue;
  }
  void error(const SourcePosition &pos         , _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
  void debug(                                    _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
  virtual void verror(const SourcePosition &pos, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);
  virtual void vdebug(                           _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);
  // virtual. default inputfunction. Will be generated in derived class by lexgen
  virtual int getNextLexeme() {
    return 0;
  }
};
