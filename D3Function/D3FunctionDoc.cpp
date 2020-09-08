#include "stdafx.h"
#include "MainFrm.h"
#include "D3Function.h"
#include "D3FunctionDoc.h"

#if defined(_DEBUG)
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
  if(!__super::OnNewDocument()) {
    return FALSE;
  }

  return TRUE;
}

void CD3FunctionDoc::Serialize(CArchive& ar) {
  if(ar.IsStoring()) {
  } else {
  }
}

#if defined(_DEBUG)
void CD3FunctionDoc::AssertValid() const {
  __super::AssertValid();
}

void CD3FunctionDoc::Dump(CDumpContext& dc) const {
  __super::Dump(dc);
}
#endif //_DEBUG

