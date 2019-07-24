#pragma once

class SourcePosition {
private:
  int    m_lineNumber;
  int    m_column;
public:
  inline SourcePosition(int lineNumber = 0, int column = 0)
    : m_lineNumber(lineNumber)
    , m_column(column)
  {
  }
  SourcePosition(const String &s, UINT index);

  void setLocation(int lineNumber, int column);

  // SourcePosition(s, index).findCharIndex(s) == index; index = [0..s.length()[
  UINT findCharIndex(const String &s) const;

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

  // assume same fileName
  inline int compare(const SourcePosition &pos) const {
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
  inline bool operator==(const SourcePosition &pos) const {
    return compare(pos) == 0;
  }
  inline bool operator!=(const SourcePosition &pos) const {
    return !(*this == pos);
  }
  String toString() const {
    return format(_T("(%d,%d)"), m_lineNumber, m_column);
  }
};

class SourcePositionWithName : public SourcePosition {
private:
  String m_name;
public:
  inline SourcePositionWithName(const String &name, const SourcePosition &pos)
    : m_name(name)
    , SourcePosition(pos)
  {
  }
  inline SourcePositionWithName(const String &name = EMPTYSTRING, int lineNumber = 0, int column = 0)
    : m_name(name)
    , SourcePosition(lineNumber, column)
  {
  }
  SourcePositionWithName(const String &name, const String &s, UINT index)
    : m_name(name)
    , SourcePosition(s, index) {
  }
  const String &getName() const {
    return m_name;
  }
  String toString() const;
};
