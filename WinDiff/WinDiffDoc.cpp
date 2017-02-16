#include "stdafx.h"
#include "MainFrm.h"
#include <MFCUtil/ProgressWindow.h>
#include "WinDiff.h"
#include "WinDiffDoc.h"
#include "CompareJob.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CWinDiffDoc, CDocument)

BEGIN_MESSAGE_MAP(CWinDiffDoc, CDocument)
END_MESSAGE_MAP()

int WinDiffComparator::compare(const TCHAR * const &s1, const TCHAR * const &s2) {
  if(m_ignoreCase) {
    return streicmp(s1, s2);
  } else {
    return _tcscmp(s1, s2);
  }
}

WinDiffComparator::WinDiffComparator() {
  m_ignoreCase = false;
}

#define RETURN_CHANGED(dst, src)  if(dst == src) return false; dst = src; return true

bool WinDiffComparator::setIgnoreCase(bool newValue) {
  RETURN_CHANGED(m_ignoreCase, newValue);
}

#define TIMEFACTOR_READFILES         5.0e-2
#define TIMEFACTOR_SORT              6.8e-3
#define TIMEFACTOR_MATCHLINES        1.1e-2
#define TIMEFACTOR_COMPARING         2.5e-2
#define TIMEFACTOR_BUILDPAIRS        3.0e-3
#define TIMEFACTOR_TABEXPAND         2.3e-2
#define TIMEFACTOR_BUILDFINALLINES   4.0e-2

double WinDiffComparator::getSortTimeFactor() const {
  return m_ignoreCase ? (1.8 * TIMEFACTOR_SORT) : TIMEFACTOR_SORT;
}

double WinDiffComparator::getMatchTimeFactor() const {
  return m_ignoreCase ? (1.8 * TIMEFACTOR_MATCHLINES) : TIMEFACTOR_MATCHLINES;
}

String WinDiffComparator::toString() const {
  return m_ignoreCase ? _T("IGNORE CASE") : _T("CASE SENSITIVE");
}

WinDiffFilter::WinDiffFilter() {
  m_flags = 0;
}

#define RETURN_FLAGSCHANGED(flag, b)              \
{ const UINT old = m_flags;               \
  if(b) m_flags |= flag; else m_flags &= ~(flag); \
  return m_flags != old;                          \
}

bool WinDiffFilter::setIgnoreWhiteSpace(bool newValue) {
  RETURN_FLAGSCHANGED(FLAG_IGNORE_WHITEPSACE, newValue);
}

bool WinDiffFilter::setIgnoreComments(bool newValue) {
  RETURN_FLAGSCHANGED(FLAG_IGNORE_COMMENTS, newValue);
}

bool WinDiffFilter::setStripComments(bool newValue) {
  RETURN_FLAGSCHANGED(FLAG_STRIP_COMMENTS, newValue);
}

bool WinDiffFilter::setIgnoreStrings(bool newValue) {
  RETURN_FLAGSCHANGED(FLAG_IGNORE_STRINGS, newValue);
}

bool WinDiffFilter::setFileFormat(const FileFormat *fileFormat) {
  bool changed = false;
  if(fileFormat) {
    if(m_fileFormat != *fileFormat) {
      m_fileFormat = *fileFormat;
      changed = true;
    }
  } else if(!m_fileFormat.isEmpty()) {
    m_fileFormat.clear();
    changed = true;
  }
  if(!m_fileFormat.isEmpty()) { 
    m_flags |= FLAG_IGNORE_COLUMNS;
  } else {
    m_flags &= ~FLAG_IGNORE_COLUMNS;
  }
  return changed;
}

bool WinDiffFilter::setRegexFilter(const RegexFilter *filter) {
  bool changed = false;
  if(filter) {
    if(m_regexFilter != *filter) {
      m_regexFilter = *filter;
      changed = true;
    }
  } else if(!m_regexFilter.isEmpty()) {
    m_regexFilter.clear();
    changed = true;
  }
  if(!m_regexFilter.isEmpty()) {
    m_flags |= FLAG_IGNORE_REGEX;
  } else {
    m_flags &= ~FLAG_IGNORE_REGEX;
  }
  return changed;
}

String WinDiffFilter::stripWhiteSpace(const String &s) { // static
  String result(s);
  TCHAR *dst = result.cstr();
  for(const TCHAR *cp = s.cstr(); *cp; cp++) {
    if(!_istspace(*cp)) {
      *(dst++) = *cp;
    }
  }
  *dst = 0;
  return result.cstr();
}

