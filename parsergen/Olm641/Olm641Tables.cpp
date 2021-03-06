/* DO NOT EDIT THIS FILE - it is machine generated */
#include "stdafx.h"
#include "olm641parser.h"
#include <ParserTablesTemplate.h>

using namespace LRParsing;

#define _tc0000 0x00000000 /*                                                           */
#define _tc0001 0x00000000 /* a   shiftFromStateArray    0, shiftToStateArrayTable    0 */
#define _tc0002 0x00050001 /* b   Shift to    2 from    1                               */
#define _tc0003 0x00118000 /* d   Shift to    8 on states in shiftStateBitSet[0]        */
#define _tc0004 0x00078001 /* f   Shift to    3 on states in shiftStateBitSet[1]        */
#define _tc0005 0x00040003 /* z   shiftFromStateArray    1, shiftToStateArrayTable    1 */
#define _tc0006 0x00080006 /* x   shiftFromStateArray    2, shiftToStateArrayTable    2 */

static const unsigned int shiftCodeArray[7] = {
   _tc0000,_tc0001,_tc0002,_tc0003,_tc0004,_tc0005,_tc0006
}; // Size of table:28(x86)/32(x64) bytes.

static const unsigned char shiftFromStateArrayTable[10] = {
     2,   0,   3                                                                                             /*   0 Used by 1 terminal  [a]                           */
  ,  2,   6,  12                                                                                             /*   1 Used by 1 terminal  [z]                           */
  ,  3,   7,  13,  15                                                                                        /*   2 Used by 1 terminal  [x]                           */
}; // Size of table:12(x86)/16(x64) bytes.

static const unsigned char shiftToStateArrayTable[7] = {
      1,   4                                                                                                 /*   0 Used by 1 terminal  [a]                           */
  ,  10,  17                                                                                                 /*   1 Used by 1 terminal  [z]                           */
  ,  11,  18,  19                                                                                            /*   2 Used by 1 terminal  [x]                           */
}; // Size of table:8(x86)/8(x64) bytes.

static const unsigned char shiftStateBitSetTable[2] = { /* range:[0-4], bytes in bitset=1 */
   0x0d /*   0   3 states Used by 1 terminal  [d]                */
  ,0x12 /*   1   2 states Used by 1 terminal  [f]                */
}; // Size of table:4(x86)/8(x64) bytes.

#define _rc0000 0x00037fff /*                                                       */
#define _rc0001 0x00037fff /*                                                       */
#define _rc0002 0x00037fff /*                                                       */
#define _rc0003 0x00037fff /*                                                       */
#define _rc0004 0x00037fff /*                                                       */
#define _rc0005 0x00010000 /* Reduce by    0 (Accept) on EOI                        */
#define _rc0006 0x00037fff /*                                                       */
#define _rc0007 0x00037fff /*                                                       */
#define _rc0008 0x002a8016 /* Split(_rs0000,_rs0001)                                */
#define _rc0009 0x00070000 /* Reduce by    3 on EOI                                 */
#define _rc0010 0x00090000 /* Reduce by    4 on EOI                                 */
#define _rc0011 0x000b0000 /* Reduce by    5 on EOI                                 */
#define _rc0012 0x00170000 /* Reduce by   11 on EOI                                 */
#define _rc0013 0x00037fff /*                                                       */
#define _rc0014 0x00110000 /* Reduce by    8 on EOI                                 */
#define _rc0015 0x00037fff /*                                                       */
#define _rc0016 0x00030000 /* Reduce by    1 on EOI                                 */
#define _rc0017 0x00190000 /* Reduce by   12 on EOI                                 */
#define _rc0018 0x00050000 /* Reduce by    2 on EOI                                 */
#define _rc0019 0x00130000 /* Reduce by    9 on EOI                                 */
#define _rc0020 0x00150000 /* Reduce by   10 on EOI                                 */
#define _rs0000 0x000d8000 /* Reduce by    6 on tokens in termBitSet[0]  Used by 1 state  [8]*/
#define _rs0001 0x000f0006 /* Reduce by    7 on x                        Used by 1 state  [8]*/

static const unsigned int reduceCodeArray[23] = {
   _rc0000,_rc0001,_rc0002,_rc0003,_rc0004,_rc0005,_rc0006,_rc0007,_rc0008,_rc0009
  ,_rc0010,_rc0011,_rc0012,_rc0013,_rc0014,_rc0015,_rc0016,_rc0017,_rc0018,_rc0019
  ,_rc0020
  ,_rs0000,_rs0001
}; // Size of table:92(x86)/96(x64) bytes.

#define termArrayTable   nullptr
#define reduceArrayTable nullptr

static const unsigned char termBitSetTable[1] = { /* range:[0-6], bytes in bitset=1 */
   0x21 /*   0   2 tokens Used by 1 state  [8]                   */
}; // Size of table:4(x86)/8(x64) bytes.

