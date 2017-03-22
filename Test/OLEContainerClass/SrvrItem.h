#pragma once

class COLEContainerClassSrvrItem : public COleServerItem {
    DECLARE_DYNAMIC(COLEContainerClassSrvrItem)

public:
    COLEContainerClassSrvrItem(COLEContainerClassDoc *pContainerDoc);

    COLEContainerClassDoc *GetDocument() const
        { return (COLEContainerClassDoc*)COleServerItem::GetDocument(); }

    // ClassWizard generated virtual function overrides
public:
    virtual BOOL OnDraw(CDC *pDC, CSize& rSize);
    virtual BOOL OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize);

    ~COLEContainerClassSrvrItem();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext &dc) const;
#endif

protected:
    virtual void Serialize(CArchive &ar);   // overridden for document i/o
};

