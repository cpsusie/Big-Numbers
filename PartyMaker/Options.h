#pragma once

#include <Registry.h>

class DefaultOptions {
public:
  String   m_dirList;
  String   m_startSelectDir;
  bool     m_confirmChoise;
  bool     m_autoSelect;
  bool     m_allowDuplicates;
  String   m_md5password;
  int      m_maxChoise;
  int      m_volume;
  COLORREF m_currentTrackColor,m_mediaQueueColor,m_backgroundColor;
  void setDefault();
  DefaultOptions();
};

class Options : public DefaultOptions {
  static RegistryKey getKey();
public:
  Options();
  void save();
  void load();
  static bool validatePassword(const String &password);
  static void setPassword(const String &oldPassword, const String &newPassword);
  void setDirList(const String &dirList);
  void setStartSelectDir(const String &startSelectDir);
  void setAutoSelect(     bool     newValue);
  void setAllowDuplicates(bool     newValue);
  void setConfirmChoise(  bool     newValue);
  void setMaxChoise(      int      newValue);
  void setVolume(         int      newValue);
  void setCurrentTrackColor(COLORREF newColor);
  void setMediaQueueColor(  COLORREF newColor);
  void setBackgroundColor(  COLORREF newColor);

  const String &getDirList() const {
    return m_dirList;
  }

  const String &getStartSelectDir() const {
    return m_startSelectDir;
  }

  bool getAutoSelect() const {
    return m_autoSelect;
  }

  bool getAllowDuplicates() const {
    return m_allowDuplicates;
  }

  bool getConfirmChoise() const {
    return m_confirmChoise;
  }

  int getMaxChoise() const {
    return m_maxChoise;
  }

  int getVolume() const {
    return m_volume;
  }

  COLORREF getCurrentTrackColor() const {
    return m_currentTrackColor;
  };

  COLORREF getMediaQueueColor() const {
    return m_mediaQueueColor;
  };

  COLORREF getBackgroundColor() const {
    return m_backgroundColor;
  };
};

