#pragma once

class Options {
public:
  int  m_cpuLoad;
  bool m_autoLaunch;
  bool m_showCounters;

  Options();
  void load();
  void save();
  void clear();
};
