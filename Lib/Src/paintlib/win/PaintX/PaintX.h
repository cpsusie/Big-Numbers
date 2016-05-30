/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Thu May 20 01:18:17 2010
 */
/* Compiler settings for PaintX.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __PaintX_h__
#define __PaintX_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IPictureDecoder_FWD_DEFINED__
#define __IPictureDecoder_FWD_DEFINED__
typedef interface IPictureDecoder IPictureDecoder;
#endif 	/* __IPictureDecoder_FWD_DEFINED__ */


#ifndef __IPictureDecoder2_FWD_DEFINED__
#define __IPictureDecoder2_FWD_DEFINED__
typedef interface IPictureDecoder2 IPictureDecoder2;
#endif 	/* __IPictureDecoder2_FWD_DEFINED__ */


#ifndef __PictureDecoder_FWD_DEFINED__
#define __PictureDecoder_FWD_DEFINED__

#ifdef __cplusplus
typedef class PictureDecoder PictureDecoder;
#else
typedef struct PictureDecoder PictureDecoder;
#endif /* __cplusplus */

#endif 	/* __PictureDecoder_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IPictureDecoder_INTERFACE_DEFINED__
#define __IPictureDecoder_INTERFACE_DEFINED__

/* interface IPictureDecoder */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IPictureDecoder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B69FE5D2-9F53-11D2-B13F-444553540000")
    IPictureDecoder : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadPicture( 
            /* [in] */ BSTR Filename,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadResPicture( 
            /* [in] */ int ResourceID,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPictureDecoderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IPictureDecoder __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IPictureDecoder __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IPictureDecoder __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IPictureDecoder __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IPictureDecoder __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IPictureDecoder __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IPictureDecoder __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LoadPicture )( 
            IPictureDecoder __RPC_FAR * This,
            /* [in] */ BSTR Filename,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LoadResPicture )( 
            IPictureDecoder __RPC_FAR * This,
            /* [in] */ int ResourceID,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk);
        
        END_INTERFACE
    } IPictureDecoderVtbl;

    interface IPictureDecoder
    {
        CONST_VTBL struct IPictureDecoderVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPictureDecoder_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPictureDecoder_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPictureDecoder_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPictureDecoder_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IPictureDecoder_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IPictureDecoder_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IPictureDecoder_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IPictureDecoder_LoadPicture(This,Filename,ppUnk)	\
    (This)->lpVtbl -> LoadPicture(This,Filename,ppUnk)

#define IPictureDecoder_LoadResPicture(This,ResourceID,ppUnk)	\
    (This)->lpVtbl -> LoadResPicture(This,ResourceID,ppUnk)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IPictureDecoder_LoadPicture_Proxy( 
    IPictureDecoder __RPC_FAR * This,
    /* [in] */ BSTR Filename,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IPictureDecoder_LoadPicture_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IPictureDecoder_LoadResPicture_Proxy( 
    IPictureDecoder __RPC_FAR * This,
    /* [in] */ int ResourceID,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IPictureDecoder_LoadResPicture_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPictureDecoder_INTERFACE_DEFINED__ */


#ifndef __IPictureDecoder2_INTERFACE_DEFINED__
#define __IPictureDecoder2_INTERFACE_DEFINED__

/* interface IPictureDecoder2 */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IPictureDecoder2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9D00A549-AB2D-4125-9654-51C958B5D058")
    IPictureDecoder2 : public IPictureDecoder
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadMemPicture( 
            /* [in] */ VARIANT __RPC_FAR *vByteArray,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPictureDecoder2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IPictureDecoder2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IPictureDecoder2 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IPictureDecoder2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IPictureDecoder2 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IPictureDecoder2 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IPictureDecoder2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IPictureDecoder2 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LoadPicture )( 
            IPictureDecoder2 __RPC_FAR * This,
            /* [in] */ BSTR Filename,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LoadResPicture )( 
            IPictureDecoder2 __RPC_FAR * This,
            /* [in] */ int ResourceID,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LoadMemPicture )( 
            IPictureDecoder2 __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *vByteArray,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk);
        
        END_INTERFACE
    } IPictureDecoder2Vtbl;

    interface IPictureDecoder2
    {
        CONST_VTBL struct IPictureDecoder2Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPictureDecoder2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPictureDecoder2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPictureDecoder2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPictureDecoder2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IPictureDecoder2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IPictureDecoder2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IPictureDecoder2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IPictureDecoder2_LoadPicture(This,Filename,ppUnk)	\
    (This)->lpVtbl -> LoadPicture(This,Filename,ppUnk)

#define IPictureDecoder2_LoadResPicture(This,ResourceID,ppUnk)	\
    (This)->lpVtbl -> LoadResPicture(This,ResourceID,ppUnk)


#define IPictureDecoder2_LoadMemPicture(This,vByteArray,ppUnk)	\
    (This)->lpVtbl -> LoadMemPicture(This,vByteArray,ppUnk)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IPictureDecoder2_LoadMemPicture_Proxy( 
    IPictureDecoder2 __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *vByteArray,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnk);


void __RPC_STUB IPictureDecoder2_LoadMemPicture_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPictureDecoder2_INTERFACE_DEFINED__ */



#ifndef __PaintX_LIBRARY_DEFINED__
#define __PaintX_LIBRARY_DEFINED__

/* library PaintX */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_PaintX;

EXTERN_C const CLSID CLSID_PictureDecoder;

#ifdef __cplusplus

class DECLSPEC_UUID("B69FE5D3-9F53-11D2-B13F-444553540000")
PictureDecoder;
#endif
#endif /* __PaintX_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long __RPC_FAR *, unsigned long            , VARIANT __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  VARIANT_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, VARIANT __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  VARIANT_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, VARIANT __RPC_FAR * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long __RPC_FAR *, VARIANT __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
