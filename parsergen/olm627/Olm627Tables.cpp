/* DO NOT EDIT THIS FILE - it is machine generated */

#line 31 "C:\\Mytools2015\\ParserGen\\Olm627\\Olm627.y"
#include "stdafx.h"
#include "Olm627Parser.h"
#line 44 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"

#pragma warning(disable : 4312)



/**********************************************************************************\
* The 4 arrays actionCode, termListTable, actionstateListTable, compressedLAsets   *
* holds a compressed action-matrix, used by LRParser to find                       *
* action = getAction(S,T), where S is current state, T is next terminal on input   *
*                                                                                  *
* The interpretation of action is:                                                 *
*   action <  0 - Reduce by production p, p == -action.                            *
*   action == 0 - Accept. Reduce by production 0.                                  *
*   action >  0 - Go to state newstate (=action),                                  *
*                 and push newstate.                                               *
*                 Then advance input 1 symbol                                      *
*   action == _ParserError -  Unexpected input - do some recovery, to synchronize. *
*                 input and stack                                                  *
*                                                                                  *
* For each state S, a #define is generated and used as element S in array          *
* actionCode. Each define has the format:                                          *
*                    3         2         1         0                               *
*                   10987654321098765432109876543210                               *
* #define _acSSSS 0xaaaaaaaaaaaaaaaCIttttttttttttttt                               *
* where SSSS : The statenumber S                                                   *
* t          : Bit[0-14],  unsigned short                                          *
* a          : Bit[17-31], signed short                                            *
* CI         : Bit[15-16], indicates how to interpret t and a.                     *
* If C is 0  : Uncompressed format                                                 *
*              t: Index into array termListTable, pointing at the first            *
*                 element of termList (see below).                                 *
*              a: Index into array actionListTable, pointing at the first          *
*                 element of actionList (see below)                                *
* If C is 1  : Compressed format                                                   *
*              This format is used if there is only 1 possible action, a.          *
*    I=0 (bit 15) There is only 1 legal terminal in this state                     *
*              t: legal terminal                                                   *
*              a: action                                                           *
*                                                                                  *
*    I=1 If all actions in the state are reduce by the same production             *
*              t: Index into compressedLAsets, pointing at the first element       *
*                 of LASet (see below)                                             *
*              a: action                                                           *
*                                                                                  *
* For Uncompressed states (C=0) then use arrays termListTable and actionListTable. *
*    n                 : termListTable[t] = number of elements in the list         *
*    termList[  0..n-1]: termListTable[t+1]..termListTable[t+n]                    *
*                        ordered list of terminals, of length n                    *
*    actionList[0..n-1]: actionListTable[a]..actionListTable[a+n-1], length = n    *
*                                                                                  *
*    To get the action, find the index k in termList, so termList[k] = T           *
*    and then pick actionList[k]. If T is not found, set action = _ParseError      *
*                                                                                  *
* For Compressed states, C=1 and I=1 then use array compressedLAsets which is a    *
* list of bitset(0..terminalCount-1), LASet. Number of bytes in each LAset         *
*                                                                                  *
*    b                 : (terminalcount-1)/8+1                                     *
*    LAset[0..b-1]     : compressedLAsets[t]..compressedLAsets[t+b-1]              *
*                                                                                  *
* As for Uncompressed states, the same check for existence is done. If terminal T  *
* is not found, set action = _ParseError.                                          *
\**********************************************************************************/
#define _ac0000 0x00000000 /* termList   0, actionList   0            */
#define _ac0001 0x00010000 /* Reduce by 0 on EOI                      */
#define _ac0002 0x000b0005 /* Shift  to 5 on f                        */
#define _ac0003 0x000f0005 /* Shift  to 7 on f                        */
#define _ac0004 0x00110004 /* Shift  to 8 on d                        */
#define _ac0005 0x00040003 /* termList   1, actionList   1            */
#define _ac0006 0x00150003 /* Shift  to 10 on c                       */
#define _ac0007 0x00080003 /* termList   1, actionList   2            */
#define _ac0008 0xffff0000 /* Reduce by 1 on EOI                      */
#define _ac0009 0xfffd0000 /* Reduce by 2 on EOI                      */
#define _ac0010 0xfffb0000 /* Reduce by 3 on EOI                      */
#define _ac0011 0xfff90000 /* Reduce by 4 on EOI                      */

static const unsigned int actionCode[12] = {
   _ac0000,_ac0001,_ac0002,_ac0003,_ac0004,_ac0005,_ac0006,_ac0007,_ac0008,_ac0009
  ,_ac0010,_ac0011
}; // Size of table:48(x86)/48(x64) bytes.

static const unsigned char termListTable[6] = {
     2,   1,   2                                                                                             /*    0 Used by state  (0) */
  ,  2,   3,   4                                                                                             /*    1 Used by states (5,7) */
}; // Size of table:8(x86)/8(x64) bytes.

static const char actionListTable[6] = {
      2,   3                                                                                                 /*   0 Used by state  (0) */
  ,   9,  -5                                                                                                 /*   1 Used by state  (5) */
  ,  -5,  11                                                                                                 /*   2 Used by state  (7) */
}; // Size of table:8(x86)/8(x64) bytes.

