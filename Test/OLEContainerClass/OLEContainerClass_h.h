

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0613 */
/* at Tue Jan 19 04:14:07 2038
 */
/* Compiler settings for OLEContainerClass.odl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.00.0613 
    protocol : all , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */


#ifndef __OLEContainerClass_h_h__
#define __OLEContainerClass_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IOLEContainerClass_FWD_DEFINED__
#define __IOLEContainerClass_FWD_DEFINED__
typedef interface IOLEContainerClass IOLEContainerClass;

#endif 	/* __IOLEContainerClass_FWD_DEFINED__ */


#ifndef __Document_FWD_DEFINED__
#define __Document_FWD_DEFINED__

#ifdef __cplusplus
typedef class Document Document;
#else
typedef struct Document Document;
#endif /* __cplusplus */

#endif 	/* __Document_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __OLEContainerClass_LIBRARY_DEFINED__
#define __OLEContainerClass_LIBRARY_DEFINED__

/* library OLEContainerClass */
/* [version][uuid] */ 


DEFINE_GUID(LIBID_OLEContainerClass,0xEF3288D0,0xB9C6,0x4212,0x82,0xE8,0x75,0xD5,0x4A,0x4A,0x61,0x5A);

#ifndef __IOLEContainerClass_DISPINTERFACE_DEFINED__
#define __IOLEContainerClass_DISPINTERFACE_DEFINED__

/* dispinterface IOLEContainerClass */
/* [uuid] */ 


DEFINE_GUID(DIID_IOLEContainerClass,0x2A8FCBEA,0x37C2,0x4561,0xBD,0x3F,0xE3,0x11,0x7D,0xEA,0xF4,0x84);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("2A8FCBEA-37C2-4561-BD3F-E3117DEAF484")
    IOLEContainerClass : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IOLEContainerClassVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOLEContainerClass * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOLEContainerClass * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOLEContainerClass * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IOLEContainerClass * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IOLEContainerClass * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IOLEContainerClass * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IOLEContainerClass * This,
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
    } IOLEContainerClassVtbl;

    interface IOLEContainerClass
    {
        CONST_VTBL struct IOLEContainerClassVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOLEContainerClass_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IOLEContainerClass_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IOLEContainerClass_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IOLEContainerClass_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IOLEContainerClass_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IOLEContainerClass_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IOLEContainerClass_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IOLEContainerClass_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_Document,0xA03FEFA9,0x8FC7,0x4FD3,0x97,0x29,0x18,0x07,0xC5,0x44,0x13,0x1B);

#ifdef __cplusplus

class DECLSPEC_UUID("A03FEFA9-8FC7-4FD3-9729-1807C544131B")
Document;
#endif
#endif /* __OLEContainerClass_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