// Remove all //...\n comments
String WinDiffFilter::stripLineComments(const String &s) { // static
  int state = 0;
  String result(s);
  TCHAR *dst = result.cstr();
  for(const TCHAR *cp = s.cstr(); *cp; cp++) {
    switch(state) {
    case 0:                                      // initial state
      switch(*cp) {
      case _T('"') : state = 1;        break;
      case _T('\''): state = 3;        break;
      case _T('/') : state = 5;        break;
      default  :                       break;
      }
      break;

    case 1:                                      // inside double-quoted string literal
      switch(*cp) {
      case _T('\\'): state = 2;        break;    // escape-character inside double-quoted string literal
      case _T('"') : 
      case NL      : state = 0;        break;    // exit double-quoted string literal
      default  :                       break;    // stay inside double-quoted string literal
      }
      break;

    case 2:                                      // after \ inside double-quoted string literal
      switch(*cp) {
      case NL      : state = 0;        break;
      default      : state = 1;        break;
      }
      break;

    case 3:                                      // inside single-quoted string literal
      switch(*cp) {
      case _T('\\'): state = 4;        break;    // escape-character inside single-quoted string listeral
      case _T('\''): 
      case NL      : state = 0;        break;    // exit single-quoted string literal
      default      :                   break;    // stay inside single-quoted string literal
      }
      break;

    case 4:                                      // after \ inside a single-quoted string literal
      switch(*cp) {
      case NL      : state = 0;        break;
      default      : state = 3;        break;
      }
      break;

    case 5:                                      // just read /
      switch(*cp) {
      case _T('/'):  state = 6; dst--; continue; // enter //... comment
      case _T('*'):  state = 7;        break;    // enter /* ... */ comment
      default     :  state = 0;        break;
      }
      break;

    case 6:                                      // inside //.... comment
      if(*cp == NL) {                            // end of comment
        state = 0;
        break;
      }
      continue;

    case 7:                                      // inside /* .... */ comment
      if(*cp == _T('*')) {
        state = 8;
      }
      break;

    case 8:                                      // after * inside /* ... */ comment, 
      if(*cp == _T('/')) {                       // end of comment
        state = 0; 
      } else {
        state = 7;
      }
      break;

    default:
      throwException(_T("Invalid state:%d: Current input:%20.20s"), state, cp);
    }
    *(dst++) = *cp;
  }
  *dst = 0;
  return result.cstr();
}

/* Remove all "..." and '  ' strings, taking care of escape-sequences like \",\' and \\ */
String WinDiffFilter::stripStrings(const String &s) { // static
  int state = 0;
  String result(s);
  TCHAR *dst = result.cstr();
  for(const TCHAR *cp = s.cstr(); *cp; cp++) {
    switch(state) {
    case 0:                               // initial state
      switch(*cp) {
      case _T('"') : state = 1; continue;
      case _T('\''): state = 3; continue;
      case _T('/') : state = 5; break;
      default      :            break;
      }
      break;

    case 1:                               // inside double-quoted string literal
      switch(*cp) {
      case _T('\\'): state = 2; continue; // escape-character inside double-quoted string literal
      case _T('"') : 
      case NL      : state = 0; continue; // exit double-quoted string literal
      default      :            continue; // stay inside double-quoted string literal
      }
      break;

    case 2:                               // after \ inside double-quoted string literal
      switch(*cp) {
      case NL      : state = 0; continue;
      default      : state = 1; continue;
      }
      break;

    case 3:                               // inside single-quoted string literal
      switch(*cp) {
      case _T('\\'): state = 4; continue; // escape-character inside single-quoted string listeral
      case _T('\''): 
      case NL      : state = 0; continue; // exit single-quoted string literal
      default      :            continue; // stay inside single-quoted string literal
      }
      break;

    case 4:                               // after \ inside a single-quoted string literal
      switch(*cp) {
      case NL      : state = 0; continue;
      default      : state = 3; continue;
      }
      break;

    case 5:                               // just read /
      switch(*cp) {
      case _T('/'):  state = 6; break;    // enter //... comment
      case _T('*'):  state = 7; break;    // enter /* ... */ comment
      default     :  state = 0; break;
      }
      break;

    case 6:                               // inside //.... comment
      if(*cp == NL) {                     // end of comment
        state = 0;
      }
      break;

    case 7:                               // inside /* .... */ comment
      if(*cp == _T('*')) {
        state = 8;
      }
      break;

    case 8:                               // after * inside /* ... */ comment, 
      if(*cp == _T('/')) {                // end of comment
        state = 0; 
      } else {
        state = 7;
      }
      break;

    default:
      throwException(_T("Invalid state:%d: Current input:%20.20s"), state, cp);
    }
    *(dst++) = *cp;
  }
  *dst = 0;
  return result.cstr();
}

