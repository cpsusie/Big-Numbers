/* this ALWAYS GENERATED file contains the proxy stub code */


/* File created by MIDL compiler version 5.01.0164 */
/* at Thu May 20 01:18:17 2010
 */
/* Compiler settings for PaintX.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 440
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "PaintX.h"

#define TYPE_FORMAT_STRING_SIZE   989                               
#define PROC_FORMAT_STRING_SIZE   103                               

typedef struct _MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } MIDL_TYPE_FORMAT_STRING;

typedef struct _MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } MIDL_PROC_FORMAT_STRING;


extern const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString;
extern const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString;


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDispatch, ver. 0.0,
   GUID={0x00020400,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IPictureDecoder, ver. 0.0,
   GUID={0xB69FE5D2,0x9F53,0x11D2,{0xB1,0x3F,0x44,0x45,0x53,0x54,0x00,0x00}} */


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IPictureDecoder_ServerInfo;

#pragma code_seg(".orpc")
static const unsigned short IPictureDecoder_FormatStringOffsetTable[] = 
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    34
    };

static const MIDL_SERVER_INFO IPictureDecoder_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IPictureDecoder_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IPictureDecoder_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IPictureDecoder_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };

CINTERFACE_PROXY_VTABLE(9) _IPictureDecoderProxyVtbl = 
{
    &IPictureDecoder_ProxyInfo,
    &IID_IPictureDecoder,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *)-1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *)-1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *)-1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *)-1 /* IPictureDecoder::LoadPicture */ ,
    (void *)-1 /* IPictureDecoder::LoadResPicture */
};


static const PRPC_STUB_FUNCTION IPictureDecoder_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IPictureDecoderStubVtbl =
{
    &IID_IPictureDecoder,
    &IPictureDecoder_ServerInfo,
    9,
    &IPictureDecoder_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IPictureDecoder2, ver. 0.0,
   GUID={0x9D00A549,0xAB2D,0x4125,{0x96,0x54,0x51,0xC9,0x58,0xB5,0xD0,0x58}} */


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IPictureDecoder2_ServerInfo;

#pragma code_seg(".orpc")
extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[2];

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    __MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x20000, /* Ndr library version */
    0,
    0x50100a4, /* MIDL Version 5.1.164 */
    0,
    UserMarshalRoutines,
    0,  /* notify & notify_flag routine table */
    1,  /* Flags */
    0,  /* Reserved3 */
    0,  /* Reserved4 */
    0   /* Reserved5 */
    };

static const unsigned short IPictureDecoder2_FormatStringOffsetTable[] = 
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    34,
    68
    };

static const MIDL_SERVER_INFO IPictureDecoder2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IPictureDecoder2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IPictureDecoder2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IPictureDecoder2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };

CINTERFACE_PROXY_VTABLE(10) _IPictureDecoder2ProxyVtbl = 
{
    &IPictureDecoder2_ProxyInfo,
    &IID_IPictureDecoder2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *)-1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *)-1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *)-1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *)-1 /* IPictureDecoder::LoadPicture */ ,
    (void *)-1 /* IPictureDecoder::LoadResPicture */ ,
    (void *)-1 /* IPictureDecoder2::LoadMemPicture */
};


static const PRPC_STUB_FUNCTION IPictureDecoder2_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IPictureDecoder2StubVtbl =
{
    &IID_IPictureDecoder2,
    &IPictureDecoder2_ServerInfo,
    10,
    &IPictureDecoder2_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};

#pragma data_seg(".rdata")

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[2] = 
        {
            
            {
            BSTR_UserSize
            ,BSTR_UserMarshal
            ,BSTR_UserUnmarshal
            ,BSTR_UserFree
            },
            {
            VARIANT_UserSize
            ,VARIANT_UserMarshal
            ,VARIANT_UserUnmarshal
            ,VARIANT_UserFree
            }

        };


