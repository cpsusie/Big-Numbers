/* DO NOT EDIT THIS FILE - it is machine generated */
#line 31 "C:\\Mytools2015\\ParserGen\\Olm627\\Olm627.y"
#include "stdafx.h"
#include "Olm627Parser.h"
#line 36 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"
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

#define _ac0000 0x00000000 /* termList    0, actionList    0                        */
#define _ac0001 0x00010000 /* Reduce by   0 on EOI                                  */
#define _ac0002 0x000b0005 /* Shift  to   5 on f                                    */
#define _ac0003 0x000f0005 /* Shift  to   7 on f                                    */
#define _ac0004 0x00110004 /* Shift  to   8 on d                                    */
#define _ac0005 0x00040003 /* termList    1, actionList    1                        */
#define _ac0006 0x00150003 /* Shift  to  10 on c                                    */
#define _ac0007 0x00080003 /* termList    1, actionList    2                        */
#define _ac0008 0xffff0000 /* Reduce by   1 on EOI                                  */
#define _ac0009 0xfffd0000 /* Reduce by   2 on EOI                                  */
#define _ac0010 0xfffb0000 /* Reduce by   3 on EOI                                  */
#define _ac0011 0xfff90000 /* Reduce by   4 on EOI                                  */

static const unsigned int actionCode[12] = {
   _ac0000,_ac0001,_ac0002,_ac0003,_ac0004,_ac0005,_ac0006,_ac0007,_ac0008,_ac0009
  ,_ac0010,_ac0011
}; // Size of table:48(x86)/48(x64) bytes.

static const unsigned char termListTable[6] = {
     2,   1,   2                                                                                             /*   0 Used by state  [0]                                */
  ,  2,   3,   4                                                                                             /*   1 Used by states [5,7]                              */
}; // Size of table:8(x86)/8(x64) bytes.

static const char actionListTable[6] = {
      2,   3                                                                                                 /*   0 Used by state  [0]                                */
  ,   9,  -5                                                                                                 /*   1 Used by state  [5]                                */
  ,  -5,  11                                                                                                 /*   2 Used by state  [7]                                */
}; // Size of table:8(x86)/8(x64) bytes.

#define termSetTable nullptr

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

#define _su0000 0x00030001 /* Goto 1 on S                                           */
#define _su0002 0x00090002 /* Goto 4 on A                                           */
#define _su0003 0x000d0002 /* Goto 6 on A                                           */

#define nil (unsigned int)-1
static const unsigned int successorCode[12] = {
   _su0000,nil    ,_su0002,_su0003,nil    ,nil    ,nil    ,nil    ,nil    ,nil
  ,nil    ,nil
}; // Size of table:48(x86)/48(x64) bytes.

#define NTindexListTable nullptr
#define stateListTable   nullptr

/************************************************************************************\
* The productionLength[] is indexed by production number and holds the number of     *
* symbols on the right side of each production.                                      *
\************************************************************************************/
static const unsigned char productionLength[6] = {
  /*   0 */    1,  3,  3,  3,  3,  1
}; // Size of table:8(x86)/8(x64) bytes.

/************************************************************************************\
* leftSideTable[] is indexed by production number.                                   *
* leftSideTable[p] = A', A' = (A - terminalCount)                                    *
*                        where A is the left side of production p.                   *
* A' = 0..nonterminalCount-1.                                                        *
* p  = 0..productionCount-1                                                          *
\************************************************************************************/
static const unsigned char leftSideTable[6] = {
  /*   0 */    0,  1,  1,  1,  1,  2
}; // Size of table:8(x86)/8(x64) bytes.

/************************************************************************************\
* rightSideTable[] holds a compressed form of the rightsides of all                  *
* productions in the grammar. Only used for debugging.                               *
\************************************************************************************/
static const unsigned char rightSideTable[14] = {
  /*   0 */    7
  /*   1 */ ,  1,  8,  4
  /*   2 */ ,  1,  5,  3
  /*   3 */ ,  2,  8,  3
  /*   4 */ ,  2,  5,  4
  /*   5 */ ,  5
}; // Size of table:16(x86)/16(x64) bytes.

/************************************************************************************\
* symbolNames is a space separated string with the names of all symbols used in      *
* grammar, terminals and nonTerminals. Only used for debugging.                      *
\************************************************************************************/
static const char *symbolNames = {
  "EOI"                                               /* T     0               */
  " a"                                                /* T     1               */
  " b"                                                /* T     2               */
  " c"                                                /* T     3               */
  " d"                                                /* T     4               */
  " f"                                                /* T     5               */
  " start"                                            /* NT    6 NTindex=0     */
  " S"                                                /* NT    7 NTindex=1     */
  " A"                                                /* NT    8 NTindex=2     */
}; // Size of string:24(x86)/24(x64) bytes

static const ParserTablesTemplate<6,9,6,12
                                 ,unsigned char
                                 ,unsigned char
                                 ,unsigned char
                                 ,char
                                 ,unsigned char> Olm627Tables_s(actionCode      , termListTable     , actionListTable, termSetTable
                                                               ,successorCode   , NTindexListTable  , stateListTable
                                                               ,productionLength, leftSideTable
                                                               ,rightSideTable  , symbolNames
                                                               ,240, 304);

const AbstractParserTables *Olm627Parser::Olm627Tables = &Olm627Tables_s;
// Size of Olm627Tables_s: 68(x86)/128(x64) bytes. Size of Olm627Tables:4(x86)/8(x64) bytes

// Total size of table data:240(x86)/304(x64) bytes.
