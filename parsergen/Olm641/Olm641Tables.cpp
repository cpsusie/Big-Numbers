/* DO NOT EDIT THIS FILE - it is machine generated */
#include "stdafx.h"
#include "olm641parser.h"
#include <ParserTablesTemplate.h>
/************************************************************************************\
* The 4 arrays actionCode, termListTable, actionListTable and termSetTable           *
* holds a compressed action-matrix, used by LRParser to find                         *
* action = getAction(S,T), where S is current state, T is next terminal on input     *
*                                                                                    *
* The interpretation of action is:                                                   *
*   action <  0 - Reduce by production p, p == -action.                              *
*   action == 0 - Accept. Reduce by production 0.                                    *
*   action >  0 - Shift to newstate (=action),                                       *
*                 ie. push(newstate), set current state=newstate                     *
*                 and advance input 1 symbol.                                        *
*   action == _ParserError - Unexpected input. Do some recovery, to try to           *
*                 synchronize input and stack, in order to continue parse.           *
*                 (See LRParser::recover() in LRParser.cpp)                          *
*                                                                                    *
* For each state S, a #define is generated and used as element S in array            *
* actionCode. Each define looks as:                                                  *
*                                                                                    *
* #define _acDDDD Code                                                               *
*                                                                                    *
* where DDDD is the statenumber S and Code is an unsigned int with the following     *
* format:                                                                            *
*            0         1         2         3                                         *
* Bit index: 01234567890123456789012345678901                                        *
* Code     : tttttttttttttttCCaaaaaaaaaaaaaaa                                        *
*                                                                                    *
* t        : Bit[ 0-14] : unsigned short                                             *
* a        : Bit[17-31] : signed short                                               *
* CC       : Bit[15-16] : Indicates how to interpret t and a:                        *
*                                                                                    *
* CC == 0: CompCodeTermList (uncompressed)                                           *
*       t: Index into array termListTable, pointing at the first element of          *
*          termList                                                                  *
*       a: Index into array actionListTable, pointing at the first element of        *
*          actionList                                                                *
*                                                                                    *
*       n                  : termListTable[t] = number of elements in termList.      *
*       termList[0..n-1]   : termListTable[t+1..t+n]                                 *
*                            Ordered list of legal terminals                         *
*       actionList[0..n-1] : actionListTable[a..a+n-1] (same length as termList).    *
*                                                                                    *
*       To get action, find index k in termList, so termList[k] == T,k=[0..n-1]      *
*       and set action = actionList[k].                                              *
*       If T is not found, set action = _ParseError.                                 *
*       Note that both termList and actionList may be shared by several states.      *
*                                                                                    *
* CC == 1: CompCodeSplitNode                                                         *
*       t and a are both indices to 2 child entries in actionCode, which can be      *
*       another _acNNNN or an extra node, _snNNNN, whichever is needed (values are   *
*       reused as much as possible, ie. if _snNNNN equals some _acNNNN then no       *
*       _snNNNN is added, but parent entry will point to _acNNNN instead.            *
*       Recursive tree search, with max-recursion level specified in                 *
*       parsegen +c options                                                          *
*                                                                                    *
* CC == 2: CompCodeOneItem (Only 1 legal terminal in the state)                      *
*       t: Legal terminal.                                                           *
*       a: Action.                                                                   *
*                                                                                    *
* CC == 3: CompCodeTermSet (always reduce by same reduce production P = -a)          *
*       t: Index into termSetTable, pointing at the first element of termSet         *
*       a: Action.                                                                   *
*                                                                                    *
*       termSetTable is a list of termSet, bitsets, each with terminalCount bits     *
*       1-bits for legal terminals, 0-bits for illegal terminals.                    *
*                                                                                    *
*       b                  : Number of bytes in each termSet=(terminalCount-1)/8+1   *
*       termSet[0..b-1]    : termSetTable[t..t+b-1]                                  *
*                                                                                    *
*       As for uncompressed states, the same check for existence is done.            *
*       If terminal T is not present in termSet, set action = _ParseError.           *
*       Note that each termSet may be shared by several states.                      *
\************************************************************************************/

