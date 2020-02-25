#include "stdafx.h"
#include <FileNameSplitter.h>
#include "MainFrm.h"
#include "D3Function.h"
#include "D3FunctionDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CD3FunctionDoc, CDocument)

BEGIN_MESSAGE_MAP(CD3FunctionDoc, CDocument)
END_MESSAGE_MAP()

CD3FunctionDoc::CD3FunctionDoc() {
  initOptions(getOptions());
  TCHAR **argv = __targv;
  argv++;
}

void CD3FunctionDoc::initOptions(const Options &options) {
}

CD3FunctionDoc::~CD3FunctionDoc() {
}

BOOL CD3FunctionDoc::OnNewDocument() {
  if(!CDocument::OnNewDocument()) {
    return FALSE;
  }

  // TODO: add reinitialization code here
  // (SDI documents will reuse this document)

  return TRUE;
}

void CD3FunctionDoc::Serialize(CArchive& ar) {
  if (ar.IsStoring()) {
  } else {
  }
}

#ifdef _DEBUG
void CD3FunctionDoc::AssertValid() const {
    CDocument::AssertValid();
}

void CD3FunctionDoc::Dump(CDumpContext& dc) const {
    CDocument::Dump(dc);
}
#endif //_DEBUG

