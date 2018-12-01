#include "pch.h"
#include <Tokenizer.h>
#include <XMLDoc.h>

using namespace std;

void XMLDoc::checkResult(HRESULT hr, int line) {
  if(FAILED(hr)) {
    throwException(_T("%s line %d:%s"), __TFILE__, line, getErrorText(hr).cstr());
  }
}

#define V(f) checkResult(f, __LINE__)

static String toXML(const TCHAR *s) {
  String result;
  _TUCHAR ch;
  for(_TUCHAR *t = (_TUCHAR*)s; *t; t++) {
    switch(ch = *t) {
    case _T('\"'):
      result += _T("&quot;");
      break;
    case _T('\''):
      result += _T("&apos;");
      break;
    case _T('<'):
      result += _T("&lt;");
      break;
    case _T('>'):
      result += _T("&gt;");
      break;
    case _T('&'):
      result += _T("&amp;");
      break;
    default:
      if((ch >= 128) || (ch < 32)) {
        result += format(_T("&#x%x;"), ch);
      } else {
        result += ch;
      }
      break;
    }
  }
  return result;
}

XMLDoc::XMLDoc() {
  V(m_doc.CreateInstance(CLSID_DOMDocument));
}

void XMLDoc::clear() {
  BSTR empty(EMPTYSTRING);
  VARIANT_BOOL vb = m_doc->loadXML(empty);
}

void XMLDoc::setDocument(XMLDocPtr doc) {
  m_doc = doc;
}

XMLDoc::XMLDoc(XMLDocPtr doc) {
  m_doc = doc;
}

void XMLDoc::loadFromString(const String &XML) {
  clear();
  String str(XML);

  intptr_t start = str.find(_T("encoding"));
  if(start >= 0) {
    intptr_t slut=str.find(_T(" ?"),start);
    str.remove(start,slut-start);
  }

  m_doc->put_async(VARIANT_FALSE);
  VARIANT_BOOL vb = m_doc->loadXML(str.cstr());
  checkLoad();
}

void XMLDoc::loadFromFile(const TCHAR *fileName) {
  clear();
  m_doc->put_async(VARIANT_FALSE);

  USES_CONVERSION;
  const char *fileNameA = T2A(fileName);
  _variant_t vName(fileNameA);

  VARIANT_BOOL vb = m_doc->load(vName);
  checkLoad();
}

void XMLDoc::saveToFile(const TCHAR *fileName) {
//  LogTrace("SaveToFile<%s>",fileName);
  m_doc->put_async(VARIANT_FALSE);

  USES_CONVERSION;
  const char *fileNameA = T2A(fileName);
  _variant_t vName(fileNameA);

  V(m_doc->save(vName));
}

void XMLDoc::saveToFile(const String &fileName) {
  saveToFile(fileName.cstr());
}

void XMLDoc::loadFromFile(const String &fileName) {
  loadFromFile(fileName.cstr());
}

void XMLDoc::checkLoad() {
  XMLParseErrorPtr pXMLError;
  LONG errorCode = E_FAIL;

  HRESULT hr=m_doc->get_parseError(&pXMLError);
  hr=pXMLError->get_errorCode(&errorCode);
  if(errorCode == S_OK) {
    m_doc->documentElement->normalize();
  } else {
    long line, linePos;
    LONG errorCode;
    BSTR pBURL, pBReason;
    HRESULT hr;

    hr=pXMLError->get_line(&line);
    hr=pXMLError->get_linepos(&linePos);
    hr=pXMLError->get_errorCode(&errorCode);
    hr=pXMLError->get_url(&pBURL);
    hr=pXMLError->get_reason(&pBReason);

    const String msg = format(_T("(%d,%d) : Error(%d) in XML:%s"), line, linePos, errorCode, BSTRToString(pBReason).cstr());

    SysFreeString(pBURL);
    SysFreeString(pBReason);
    throwException(_T("%s"), msg.cstr());
  }
}

