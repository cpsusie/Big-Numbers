#include "stdafx.h"
#include "Options.h"

const TCHAR *Options::rootPath =  _T("JGMData\\WinDiff");

RegistryKey Options::getRootKey() { // static 
  return RegistryKey(HKEY_CURRENT_USER, _T("Software")).createOrOpenPath(rootPath);
}

RegistryKey Options::getOptionsRootKey() { // static
  return getSubKey(_T("Settings"));
}

RegistryKey Options::getOptionsSubKey(const String &name) { // static
  return getOptionsRootKey().createOrOpenKey(name);
}

Options::Options() {
  setDefault();
}

void Options::setDefault() {
  m_index                 = -1;
  m_name                  = _T("Default");
  m_ignoreWhiteSpace      = false;
  m_ignoreCase            = false;
  m_ignoreStrings         = false;
  m_ignoreComments        = false;
  m_ignoreColumns         = false;
  m_ignoreRegex           = false;
  m_stripComments         = false;
  m_viewWhiteSpace        = false;
  m_tabSize               = 4;
  m_defaultExtensionIndex = 0;
  m_show1000Separator     = false;
  m_windowSize            = CSize(1200,625);
  m_logFont               = getDefaultLogFont();
  m_nameFontSizePct       = 100;
}

String Options::getKeyName() const {
  return getKeyName(m_index);
}

String Options::getKeyName(int index) { // static 
  return format(_T("%02d"), index);
}

bool Options::save() {
  if(!validate()) {
    return false;
  }
  try {
    m_index = getNameToIndex(m_name);
    if(m_index < 0) {
      m_index = getNextIndex();
    }
    saveData(getOptionsSubKey(getKeyName()), *this);
  } catch(Exception e) {
    showException(e);
    return false;
  }
  return true;
}

bool Options::load(const String &name) {
  return load(getNameToIndex(name));
}

bool Options::remove(const String &name) { // static
  return remove(getNameToIndex(name));
}

bool Options::load(int index) {
  try {
    loadData(getOptionsSubKey(getKeyName(index)), *this);
    m_index = index;
  } catch(Exception e) {
    showException(e);
    return false;
  }
  return true;
  
}

bool Options::remove(int index) { // static 
  try {
    RegistryKey optroot = getOptionsRootKey();
    optroot.deleteKey(getKeyName(index));
  } catch(Exception e) {
    showException(e);
    return false;
  }
  return true;
}

static const TCHAR *NAME               = EMPTYSTRING;
static const TCHAR *IGNOREWHITESPACE   = _T("IgnoreWhiteSpace");
static const TCHAR *IGNORECASE         = _T("IgnoreCase");
static const TCHAR *IGNORESTRINGS      = _T("IgnoreStrings");
static const TCHAR *IGNORECOMMENTS     = _T("IgnoreComments");
static const TCHAR *IGNORECOLUMNS      = _T("IgnoreColumns");
static const TCHAR *IGNOREREGEX        = _T("IgnoreRegexp");
static const TCHAR *STRIPCOMMENTS      = _T("StripComments");
static const TCHAR *VIEWWHITESPACE     = _T("ViewWhiteSpace");
static const TCHAR *TABSIZE            = _T("TabSize");
static const TCHAR *DEFAULTEXTENTION   = _T("DefaultExtention");
static const TCHAR *SHOW1000SEPARATOR  = _T("Show1000Separator");
static const TCHAR *WINDOWWIDTH        = _T("WindowWidth");
static const TCHAR *WINDOWHEIGHT       = _T("WindowHeight");
static const TCHAR *NAMEFONTSIZE       = _T("NameFontSize");
static const TCHAR *FONTDEF            = _T("Font");
static const TCHAR *DEFAULTNAME        = _T("Default options");

