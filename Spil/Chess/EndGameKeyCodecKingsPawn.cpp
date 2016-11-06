#include "stdafx.h"
#include "EndGameUtil.h"
#include "EndGameKeyCodec.h"

// -------------------------------------- Inner fields ------------------------------------

static const char _wkB2BkP2I[64] = {
  -1,-1,-1, 0, 1, 2, 3, 4
 ,-1,-1,-1, 5, 6, 7, 8, 9
 ,-1,-1,-1,10,11,12,13,14
 ,15,16,17,18,19,20,21,22
 ,23,24,25,26,27,28,29,30
 ,31,32,33,34,35,36,37,38
 ,39,40,41,42,43,44,45,46
 ,47,48,49,50,51,52,53,54
};
static const char _wkB2BkI2P[55] = {
           D1,E1,F1,G1,H1
          ,D2,E2,F2,G2,H2
          ,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
const char _wkC2BkP2I[64] = { /*non-static*/
   0,-1,-1,-1, 1, 2, 3, 4
 , 5,-1,-1,-1, 6, 7, 8, 9
 ,10,-1,-1,-1,11,12,13,14
 ,15,16,17,18,19,20,21,22
 ,23,24,25,26,27,28,29,30
 ,31,32,33,34,35,36,37,38
 ,39,40,41,42,43,44,45,46
 ,47,48,49,50,51,52,53,54
};
const char _wkC2BkI2P[55] = { /*non-static*/
  A1         ,E1,F1,G1,H1
 ,A2         ,E2,F2,G2,H2
 ,A3         ,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
const char _wkD2BkP2I[64] = { /*non-static*/
   0, 1,-1,-1,-1, 2, 3, 4
 , 5, 6,-1,-1,-1, 7, 8, 9
 ,10,11,-1,-1,-1,12,13,14
 ,15,16,17,18,19,20,21,22
 ,23,24,25,26,27,28,29,30
 ,31,32,33,34,35,36,37,38
 ,39,40,41,42,43,44,45,46
 ,47,48,49,50,51,52,53,54
};
const char _wkD2BkI2P[55] = { /*non-static*/
  A1,B1         ,F1,G1,H1
 ,A2,B2         ,F2,G2,H2
 ,A3,B3         ,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkE2BkP2I[64] = {
   0, 1, 2,-1,-1,-1, 3, 4
 , 5, 6, 7,-1,-1,-1, 8, 9
 ,10,11,12,-1,-1,-1,13,14
 ,15,16,17,18,19,20,21,22
 ,23,24,25,26,27,28,29,30
 ,31,32,33,34,35,36,37,38
 ,39,40,41,42,43,44,45,46
 ,47,48,49,50,51,52,53,54
};
static const char _wkE2BkI2P[55] = {
  A1,B1,C1         ,G1,H1
 ,A2,B2,C2         ,G2,H2
 ,A3,B3,C3         ,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkF2BkP2I[64] = {
   0, 1, 2, 3,-1,-1,-1, 4
 , 5, 6, 7, 8,-1,-1,-1, 9
 ,10,11,12,13,-1,-1,-1,14
 ,15,16,17,18,19,20,21,22
 ,23,24,25,26,27,28,29,30
 ,31,32,33,34,35,36,37,38
 ,39,40,41,42,43,44,45,46
 ,47,48,49,50,51,52,53,54
};
static const char _wkF2BkI2P[55] = {
  A1,B1,C1,D1         ,H1
 ,A2,B2,C2,D2         ,H2
 ,A3,B3,C3,D3         ,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkG2BkP2I[64] = {
   0, 1, 2, 3, 4,-1,-1,-1
 , 5, 6, 7, 8, 9,-1,-1,-1
 ,10,11,12,13,14,-1,-1,-1
 ,15,16,17,18,19,20,21,22
 ,23,24,25,26,27,28,29,30
 ,31,32,33,34,35,36,37,38
 ,39,40,41,42,43,44,45,46
 ,47,48,49,50,51,52,53,54
};
static const char _wkG2BkI2P[55] = {
  A1,B1,C1,D1,E1
 ,A2,B2,C2,D2,E2
 ,A3,B3,C3,D3,E3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};

static const char _wkB3BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 ,-1,-1,-1, 8, 9,10,11,12
 ,-1,-1,-1,13,14,15,16,17
 ,-1,-1,-1,18,19,20,21,22
 ,23,24,25,26,27,28,29,30
 ,31,32,33,34,35,36,37,38
 ,39,40,41,42,43,44,45,46
 ,47,48,49,50,51,52,53,54
};
static const char _wkB3BkI2P[55] = {
  A1,B1,C1,D1,E1,F1,G1,H1
          ,D2,E2,F2,G2,H2
          ,D3,E3,F3,G3,H3
          ,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkC3BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8,-1,-1,-1, 9,10,11,12
 ,13,-1,-1,-1,14,15,16,17
 ,18,-1,-1,-1,19,20,21,22
 ,23,24,25,26,27,28,29,30
 ,31,32,33,34,35,36,37,38
 ,39,40,41,42,43,44,45,46
 ,47,48,49,50,51,52,53,54
};
static const char _wkC3BkI2P[55] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2         ,E2,F2,G2,H2
 ,A3         ,E3,F3,G3,H3
 ,A4         ,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
const char _wkD3BkP2I[64] = { /*non-static*/
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,-1,-1,-1,10,11,12
 ,13,14,-1,-1,-1,15,16,17
 ,18,19,-1,-1,-1,20,21,22
 ,23,24,25,26,27,28,29,30
 ,31,32,33,34,35,36,37,38
 ,39,40,41,42,43,44,45,46
 ,47,48,49,50,51,52,53,54
};
const char _wkD3BkI2P[55] = { /*non-static*/
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2         ,F2,G2,H2
 ,A3,B3         ,F3,G3,H3
 ,A4,B4         ,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkE3BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,-1,-1,-1,11,12
 ,13,14,15,-1,-1,-1,16,17
 ,18,19,20,-1,-1,-1,21,22
 ,23,24,25,26,27,28,29,30
 ,31,32,33,34,35,36,37,38
 ,39,40,41,42,43,44,45,46
 ,47,48,49,50,51,52,53,54
};
static const char _wkE3BkI2P[55] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2         ,G2,H2
 ,A3,B3,C3         ,G3,H3
 ,A4,B4,C4         ,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkF3BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,-1,-1,-1,12
 ,13,14,15,16,-1,-1,-1,17
 ,18,19,20,21,-1,-1,-1,22
 ,23,24,25,26,27,28,29,30
 ,31,32,33,34,35,36,37,38
 ,39,40,41,42,43,44,45,46
 ,47,48,49,50,51,52,53,54
};
static const char _wkF3BkI2P[55] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2         ,H2
 ,A3,B3,C3,D3         ,H3
 ,A4,B4,C4,D4         ,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkG3BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,-1,-1,-1
 ,13,14,15,16,17,-1,-1,-1
 ,18,19,20,21,22,-1,-1,-1
 ,23,24,25,26,27,28,29,30
 ,31,32,33,34,35,36,37,38
 ,39,40,41,42,43,44,45,46
 ,47,48,49,50,51,52,53,54
};
static const char _wkG3BkI2P[55] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2
 ,A3,B3,C3,D3,E3
 ,A4,B4,C4,D4,E4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkB4BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,-1,-1,-1,16,17,18,19,20
 ,-1,-1,-1,21,22,23,24,25
 ,-1,-1,-1,26,27,28,29,30
 ,31,32,33,34,35,36,37,38
 ,39,40,41,42,43,44,45,46
 ,47,48,49,50,51,52,53,54
};
static const char _wkB4BkI2P[55] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
          ,D3,E3,F3,G3,H3
          ,D4,E4,F4,G4,H4
          ,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkC4BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,-1,-1,-1,17,18,19,20
 ,21,-1,-1,-1,22,23,24,25
 ,26,-1,-1,-1,27,28,29,30
 ,31,32,33,34,35,36,37,38
 ,39,40,41,42,43,44,45,46
 ,47,48,49,50,51,52,53,54
};
static const char _wkC4BkI2P[55] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3         ,E3,F3,G3,H3
 ,A4         ,E4,F4,G4,H4
 ,A5         ,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkD4BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,-1,-1,-1,18,19,20
 ,21,22,-1,-1,-1,23,24,25
 ,26,27,-1,-1,-1,28,29,30
 ,31,32,33,34,35,36,37,38
 ,39,40,41,42,43,44,45,46
 ,47,48,49,50,51,52,53,54
};
static const char _wkD4BkI2P[55] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3         ,F3,G3,H3
 ,A4,B4         ,F4,G4,H4
 ,A5,B5         ,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkE4BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,-1,-1,-1,19,20
 ,21,22,23,-1,-1,-1,24,25
 ,26,27,28,-1,-1,-1,29,30
 ,31,32,33,34,35,36,37,38
 ,39,40,41,42,43,44,45,46
 ,47,48,49,50,51,52,53,54
};
static const char _wkE4BkI2P[55] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3         ,G3,H3
 ,A4,B4,C4         ,G4,H4
 ,A5,B5,C5         ,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkF4BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,-1,-1,-1,20
 ,21,22,23,24,-1,-1,-1,25
 ,26,27,28,29,-1,-1,-1,30
 ,31,32,33,34,35,36,37,38
 ,39,40,41,42,43,44,45,46
 ,47,48,49,50,51,52,53,54
};
static const char _wkF4BkI2P[55] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3         ,H3
 ,A4,B4,C4,D4         ,H4
 ,A5,B5,C5,D5         ,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkG4BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,-1,-1,-1
 ,21,22,23,24,25,-1,-1,-1
 ,26,27,28,29,30,-1,-1,-1
 ,31,32,33,34,35,36,37,38
 ,39,40,41,42,43,44,45,46
 ,47,48,49,50,51,52,53,54
};
static const char _wkG4BkI2P[55] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3
 ,A4,B4,C4,D4,E4
 ,A5,B5,C5,D5,E5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkB5BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,-1,-1,-1,24,25,26,27,28
 ,-1,-1,-1,29,30,31,32,33
 ,-1,-1,-1,34,35,36,37,38
 ,39,40,41,42,43,44,45,46
 ,47,48,49,50,51,52,53,54
};
static const char _wkB5BkI2P[55] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
          ,D4,E4,F4,G4,H4
          ,D5,E5,F5,G5,H5
          ,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkC5BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,24,-1,-1,-1,25,26,27,28
 ,29,-1,-1,-1,30,31,32,33
 ,34,-1,-1,-1,35,36,37,38
 ,39,40,41,42,43,44,45,46
 ,47,48,49,50,51,52,53,54
};
static const char _wkC5BkI2P[55] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4         ,E4,F4,G4,H4
 ,A5         ,E5,F5,G5,H5
 ,A6         ,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkD5BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,24,25,-1,-1,-1,26,27,28
 ,29,30,-1,-1,-1,31,32,33
 ,34,35,-1,-1,-1,36,37,38
 ,39,40,41,42,43,44,45,46
 ,47,48,49,50,51,52,53,54
};
static const char _wkD5BkI2P[55] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4         ,F4,G4,H4
 ,A5,B5         ,F5,G5,H5
 ,A6,B6         ,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkE5BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,24,25,26,-1,-1,-1,27,28
 ,29,30,31,-1,-1,-1,32,33
 ,34,35,36,-1,-1,-1,37,38
 ,39,40,41,42,43,44,45,46
 ,47,48,49,50,51,52,53,54
};
static const char _wkE5BkI2P[55] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4         ,G4,H4
 ,A5,B5,C5         ,G5,H5
 ,A6,B6,C6         ,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkF5BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,24,25,26,27,-1,-1,-1,28
 ,29,30,31,32,-1,-1,-1,33
 ,34,35,36,37,-1,-1,-1,38
 ,39,40,41,42,43,44,45,46
 ,47,48,49,50,51,52,53,54
};
static const char _wkF5BkI2P[55] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4         ,H4
 ,A5,B5,C5,D5         ,H5
 ,A6,B6,C6,D6         ,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkG5BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,24,25,26,27,28,-1,-1,-1
 ,29,30,31,32,33,-1,-1,-1
 ,34,35,36,37,38,-1,-1,-1
 ,39,40,41,42,43,44,45,46
 ,47,48,49,50,51,52,53,54
};
static const char _wkG5BkI2P[55] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4
 ,A5,B5,C5,D5,E5
 ,A6,B6,C6,D6,E6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkB6BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,24,25,26,27,28,29,30,31
 ,-1,-1,-1,32,33,34,35,36
 ,-1,-1,-1,37,38,39,40,41
 ,-1,-1,-1,42,43,44,45,46
 ,47,48,49,50,51,52,53,54
};
static const char _wkB6BkI2P[55] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
          ,D5,E5,F5,G5,H5
          ,D6,E6,F6,G6,H6
          ,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkC6BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,24,25,26,27,28,29,30,31
 ,32,-1,-1,-1,33,34,35,36
 ,37,-1,-1,-1,38,39,40,41
 ,42,-1,-1,-1,43,44,45,46
 ,47,48,49,50,51,52,53,54
};
static const char _wkC6BkI2P[55] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5         ,E5,F5,G5,H5
 ,A6         ,E6,F6,G6,H6
 ,A7         ,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkD6BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,24,25,26,27,28,29,30,31
 ,32,33,-1,-1,-1,34,35,36
 ,37,38,-1,-1,-1,39,40,41
 ,42,43,-1,-1,-1,44,45,46
 ,47,48,49,50,51,52,53,54
};
static const char _wkD6BkI2P[55] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5         ,F5,G5,H5
 ,A6,B6         ,F6,G6,H6
 ,A7,B7         ,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkE6BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,24,25,26,27,28,29,30,31
 ,32,33,34,-1,-1,-1,35,36
 ,37,38,39,-1,-1,-1,40,41
 ,42,43,44,-1,-1,-1,45,46
 ,47,48,49,50,51,52,53,54
};
static const char _wkE6BkI2P[55] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5         ,G5,H5
 ,A6,B6,C6         ,G6,H6
 ,A7,B7,C7         ,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkF6BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,24,25,26,27,28,29,30,31
 ,32,33,34,35,-1,-1,-1,36
 ,37,38,39,40,-1,-1,-1,41
 ,42,43,44,45,-1,-1,-1,46
 ,47,48,49,50,51,52,53,54
};
static const char _wkF6BkI2P[55] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5         ,H5
 ,A6,B6,C6,D6         ,H6
 ,A7,B7,C7,D7         ,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkG6BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,24,25,26,27,28,29,30,31
 ,32,33,34,35,36,-1,-1,-1
 ,37,38,39,40,41,-1,-1,-1
 ,42,43,44,45,46,-1,-1,-1
 ,47,48,49,50,51,52,53,54
};
static const char _wkG6BkI2P[55] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5
 ,A6,B6,C6,D6,E6
 ,A7,B7,C7,D7,E7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkB7BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,24,25,26,27,28,29,30,31
 ,32,33,34,35,36,37,38,39
 ,-1,-1,-1,40,41,42,43,44
 ,-1,-1,-1,45,46,47,48,49
 ,-1,-1,-1,50,51,52,53,54
};
static const char _wkB7BkI2P[55] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
          ,D6,E6,F6,G6,H6
          ,D7,E7,F7,G7,H7
          ,D8,E8,F8,G8,H8
};
static const char _wkC7BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,24,25,26,27,28,29,30,31
 ,32,33,34,35,36,37,38,39
 ,40,-1,-1,-1,41,42,43,44
 ,45,-1,-1,-1,46,47,48,49
 ,50,-1,-1,-1,51,52,53,54
};
static const char _wkC7BkI2P[55] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6         ,E6,F6,G6,H6
 ,A7         ,E7,F7,G7,H7
 ,A8         ,E8,F8,G8,H8
};
static const char _wkD7BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,24,25,26,27,28,29,30,31
 ,32,33,34,35,36,37,38,39
 ,40,41,-1,-1,-1,42,43,44
 ,45,46,-1,-1,-1,47,48,49
 ,50,51,-1,-1,-1,52,53,54
};
static const char _wkD7BkI2P[55] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6         ,F6,G6,H6
 ,A7,B7         ,F7,G7,H7
 ,A8,B8         ,F8,G8,H8
};
static const char _wkE7BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,24,25,26,27,28,29,30,31
 ,32,33,34,35,36,37,38,39
 ,40,41,42,-1,-1,-1,43,44
 ,45,46,47,-1,-1,-1,48,49
 ,50,51,52,-1,-1,-1,53,54
};
static const char _wkE7BkI2P[55] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6         ,G6,H6
 ,A7,B7,C7         ,G7,H7
 ,A8,B8,C8         ,G8,H8
};
static const char _wkF7BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,24,25,26,27,28,29,30,31
 ,32,33,34,35,36,37,38,39
 ,40,41,42,43,-1,-1,-1,44
 ,45,46,47,48,-1,-1,-1,49
 ,50,51,52,53,-1,-1,-1,54
};
static const char _wkF7BkI2P[55] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6         ,H6
 ,A7,B7,C7,D7         ,H7
 ,A8,B8,C8,D8         ,H8
};
static const char _wkG7BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,24,25,26,27,28,29,30,31
 ,32,33,34,35,36,37,38,39
 ,40,41,42,43,44,-1,-1,-1
 ,45,46,47,48,49,-1,-1,-1
 ,50,51,52,53,54,-1,-1,-1
};
static const char _wkG7BkI2P[55] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6
 ,A7,B7,C7,D7,E7
 ,A8,B8,C8,D8,E8
};