#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT40_OR_LATER)
#error You need a Windows NT 4.0 or later to run this stub because it uses these features:
#error   -Oif or -Oicf, [wire_marshal] or [user_marshal] attribute, more than 32 methods in the interface.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will die there with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure LoadPicture */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x7 ),	/* 7 */
#ifndef _ALPHA_
/*  8 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter Filename */

/* 16 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
#ifndef _ALPHA_
/* 18 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 20 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter ppUnk */

/* 22 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
#ifndef _ALPHA_
/* 24 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 26 */	NdrFcShort( 0x24 ),	/* Type Offset=36 */

	/* Return value */

/* 28 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 30 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 32 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LoadResPicture */

/* 34 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 36 */	NdrFcLong( 0x0 ),	/* 0 */
/* 40 */	NdrFcShort( 0x8 ),	/* 8 */
#ifndef _ALPHA_
/* 42 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 44 */	NdrFcShort( 0x8 ),	/* 8 */
/* 46 */	NdrFcShort( 0x8 ),	/* 8 */
/* 48 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter ResourceID */

/* 50 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
#ifndef _ALPHA_
/* 52 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 54 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppUnk */

/* 56 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
#ifndef _ALPHA_
/* 58 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 60 */	NdrFcShort( 0x24 ),	/* Type Offset=36 */

	/* Return value */

/* 62 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 64 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 66 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LoadMemPicture */

/* 68 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 70 */	NdrFcLong( 0x0 ),	/* 0 */
/* 74 */	NdrFcShort( 0x9 ),	/* 9 */
#ifndef _ALPHA_
/* 76 */	NdrFcShort( 0x10 ),	/* x86, MIPS, PPC Stack size/offset = 16 */
#else
			NdrFcShort( 0x20 ),	/* Alpha Stack size/offset = 32 */
#endif
/* 78 */	NdrFcShort( 0x0 ),	/* 0 */
/* 80 */	NdrFcShort( 0x8 ),	/* 8 */
/* 82 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter vByteArray */

/* 84 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
#ifndef _ALPHA_
/* 86 */	NdrFcShort( 0x4 ),	/* x86, MIPS, PPC Stack size/offset = 4 */
#else
			NdrFcShort( 0x8 ),	/* Alpha Stack size/offset = 8 */
#endif
/* 88 */	NdrFcShort( 0x3d2 ),	/* Type Offset=978 */

	/* Parameter ppUnk */

/* 90 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
#ifndef _ALPHA_
/* 92 */	NdrFcShort( 0x8 ),	/* x86, MIPS, PPC Stack size/offset = 8 */
#else
			NdrFcShort( 0x10 ),	/* Alpha Stack size/offset = 16 */
#endif
/* 94 */	NdrFcShort( 0x24 ),	/* Type Offset=36 */

	/* Return value */

/* 96 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
#ifndef _ALPHA_
/* 98 */	NdrFcShort( 0xc ),	/* x86, MIPS, PPC Stack size/offset = 12 */
#else
			NdrFcShort( 0x18 ),	/* Alpha Stack size/offset = 24 */
#endif
/* 100 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x12, 0x0,	/* FC_UP */
/*  4 */	NdrFcShort( 0xc ),	/* Offset= 12 (16) */
/*  6 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/*  8 */	NdrFcShort( 0x2 ),	/* 2 */
/* 10 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 12 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 14 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 16 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 18 */	NdrFcShort( 0x8 ),	/* 8 */
/* 20 */	NdrFcShort( 0xfffffff2 ),	/* Offset= -14 (6) */
/* 22 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 24 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 26 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 28 */	NdrFcShort( 0x0 ),	/* 0 */
/* 30 */	NdrFcShort( 0x4 ),	/* 4 */
/* 32 */	NdrFcShort( 0x0 ),	/* 0 */
/* 34 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (2) */
/* 36 */	
			0x11, 0x10,	/* FC_RP */