String XMLDoc::getXML(bool includePi) {
  m_doc->PutpreserveWhiteSpace(VARIANT_TRUE);
  BSTR b = getRoot()->Getxml();

  String XML = BSTRToString(b);

  ::SysFreeString(b);
  b = NULL;

  if(includePi) {
    XML = _T("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>") + XML;
  }
  return XML.trim();
}

XMLNodePtr XMLDoc::createNode(const TCHAR *nodeName, bool force) {
  return createNode(m_doc->documentElement, nodeName, force);
}

XMLNodePtr XMLDoc::createNode(const XMLNodePtr &parent, const TCHAR *nodeName, bool force) {
  DEFINEMETHODNAME;
  if(parent == NULL) {
    throwInvalidArgumentException(method, _T("parent=NULL"));
  }
  if(nodeName == NULL || _tcsclen(nodeName) == 0) {
    throwInvalidArgumentException(method, _T("nodeName=%s"), nodeName?nodeName:_T("null"));
  }

  StringArray tokens(Tokenizer(nodeName,_T(".")));

  // Create path if we got any dots
  XMLNodePtr node = parent;
  size_t i;
  for(i = 0; i < tokens.size() - 1; i++) {
    XMLNodePtr tmp = findChild(node,tokens[i].cstr());
    if(tmp == NULL) {
      tmp  = m_doc->createElement(tokens[i].cstr());
      node = node->appendChild(tmp);
    }
  }

  XMLNodePtr result = findChild(node,tokens[i].cstr());
  if(result != NULL && !force) {
    throwException(_T("Node %s alredy exist. set force to true, if duplicates are allowed"), nodeName);
  } else {
    result = m_doc->createElement(tokens[i].cstr());
    result = node->appendChild(result);
  }
  return result;
}

XMLNodePtr XMLDoc::createRoot(const TCHAR *rootName) {
  return m_doc->appendChild(m_doc->createElement(rootName));
}

XMLNodePtr XMLDoc::getRoot() {
  return m_doc->documentElement;
}

XMLNodePtr XMLDoc::findChild(const XMLNodePtr &node, const TCHAR *nodeName, int instans) {
  if(node == NULL) {
    return NULL;
  }
  int i = 0;
  for(XMLNodePtr result=node->firstChild; result!=NULL; result=result->nextSibling) {
    if(_tcsicmp(result->nodeName, nodeName)==0 && i++==instans) {
      return result;
    }
  }
  return NULL;
}

XMLNodePtr XMLDoc::findNode(const TCHAR *nodeName) {
  return findNode(m_doc->documentElement,nodeName);
}

XMLNodePtr XMLDoc::findNode(const XMLNodePtr &node, const TCHAR *nodeName) {
  if(node == NULL) {
    return NULL;
  }

  StringArray tokens(Tokenizer(nodeName,_T(".")));

  XMLNodePtr result = node;
  for(size_t i = 0; i < tokens.size() && result != NULL; result = result->nextSibling) {
//    String sss = BSTRToString(node->GetbaseName());
//    printf("tagName:%s\n",sss.cstr());
    if(_tcsicmp(result->nodeName,tokens[i].cstr())==0) {
      i++;
      if(i < tokens.size()) {
        result = result->firstChild;
      }
    }
  }

  return result;
}

String XMLDoc::getText(const XMLNodePtr &node) {
  return (TCHAR*)findText(node)->text;
}

XMLNodePtr XMLDoc::findText(const XMLNodePtr &node) {
  for(XMLNodePtr p = node->firstChild; p != NULL; p = p->nextSibling) {
    if(p->nodeType == NODE_TEXT) {
      return p;
    }
  }
  return NULL;
}

void XMLDoc::setText(const XMLNodePtr &node, const TCHAR *value) {
  if(node == NULL) {
    return;
  }
  XMLNodePtr Text = findText(node);
  if(Text == NULL) {
    node->appendChild(m_doc->createTextNode(value));
  } else {
    Text->nodeValue = value;
  }
}

