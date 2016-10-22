//  GREKO Chess Engine
//  (c) 2002-2011 Vladimir Medvedev <vrm@bk.ru>
//  http://greko.110mb.com

//  types.h: chess types and enums, standard headers
//  modified: 1-Aug-2011

#ifndef TYPES_H
#define TYPES_H

#include <algorithm>
#include <list>
#include <string>
#include <vector>
using namespace std;

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef _MSC_VER
  typedef unsigned char     U8;
  typedef signed short      I8;
  typedef unsigned short   U16;
  typedef unsigned int     U32;
  typedef int              I32;
  typedef unsigned __int64 U64;
  #define LL(x) x##L
#else
  #include <stdint.h>
  typedef uint8_t   U8;
  typedef int8_t    I8;
  typedef uint16_t U16;
  typedef uint32_t U32;
  typedef int32_t  I32;
  typedef uint64_t U64;
  #define LL(x) x##LL
#endif

typedef U8  PIECE;
typedef U8  COLOR;
typedef U8  FLD;
typedef I32 EVAL;
typedef U64 NODES;

enum PIECE_T
{
  NOPIECE =  0,
  PW      =  2,
  PB      =  3,
  NW      =  4,
  NB      =  5,
  BW      =  6,
  BB      =  7,
  RW      =  8,
  RB      =  9,
  QW      = 10,
  QB      = 11,
  KW      = 12,
  KB      = 13
};

enum COLOR_T 
{
  WHITE = 0,
  BLACK = 1
};

enum FLD_T
{
  A8 =  0, B8 =  1, C8 =  2, D8 =  3, E8 =  4, F8 =  5, G8 =  6, H8 =  7,
  A7 =  8, B7 =  9, C7 = 10, D7 = 11, E7 = 12, F7 = 13, G7 = 14, H7 = 15,
  A6 = 16, B6 = 17, C6 = 18, D6 = 19, E6 = 20, F6 = 21, G6 = 22, H6 = 23,
  A5 = 24, B5 = 25, C5 = 26, D5 = 27, E5 = 28, F5 = 29, G5 = 30, H5 = 31,
  A4 = 32, B4 = 33, C4 = 34, D4 = 35, E4 = 36, F4 = 37, G4 = 38, H4 = 39,
  A3 = 40, B3 = 41, C3 = 42, D3 = 43, E3 = 44, F3 = 45, G3 = 46, H3 = 47,
  A2 = 48, B2 = 49, C2 = 50, D2 = 51, E2 = 52, F2 = 53, G2 = 54, H2 = 55,
  A1 = 56, B1 = 57, C1 = 58, D1 = 59, E1 = 60, F1 = 61, G1 = 62, H1 = 63,
  NF = 64
};

enum DIR_T
{
  DIR_R  = 0,
  DIR_UR = 1,
  DIR_U  = 2,
  DIR_UL = 3,
  DIR_L  = 4,
  DIR_DL = 5,
  DIR_D  = 6,
  DIR_DR = 7,
  DIR_NO = 8
};

enum CASTLE_T
{
  WHITE_CAN_O_O   = 0x01,
  WHITE_CAN_O_O_O = 0x02,
  BLACK_CAN_O_O   = 0x04,
  BLACK_CAN_O_O_O = 0x08,
  WHITE_DID_O_O   = 0x10,
  WHITE_DID_O_O_O = 0x20,
  BLACK_DID_O_O   = 0x40,
  BLACK_DID_O_O_O = 0x80
};

enum PROTOCOL_T
{
  CONSOLE = 0,
  WINBOARD = 1,
  UCI = 2
};

#endif