// --------------------------------- Edge fields -------------------------------------

const char _wkB1BkP2I[64] = { /*non-static*/
  -1,-1,-1, 0, 1, 2, 3, 4
 ,-1,-1,-1, 5, 6, 7, 8, 9
 ,10,11,12,13,14,15,16,17
 ,18,19,20,21,22,23,24,25
 ,26,27,28,29,30,31,32,33
 ,34,35,36,37,38,39,40,41
 ,42,43,44,45,46,47,48,49
 ,50,51,52,53,54,55,56,57
};
const char _wkB1BkI2P[58] = { /*non-static*/
           D1,E1,F1,G1,H1
          ,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
const char _wkC1BkP2I[64] = { /*non-static*/
   0,-1,-1,-1, 1, 2, 3, 4
 , 5,-1,-1,-1, 6, 7, 8, 9
 ,10,11,12,13,14,15,16,17
 ,18,19,20,21,22,23,24,25
 ,26,27,28,29,30,31,32,33
 ,34,35,36,37,38,39,40,41
 ,42,43,44,45,46,47,48,49
 ,50,51,52,53,54,55,56,57
};
const char _wkC1BkI2P[58] = { /*non-static*/
  A1         ,E1,F1,G1,H1
 ,A2         ,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
const char _wkD1BkP2I[64] = { /*non-static*/
   0, 1,-1,-1,-1, 2, 3, 4
 , 5, 6,-1,-1,-1, 7, 8, 9
 ,10,11,12,13,14,15,16,17
 ,18,19,20,21,22,23,24,25
 ,26,27,28,29,30,31,32,33
 ,34,35,36,37,38,39,40,41
 ,42,43,44,45,46,47,48,49
 ,50,51,52,53,54,55,56,57
};
const char _wkD1BkI2P[58] = { /*non-static*/
  A1,B1         ,F1,G1,H1
 ,A2,B2         ,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkE1BkP2I[64] = {
   0, 1, 2,-1,-1,-1, 3, 4
 , 5, 6, 7,-1,-1,-1, 8, 9
 ,10,11,12,13,14,15,16,17
 ,18,19,20,21,22,23,24,25
 ,26,27,28,29,30,31,32,33
 ,34,35,36,37,38,39,40,41
 ,42,43,44,45,46,47,48,49
 ,50,51,52,53,54,55,56,57
};
static const char _wkE1BkI2P[58] = {
  A1,B1,C1         ,G1,H1
 ,A2,B2,C2         ,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkF1BkP2I[64] = {
   0, 1, 2, 3,-1,-1,-1, 4
 , 5, 6, 7, 8,-1,-1,-1, 9
 ,10,11,12,13,14,15,16,17
 ,18,19,20,21,22,23,24,25
 ,26,27,28,29,30,31,32,33
 ,34,35,36,37,38,39,40,41
 ,42,43,44,45,46,47,48,49
 ,50,51,52,53,54,55,56,57
};
static const char _wkF1BkI2P[58] = {
  A1,B1,C1,D1         ,H1
 ,A2,B2,C2,D2         ,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkG1BkP2I[64] = {
   0, 1, 2, 3, 4,-1,-1,-1
 , 5, 6, 7, 8, 9,-1,-1,-1
 ,10,11,12,13,14,15,16,17
 ,18,19,20,21,22,23,24,25
 ,26,27,28,29,30,31,32,33
 ,34,35,36,37,38,39,40,41
 ,42,43,44,45,46,47,48,49
 ,50,51,52,53,54,55,56,57
};
static const char _wkG1BkI2P[58] = {
  A1,B1,C1,D1,E1
 ,A2,B2,C2,D2,E2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkA2BkP2I[64] = {
  -1,-1, 0, 1, 2, 3, 4, 5
 ,-1,-1, 6, 7, 8, 9,10,11
 ,-1,-1,12,13,14,15,16,17
 ,18,19,20,21,22,23,24,25
 ,26,27,28,29,30,31,32,33
 ,34,35,36,37,38,39,40,41
 ,42,43,44,45,46,47,48,49
 ,50,51,52,53,54,55,56,57
};
static const char _wkA2BkI2P[58] = {
        C1,D1,E1,F1,G1,H1
       ,C2,D2,E2,F2,G2,H2
       ,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkH2BkP2I[64] = {
   0, 1, 2, 3, 4, 5,-1,-1
 , 6, 7, 8, 9,10,11,-1,-1
 ,12,13,14,15,16,17,-1,-1
 ,18,19,20,21,22,23,24,25
 ,26,27,28,29,30,31,32,33
 ,34,35,36,37,38,39,40,41
 ,42,43,44,45,46,47,48,49
 ,50,51,52,53,54,55,56,57
};
static const char _wkH2BkI2P[58] = {
  A1,B1,C1,D1,E1,F1
 ,A2,B2,C2,D2,E2,F2
 ,A3,B3,C3,D3,E3,F3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkA3BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 ,-1,-1, 8, 9,10,11,12,13
 ,-1,-1,14,15,16,17,18,19
 ,-1,-1,20,21,22,23,24,25
 ,26,27,28,29,30,31,32,33
 ,34,35,36,37,38,39,40,41
 ,42,43,44,45,46,47,48,49
 ,50,51,52,53,54,55,56,57
};
static const char _wkA3BkI2P[58] = {
  A1,B1,C1,D1,E1,F1,G1,H1
       ,C2,D2,E2,F2,G2,H2
       ,C3,D3,E3,F3,G3,H3
       ,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkH3BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,-1,-1
 ,14,15,16,17,18,19,-1,-1
 ,20,21,22,23,24,25,-1,-1
 ,26,27,28,29,30,31,32,33
 ,34,35,36,37,38,39,40,41
 ,42,43,44,45,46,47,48,49
 ,50,51,52,53,54,55,56,57
};
static const char _wkH3BkI2P[58] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2
 ,A3,B3,C3,D3,E3,F3
 ,A4,B4,C4,D4,E4,F4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkA4BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,-1,-1,16,17,18,19,20,21
 ,-1,-1,22,23,24,25,26,27
 ,-1,-1,28,29,30,31,32,33
 ,34,35,36,37,38,39,40,41
 ,42,43,44,45,46,47,48,49
 ,50,51,52,53,54,55,56,57
};
static const char _wkA4BkI2P[58] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
       ,C3,D3,E3,F3,G3,H3
       ,C4,D4,E4,F4,G4,H4
       ,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkH4BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,-1,-1
 ,22,23,24,25,26,27,-1,-1
 ,28,29,30,31,32,33,-1,-1
 ,34,35,36,37,38,39,40,41
 ,42,43,44,45,46,47,48,49
 ,50,51,52,53,54,55,56,57
};
static const char _wkH4BkI2P[58] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3
 ,A4,B4,C4,D4,E4,F4
 ,A5,B5,C5,D5,E5,F5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkA5BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,-1,-1,24,25,26,27,28,29
 ,-1,-1,30,31,32,33,34,35
 ,-1,-1,36,37,38,39,40,41
 ,42,43,44,45,46,47,48,49
 ,50,51,52,53,54,55,56,57
};
static const char _wkA5BkI2P[58] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
       ,C4,D4,E4,F4,G4,H4
       ,C5,D5,E5,F5,G5,H5
       ,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkH5BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,24,25,26,27,28,29,-1,-1
 ,30,31,32,33,34,35,-1,-1
 ,36,37,38,39,40,41,-1,-1
 ,42,43,44,45,46,47,48,49
 ,50,51,52,53,54,55,56,57
};
static const char _wkH5BkI2P[58] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4
 ,A5,B5,C5,D5,E5,F5
 ,A6,B6,C6,D6,E6,F6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkA6BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,24,25,26,27,28,29,30,31
 ,-1,-1,32,33,34,35,36,37
 ,-1,-1,38,39,40,41,42,43
 ,-1,-1,44,45,46,47,48,49
 ,50,51,52,53,54,55,56,57
};
static const char _wkA6BkI2P[58] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
       ,C5,D5,E5,F5,G5,H5
       ,C6,D6,E6,F6,G6,H6
       ,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkH6BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,24,25,26,27,28,29,30,31
 ,32,33,34,35,36,37,-1,-1
 ,38,39,40,41,42,43,-1,-1
 ,44,45,46,47,48,49,-1,-1
 ,50,51,52,53,54,55,56,57
};
static const char _wkH6BkI2P[58] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5
 ,A6,B6,C6,D6,E6,F6
 ,A7,B7,C7,D7,E7,F7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkA7BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,24,25,26,27,28,29,30,31
 ,32,33,34,35,36,37,38,39
 ,-1,-1,40,41,42,43,44,45
 ,-1,-1,46,47,48,49,50,51
 ,-1,-1,52,53,54,55,56,57
};
static const char _wkA7BkI2P[58] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
       ,C6,D6,E6,F6,G6,H6
       ,C7,D7,E7,F7,G7,H7
       ,C8,D8,E8,F8,G8,H8
};
static const char _wkH7BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,24,25,26,27,28,29,30,31
 ,32,33,34,35,36,37,38,39
 ,40,41,42,43,44,45,-1,-1
 ,46,47,48,49,50,51,-1,-1
 ,52,53,54,55,56,57,-1,-1
};
static const char _wkH7BkI2P[58] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6
 ,A7,B7,C7,D7,E7,F7
 ,A8,B8,C8,D8,E8,F8
};
static const char _wkB8BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,24,25,26,27,28,29,30,31
 ,32,33,34,35,36,37,38,39
 ,40,41,42,43,44,45,46,47
 ,-1,-1,-1,48,49,50,51,52
 ,-1,-1,-1,53,54,55,56,57
};
static const char _wkB8BkI2P[58] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
          ,D7,E7,F7,G7,H7
          ,D8,E8,F8,G8,H8
};
static const char _wkC8BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,24,25,26,27,28,29,30,31
 ,32,33,34,35,36,37,38,39
 ,40,41,42,43,44,45,46,47
 ,48,-1,-1,-1,49,50,51,52
 ,53,-1,-1,-1,54,55,56,57
};
static const char _wkC8BkI2P[58] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7         ,E7,F7,G7,H7
 ,A8         ,E8,F8,G8,H8
};
static const char _wkD8BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,24,25,26,27,28,29,30,31
 ,32,33,34,35,36,37,38,39
 ,40,41,42,43,44,45,46,47
 ,48,49,-1,-1,-1,50,51,52
 ,53,54,-1,-1,-1,55,56,57
};
static const char _wkD8BkI2P[58] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7         ,F7,G7,H7
 ,A8,B8         ,F8,G8,H8
};
static const char _wkE8BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,24,25,26,27,28,29,30,31
 ,32,33,34,35,36,37,38,39
 ,40,41,42,43,44,45,46,47
 ,48,49,50,-1,-1,-1,51,52
 ,53,54,55,-1,-1,-1,56,57
};
static const char _wkE8BkI2P[58] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7         ,G7,H7
 ,A8,B8,C8         ,G8,H8
};
static const char _wkF8BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,24,25,26,27,28,29,30,31
 ,32,33,34,35,36,37,38,39
 ,40,41,42,43,44,45,46,47
 ,48,49,50,51,-1,-1,-1,52
 ,53,54,55,56,-1,-1,-1,57
};
static const char _wkF8BkI2P[58] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7         ,H7
 ,A8,B8,C8,D8         ,H8
};
static const char _wkG8BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,24,25,26,27,28,29,30,31
 ,32,33,34,35,36,37,38,39
 ,40,41,42,43,44,45,46,47
 ,48,49,50,51,52,-1,-1,-1
 ,53,54,55,56,57,-1,-1,-1
};
static const char _wkG8BkI2P[58] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7
 ,A8,B8,C8,D8,E8
};

