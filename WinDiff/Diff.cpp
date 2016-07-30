#include "stdafx.h"
#include <Tokenizer.h>
#include <FileContent.h>
#include "Diff.h"

//#define DUMP_DEBUG

DiffDoc::DiffDoc() {
  setToBuf(_T(""));
  m_job = NULL;
}

DiffDoc::DiffDoc(DiffDocType type, const TCHAR *arg) {
  switch(type) {
  case DIFFDOC_FILE:
    setToFile(arg);
    break;
  case DIFFDOC_BUF :
    setToBuf(arg);
    break;
  }
  m_job = NULL;
}

DiffDoc::~DiffDoc() {
  clear();
}

void DiffDoc::clear() {
  m_buf      = _T("");
  m_fileSize = 0;
}

bool DiffDoc::isEmpty() const {
  switch(m_type) {
  case DIFFDOC_FILE:
    return m_name.length() == 0;
  case DIFFDOC_BUF :
    return m_buf.length() == 0;
  }
  return true;
}

UINT DiffDoc::getSize() const {
  switch(m_type) {
  case DIFFDOC_FILE:
    if(m_fileSize == 0) {
      try {
        m_fileSize = STAT(m_name).st_size;
      } catch(...) {
      }
    }
    return m_fileSize;
  case DIFFDOC_BUF :
    return (UINT)m_buf.length();
  }
  return 0;
}

void DiffDoc::setToFile(const TCHAR *fname) {
  clear();
  m_type         = DIFFDOC_FILE,
  m_name         = fname;
  m_lastReadTime = 0;
}

void DiffDoc::setToBuf(const TCHAR *buf) {
  m_type         = DIFFDOC_BUF;
  m_buf          = buf;
  m_name         = _T("Untitled");
  m_lastReadTime = 0;
  m_fileSize     = 0;
}

static UINT getLineCount(const String &s) {
  UINT count = 0;
  for(const TCHAR *cp = s.cstr(); *cp;) {
    if(*(cp++) == NL) {
      count++;
    }
  }
  return count;
}

void DiffDoc::processBuffer(const TCHAR *buf, DiffFilter &filter, LineArray &la) const {
  String tmp;
  if(filter.hasDocFilter()) {
    tmp = filter.docFilter(buf, m_job);
  } else {
    tmp = buf;
  }

  const size_t totalLineCount = getLineCount(tmp);
  size_t       count          = 0;

  la.setLineCapacity(totalLineCount+2);

  if(m_job) m_job->incrProgress();

BEGIN_TIMEMEASURE(1, _T("Building lineArray"));
  
  for(TCHAR *cp = tmp.cstr();;) { // will chop tmp into 0-terminated strings. Should persist until we are done building s
    TCHAR *nl = _tcschr(cp, NL);
    if(!nl) {
      if(*cp) {
        la.add(cp);
      }
      break;
    }
    if((nl > cp) && (*(nl-1) == _T('\r'))) {
      *(nl-1) = 0;
      la.add(cp);
      if(((count++ & CHECK_INTERVAL) == 0) && m_job) {
        m_job->setSubProgressPercent(SPERCENT(count,totalLineCount));
      }
    } else {    
      *nl = 0;
      la.add(cp);
      if(((count++ & CHECK_INTERVAL) == 0) && m_job) {
        m_job->setSubProgressPercent(SPERCENT(count,totalLineCount));
      }
    }
    if(*(cp = nl + 1) == _T('\r')) {
      cp++;
    }
  }
  la.updateCapacity();

END_TIMEMEASURE(1, tmp.length());
}

void DiffDoc::readFile(DiffFilter &filter, LineArray &la) const {
  FileContent content(m_name);
  m_lastReadTime = STAT(m_name).st_mtime;
  m_fileSize     = (UINT)content.size();

  for(UINT i = 0; i < m_fileSize; i++) {
    if(content[i] == 0) {
      content[i] = _T(' '); // remove any 0-character from file
    }
  }
  TCHAR zterminator = 0;
  content.append((BYTE*)&zterminator, sizeof(zterminator));

  String buf = content.converToString();
  content.clear();
  processBuffer(buf.cstr(), filter, la);
}

