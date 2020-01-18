#include "stdafx.h"
#include "MainFrm.h"
#include "D3FunctionDoc.h"
#include "D3SceneView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CD3SceneView, CView)

BEGIN_MESSAGE_MAP(CD3SceneView, CView)
    ON_WM_SIZE()
    ON_COMMAND(ID_FILE_PRINT        , OnFilePrint       )
    ON_COMMAND(ID_FILE_PRINT_DIRECT , OnFilePrint       )
END_MESSAGE_MAP()

CD3SceneView::CD3SceneView() {
}

CD3SceneView::~CD3SceneView() {
}

BOOL CD3SceneView::OnPreparePrinting(CPrintInfo *pInfo) {
  return DoPreparePrinting(pInfo);
}

void CD3SceneView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/) {
}

void CD3SceneView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/) {
}

#ifdef _DEBUG
void CD3SceneView::AssertValid() const {
  __super::AssertValid();
}

void CD3SceneView::Dump(CDumpContext& dc) const {
  __super::Dump(dc);
}

CD3FunctionDoc* CD3SceneView::GetDocument() { // non-debug version is inline
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CD3FunctionDoc)));
  return (CD3FunctionDoc*)m_pDocument;
}
#endif //_DEBUG

D3Scene *CD3SceneView::getScene() {
  CMainFrame *mainFrm = theApp.getMainFrame();
  return mainFrm ? &mainFrm->getScene() : NULL;
}

void CD3SceneView::OnDraw(CDC *pDC) {
  D3Scene *scene = getScene();
  if(scene) scene->render();
}

void CD3SceneView::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  D3Scene *scene = getScene();
  if(scene) scene->OnSize();
}