// ----------------------------------- Corner fields -------------------------------------

static const char _wkA1BkP2I[64] = {
  -1,-1, 0, 1, 2, 3, 4, 5
 ,-1,-1, 6, 7, 8, 9,10,11
 ,12,13,14,15,16,17,18,19
 ,20,21,22,23,24,25,26,27
 ,28,29,30,31,32,33,34,35
 ,36,37,38,39,40,41,42,43
 ,44,45,46,47,48,49,50,51
 ,52,53,54,55,56,57,58,59
};
static const char _wkA1BkI2P[60] = {
        C1,D1,E1,F1,G1,H1
       ,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkH1BkP2I[64] = {
   0, 1, 2, 3, 4, 5,-1,-1
 , 6, 7, 8, 9,10,11,-1,-1
 ,12,13,14,15,16,17,18,19
 ,20,21,22,23,24,25,26,27
 ,28,29,30,31,32,33,34,35
 ,36,37,38,39,40,41,42,43
 ,44,45,46,47,48,49,50,51
 ,52,53,54,55,56,57,58,59
};
static const char _wkH1BkI2P[60] = {
  A1,B1,C1,D1,E1,F1
 ,A2,B2,C2,D2,E2,F2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7,G7,H7
 ,A8,B8,C8,D8,E8,F8,G8,H8
};
static const char _wkA8BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,24,25,26,27,28,29,30,31
 ,32,33,34,35,36,37,38,39
 ,40,41,42,43,44,45,46,47
 ,-1,-1,48,49,50,51,52,53
 ,-1,-1,54,55,56,57,58,59
};
static const char _wkA8BkI2P[60] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
       ,C7,D7,E7,F7,G7,H7
       ,C8,D8,E8,F8,G8,H8
};
static const char _wkH8BkP2I[64] = {
   0, 1, 2, 3, 4, 5, 6, 7
 , 8, 9,10,11,12,13,14,15
 ,16,17,18,19,20,21,22,23
 ,24,25,26,27,28,29,30,31
 ,32,33,34,35,36,37,38,39
 ,40,41,42,43,44,45,46,47
 ,48,49,50,51,52,53,-1,-1
 ,54,55,56,57,58,59,-1,-1
};
static const char _wkH8BkI2P[60] = {
  A1,B1,C1,D1,E1,F1,G1,H1
 ,A2,B2,C2,D2,E2,F2,G2,H2
 ,A3,B3,C3,D3,E3,F3,G3,H3
 ,A4,B4,C4,D4,E4,F4,G4,H4
 ,A5,B5,C5,D5,E5,F5,G5,H5
 ,A6,B6,C6,D6,E6,F6,G6,H6
 ,A7,B7,C7,D7,E7,F7
 ,A8,B8,C8,D8,E8,F8
};


