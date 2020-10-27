#pragma once

#include <time.h>
#include "LcsSimple.h"
#include "LcsBitSet.h"

typedef enum {
  EQUALLINES
 ,CHANGEDLINES
 ,DELETEDLINES
 ,INSERTEDLINES
} DiffLineAttribute;

class TextWithIndex {
private:
  union {
    const TCHAR *m_str;    // Pointer into Diff.m_stringPool
    UINT         m_offset; // Used when building the DiffLines in makeDiffLines. This ends with a recalculation of m_offset,
                           // so m_str points into m_stringPool
                           // When this is done, m_stringPool should NOT be resized, as the buffer-address may change,
                           // causing all pointers (m_str) to be invalid => Disaster!!
  };
  int         m_index;
  TextWithIndex(UINT offset, int index) : m_offset(offset), m_index(index) {
  }
public:
  TextWithIndex() : m_offset(0), m_index(-1) {
  }
  const TCHAR *getString() const {
    return m_str;
  }
  int getIndex() const {
    return m_index;
  }
  friend class Diff;
  friend class DiffLine;
};

class DiffLine {
private:
  TextWithIndex      m_text[2];
  DiffLineAttribute  m_attr;
  DiffLine(UINT offset0, UINT offset1, int l0, int l1, DiffLineAttribute attr);
public:
  DiffLine();
  const TextWithIndex &getText(int id) const {
    return m_text[id];
  }
  DiffLineAttribute getAttr() const {
    return m_attr;
  }
  void swap();
  bool linesAreEqual() const;
  friend class Diff;
};

typedef CompactArray<DiffLine> DiffLineArray;

class DiffFilter {
public:
  virtual String lineFilter(const TCHAR *s) const = 0;
  virtual String docFilter( const TCHAR *s) const = 0;
  virtual bool   hasLineFilter()            const = 0;
  virtual bool   hasDocFilter()             const = 0;
  virtual DiffFilter *clone()               const = 0;
};

extern DiffFilter *stdDiffFilter;

typedef enum {
  DIFFDOC_FILE
 ,DIFFDOC_BUF
} DiffDocType;

class DiffDoc {
private:
  DiffDocType          m_type;
  String               m_buf;
  String               m_name;
  mutable time_t       m_lastReadTime;
  mutable size_t       m_lineCount;
  mutable UINT         m_fileSize;
  void clear();
  void processBuffer(const TCHAR *buf, DiffFilter &filter, LineArray &la, InterruptableRunnable *runnable) const;
  void readTextFile(                   DiffFilter &filter, LineArray &la, InterruptableRunnable *runnable) const;

public:
  DiffDoc();
  DiffDoc(DiffDocType type, const TCHAR *arg);
  ~DiffDoc();
  void setToFile(const TCHAR *fname);
  void setToBuf( const TCHAR *buf  );
  void getLines(DiffFilter &filter, LineArray &s, InterruptableRunnable *runnable = nullptr) const;

  inline const String &getName() const {
    return m_name;
  }

  inline DiffDocType getType() const {
    return m_type;
  }

  inline bool isFile() const {
    return getType() == DIFFDOC_FILE;
  }

  time_t getLastModifiedTime() const;

  inline time_t getLastReadTime() const {
    return m_lastReadTime;
  }

  inline bool isDirty() const {
    return getLastReadTime() != getLastModifiedTime();
  }

  const String &getArg() const {
    return (m_type == DIFFDOC_FILE) ? m_name : m_buf;
  }

  bool isEmpty() const;
  UINT getSize() const;
  inline size_t getLineCount() const {
    return m_lineCount;
  }
};

class Diff {
private:
  DiffDoc                 m_doc[2];
  size_t                  m_maxLineLength;
  int                     m_firstDiffLine;
  int                     m_lastDiffLine;
  int                     m_tabSize;
  bool                    m_viewWhiteSpace;
  DiffLineArray           m_lines;
  StringPool              m_stringPool;
  DiffFilter             *m_diffFilter;
  bool                    m_enableAddDiffLines;
  UINT                    m_docSize[2];
  mutable CompareJob     *m_job;