#define compressedLAsets nullptr

/**********************************************************************************\
* The 3 arrays NTindexListTable, stateListTable and successorCode holds a          *
* compressed succesor-matrix, used by LRParser to find newstate = successor(S,A)   *
* as last part of a reduction with production P, A -> alfa. The number of elements *
* popped from the stack is the length of alfa, the state S is then taken from      *
* stacktop, the nonterminal A is leftside of the reduce production P.              *
* To complete the reduction, push(newstate)                                        *
* For each relevant state S, a #define is generated and used as element S in array *
* successorCode. Each define has the format:                                       *
*                    3         2         1         0                               *
*                   10987654321098765432109876543210                               *
* #define _suSSSS 0xrrrrrrrrrrrrrrrCiiiiiiiiiiiiiiii                               *
* where SSSS : The statenumber S                                                   *
* i          : Bit[ 0-15], unsigned short                                          *
* r          : Bit[17-31], unsigned short                                          *
* C          : Bit 16, indicates how to interpret i and r.                         *
* If C is 0  : Uncompressed format                                                 *
*              i is the index into array NTindexListTable, pointing at the first   *
*              element of NTIndexList (see below).                                 *
*              r is the index into array stateListTable, pointing at the first     *
*              element of stateList (see below)                                    *
* If C is 1  : Compressed format                                                   *
*              i is the index A' of nonterminal A, A' = (A - terminalCount)        *
*              r is the new state. This format is used if there is only 1 possible *
*              successor-state.                                                    *
*                                                                                  *
* For Uncompressed states (C=0) then use arrays NTIndexListTable and stateTable.   *
*    n                  : NTIndexListTable[i] = number of elements in the list     *
*    NTIndexList[0..n-1]: NTIndexListTable[i+1]..NTIndexListTable[i+n]             *
*                         ordered list of non-terminals-indices, of length n       *
*    stateList[  0..n-1]: stateListTable[r]..stateListTable[r+n-1], length = n     *
*                                                                                  *
* To get the new state, find the index k in NTIndexList, so NTIndexList[k] = A'    *
* and use stateList[k] as the new state. Note that the non-terminal always exist   *
\**********************************************************************************/
#define _su0000 0x00030001 /* Goto 1 on S                             */
#define _su0002 0x00090002 /* Goto 4 on A                             */
#define _su0003 0x000d0002 /* Goto 6 on A                             */

#define nil (unsigned int)-1
static const unsigned int successorCode[12] = {
   _su0000,nil    ,_su0002,_su0003,nil    ,nil    ,nil    ,nil    ,nil    ,nil
  ,nil    ,nil
}; // Size of table:48(x86)/48(x64) bytes.

#define NTindexListTable nullptr
#define stateListTable   nullptr

/**********************************************************************************\
* The productionLength[] array is indexed by production number and holds           *
* the number of symbols on the right side of each production.                      *
\**********************************************************************************/
static const unsigned char productionLength[6] = {
  /*   0 */    1,   3,   3,   3,   3,   1
}; // Size of table:8(x86)/8(x64) bytes.

/*********************************************************************************\
* The leftSide[] array is indexed by production number, and holds the             *
* index, A' of nonTerminal A on the left side of each production. A'=A-#terminals *
\*********************************************************************************/
static const unsigned char leftSideTable[6] = {
  /*   0 */    0,   1,   1,   1,   1,   2
}; // Size of table:8(x86)/8(x64) bytes.

/*********************************************************************************\
* The rightSide[] matrix is indexed by production number and holds                *
* the right side symbols of each production.                                      *
* Compressed and only used for debugging.                                         *
\*********************************************************************************/
static const unsigned char rightSideTable[14] = {
  /*   0 */    7
  /*   1 */ ,  1,  8,  4
  /*   2 */ ,  1,  5,  3
  /*   3 */ ,  2,  8,  3
  /*   4 */ ,  2,  5,  4
  /*   5 */ ,  5
}; // Size of table:16(x86)/16(x64) bytes.

/********************************************************************************\
* symbolNames contains names of terminal and nonTerminal separated by space      *
* Used for debugging.                                                            *
\********************************************************************************/
static const char *symbolNames = {
  "EOI"
  " a"
  " b"
  " c"
  " d"
  " f"
  " start"
  " S"
  " A"
}; // Total size of string:24(x86)/24(x64) bytes

static const ParserTablesTemplate<6,9,6,12
                                 ,unsigned char
                                 ,unsigned char
                                 ,unsigned char
                                 ,char
                                 ,unsigned char> Olm627Tables_s(actionCode      , termListTable     , actionListTable, compressedLAsets
                                                               ,successorCode   , NTindexListTable  , stateListTable
                                                               ,productionLength, leftSideTable
                                                               ,rightSideTable  , symbolNames
                                                               ,240, 296);

const ParserTables *Olm627Parser::Olm627Tables = &Olm627Tables_s;
// Size of Olm627Tables_s: 68(x86)/120(x64) bytes. Size of Olm627Tables:4(x86)/8(x64) bytes

// Total size of table data:240(x86)/296(x64) bytes.