/* 38 */	NdrFcShort( 0x2 ),	/* Offset= 2 (40) */
/* 40 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 42 */	NdrFcLong( 0x0 ),	/* 0 */
/* 46 */	NdrFcShort( 0x0 ),	/* 0 */
/* 48 */	NdrFcShort( 0x0 ),	/* 0 */
/* 50 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 52 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 54 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 56 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 58 */	
			0x11, 0x0,	/* FC_RP */
/* 60 */	NdrFcShort( 0x396 ),	/* Offset= 918 (978) */
/* 62 */	
			0x12, 0x0,	/* FC_UP */
/* 64 */	NdrFcShort( 0x37e ),	/* Offset= 894 (958) */
/* 66 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 68 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 70 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 72 */	NdrFcShort( 0x2 ),	/* Offset= 2 (74) */
/* 74 */	NdrFcShort( 0x10 ),	/* 16 */
/* 76 */	NdrFcShort( 0x2b ),	/* 43 */
/* 78 */	NdrFcLong( 0x3 ),	/* 3 */
/* 82 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 84 */	NdrFcLong( 0x11 ),	/* 17 */
/* 88 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 90 */	NdrFcLong( 0x2 ),	/* 2 */
/* 94 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 96 */	NdrFcLong( 0x4 ),	/* 4 */
/* 100 */	NdrFcShort( 0x800a ),	/* Simple arm type: FC_FLOAT */
/* 102 */	NdrFcLong( 0x5 ),	/* 5 */
/* 106 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 108 */	NdrFcLong( 0xb ),	/* 11 */
/* 112 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 114 */	NdrFcLong( 0xa ),	/* 10 */
/* 118 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 120 */	NdrFcLong( 0x6 ),	/* 6 */
/* 124 */	NdrFcShort( 0xd6 ),	/* Offset= 214 (338) */
/* 126 */	NdrFcLong( 0x7 ),	/* 7 */
/* 130 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 132 */	NdrFcLong( 0x8 ),	/* 8 */
/* 136 */	NdrFcShort( 0xffffff7a ),	/* Offset= -134 (2) */
/* 138 */	NdrFcLong( 0xd ),	/* 13 */
/* 142 */	NdrFcShort( 0xffffff9a ),	/* Offset= -102 (40) */
/* 144 */	NdrFcLong( 0x9 ),	/* 9 */
/* 148 */	NdrFcShort( 0xc4 ),	/* Offset= 196 (344) */
/* 150 */	NdrFcLong( 0x2000 ),	/* 8192 */
/* 154 */	NdrFcShort( 0xd0 ),	/* Offset= 208 (362) */
/* 156 */	NdrFcLong( 0x24 ),	/* 36 */
/* 160 */	NdrFcShort( 0x2da ),	/* Offset= 730 (890) */
/* 162 */	NdrFcLong( 0x4024 ),	/* 16420 */
/* 166 */	NdrFcShort( 0x2d4 ),	/* Offset= 724 (890) */
/* 168 */	NdrFcLong( 0x4011 ),	/* 16401 */
/* 172 */	NdrFcShort( 0x2d2 ),	/* Offset= 722 (894) */
/* 174 */	NdrFcLong( 0x4002 ),	/* 16386 */
/* 178 */	NdrFcShort( 0x2d0 ),	/* Offset= 720 (898) */
/* 180 */	NdrFcLong( 0x4003 ),	/* 16387 */
/* 184 */	NdrFcShort( 0x2ce ),	/* Offset= 718 (902) */
/* 186 */	NdrFcLong( 0x4004 ),	/* 16388 */
/* 190 */	NdrFcShort( 0x2cc ),	/* Offset= 716 (906) */
/* 192 */	NdrFcLong( 0x4005 ),	/* 16389 */
/* 196 */	NdrFcShort( 0x2ca ),	/* Offset= 714 (910) */
/* 198 */	NdrFcLong( 0x400b ),	/* 16395 */
/* 202 */	NdrFcShort( 0x2b8 ),	/* Offset= 696 (898) */
/* 204 */	NdrFcLong( 0x400a ),	/* 16394 */
/* 208 */	NdrFcShort( 0x2b6 ),	/* Offset= 694 (902) */
/* 210 */	NdrFcLong( 0x4006 ),	/* 16390 */
/* 214 */	NdrFcShort( 0x2bc ),	/* Offset= 700 (914) */
/* 216 */	NdrFcLong( 0x4007 ),	/* 16391 */
/* 220 */	NdrFcShort( 0x2b2 ),	/* Offset= 690 (910) */
/* 222 */	NdrFcLong( 0x4008 ),	/* 16392 */
/* 226 */	NdrFcShort( 0x2b4 ),	/* Offset= 692 (918) */
/* 228 */	NdrFcLong( 0x400d ),	/* 16397 */
/* 232 */	NdrFcShort( 0x2b2 ),	/* Offset= 690 (922) */
/* 234 */	NdrFcLong( 0x4009 ),	/* 16393 */
/* 238 */	NdrFcShort( 0x2b0 ),	/* Offset= 688 (926) */
/* 240 */	NdrFcLong( 0x6000 ),	/* 24576 */
/* 244 */	NdrFcShort( 0x2ae ),	/* Offset= 686 (930) */
/* 246 */	NdrFcLong( 0x400c ),	/* 16396 */
/* 250 */	NdrFcShort( 0x2ac ),	/* Offset= 684 (934) */
/* 252 */	NdrFcLong( 0x10 ),	/* 16 */
/* 256 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 258 */	NdrFcLong( 0x12 ),	/* 18 */
/* 262 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 264 */	NdrFcLong( 0x13 ),	/* 19 */
/* 268 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 270 */	NdrFcLong( 0x16 ),	/* 22 */
/* 274 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 276 */	NdrFcLong( 0x17 ),	/* 23 */
/* 280 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 282 */	NdrFcLong( 0xe ),	/* 14 */
/* 286 */	NdrFcShort( 0x290 ),	/* Offset= 656 (942) */
/* 288 */	NdrFcLong( 0x400e ),	/* 16398 */
/* 292 */	NdrFcShort( 0x296 ),	/* Offset= 662 (954) */
/* 294 */	NdrFcLong( 0x4010 ),	/* 16400 */
/* 298 */	NdrFcShort( 0x254 ),	/* Offset= 596 (894) */
/* 300 */	NdrFcLong( 0x4012 ),	/* 16402 */
/* 304 */	NdrFcShort( 0x252 ),	/* Offset= 594 (898) */
/* 306 */	NdrFcLong( 0x4013 ),	/* 16403 */
/* 310 */	NdrFcShort( 0x250 ),	/* Offset= 592 (902) */
/* 312 */	NdrFcLong( 0x4016 ),	/* 16406 */
/* 316 */	NdrFcShort( 0x24a ),	/* Offset= 586 (902) */
/* 318 */	NdrFcLong( 0x4017 ),	/* 16407 */
/* 322 */	NdrFcShort( 0x244 ),	/* Offset= 580 (902) */
/* 324 */	NdrFcLong( 0x0 ),	/* 0 */
/* 328 */	NdrFcShort( 0x0 ),	/* Offset= 0 (328) */
/* 330 */	NdrFcLong( 0x1 ),	/* 1 */
/* 334 */	NdrFcShort( 0x0 ),	/* Offset= 0 (334) */
/* 336 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (335) */
/* 338 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 340 */	NdrFcShort( 0x8 ),	/* 8 */
/* 342 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 344 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 346 */	NdrFcLong( 0x20400 ),	/* 132096 */
/* 350 */	NdrFcShort( 0x0 ),	/* 0 */
/* 352 */	NdrFcShort( 0x0 ),	/* 0 */
/* 354 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 356 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 358 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 360 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 362 */	
			0x12, 0x0,	/* FC_UP */
