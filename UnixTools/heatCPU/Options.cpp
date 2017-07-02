#include "stdafx.h"
#include <Registry.h>
#include "Options.h"

Options::Options() {
  load();
}

static RegistryKey getKey() {
  return RegistryKey(HKEY_CURRENT_USER,"Software").createOrOpenKey("JGMData\\HeatCPU\\Options");
}

void Options::load() {
  try {
    RegistryKey key = getKey();
    clear();
    key.getValue("cpuload"     , m_cpuLoad     );
    key.getValue("autolaunch"  , m_autoLaunch  );
    key.getValue("showcounters", m_showCounters);
  } catch(Exception) {
    // ignore
  }
}

void Options::save() {
  try {
    RegistryKey key = getKey();

    key.setValue("cpuload"     , m_cpuLoad     );
    key.setValue("autolaunch"  , m_autoLaunch  );
    key.setValue("showcounters", m_showCounters);
  } catch(Exception) {
    // ignore
  }
}

void Options::clear() {
  m_cpuLoad      = 0;
  m_autoLaunch   = false;
  m_showCounters = false;
}

