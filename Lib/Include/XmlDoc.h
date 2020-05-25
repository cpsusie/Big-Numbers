#pragma once

#include "Date.h"
#include "CompactKeyType.h"
#include <comdef.h>
#include <atlconv.h>

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
  VARIANT    getVariant(  const TCHAR      *nodeName, const TCHAR *tagName, int instans=0);
  VARIANT    getVariant(  const XMLNodePtr &node);
  VARIANT    getVariant(  const XMLNodePtr &node    , const TCHAR *tagName, int instans=0);
  XMLNodePtr findTextNode(const XMLNodePtr &node);
  XMLDocPtr getDocument() {
    return m_doc;
  }
  void checkResult(HRESULT hr, int line);
  void checkLoad();
  void recursiveDelete(XMLNodePtr &node);
  void loadFromFile(const TCHAR *filename);
  void saveToFile(  const TCHAR *filename);
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

  void getValue(  const XMLNodePtr &node  , const TCHAR *tagName, String          &value, int  instans=0);
  void getValueLF(const XMLNodePtr &node  , const TCHAR *tagName, String          &value, int  instans=0);
  void getValue(  const XMLNodePtr &node  , const TCHAR *tagName, int             &value, int  instans=0);
  void getValue(  const XMLNodePtr &node  , const TCHAR *tagName, UINT            &value, int  instans=0);
  void getValue(  const XMLNodePtr &node  , const TCHAR *tagName, long            &value, int  instans=0);
  void getValue(  const XMLNodePtr &node  , const TCHAR *tagName, ULONG           &value, int  instans=0);
  void getValue(  const XMLNodePtr &node  , const TCHAR *tagName, INT64           &value, int  instans=0);
  void getValue(  const XMLNodePtr &node  , const TCHAR *tagName, UINT64          &value, int  instans=0);
  void getValue(  const XMLNodePtr &node  , const TCHAR *tagName, float           &value, int  instans=0);
  void getValue(  const XMLNodePtr &node  , const TCHAR *tagName, double          &value, int  instans=0);
  void getValue(  const XMLNodePtr &node  , const TCHAR *tagName, Date            &value, int  instans=0);
  void getValue(  const XMLNodePtr &node  , const TCHAR *tagName, Timestamp       &value, int  instans=0);
  void getValue(  const XMLNodePtr &node  , const TCHAR *tagName, bool            &value, int  instans=0);

  template<typename T> T getValueOrDefault(const XMLNodePtr &node, const TCHAR *tagName, const T &defaultValue, int instans = 0) {
    if(findChild(node, tagName, instans) == NULL) {
      return defaultValue;
    }
    T v;
    getValue(node, tagName, v, instans);
    return v;
  }

  void setValue(const XMLNodePtr &node    , const TCHAR *tagName, const String    &value, bool force=true);
  void setValue(const XMLNodePtr &node    , const TCHAR *tagName, const TCHAR     *value, bool force=true);
  void setValue(const XMLNodePtr &node    , const TCHAR *tagName, int              value, bool force=true);
  void setValue(const XMLNodePtr &node    , const TCHAR *tagName, UINT             value, bool force=true);
  void setValue(const XMLNodePtr &node    , const TCHAR *tagName, long             value, bool force=true);
  void setValue(const XMLNodePtr &node    , const TCHAR *tagName, ULONG            value, bool force=true);
  void setValue(const XMLNodePtr &node    , const TCHAR *tagName, INT64            value, bool force=true);
  void setValue(const XMLNodePtr &node    , const TCHAR *tagName, UINT64           value, bool force=true);
  void setValue(const XMLNodePtr &node    , const TCHAR *tagName, float            value, bool force=true);
  void setValue(const XMLNodePtr &node    , const TCHAR *tagName, double           value, bool force=true);
  void setValue(const XMLNodePtr &node    , const TCHAR *tagName, const Date      &value, bool force=true);
  void setValue(const XMLNodePtr &node    , const TCHAR *tagName, const Timestamp &value, bool force=true);
  void setValue(const XMLNodePtr &node    , const TCHAR *tagName, bool             value, bool force=true);

  inline void getValueLF(const TCHAR *nodeName, const TCHAR *tagName, String  &value, int  instans = 0) {
    getValueLF(findNode(nodeName), tagName, value, instans);
  }
  template<typename T> void getValue(const TCHAR *nodeName, const TCHAR *tagName, T &value, int  instans = 0) {
    getValue(findNode(nodeName), tagName, value, instans);
  }
  template<typename T> void setValue(const TCHAR *nodeName, const TCHAR *tagName, const T &value, bool force = true) {
    setValue(findNode(nodeName),tagName,value,force);
  }

  // throws Exception if node hasn't the expected tag
  static void checkTag(     XMLNodePtr &node, const TCHAR *expectedTag);

  void    setNodeText(const XMLNodePtr &node, const TCHAR *value) ;
  String &getNodeText(const XMLNodePtr &node,       String &value);

  template<typename T> T &getValueTemplate(const XMLNodePtr &node, T &v) {
    String s;
    std::wstringstream(getNodeText(node,s).cstr()) >> v;
    return v;
  }
  template<typename T> void setValueTemplate(const XMLNodePtr &node, const T &v) {
    setNodeText(node, (std::wstringstream() << v).str().c_str());
  }
};

inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, const String    &value) { doc.setValueTemplate(n,value); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, const TCHAR     *value) { doc.setValueTemplate(n,value); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, int              value) { doc.setValueTemplate(n,value); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, UINT             value) { doc.setValueTemplate(n,value); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, long             value) { doc.setValueTemplate(n,value); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, ULONG            value) { doc.setValueTemplate(n,value); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, INT64            value) { doc.setValueTemplate(n,value); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, UINT64           value) { doc.setValueTemplate(n,value); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, float            value) { doc.setValueTemplate(n,value); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, double           value) { doc.setValueTemplate(n,value); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, const Date      &value) { doc.setValueTemplate(n,value); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, const Timestamp &value) { doc.setValueTemplate(n,value); }
inline void setValue(  XMLDoc &doc, const XMLNodePtr &n, bool             value) { doc.setValueTemplate(n,value); }
inline void getValue(  XMLDoc &doc, const XMLNodePtr &n, String          &value) { doc.setValueTemplate(n,value); }
inline void getValueLF(XMLDoc &doc, const XMLNodePtr &n, String          &value) { doc.getValueTemplate(n,value); }
inline void getValue(  XMLDoc &doc, const XMLNodePtr &n, int             &value) { doc.getValueTemplate(n,value); }
inline void getValue(  XMLDoc &doc, const XMLNodePtr &n, UINT            &value) { doc.getValueTemplate(n,value); }
inline void getValue(  XMLDoc &doc, const XMLNodePtr &n, long            &value) { doc.getValueTemplate(n,value); }
inline void getValue(  XMLDoc &doc, const XMLNodePtr &n, ULONG           &value) { doc.getValueTemplate(n,value); }
inline void getValue(  XMLDoc &doc, const XMLNodePtr &n, INT64           &value) { doc.getValueTemplate(n,value); }
inline void getValue(  XMLDoc &doc, const XMLNodePtr &n, UINT64          &value) { doc.getValueTemplate(n,value); }
inline void getValue(  XMLDoc &doc, const XMLNodePtr &n, float           &value) { doc.getValueTemplate(n,value); }
inline void getValue(  XMLDoc &doc, const XMLNodePtr &n, double          &value) { doc.getValueTemplate(n,value); }
inline void getValue(  XMLDoc &doc, const XMLNodePtr &n, Date            &value) { doc.getValueTemplate(n,value); }
inline void getValue(  XMLDoc &doc, const XMLNodePtr &n, Timestamp       &value) { doc.getValueTemplate(n,value); }
inline void getValue(  XMLDoc &doc, const XMLNodePtr &n, bool            &value) { doc.getValueTemplate(n,value); }

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

template<typename T> void setValue(XMLDoc &doc, XMLNodePtr n, const Iterator<T> &it) {
  UINT index = 0;
  for(Iterator<T> it1 = it; it1.hasNext();index++) {
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