static const char wkPosToIndex[64] = {
  60,36,37,38,39,40,41,61
 ,42, 0, 1, 2, 3, 4, 5,43
 ,44, 6, 7, 8, 9,10,11,45
 ,46,12,13,14,15,16,17,47
 ,48,18,19,20,21,22,23,49
 ,50,24,25,26,27,28,29,51
 ,52,30,31,32,33,34,35,53
 ,62,54,55,56,57,58,59,63
};

static const char wkIndexToPos[64] = {
     B2,C2,D2,E2,F2,G2
    ,B3,C3,D3,E3,F3,G3
    ,B4,C4,D4,E4,F4,G4
    ,B5,C5,D5,E5,F5,G5
    ,B6,C6,D6,E6,F6,G6
    ,B7,C7,D7,E7,F7,G7

    ,B1,C1,D1,E1,F1,G1
 ,A2                  ,H2
 ,A3                  ,H3
 ,A4                  ,H4
 ,A5                  ,H5
 ,A6                  ,H6
 ,A7                  ,H7
    ,B8,C8,D8,E8,F8,G8

 ,A1                  ,H1
 ,A8                  ,H8
};

static const char *kkWithPawnPosToIndex[64] = {
             _wkB2BkP2I,_wkC2BkP2I,_wkD2BkP2I,_wkE2BkP2I,_wkF2BkP2I,_wkG2BkP2I             // Innerfields : 36 fields with 55 elements each
            ,_wkB3BkP2I,_wkC3BkP2I,_wkD3BkP2I,_wkE3BkP2I,_wkF3BkP2I,_wkG3BkP2I
            ,_wkB4BkP2I,_wkC4BkP2I,_wkD4BkP2I,_wkE4BkP2I,_wkF4BkP2I,_wkG4BkP2I
            ,_wkB5BkP2I,_wkC5BkP2I,_wkD5BkP2I,_wkE5BkP2I,_wkF5BkP2I,_wkG5BkP2I
            ,_wkB6BkP2I,_wkC6BkP2I,_wkD6BkP2I,_wkE6BkP2I,_wkF6BkP2I,_wkG6BkP2I
            ,_wkB7BkP2I,_wkC7BkP2I,_wkD7BkP2I,_wkE7BkP2I,_wkF7BkP2I,_wkG7BkP2I

            ,_wkB1BkP2I,_wkC1BkP2I,_wkD1BkP2I,_wkE1BkP2I,_wkF1BkP2I,_wkG1BkP2I             // Edgefields  : 24 fields with 58 elements each
 ,_wkA2BkP2I                                                                  ,_wkH2BkP2I
 ,_wkA3BkP2I                                                                  ,_wkH3BkP2I
 ,_wkA4BkP2I                                                                  ,_wkH4BkP2I
 ,_wkA5BkP2I                                                                  ,_wkH5BkP2I
 ,_wkA6BkP2I                                                                  ,_wkH6BkP2I
 ,_wkA7BkP2I                                                                  ,_wkH7BkP2I
            ,_wkB8BkP2I,_wkC8BkP2I,_wkD8BkP2I,_wkE8BkP2I,_wkF8BkP2I,_wkG8BkP2I

 ,_wkA1BkP2I                                                                  ,_wkH1BkP2I  // Cornerfields: 4 fields with 60 elements each
 ,_wkA8BkP2I                                                                  ,_wkH8BkP2I
};