void Options::saveData(RegistryKey &key, const Options &options) { // static
  key.setValue(NAME               , options.m_name                    );
  key.setValue(IGNOREWHITESPACE   , options.m_ignoreWhiteSpace        );
  key.setValue(IGNORECASE         , options.m_ignoreCase              );
  key.setValue(IGNORESTRINGS      , options.m_ignoreStrings           );
  key.setValue(IGNORECOMMENTS     , options.m_ignoreComments          );
  key.setValue(IGNORECOLUMNS      , options.m_ignoreColumns           );
  key.setValue(IGNOREREGEX        , options.m_ignoreRegex             );
  key.setValue(STRIPCOMMENTS      , options.m_stripComments           );
  key.setValue(VIEWWHITESPACE     , options.m_viewWhiteSpace          );
  key.setValue(TABSIZE            , options.m_tabSize                 );
  key.setValue(DEFAULTEXTENTION   , options.m_defaultExtensionIndex   );
  key.setValue(SHOW1000SEPARATOR  , options.m_show1000Separator       );
  key.setValue(WINDOWWIDTH        , options.m_windowSize.cx           );
  key.setValue(WINDOWHEIGHT       , options.m_windowSize.cy           );
  key.setValue(NAMEFONTSIZE       , options.m_nameFontSizePct         );
  key.setValue(FONTDEF            , logFontToString(options.m_logFont));
  if(options.m_ignoreColumns) {
    FileFormat::saveData( key  , options.m_fileFormat );
  }
  if(options.m_ignoreRegex) {
    RegexFilter::saveData(key  , options.m_regexFilter     );
  }
}

void Options::loadData(RegistryKey &key, Options &options) { // static
  const Options defaultOptions;
  options.m_name                  = key.getString(NAME                 , defaultOptions.m_name                  );
  options.m_ignoreWhiteSpace      = key.getBool(  IGNOREWHITESPACE     , defaultOptions.m_ignoreWhiteSpace      );
  options.m_ignoreCase            = key.getBool(  IGNORECASE           , defaultOptions.m_ignoreCase            );
  options.m_ignoreStrings         = key.getBool(  IGNORESTRINGS        , defaultOptions.m_ignoreStrings         );
  options.m_ignoreComments        = key.getBool(  IGNORECOMMENTS       , defaultOptions.m_ignoreComments        );
  options.m_ignoreColumns         = key.getBool(  IGNORECOLUMNS        , defaultOptions.m_ignoreColumns         );
  options.m_ignoreRegex           = key.getBool(  IGNOREREGEX          , defaultOptions.m_ignoreRegex           );
  options.m_stripComments         = key.getBool(  STRIPCOMMENTS        , defaultOptions.m_stripComments         );
  options.m_viewWhiteSpace        = key.getBool(  VIEWWHITESPACE       , defaultOptions.m_viewWhiteSpace        );
  options.m_tabSize               = key.getInt(   TABSIZE              , defaultOptions.m_tabSize               );
  options.m_defaultExtensionIndex = key.getInt(   DEFAULTEXTENTION     , defaultOptions.m_defaultExtensionIndex );
  options.m_show1000Separator     = key.getBool(  SHOW1000SEPARATOR    , defaultOptions.m_show1000Separator     );
  options.m_windowSize.cx         = key.getInt(   WINDOWWIDTH          , defaultOptions.m_windowSize.cx         );
  options.m_windowSize.cy         = key.getInt(   WINDOWHEIGHT         , defaultOptions.m_windowSize.cy         );
  options.m_nameFontSizePct       = key.getInt(   NAMEFONTSIZE         , defaultOptions.m_nameFontSizePct       );
  options.m_nameFontSizePct       = minMax(options.m_nameFontSizePct, 100, 200);
  
  const String logFontStr         = key.getString(FONTDEF              , "");
  try {
    options.m_logFont = logFontFromString(logFontStr);
  } catch(...) {
    options.m_logFont = defaultOptions.getDefaultLogFont();
  }

  if(options.m_ignoreColumns) {
    FileFormat::loadData(key, options.m_fileFormat);
  }
  if(options.m_ignoreRegex) {
    try {
      RegexFilter::loadData(key, options.m_regexFilter);
    } catch(...) {
      options.m_ignoreCase = false;
    }
  }
}

bool Options::validate() const {
  if(m_name.length() == 0) {
    showWarning(_T("Must specify name"));
    return false;
  }
  return true;
}

