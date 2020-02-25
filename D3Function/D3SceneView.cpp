#include "stdafx.h"
#include "D3Function.h"
#include "MainFrm.h"
#include <D3DGraphics/D3Camera.h>
#include "D3FunctionDoc.h"
#include "D3SceneView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(C3DSceneView, CView)

BEGIN_MESSAGE_MAP(C3DSceneView, CView)
  ON_WM_SIZE()
  ON_COMMAND(ID_FILE_PRINT        , OnFilePrint )
  ON_COMMAND(ID_FILE_PRINT_DIRECT , OnFilePrint )
  ON_WM_CREATE()
  ON_WM_DESTROY()
END_MESSAGE_MAP()

C3DSceneView::C3DSceneView() {
  m_camera = NULL;
}

C3DSceneView::~C3DSceneView() {
}

int C3DSceneView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if(__super::OnCreate(lpCreateStruct) == -1) {
    return -1;
  }

  theApp.m_3DViewArray.add(this);
  return 0;
}


void C3DSceneView::OnDestroy() {
  theApp.m_3DViewArray.remove(this);
  __super::OnDestroy();
}

BOOL C3DSceneView::OnPreparePrinting(CPrintInfo *pInfo) {
  return DoPreparePrinting(pInfo);
}

void C3DSceneView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/) {
}

void C3DSceneView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/) {
}

#ifdef _DEBUG
void C3DSceneView::AssertValid() const {
  __super::AssertValid();
}

void C3DSceneView::Dump(CDumpContext& dc) const {
  __super::Dump(dc);
}

CD3FunctionDoc* C3DSceneView::GetDocument() { // non-debug version is inline
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CD3FunctionDoc)));
  return (CD3FunctionDoc*)m_pDocument;
}
#endif //_DEBUG

D3Camera *C3DSceneView::findCamera() const {
  const D3Scene *scene = getScene();
  return scene ? scene->getCameraArray().findCamera(*this) : NULL;
}

D3Camera *C3DSceneView::getCamera() {
  if((m_camera == NULL) || (m_camera->getHwnd() != *this)) {
    m_camera = findCamera();
  }
  return m_camera;
}

D3Scene *C3DSceneView::getScene() const {
  CMainFrame *mainFrm = theApp.getMainFrame();
  return mainFrm ? &mainFrm->getScene() : NULL;
}

void C3DSceneView::OnDraw(CDC *pDC) {
  D3Camera *cam = getCamera();
  if(cam) {
    cam->render();
  }
}

void C3DSceneView::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  D3Camera *cam = getCamera();
  if(cam) {
    cam->OnSize();
  }
}
