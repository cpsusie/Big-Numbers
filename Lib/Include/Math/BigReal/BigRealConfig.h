#pragma once

#include <Math/Int128.h>

// Define this to have 2 different version of getDecimalDigitCount64(UINT64 n).
// Measures of time show that getDecimalDigitCount64 is 5 times faster than getDecimalDigitCount64Loop
//#define HAS_LOOP_DIGITCOUNT

#define USE_FETCHDIGITLIST

// Define this to keep track of invariant for DigitPools, and fetched lists of Digits
//#define CHECK_DIGITPOOLINVARIANT

// Define this to count the number of fetchDigit on DigitPools.
// if COUNT_DIGITPOOLFETCHDIGIT == 1, the number of allocated pages and digits will be logged (debugLog), when digitpool is deleted
// if COUNT_DIGITPOOLFETCHDIGIT == 2, the total number of calls to fetchDigit + fetchDigitList to each digitpool will also be logged
//#define COUNT_DIGITPOOLFETCHDIGIT 2

// If this is defined, product wil trace recursive calls when multiplying long BigReals
// which will slow down the program (testBigReal) by a factor 20!!
//#define TRACEPRODUCTRECURSION

// If this is defined, load/save/the number of requests and cache-hits to pow2Cache will be logged
// and written to debugLog at program exit
//#define TRACEPOW2CACHE

// If this is defined, the number of requests and cache-hits to pow2Cache will be logged
// and written to debugLog when program exit
//#define TRACEPOW2CACHEHIT

#define           BIGREAL_LOG10BASEx86                          8
constexpr int     BIGREAL_POW10TABLESIZEx86 =                  10;
constexpr int     BIGREAL_NONNORMALx86      =          -900000000;
constexpr int     BIGREAL_MAXEXPOx86        =            99999999;
constexpr int     BIGREAL_MINEXPOx86        =           -99999999;

typedef ULONG     BRDigitTypex86;
typedef UINT64    BR2DigitTypex86;
typedef long      BRExpoTypex86;
typedef long      BRDigitDiffTypex86;


#define           BIGREAL_LOG10BASEx64                         18
constexpr int     BIGREAL_POW10TABLESIZEx64 =                  20;
constexpr __int64 BIGREAL_NONNORMALx64      = -900000000000000000;
constexpr __int64 BIGREAL_MAXEXPOx64        =   99999999999999999;
constexpr __int64 BIGREAL_MINEXPOx64        =  -99999999999999999;

typedef UINT64    BRDigitTypex64;
typedef _uint128  BR2DigitTypex64;
typedef INT64     BRExpoTypex64;
typedef INT64     BRDigitDiffTypex64;

// Basic definitions depends on registersize. 32/64-bit
#if defined(IS32BIT)

#define BIGREAL_LOG10BASE  BIGREAL_LOG10BASEx86

#define BIGREAL_NONNORMAL  BIGREAL_NONNORMALx86
#define BIGREAL_MAXEXPO    BIGREAL_MAXEXPOx86
#define BIGREAL_MINEXPO    BIGREAL_MINEXPOx86

#define BRDigitType        BRDigitTypex86
#define BR2DigitType       BR2DigitTypex86
#define BRExpoType         BRExpoTypex86
#define BRDigitDiffType    BRDigitDiffTypex86

#else // IS64BIT

#define BIGREAL_LOG10BASE  BIGREAL_LOG10BASEx64

#define BIGREAL_NONNORMAL  BIGREAL_NONNORMALx64
#define BIGREAL_MAXEXPO    BIGREAL_MAXEXPOx64
#define BIGREAL_MINEXPO    BIGREAL_MINEXPOx64

#define BRDigitType        BRDigitTypex64
#define BR2DigitType       BR2DigitTypex64
#define BRExpoType         BRExpoTypex64
#define BRDigitDiffType    BRDigitDiffTypex64

#endif // IS32BIT

// Values for BigReal::m_low, if m_expo == BIGREAL_NONNORMAL (_isnormal() is false)
constexpr int BIGREAL_ZEROLOW  = 0; // isZero() is true
constexpr int BIGREAL_INFLOW   = 1; // _isinf() is true, +/- infinite depending on BR_NEG bit in m_flags
constexpr int BIGREAL_QNANLOW  = 2; // _isnan() is true

