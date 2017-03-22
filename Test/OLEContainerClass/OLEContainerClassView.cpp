
#include "stdafx.h"
#include "OLEContainerClass.h"

#include "OLEContainerClassDoc.h"
#include "OLEContainerClassView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(COLEContainerClassView, CView)

BEGIN_MESSAGE_MAP(COLEContainerClassView, CView)
    ON_COMMAND(ID_CANCEL_EDIT_SRVR, OnCancelEditSrvr)
END_MESSAGE_MAP()

COLEContainerClassView::COLEContainerClassView() {
    // TODO: add construction code here

}

COLEContainerClassView::~COLEContainerClassView() {
}

BOOL COLEContainerClassView::PreCreateWindow(CREATESTRUCT &cs) {
    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// COLEContainerClassView drawing

void COLEContainerClassView::OnDraw(CDC *pDC) {
    COLEContainerClassDoc *pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    // TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// OLE Server support

// The following command handler provides the standard keyboard
//  user interface to cancel an in-place editing session.  Here,
//  the server (not the container) causes the deactivation.
void COLEContainerClassView::OnCancelEditSrvr() {
    GetDocument()->OnDeactivateUI(FALSE);
}

#ifdef _DEBUG
void COLEContainerClassView::AssertValid() const {
    CView::AssertValid();
}

void COLEContainerClassView::Dump(CDumpContext &dc) const {
    CView::Dump(dc);
}

COLEContainerClassDoc *COLEContainerClassView::GetDocument() {
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COLEContainerClassDoc)));
    return (COLEContainerClassDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COLEContainerClassView message handlers
