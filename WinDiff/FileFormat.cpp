#include "stdafx.h"
#include "FileFormat.h"
#include <Tokenizer.h>
#include <Exception.h>

RegistryKey FileFormat::getRootKey() { // static
  return Options::getSubKey(_T("Fileformats"));
}

FileFormat::FileFormat() {
  clear();
}

void FileFormat::clear() {
  m_name               = _T("Untitled");
  m_delimited          = false;
  m_textQualifier      = _T('\"');
  m_multipleDelimiters = false;
}

bool FileFormat::save() const {
  if(!validate()) {
    return false;
  }
  try {
    saveData(getRootKey().createOrOpenKey(m_name), *this);
  } catch(Exception e) {
    showException(e);
    return false;
  }
  return true;
}

bool FileFormat::load(const String &name) {
  try {
    loadData(getRootKey().createOrOpenKey(name), *this);
    m_name = name;
  } catch(Exception e) {
    showException(e);
    return false;
  }
  return true;
}

static const TCHAR *DELIMITED          = _T("Delimited");
static const TCHAR *FIELDDELIMITERS    = _T("FieldDelimiters");
static const TCHAR *MULTIPLEDELIMITERS = _T("MultipleDelimiters");
static const TCHAR *TEXTQUALIFIER      = _T("TextQualifier");
static const TCHAR *COLUMNS            = _T("Columns");

void FileFormat::saveData(RegistryKey &key, const FileFormat &param) { // static
  key.setValue(DELIMITED          , param.m_delimited                  );
  key.setValue(FIELDDELIMITERS    , expandEscape(param.m_delimiters)   );
  key.setValue(MULTIPLEDELIMITERS , param.m_multipleDelimiters         );
  key.setValue(TEXTQUALIFIER      , format(_T("%c"), param.m_textQualifier));
  key.setValue(COLUMNS            , param.columnsToString()            );
}

void FileFormat::loadData(RegistryKey &key, FileFormat &param) { // static
  const FileFormat defaultValues;
  param.m_delimited          =               key.getBool(  DELIMITED          , defaultValues.m_delimited);
  param.m_delimiters         = convertEscape(key.getString(FIELDDELIMITERS    , defaultValues.m_delimiters));
  param.m_multipleDelimiters =               key.getBool(  MULTIPLEDELIMITERS , defaultValues.m_multipleDelimiters );
  const String txtq          =               key.getString(TEXTQUALIFIER      , format(_T("%c"), defaultValues.m_textQualifier));
  param.m_textQualifier      = txtq.length() ? txtq[0] : _T('\"');
  param.columnsFromString(                   key.getString(COLUMNS            ,EMPTYSTRING     ));
}

bool FileFormat::remove(const String &name) { // static
  try {
    RegistryKey root = getRootKey();
    root.deleteKey(name);
  } catch(Exception e) {
    showException(e);
    return false;
  }
  return true;
}

StringArray FileFormat::getExistingNames() { // static
  StringArray result;
  RegistryKey root = getRootKey();

  for(Iterator<String> it = root.getSubKeyIterator(); it.hasNext(); ) {
    result.add(it.next());
  }
  return result;
}

static int fileFormatNameCmp(const FileFormat &c1, const FileFormat &c2) {
  return _tcscmp(c1.m_name.cstr(), c2.m_name.cstr());
}

Array<FileFormat> FileFormat::getExisting() { // static
  Array<FileFormat> result;
  RegistryKey root = getRootKey();

  for(Iterator<String> it = root.getSubKeyIterator(); it.hasNext(); ) {
    FileFormat tmp;
    tmp.load(it.next());
    result.add(tmp);
  }
  result.sort(fileFormatNameCmp);
  return result;
}

void FileFormat::columnsFromString(const String &s) {
  m_columns.clear();

  for(Tokenizer tok(s,_T(",")); tok.hasNext();) {
    try {
      addInterval(ColumnInterval(tok.next()));
    } catch(Exception) {
    }
  }
}

String FileFormat::columnsToString() const {
  String result;
  for(size_t i = 0; i < m_columns.size(); i++) {
    if(i > 0) {
      result += _T(",");
    }
    result += m_columns[i].toString();
  }
  return result;
}

String ColumnInterval::toString() const {
  return format(_T("%3d - %3d"),m_from,m_to);
}

ColumnInterval::ColumnInterval(int from, int to) {
  if(from < 1 || to < from) {
    throwException(_T("Illegal interval (%d,%d)"),from,to);
  }

  m_from = from;
  m_to   = to;
}

ColumnInterval::ColumnInterval(const String &s) {
  if(_stscanf(s.cstr(),_T("%d - %d"),&m_from, &m_to) != 2 || m_from < 1 || m_to < m_from) {
    throwException(_T("Illegal interval (%s)"),s.cstr());
  }
}

bool ColumnInterval::overlap(const ColumnInterval &ci) const {
  return m_from <= ci.m_to && m_to >= ci.m_from;
}

static int intervalCmp(const ColumnInterval &ci1,const ColumnInterval &ci2) {
  return ci1.m_from - ci2.m_from;
}

bool ColumnInterval::operator==(const ColumnInterval &ci) const {
  return m_from == ci.m_from && m_to == ci.m_to;
}

