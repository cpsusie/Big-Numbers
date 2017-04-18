#pragma once

#include <XmlDoc.h>
#include <NumberInterval.h>

class PersistentParameter {
private:
  String m_name;
public:
  PersistentParameter() : m_name(_T("Untitled")) {
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
  inline bool hasDefaultName() const {
    return m_name == _T("Untitled");
  }
  String getDisplayName() const;
  virtual int getType() const = 0;
};
