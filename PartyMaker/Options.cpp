#include "stdafx.h"
#include <Registry.h>
#include "Options.h"
#include <MD5.h>

static const TCHAR *registryEntry = _T("Software\\JGMData\\PartyMaker");

class InitRegistry {
public:
  InitRegistry();
};

InitRegistry::InitRegistry() {
  try {
    RegistryKey key0(HKEY_CURRENT_USER);
    RegistryKey key1 = key0.createOrOpenPath(registryEntry);
  } catch(Exception) {
    // ignore
  }
}

static InitRegistry KeyCreator;

DefaultOptions::DefaultOptions() {
  setDefault();
}

void DefaultOptions::setDefault() {
  m_dirList           = _T("C:\\Users");
  m_startSelectDir    = _T("C:\\");
  m_confirmChoise     = false;
  m_autoSelect        = false;
  m_allowDuplicates   = false;
  m_md5password       = EMPTYSTRING;
  m_maxChoise         = 20;
  m_volume            = 50;
  m_backgroundColor   = RGB(216,233,236);
  m_currentTrackColor = RGB(253,176,184);
  m_mediaQueueColor   = RGB(157,239,148);
}

Options::Options() {
  load();
}

void Options::save() {
  try {
    RegistryKey settings = getKey();

    settings.setValue(_T("dirlist")         ,m_dirList           );
    settings.setValue(_T("startselectdir")  ,m_startSelectDir    );
    settings.setValue(_T("confirmchoise")   ,m_confirmChoise     );
    settings.setValue(_T("autoselect")      ,m_autoSelect        );
    settings.setValue(_T("allowduplicates") ,m_allowDuplicates   );
  //  settings.setValue(_T("password")        ,m_md5password       ); // NB
    settings.setValue(_T("maxchoise")       ,m_maxChoise         );
    settings.setValue(_T("volume")          ,m_volume            );
    settings.setValue(_T("backgroundcolor") ,m_backgroundColor   );
    settings.setValue(_T("currentcolor")    ,m_currentTrackColor );
    settings.setValue(_T("playqueuecolor")  ,m_mediaQueueColor   );
  } catch(Exception e) {
    Message(_T("%s"), e.what());
  }
}

void Options::load() {
  try {
    RegistryKey settings = getKey();
    const DefaultOptions defaultOptions;
    m_dirList           = settings.getString(_T("dirlist")         , defaultOptions.m_dirList           );
    m_startSelectDir    = settings.getString(_T("startselectdir")  , defaultOptions.m_startSelectDir    );
    m_confirmChoise     = settings.getBool(  _T("confirmchoise")   , defaultOptions.m_confirmChoise     );
    m_autoSelect        = settings.getBool(  _T("autoselect")      , defaultOptions.m_autoSelect        );
    m_allowDuplicates   = settings.getBool(  _T("allowduplicates") , defaultOptions.m_allowDuplicates   );
    m_md5password       = settings.getString(_T("password")        , defaultOptions.m_md5password       );
    m_maxChoise         = settings.getInt(   _T("maxchoise")       , defaultOptions.m_maxChoise         );
    m_volume            = settings.getInt(   _T("volume")          , defaultOptions.m_volume            );
    m_backgroundColor   = settings.getUint(  _T("backgroundcolor") , defaultOptions.m_backgroundColor   );
    m_currentTrackColor = settings.getUint(  _T("currentcolor")    , defaultOptions.m_currentTrackColor );
    m_mediaQueueColor   = settings.getUint(  _T("playqueuecolor")  , defaultOptions.m_mediaQueueColor   );
  } catch(Exception e) {
    Message(_T("%s"), e.what());
  }
}

RegistryKey Options::getKey() { // static
  return RegistryKey(HKEY_CURRENT_USER,registryEntry).createOrOpenKey(_T("Settings"));
}

bool Options::validatePassword(const String &password) { // static
  Options options;
  options.load();
  if(options.m_md5password.length() == 0) {
    return password.length() == 0;
  } else {
    MD5Context md5;
    const String coded = md5.digest(password);
    return coded == options.m_md5password;
  }
}

void Options::setPassword(const String &oldPassword, const String &newPassword) { // static
  if(!validatePassword(oldPassword)) {
    throwException(_T("Forkert password"));
  }

  String newMD5Password;
  if(newPassword.length() == 0) {
    newMD5Password = EMPTYSTRING;
  } else {
    MD5Context md5;
    newMD5Password = md5.digest(newPassword);
  }
  getKey().setValue(_T("password"),newMD5Password);
}

void Options::setDirList(const String &dirList) {
  if(dirList != m_dirList) {
    m_dirList = dirList;
    save();
  }
}

void Options::setStartSelectDir(const String &startSelectDir) {
  if(startSelectDir != m_startSelectDir) {
    m_startSelectDir = startSelectDir;
    save();
  }
}

void Options::setAutoSelect(bool newValue) {
  if(newValue != m_autoSelect) {
    m_autoSelect = newValue;
    save();
  }
}

void Options::setAllowDuplicates(bool newValue) {
  if(newValue != m_allowDuplicates) {
    m_allowDuplicates = newValue;
    save();
  }
}

void Options::setConfirmChoise(bool newValue) {
  if(newValue != m_confirmChoise) {
    m_confirmChoise = newValue;
    save();
  }
}

void Options::setMaxChoise(int newValue) {
  if(newValue != m_maxChoise) {
    m_maxChoise = newValue;
    save();
  }
}

void Options::setVolume(int newValue) {
  if(newValue != m_volume) {
    m_volume = newValue;
    save();
  }
}

void Options::setCurrentTrackColor(COLORREF newColor) {
  if(newColor != m_currentTrackColor) {
    m_currentTrackColor = newColor;
    save();
  }
}

void Options::setMediaQueueColor(COLORREF newColor) {
  if(newColor != m_mediaQueueColor) {
    m_mediaQueueColor = newColor;
    save();
  }
}

void Options::setBackgroundColor(COLORREF newColor) {
  if(newColor != m_backgroundColor) {
    m_backgroundColor = newColor;
    save();
  }
}