static const char *kkWithPawnIndexToPos[64] = {
             _wkB2BkI2P,_wkC2BkI2P,_wkD2BkI2P,_wkE2BkI2P,_wkF2BkI2P,_wkG2BkI2P             // Innerfields : 36 fields with 55 elements each
            ,_wkB3BkI2P,_wkC3BkI2P,_wkD3BkI2P,_wkE3BkI2P,_wkF3BkI2P,_wkG3BkI2P
            ,_wkB4BkI2P,_wkC4BkI2P,_wkD4BkI2P,_wkE4BkI2P,_wkF4BkI2P,_wkG4BkI2P
            ,_wkB5BkI2P,_wkC5BkI2P,_wkD5BkI2P,_wkE5BkI2P,_wkF5BkI2P,_wkG5BkI2P
            ,_wkB6BkI2P,_wkC6BkI2P,_wkD6BkI2P,_wkE6BkI2P,_wkF6BkI2P,_wkG6BkI2P
            ,_wkB7BkI2P,_wkC7BkI2P,_wkD7BkI2P,_wkE7BkI2P,_wkF7BkI2P,_wkG7BkI2P

            ,_wkB1BkI2P,_wkC1BkI2P,_wkD1BkI2P,_wkE1BkI2P,_wkF1BkI2P,_wkG1BkI2P             // Edgefields  : 24 fields with 58 elements each
 ,_wkA2BkI2P                                                                  ,_wkH2BkI2P
 ,_wkA3BkI2P                                                                  ,_wkH3BkI2P
 ,_wkA4BkI2P                                                                  ,_wkH4BkI2P
 ,_wkA5BkI2P                                                                  ,_wkH5BkI2P
 ,_wkA6BkI2P                                                                  ,_wkH6BkI2P
 ,_wkA7BkI2P                                                                  ,_wkH7BkI2P
            ,_wkB8BkI2P,_wkC8BkI2P,_wkD8BkI2P,_wkE8BkI2P,_wkF8BkI2P,_wkG8BkI2P

 ,_wkA1BkI2P                                                                  ,_wkH1BkI2P  // Cornerfields: 4 fields with 60 elements each
 ,_wkA8BkI2P                                                                  ,_wkH8BkI2P
};

