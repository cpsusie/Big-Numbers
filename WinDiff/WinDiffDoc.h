#pragma once

#include "Diff.h"
#include "FileFormat.h"
#include "RegexFilter.h"

class WinDiffComparator : public LcsComparator {
private:
  bool m_ignoreCase;
public:
  WinDiffComparator();

  int compare(const TCHAR * const &s1, const TCHAR * const &s2);

  bool setIgnoreCase(bool newValue); // return true if changed

  AbstractComparator *clone() const {
    return new WinDiffComparator(*this);
  }

  double getSortTimeFactor() const;
  double getMatchTimeFactor() const;
  String toString() const;
};

#define FLAG_IGNORE_WHITEPSACE 0x01
#define FLAG_IGNORE_COMMENTS   0x02
#define FLAG_STRIP_COMMENTS    0x04
#define FLAG_IGNORE_STRINGS    0x08
#define FLAG_IGNORE_COLUMNS    0x10
#define FLAG_IGNORE_REGEX      0x20

class WinDiffFilter : public DiffFilter {
private:
  UINT m_flags;

  FileFormat    m_fileFormat;
  RegexFilter   m_regexFilter;

  static String stripWhiteSpace(       const String &s);
  static String stripLineComments(     const String &s); // strip //... comments
  static String stripStrings(          const String &s);
  static String stripMultilineComments(const String &s); // strip /* ... */ comments

public:
  WinDiffFilter();

  String lineFilter(const TCHAR *s) const;
  String docFilter( const TCHAR *s) const;
  bool   hasLineFilter() const {
    return (m_flags & (FLAG_IGNORE_WHITEPSACE | FLAG_IGNORE_COMMENTS | FLAG_IGNORE_STRINGS | FLAG_IGNORE_COLUMNS | FLAG_IGNORE_REGEX)) != 0;
  }
  bool   hasDocFilter()  const {
    return (m_flags & FLAG_STRIP_COMMENTS) != 0;
  }
  DiffFilter *clone() const {
    return new WinDiffFilter(*this);
  }

  bool setIgnoreWhiteSpace(bool newValue);                  // return true if changed
  bool setIgnoreComments(  bool newValue);                  // return true if changed
  bool setStripComments(   bool newValue);                  // return true if changed
  bool setIgnoreStrings(   bool newValue);                  // return true if changed
  bool setFileFormat(      const FileFormat  *fileFormat);  // return true if changed
  bool setRegexFilter(     const RegexFilter *filter    );  // return true if changed
  double getTimeFactor() const;
  String toString() const;
};

class CWinDiffDoc : public CDocument, public OptionsUpdater {
private:
  void updateDiffDoc(bool recompare);
  void initOptions(const Options &options);
protected: // create from serialization only
  CWinDiffDoc();
  DECLARE_DYNCREATE(CWinDiffDoc)

// Attributes
public:
  Diff              m_diff;
  WinDiffComparator m_cmp;
  WinDiffFilter     m_filter;
  void setDoc(int id, DiffDocType type, const String &arg, bool recomp=true);
  void setDocs(const String &name1, const String &name2);
  bool setIgnoreWhiteSpace( bool newValue, bool recomp);
  bool setIgnoreCase(       bool newValue, bool recomp);
  bool setIgnoreStrings(    bool newValue, bool recomp);
  bool setIgnoreComments(   bool newValue, bool recomp);
  bool setFileFormat(       const FileFormat   *columnParameter, bool recomp);
  bool setRegexFilter(      const RegexFilter  *filter         , bool recomp);
  bool setStripComments(    bool newValue, bool recomp);
  bool setViewWhiteSpace(   bool newValue, bool update);
  bool setTabSize(          int  newValue, bool update);
  int  getDocSize(int id) const;
  void recompare();
  void refresh(); // dont recompare, just make lines again. Faster than recompare()
// Operations
public:

public:
  virtual BOOL OnNewDocument();
  virtual void Serialize(CArchive& ar);

public:
    virtual ~CWinDiffDoc();
#if defined(_DEBUG)
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
  DECLARE_MESSAGE_MAP()
};