int Options::getNameToIndex(const String &name) { // static 
  const Array<Options> optionsArray = getExisting();
  for(size_t i = 0; i < optionsArray.size(); i++) {
    const Options &opt = optionsArray[i];
    if(opt.m_name == name) {
      return opt.m_index;
    }
  }
  return -1;
}
  
StringArray Options::getExistingNames() { // static 
  const Array<Options> optArray = getExisting();
  StringArray result;

  for(size_t i = 0; i < optArray.size(); i++) {
    result.add(optArray[i].m_name);
  }
  return result;
}

static int optionsIndexCmp(const Options &opt1, const Options &opt2) {
  return opt1.m_index - opt2.m_index;
}

Array<Options> Options::getExisting() { // static
  Array<Options> result;
  RegistryKey root = getOptionsRootKey();

  for(Iterator<String> it = root.getSubKeyIterator(); it.hasNext(); ) {
    const int index = _ttoi(it.next().cstr());
    Options tmp;
    tmp.load(index);
    result.add(tmp);
  }
  result.sort(optionsIndexCmp);
  return result;
}

int Options::getNextIndex() { // static
  RegistryKey root = getOptionsRootKey();
  int maxExisting = -1;
  for(Iterator<String> it = root.getSubKeyIterator(); it.hasNext(); ) {
    const int index = _ttoi(it.next().cstr());
    if(index > maxExisting) {
      maxExisting = index;
    }
  }
  return maxExisting + 1;
}

void Options::setAsDefault() {
  setDefaultOptionName(m_name);
  save();
}

void Options::setDefaultOptionName(const String &name) { // static 
  getOptionsRootKey().setValue(DEFAULTNAME, name);
}

String Options::getDefaultOptionName() { // static
  return getOptionsRootKey().getString(DEFAULTNAME, EMPTYSTRING);
}

Options Options::getDefaultOptions() { // static
  const String defaultName = getDefaultOptionName();
  if(defaultName.length() > 0) {
    const Array<Options> optionList = getExisting();
    for(size_t i = 0; i < optionList.size(); i++) {
      const Options opt = optionList[i];
      if(opt.m_name == defaultName) {
        return opt;
      }
    }
  }
  Options result;
  return result;
}

const Options &OptionsAccessor::getOptions() const {
  return theApp.m_options;
}

Options &OptionsUpdater::getOptions() {
  return theApp.m_options;
}

// --------------------------------------------------------------------------


OptionList::OptionList() {
  const Array<Options> optArray = Options::getExisting();
  for(size_t i = 0; i < optArray.size(); i++) {
    const Options &opt = optArray[i];
    add(OptionsEditData(opt.m_name, opt.m_index));
  }
}

bool OptionList::save() {
  try {
    Array<Options> existing = Options::getExisting();

    const String defaultName = Options::getDefaultOptionName();
    String newDefaultName;

    for(size_t i = 0; i < existing.size(); i++) {
      const Options &opt = existing[i];
      Options::remove(opt.m_index);
    }
    for(size_t i = 0; i < size(); i++) {
      const OptionsEditData &optData = (*this)[i];
      if(optData.m_originalIndex < (int)existing.size()) {
        Options &opt = existing[optData.m_originalIndex];
        if(opt.m_name == defaultName) {
          newDefaultName = optData.m_name;
        }
        opt.m_name  = optData.m_name;
        opt.m_index = (int)i;
        opt.save();
      }
    }
    Options::setDefaultOptionName(newDefaultName);
    return true;
  } catch(Exception e) {
    showException(e);
    return false;
  }
}

bool OptionList::validate(int &errorLine) const {
  for(size_t i = 0; i < size(); i++) {
    const OptionsEditData &opti = (*this)[i];
    if(trim(opti.m_name).length() == 0) {
      showWarning(_T("Invalid name for options:\"%s\""), opti.m_name.cstr());
      errorLine = (int)i;
      return false;
    }
    for(size_t j = 0; j < i; j++) {
      const OptionsEditData &optj = (*this)[j];
      if(optj.m_name == opti.m_name) {
        showWarning(_T("Name %s used more than once"), opti.m_name.cstr());
        errorLine = (int)j;
        return false;
      }
    }
  }
  errorLine = -1;
  return true;
}