// s_pawnPosToIndex[s_pawnIndexToPos[index]] == index, index = [0..47]
const char EndGameKeyDefinition::s_pawnPosToIndex[64] = {
   -1,-1,-1,-1,-1,-1,-1,-1
  , 0, 1, 2, 3,24,25,26,27
  , 4, 5, 6, 7,28,29,30,31
  , 8, 9,10,11,32,33,34,35
  ,12,13,14,15,36,37,38,39
  ,16,17,18,19,40,41,42,43
  ,20,21,22,23,44,45,46,47
  ,-1,-1,-1,-1,-1,-1,-1,-1
};

const char EndGameKeyDefinition::s_pawnIndexToPos[PAWN_POSCOUNT] = {
   A2,B2,C2,D2 // Queenside
  ,A3,B3,C3,D3
  ,A4,B4,C4,D4
  ,A5,B5,C5,D5
  ,A6,B6,C6,D6
  ,A7,B7,C7,D7
  ,E2,F2,G2,H2 // Kingside
  ,E3,F3,G3,H3
  ,E4,F4,G4,H4
  ,E5,F5,G5,H5
  ,E6,F6,G6,H6
  ,E7,F7,G7,H7
};

#define MAXINDEX_WK_INNERFIELD     (36*55)
#define MAXINDEX_WK_EDGEFIELD      (24*58)
#define MAXINDEX_WK_CORNERFIELD    ( 4*60)

