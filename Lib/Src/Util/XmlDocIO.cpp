#include "pch.h"
#include <MyUtil.h>
#include <XMLDoc.h>

#define V(f) checkResult(f, __TFILE__, __LINE__)

void XMLDoc::clear() {
  BSTR empty(EMPTYSTRING);
  VARIANT_BOOL vb = m_doc->loadXML(empty);
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
#ifndef NEWVERSION
  USES_CONVERSION;
  const char *fileNameA = T2A(fileName);
  _variant_t vName(fileNameA);
  V(m_doc->save(vName));
#else
  m_doc->PutpreserveWhiteSpace(VARIANT_FALSE);
  MSXML2::IMXWriterPtr writer;
  V(writer.CreateInstance(MSXML2::CLSID_MXXMLWriter60));
  writer->Putindent(VARIANT_TRUE);
  MSXML2::ISAXContentHandler *contentHandler;
  MSXML2::ISAXErrorHandler   *errorHandler;
  MSXML2::ISAXDTDHandler     *dtdHandler;
  MSXML2::ISAXDeclHandler    *declHandler;
  MSXML2::ISAXLexicalHandler *lexHandler;
  V(writer.QueryInterface(MSXML2::IID_ISAXErrorHandler  , &contentHandler));
  V(writer.QueryInterface(MSXML2::IID_ISAXErrorHandler  , &errorHandler  ));
  V(writer.QueryInterface(MSXML2::IID_ISAXDTDHandler    , &dtdHandler    ));
  V(writer.QueryInterface(MSXML2::IID_ISAXDeclHandler   , &declHandler   ));
  V(writer.QueryInterface(MSXML2::IID_ISAXLexicalHandler, &lexHandler    ));

  MSXML2::ISAXXMLReaderPtr saxReader;
  V(saxReader.CreateInstance(MSXML2::CLSID_SAXXMLReader60));
  V(saxReader->putContentHandler(contentHandler));
  V(saxReader->putErrorHandler(  errorHandler  ));
  V(saxReader->putDTDHandler(    dtdHandler    ));
  V(saxReader->putProperty((USHORT*)L"http://xml.org/sax/properties/declaration-handler", declHandler));
  V(saxReader->putProperty((USHORT*)L"http://xml.org/sax/properties/lexical-handler"    , lexHandler ));

  IStream *iStream = NULL;
  V(StgCreateStorageEx(fileName, STGM_CREATE | STGM_READWRITE | STGM_SHARE_EXCLUSIVE, STGFMT_STORAGE, 0, NULL, NULL, IID_IStream, (void**)&iStream));
  ISequentialStream *seqStreamPtr;
  V(iStream->QueryInterface(IID_ISequentialStream, (void**)&seqStreamPtr));

  writer->Putoutput(&seqStreamPtr);
  IDispatch *writerDispatchPtr;
  V(writer.QueryInterface(IID_IDispatch, &writerDispatchPtr));
  _variant_t wrv = writerDispatchPtr;
  V(m_doc->save(wrv));
  V(writer->flush());
#endif // NEWVERSION
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
