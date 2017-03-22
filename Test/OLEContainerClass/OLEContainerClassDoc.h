#pragma once

class COLEContainerClassSrvrItem;

class COLEContainerClassDoc : public COleServerDoc {
protected: // create from serialization only
    COLEContainerClassDoc();
    DECLARE_DYNCREATE(COLEContainerClassDoc)

public:
    COLEContainerClassSrvrItem* GetEmbeddedItem()
        { return (COLEContainerClassSrvrItem*)COleServerDoc::GetEmbeddedItem(); }

    virtual COleServerItem *OnGetEmbeddedItem();
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive &ar);

public:
    virtual ~COLEContainerClassDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext &dc) const;
#endif

protected:

protected:
    DECLARE_MESSAGE_MAP()

    DECLARE_DISPATCH_MAP()
    DECLARE_INTERFACE_MAP()
};

