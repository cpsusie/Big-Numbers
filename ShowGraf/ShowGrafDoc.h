#pragma once

class CShowGrafDoc : public CDocument {
protected:
	CShowGrafDoc();
	DECLARE_DYNCREATE(CShowGrafDoc)

public:

	//{{AFX_VIRTUAL(CShowGrafDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

public:
	virtual ~CShowGrafDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	//{{AFX_MSG(CShowGrafDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
