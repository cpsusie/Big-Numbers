#include "stdafx.h"
#include <Registry.h>

DefaultSettings::DefaultSettings() {
  setDefault();
}

#define WHITE         RGB(255,255,255)
#define BLACK         RGB(  0,  0,  0)
#define GREY          RGB(220,220,220)
#define LIGHTRED      RGB(255,  0,  0)
#define RED           RGB(128,  0,  0)
#define LIGHTYELLOW   RGB(255,255,138)

void DefaultSettings::setDefault() {
  m_dataRadix        = 10;
  m_addrRadix        = 10;
  m_hex3Pos          = false;
  m_dataHexUppercase = false;
  m_addrHexUppercase = false;
  m_addrSeparators   = false;
  m_showAddr         = true;
  m_showAscii        = true;
  m_lineSize         = 20;
  m_fitLinesToWindow = false;
  m_dataTextColor    = BLACK;
  m_dataBackColor    = WHITE;
  m_asciiTextColor   = BLACK;
  m_asciiBackColor   = LIGHTYELLOW;
  m_addrTextColor    = BLACK;
  m_addrBackColor    = GREY;
  m_windowSize.cx    = 770;
  m_windowSize.cy    = 640;
  m_wrapEndOfLine    = false;
}

RegistryKey Settings::getRootKey() { // static
  return RegistryKey(HKEY_CURRENT_USER, _T("Software")).createOrOpenPath(_T("JGMData\\HexView"));
}

Settings::Settings() {
  load();
}

static const TCHAR *FIELD_RADIX            = _T("Radix");
static const TCHAR *FIELD_ADDRRADIX        = _T("AddrRadix");
static const TCHAR *FIELD_HEX3POS          = _T("Hex3Pos");
static const TCHAR *FIELD_DATAHEXUPPERCASE = _T("DataHexUpper");
static const TCHAR *FIELD_ADDRHEXUPPERCASE = _T("AddrHexUpper");
static const TCHAR *FIELD_ADDRSEPARATORS   = _T("AddrSeparators");
static const TCHAR *FIELD_SHOWADDR         = _T("ShowAddr");
static const TCHAR *FIELD_SHOWASCII        = _T("ShowAscii");
static const TCHAR *FIELD_LINESIZE         = _T("LineSize");
static const TCHAR *FIELD_FITLINES         = _T("FitLines");
static const TCHAR *FIELD_DATATEXTCOLOR    = _T("DataTColor");
static const TCHAR *FIELD_DATABACKCOLOR    = _T("DataBColor");
static const TCHAR *FIELD_ASCIITEXTCOLOR   = _T("AsciiTColor");
static const TCHAR *FIELD_ASCIIBACKCOLOR   = _T("AsciiBColor");
static const TCHAR *FIELD_ADDRTEXTCOLOR    = _T("AddrTColor");
static const TCHAR *FIELD_ADDRBACKCOLOR    = _T("AddrBColor");
static const TCHAR *FIELD_WINDOWWIDTH      = _T("WinWidth");
static const TCHAR *FIELD_WINDOWHEIGHT     = _T("WinHeight");
static const TCHAR *FIELD_WRAPENDOFLINE    = _T("WrapEOL");

void Settings::save() {
  try {
    RegistryKey key = getKey();

    key.setValue(FIELD_RADIX            ,m_dataRadix        );
    key.setValue(FIELD_ADDRRADIX        ,m_addrRadix        );
    key.setValue(FIELD_HEX3POS          ,m_hex3Pos          );
    key.setValue(FIELD_DATAHEXUPPERCASE ,m_dataHexUppercase );
    key.setValue(FIELD_ADDRHEXUPPERCASE ,m_addrHexUppercase );
    key.setValue(FIELD_ADDRSEPARATORS   ,m_addrSeparators   );
    key.setValue(FIELD_SHOWADDR         ,m_showAddr         );
    key.setValue(FIELD_SHOWASCII        ,m_showAscii        );
    key.setValue(FIELD_LINESIZE         ,m_lineSize         );
    key.setValue(FIELD_FITLINES         ,m_fitLinesToWindow );
    key.setValue(FIELD_DATATEXTCOLOR    ,m_dataTextColor    );
    key.setValue(FIELD_DATABACKCOLOR    ,m_dataBackColor    );
    key.setValue(FIELD_ASCIITEXTCOLOR   ,m_asciiTextColor   );
    key.setValue(FIELD_ASCIIBACKCOLOR   ,m_asciiBackColor   );
    key.setValue(FIELD_ADDRTEXTCOLOR    ,m_addrTextColor    );
    key.setValue(FIELD_ADDRBACKCOLOR    ,m_addrBackColor    );
    key.setValue(FIELD_WINDOWWIDTH      ,m_windowSize.cx    );
    key.setValue(FIELD_WINDOWHEIGHT     ,m_windowSize.cy    );
    key.setValue(FIELD_WRAPENDOFLINE    ,m_wrapEndOfLine    );
  } catch(Exception e) {
    AfxMessageBox(e.what(), MB_ICONWARNING);
  }
}