#define _ac0000 0x00000000 /* termArray    0, actionArray    0                      */
#define _ac0001 0x00010000 /* Reduce by    0 on EOI                                 */
#define _ac0002 0x00040003 /* termArray    1, actionArray    1                      */
#define _ac0003 0x00130005 /* Shift  to    9 on z                                   */
#define _ac0004 0x00150006 /* Shift  to   10 on x                                   */
#define _ac0005 0x002a8016 /* Split(_as0000,_as0001)                                */
#define _ac0006 0x000b0003 /* Shift  to    5 on d                                   */
#define _ac0007 0xfffb0000 /* Reduce by    3 on EOI                                 */
#define _ac0008 0x00080000 /* termArray    0, actionArray    2                      */
#define _ac0009 0xfff90000 /* Reduce by    4 on EOI                                 */
#define _ac0010 0xfff70000 /* Reduce by    5 on EOI                                 */
#define _ac0011 0x000c0006 /* termArray    2, actionArray    3                      */
#define _ac0012 0x00250006 /* Shift  to   18 on x                                   */
#define _ac0013 0xfff10000 /* Reduce by    8 on EOI                                 */
#define _ac0014 0x00270006 /* Shift  to   19 on x                                   */
#define _ac0015 0x00110004 /* Shift  to    8 on f                                   */
#define _ac0016 0xffff0000 /* Reduce by    1 on EOI                                 */
#define _ac0017 0xffe90000 /* Reduce by   12 on EOI                                 */
#define _ac0018 0xfffd0000 /* Reduce by    2 on EOI                                 */
#define _ac0019 0xffef0000 /* Reduce by    9 on EOI                                 */
#define _ac0020 0xffed0000 /* Reduce by   10 on EOI                                 */
#define _as0000 0xfff58000 /* Reduce by    6 on tokens in termBitSet[0]  Used by state  [5]*/
#define _as0001 0xfff30006 /* Reduce by    7 on x                        Used by state  [5]*/

static const unsigned int actionCodeArray[23] = {
   _ac0000,_ac0001,_ac0002,_ac0003,_ac0004,_ac0005,_ac0006,_ac0007,_ac0008,_ac0009
  ,_ac0010,_ac0011,_ac0012,_ac0013,_ac0014,_ac0015,_ac0016,_ac0017,_ac0018,_ac0019
  ,_ac0020,_as0000,_as0001
}; // Size of table:92(x86)/96(x64) bytes.

static const unsigned char termArrayTable[9] = {
     2,   1,   3                                                                                             /*   0 Used by states [0,8]                              */
  ,  2,   2,   4                                                                                             /*   1 Used by state  [2]                                */
  ,  2,   0,   5                                                                                             /*   2 Used by state  [11]                               */
}; // Size of table:12(x86)/16(x64) bytes.

static const char actionArrayTable[8] = {
      2,   5                                                                                                 /*   0 Used by state  [0]                                */
  ,   6,   8                                                                                                 /*   1 Used by state  [2]                                */
  ,  15,   5                                                                                                 /*   2 Used by state  [8]                                */
  , -11,  17                                                                                                 /*   3 Used by state  [11]                               */
}; // Size of table:8(x86)/8(x64) bytes.

static const unsigned char termBitSetTable[1] = {
   0x21 /*   0   2 tokens Used by state  [5]                     */
}; // Size of table:4(x86)/8(x64) bytes.

/************************************************************************************\
* The 3 arrays successorCode, NTindexListTable and stateListTable holds a            *
* compressed succesor-matrix, used by LRParser to find newstate = successor(S,A)     *
* as last part of a reduction with production P, A -> alfa.                          *
* A reduction by production P goes as follows:                                       *
*   Pop L elements from stack, where L = length of alfa;                             *
*   S = state on stacktop;                                                           *
*   A = leftside of the reduce production P;                                         *
*   newstate = successor(S,A);                                                       *
*   push(newstate), and set current state = newstate.                                *
*                                                                                    *
* For each relevant state S, a #define is generated and used as element S in array   *
* successorCode. Each define has the format:                                         *
*                                                                                    *
* #define _suDDDD Code                                                               *
*                                                                                    *
* where DDDD is the statenumber S and Code is an unsigned int with the following     *
* format:                                                                            *
*            0         1         2         3                                         *
* Bit index: 01234567890123456789012345678901                                        *
* Code       iiiiiiiiiiiiiiiCCsssssssssssssss                                        *
*                                                                                    *
* i          : Bit[ 0-14]: unsigned short                                            *
* s          : Bit[17-31]: unsigned short                                            *
* CC         : Bit[15-16]: Indicates how to interpret i and s.                       *
*                                                                                    *
* CC has the same meaning as for actionCode, but only CC={0,2} are used.             *
* CC == 0: Uncompressed Format.                                                      *
*       i: Index into array NTindexListTable, pointing at the first element of       *
*          NTIndexList                                                               *
*       s: Index into array stateListTable, pointing at the first element of         *
*          stateList                                                                 *
*                                                                                    *
*       n                  : NTIndexListTable[i] = number of elements in NTIndexList *
*       NTIndexList[0..n-1]: NTIndexListTable[i+1..i+n]                              *
*                            Ordered list of possible nonterminal-indices.           *
*       stateList[0..n-1]  : stateListTable[s..s+n-1], same length as NTIndexList    *
*                                                                                    *
*       To get newstate, find index k in NTIndexList, so NTIndexList[k] == A',       *
*       and set newstate = stateList[k].                                             *
*       A' = (A - terminalCount) will always exist.                                  *
*       Note that both NTIndexList and stateList may be shared by several states.    *
* CC == 2: Compressed Format, used if there is only 1 possible newstate.             *
*       i: Index A' of nonterminal A, A' = (A - terminalCount).                      *
*       s: New state.                                                                *
*                                                                                    *
\************************************************************************************/