#define SP_OPT_NONE      0
#define SP_OPT_BY_FPU    1
#define SP_OPT_BY_FPU2   2
#define SP_OPT_BY_REG32  3
#define SP_OPT_BY_REG64  4

// Define SP_OPT_METHOD to one of these, to select the desired optimisation of shortProduct.
// Best performance in x86 is SP_OPT_BY_REG32. Best (and only) in x64-mode is SP_OPT_BY_REG64

#if defined(IS32BIT)
//#define SP_OPT_METHOD  SP_OPT_NONE
//#define SP_OPT_METHOD  SP_OPT_BY_FPU
//#define SP_OPT_METHOD  SP_OPT_BY_FPU2
#define SP_OPT_METHOD SP_OPT_BY_REG32
#else
#define SP_OPT_METHOD SP_OPT_BY_REG64
#endif

#if !defined(SP_OPT_METHOD)

#error Must define SP_OPT_METHOD

#elif((SP_OPT_METHOD != SP_OPT_NONE) && (SP_OPT_METHOD != SP_OPT_BY_FPU) && (SP_OPT_METHOD != SP_OPT_BY_FPU2) && (SP_OPT_METHOD != SP_OPT_BY_REG32) && (SP_OPT_METHOD != SP_OPT_BY_REG64))

#error Must define SP_OPT_METHOD

#endif

#if(SP_OPT_METHOD == SP_OPT_NONE)

#if   BIGREAL_LOG10BASE == 2
#define SQRT_BIGREALBASE 10
#elif BIGREAL_LOG10BASE == 4
#define SQRT_BIGREALBASE 100
#elif BIGREAL_LOG10BASE == 6
#define SQRT_BIGREALBASE 1000
#elif BIGREAL_LOG10BASE == 8
#define SQRT_BIGREALBASE 10000
#else // BIGREAL_LOG10BASE
#error Illegal BIGREAL_LOG10BASE. Must be in the set {2,4,6 or 8}
#endif // BIGREAL_LOG10BASE
#define BIGREALBASE (SQRT_BIGREALBASE*SQRT_BIGREALBASE)

#elif(SP_OPT_METHOD == SP_OPT_BY_FPU)

#if   BIGREAL_LOG10BASE == 1
#define BIGREALBASE 10
#elif BIGREAL_LOG10BASE == 2
#define BIGREALBASE 100
#elif BIGREAL_LOG10BASE == 3
#define BIGREALBASE 1000
#elif BIGREAL_LOG10BASE == 4
#define BIGREALBASE 10000
#elif BIGREAL_LOG10BASE == 5
#define BIGREALBASE 100000
#elif BIGREAL_LOG10BASE == 6
#define BIGREALBASE 1000000
#elif BIGREAL_LOG10BASE == 7
#define BIGREALBASE 10000000
#elif BIGREAL_LOG10BASE == 8
#define BIGREALBASE 100000000
#else
#error Illegal BIGREAL_LOG10BASE. Must be in the interval [1-8]
#endif // BIGREAL_LOG10BASE
// Cannot use 1e9 because of overflow in function shortProductNoZeroCheck, when accumulating more than 18 products, which will
// occur when splitLength > 18. And a splitlength of 18 is far to small to give product a boost

#elif(SP_OPT_METHOD == SP_OPT_BY_REG32 || SP_OPT_METHOD == SP_OPT_BY_FPU2)

#if BIGREAL_LOG10BASE == 8
// this is the only valid BIGREALBASE for these optimizations
#define BIGREALBASE 100000000
#else
#error Illegal BIGREAL_LOG10BASE. Must be 8.
#endif // BIGREAL_LOG10BASE == 8

#elif (SP_OPT_METHOD == SP_OPT_BY_REG64)

#if BIGREAL_LOG10BASE == 18
// this is the only valid BIGREALBASE for SP_OPT_BY_REG64
#define BIGREALBASE 1000000000000000000
#else
#error Illegal BIGREAL_LOG10BASE. Must be 18.
#endif // BIGREAL_LOG10BASE == 18

#endif // if(SP_OPT_METHOD == ...)

constexpr int    APC_DIGITS                =    6;
constexpr size_t CONVERSION_POW2DIGITCOUNT =   24;

constexpr BYTE   BR_NEG                    = 0x01;
constexpr BYTE   BR_INITDONE               = 0x02;
constexpr BYTE   BR_MUTABLE                = 0x04;