/* 364 */	NdrFcShort( 0x1fc ),	/* Offset= 508 (872) */
/* 366 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x49,		/* 73 */
/* 368 */	NdrFcShort( 0x18 ),	/* 24 */
/* 370 */	NdrFcShort( 0xa ),	/* 10 */
/* 372 */	NdrFcLong( 0x8 ),	/* 8 */
/* 376 */	NdrFcShort( 0x58 ),	/* Offset= 88 (464) */
/* 378 */	NdrFcLong( 0xd ),	/* 13 */
/* 382 */	NdrFcShort( 0x78 ),	/* Offset= 120 (502) */
/* 384 */	NdrFcLong( 0x9 ),	/* 9 */
/* 388 */	NdrFcShort( 0x94 ),	/* Offset= 148 (536) */
/* 390 */	NdrFcLong( 0xc ),	/* 12 */
/* 394 */	NdrFcShort( 0xbc ),	/* Offset= 188 (582) */
/* 396 */	NdrFcLong( 0x24 ),	/* 36 */
/* 400 */	NdrFcShort( 0x114 ),	/* Offset= 276 (676) */
/* 402 */	NdrFcLong( 0x800d ),	/* 32781 */
/* 406 */	NdrFcShort( 0x130 ),	/* Offset= 304 (710) */
/* 408 */	NdrFcLong( 0x10 ),	/* 16 */
/* 412 */	NdrFcShort( 0x148 ),	/* Offset= 328 (740) */
/* 414 */	NdrFcLong( 0x2 ),	/* 2 */
/* 418 */	NdrFcShort( 0x160 ),	/* Offset= 352 (770) */
/* 420 */	NdrFcLong( 0x3 ),	/* 3 */
/* 424 */	NdrFcShort( 0x178 ),	/* Offset= 376 (800) */
/* 426 */	NdrFcLong( 0x14 ),	/* 20 */
/* 430 */	NdrFcShort( 0x190 ),	/* Offset= 400 (830) */
/* 432 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (431) */
/* 434 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 436 */	NdrFcShort( 0x4 ),	/* 4 */
/* 438 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 440 */	NdrFcShort( 0x0 ),	/* 0 */
/* 442 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 444 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 446 */	NdrFcShort( 0x4 ),	/* 4 */
/* 448 */	NdrFcShort( 0x0 ),	/* 0 */
/* 450 */	NdrFcShort( 0x1 ),	/* 1 */
/* 452 */	NdrFcShort( 0x0 ),	/* 0 */
/* 454 */	NdrFcShort( 0x0 ),	/* 0 */
/* 456 */	0x12, 0x0,	/* FC_UP */
/* 458 */	NdrFcShort( 0xfffffe46 ),	/* Offset= -442 (16) */
/* 460 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 462 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 464 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 466 */	NdrFcShort( 0x8 ),	/* 8 */
/* 468 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 470 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 472 */	NdrFcShort( 0x4 ),	/* 4 */
/* 474 */	NdrFcShort( 0x4 ),	/* 4 */
/* 476 */	0x11, 0x0,	/* FC_RP */
/* 478 */	NdrFcShort( 0xffffffd4 ),	/* Offset= -44 (434) */
/* 480 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 482 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 484 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 486 */	NdrFcShort( 0x0 ),	/* 0 */
/* 488 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 490 */	NdrFcShort( 0x0 ),	/* 0 */
/* 492 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 496 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 498 */	NdrFcShort( 0xfffffe36 ),	/* Offset= -458 (40) */
/* 500 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 502 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 504 */	NdrFcShort( 0x8 ),	/* 8 */
/* 506 */	NdrFcShort( 0x0 ),	/* 0 */
/* 508 */	NdrFcShort( 0x6 ),	/* Offset= 6 (514) */
/* 510 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 512 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 514 */	
			0x11, 0x0,	/* FC_RP */
