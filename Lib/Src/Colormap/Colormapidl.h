

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0613 */
/* at Tue Jan 19 04:14:07 2038
 */
/* Compiler settings for Colormap.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.00.0613 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */


#ifndef __Colormapidl_h__
#define __Colormapidl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef ___DColormap_FWD_DEFINED__
#define ___DColormap_FWD_DEFINED__
typedef interface _DColormap _DColormap;

#endif 	/* ___DColormap_FWD_DEFINED__ */


#ifndef ___DColormapEvents_FWD_DEFINED__
#define ___DColormapEvents_FWD_DEFINED__
typedef interface _DColormapEvents _DColormapEvents;

#endif 	/* ___DColormapEvents_FWD_DEFINED__ */


#ifndef __Colormap_FWD_DEFINED__
#define __Colormap_FWD_DEFINED__

#ifdef __cplusplus
typedef class Colormap Colormap;
#else
typedef struct Colormap Colormap;
#endif /* __cplusplus */

#endif 	/* __Colormap_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_Colormap_0000_0000 */
/* [local] */ 

#pragma warning(push)
#pragma warning(disable:4001) 
#pragma once
#pragma warning(push)
#pragma warning(disable:4001) 
#pragma once
#pragma warning(pop)
#pragma warning(pop)
#pragma region Desktop Family
#pragma endregion


extern RPC_IF_HANDLE __MIDL_itf_Colormap_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_Colormap_0000_0000_v0_0_s_ifspec;


#ifndef __ColormapLib_LIBRARY_DEFINED__
#define __ColormapLib_LIBRARY_DEFINED__

/* library ColormapLib */
/* [control][version][uuid] */ 


EXTERN_C const IID LIBID_ColormapLib;

#ifndef ___DColormap_DISPINTERFACE_DEFINED__
#define ___DColormap_DISPINTERFACE_DEFINED__

/* dispinterface _DColormap */
/* [uuid] */ 


EXTERN_C const IID DIID__DColormap;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("5C84D2A7-5B07-4B1E-934B-B1A3089A79F1")
    _DColormap : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DColormapVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DColormap * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DColormap * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DColormap * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DColormap * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DColormap * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DColormap * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DColormap * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } _DColormapVtbl;

    interface _DColormap
    {
        CONST_VTBL struct _DColormapVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DColormap_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DColormap_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DColormap_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DColormap_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DColormap_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DColormap_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DColormap_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DColormap_DISPINTERFACE_DEFINED__ */


#ifndef ___DColormapEvents_DISPINTERFACE_DEFINED__
#define ___DColormapEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DColormapEvents */
/* [uuid] */ 


EXTERN_C const IID DIID__DColormapEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("0CE00D37-27E5-4C2E-A641-A578BE2344BF")
    _DColormapEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DColormapEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DColormapEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DColormapEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DColormapEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DColormapEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DColormapEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DColormapEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DColormapEvents * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } _DColormapEventsVtbl;

    interface _DColormapEvents
    {
        CONST_VTBL struct _DColormapEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DColormapEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DColormapEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DColormapEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DColormapEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DColormapEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DColormapEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DColormapEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DColormapEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_Colormap;

#ifdef __cplusplus

class DECLSPEC_UUID("B4097DDA-446A-4AB9-B210-3AF19276DF6C")
Colormap;
#endif
#endif /* __ColormapLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