// ---------------------------------------------------------------------------------------------------

void XMLDoc::setValue(const XMLNodePtr     &node,     const TCHAR *tagName, const String    &value, bool force) {
  setValue(node,tagName,value.cstr(),force);
}

void XMLDoc::setValue(const XMLNodePtr     &node,     const TCHAR *tagName, const TCHAR *value, bool force) {
  setText(force ? createNode(node,tagName,force) : findChild(node,tagName), value);
}

void XMLDoc::setValue(const XMLNodePtr     &node,     const TCHAR *tagName, int              value, bool force) {
  TCHAR buf[12];
  setValue(node,tagName,_itot(value,buf,10),force);
}

void XMLDoc::setValue(const XMLNodePtr     &node,     const TCHAR *tagName, UINT             value, bool force) {
  setValue(node,tagName,format(_T("%u"), value).cstr(),force);
}

void XMLDoc::setValue(const XMLNodePtr     &node,     const TCHAR *tagName, long             value, bool force) {
  setValue(node,tagName,(int)value,force);
}

void XMLDoc::setValue(const XMLNodePtr     &node,     const TCHAR *tagName, ULONG            value, bool force) {
  setValue(node,tagName,(UINT)value,force);
}

void XMLDoc::setValue(const XMLNodePtr     &node,     const TCHAR *tagName, INT64            value, bool force) {
  setValue(node,tagName,format(_T("%I64d"), value).cstr(),force);
}

void XMLDoc::setValue(const XMLNodePtr     &node,     const TCHAR *tagName, UINT64           value, bool force) {
  setValue(node,tagName,format(_T("%I64u"), value).cstr(),force);
}

void XMLDoc::setValue(const XMLNodePtr     &node,     const TCHAR *tagName, float            value, bool force) {
  if(     isInt64( value)) setValue(node,tagName,getInt64( value),force);
  else if(isUint64(value)) setValue(node,tagName,getUint64(value),force);
  else                     setValue(node,tagName,toString( value,7,0,ios::scientific).cstr(),force);
}

void XMLDoc::setValue(const XMLNodePtr     &node,     const TCHAR *tagName, double           value, bool force) {
  if(     isInt64( value)) setValue(node,tagName,getInt64( value),force);
  else if(isUint64(value)) setValue(node,tagName,getUint64(value),force);
  else if(isFloat( value)) setValue(node,tagName,getFloat( value),force);
  else                     setValue(node,tagName,toString( value,15,0,ios::scientific).cstr(),force);
}

void XMLDoc::setValue(const XMLNodePtr     &node,     const TCHAR *tagName, const Date      &value, bool force) {
  setValue(node,tagName,value.toString(),force);
}

void XMLDoc::setValue(const XMLNodePtr     &node,     const TCHAR *tagName, const Timestamp &value, bool force) {
  setValue(node,tagName,value.toString(ddMMyyyyhhmmss),force);
}

void XMLDoc::setValue(const XMLNodePtr     &node,     const TCHAR *tagName, bool             value, bool force) {
  setValue(node,tagName,value ? 1 : 0   ,force);
}
// ---------------------------------------------------------------------------------------------------

void XMLDoc::getValue(const XMLNodePtr     &node,     const TCHAR *tagName, String      &value, int instans) {
  VARIANT NodeValue = getVariant(node,tagName,instans);

  switch(NodeValue.vt) {
  case VT_BSTR:
    { _bstr_t b = NodeValue.bstrVal;
      value = (TCHAR*)b;
      value.trim();
    }
    break;

  default:
    value=EMPTYSTRING;
    break;
  }
}

static void throwNotNumericException(const TCHAR *tagName, const String &str) {
  throwException(_T("Invalid content. Tag=\"%s\". content=\"%s\". Must be numeric"),tagName,str.cstr());
}

void XMLDoc::getValueLF(const XMLNodePtr   &node,     const TCHAR *tagName, String      &value, int instans) {
  getValue(node, tagName, value, instans);
  value.replace(_T('\n'), _T("\r\n"));
}