/* 516 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (484) */
/* 518 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 520 */	NdrFcShort( 0x0 ),	/* 0 */
/* 522 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 524 */	NdrFcShort( 0x0 ),	/* 0 */
/* 526 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 530 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 532 */	NdrFcShort( 0xffffff44 ),	/* Offset= -188 (344) */
/* 534 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 536 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 538 */	NdrFcShort( 0x8 ),	/* 8 */
/* 540 */	NdrFcShort( 0x0 ),	/* 0 */
/* 542 */	NdrFcShort( 0x6 ),	/* Offset= 6 (548) */
/* 544 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 546 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 548 */	
			0x11, 0x0,	/* FC_RP */
/* 550 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (518) */
/* 552 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 554 */	NdrFcShort( 0x4 ),	/* 4 */
/* 556 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 558 */	NdrFcShort( 0x0 ),	/* 0 */
/* 560 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 562 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 564 */	NdrFcShort( 0x4 ),	/* 4 */
/* 566 */	NdrFcShort( 0x0 ),	/* 0 */
/* 568 */	NdrFcShort( 0x1 ),	/* 1 */
/* 570 */	NdrFcShort( 0x0 ),	/* 0 */
/* 572 */	NdrFcShort( 0x0 ),	/* 0 */
/* 574 */	0x12, 0x0,	/* FC_UP */
/* 576 */	NdrFcShort( 0x17e ),	/* Offset= 382 (958) */
/* 578 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 580 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 582 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 584 */	NdrFcShort( 0x8 ),	/* 8 */
/* 586 */	NdrFcShort( 0x0 ),	/* 0 */
/* 588 */	NdrFcShort( 0x6 ),	/* Offset= 6 (594) */
/* 590 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 592 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 594 */	
			0x11, 0x0,	/* FC_RP */
