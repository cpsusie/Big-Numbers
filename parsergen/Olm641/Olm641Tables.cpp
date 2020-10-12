/* DO NOT EDIT THIS FILE - it is machine generated */

#include "stdafx.h"
#include "olm641parser.h"

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
#define compressedLasets NULL

static const TableType uncompressedActions[] = {
  /* state[0],Index=0                     */    2,   1,   2,   3,   5
  /* state[2],Index=5                     */ ,  2,   2,   6,   4,   8
  /* state[5],Index=10                    */ ,  3,   0,  -6,   5,  -6,   6,  -7
  /* state[8],Index=17                    */ ,  2,   1,  15,   3,   5
  /* state[11],Index=22                   */ ,  2,   0, -11,   5,  17
}; // Size of table:28(x86)/32(x64) bytes.

#define _ac0000 0          /* Index of uncompressed state[0]          */
#define _ac0001 0x80000000 /* Reduce by 0 on EOI                      */
#define _ac0002 5          /* Index of uncompressed state[2]          */
#define _ac0003 0x80090005 /* Shift  to 9 on z                        */
#define _ac0004 0x800a0006 /* Shift  to 10 on x                       */
#define _ac0005 10         /* Index of uncompressed state[5]          */
#define _ac0006 0x80050003 /* Shift  to 5 on d                        */
#define _ac0007 0xfffd0000 /* Reduce by 3 on EOI                      */
#define _ac0008 17         /* Index of uncompressed state[8]          */
#define _ac0009 0xfffc0000 /* Reduce by 4 on EOI                      */
#define _ac0010 0xfffb0000 /* Reduce by 5 on EOI                      */
#define _ac0011 22         /* Index of uncompressed state[11]         */
#define _ac0012 0x80120006 /* Shift  to 18 on x                       */
#define _ac0013 0xfff80000 /* Reduce by 8 on EOI                      */
#define _ac0014 0x80130006 /* Shift  to 19 on x                       */
#define _ac0015 0x80080004 /* Shift  to 8 on f                        */
#define _ac0016 0xffff0000 /* Reduce by 1 on EOI                      */
#define _ac0017 0xfff40000 /* Reduce by 12 on EOI                     */
#define _ac0018 0xfffe0000 /* Reduce by 2 on EOI                      */
#define _ac0019 0xfff70000 /* Reduce by 9 on EOI                      */
#define _ac0020 0xfff60000 /* Reduce by 10 on EOI                     */

static const unsigned int actionCode[21] = {
   _ac0000,_ac0001,_ac0002,_ac0003,_ac0004,_ac0005,_ac0006,_ac0007,_ac0008,_ac0009
  ,_ac0010,_ac0011,_ac0012,_ac0013,_ac0014,_ac0015,_ac0016,_ac0017,_ac0018,_ac0019
  ,_ac0020
}; // Size of table:84(x86)/88(x64) bytes.

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
  /* successors state[0], Index=0         */    3,   8,   1,   9,   3,  11,   4
  /* successors state[2], Index=7         */ ,  1,  12,   7
  /* successors state[6], Index=10        */ ,  2,   9,  11,  11,  12
  /* successors state[8], Index=15        */ ,  2,   9,  13,  11,  14
  /* successors state[11], Index=20       */ ,  1,  10,  16
  /* successors state[15], Index=23       */ ,  1,  12,  20
}; // Size of table:28(x86)/32(x64) bytes.

#define nil (unsigned char)-1
static const unsigned char successorsIndex[21] = {
       0,  nil,    7,  nil,  nil,  nil,   10,  nil,   15,  nil
  ,  nil,   20,  nil,  nil,  nil,   23,  nil,  nil,  nil,  nil
  ,  nil
}; // Size of table:24(x86)/24(x64) bytes.

/******************************************************************************\
* The productionLength[] array is indexed by production number and holds       *
* the number of symbols on the right side of each production.                  *
\******************************************************************************/
static const unsigned char productionLength[13] = {
  /*   0 */    1,   4,   4,   2,   2,   2,   1,   1,   2,   3,
  /*  10 */    3,   0,   1
}; // Size of table:16(x86)/16(x64) bytes.

/******************************************************************************\
* The leftSide[] array is indexed by production number, and holds the          *
* nonTerminal A on the left side of each production.                           *
\******************************************************************************/
static const TableType leftSide[13] = {
  /*   0 */    7,   8,   8,   8,   8,   8,   9,  11,  12,  12,
  /*  10 */   12,  10,  10
}; // Size of table:16(x86)/16(x64) bytes.

/******************************************************************************\
* The rightSide[] matrix is indexed by production number and holds             *
* the right side symbols of each production.                                   *
* Compressed and only used for debugging.                                      *
\******************************************************************************/
static const TableType rightSideTable[26] = {
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

/******************************************************************************\
* symbolNames contains names of terminal and nonTerminal separated by space    *
* Used for debugging.                                                          *
\******************************************************************************/
static const char *symbolNames = {
  "EOI"
  " a"
  " b"
  " d"
  " f"
  " z"
  " x"
  " start"
  " S"
  " A"
  " E"
  " B"
  " D"
}; // Total size of strings:32(x86)/32(x64) bytes

static const ParserTablesTemplate<TableType, unsigned char> Olm641Tables_s(actionCode, compressedLasets, uncompressedActions
                                                                          ,successorsIndex , stateSuccessors
                                                                          ,productionLength, leftSide
                                                                          ,rightSideTable  , symbolNames
                                                                          ,7, 13, 13, 21, 328, 400);

const ParserTables *Olm641Parser::Olm641Tables = &Olm641Tables_s;
// Size of Olm641Tables_s: 68(x86)/120(x64) bytes. Size of Olm641Tables:4(x86)/8(x64) bytes

// Total size of table data:328(x86)/400(x64) bytes.


