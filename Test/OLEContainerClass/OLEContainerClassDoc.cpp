// OLEContainerClassDoc.cpp : implementation of the COLEContainerClassDoc class
//

#include "stdafx.h"
#include "OLEContainerClass.h"

#include "OLEContainerClassDoc.h"
#include "SrvrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// COLEContainerClassDoc

IMPLEMENT_DYNCREATE(COLEContainerClassDoc, COleServerDoc)

BEGIN_MESSAGE_MAP(COLEContainerClassDoc, COleServerDoc)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(COLEContainerClassDoc, COleServerDoc)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //      DO NOT EDIT what you see in these blocks of generated code!
END_DISPATCH_MAP()

// Note: we add support for IID_IOLEContainerClass to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {2A8FCBEA-37C2-4561-BD3F-E3117DEAF484}
static const IID IID_IOLEContainerClass =
{ 0x2a8fcbea, 0x37c2, 0x4561, { 0xbd, 0x3f, 0xe3, 0x11, 0x7d, 0xea, 0xf4, 0x84 } };

BEGIN_INTERFACE_MAP(COLEContainerClassDoc, COleServerDoc)
    INTERFACE_PART(COLEContainerClassDoc, IID_IOLEContainerClass, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COLEContainerClassDoc construction/destruction

COLEContainerClassDoc::COLEContainerClassDoc()
{
    // Use OLE compound files
    EnableCompoundFile();

    // TODO: add one-time construction code here

    EnableAutomation();

    AfxOleLockApp();
}

COLEContainerClassDoc::~COLEContainerClassDoc()
{
    AfxOleUnlockApp();
}

BOOL COLEContainerClassDoc::OnNewDocument() {
    if (!COleServerDoc::OnNewDocument())
        return FALSE;

    // TODO: add reinitialization code here
    // (SDI documents will reuse this document)

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// COLEContainerClassDoc server implementation

COleServerItem *COLEContainerClassDoc::OnGetEmbeddedItem() {
    // OnGetEmbeddedItem is called by the framework to get the COleServerItem
    //  that is associated with the document.  It is only called when necessary.

    COLEContainerClassSrvrItem* pItem = new COLEContainerClassSrvrItem(this);
    ASSERT_VALID(pItem);
    return pItem;
}



void COLEContainerClassDoc::Serialize(CArchive &ar) {
    if (ar.IsStoring()) {
    } else {
    }
}

#ifdef _DEBUG
void COLEContainerClassDoc::AssertValid() const {
    COleServerDoc::AssertValid();
}

void COLEContainerClassDoc::Dump(CDumpContext &dc) const {
    COleServerDoc::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COLEContainerClassDoc commands