static bool checkRadix(int &radix) {
  switch(radix) {
  case  8:
  case 10:
  case 16:
    return true;
  default:
    radix = 10;
    return false;
  }
}

void Settings::load() {
  try {
    RegistryKey key = getKey();
    const DefaultSettings DefaultSettings;
    m_dataRadix         = key.getInt( FIELD_RADIX            , DefaultSettings.m_dataRadix        );
    m_addrRadix         = key.getInt( FIELD_ADDRRADIX        , DefaultSettings.m_addrRadix        );
    m_hex3Pos           = key.getBool(FIELD_HEX3POS          , DefaultSettings.m_hex3Pos          );
    m_dataHexUppercase  = key.getBool(FIELD_DATAHEXUPPERCASE , DefaultSettings.m_dataHexUppercase );
    m_addrHexUppercase  = key.getBool(FIELD_ADDRHEXUPPERCASE , DefaultSettings.m_addrHexUppercase );
    m_addrSeparators    = key.getBool(FIELD_ADDRSEPARATORS   , DefaultSettings.m_addrSeparators   );
    m_showAddr          = key.getBool(FIELD_SHOWADDR         , DefaultSettings.m_showAddr         );
    m_showAscii         = key.getBool(FIELD_SHOWASCII        , DefaultSettings.m_showAscii        );
    m_lineSize          = key.getInt( FIELD_LINESIZE         , DefaultSettings.m_lineSize         );
    m_fitLinesToWindow  = key.getBool(FIELD_FITLINES         , DefaultSettings.m_fitLinesToWindow );
    m_dataTextColor     = key.getInt( FIELD_DATATEXTCOLOR    , DefaultSettings.m_dataTextColor    );
    m_dataBackColor     = key.getInt( FIELD_DATABACKCOLOR    , DefaultSettings.m_dataBackColor    );
    m_asciiTextColor    = key.getInt( FIELD_ASCIITEXTCOLOR   , DefaultSettings.m_asciiTextColor   );
    m_asciiBackColor    = key.getInt( FIELD_ASCIIBACKCOLOR   , DefaultSettings.m_asciiBackColor   );
    m_addrTextColor     = key.getInt( FIELD_ADDRTEXTCOLOR    , DefaultSettings.m_addrTextColor    );
    m_addrBackColor     = key.getInt( FIELD_ADDRBACKCOLOR    , DefaultSettings.m_addrBackColor    );
    m_windowSize.cx     = key.getInt( FIELD_WINDOWWIDTH      , DefaultSettings.m_windowSize.cx    );
    m_windowSize.cy     = key.getInt( FIELD_WINDOWHEIGHT     , DefaultSettings.m_windowSize.cy    );
    m_wrapEndOfLine     = key.getBool(FIELD_WRAPENDOFLINE    , DefaultSettings.m_wrapEndOfLine    );
    checkRadix(m_dataRadix);
    checkRadix(m_addrRadix);
    m_lineSize = min(m_lineSize, 65536);
  } catch(Exception e) {
    AfxMessageBox(e.what(), MB_ICONWARNING);
  }
}

void Settings::throwInvalidRadix(const TCHAR *function, int radix) {
  throwException(_T("%s:Invalid radix (=%d)"), function, radix);
}

bool Settings::setDataRadix(int radix) {
  if(radix != m_dataRadix) {
    checkRadix(radix);
    m_dataRadix = radix;
    save();
    return true;
  }
  return false;
}

bool Settings::setAddrRadix(int radix) {
  if(radix != m_addrRadix) {
    checkRadix(radix);
    m_addrRadix = radix;
    save();
    return true;
  }
  return false;
}

bool Settings::setHex3Pos(bool newValue) {
  if(newValue != m_hex3Pos) {
    m_hex3Pos = newValue;
    save();
    return true;
  }
  return false;
}

bool Settings::setDataHexUppercase(bool uppercase) {
  if(uppercase != m_dataHexUppercase) {
    m_dataHexUppercase = uppercase;
    save();
    return true;
  }
  return false;
}

bool Settings::setAddrHexUppercase(bool uppercase) {
  if(uppercase != m_addrHexUppercase) {
    m_addrHexUppercase = uppercase;
    save();
    return true;
  }
  return false;
}

bool Settings::setAddrSeparators(bool separators) {
  if(separators != m_addrSeparators) {
    m_addrSeparators = separators;
    save();
    return true;
  }
  return false;
}

bool Settings::setShowAddr(bool newValue) {
  if(newValue != m_showAddr) {
    m_showAddr = newValue;
    save();
    return true;
  }
  return false;
}

bool Settings::setShowAscii(bool newValue) {
  if(newValue != m_showAscii) {
    m_showAscii = newValue;
    save();
    return true;
  }
  return false;
}

bool Settings::setLineSize(int newValue) {
  if(newValue != m_lineSize) {
    m_lineSize = newValue;
    save();
    return true;
  }
  return false;
}

bool Settings::setFitLinesToWindow(bool fit) {
  if(fit != m_fitLinesToWindow) {
    m_fitLinesToWindow = fit;
    save();
    return true;
  }
  return false;
}

