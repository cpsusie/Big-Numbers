#pragma once

#include "CompactKeyType.h"
#include <Map.h>

#pragma warning(disable:4192)

#import <msxml6.dll> named_guids

typedef MSXML2::IXMLDOMDocumentPtr   XMLDocPtr;
typedef MSXML2::IXMLDOMNodePtr       XMLNodePtr;
typedef MSXML2::IXMLDOMElementPtr    XMLElementPtr;
typedef MSXML2::IXMLDOMParseErrorPtr XMLParseErrorPtr;
typedef MSXML2::DOMNodeType          XMLNodeType;

class XMLDoc {
private:
  XMLDocPtr m_doc;
  VARIANT    getVariant(  const XMLNodePtr &node) const;
  XMLNodePtr findTextNode(const XMLNodePtr &node) const;
  XMLDocPtr getDocument() {
    return m_doc;
  }
  static void checkResult(HRESULT hr, const TCHAR *fileName, int line);
  void checkLoad();
  void loadFromFile(const TCHAR *filename);
  void saveToFile(  const TCHAR *filename);
  static void prettyWriteXmlDocument(MSXML2::IXMLDOMDocument *xmlDoc, IStream *stream);
  static void prettySaveXmlDocument(MSXML2::IXMLDOMDocument *xmlDoc, const wchar_t *filePath);

  static String &variantToString(String &dst, const VARIANT &var);
public:
  void setDocument(XMLDocPtr doc);
  XMLDoc();
  XMLDoc(XMLDocPtr doc);
  void clear();
  String getXML(bool includePi=false);
  void loadFromFile(const String &filename);
  void saveToFile(  const String &filename);

  void loadFromString(const String &XML);
  inline bool isEmpty() const {
    return getRoot() == nullptr;
  }
  XMLNodePtr findNode( const TCHAR *nodeName) const;
  XMLNodePtr findNode( const XMLNodePtr &node, const TCHAR *nodeName) const;
  // return nullptr, if not found
  XMLNodePtr findChild(const XMLNodePtr &node, const TCHAR *nodeName, int instans=0) const;
  // throws exception, if not found
  XMLNodePtr getChild( const XMLNodePtr &node, const TCHAR *nodeName, int instans=0) const;
  XMLNodePtr createNode(const TCHAR *nodeName, bool force=true);
  XMLNodePtr createNode(const XMLNodePtr &parent, const TCHAR *nodeName, bool force=true);
  XMLNodePtr createRoot(const TCHAR *rootName);
  XMLNodePtr getRoot() const;
  // throws Exception if node hasn't the expected tag
  static void checkTag(     XMLNodePtr &node, const TCHAR *expectedTag);

  void    setNodeText(const XMLNodePtr &node, const TCHAR *value) ;
  String &getNodeText(const XMLNodePtr &node,       String &value) const;

  template<typename T> T &getNodeValue(const XMLNodePtr &node, T &v) {
    String s;
    std::wstringstream(getNodeText(node,s).cstr()) >> v;
    return v;
  }
  template<typename T> void setNodeValue(const XMLNodePtr &node, const T &v) {
    setNodeText(node, (std::wstringstream() << v).str().c_str());
  }
  template<typename T> T &getNodeValue(const XMLNodePtr &node, T &v, bool hex) {
    String s;
    std::wstringstream wstr(getNodeText(node, s).cstr());
    if(hex || (_tcsnicmp(s.cstr(),_T("0x"),2) == 0)) {
      wstr.setf(std::ios::hex, std::ios::basefield);
    }
    wstr >> v;
    return v;
  }
  template<typename T> void setNodeValue(const XMLNodePtr &node, const T &v, bool hex) {
    std::wstringstream wstr;
    if(hex) {
      wstr.setf(std::ios::hex, std::ios::basefield);
      wstr.setf(std::ios::showbase);
    }

    wstr << v;
    setNodeText(node, wstr.str().c_str());
  }
  template<typename T> void setNodeValue(const XMLNodePtr &node, const T &v, std::streamsize prec) {
    std::wstringstream wstr;
    if(prec && (v!=0)) {
      if(prec < 0) prec = std::numeric_limits<T>::max_digits10;
      wstr.precision(prec);
    }
    wstr << v;
    setNodeText(node, wstr.str().c_str());
  }
};

inline size_t getChildCount(const XMLNodePtr &node) {
  return (node == nullptr) ? 0 : node->childNodes->Getlength();
}

