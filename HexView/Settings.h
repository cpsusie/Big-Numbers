#pragma once

#include <Registry.h>

class DefaultSettings {
public:
  int      m_dataRadix;
  int      m_addrRadix;
  bool     m_hex3Pos;
  bool     m_dataHexUppercase;
  bool     m_addrHexUppercase;
  bool     m_addrSeparators;
  bool     m_showAddr;
  bool     m_showAscii;
  int      m_lineSize;
  bool     m_fitLinesToWindow;
  COLORREF m_dataTextColor , m_dataBackColor;
  COLORREF m_asciiTextColor, m_asciiBackColor;
  COLORREF m_addrTextColor , m_addrBackColor;
  CSize    m_windowSize;
  bool     m_wrapEndOfLine;

  void setDefault();
  DefaultSettings();
};

class Settings : private DefaultSettings {
  static RegistryKey getRootKey();
  static RegistryKey getKey() {
    return getSubKey(_T("Settings"));
  }
  static void throwInvalidRadix(const TCHAR *function, int radix);
public:
  Settings();
  static RegistryKey getSubKey(const String &name) {
    return getRootKey().createOrOpenKey(name);
  }
  void save();
  void load();
  bool setDataRadix(       int          radix     );
  bool setAddrRadix(       int          radix     );
  bool setHex3Pos(         bool         hex3Pos   );
  bool setDataHexUppercase(bool         uppercase );
  bool setAddrHexUppercase(bool         uppercase );
  bool setAddrSeparators(  bool         separators);
  bool setShowAddr(        bool         show      );
  bool setShowAscii(       bool         show      );
  bool setLineSize(        int          lineSize  );
  bool setFitLinesToWindow(bool         fit       );
  bool setDataTextColor(   COLORREF     color     );
  bool setDataBackColor(   COLORREF     color     );
  bool setAsciiTextColor(  COLORREF     color     );
  bool setAsciiBackColor(  COLORREF     color     );
  bool setAddrTextColor(   COLORREF     color     );
  bool setAddrBackColor(   COLORREF     color     );
  bool setWindowSize(      const CSize &size      );
  bool setWrapEndOfLine(   bool         wrap      );

  int getDataRadix() const {
    return m_dataRadix;
  }

  bool getHex3Pos() const {
    return m_hex3Pos;
  }

  bool getDataHexUppercase() const {
    return m_dataHexUppercase;
  }

  bool getAddrHexUppercase() const {
    return m_addrHexUppercase;
  }

  bool getAddrSeparators() const {
    return m_addrSeparators;
  }

  int getAddrRadix() const {
    return m_addrRadix;
  }

  bool getShowAddr() const {
    return m_showAddr;
  }

  bool getShowAscii() const {
    return m_showAscii;
  }

  int getLineSize() const {
    return m_lineSize;
  }

  bool getFitLinesToWindow() const {
    return m_fitLinesToWindow;
  }

  COLORREF getDataTextColor() const {
    return m_dataTextColor;
  }

  COLORREF getDataBackColor() const {
    return m_dataBackColor;
  }

  COLORREF getAsciiTextColor() const {
    return m_asciiTextColor;
  }

  COLORREF getAsciiBackColor() const {
    return m_asciiBackColor;
  }

  COLORREF getAddrTextColor() const {
    return m_addrTextColor;
  }

  COLORREF getAddrBackColor() const {
    return m_addrBackColor;
  }

  const CSize &getWindowSize() const {
    return m_windowSize;
  }

  bool getWrapEndOfLine() const {
    return m_wrapEndOfLine;
  }

  static const TCHAR *getRadixShortName(int radix);
  static const TCHAR *getRadixName(     int radix);

  const TCHAR *getDataRadixShortName() const {
    return getRadixShortName(m_dataRadix);
  }

  const TCHAR *getAddrRadixShortName() const {
    return getRadixShortName(m_addrRadix);
  }

  const TCHAR *getDataRadixName() const {
    return getRadixName(m_dataRadix);
  }

  const TCHAR *getAddrRadixName() const {
    return getRadixName(m_addrRadix);
  }

  String        getAddrAsString(__int64 addr) const;
  const TCHAR  *getAsciiFormat()              const;
  const TCHAR  *getRadixFormat()              const;
  CString       getDataSampleText()           const;
  int           getByteLength()               const; // Number of characters used to show 1 byte
  BYTE          charToByte(unsigned char ch)  const;

  String unEscapeByte(BYTE byte) const;
  String unEscape(const ByteArray &a) const;

  static bool isValidRadixChar(TCHAR ch, int radix);
};

class AddressToString {
private:
  __int64          m_maxValue;
  int              m_radix;
  bool             m_uppercase;
  bool             m_separators;

  String           m_offsetFormatString;
  String           m_addrFormatString;
  String           m_sampleTextString;
  int              m_addrLength;
  const TCHAR     *m_offsetFormat, *m_addrFormat;

  void init(__int64 maxValue, int radix, bool uppercase, bool separators);
public:
  AddressToString();
  AddressToString(__int64 maxValue, const Settings &settings);
  void update(    __int64 maxValue, const Settings &settings);
  String addrToString(__int64 addr) const;
  String offsetToString(int offset) const;

  const String &getSampleText() const {
    return m_sampleTextString;
  }
};

class SettingsAccessor {
public:
  Settings &getSettings();
  const Settings &getSettings() const;
};
