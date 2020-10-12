/* DO NOT EDIT THIS FILE - it is machine generated */

#line 33 "C:\\Mytools2015\\ParserGen\\Olm637\\Olm637.y"
#include "stdafx.h"
#include "Olm637parser.h"
#line 44 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"

#pragma warning(disable : 4312)



typedef char TableType;

/******************************************************************************\
* The action matrix holds the parse action(state,terminal)                     *
* Used in LRParser::parserStep() tp determine what to do in the current state  *
* and a given terminal on input. The interpretation of action is:              *
*                                                                              *
*   action <  0   - Reduce by production p, p == -action.                      *
*   action == 0   - Accept. Reduce by production 0.                            *
*   action >  0   - Go to state s (=action),                                   *
*                   and push [s,input,pos] to parser stack.                    *
*                   Then read next symbol from input.                          *
*   action == _ParserError - not found (=unexpected input).                    *
*                                                                              *
* 3 different formats are used:1 Uncompressed and 2 Compressed.                *
* Uncompressed state:                                                          *
*   The array uncompressedActions[] holds a list of numbers for each state     *
*   starting with number of items, M, belonging to the state, followed by M    *
*   pairs, each consisting of (token,action)                                   *
*     Item list for state NNNN with 2 items             2, 1, 2, 2,-3          *
*     Number of pairs in list---------------------------+  |  |  |  |          *
*     Legal input symbol-----------------------------------+  |  |  |          *
*     Action-(> 0 => shift input and goto state 1)------------+  |  |          *
*     Legal input symbol-----------------------------------------+  |          *
*     Action-(< 0 => reduce by production 3)------------------------+          *
*   A line containing "#define _acNNNN Index" is added, where Index is a       *
*   reference to the first number in list of numbers belonging to the state    *
*                                                                              *
* Compressed state:                                                            *
*   Single-item-state (SIS) ot Multi-item-state (MIS)                          *
*   SIS:If there is only 1 possible action in the state:                       *
*     A line containing "#define _acNNNN code" is added, where code is         *
*     encoded as:          ((Action&0x7fff) << 16) | (Token&0x7fff) )          *
*     Bit[16-30] = Action-------+                       |                      *
*     Bit[0 -14] = Legal token--------------------------+                      *
*     Bit 31     = 1 Compressed state indicator                                *
*     Bit 15     = 0 SIS compression                                           *
*                                                                              *
*   MIS:If all actions in the state are reduce by the same production:         *
*     A line containing "#define _acNNNN code" is added, where code is         *
*     encoded as:          ((Action&0x7fff) << 16) | (Index&0x7fff))           *
*     Bit[16-30] = Action-------+                       |                      *
*     Bit[0 -14] = Index into compressedLasets----------+                      *
*     Bit 31     = 1 Compressed state indicator                                *
*     Bit 15     = 1 MIS compression                                           *
*                                                                              *
* CompressedLasets is a list of bitsets, containing 1-bits for all legal       *
* inputsymbols in the given state (MIS). Many MIS may refer to the same bitset *
* Number of bytes in each bitset = (terminalcount-1)/8+1                       *
* Index in MIS-code refer to the first byte in the bitset belonging the state  *
*                                                                              *
* If any uncompressed state N has the same actionarray as a previous           *
* generated uncompressed state M, a #define _acN _acM                          *
*                                                                              *
\******************************************************************************/
static const BYTE compressedLasets[1] = {/*  0   2 tokens */  0x18
}; // Size of table:4(x86)/8(x64) bytes.

static const TableType uncompressedActions[] = {
  /* state[0],Index=0                     */    2,   1,   2,   2,   3
}; // Size of table:8(x86)/8(x64) bytes.

#define _ac0000 0          /* Index of uncompressed state[0]          */
#define _ac0001 0x80000000 /* Reduce by 0 on EOI                      */
#define _ac0002 0x80060005 /* Shift  to 6 on f                        */
#define _ac0003 0x80060005 /* Shift  to 6 on f                        */
#define _ac0004 0x80090004 /* Shift  to 9 on d                        */
#define _ac0005 0x800a0003 /* Shift  to 10 on c                       */
#define _ac0006 0xfffb8000 /* Reduce by 5 on tokens in set[0]         */
#define _ac0007 0x800b0003 /* Shift  to 11 on c                       */
#define _ac0008 0x800c0004 /* Shift  to 12 on d                       */
#define _ac0009 0xffff0000 /* Reduce by 1 on EOI                      */
#define _ac0010 0xfffe0000 /* Reduce by 2 on EOI                      */
#define _ac0011 0xfffd0000 /* Reduce by 3 on EOI                      */
#define _ac0012 0xfffc0000 /* Reduce by 4 on EOI                      */