#define _nc0000 0x00000000 /*                                                       */
#define _nc0001 0x000b7fff /* S     Shift to    5 No check (1 state )               */
#define _nc0002 0x00000000 /* A     stateArray    0, newStateArray    0             */
#define _nc0003 0x00217fff /* E     Shift to   16 No check (1 state )               */
#define _nc0004 0x00060000 /* B     stateArray    0, newStateArray    1             */
#define _nc0005 0x000c0004 /* D     stateArray    1, newStateArray    2             */

static const unsigned int succCodeArray[6] = {
   _nc0000,_nc0001,_nc0002,_nc0003,_nc0004,_nc0005
}; // Size of table:24(x86)/24(x64) bytes.

static const unsigned char succFromStateArrayTable[7] = {
     3,   0,   2,   3                                                                                        /*   0 Used by 2 ntIndices [A B]                         */
  ,  2,   1,   4                                                                                             /*   1 Used by 1 ntIndex   [D]                           */
}; // Size of table:8(x86)/8(x64) bytes.

static const unsigned char succToStateArrayTable[8] = {
      6,  12,  14                                                                                            /*   0 Used by 1 ntIndex   [A]                           */
  ,   7,  13,  15                                                                                            /*   1 Used by 1 ntIndex   [B]                           */
  ,   9,  20                                                                                                 /*   2 Used by 1 ntIndex   [D]                           */
}; // Size of table:8(x86)/8(x64) bytes.

#define succStateBitSetTable nullptr

/************************************************************************************\
* The prodLengthArray[] is indexed by production number and holds the number of      *
* symbols on the right side of each production.                                      *
\************************************************************************************/
static const unsigned char prodLengthArray[13] = {
  /*   0 */    1,  4,  4,  2,  2,  2,  1,  1,  2,  3
  /*  10 */ ,  3,  0,  1
}; // Size of table:16(x86)/16(x64) bytes.

/************************************************************************************\
* leftSideArray[] is indexed by production number.                                   *
* leftSideArray[p] = A', A' = (A - termCount)                                        *
*                        where A is the left side of production p.                   *
* A' = [0..ntermCount-1]                                                             *
* p  = [0..productionCount-1]                                                        *
\************************************************************************************/
static const unsigned char leftSideArray[13] = {
  /*   0 */    0,  1,  1,  1,  1,  1,  2,  4,  5,  5
  /*  10 */ ,  5,  3,  3
}; // Size of table:16(x86)/16(x64) bytes.

/************************************************************************************\
* rightSideTable[] holds a compressed form of the rightsides of all productions in   *
* the grammar. Only used for debugging.                                              *
\************************************************************************************/
static const unsigned char rightSideTable[26] = {
  /*   0 */    8
  /*   1 */ ,  1,  2,  9, 10
  /*   2 */ ,  1,  2, 11,  6
  /*   3 */ ,  1, 12
  /*   4 */ ,  9,  5
  /*   5 */ , 11,  6
  /*   6 */ ,  3
  /*   7 */ ,  3
  /*   8 */ ,  4,  9
  /*   9 */ ,  4, 11,  6
  /*  10 */ ,  4,  1, 12
  /*  12 */ ,  5
}; // Size of table:28(x86)/32(x64) bytes.

/************************************************************************************\
* symbolNames is a space separated string with the names of all symbols used in the  *
* grammar, terminals and nonTerminals. Only used for debugging.                      *
\************************************************************************************/
static const char *symbolNames = {
  "EOI"                                               /* T     0               */
  " a"                                                /* T     1               */
  " b"                                                /* T     2               */
  " d"                                                /* T     3               */
  " f"                                                /* T     4               */
  " z"                                                /* T     5               */
  " x"                                                /* T     6               */
  " start"                                            /* NT    7 ntIndex=0     */
  " S"                                                /* NT    8 ntIndex=1     */
  " A"                                                /* NT    9 ntIndex=2     */
  " E"                                                /* NT   10 ntIndex=3     */
  " B"                                                /* NT   11 ntIndex=4     */
  " D"                                                /* NT   12 ntIndex=5     */
}; // Size of string:32(x86)/32(x64) bytes

static const ParserTablesTemplateTransShift<13,7,13,21,352,440,0,7,0,5,0,0
                                          ,unsigned char
                                          ,unsigned char
                                          ,unsigned char
                                          ,unsigned char
                                          ,unsigned char> Olm641Tables_s(prodLengthArray   , leftSideArray
                                                                        ,rightSideTable    , symbolNames
                                                                        ,shiftCodeArray    , shiftFromStateArrayTable, shiftToStateArrayTable, shiftStateBitSetTable
                                                                        ,reduceCodeArray   , termArrayTable          , reduceArrayTable      , termBitSetTable
                                                                        ,succCodeArray     , succFromStateArrayTable , succToStateArrayTable , succStateBitSetTable
                                                                        );

const AbstractParserTables *Olm641Parser::Olm641Tables = &Olm641Tables_s;
// Size of Olm641Tables_s: 68(x86)/128(x64) bytes. Size of Olm641Tables:4(x86)/8(x64) bytes

// Total size of table data:352(x86)/440(x64) bytes.