String WinDiffFilter::lineFilter(const TCHAR *s) const {
  String result(s);
  if(m_flags & FLAG_IGNORE_COLUMNS   ) result = m_fileFormat.stripColumns(result);
  if(m_flags & FLAG_IGNORE_REGEX     ) result = m_regexFilter.filter(result);
  if(m_flags & FLAG_IGNORE_WHITEPSACE) result = stripWhiteSpace(result);
  if(m_flags & FLAG_IGNORE_COMMENTS  ) result = stripLineComments(result);
  if(m_flags & FLAG_IGNORE_STRINGS   ) result = stripStrings(result);
  return result;
}

double WinDiffFilter::getTimeFactor() const {
  double factor = 0;
  if(m_flags & FLAG_IGNORE_COLUMNS   ) factor += 0.1;
  if(m_flags & FLAG_IGNORE_REGEX     ) factor += 0.1;
  if(m_flags & FLAG_IGNORE_WHITEPSACE) factor += 0.085;
  if(m_flags & FLAG_IGNORE_COMMENTS  ) factor += 0.06;
  if(m_flags & FLAG_IGNORE_STRINGS   ) factor += 0.075;
  return factor;
}

String WinDiffFilter::toString() const {
  String result;
  if(m_flags & FLAG_IGNORE_COLUMNS   ) result += _T("COLUMNS ");
  if(m_flags & FLAG_IGNORE_REGEX     ) result += _T("REGEX   ");
  if(m_flags & FLAG_IGNORE_WHITEPSACE) result += _T("SPACES  ");
  if(m_flags & FLAG_IGNORE_COMMENTS  ) result += _T("COMMENT ");
  if(m_flags & FLAG_IGNORE_STRINGS   ) result += _T("STRINGS" );
  return result;
}

// Remove all /* ... */ comments
String WinDiffFilter::stripMultilineComments(const String &s) { // static.
  int state = 0;
  String result(s);
  TCHAR *dst = result.cstr();
  for(const TCHAR *start = s.cstr(), *cp = start; *cp; cp++) {
    switch(state) {
    case 0:                                   // initial state
      switch(*cp) {
      case _T('"') : state = 1;        break;
      case _T('\''): state = 3;        break;
      case _T('/') : state = 5;        break;
      default      :                   break;
      }
      break;

    case 1:                                       // inside double-quoted string literal
      switch(*cp) {
      case _T('\\'): state = 2;        break;     // escape-character inside double-quoted string literal
      case _T('"') : 
      case NL      : state = 0;        break;     // exit double-quoted string literal
      default      :                   break;     // stay inside double-quoted string literal
      }
      break;

    case 2:                                       // after \ inside double-quoted string literal
      switch(*cp) {
      case NL      : state = 0;        break;
      default      : state = 1;        break;
      }
      break;

    case 3:                                       // inside single-quoted string literal
      switch(*cp) {
      case _T('\\'): state = 4;        break;     // escape-character inside single-quoted string listeral
      case _T('\''): 
      case NL      : state = 0;        break;     // exit single-quoted string literal
      default      :                   break;     // stay inside single-quoted string literal
      }
      break;

    case 4:                                       // after \ inside a single-quoted string literal
      switch(*cp) {
      case NL      : state = 0;        break;
      default      : state = 3;        break;
      }
      break;

    case 5:                                      // just read /
      switch(*cp) {
      case _T('/'):  state = 6;        break;    // enter //... comment
      case _T('*'):  state = 7; dst--; continue; // remove last added character ('/') and enter /* ... */ comment, 
      default     :  state = 0;        break;
      }
      break;

    case 6:                                      // inside //.... comment
      if(*cp == NL) {                            // end of comment
        state = 0;
      }
      break;

    case 7:                                      // inside /* .... */ comment
      if(*cp == _T('*')) {
        state = 8;
      }
      continue;                                  // don't add anything to result

    case 8:                                      // after * inside /* ... */ comment, 
      if(*cp == _T('/')) {                       // end of comment
        state = 0;
      } else {
        state = 7;
      }
      continue;                                  // don't add anything to result

    default:
      throwException(_T("Invalid state:%d: Current input:%20.20s"), state, cp);
    }
    *(dst++) = *cp;
  }
  *dst = 0;
  return result.cstr();
}