static const unsigned int actionCode[13] = {
   _ac0000,_ac0001,_ac0002,_ac0003,_ac0004,_ac0005,_ac0006,_ac0007,_ac0008,_ac0009
  ,_ac0010,_ac0011,_ac0012
}; // Size of table:52(x86)/56(x64) bytes.

/******************************************************************************\
* The successor matrix is used when the parser has reduced by prod A -> alfa   *
* The number of elements popped from the stack is the length of the alfa, L.   *
* and the state is taken from stacktop. The nonterminal A is leftside of the   *
* reduce production                                                            *
* Used by LRParser to find newstate = successor(state,A).                      *
* For each relevant state the array stateSuccessors contains a list of numbers *
*                                                                              *
*   Item list for state NNNN with 3 items   3, 5,3 , 6,2, 8,5                  *
*   Number of pairs in list ----------------+  | |                             *
*   NonTerminal A------------------------------+ |                             *
*   Goto this state------------------------------+                             *
*                                                                              *
* The array successorsIndex contains an index for each of these states         *
* referering to the first number belonging to the state (as actionCode)        *
* or 0 if a state has no items of the form B -> beta . A gamma                 *
\******************************************************************************/
static const TableType stateSuccessors[] = {
  /* successors state[0], Index=0         */    1,   7,   1
  /* successors state[2], Index=3         */ ,  2,   8,   4,   9,   5
  /* successors state[3], Index=8         */ ,  2,   8,   7,   9,   8
}; // Size of table:16(x86)/16(x64) bytes.

#define nil (unsigned char)-1
static const unsigned char successorsIndex[13] = {
       0,  nil,    3,    8,  nil,  nil,  nil,  nil,  nil,  nil
  ,  nil,  nil,  nil
}; // Size of table:16(x86)/16(x64) bytes.

/******************************************************************************\
* The productionLength[] array is indexed by production number and holds       *
* the number of symbols on the right side of each production.                  *
\******************************************************************************/
static const unsigned char productionLength[7] = {
  /*   0 */    1,   3,   3,   3,   3,   1,   1
}; // Size of table:8(x86)/8(x64) bytes.

/******************************************************************************\
* The leftSide[] array is indexed by production number, and holds the          *
* nonTerminal A on the left side of each production.                           *
\******************************************************************************/
static const TableType leftSide[7] = {
  /*   0 */    6,   7,   7,   7,   7,   8,   9
}; // Size of table:8(x86)/8(x64) bytes.

/******************************************************************************\
* The rightSide[] matrix is indexed by production number and holds             *
* the right side symbols of each production.                                   *
* Compressed and only used for debugging.                                      *
\******************************************************************************/
static const TableType rightSideTable[15] = {
  /*   0 */    7
  /*   1 */ ,  1,  8,  4
  /*   2 */ ,  1,  9,  3
  /*   3 */ ,  2,  8,  3
  /*   4 */ ,  2,  9,  4
  /*   5 */ ,  5
  /*   6 */ ,  5
}; // Size of table:16(x86)/16(x64) bytes.

/******************************************************************************\
* symbolNames contains names of terminal and nonTerminal separated by space    *
* Used for debugging.                                                          *
\******************************************************************************/
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
  " B"
}; // Total size of strings:28(x86)/32(x64) bytes

static const ParserTablesTemplate<TableType, unsigned char> Olm637Tables_s(actionCode, compressedLasets, uncompressedActions
                                                                          ,successorsIndex , stateSuccessors
                                                                          ,productionLength, leftSide
                                                                          ,rightSideTable  , symbolNames
                                                                          ,6, 10, 7, 13, 228, 296);

const ParserTables *Olm637Parser::Olm637Tables = &Olm637Tables_s;
// Size of Olm637Tables_s: 68(x86)/120(x64) bytes. Size of Olm637Tables:4(x86)/8(x64) bytes

// Total size of table data:228(x86)/296(x64) bytes.