  void init();
  void syncArrays(   const LineArray &a, const LineArray &b, int &ai, int ato, int &bi, int bto);
  void makeDiffLines(const LineArray &a, const LineArray &b, const ElementPairArray &pairs);
  void makeDiffLines(const LineArray &a, const LineArray &b);
  void compare(      const LineArray &a, const LineArray &b, DiffFilter &filter, LcsComparator &cmp);
  void findLcs(ElementPairArray &p, const LineArray &a, const LineArray &b, LcsComparator &cmp);
  void findFirstLastDiff();
  void tabExpand(LineArray &dst, const LineArray &src, int tabSize, bool viewWhiteSpace, int id);
  void addDiffLine(  const TCHAR *s0, const TCHAR *s1, int l0, int l1, DiffLineAttribute attr);
  void recalculateTextPointers();
public:
  Diff();
  Diff(const TCHAR *fname1, const TCHAR *fname2, CompareJob *job);
  Diff(const Diff &src);             // not implemented
  Diff &operator=(const Diff &src); // not implemented
  ~Diff();
  bool refreshLines(CompareJob *job);
  void compare(                                                 DiffFilter &filter, LcsComparator &cmp, CompareJob *job);
  void compareBuffers(const TCHAR *b1    , const TCHAR *b2    , DiffFilter &filter, LcsComparator &cmp, CompareJob *job);
  void compareFiles(  const TCHAR *fname1, const TCHAR *fname2, DiffFilter &filter, LcsComparator &cmp, CompareJob *job);
  void setDoc(int id, DiffDocType type,    const TCHAR *arg); // id = {0,1}, arg = filename or buffer
  void setDoc(int id, const DiffDoc &doc);
  void clear();

  inline bool isDirty() const {
    return m_doc[0].isDirty() || m_doc[1].isDirty();
  }

  inline const DiffLineArray &getDiffLines() const {
    return m_lines;
  }

  inline const DiffLine &getDiffLine(int i) const {
    return m_lines[i];
  }

  inline int getLineCount() const {
    return (int)m_lines.size();
  }

  inline int getMaxLineLength() const {
    return (int)m_maxLineLength;
  }

  inline int getFirstDiffLine() const {
    return m_firstDiffLine;
  }

  inline int getLastDiffLine() const {
    return m_lastDiffLine;
  }

  inline int getTabSize() const {
    return m_tabSize;
  }

  bool setTabSize(int newValue, bool refresh);

  bool setViewWhiteSpace(bool newValue, bool refresh);

  inline bool getViewWhiteSpace() const {
    return m_viewWhiteSpace;
  }

  int findLineIndex(int id, UINT line) const;

  inline String getName(int id) const {
    return m_doc[id].getName();
  }

  inline const DiffDoc &getDoc(int id) const {
    return m_doc[id];
  }

  void swapDocs();

  inline bool isEmpty() const {
    return m_lines.isEmpty();
  }

  inline bool hasFileDoc() const {
    return m_doc[0].isFile() || m_doc[1].isFile();
  }
};

class StrDiff {
private:
  void syncArrays(   const LineArray &a, const LineArray &b, int &ai, int ato, int &bi, int bto);
  void makeDiffLines(const LineArray &a, const LineArray &b, const ElementPairArray &pairs);
public:
  void compareStrings(const TCHAR *s1, const TCHAR *s2, LcsComparator &cmp = *stdLcsComparator);
  CompactIntArray d1,d2; // d1.size = _tcslen(s1), d2.size = _tcslen(s2). d1[i],d2[i] = {EQUALLINES,CHANGEDLINES,DELETEDLINES,INSERTEDLINES}
};