String WinDiffFilter::docFilter(const TCHAR *s) const {
  if(m_flags & FLAG_STRIP_COMMENTS) {
    return stripMultilineComments(s);
  } else {
    return s;
  }
}

CWinDiffDoc::CWinDiffDoc() {
  initOptions(getOptions());
  TCHAR **argv = __targv;
  argv++;
  TCHAR *f1 = *argv ? *(argv++) : NULL;
  TCHAR *f2 = *argv ? *(argv++) : NULL;
  if(f1 && f2) {
    setDocs(FileNameSplitter(f1).getAbsolutePath(),FileNameSplitter(f2).getAbsolutePath());
  } else if(f1) {
    setDoc(0,DIFFDOC_FILE, FileNameSplitter(f1).getAbsolutePath());
  }
}

void CWinDiffDoc::initOptions(const Options &options) {
  setIgnoreWhiteSpace(options.m_ignoreWhiteSpace                              , false);
  setIgnoreCase(      options.m_ignoreCase                                    , false);
  setIgnoreStrings(   options.m_ignoreStrings                                 , false);
  setIgnoreComments(  options.m_ignoreComments                                , false);
  setFileFormat(      options.m_ignoreColumns ? &options.m_fileFormat  : NULL , false);
  setRegexFilter(     options.m_ignoreRegex   ? &options.m_regexFilter : NULL , false);
  setStripComments(   options.m_stripComments                                 , false);
  setViewWhiteSpace(  options.m_viewWhiteSpace                                , false);
  setTabSize(         options.m_tabSize                                       , false);
}

void CWinDiffDoc::setDoc(int id, DiffDocType type, const String &arg, bool recomp) {
  DiffDoc oldDoc = m_diff.getDoc(id);
  m_diff.setDoc(id,type,arg.cstr());
  if(recomp) {
    try {
      recompare();
      if(type == DIFFDOC_FILE && (arg != oldDoc.getName() || oldDoc.getType() != DIFFDOC_FILE))  {
        theApp.addToRecentFileList(arg.cstr());
      }
    } catch(...) {
      m_diff.setDoc(id, oldDoc);
      recompare();
      throw;
    }
  }
}

int CWinDiffDoc::getDocSize(int id) const {
  return m_diff.getDoc(id).getSize();
}

void CWinDiffDoc::setDocs(const String &name1, const String &name2) { // always files
  DiffDoc oldDoc0 = m_diff.getDoc(0);
  DiffDoc oldDoc1 = m_diff.getDoc(1);

  setDoc(0, DIFFDOC_FILE, name1, false);
  setDoc(1, DIFFDOC_FILE, name2, false);
  try {
    recompare();
    if(m_diff.getDoc(0).getLastReadTime()) {
      theApp.addToRecentFileList(name1.cstr());
    }
    if(m_diff.getDoc(1).getLastReadTime()) {
      theApp.addToRecentFileList(name2.cstr());
    }
  } catch(CMemoryException *e) {
    TCHAR msg[1024];
    e->GetErrorMessage(msg, ARRAYSIZE(msg));
    showError(_T("%s"), msg);
    return;
  } catch(...) {
    m_diff.setDoc(0,oldDoc0);
    m_diff.setDoc(1,oldDoc1);
    recompare();
  }
}

void CWinDiffDoc::recompare() {
  updateDiffDoc(true);
}

void CWinDiffDoc::refresh() {
  updateDiffDoc(m_diff.isDirty());
}

