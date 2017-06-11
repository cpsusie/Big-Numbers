#pragma once

#include <XmlDoc.h>
#include <NumberInterval.h>

class PersistentData {
private:
  String m_name;
protected:
  static const TCHAR *s_defaultName;
public:
  PersistentData() : m_name(s_defaultName) {
  }
  void load(const String &fileName);
  void save(const String &fileName);
  static void           checkTag(XMLNodePtr node, const TCHAR *expectedTag);
  static XMLNodePtr     getChild(XMLDoc &doc, XMLNodePtr n, const TCHAR *tag);
  virtual void putDataToDoc(XMLDoc &doc) = 0;
  virtual void getDataFromDoc(XMLDoc &doc) = 0;
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
    return m_name == s_defaultName;
  }
  String getDisplayName() const;
  virtual int getType() const = 0;
};
