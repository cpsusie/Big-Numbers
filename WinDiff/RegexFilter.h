#pragma once

#include <Regex.h>
#include <Registry.h>

class RegexFilter {
private:
  Regex         m_reg;
public:
  bool          m_matchCase;
  bool          m_matchWholeWord;
  String        m_regex;
  RegexFilter();
  void compile();
  void clear();
  bool isEmpty() const { return m_regex.length() == 0; }
  String filter(const String &s) const; // remove all occurences of regex from s
  static void saveData(RegistryKey &key, const RegexFilter &rf);
  static void loadData(RegistryKey &key,       RegexFilter &rf);
  bool operator==(const RegexFilter &ref) const;
  bool operator!=(const RegexFilter &ref) const {
    return !(*this == ref);
  }
};