void CWinDiffDoc::updateDiffDoc(bool recompare) {
  RESET_TIMEMEASURE(format(_T("LineFilter:%s, CompareFilter:%s\n"), m_filter.toString().cstr(), m_cmp.toString().cstr()));

  CompareJob job(this, recompare);
  const UINT doc0Size   = getDocSize(0);
  const UINT doc1Size   = getDocSize(1);
  const UINT sumDocSize = doc0Size+doc1Size;
  const UINT maxDocSize = max(doc0Size,doc1Size);

  job.addStep(TIMEFACTOR_READFILES * sumDocSize, _T("Reading files...")            );

  if(recompare) {
    if(m_filter.hasLineFilter()) {
      job.addStep(m_filter.getTimeFactor() * maxDocSize, _T("Filter lines...")      );
    }

    job.addStep(m_cmp.getSortTimeFactor() * nlogn(maxDocSize), _T("Sorting docs..."));

    job.addStep(m_cmp.getMatchTimeFactor() * sumDocSize, _T("Finding matching lines...")        );
    job.addStep(TIMEFACTOR_COMPARING       * sumDocSize, _T("Comparing...")                     );
    job.addStep(TIMEFACTOR_BUILDPAIRS      * sumDocSize, _T("Building pairs...")                );
  }

  job.addStep(TIMEFACTOR_TABEXPAND       * doc0Size  , _T("Expanding tabs doc1...")             );
  job.addStep(TIMEFACTOR_TABEXPAND       * doc1Size  , _T("Expanding tabs doc2...")             );
  job.addStep(TIMEFACTOR_BUILDFINALLINES * (doc0Size + doc1Size)/2, _T("Building final lines.."));

  CWnd *mainWindow = theApp.GetMainWnd();
  if(mainWindow && !mainWindow->IsWindowVisible()) {
    mainWindow = NULL;
  }
  ProgressWindow(mainWindow, job, 1000, 200);
}

CWinDiffDoc::~CWinDiffDoc() {
}

BOOL CWinDiffDoc::OnNewDocument() {
  if(!CDocument::OnNewDocument()) {
    return FALSE;
  }

  // TODO: add reinitialization code here
  // (SDI documents will reuse this document)

  return TRUE;
}

void CWinDiffDoc::Serialize(CArchive& ar) {
  if (ar.IsStoring()) {
  } else {
  }
}

#ifdef _DEBUG
void CWinDiffDoc::AssertValid() const {
    CDocument::AssertValid();
}

void CWinDiffDoc::Dump(CDumpContext& dc) const {
    CDocument::Dump(dc);
}
#endif //_DEBUG

bool CWinDiffDoc::setIgnoreWhiteSpace(bool newValue, bool recomp) {
  const bool changed = m_filter.setIgnoreWhiteSpace(newValue);
  if(changed && recomp) {
    recompare();
  }
  getOptions().m_ignoreWhiteSpace = newValue;
  return changed;
}

bool CWinDiffDoc::setIgnoreCase(bool newValue, bool recomp) {
  const bool changed = m_cmp.setIgnoreCase(newValue);
  if(changed && recomp) {
    recompare();
  }
  getOptions().m_ignoreCase = newValue;
  return changed;
}

bool CWinDiffDoc::setIgnoreStrings(bool newValue, bool recomp) {
  const bool changed = m_filter.setIgnoreStrings(newValue);
  if(changed && recomp) {
    recompare();
  }
  getOptions().m_ignoreStrings = newValue;
  return changed;
}

bool CWinDiffDoc::setIgnoreComments(bool newValue, bool recomp) {
  const bool changed = m_filter.setIgnoreComments(newValue);
  if(changed && recomp) {
    recompare();
  }
  getOptions().m_ignoreComments = newValue;
  return changed;
}

bool CWinDiffDoc::setFileFormat(const FileFormat *colparam, bool recomp) {
  const bool changed = m_filter.setFileFormat(colparam);
  if(changed && recomp) {
    recompare();
  }
  getOptions().m_ignoreColumns = (colparam != NULL);
  return changed;
}

bool CWinDiffDoc::setRegexFilter(const RegexFilter *filter, bool recomp) {
  const bool changed = m_filter.setRegexFilter(filter);
  if(changed && recomp) {
    recompare();
  }
  getOptions().m_ignoreRegex = (filter != NULL);
  return changed;
}

bool CWinDiffDoc::setStripComments(bool newValue, bool recomp) {
  const bool changed = m_filter.setStripComments(newValue);
  if(changed && recomp) {
    recompare();
  }
  getOptions().m_stripComments = newValue;
  return changed;
}

bool CWinDiffDoc::setViewWhiteSpace(bool newValue, bool update) {
  getOptions().m_viewWhiteSpace = newValue;
  const bool changed = m_diff.setViewWhiteSpace(newValue, false);
  if(update) {
    refresh();
  }
  return changed;
}

bool CWinDiffDoc::setTabSize(int newValue, bool update) {
  getOptions().m_tabSize = newValue;
  const bool changed = m_diff.setTabSize(newValue, false);
  if(update) {
    refresh();
  }
  return changed;
}