#define _su0000 0x00000000 /* NTindexArray   0, newStateArray   0                   */
#define _su0002 0x000f0005 /* Goto 7 on D                                           */
#define _su0006 0x00060004 /* NTindexArray   1, newStateArray   1                   */
#define _su0008 0x000a0004 /* NTindexArray   1, newStateArray   2                   */
#define _su0011 0x00210003 /* Goto 16 on E                                          */
#define _su0015 0x00290005 /* Goto 20 on D                                          */

#define nil (unsigned int)-1
static const unsigned int successorCodeArray[21] = {
   _su0000,nil    ,_su0002,nil    ,nil    ,nil    ,_su0006,nil    ,_su0008,nil
  ,nil    ,_su0011,nil    ,nil    ,nil    ,_su0015,nil    ,nil    ,nil    ,nil
  ,nil
}; // Size of table:84(x86)/88(x64) bytes.

static const unsigned char NTindexArrayTable[7] = {
     3,   1,   2,   4                                                                                        /*   0 Used by state  [0]                                */
  ,  2,   2,   4                                                                                             /*   1 Used by states [6,8]                              */
}; // Size of table:8(x86)/8(x64) bytes.

static const unsigned char newStateArrayTable[7] = {
      1,   3,   4                                                                                            /*   0 Used by state  [0]                                */
  ,  11,  12                                                                                                 /*   1 Used by state  [6]                                */
  ,  13,  14                                                                                                 /*   2 Used by state  [8]                                */
}; // Size of table:8(x86)/8(x64) bytes.

/************************************************************************************\
* The productionLength[] is indexed by production number and holds the number of     *
* symbols on the right side of each production.                                      *
\************************************************************************************/
static const unsigned char prodLengthArray[13] = {
  /*   0 */    1,  4,  4,  2,  2,  2,  1,  1,  2,  3
  /*  10 */ ,  3,  0,  1
}; // Size of table:16(x86)/16(x64) bytes.

/************************************************************************************\
* leftSideTable[] is indexed by production number.                                   *
* leftSideTable[p] = A', A' = (A - terminalCount)                                    *
*                        where A is the left side of production p.                   *
* A' = 0..nonterminalCount-1.                                                        *
* p  = 0..productionCount-1                                                          *
\************************************************************************************/
static const unsigned char leftSideArray[13] = {
  /*   0 */    0,  1,  1,  1,  1,  1,  2,  4,  5,  5
  /*  10 */ ,  5,  3,  3
}; // Size of table:16(x86)/16(x64) bytes.

/************************************************************************************\
* rightSideTable[] holds a compressed form of the rightsides of all                  *
* productions in the grammar. Only used for debugging.                               *
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
* symbolNames is a space separated string with the names of all symbols used in      *
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
  " start"                                            /* NT    7 NTindex=0     */
  " S"                                                /* NT    8 NTindex=1     */
  " A"                                                /* NT    9 NTindex=2     */
  " E"                                                /* NT   10 NTindex=3     */
  " B"                                                /* NT   11 NTindex=4     */
  " D"                                                /* NT   12 NTindex=5     */
}; // Size of string:32(x86)/32(x64) bytes

static const ParserTablesTemplate<13,7,13,21
                                 ,unsigned char
                                 ,unsigned char
                                 ,unsigned char
                                 ,char
                                 ,unsigned char> Olm641Tables_s(prodLengthArray   , leftSideArray
                                                               ,rightSideTable    , symbolNames
                                                               ,380, 464
                                                               ,actionCodeArray   , termArrayTable    , actionArrayTable, termBitSetTable
                                                               ,successorCodeArray, NTindexArrayTable , newStateArrayTable
                                                               );

const AbstractParserTables *Olm641Parser::Olm641Tables = &Olm641Tables_s;
// Size of Olm641Tables_s: 68(x86)/128(x64) bytes. Size of Olm641Tables:4(x86)/8(x64) bytes

// Total size of table data:380(x86)/464(x64) bytes.