bool OptionList::move(UINT index, bool up) {
  if(index < 0) {
    return false;
  }
  if(up) {
    if(index == 0) {
      return false;
    }
    swap(index, index-1);
  } else {
    if(index >= size()-1) {
      return false;
    }
    swap(index, index+1);
  }
  return true;
}

bool OptionList::remove(UINT index) {
  removeIndex(index);
  return true;
}

OptionsEditData::OptionsEditData(const String &name, int index) 
: m_name(name)
, m_originalIndex(index)
{
}

static void vshowMessage(int id, const TCHAR *format, va_list argptr) {
  AfxMessageBox(vformat(format, argptr).cstr(), id);
}

void showWarning(const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  vshowMessage(MB_ICONWARNING, format, argptr);
  va_end(argptr);
}

void showError(const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  vshowMessage(MB_ICONERROR, format, argptr);
  va_end(argptr);
}

void showException(const Exception &e) {
  showWarning(_T("%s"), e.what());
}

#define FORMAT14 _T("%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%s")

String Options::logFontToString(const LOGFONT &lf) { // static
  return format(FORMAT14,
    lf.lfHeight,
    lf.lfWidth,
    lf.lfEscapement,
    lf.lfOrientation,
    lf.lfWeight,
    lf.lfItalic,
    lf.lfUnderline,
    lf.lfStrikeOut,
    lf.lfCharSet,
    lf.lfOutPrecision,
    lf.lfClipPrecision,
    lf.lfQuality,
    lf.lfPitchAndFamily,
    lf.lfFaceName
  );
}

typedef enum {
  HEIGHT
 ,WIDTH
 ,ESCAPEMENT
 ,ORIENTATION
 ,WEIGHT
 ,ITALIC
 ,UNDERLINE
 ,STRIKEOUT
 ,CHARSET
 ,OUTPRECISION
 ,CLIPPRECISION
 ,QUALITY
 ,PITCHANDFAMILY
 ,FACENAME
} LogFontFields;

LOGFONT Options::logFontFromString(const String &str) { // static 
  LOGFONT result;
  try {
    int field = 0;
    for(Tokenizer tok(str, _T(";")); tok.hasNext(); field++) {
      switch(field) {
      case HEIGHT         : result.lfHeight         = tok.getInt();     break;
      case WIDTH          : result.lfWidth          = tok.getInt();     break;
      case ESCAPEMENT     : result.lfEscapement     = tok.getInt();     break;
      case ORIENTATION    : result.lfOrientation    = tok.getInt();     break;
      case WEIGHT         : result.lfWeight         = tok.getInt();     break;
      case ITALIC         : result.lfItalic         = tok.getInt();     break;
      case UNDERLINE      : result.lfUnderline      = tok.getInt();     break;
      case STRIKEOUT      : result.lfStrikeOut      = tok.getInt();     break;
      case CHARSET        : result.lfCharSet        = tok.getInt();     break;
      case OUTPRECISION   : result.lfOutPrecision   = tok.getInt();     break;
      case CLIPPRECISION  : result.lfClipPrecision  = tok.getInt();     break;
      case QUALITY        : result.lfQuality        = tok.getInt();     break;
      case PITCHANDFAMILY : result.lfPitchAndFamily = tok.getInt();     break;
      case FACENAME       : 
        { const String name = tok.next();
          _tcsncpy(result.lfFaceName, name.cstr(), LF_FACESIZE);
        }
        break;
      default: throwException(_T("Unknown field for logfont:%d"), field);
      }
    }
    if(field != FACENAME + 1) {
      throwException(_T("Not enough fields for logFont <%s>"), str.cstr());
    }
  } catch(Exception e) {
    result = Options::getDefaultLogFont();
  }
  return result;
}

LOGFONT Options::getDefaultLogFont() {
  CFont f;
  f.CreateFont(-11, 8, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS
              ,CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY
              ,DEFAULT_PITCH | FF_MODERN
              ,_T("Dejavu Sans Mono")
              );
  LOGFONT lf;
  f.GetLogFont(&lf);
  f.DeleteObject();
  return lf;
}
