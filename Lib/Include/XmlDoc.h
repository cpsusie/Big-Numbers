#pragma once

#include "CompactKeyType.h"

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
  VARIANT    getVariant(  const XMLNodePtr &node);
  XMLNodePtr findTextNode(const XMLNodePtr &node);
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

  XMLNodePtr findNode( const TCHAR *nodeName);
  XMLNodePtr findNode( const XMLNodePtr &node, const TCHAR *nodeName);
  // return NULL, if not found
  XMLNodePtr findChild(const XMLNodePtr &node, const TCHAR *nodeName, int instans=0);
  // throws exception, if not found
  XMLNodePtr getChild( const XMLNodePtr &node, const TCHAR *nodeName, int instans=0);

  XMLNodePtr createNode(const TCHAR *nodeName, bool force=true);
  XMLNodePtr createNode(const XMLNodePtr &parent, const TCHAR *nodeName, bool force=true);
  XMLNodePtr createRoot(const TCHAR *rootName);
  XMLNodePtr getRoot();
  // throws Exception if node hasn't the expected tag
  static void checkTag(     XMLNodePtr &node, const TCHAR *expectedTag);

  void    setNodeText(const XMLNodePtr &node, const TCHAR *value) ;
  String &getNodeText(const XMLNodePtr &node,       String &value);

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
    if(hex) {
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
};

inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, const String    &value                  ) { doc.setNodeValue(n, value     ); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, const TCHAR     *value                  ) { doc.setNodeValue(n, value     ); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, int              value, bool hex = false) { doc.setNodeValue(n, value, hex); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, UINT             value, bool hex = false) { doc.setNodeValue(n, value, hex); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, long             value, bool hex = false) { doc.setNodeValue(n, value, hex); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, ULONG            value, bool hex = false) { doc.setNodeValue(n, value, hex); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, INT64            value, bool hex = false) { doc.setNodeValue(n, value, hex); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, UINT64           value, bool hex = false) { doc.setNodeValue(n, value, hex); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, float            value                  ) { doc.setNodeValue(n, value     ); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, double           value                  ) { doc.setNodeValue(n, value     ); }

inline void getValue(  XMLDoc &doc, const XMLNodePtr &n, String          &value                  ) { doc.getNodeText( n, value     ); }
inline void getValueLF(XMLDoc &doc, const XMLNodePtr &n, String          &value                  ) { doc.getNodeText( n, value).replace('\n',_T("\r\n")); }
inline void getValue(  XMLDoc &doc, const XMLNodePtr &n, int             &value, bool hex = false) { doc.getNodeValue(n, value, hex); }
inline void getValue(  XMLDoc &doc, const XMLNodePtr &n, UINT            &value, bool hex = false) { doc.getNodeValue(n, value, hex); }
inline void getValue(  XMLDoc &doc, const XMLNodePtr &n, long            &value, bool hex = false) { doc.getNodeValue(n, value, hex); }
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

template<typename T> void setValue(XMLDoc &doc, XMLNodePtr n, const CompactKeyType<T> &v) {
  setValue(doc, n, (T)v);
}

template<typename T> void getValue(XMLDoc &doc, XMLNodePtr n, CompactKeyType<T> &v) {
  T value;
  getValue(doc, n, value);
  v = value;
}

String BSTRToString(const BSTR &s);

template<typename T> void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const T &v) {
  XMLNodePtr n = doc.createNode(parent, tag);
  setValue(doc, n, v);
}

template<typename T> void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, T &v) {
  XMLNodePtr n = doc.getChild(parent, tag);
  getValue(doc, n, v);
}

template<typename T> void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const T &v, bool hex) {
  XMLNodePtr n = doc.createNode(parent, tag);
  setValue(doc, n, v, hex);
}

template<typename T> void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, T &v, bool hex) {
  XMLNodePtr n = doc.getChild(parent, tag);
  getValue(doc, n, v, hex);
}

void getValueLF(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, String &v);
void getValueLF(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, String &v, const String &defaultValue);

template<typename T, typename D> void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, T &v, const D &defaultValue) {
  XMLNodePtr n = doc.findChild(parent, tag);
  if(n) {
    getValue(doc, n, v);
  } else {
    v = (T)defaultValue;
  }
}

template<typename T> void setValue(XMLDoc &doc, XMLNodePtr n, const Iterator<T> &it) {
  UINT index = 0;
  for(Iterator<T> it1 = it; it1.hasNext(); index++) {
    setValue<T>(doc, n, format(_T("id%u"),index).cstr(), it1.next());
  }
}

template<typename Iterable, typename ElementType> void setValue(XMLDoc &doc, XMLNodePtr n, const Iterable &a) {
  setValue(doc, n, ((Iterable&)a).getIterator());
}

template<typename Collection, typename ElementType> void getValue(XMLDoc &doc, XMLNodePtr n, Collection &c) {
  UINT index = 0;
  c.clear();
  for(XMLNodePtr child = n->firstChild; child != NULL; child = child->nextSibling, index++) {
    const String expectedName = format(_T("id%u"), index);
    const String childName    = (wchar_t*)child->nodeName;
    if(childName != expectedName) {
      throwException(_T("Invalid nodename:%s, expected:%s"), childName.cstr(), expectedName.cstr());
    }
    ElementType e;
    getValue(doc, child, e);
    c.add(e);
  }
}

template<typename MAP, typename KEY, typename VALUE> void setValue(XMLDoc &doc, XMLNodePtr n, const MAP &map) {
  UINT index = 0;
  for(Iterator<Entry<KEY, VALUE> > it = map.getEntryIterator(); it.hasNext(); index++) {
    const Entry<KEY, VALUE> &entry = it.next();
    XMLNodePtr entryNode = doc.createNode(n, format(_T("e%u"), index).cstr());
    setValue<KEY>(  doc, entryNode, _T("key"  ), entry.getKey()  );
    setValue<VALUE>(doc, entryNode, _T("value"), entry.getValue());
  }
}

template<typename MAP, typename KEY, typename VALUE> void getValue(XMLDoc &doc, XMLNodePtr n, MAP &map) {
  map.clear();
  UINT index = 0;
  for(XMLNodePtr child = n->firstChild; child; child = child->nextSibling, index++) {
    const String expectedName = format(_T("e%u"), index);
    const String childName    = (wchar_t*)child->nodeName;
    if(childName != expectedName) {
      throwException(_T("Invalid nodename:%s, expected:%s"), childName.cstr(), expectedName.cstr());
    }
    KEY   key;
    VALUE value;
    getValue<KEY>  (doc, child, _T("key"  ), key  );
    getValue<VALUE>(doc, child, _T("value"), value);
    map.put(key, value);
  }
}