#define START_RANGE_WK_INNERFIELD  0
#define END_RANGE_WK_INNERFIELD    (START_RANGE_WK_INNERFIELD  + MAXINDEX_WK_INNERFIELD )
#define START_RANGE_WK_EDGEFIELD   END_RANGE_WK_INNERFIELD
#define END_RANGE_WK_EDGEFIELD     (START_RANGE_WK_EDGEFIELD   + MAXINDEX_WK_EDGEFIELD  )
#define START_RANGE_WK_CORNERFIELD END_RANGE_WK_EDGEFIELD
#define END_RANGE_WK_CORNERFIELD   (START_RANGE_WK_CORNERFIELD + MAXINDEX_WK_CORNERFIELD)

// Calculates an index in the range [0..3611] for the 2 kings which can be decoded with decodeKKWithPawn.
EndGamePosIndex EndGameKeyDefinition::encodeKKWithPawn(EndGameKey key) { // static
  const EndGamePosIndex wki = wkPosToIndex[key.getWhiteKingPosition()];
  if(wki < 36) {
    return (wki -  0) * 55 + kkWithPawnPosToIndex[wki][key.getBlackKingPosition()] + START_RANGE_WK_INNERFIELD;
  } else if(wki < 60) {
    return (wki - 36) * 58 + kkWithPawnPosToIndex[wki][key.getBlackKingPosition()] + START_RANGE_WK_EDGEFIELD;
  } else {
    return (wki - 60) * 60 + kkWithPawnPosToIndex[wki][key.getBlackKingPosition()] + START_RANGE_WK_CORNERFIELD;
  }
}

void EndGameKeyDefinition::decodeKKWithPawn(EndGameKey &key, EndGamePosIndex index) { // static
  EndGamePosIndex wki;
  if(index < START_RANGE_WK_EDGEFIELD) {
    index -= START_RANGE_WK_INNERFIELD;
    key.setWhiteKingPosition(wkIndexToPos[wki = index / 55]);
    key.setBlackKingPosition(kkWithPawnIndexToPos[wki][index % 55]);
  } else if(index < START_RANGE_WK_CORNERFIELD) {
    index -= START_RANGE_WK_EDGEFIELD;
    key.setWhiteKingPosition(wkIndexToPos[wki = index / 58 + 36]);
    key.setBlackKingPosition(kkWithPawnIndexToPos[wki][index % 58]);
  } else {
    index -= START_RANGE_WK_CORNERFIELD;
    key.setWhiteKingPosition(wkIndexToPos[wki = index / 60 + 60]);
    key.setBlackKingPosition(kkWithPawnIndexToPos[wki][index % 60]);
  }
}




static const char leftWKPosToIndex[64] = {
  30,18,19,20,-1,-1,-1,-1
 ,21, 0, 1, 2,-1,-1,-1,-1
 ,22, 3, 4, 5,-1,-1,-1,-1
 ,23, 6, 7, 8,-1,-1,-1,-1
 ,24, 9,10,11,-1,-1,-1,-1
 ,25,12,13,14,-1,-1,-1,-1
 ,26,15,16,17,-1,-1,-1,-1
 ,31,27,28,29,-1,-1,-1,-1
};

static const char leftWKIndexToPos[32] = {
  B2,C2,D2
 ,B3,C3,D3
 ,B4,C4,D4
 ,B5,C5,D5
 ,B6,C6,D6
 ,B7,C7,D7

 ,B1,C1,D1
 ,A2
 ,A3
 ,A4
 ,A5
 ,A6
 ,A7
 ,B8,C8,D8

 ,A1,A8
};

static const char *leftWKKWithPawnIndexToPos[32] = {
  _wkB2BkI2P,_wkC2BkI2P,_wkD2BkI2P  // Innerfields : 18 fields with 55 elements each
 ,_wkB3BkI2P,_wkC3BkI2P,_wkD3BkI2P
 ,_wkB4BkI2P,_wkC4BkI2P,_wkD4BkI2P
 ,_wkB5BkI2P,_wkC5BkI2P,_wkD5BkI2P
 ,_wkB6BkI2P,_wkC6BkI2P,_wkD6BkI2P
 ,_wkB7BkI2P,_wkC7BkI2P,_wkD7BkI2P

 ,_wkB1BkI2P,_wkC1BkI2P,_wkD1BkI2P  // Edgefields  : 12 fields with 58 elements each
 ,_wkA2BkI2P
 ,_wkA3BkI2P
 ,_wkA4BkI2P
 ,_wkA5BkI2P
 ,_wkA6BkI2P
 ,_wkA7BkI2P
 ,_wkB8BkI2P,_wkC8BkI2P,_wkD8BkI2P

 ,_wkA1BkI2P,_wkA8BkI2P             // Cornerfields: 2 fields with 60 elements each
};

static const char *leftWKKWithPawnPosToIndex[32] = {
  _wkB2BkP2I,_wkC2BkP2I,_wkD2BkP2I  // Innerfields : 18 fields with 55 elements each
 ,_wkB3BkP2I,_wkC3BkP2I,_wkD3BkP2I
 ,_wkB4BkP2I,_wkC4BkP2I,_wkD4BkP2I
 ,_wkB5BkP2I,_wkC5BkP2I,_wkD5BkP2I
 ,_wkB6BkP2I,_wkC6BkP2I,_wkD6BkP2I
 ,_wkB7BkP2I,_wkC7BkP2I,_wkD7BkP2I

 ,_wkB1BkP2I,_wkC1BkP2I,_wkD1BkP2I  // Edgefields  : 12 fields with 58 elements each
 ,_wkA2BkP2I
 ,_wkA3BkP2I
 ,_wkA4BkP2I
 ,_wkA5BkP2I
 ,_wkA6BkP2I
 ,_wkA7BkP2I
 ,_wkB8BkP2I,_wkC8BkP2I,_wkD8BkP2I

 ,_wkA1BkP2I,_wkA8BkP2I             // Cornerfields: 2 fields with 60 elements each
};

#define MAXINDEX_LEFTWK_INNERFIELD     (18*55)
#define MAXINDEX_LEFTWK_EDGEFIELD      (12*58)
#define MAXINDEX_LEFTWK_CORNERFIELD    ( 2*60)