time_t DiffDoc::getLastModifiedTime() const {
  if(!isFile()) {
    return 0;
  }
  try {
    return STAT(m_name).st_mtime;
  } catch(...) {
    return 0;
  }
}

void DiffDoc::getLines(DiffFilter &filter, LineArray &la, CompareJob *job) const {
  m_job = job;
  try {
    switch(m_type) {
    case DIFFDOC_FILE:
      readFile(filter, la);
      break;
    case DIFFDOC_BUF :
      processBuffer(m_buf.cstr(), filter, la);
      break;
    }
    m_job = NULL;
  } catch(...) {
    m_job = NULL;
    throw;
  }
}

DiffLine::DiffLine() {
  m_text[0] = m_text[1] = TextWithIndex(0,-1);
  m_attr = EQUALLINES;
}

DiffLine::DiffLine(UINT offset0, UINT offset1, int l0, int l1, DiffLineAttribute attr) {
  m_text[0] = TextWithIndex(offset0, l0);
  m_text[1] = TextWithIndex(offset1, l1);
  m_attr    = attr;
}

bool DiffLine::linesAreEqual() const {
  return (getAttr() == EQUALLINES) && (_tcscmp(m_text[0].getString(), m_text[1].getString()) == 0);
}

void DiffLine::swap() {
  TextWithIndex tmp       = m_text[0];
                m_text[0] = m_text[1];
                m_text[1] = tmp;
  switch(m_attr) {
  case EQUALLINES   :
  case CHANGEDLINES :
    break;
  case DELETEDLINES :
    m_attr = INSERTEDLINES;
    break;
  case INSERTEDLINES:
    m_attr = DELETEDLINES;
    break;
  }
}

#define VISIBLE_SPACE ((_TUCHAR)183)
#define VISIBLE_TAB   ((_TUCHAR)187)

static void tabExpand(String &s, int tabSize, bool viewWhiteSpace) {
  if(s.length() == 0) {
    return;
  }
  if(tabSize == 0) {
    if(viewWhiteSpace) {
      strReplace(strReplace(s.cstr(), _T('\t'), VISIBLE_TAB), _T(' '), VISIBLE_SPACE);
    } else if(s.find('\t') < 0) {
      strReplace(s.cstr(), _T(' '), VISIBLE_SPACE);
    } else {
      TCHAR *buf = new TCHAR[s.length() + 1];
      _tcscpy(buf, s.cstr());
      strRemove(buf,_T('\t'));
      strReplace(buf, _T(' '), VISIBLE_SPACE);
      s = buf;
      delete[] buf;
    }
  } else { // tabSize > 0
    if(s.find(_T('\t')) < 0) {
      if(viewWhiteSpace) {
        strReplace(s.cstr(), _T(' '), VISIBLE_SPACE);
      }
    } else {
      TCHAR *buf = new TCHAR[s.length() * tabSize + 1];
      if(viewWhiteSpace) {
        strReplace(s.cstr(), _T(' '), VISIBLE_SPACE);
        strTabExpand(buf, s.cstr(), tabSize, VISIBLE_TAB);
      } else {
        strTabExpand(buf, s.cstr(), tabSize);
      }
      s = buf;
      delete[] buf;
    }
  }
}

void Diff::tabExpand(LineArray &dst, const LineArray &src, int tabSize, bool viewWhiteSpace, int id) {

  if(m_job) m_job->incrProgress();

BEGIN_TIMEMEASURE(8, _T("TabExpand"));

  const size_t n = src.size();
  dst.setLineCapacity(src.size());
  String tmp;
  for(size_t i = 0; i < n; i++) {
    tmp = src[i];
    ::tabExpand(tmp, tabSize, viewWhiteSpace);
    dst.add(tmp.cstr());
    if(((i & CHECK_INTERVAL) == 0) && m_job) { m_job->setSubProgressPercent(SPERCENT(i,n)); }
  }

END_TIMEMEASURE(  8, m_docSize[id]);
}