/* 596 */	NdrFcShort( 0xffffffd4 ),	/* Offset= -44 (552) */
/* 598 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 600 */	NdrFcLong( 0x2f ),	/* 47 */
/* 604 */	NdrFcShort( 0x0 ),	/* 0 */
/* 606 */	NdrFcShort( 0x0 ),	/* 0 */
/* 608 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 610 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 612 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 614 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 616 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 618 */	NdrFcShort( 0x1 ),	/* 1 */
/* 620 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 622 */	NdrFcShort( 0x4 ),	/* 4 */
/* 624 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 626 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 628 */	NdrFcShort( 0x10 ),	/* 16 */
/* 630 */	NdrFcShort( 0x0 ),	/* 0 */
/* 632 */	NdrFcShort( 0xa ),	/* Offset= 10 (642) */
/* 634 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 636 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 638 */	NdrFcShort( 0xffffffd8 ),	/* Offset= -40 (598) */
/* 640 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 642 */	
			0x12, 0x0,	/* FC_UP */
/* 644 */	NdrFcShort( 0xffffffe4 ),	/* Offset= -28 (616) */
/* 646 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 648 */	NdrFcShort( 0x4 ),	/* 4 */
/* 650 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 652 */	NdrFcShort( 0x0 ),	/* 0 */
/* 654 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 656 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 658 */	NdrFcShort( 0x4 ),	/* 4 */
/* 660 */	NdrFcShort( 0x0 ),	/* 0 */
/* 662 */	NdrFcShort( 0x1 ),	/* 1 */
/* 664 */	NdrFcShort( 0x0 ),	/* 0 */
/* 666 */	NdrFcShort( 0x0 ),	/* 0 */
/* 668 */	0x12, 0x0,	/* FC_UP */
/* 670 */	NdrFcShort( 0xffffffd4 ),	/* Offset= -44 (626) */
/* 672 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 674 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 676 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 678 */	NdrFcShort( 0x8 ),	/* 8 */
/* 680 */	NdrFcShort( 0x0 ),	/* 0 */
/* 682 */	NdrFcShort( 0x6 ),	/* Offset= 6 (688) */
/* 684 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 686 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 688 */	
			0x11, 0x0,	/* FC_RP */
/* 690 */	NdrFcShort( 0xffffffd4 ),	/* Offset= -44 (646) */
/* 692 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 694 */	NdrFcShort( 0x8 ),	/* 8 */
/* 696 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 698 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 700 */	NdrFcShort( 0x10 ),	/* 16 */
/* 702 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 704 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 706 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffff1 ),	/* Offset= -15 (692) */
			0x5b,		/* FC_END */
