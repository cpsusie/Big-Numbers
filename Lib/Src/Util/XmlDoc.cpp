#include "pch.h"
#include <MyUtil.h>
#include <StringArray.h>
#include <Tokenizer.h>
#include <XMLDoc.h>

void XMLDoc::checkResult(HRESULT hr, const TCHAR *fileName, int line) { // static
  if(FAILED(hr)) {
    throwException(_T("%s line %d:%s"), fileName, line, getErrorText(hr).cstr());
  }
}

#define V(f) checkResult(f, __TFILE__, __LINE__)

XMLDoc::XMLDoc() {
  V(m_doc.CreateInstance(CLSID_DOMDocument));
}

void XMLDoc::setDocument(XMLDocPtr doc) {
  m_doc = doc;
}

XMLDoc::XMLDoc(XMLDocPtr doc) {
  m_doc = doc;
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

  XMLNodePtr result;
  if(_tcschr(nodeName, '.') == NULL) {
    result = findChild(parent,nodeName);
    if(result != NULL && !force) {
      throwException(_T("Node %s alredy exist. set force to true, if duplicates are allowed"), nodeName);
    } else {
      result = m_doc->createElement(nodeName);
      result = parent->appendChild(result);
    }
  } else {
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

XMLNodePtr XMLDoc::getChild(const XMLNodePtr &node, const TCHAR *nodeName, int instans) {
  XMLNodePtr child = findChild(node, nodeName, instans);
  if(child == NULL) {
    throwException(_T("ChildNode with name=\"%s\" not found in node %s"), nodeName, (TCHAR*)node->nodeName);
  }
  return child;
}

XMLNodePtr XMLDoc::findNode(const TCHAR *nodeName) {
  return findNode(m_doc->documentElement,nodeName);
}

XMLNodePtr XMLDoc::findNode(const XMLNodePtr &node, const TCHAR *nodeName) {
  if(node == NULL) {
    return NULL;
  }
  if(_tcschr(nodeName, '.') == NULL) {
    return findChild(node, nodeName);
  } else {
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
}

void XMLDoc::checkTag(XMLNodePtr &node, const TCHAR *expectedTag) { // static
  if(_tcsicmp(node->nodeName, expectedTag) != 0) {
    throwException(_T("nodename:%s. Expected name:%s"), (TCHAR*)node->nodeName, expectedTag);
  }
}

String &XMLDoc::variantToString(String &dst, const VARIANT &var) { // static
  switch(var.vt) {
  case VT_BSTR:
    { _bstr_t b = var.bstrVal;
      dst = (TCHAR*)b;
      dst.trim();
    }
    break;

  default:
    dst=EMPTYSTRING;
    break;
  }
  return dst;
}

String &XMLDoc::getNodeText(const XMLNodePtr &node, String &value) {
  return variantToString(value, getVariant(node));
}

XMLNodePtr XMLDoc::findTextNode(const XMLNodePtr &node) {
  for(XMLNodePtr p = node->firstChild; p != NULL; p = p->nextSibling) {
    if(p->nodeType == NODE_TEXT) {
      return p;
    }
  }
  return NULL;
}

void XMLDoc::setNodeText(const XMLNodePtr &node, const TCHAR *value) {
  if(node == NULL) {
    return;
  }
  XMLNodePtr textNode = findTextNode(node);
  if(textNode == NULL) {
    node->appendChild(m_doc->createTextNode(value));
  } else {
    textNode->nodeValue = value;
  }
}

VARIANT XMLDoc::getVariant(const XMLNodePtr &node) {
  VARIANT result;
  result.vt = NULL;
  XMLNodeType type;
  node->get_nodeType(&type);
  switch(type) {
  case NODE_TEXT  :
    node->get_nodeValue(&result);
    break;
  case NODE_ELEMENT:
    { XMLNodePtr textNode = findTextNode(node);
      if(textNode) {
        textNode->get_nodeValue(&result);
      }
    }
    break;
  }
  return result;
}