void Diff::addDiffLine(const TCHAR *s0, const TCHAR *s1, int l0, int l1, DiffLineAttribute attr) {
  if(!m_enableAddDiffLines) {
    throwException(_T("addDiffLines disabled"));
  }
  if((attr == EQUALLINES) && (_tcscmp(s0, s1) == 0)) { // s0 and s1 may differ, even when attr == EQUALLINES.
    if(*s0 == 0) {
      m_lines.add(DiffLine(0,0, l0, l1, EQUALLINES));
    } else {
      const UINT offset = (UINT)m_stringPool.addString(s0);
      m_lines.add(DiffLine(offset, offset, l0, l1, EQUALLINES));
    }
  } else {
    const UINT offset0 = (UINT)m_stringPool.addString(s0);
    const UINT offset1 = (UINT)m_stringPool.addString(s1);
    m_lines.add(DiffLine(offset0, offset1, l0, l1, attr));
  }
}

void Diff::recalculateTextPointers() { // When this has been called, DONT do any more calls to addDiffLine until clear has been called
  if(!m_enableAddDiffLines) {
    throwException(_T("recalculateTextPointers already called")); // Should only be called once. Only after a call to clear, we can again addLines !!
  }
  
  m_stringPool.clearIndex();
  m_stringPool.setTextCapacity(m_stringPool.getTextSize());
  m_lines.setCapacity(m_lines.size());

  size_t n = m_lines.size();
  if(n) {
    const TCHAR *base = m_stringPool.getString(0); // Will not be changed before clear is called !!
    for(DiffLine *dfp = &m_lines[0]; n--; dfp++) {
      dfp->m_text[0].m_str = base + dfp->m_text[0].m_offset;
      dfp->m_text[1].m_str = base + dfp->m_text[1].m_offset;
    }
  }
  m_enableAddDiffLines = false;
}

void Diff::syncArrays(const LineArray &a, const LineArray &b, int &ai, int ato, int &bi, int bto) {
  if(ai < ato && bi < bto) {
    const int n = max(ato-ai, bto-bi);
    for(int i = 0; i < n; i++) {
      if(ai < ato && bi < bto) {
        addDiffLine(a[ai], b[bi], ai, bi, CHANGEDLINES);
        ai++; bi++;
      } else if(ai < ato) {
        addDiffLine(a[ai], _T(""), ai, -1, CHANGEDLINES);
        ai++;
      } else {
        addDiffLine(_T(""), b[bi], -1, bi, CHANGEDLINES);
        bi++;
      }
    }
  }
  for(;ai < ato; ai++) {
    addDiffLine(a[ai], _T(""), ai, -1, DELETEDLINES);
  }
  for(;bi < bto; bi++) {
    addDiffLine(_T(""), b[bi], -1, bi, INSERTEDLINES);
  }
}

void Diff::makeDiffLines(const LineArray &a, const LineArray &b, const ElementPairArray &pairs) {
  clear();

  m_stringPool.setIndexCapacity(a.size() + b.size());
  m_stringPool.setTextCapacity((a.getTextCapacity() + b.getTextCapacity()) * 2 / 3);

  LineArray ea(m_stringPool), eb(m_stringPool);
  tabExpand(ea, a, m_tabSize, m_viewWhiteSpace, 0);
  tabExpand(eb, b, m_tabSize, m_viewWhiteSpace, 1);

  int ai = 0;
  int bi = 0;

  if(m_job) m_job->incrProgress();

BEGIN_TIMEMEASURE(9, _T("Build final lines"));

  for(size_t i = 0; i < pairs.size(); i++) {
    const ElementPair &e = pairs[i];
    syncArrays(ea, eb, ai, e.m_aIndex, bi, e.m_bIndex);
    const TCHAR *sa = ea[ai];
    const TCHAR *sb = eb[bi];
    addDiffLine(ea[ai], eb[bi], ai, bi, EQUALLINES);

    if(((i & CHECK_INTERVAL) == 0) && m_job) { m_job->setSubProgressPercent(SPERCENT(i,pairs.size())); }

    ai = e.m_aIndex+1;
    bi = e.m_bIndex+1;
  }
  syncArrays(ea, eb, ai, (int)ea.size(), bi, (int)eb.size());
  ea.clear();
  eb.clear();

#ifdef DUMP_DEBUG
a.dump(_T("c:\\temp\\lineArray_A_Before.txt"));
b.dump(_T("c:\\temp\\lineArray_B_Before.txt"));
#endif

  recalculateTextPointers();

  for(size_t i = 0; i < m_lines.size(); i++) {
    for(int id = 0; id < 2; id++) {
      size_t len;
      if((len = _tcsclen(m_lines[i].getText(id).getString())) > m_maxLineLength) {
        m_maxLineLength = len;
      }
    }
  }
  findFirstLastDiff();

END_TIMEMEASURE(  9, m_docSize[0] + m_docSize[1]);

  if(m_job) { m_job->setSubProgressPercent(100); }
}