/* 710 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 712 */	NdrFcShort( 0x18 ),	/* 24 */
/* 714 */	NdrFcShort( 0x0 ),	/* 0 */
/* 716 */	NdrFcShort( 0xa ),	/* Offset= 10 (726) */
/* 718 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 720 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 722 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (698) */
/* 724 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 726 */	
			0x11, 0x0,	/* FC_RP */
/* 728 */	NdrFcShort( 0xffffff0c ),	/* Offset= -244 (484) */
/* 730 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 732 */	NdrFcShort( 0x1 ),	/* 1 */
/* 734 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 736 */	NdrFcShort( 0x0 ),	/* 0 */
/* 738 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 740 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 742 */	NdrFcShort( 0x8 ),	/* 8 */
/* 744 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 746 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 748 */	NdrFcShort( 0x4 ),	/* 4 */
/* 750 */	NdrFcShort( 0x4 ),	/* 4 */
/* 752 */	0x12, 0x0,	/* FC_UP */
/* 754 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (730) */
/* 756 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 758 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 760 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 762 */	NdrFcShort( 0x2 ),	/* 2 */
/* 764 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 766 */	NdrFcShort( 0x0 ),	/* 0 */
/* 768 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 770 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 772 */	NdrFcShort( 0x8 ),	/* 8 */
/* 774 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 776 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 778 */	NdrFcShort( 0x4 ),	/* 4 */
/* 780 */	NdrFcShort( 0x4 ),	/* 4 */
/* 782 */	0x12, 0x0,	/* FC_UP */
/* 784 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (760) */
/* 786 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 788 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 790 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 792 */	NdrFcShort( 0x4 ),	/* 4 */
/* 794 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 796 */	NdrFcShort( 0x0 ),	/* 0 */
/* 798 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 800 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 802 */	NdrFcShort( 0x8 ),	/* 8 */
/* 804 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 806 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 808 */	NdrFcShort( 0x4 ),	/* 4 */
/* 810 */	NdrFcShort( 0x4 ),	/* 4 */
/* 812 */	0x12, 0x0,	/* FC_UP */
/* 814 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (790) */
/* 816 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 818 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 820 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 822 */	NdrFcShort( 0x8 ),	/* 8 */
/* 824 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 826 */	NdrFcShort( 0x0 ),	/* 0 */
/* 828 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 830 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 832 */	NdrFcShort( 0x8 ),	/* 8 */
/* 834 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 836 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 838 */	NdrFcShort( 0x4 ),	/* 4 */
/* 840 */	NdrFcShort( 0x4 ),	/* 4 */
/* 842 */	0x12, 0x0,	/* FC_UP */
/* 844 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (820) */
/* 846 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 848 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 850 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 852 */	NdrFcShort( 0x8 ),	/* 8 */
/* 854 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 856 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 858 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 860 */	NdrFcShort( 0x8 ),	/* 8 */
/* 862 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 864 */	NdrFcShort( 0xffd8 ),	/* -40 */
/* 866 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 868 */	NdrFcShort( 0xffffffee ),	/* Offset= -18 (850) */
/* 870 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 872 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 874 */	NdrFcShort( 0x28 ),	/* 40 */
/* 876 */	NdrFcShort( 0xffffffee ),	/* Offset= -18 (858) */
/* 878 */	NdrFcShort( 0x0 ),	/* Offset= 0 (878) */
/* 880 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 882 */	0x38,		/* FC_ALIGNM4 */
			0x8,		/* FC_LONG */
/* 884 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 886 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffdf7 ),	/* Offset= -521 (366) */
			0x5b,		/* FC_END */
/* 890 */	
			0x12, 0x0,	/* FC_UP */
