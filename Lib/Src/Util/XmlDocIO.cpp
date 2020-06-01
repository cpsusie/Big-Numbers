#include "pch.h"
#include <MyUtil.h>
#include <comutil.h>
#include <comdef.h>
#include <comdefsp.h>
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

#define NEWVERSION

#ifdef NEWVERSION

// see https://stackoverflow.com/questions/164575/msxml-from-c-pretty-print-indent-newly-created-documents/36982487#36982487

#import "msado15.dll" rename("EOF", "EndOfFile")  // requires: /I $(CommonProgramFiles)\System\ado

void XMLDoc::prettyWriteXmlDocument(MSXML2::IXMLDOMDocument *xmlDoc, IStream *stream) { // static
  MSXML2::IMXWriterPtr writer(__uuidof(MSXML2::MXXMLWriter60));
  writer->encoding   = L"utf-8";
  writer->indent     = _variant_t(true);
  writer->standalone = _variant_t(true);
  writer->output     = stream;

  MSXML2::ISAXXMLReaderPtr saxReader(__uuidof(MSXML2::SAXXMLReader60));
  saxReader->putContentHandler(MSXML2::ISAXContentHandlerPtr(writer));
  saxReader->putProperty(PUSHORT(L"http://xml.org/sax/properties/lexical-handler"), writer.GetInterfacePtr());
  saxReader->parse(xmlDoc);
}

void XMLDoc::prettySaveXmlDocument(MSXML2::IXMLDOMDocument *xmlDoc, const wchar_t *filePath) { // static
  ADODB::_StreamPtr stream(__uuidof(ADODB::Stream));
  stream->Type = ADODB::adTypeBinary;
  stream->Open(vtMissing, ADODB::adModeUnknown, ADODB::adOpenStreamUnspecified, _bstr_t(), _bstr_t());
  prettyWriteXmlDocument(xmlDoc, IStreamPtr(stream));
  stream->SaveToFile(filePath, ADODB::adSaveCreateOverWrite);
}

#endif

void XMLDoc::saveToFile(const TCHAR *fileName) {
#ifndef NEWVERSION
  m_doc->put_async(VARIANT_FALSE);
  USES_CONVERSION;
  const char *fileNameA = T2A(fileName);
  _variant_t vName(fileNameA);
  V(m_doc->save(vName));
#else
  try {
    prettySaveXmlDocument(m_doc, fileName);
  } catch(_com_error &e) {
    throw Exception(BSTRToString(e.Description()));
  }
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