void Diff::makeDiffLines(const LineArray &a, const LineArray &b) {
  ElementPairArray pairs;
  for(size_t i = 0; i < m_lines.size(); i++) {
    const DiffLine &df = m_lines[i];
    if(df.getAttr() == EQUALLINES) {
      pairs.add(ElementPair(df.getText(0).getIndex(), df.getText(1).getIndex()));
    }
  }
  makeDiffLines(a, b, pairs);
}

void Diff::findFirstLastDiff() {
  m_firstDiffLine = (int)m_lines.size();
  m_lastDiffLine  = -1;

  const int n = (int)m_lines.size();
  int i;
  for(i = 0; i < n; i++) {
    if(m_lines[i].getAttr() != EQUALLINES) {
      break;
    }
  }
  for(; i < n; i++) {
    if(m_lines[i].getAttr() == EQUALLINES) {
      m_firstDiffLine = i - 1;
      break;
    }
  }
  for(i = n - 1; i >= 0; i--) {
    if(m_lines[i].getAttr() != EQUALLINES) {
      break;
    }
  }
  for(; i >= 0; i--) {
    if(m_lines[i].getAttr() == EQUALLINES) {
      m_lastDiffLine = i + 1;
      break;
    }
  }
}

void Diff::compare(const LineArray &a, const LineArray &b, DiffFilter &filter, LcsComparator &cmp) {

  m_docSize[0] = getDoc(0).getSize();
  m_docSize[1] = getDoc(1).getSize();

  m_diffFilter = &filter;

  ElementPairArray epa;
  if(!filter.hasLineFilter()) {
    findLcs(epa, a, b, cmp);
  } else {
    StringPool sp;
    LineArray fa(sp), fb(sp);
    sp.setIndexCapacity(a.size() + b.size());
    sp.setTextCapacity( a.getStringPool().getTextCapacity());
    fa.setLineCapacity( a.size());
    fb.setLineCapacity( b.size());

    if(m_job) m_job->incrProgress();

BEGIN_TIMEMEASURE(4, _T("LineFiltering doc1"));

    const size_t asize = a.size();
    for(size_t i = 0; i < asize; i++) {
      fa.add(filter.lineFilter(a[i]).cstr());
      if(((i & CHECK_INTERVAL) == 0) && m_job) { m_job->setSubProgressPercent(SPERCENT(i,asize)); }
    }
END_TIMEMEASURE(4, m_docSize[0]);

    if(m_job) m_job->incrProgress();

BEGIN_TIMEMEASURE(5, _T("LineFiltering doc2"));

    const size_t bsize = b.size();
    for(size_t i = 0; i < bsize; i++) {
      fb.add(filter.lineFilter(b[i]).cstr());
      if(((i & CHECK_INTERVAL) == 0) && m_job) { m_job->setSubProgressPercent(SPERCENT(i,bsize)); }
    }
END_TIMEMEASURE(5, m_docSize[1]);

    fa.updateCapacity();
    fb.updateCapacity();

    findLcs(epa, fa, fb, cmp);
  }
  makeDiffLines(a, b, epa);
}

void Diff::findLcs(ElementPairArray &p, const LineArray &a, const LineArray &b, LcsComparator &cmp) {
//  if(a.size() + b.size() > 50000) {
    LcsSimple(a,b, m_docSize, cmp, m_job).findLcs(p);
//  } else {
//    LcsBitSet(a,b, cmp).findLcs(p);
//  }
}