void XMLDoc::getValue(const XMLNodePtr &node, const TCHAR *tagName, int         &value, int instans) {
  String str;
  getValue(node,tagName,str,instans);
  if(_stscanf(str.cstr(),_T("%d"),&value) != 1) {
    throwNotNumericException(tagName, str);
  }
}

void XMLDoc::getValue(const XMLNodePtr &node, const TCHAR *tagName, UINT        &value, int instans) {
  String str;
  getValue(node,tagName,str,instans);
  if(_stscanf(str.cstr(),_T("%u"),&value) != 1) {
    throwNotNumericException(tagName, str);
  }
}

void XMLDoc::getValue(const XMLNodePtr &node, const TCHAR *tagName, long        &value, int instans) {
  getValue(node,tagName,(int&)value,instans);
}

void XMLDoc::getValue(const XMLNodePtr &node, const TCHAR *tagName, ULONG       &value, int instans) {
  getValue(node,tagName,(UINT&)value,instans);
}

void XMLDoc::getValue(const XMLNodePtr &node, const TCHAR *tagName, INT64       &value, int instans) {
  String str;
  getValue(node,tagName,str,instans);
  if(_stscanf(str.cstr(),_T("%I64d"),&value) != 1) {
    throwNotNumericException(tagName, str);
  }
}

void XMLDoc::getValue(const XMLNodePtr &node, const TCHAR *tagName, UINT64      &value, int instans) {
  String str;
  getValue(node,tagName,str,instans);
  if(_stscanf(str.cstr(),_T("%I64u"),&value) != 1) {
    throwNotNumericException(tagName, str);
  }
}

void XMLDoc::getValue(const XMLNodePtr &node, const TCHAR *tagName, float       &value, int instans) {
  double tmp;
  getValue(node,tagName,tmp,instans);
  value = (float)tmp;
}

void XMLDoc::getValue(const XMLNodePtr &node, const TCHAR *tagName, double      &value, int instans) {
  String str;
  getValue(node,tagName,str,instans);
  if(_stscanf(str.cstr(),_T("%le"),&value) != 1) {
    throwNotNumericException(tagName, str);
  }
}

void XMLDoc::getValue(const XMLNodePtr &node, const TCHAR *tagName, Date        &value, int instans) {
  String str;
  getValue(node, tagName, str, instans);
  value = Date(str);
}

void XMLDoc::getValue(const XMLNodePtr &node, const TCHAR *tagName, Timestamp   &value, int instans) {
  String str;
  getValue(node, tagName, str, instans);
  value = Timestamp(str);
}

void XMLDoc::getValue(const XMLNodePtr &node, const TCHAR *tagName, bool        &value, int instans) {
  int v;
  getValue(node, tagName, v, instans);
  value = v != 0;
}
// -----------------------------------------------------------------------------------------

VARIANT XMLDoc::getVariant(const TCHAR *nodeName, const TCHAR *tagName, int instans) {
  return getVariant(findNode(nodeName),tagName,instans);
}

VARIANT XMLDoc::getVariant(const XMLNodePtr &node, const TCHAR *tagName, int instans) {
  VARIANT result;
  result.vt = NULL;

  if(node!=NULL) {
    int i=0;
    for(XMLNodePtr p = node->firstChild; p != NULL; p = p->nextSibling) {
      if(p->nodeType==NODE_ELEMENT &&
          _tcsicmp(((XMLElementPtr)p)->tagName,tagName) == 0 &&
          i++ == instans) {
        IXMLDOMNodeListPtr childList = p->childNodes;
        LONG length;
        childList->get_length(&length);
        for(int j = 0; j < length; j++) {
          IXMLDOMNodePtr item;
          childList->get_item(j, &item);
          DOMNodeType itemType;
          item->get_nodeType(&itemType);
          if(itemType == NODE_TEXT) {
            item->get_nodeValue(&result);
            return result;
          }
        }
        return result;
      }
    }
  }
  return result;
}
