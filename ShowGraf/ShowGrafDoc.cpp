#include "stdafx.h"
#include "ShowGraf.h"

#include "ShowGrafDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CShowGrafDoc, CDocument)

BEGIN_MESSAGE_MAP(CShowGrafDoc, CDocument)
  //{{AFX_MSG_MAP(CShowGrafDoc)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

CShowGrafDoc::CShowGrafDoc() {
}

CShowGrafDoc::~CShowGrafDoc() {
}

BOOL CShowGrafDoc::OnNewDocument() {
  if (!CDocument::OnNewDocument())
    return FALSE;

  return TRUE;
}

void CShowGrafDoc::Serialize(CArchive& ar) {
  if (ar.IsStoring()) {
  } else {
  }
}

#ifdef _DEBUG
void CShowGrafDoc::AssertValid() const {
  CDocument::AssertValid();
}

void CShowGrafDoc::Dump(CDumpContext& dc) const {
  CDocument::Dump(dc);
}
#endif //_DEBUG