#define START_RANGE_LEFTWK_INNERFIELD  0
#define END_RANGE_LEFTWK_INNERFIELD    (START_RANGE_LEFTWK_INNERFIELD  + MAXINDEX_LEFTWK_INNERFIELD )
#define START_RANGE_LEFTWK_EDGEFIELD   END_RANGE_LEFTWK_INNERFIELD
#define END_RANGE_LEFTWK_EDGEFIELD     (START_RANGE_LEFTWK_EDGEFIELD   + MAXINDEX_LEFTWK_EDGEFIELD  )
#define START_RANGE_LEFTWK_CORNERFIELD END_RANGE_LEFTWK_EDGEFIELD
#define END_RANGE_LEFTWK_CORNERFIELD   (START_RANGE_LEFTWK_CORNERFIELD + MAXINDEX_LEFTWK_CORNERFIELD)

EndGamePosIndex EndGameKeyDefinition::encodeLeftWKKWithPawn(EndGameKey key) { // static
  const EndGamePosIndex wki = leftWKPosToIndex[key.getWhiteKingPosition()];
  if(wki < 18) {
    return (wki -  0) * 55 + leftWKKWithPawnPosToIndex[wki][key.getBlackKingPosition()] + START_RANGE_LEFTWK_INNERFIELD;
  } else if(wki < 30) {
    return (wki - 18) * 58 + leftWKKWithPawnPosToIndex[wki][key.getBlackKingPosition()] + START_RANGE_LEFTWK_EDGEFIELD;
  } else {
    return (wki - 30) * 60 + leftWKKWithPawnPosToIndex[wki][key.getBlackKingPosition()] + START_RANGE_LEFTWK_CORNERFIELD;
  }
}

void EndGameKeyDefinition::decodeLeftWKKWithPawn(EndGameKey &key, EndGamePosIndex index) { // static
  EndGamePosIndex wki;
  if(index < START_RANGE_LEFTWK_EDGEFIELD) {
    index -= START_RANGE_LEFTWK_INNERFIELD;
    key.setWhiteKingPosition(leftWKIndexToPos[wki = index / 55]);
    key.setBlackKingPosition(leftWKKWithPawnIndexToPos[wki][index % 55]);
  } else if(index < START_RANGE_LEFTWK_CORNERFIELD) {
    index -= START_RANGE_LEFTWK_EDGEFIELD;
    key.setWhiteKingPosition(leftWKIndexToPos[wki = index / 58 + 18]);
    key.setBlackKingPosition(leftWKKWithPawnIndexToPos[wki][index % 58]);
  } else {
    index -= START_RANGE_LEFTWK_CORNERFIELD;
    key.setWhiteKingPosition(leftWKIndexToPos[wki = index / 60 + 30]);
    key.setBlackKingPosition(leftWKKWithPawnIndexToPos[wki][index % 60]);
  }
}


//#define _TEST_MODULE

#ifdef _TEST_MODULE

static EndGamePosIndex checkKKWithPawnKey(const EndGameKey &key) {
  EndGamePosIndex index = EndGameKeyDefinition::encodeKKWithPawn(key);
  EndGameKey key1;
  EndGameKeyDefinition::decodeKKWithPawn(key1, index);
  if(key1 != key) {
    _tprintf(_T("(wkPos,bkPos)=(%s,%s) -> %lu -> (%s,%s)\n"), getFieldName(key.getWhiteKingPosition())
                                                      , getFieldName(key.getBlackKingPosition())
                                                      , index
                                                      , getFieldName(key1.getWhiteKingPosition())
                                                      , getFieldName(key1.getBlackKingPosition()));
    EndGamePosIndex index2 = EndGameKeyDefinition::encodeKKWithPawn(key);
    EndGameKey key2;
    EndGameKeyDefinition::decodeKKWithPawn(key2, index2);
  }
  return index;
}

void testKKWithPawn() {
  UINT64          keyCount = 0;
  EndGamePosIndex minIndex = -1;
  EndGamePosIndex maxIndex = 0;
  for(int wkPos = 0; wkPos < 64; wkPos++) {
    for(int bkPos = 0; bkPos < 64; bkPos++) {
      if(!KINGSADJACENT(wkPos,bkPos)) {
        EndGameKey key;
        key.setWhiteKingPosition(wkPos);
        key.setBlackKingPosition(bkPos);
        const UINT index = checkKKWithPawnKey(key);
        minIndex = min(minIndex, index);
        maxIndex = max(maxIndex, index);
        keyCount++;
      }
    }
  }
  _tprintf(_T("keyCount:%llu\n"), keyCount);
  _tprintf(_T("minIndex:%llu\n"), minIndex);
  _tprintf(_T("maxIndex:%llu\n"), maxIndex);
  DUMP_MACRO(END_RANGE_WK_CORNERFIELD);
}

static EndGamePosIndex checkLeftWKKWithPawnKey(const EndGameKey &key) {
  EndGamePosIndex index = EndGameKeyDefinition::encodeLeftWKKWithPawn(key);
  EndGameKey      key1;
  EndGameKeyDefinition::decodeLeftWKKWithPawn(key1, index);
  if(key1 != key) {
    _tprintf(_T("(wkPos,bkPos)=(%s,%s) -> %llu -> (%s,%s)\n"), getFieldName(key.getWhiteKingPosition())
                                                      , getFieldName(key.getBlackKingPosition())
                                                      , index
                                                      , getFieldName(key1.getWhiteKingPosition())
                                                      , getFieldName(key1.getBlackKingPosition()));
    EndGamePosIndex index2 = EndGameKeyDefinition::encodeLeftWKKWithPawn(key);
    EndGameKey      key2;
    EndGameKeyDefinition::decodeLeftWKKWithPawn(key2, index2);
  }
  return index;
}

void testLeftWKKWithPawn() {
  UINT64          keyCount = 0;
  EndGamePosIndex minIndex = -1;
  EndGamePosIndex maxIndex = 0;
  for(int wkPos = 0; wkPos < 64; wkPos++) {
    if(IS_KINGSIDE(wkPos)) {
      continue;
    }
    for(int bkPos = 0; bkPos < 64; bkPos++) {
      if(!KINGSADJACENT(wkPos,bkPos)) {
        EndGameKey key;
        key.setWhiteKingPosition(wkPos);
        key.setBlackKingPosition(bkPos);
        const EndGamePosIndex index = checkLeftWKKWithPawnKey(key);
        minIndex = min(minIndex, index);
        maxIndex = max(maxIndex, index);
        keyCount++;
      }
    }
  }
  _tprintf(_T("keyCount:%llu\n"), keyCount);
  _tprintf(_T("minIndex:%llu\n"), minIndex);
  _tprintf(_T("maxIndex:%llu\n"), maxIndex);
  DUMP_MACRO(END_RANGE_LEFTWK_CORNERFIELD);
}

#endif
