#pragma once

#include "Date.h"
#include <comdef.h>
#include <atlconv.h>

#pragma warning(disable:4192)

#import <msxml6.dll> named_guids

typedef MSXML2::IXMLDOMDocumentPtr   XMLDocPtr;
typedef MSXML2::IXMLDOMNodePtr       XMLNodePtr;
typedef MSXML2::IXMLDOMElementPtr    XMLElementPtr;
typedef MSXML2::IXMLDOMParseErrorPtr XMLParseErrorPtr;

class XMLDoc {
private:
  XMLDocPtr m_doc;
  VARIANT    getVariant(const TCHAR      *nodeName, const TCHAR *tagName, int instans=0);
  VARIANT    getVariant(const XMLNodePtr &node    , const TCHAR *tagName, int instans=0);
  XMLNodePtr findText(  const XMLNodePtr &node);
  XMLDocPtr getDocument() {
    return m_doc;
  }
  void checkResult(HRESULT hr, int line);
  void checkLoad();
  void recursiveDelete(XMLNodePtr &node);
  void loadFromFile(const TCHAR *filename);
  void saveToFile(  const TCHAR *filename);
public:
  void setDocument(XMLDocPtr doc);
  XMLDoc();
  XMLDoc(XMLDocPtr doc);
  void clear();
  String getXML(bool includePi=false);
  void loadFromFile(const String &filename);
  void saveToFile(  const String &filename);

  void loadFromString(const String &XML);

  String getText(   const XMLNodePtr &node);
  void setText(     const XMLNodePtr &node, const TCHAR *value) ;

  XMLNodePtr findNode( const TCHAR *nodeName);
  XMLNodePtr findNode( const XMLNodePtr &node, const TCHAR *nodeName);
  XMLNodePtr findChild(const XMLNodePtr &node, const TCHAR *nodeName, int instans=0);

  XMLNodePtr createNode(const TCHAR *nodeName, bool force=true);
  XMLNodePtr createNode(const XMLNodePtr &parent, const TCHAR *nodeName, bool force=true);
  XMLNodePtr createRoot(const TCHAR *rootName);
  XMLNodePtr getRoot();

  inline void getValue(const TCHAR *nodeName, const TCHAR *tagName, String    &value, int  instans = 0) {
    getValue(findNode(nodeName),tagName,value,instans);
  }
  inline void getValueLF(const TCHAR *nodeName, const TCHAR *tagName, String  &value, int  instans = 0) {
    getValueLF(findNode(nodeName),tagName,value,instans);
  }
  inline void getValue(const TCHAR *nodeName, const TCHAR *tagName, int       &value, int  instans = 0) {
    getValue(findNode(nodeName),tagName,value,instans);
  }
  inline void getValue(const TCHAR *nodeName, const TCHAR *tagName, UINT      &value, int  instans = 0) {
    getValue(findNode(nodeName),tagName,value,instans);
  }
  inline void getValue(const TCHAR *nodeName, const TCHAR *tagName, long      &value, int  instans = 0) {
    getValue(findNode(nodeName),tagName,value,instans);
  }
  inline void getValue(const TCHAR *nodeName, const TCHAR *tagName, ULONG     &value, int  instans = 0) {
    getValue(findNode(nodeName),tagName,value,instans);
  }
  inline void getValue(const TCHAR *nodeName, const TCHAR *tagName, INT64     &value, int  instans = 0) {
    getValue(findNode(nodeName),tagName,value,instans);
  }
  inline void getValue(const TCHAR *nodeName, const TCHAR *tagName, UINT64    &value, int  instans = 0) {
    getValue(findNode(nodeName),tagName,value,instans);
  }
  inline void getValue(const TCHAR *nodeName, const TCHAR *tagName, float     &value, int  instans = 0) {
    getValue(findNode(nodeName),tagName,value,instans);
  }
  inline void getValue(const TCHAR *nodeName, const TCHAR *tagName, double    &value, int  instans = 0) {
    getValue(findNode(nodeName),tagName,value,instans);
  }
  inline void getValue(const TCHAR *nodeName, const TCHAR *tagName, Date      &value, int  instans = 0) {
    getValue(findNode(nodeName),tagName,value,instans);
  }
  inline void getValue(const TCHAR *nodeName, const TCHAR *tagName, Timestamp &value, int  instans = 0) {
    getValue(findNode(nodeName),tagName,value,instans);
  }
  inline void getValue(const TCHAR *nodeName, const TCHAR *tagName, bool      &value, int  instans = 0) {
    getValue(findNode(nodeName),tagName,value,instans);
  }

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

  inline void setValue(const TCHAR *nodeName, const TCHAR *tagName, const String    &value, bool force=true) {
    setValue(findNode(nodeName),tagName,value.cstr(),force);
  }
  inline void setValue(const TCHAR *nodeName, const TCHAR *tagName, const TCHAR     *value, bool force = true) {
    setValue(findNode(nodeName),tagName,value,force);
  }
  inline void setValue(const TCHAR *nodeName, const TCHAR *tagName, int              value, bool force = true) {
    setValue(findNode(nodeName),tagName,value,force);
  }
  inline void setValue(const TCHAR *nodeName, const TCHAR *tagName, UINT             value, bool force = true) {
    setValue(findNode(nodeName),tagName,value,force);
  }
  inline void setValue(const TCHAR *nodeName, const TCHAR *tagName, long             value, bool force = true) {
    setValue(findNode(nodeName),tagName,value,force);
  }
  inline void setValue(const TCHAR *nodeName, const TCHAR *tagName, ULONG            value, bool force = true) {
    setValue(findNode(nodeName),tagName,value,force);
  }
  inline void setValue(const TCHAR *nodeName, const TCHAR *tagName, INT64            value, bool force = true) {
    setValue(findNode(nodeName),tagName,value,force);
  }
  inline void setValue(const TCHAR *nodeName, const TCHAR *tagName, UINT64           value, bool force = true) {
    setValue(findNode(nodeName),tagName,value,force);
  }
  inline void setValue(const TCHAR *nodeName, const TCHAR *tagName, float            value, bool force = true) {
    setValue(findNode(nodeName),tagName,value,force);
  }
  inline void setValue(const TCHAR *nodeName, const TCHAR *tagName, double           value, bool force = true) {
    setValue(findNode(nodeName),tagName,value,force);
  }
  inline void setValue(const TCHAR *nodeName, const TCHAR *tagName, const Date      &value, bool force = true) {
    setValue(findNode(nodeName),tagName,value,force);
  }
  inline void setValue(const TCHAR *nodeName, const TCHAR *tagName, const Timestamp &value, bool force = true) {
    setValue(findNode(nodeName),tagName,value,force);
  }
  inline void setValue(const TCHAR *nodeName, const TCHAR *tagName, bool             value, bool force = true) {
    setValue(findNode(nodeName),tagName,value,force);
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
};

String BSTRToString(const BSTR &s);