bool Settings::setDataTextColor(COLORREF color) {
  if(color != m_dataTextColor) {
    m_dataTextColor = color;
    save();
    return true;
  }
  return false;
}

bool Settings::setDataBackColor(COLORREF color) {
  if(color != m_dataBackColor) {
    m_dataBackColor = color;
    save();
    return true;
  }
  return false;
}

bool Settings::setAsciiTextColor(COLORREF color) {
  if(color != m_asciiTextColor) {
    m_asciiTextColor = color;
    save();
    return true;
  }
  return false;
}

bool Settings::setAsciiBackColor(COLORREF color) {
  if(color != m_asciiBackColor) {
    m_asciiBackColor = color;
    save();
    return true;
  }
  return false;
}

bool Settings::setAddrTextColor(COLORREF color) {
  if(color != m_addrTextColor) {
    m_addrTextColor = color;
    save();
    return true;
  }
  return false;
}

bool Settings::setAddrBackColor(COLORREF color) {
  if(color != m_addrBackColor) {
    m_addrBackColor = color;
    save();
    return true;
  }
  return false;
}

bool Settings::setWindowSize(const CSize &size) {
  if(size != m_windowSize) {
    m_windowSize = size;
    save();
    return true;
  }
  return false;
}

bool Settings::setWrapEndOfLine(bool wrap) {
  if(wrap != m_wrapEndOfLine) {
    m_wrapEndOfLine = wrap;
    save();
    return true;
  }
  return false;
}

const TCHAR *Settings::getRadixShortName(int radix) { // static
  switch(radix) {
  case 8 : return _T("OCT");
  case 10: return _T("DEC");
  case 16: return _T("HEX");
  default: throwInvalidRadix(_T("getRadixShortName"), radix);
           return _T("DEC");
  }
}

const TCHAR *Settings::getRadixName(int radix) { // static
  switch(radix) {
  case 8 : return _T("Octal");
  case 10: return _T("Decimal");
  case 16: return _T("Hexadecimal");
  default: throwInvalidRadix(_T("getRadixName"), radix);
           return _T("Decimal");
  }
}

String Settings::getAddrAsString(__int64 addr) const {
  switch(m_addrRadix) {
  case 8 : return format(_T("%I64o"), addr);
  case 10: return format(_T("%I64u"), addr);
  case 16: return format(m_addrHexUppercase?_T("%I64X"):_T("%I64x"), addr);
  default: throwInvalidRadix(_T("getAddrAsString"), m_addrRadix);
           return format(_T("%I64u"), addr);
  }
}

const TCHAR *Settings::getAsciiFormat() const {
  switch(m_dataRadix) {
  case 8 :
  case 10:
    break;
  case 16:
    if(!m_hex3Pos) {
      return _T("%-2c ");
    }
    break;
  default:
    throwInvalidRadix(_T("getAsciiFormat"), m_dataRadix);
  }
  return _T("%-3c ");
}

const TCHAR *Settings::getRadixFormat() const {
  switch(m_dataRadix) {
  case 8 : return _T("%03o ");
  case 10: return _T("%03d ");
  case 16: if(m_dataHexUppercase) {
             return m_hex3Pos ? _T("%03X ") : _T("%02X ");
           } else {
             return m_hex3Pos ? _T("%03x ") : _T("%02x ");
           }
  default: throwInvalidRadix(_T("getRadixFormat"), m_dataRadix);
  }
  return _T("%03d ");
}

CString Settings::getDataSampleText()  const {
  switch(m_dataRadix) {
  case 8 :
  case 10:
    break;
  case 16:
    if(!m_hex3Pos) {
      return _T("00 ");
    }
    break;
  default:
    throwInvalidRadix(_T("getDataSampleText"), m_dataRadix);
  }
  return _T("000 ");
}

int Settings::getByteLength() const { // Number of characters used to show 1 byte
  switch(m_dataRadix) {
  case 8 :
  case 10: return 3;
  case 16: return m_hex3Pos ? 3 : 2;
  default: throwInvalidRadix(_T("getByteLength"), m_dataRadix);
  }
  return 3;
}

bool Settings::isValidRadixChar(TCHAR ch, int radix) { // static
  switch(radix) {
  case 8 : return isOctDigit(ch);
  case 10: return _istdigit(ch) ? true : false;
  case 16: return _istxdigit(ch) ? true : false;
  default: throwInvalidRadix(_T("isValidRadixChar"), radix);
  }
  return false;
}

BYTE Settings::charToByte(unsigned char ch) const {
  if(!isValidRadixChar(ch, m_dataRadix)) {
    throwException(_T("'%c' is not a valid %s digit"), ch, getDataRadixName());
  }
  switch(m_dataRadix) {
  case 8 : return octToByte(ch);
  case 10: return decToByte(ch);
  case 16: return hexToByte(ch);
  default: throwInvalidRadix(_T("charToByte"), m_dataRadix);
  }
  return 0;
}


Settings &SettingsAccessor::getSettings() {
  return theApp.m_settings;
}

const Settings &SettingsAccessor::getSettings() const {
  return theApp.m_settings;
}