/* 892 */	NdrFcShort( 0xfffffef6 ),	/* Offset= -266 (626) */
/* 894 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 896 */	0x2,		/* FC_CHAR */
			0x5c,		/* FC_PAD */
/* 898 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 900 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 902 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 904 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 906 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 908 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 910 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 912 */	0xc,		/* FC_DOUBLE */
			0x5c,		/* FC_PAD */
/* 914 */	
			0x12, 0x0,	/* FC_UP */
/* 916 */	NdrFcShort( 0xfffffdbe ),	/* Offset= -578 (338) */
/* 918 */	
			0x12, 0x10,	/* FC_UP */
/* 920 */	NdrFcShort( 0xfffffc6a ),	/* Offset= -918 (2) */
/* 922 */	
			0x12, 0x10,	/* FC_UP */
/* 924 */	NdrFcShort( 0xfffffc8c ),	/* Offset= -884 (40) */
/* 926 */	
			0x12, 0x10,	/* FC_UP */
/* 928 */	NdrFcShort( 0xfffffdb8 ),	/* Offset= -584 (344) */
/* 930 */	
			0x12, 0x10,	/* FC_UP */
/* 932 */	NdrFcShort( 0xfffffdc6 ),	/* Offset= -570 (362) */
/* 934 */	
			0x12, 0x10,	/* FC_UP */
/* 936 */	NdrFcShort( 0x2 ),	/* Offset= 2 (938) */
/* 938 */	
			0x12, 0x0,	/* FC_UP */
/* 940 */	NdrFcShort( 0xfffffc54 ),	/* Offset= -940 (0) */
/* 942 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 944 */	NdrFcShort( 0x10 ),	/* 16 */
/* 946 */	0x6,		/* FC_SHORT */
			0x2,		/* FC_CHAR */
/* 948 */	0x2,		/* FC_CHAR */
			0x38,		/* FC_ALIGNM4 */
/* 950 */	0x8,		/* FC_LONG */
			0x39,		/* FC_ALIGNM8 */
/* 952 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 954 */	
			0x12, 0x0,	/* FC_UP */
/* 956 */	NdrFcShort( 0xfffffff2 ),	/* Offset= -14 (942) */
/* 958 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 960 */	NdrFcShort( 0x20 ),	/* 32 */
/* 962 */	NdrFcShort( 0x0 ),	/* 0 */
/* 964 */	NdrFcShort( 0x0 ),	/* Offset= 0 (964) */
/* 966 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 968 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 970 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 972 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 974 */	NdrFcShort( 0xfffffc74 ),	/* Offset= -908 (66) */
/* 976 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 978 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 980 */	NdrFcShort( 0x1 ),	/* 1 */
/* 982 */	NdrFcShort( 0x10 ),	/* 16 */
/* 984 */	NdrFcShort( 0x0 ),	/* 0 */
/* 986 */	NdrFcShort( 0xfffffc64 ),	/* Offset= -924 (62) */

			0x0
        }
    };

const CInterfaceProxyVtbl * _PaintX_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IPictureDecoder2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IPictureDecoderProxyVtbl,
    0
};

const CInterfaceStubVtbl * _PaintX_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IPictureDecoder2StubVtbl,
    ( CInterfaceStubVtbl *) &_IPictureDecoderStubVtbl,
    0
};

PCInterfaceName const _PaintX_InterfaceNamesList[] = 
{
    "IPictureDecoder2",
    "IPictureDecoder",
    0
};

const IID *  _PaintX_BaseIIDList[] = 
{
    &IID_IDispatch,
    &IID_IDispatch,
    0
};


#define _PaintX_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _PaintX, pIID, n)

int __stdcall _PaintX_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _PaintX, 2, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _PaintX, 2, *pIndex )
    
}

const ExtendedProxyFileInfo PaintX_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _PaintX_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _PaintX_StubVtblList,
    (const PCInterfaceName * ) & _PaintX_InterfaceNamesList,
    (const IID ** ) & _PaintX_BaseIIDList,
    & _PaintX_IID_Lookup, 
    2,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