void Diff::setDoc(int id, DiffDocType type, const TCHAR *arg) {
  switch(type) {
  case DIFFDOC_FILE :
    m_doc[id].setToFile(arg);
    break;
  case DIFFDOC_BUF  :
    m_doc[id].setToBuf(arg);
    break;
  }
}

void Diff::setDoc(int id, const DiffDoc &doc) {
  setDoc(id, doc.getType(), doc.getArg().cstr());
}

void Diff::compare(DiffFilter &filter, LcsComparator &cmp, CompareJob *job) {
  m_job = job;
  StringPool sp;
  LineArray s0(sp), s1(sp);

  getDoc(0).getLines(filter, s0, m_job);
  getDoc(1).getLines(filter, s1, m_job);
  compare(s0, s1, filter, cmp);

  m_job = NULL;
}

void Diff::compareBuffers(const TCHAR *b1, const TCHAR *b2, DiffFilter &filter, LcsComparator &cmp, CompareJob *job) {
  setDoc(0,DIFFDOC_BUF, b1);
  setDoc(1,DIFFDOC_BUF, b2);
  compare(filter, cmp, job);
}

void Diff::compareFiles(const TCHAR *fname1, const TCHAR *fname2, DiffFilter &filter, LcsComparator &cmp, CompareJob *job) {
  setDoc(0,DIFFDOC_FILE, fname1);
  setDoc(1,DIFFDOC_FILE, fname2);
  compare(filter, cmp, job);
}

bool Diff::refreshLines(CompareJob *job) {
  m_job = job;
  if(isEmpty()) {
    return false;
  }
  StringPool sp;
  LineArray s0(sp), s1(sp);

  getDoc(0).getLines(*m_diffFilter, s0, m_job);
  getDoc(1).getLines(*m_diffFilter, s1, m_job);
  makeDiffLines(s0, s1);

  m_job = NULL;
  return true;
}

void Diff::clear() { 
  m_lines.clear();
  m_stringPool.clear();
  m_maxLineLength      = 0;
  m_firstDiffLine      = 0;
  m_lastDiffLine       = 0;
  m_enableAddDiffLines = true;
}

void Diff::init() {
  m_tabSize        = 4;
  m_viewWhiteSpace = false;
  m_diffFilter     = NULL;
  m_job            = NULL;
  clear();
}

void Diff::swapDocs() {
  DiffDoc tmp = m_doc[0];
  m_doc[0] = m_doc[1];
  m_doc[1] = tmp;
  size_t n = m_lines.size();
  if(n) {
    for(DiffLine *dfp = &m_lines[0];n--;) {
      (dfp++)->swap();
    }
  }
}

int Diff::findLineIndex(int id, UINT line) const {
  const int n = (int)m_lines.size();
  if(n) {
    const DiffLine *dfp = &m_lines[0];
    for(int i = 0; i < n; i++) {
      if((dfp++)->getText(id).getIndex() == line) {;
        return i;
      }
    }
  }
  return -1;
}

Diff::Diff() {
  init();
}

Diff::Diff(const TCHAR *fname1, const TCHAR *fname2, CompareJob *job) {
  init();
  compareFiles(fname1,fname2, *stdDiffFilter, *stdLcsComparator, job);
}

Diff::~Diff() {
}

bool Diff::setTabSize(int newValue, bool refresh) {
  if(newValue == m_tabSize) {
    return false;
  }
  m_tabSize = newValue;
  if(refresh) {
    refreshLines(NULL);
  }
  return true;
}
  
bool Diff::setViewWhiteSpace(bool newValue, bool refresh) {
  if(newValue == m_viewWhiteSpace) {
    return false;
  }
  m_viewWhiteSpace = newValue;
  if(refresh) {
    refreshLines(NULL);
  }
  return true;
}

class StdDiffFilter : public DiffFilter {
  String lineFilter(const TCHAR *s) const {
    return s;
  }
  String docFilter(const TCHAR *s, CompareJob *job) const {
    return s;
  }
  bool hasLineFilter() const {
    return false;
  };
  bool hasDocFilter() const {
    return false;
  }
};

static StdDiffFilter stdfilter;
DiffFilter *stdDiffFilter = &stdfilter;