class AbstractChildIterator: public AbstractIterator {
private:
  XMLNodePtr m_next, m_child;
  BYTE      *m_childAddr;
protected:
  AbstractChildIterator(const AbstractChildIterator &src)
    : m_next (src.m_next )
  {
    m_childAddr = (BYTE *)&m_child;
    m_child     = src.m_child;
  }
public:
  AbstractChildIterator(XMLNodePtr n)
    : m_next(n->firstChild)
  {
    m_childAddr = (BYTE *)&m_child;
    m_child     = nullptr;
  }
  AbstractIterator *clone() override {
    return new AbstractChildIterator(*this);
  }
  bool hasNext() const override {
    return m_next != nullptr;
  }
  void *next() override {
    if(!hasNext()) noNextElementError(__TFUNCTION__);
    m_child = m_next;
    m_next  = m_next->nextSibling;
    return m_childAddr;
  }
  void remove() override {
    unsupportedOperationError(__TFUNCTION__);
  }

  inline XMLNodePtr &nextChild() {
    AbstractChildIterator::next();
    return m_child;
  }
};

inline Iterator<XMLNodePtr> getChildIterator(XMLNodePtr n) {
  return Iterator<XMLNodePtr>(new AbstractChildIterator(n));
}


inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, const String    &value                  ) { doc.setNodeValue(n, value     ); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, const char      *value                  ) { doc.setNodeValue(n, value     ); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, const wchar_t   *value                  ) { doc.setNodeValue(n, value     ); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, INT              value, bool hex = false) { doc.setNodeValue(n, value, hex); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, UINT             value, bool hex = false) { doc.setNodeValue(n, value, hex); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, LONG             value, bool hex = false) { doc.setNodeValue(n, value, hex); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, ULONG            value, bool hex = false) { doc.setNodeValue(n, value, hex); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, INT64            value, bool hex = false) { doc.setNodeValue(n, value, hex); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, UINT64           value, bool hex = false) { doc.setNodeValue(n, value, hex); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, float            value, std::streamsize prec = 0) { doc.setNodeValue(n, value,prec); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, double           value, std::streamsize prec = 0) { doc.setNodeValue(n, value,prec); }

inline void getValue(  XMLDoc &doc, const XMLNodePtr &n, String          &value                  ) { doc.getNodeText( n, value     ); }
inline void getValueLF(XMLDoc &doc, const XMLNodePtr &n, String          &value                  ) { doc.getNodeText( n, value).replace('\n',"\r\n"); }
inline void getValue(  XMLDoc &doc, const XMLNodePtr &n, INT             &value, bool hex = false) { doc.getNodeValue(n, value, hex); }
inline void getValue(  XMLDoc &doc, const XMLNodePtr &n, UINT            &value, bool hex = false) { doc.getNodeValue(n, value, hex); }
inline void getValue(  XMLDoc &doc, const XMLNodePtr &n, LONG            &value, bool hex = false) { doc.getNodeValue(n, value, hex); }
inline void getValue(  XMLDoc &doc, const XMLNodePtr &n, ULONG           &value, bool hex = false) { doc.getNodeValue(n, value, hex); }
inline void getValue(  XMLDoc &doc, const XMLNodePtr &n, INT64           &value, bool hex = false) { doc.getNodeValue(n, value, hex); }
inline void getValue(  XMLDoc &doc, const XMLNodePtr &n, UINT64          &value, bool hex = false) { doc.getNodeValue(n, value, hex); }
inline void getValue(  XMLDoc &doc, const XMLNodePtr &n, float           &value                  ) { doc.getNodeValue(n, value     ); }
inline void getValue(  XMLDoc &doc, const XMLNodePtr &n, double          &value                  ) { doc.getNodeValue(n, value     ); }
void        setValue(  XMLDoc &doc, const XMLNodePtr &n, char    c, bool hex = false);
void        getValue(  XMLDoc &doc, const XMLNodePtr &n, char   &c, bool hex = false);
void        setValue(  XMLDoc &doc, const XMLNodePtr &n, BYTE    b, bool hex = false);
void        getValue(  XMLDoc &doc, const XMLNodePtr &n, BYTE   &b, bool hex = false);
void        setValue(  XMLDoc &doc, const XMLNodePtr &n, short   s, bool hex = false);
void        getValue(  XMLDoc &doc, const XMLNodePtr &n, short  &s, bool hex = false);
void        setValue(  XMLDoc &doc, const XMLNodePtr &n, USHORT  s, bool hex = false);
void        getValue(  XMLDoc &doc, const XMLNodePtr &n, USHORT &s, bool hex = false);
void        setValue(  XMLDoc &doc, const XMLNodePtr &n, bool    value);
void        getValue(  XMLDoc &doc, const XMLNodePtr &n, bool   &value);

void getValueLF(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, String &v);
void getValueLF(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, String &v, const String &defaultValue);

template<typename T, typename... Args> void setValue(XMLDoc &doc, XMLNodePtr n, const CompactKeyType<T> &v, Args... args) {
  setValue(doc, n, (T)v, args...);
}

