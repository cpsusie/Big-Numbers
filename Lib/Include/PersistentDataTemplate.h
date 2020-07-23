#pragma once

#include "TypeContainerTemplate.h"

class XMLDoc;

class _PersistentData {
private:
  String m_name;
protected:
  static const TCHAR *s_defaultName; // = "Untitled"
  _PersistentData(const String &name = s_defaultName) : m_name(name) {
  }
public:
  virtual ~_PersistentData() {
  }
  inline const String &getName() const {
    return m_name;
  }
  inline void setName(const String &name) {
    m_name = name;
  }
  inline void setDefaultName() {
    setName(s_defaultName);
  }
  inline bool hasName() const {
    return m_name.length() > 0;
  }
  inline bool hasDefaultName() const {
    return getDisplayName() == s_defaultName;
  }
  String getDisplayName() const;
  void load(const String &fileName);
  void save(const String &fileName);
  virtual void putDataToDoc(XMLDoc &doc) = 0;
  virtual void getDataFromDoc(XMLDoc &doc) = 0;
};

template<typename T> class PersistentDataTemplate : public _PersistentData, public TypeContainerTemplate<T> {
protected:
  PersistentDataTemplate(const String &name = s_defaultName) : _PersistentData(name) {
  }
};
