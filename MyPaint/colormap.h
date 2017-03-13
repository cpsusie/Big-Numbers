#if !defined(AFX_COLORMAP_H__9C2D1B75_0AF1_40AF_97D6_FA8BB5F2D88D__INCLUDED_)
#define AFX_COLORMAP_H__9C2D1B75_0AF1_40AF_97D6_FA8BB5F2D88D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.

/////////////////////////////////////////////////////////////////////////////
// CColormap wrapper class

class CColormap : public CWnd
{
protected:
    DECLARE_DYNCREATE(CColormap)
public:
    CLSID const& GetClsid()
    {
        static CLSID const clsid
            = { 0x5c04522b, 0xc18b, 0x4485, { 0x83, 0x89, 0x5c, 0x89, 0x6b, 0xf5, 0x98, 0x81 } };
        return clsid;
    }
    virtual BOOL Create(LPCTSTR lpszClassName,
        LPCTSTR lpszWindowName, DWORD dwStyle,
        const RECT& rect,
        CWnd *pParentWnd, UINT nID,
        CCreateContext* pContext = NULL)
    { return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); }

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle,
        const RECT& rect, CWnd *pParentWnd, UINT nID,
        CFile* pPersist = NULL, BOOL bStorage = FALSE,
        BSTR bstrLicKey = NULL)
    { return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
        pPersist, bStorage, bstrLicKey); }

// Attributes
public:
    long GetColor();
    void SetColor(long);
    BOOL GetHasFrame();
    void SetHasFrame(BOOL);
    CString GetTitle();
    void SetTitle(LPCTSTR);

// Operations
public:
    void AboutBox();
};

// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORMAP_H__9C2D1B75_0AF1_40AF_97D6_FA8BB5F2D88D__INCLUDED_)