template<typename T, typename... Args> void getValue(XMLDoc &doc, XMLNodePtr n, CompactKeyType<T> &v, Args... args) {
  T value;
  getValue(doc, n, value, args...);
  v = value;
}

template<typename K, typename V, typename... Args> void setValue(XMLDoc &doc, XMLNodePtr n, const Entry<K, V> &e, Args... args) {
  setValue<K>(doc, n, _T("key"  ), e.getKey()  ,args...);
  setValue<V>(doc, n, _T("value"), e.getValue(),args...);
}

template<typename K, typename V> class PairEntry : public Entry<K,V> {
public:
  K m_key;
  V m_value;
  const void *key() const override {
    return &m_key;
  }
  void *value()     const override {
    return (void*)&m_value;
  }
};

template<typename K, typename V, typename... Args> void getValue(XMLDoc &doc, XMLNodePtr n, PairEntry<K, V> &e, Args... args) {
  getValue<K>(doc, n, _T("key"  ), e.m_key  ,args...);
  getValue<V>(doc, n, _T("value"), e.m_value,args...);
}

String BSTRToString(const BSTR &s);

template<typename T, typename... Args> void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const T &v, Args... args) {
  XMLNodePtr n = doc.createNode(parent, tag);
  setValue(doc, n, v, args...);
}

template<typename T, typename... Args> void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, T &v, Args... args) {
  XMLNodePtr n = doc.getChild(parent, tag);
  getValue(doc, n, v, args...);
}


// try to get value from child(n) with specified tag.
// If childnode not found, v is set to defaultValue
template<typename T, typename D> void getOptValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, T &v, const D &defaultValue) {
  XMLNodePtr n = doc.findChild(parent, tag);
  if(n) {
    getValue(doc, n, v);
  } else {
    v = (T)defaultValue;
  }
}

template<typename T, typename... Args> void setValue(XMLDoc &doc, XMLNodePtr n, const ConstIterator<T> &it, Args... args) {
  UINT index = 0;
  for(auto it1 = it; it1.hasNext(); index++) {
    setValue(doc, n, format(_T("id%u"),index).cstr(), it1.next(), args...);
  }
}

template<typename T, typename... Args> void setValue(XMLDoc &doc, XMLNodePtr n, const CollectionBase<T> &c, Args... args) {
  setValue(doc, n, c.getIterator(), args...);
}

template<typename T, typename... Args> class AbstractValueListIterator : public AbstractChildIterator {
private:
  XMLDoc     &m_doc;
  UINT        m_index;
  T           m_v;
  AbstractValueListIterator(const AbstractValueListIterator &src)
    : AbstractChildIterator(src        )
    , m_doc(                src.m_doc  )
    , m_index(              src.m_index)
  {
  }
public:
  AbstractValueListIterator(XMLDoc &doc, XMLNodePtr n)
    : AbstractChildIterator(n  )
    , m_doc(                doc)
    , m_index(              0  )
  {
  }
  AbstractIterator *clone() override {
    return new AbstractValueListIterator(*this);
  }
  void *next() override {
    XMLNodePtr n = nextChild();
    const String expectedName = format(_T("id%u"), m_index++);
    const String childName    = (wchar_t *)n->nodeName;
    if(childName != expectedName) {
      throwException(_T("Invalid nodename:%s, expected:%s"), childName.cstr(), expectedName.cstr());
    }
    getValue(m_doc, n, m_v, Args...);
    return &m_v;
  }
};

template<typename T, typename... Args> ConstIterator<T> getValueListIterator(XMLDoc &doc, XMLNodePtr n) {
  return ConstIterator<T>(new AbstractValueListIterator<T, Args...>(doc, n));
}

template<typename T, typename... Args> void getValue(XMLDoc &doc, XMLNodePtr n, CollectionBase<T> &c, Args... args) {
  c.clear();
  c.addAll(getValueListIterator<T, Args...>(doc, n));
}

template<typename K,typename V, typename... Args> ConstIterator<Entry<K,V> > getEntryListIterator(XMLDoc &doc, XMLNodePtr n) {
  return Iterator<Entry<K, V> >(new AbstractValueListIterator<PairEntry<K,V>, Args...>(doc, n));
}

template<typename K, typename V, typename... Args> void setValue(XMLDoc &doc, XMLNodePtr n, const MapBase<K,V> &map, Args... args) {
  setValue(doc, n, map.getIterator(), args...);
}

template<typename K, typename V, typename... Args> void getValue(XMLDoc &doc, XMLNodePtr n, MapBase<K,V> &map, Args... args) {
  map.clear();
  map.addAll(getEntryListIterator<K,V, Args...>(doc, n));
}
