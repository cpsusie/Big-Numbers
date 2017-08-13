#include "stdafx.h"
#include <Registry.h>
#include "Whist3.h"
#include "GameTypes.h"

static const TCHAR *registryEntry = _T("Software\\JGMData\\3mandsWhist");

class InitRegistry {
public:
  InitRegistry();
};

InitRegistry::InitRegistry() {
  try {
    RegistryKey(HKEY_CURRENT_USER).createOrOpenPath(registryEntry);
  } catch(Exception) {
    // ignore
  }
}

static RegistryKey getKey() {
  static InitRegistry dummy;
  return RegistryKey(HKEY_CURRENT_USER, registryEntry).createOrOpenKey(_T("Settings"));
}

void Options::load() {
  try {
    RegistryKey key = getKey();
    m_myName     = key.getString(_T("name")      ,EMPTYSTRING   );
    m_dealerName = key.getString(_T("dealername"),EMPTYSTRING   );
    m_connected  = key.getBool(  _T("connected") ,false);
    m_backside   = key.getInt(   _T("backside")  ,53   );
  } catch(Exception e) {
    showException(e);
  }

  if(m_backside < 53) {
    m_backside = 53;
  }
}

void Options::save() {
  try {
    RegistryKey key = getKey();
    key.setValue(_T("name")      ,m_myName    );
    key.setValue(_T("dealername"),m_dealerName);
    key.setValue(_T("connected") ,m_connected );
    key.setValue(_T("backside")  ,m_backside  );
  } catch(Exception e) {
    showException(e);
  }
}

Options &OptionsAccessor::getOptions() {
  return theApp.m_options;
}

const Options &OptionsAccessor::getOptions() const {
  return theApp.m_options;
}