void FileFormat::addInterval(const ColumnInterval &ci) {
  for(size_t i = 0; i < m_columns.size(); i++) {
    if(m_columns[i].overlap(ci)) {
      throwException(_T("%s"),_T("This interval overlaps another interval"));
    }
  }
  m_columns.add(ci);
  m_columns.sort(intervalCmp);
}

bool FileFormat::validate() const {
  if(m_columns.size() == 0) {
    showWarning(_T("No columns specified"));
    return false;
  }
  if(m_delimited) {
    if(m_delimiters.length() == 0) {
      showWarning(_T("Must specify fielddelimiters"));
      return false;
    }
    if((m_textQualifier != 0) && m_delimiters.find(m_textQualifier) >= 0) {
      showWarning(_T("The textqualifier is member of the set of fielddelimiters"));
      return false;
    }
  }
  return true;
}

bool FileFormat::operator==(const FileFormat &cp) const {
  if(m_delimited          != cp.m_delimited         ) return false;
  if(m_delimiters         != cp.m_delimiters        ) return false;
  if(m_textQualifier      != cp.m_textQualifier     ) return false;
  if(m_multipleDelimiters != cp.m_multipleDelimiters) return false;
  if(m_columns            != cp.m_columns           ) return false;
  return true;
}

String FileFormat::concatFields(const StringArray &fields) const { // assume m_columns.size > 0
  String result;
  UINT from = 0;
  UINT to   = min((UINT)fields.size(), m_columns[0].m_from);
  for(UINT f = from; f < to; f++) {
    result += fields[f];
  }
  for(size_t c = 1; c < m_columns.size(); c++) {
    from = m_columns[c-1].m_to+1;
    to   = min((UINT)fields.size(), m_columns[c].m_from);
    for(UINT f = from; f < to; f++) {
      result += fields[f];
    }
  }
  from = m_columns.last().m_to + 1;
  to   = (UINT)fields.size();
  for(UINT f = from; f < to; f++) {
    result += fields[f];
  }
  return result;
}

String FileFormat::stripColumns(const String &s) const {
  if(m_columns.size() == 0) {
    return s;
  }

  String result;

  if(m_delimited) {
    StringArray fields;
    fields.add(EMPTYSTRING); // add dummystring to make real fields indexed from 1... as m_columns are
    Tokenizer tok(s.cstr(),m_delimiters.cstr(),m_textQualifier,m_multipleDelimiters?0:TOK_SINGLEDELIMITERS);
    while(tok.hasNext()) {
      fields.add(tok.next());
    }
    result = concatFields(fields);
  } else { // fixed width
    int l    = (int)s.length();
    int from = 0;
    int to   = min(l, (int)m_columns[0].m_from-1);
    result += substr(s,from,to - from);
    for(size_t c = 1; c < m_columns.size(); c++) {
      from = m_columns[c-1].m_to;
      to   = min(l,(int)m_columns[c].m_from-1);
      result += substr(s,from,to-from);
    }
    from = m_columns.last().m_to;
    to   = l;
    result += substr(s,from,to-from);
  }
  return result;
}

// --------------------------------------------------------------------------


FileFormatList::FileFormatList() {
  const Array<FileFormat> ffArray = FileFormat::getExisting();
  for(size_t i = 0; i < ffArray.size(); i++) {
    const FileFormat &ff = ffArray[i];
    add(FileFormatEditData(ff.m_name));
  }
}

bool FileFormatList::save() {
  Array<FileFormat> existing = FileFormat::getExisting();

  for(size_t i = 0; i < existing.size(); i++) {
    const FileFormat &ff = existing[i];
    FileFormat::remove(ff.m_name);
  }
  for(size_t i = 0; i < size(); i++) {
    const FileFormatEditData &ffi = (*this)[i];
    for(size_t j = 0; j < existing.size(); j++) {
      FileFormat &ffj = existing[j];
      if(ffi.m_originalName == ffj.m_name) {
        ffj.m_name  = ffi.m_name;
        ffj.save();
        break;
      }
    }
  }
  return true;
}

bool FileFormatList::validate(int &errorLine) const {
  for(size_t i = 0; i < size(); i++) {
    const FileFormatEditData &ffi = (*this)[i];
    if(trim(ffi.m_name).length() == 0) {
      showWarning(_T("Invalid name for fileformat:\"%s\""), ffi.m_name.cstr());
      errorLine = (int)i;
      return false;
    }
    for(size_t j = 0; j < i; j++) {
      const FileFormatEditData &ffj = (*this)[j];
      if(ffj.m_name == ffi.m_name) {
        showWarning(_T("Name %s used more than once"), ffi.m_name.cstr());
        errorLine = (int)j;
        return false;
      }
    }
  }
  errorLine = -1;
  return true;
}

bool FileFormatList::remove(UINT index) {
  removeIndex(index);
  return true;
}

FileFormatEditData::FileFormatEditData(const String &name)
: m_name(name)
, m_originalName(name)
{
}

bool FileFormatEditData::operator==(const FileFormatEditData &fd) const {
  return fd.m_name         == m_name
      && fd.m_originalName == m_originalName;
}
