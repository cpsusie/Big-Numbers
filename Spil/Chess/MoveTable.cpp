#include "stdafx.h"
#include "Game.h"
#include "FieldNames.h"

static const int kingMoves_A1_0[2] = { 1,B1 };
static const int kingMoves_A1_1[2] = { 1,A2 };
static const int kingMoves_A1_2[2] = { 1,B2 };
#ifndef _DEBUG
static const Direction kingMoveDirections_A1[3] = {
  Direction(MD_RIGHT    ,kingMoves_A1_0)
 ,Direction(MD_UP       ,kingMoves_A1_1)
 ,Direction(MD_UPDIAG1  ,kingMoves_A1_2)
};
#else
static const Direction kingMoveDirections_A1[3] = {
  Direction(MD_RIGHT    ,kingMoves_A1_0,_T("king MD_RIGHT from A1:B1"))
 ,Direction(MD_UP       ,kingMoves_A1_1,_T("king MD_UP from A1:A2"))
 ,Direction(MD_UPDIAG1  ,kingMoves_A1_2,_T("king MD_UPDIAG1 from A1:B2"))
};
#endif
static const DirectionArray kingMoves_A1(3,kingMoveDirections_A1);

static const int kingMoves_B1_0[2] = { 1,A1 };
static const int kingMoves_B1_1[2] = { 1,C1 };
static const int kingMoves_B1_2[2] = { 1,A2 };
static const int kingMoves_B1_3[2] = { 1,B2 };
static const int kingMoves_B1_4[2] = { 1,C2 };
#ifndef _DEBUG
static const Direction kingMoveDirections_B1[5] = {
  Direction(MD_LEFT     ,kingMoves_B1_0)
 ,Direction(MD_RIGHT    ,kingMoves_B1_1)
 ,Direction(MD_UPDIAG2  ,kingMoves_B1_2)
 ,Direction(MD_UP       ,kingMoves_B1_3)
 ,Direction(MD_UPDIAG1  ,kingMoves_B1_4)
};
#else
static const Direction kingMoveDirections_B1[5] = {
  Direction(MD_LEFT     ,kingMoves_B1_0,_T("king MD_LEFT from B1:A1"))
 ,Direction(MD_RIGHT    ,kingMoves_B1_1,_T("king MD_RIGHT from B1:C1"))
 ,Direction(MD_UPDIAG2  ,kingMoves_B1_2,_T("king MD_UPDIAG2 from B1:A2"))
 ,Direction(MD_UP       ,kingMoves_B1_3,_T("king MD_UP from B1:B2"))
 ,Direction(MD_UPDIAG1  ,kingMoves_B1_4,_T("king MD_UPDIAG1 from B1:C2"))
};
#endif
static const DirectionArray kingMoves_B1(5,kingMoveDirections_B1);

static const int kingMoves_C1_0[2] = { 1,B1 };
static const int kingMoves_C1_1[2] = { 1,D1 };
static const int kingMoves_C1_2[2] = { 1,B2 };
static const int kingMoves_C1_3[2] = { 1,C2 };
static const int kingMoves_C1_4[2] = { 1,D2 };
#ifndef _DEBUG
static const Direction kingMoveDirections_C1[5] = {
  Direction(MD_LEFT     ,kingMoves_C1_0)
 ,Direction(MD_RIGHT    ,kingMoves_C1_1)
 ,Direction(MD_UPDIAG2  ,kingMoves_C1_2)
 ,Direction(MD_UP       ,kingMoves_C1_3)
 ,Direction(MD_UPDIAG1  ,kingMoves_C1_4)
};
#else
static const Direction kingMoveDirections_C1[5] = {
  Direction(MD_LEFT     ,kingMoves_C1_0,_T("king MD_LEFT from C1:B1"))
 ,Direction(MD_RIGHT    ,kingMoves_C1_1,_T("king MD_RIGHT from C1:D1"))
 ,Direction(MD_UPDIAG2  ,kingMoves_C1_2,_T("king MD_UPDIAG2 from C1:B2"))
 ,Direction(MD_UP       ,kingMoves_C1_3,_T("king MD_UP from C1:C2"))
 ,Direction(MD_UPDIAG1  ,kingMoves_C1_4,_T("king MD_UPDIAG1 from C1:D2"))
};
#endif
static const DirectionArray kingMoves_C1(5,kingMoveDirections_C1);

static const int kingMoves_D1_0[2] = { 1,C1 };
static const int kingMoves_D1_1[2] = { 1,E1 };
static const int kingMoves_D1_2[2] = { 1,C2 };
static const int kingMoves_D1_3[2] = { 1,D2 };
static const int kingMoves_D1_4[2] = { 1,E2 };
#ifndef _DEBUG
static const Direction kingMoveDirections_D1[5] = {
  Direction(MD_LEFT     ,kingMoves_D1_0)
 ,Direction(MD_RIGHT    ,kingMoves_D1_1)
 ,Direction(MD_UPDIAG2  ,kingMoves_D1_2)
 ,Direction(MD_UP       ,kingMoves_D1_3)
 ,Direction(MD_UPDIAG1  ,kingMoves_D1_4)
};
#else
static const Direction kingMoveDirections_D1[5] = {
  Direction(MD_LEFT     ,kingMoves_D1_0,_T("king MD_LEFT from D1:C1"))
 ,Direction(MD_RIGHT    ,kingMoves_D1_1,_T("king MD_RIGHT from D1:E1"))
 ,Direction(MD_UPDIAG2  ,kingMoves_D1_2,_T("king MD_UPDIAG2 from D1:C2"))
 ,Direction(MD_UP       ,kingMoves_D1_3,_T("king MD_UP from D1:D2"))
 ,Direction(MD_UPDIAG1  ,kingMoves_D1_4,_T("king MD_UPDIAG1 from D1:E2"))
};
#endif
static const DirectionArray kingMoves_D1(5,kingMoveDirections_D1);

static const int kingMoves_E1_0[2] = { 1,D1 };
static const int kingMoves_E1_1[2] = { 1,F1 };
static const int kingMoves_E1_2[2] = { 1,D2 };
static const int kingMoves_E1_3[2] = { 1,E2 };
static const int kingMoves_E1_4[2] = { 1,F2 };
#ifndef _DEBUG
static const Direction kingMoveDirections_E1[5] = {
  Direction(MD_LEFT     ,kingMoves_E1_0)
 ,Direction(MD_RIGHT    ,kingMoves_E1_1)
 ,Direction(MD_UPDIAG2  ,kingMoves_E1_2)
 ,Direction(MD_UP       ,kingMoves_E1_3)
 ,Direction(MD_UPDIAG1  ,kingMoves_E1_4)
};
#else
static const Direction kingMoveDirections_E1[5] = {
  Direction(MD_LEFT     ,kingMoves_E1_0,_T("king MD_LEFT from E1:D1"))
 ,Direction(MD_RIGHT    ,kingMoves_E1_1,_T("king MD_RIGHT from E1:F1"))
 ,Direction(MD_UPDIAG2  ,kingMoves_E1_2,_T("king MD_UPDIAG2 from E1:D2"))
 ,Direction(MD_UP       ,kingMoves_E1_3,_T("king MD_UP from E1:E2"))
 ,Direction(MD_UPDIAG1  ,kingMoves_E1_4,_T("king MD_UPDIAG1 from E1:F2"))
};
#endif
static const DirectionArray kingMoves_E1(5,kingMoveDirections_E1);

static const int kingMoves_F1_0[2] = { 1,E1 };
static const int kingMoves_F1_1[2] = { 1,G1 };
static const int kingMoves_F1_2[2] = { 1,E2 };
static const int kingMoves_F1_3[2] = { 1,F2 };
static const int kingMoves_F1_4[2] = { 1,G2 };
#ifndef _DEBUG
static const Direction kingMoveDirections_F1[5] = {
  Direction(MD_LEFT     ,kingMoves_F1_0)
 ,Direction(MD_RIGHT    ,kingMoves_F1_1)
 ,Direction(MD_UPDIAG2  ,kingMoves_F1_2)
 ,Direction(MD_UP       ,kingMoves_F1_3)
 ,Direction(MD_UPDIAG1  ,kingMoves_F1_4)
};
#else
static const Direction kingMoveDirections_F1[5] = {
  Direction(MD_LEFT     ,kingMoves_F1_0,_T("king MD_LEFT from F1:E1"))
 ,Direction(MD_RIGHT    ,kingMoves_F1_1,_T("king MD_RIGHT from F1:G1"))
 ,Direction(MD_UPDIAG2  ,kingMoves_F1_2,_T("king MD_UPDIAG2 from F1:E2"))
 ,Direction(MD_UP       ,kingMoves_F1_3,_T("king MD_UP from F1:F2"))
 ,Direction(MD_UPDIAG1  ,kingMoves_F1_4,_T("king MD_UPDIAG1 from F1:G2"))
};
#endif
static const DirectionArray kingMoves_F1(5,kingMoveDirections_F1);

static const int kingMoves_G1_0[2] = { 1,F1 };
static const int kingMoves_G1_1[2] = { 1,H1 };
static const int kingMoves_G1_2[2] = { 1,F2 };
static const int kingMoves_G1_3[2] = { 1,G2 };
static const int kingMoves_G1_4[2] = { 1,H2 };
#ifndef _DEBUG
static const Direction kingMoveDirections_G1[5] = {
  Direction(MD_LEFT     ,kingMoves_G1_0)
 ,Direction(MD_RIGHT    ,kingMoves_G1_1)
 ,Direction(MD_UPDIAG2  ,kingMoves_G1_2)
 ,Direction(MD_UP       ,kingMoves_G1_3)
 ,Direction(MD_UPDIAG1  ,kingMoves_G1_4)
};
#else
static const Direction kingMoveDirections_G1[5] = {
  Direction(MD_LEFT     ,kingMoves_G1_0,_T("king MD_LEFT from G1:F1"))
 ,Direction(MD_RIGHT    ,kingMoves_G1_1,_T("king MD_RIGHT from G1:H1"))
 ,Direction(MD_UPDIAG2  ,kingMoves_G1_2,_T("king MD_UPDIAG2 from G1:F2"))
 ,Direction(MD_UP       ,kingMoves_G1_3,_T("king MD_UP from G1:G2"))
 ,Direction(MD_UPDIAG1  ,kingMoves_G1_4,_T("king MD_UPDIAG1 from G1:H2"))
};
#endif
static const DirectionArray kingMoves_G1(5,kingMoveDirections_G1);

static const int kingMoves_H1_0[2] = { 1,G1 };
static const int kingMoves_H1_1[2] = { 1,G2 };
static const int kingMoves_H1_2[2] = { 1,H2 };
#ifndef _DEBUG
static const Direction kingMoveDirections_H1[3] = {
  Direction(MD_LEFT     ,kingMoves_H1_0)
 ,Direction(MD_UPDIAG2  ,kingMoves_H1_1)
 ,Direction(MD_UP       ,kingMoves_H1_2)
};
#else
static const Direction kingMoveDirections_H1[3] = {
  Direction(MD_LEFT     ,kingMoves_H1_0,_T("king MD_LEFT from H1:G1"))
 ,Direction(MD_UPDIAG2  ,kingMoves_H1_1,_T("king MD_UPDIAG2 from H1:G2"))
 ,Direction(MD_UP       ,kingMoves_H1_2,_T("king MD_UP from H1:H2"))
};
#endif
static const DirectionArray kingMoves_H1(3,kingMoveDirections_H1);

static const int kingMoves_A2_0[2] = { 1,A1 };
static const int kingMoves_A2_1[2] = { 1,B1 };
static const int kingMoves_A2_2[2] = { 1,B2 };
static const int kingMoves_A2_3[2] = { 1,A3 };
static const int kingMoves_A2_4[2] = { 1,B3 };
#ifndef _DEBUG
static const Direction kingMoveDirections_A2[5] = {
  Direction(MD_DOWN     ,kingMoves_A2_0)
 ,Direction(MD_DOWNDIAG2,kingMoves_A2_1)
 ,Direction(MD_RIGHT    ,kingMoves_A2_2)
 ,Direction(MD_UP       ,kingMoves_A2_3)
 ,Direction(MD_UPDIAG1  ,kingMoves_A2_4)
};
#else
static const Direction kingMoveDirections_A2[5] = {
  Direction(MD_DOWN     ,kingMoves_A2_0,_T("king MD_DOWN from A2:A1"))
 ,Direction(MD_DOWNDIAG2,kingMoves_A2_1,_T("king MD_DOWNDIAG2 from A2:B1"))
 ,Direction(MD_RIGHT    ,kingMoves_A2_2,_T("king MD_RIGHT from A2:B2"))
 ,Direction(MD_UP       ,kingMoves_A2_3,_T("king MD_UP from A2:A3"))
 ,Direction(MD_UPDIAG1  ,kingMoves_A2_4,_T("king MD_UPDIAG1 from A2:B3"))
};
#endif
static const DirectionArray kingMoves_A2(5,kingMoveDirections_A2);

static const int kingMoves_B2_0[2] = { 1,A1 };
static const int kingMoves_B2_1[2] = { 1,B1 };
static const int kingMoves_B2_2[2] = { 1,C1 };
static const int kingMoves_B2_3[2] = { 1,A2 };
static const int kingMoves_B2_4[2] = { 1,C2 };
static const int kingMoves_B2_5[2] = { 1,A3 };
static const int kingMoves_B2_6[2] = { 1,B3 };
static const int kingMoves_B2_7[2] = { 1,C3 };
#ifndef _DEBUG
static const Direction kingMoveDirections_B2[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_B2_0)
 ,Direction(MD_DOWN     ,kingMoves_B2_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_B2_2)
 ,Direction(MD_LEFT     ,kingMoves_B2_3)
 ,Direction(MD_RIGHT    ,kingMoves_B2_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_B2_5)
 ,Direction(MD_UP       ,kingMoves_B2_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_B2_7)
};
#else
static const Direction kingMoveDirections_B2[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_B2_0,_T("king MD_DOWNDIAG1 from B2:A1"))
 ,Direction(MD_DOWN     ,kingMoves_B2_1,_T("king MD_DOWN from B2:B1"))
 ,Direction(MD_DOWNDIAG2,kingMoves_B2_2,_T("king MD_DOWNDIAG2 from B2:C1"))
 ,Direction(MD_LEFT     ,kingMoves_B2_3,_T("king MD_LEFT from B2:A2"))
 ,Direction(MD_RIGHT    ,kingMoves_B2_4,_T("king MD_RIGHT from B2:C2"))
 ,Direction(MD_UPDIAG2  ,kingMoves_B2_5,_T("king MD_UPDIAG2 from B2:A3"))
 ,Direction(MD_UP       ,kingMoves_B2_6,_T("king MD_UP from B2:B3"))
 ,Direction(MD_UPDIAG1  ,kingMoves_B2_7,_T("king MD_UPDIAG1 from B2:C3"))
};
#endif
static const DirectionArray kingMoves_B2(8,kingMoveDirections_B2);

static const int kingMoves_C2_0[2] = { 1,B1 };
static const int kingMoves_C2_1[2] = { 1,C1 };
static const int kingMoves_C2_2[2] = { 1,D1 };
static const int kingMoves_C2_3[2] = { 1,B2 };
static const int kingMoves_C2_4[2] = { 1,D2 };
static const int kingMoves_C2_5[2] = { 1,B3 };
static const int kingMoves_C2_6[2] = { 1,C3 };
static const int kingMoves_C2_7[2] = { 1,D3 };
#ifndef _DEBUG
static const Direction kingMoveDirections_C2[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_C2_0)
 ,Direction(MD_DOWN     ,kingMoves_C2_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_C2_2)
 ,Direction(MD_LEFT     ,kingMoves_C2_3)
 ,Direction(MD_RIGHT    ,kingMoves_C2_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_C2_5)
 ,Direction(MD_UP       ,kingMoves_C2_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_C2_7)
};
#else
static const Direction kingMoveDirections_C2[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_C2_0,_T("king MD_DOWNDIAG1 from C2:B1"))
 ,Direction(MD_DOWN     ,kingMoves_C2_1,_T("king MD_DOWN from C2:C1"))
 ,Direction(MD_DOWNDIAG2,kingMoves_C2_2,_T("king MD_DOWNDIAG2 from C2:D1"))
 ,Direction(MD_LEFT     ,kingMoves_C2_3,_T("king MD_LEFT from C2:B2"))
 ,Direction(MD_RIGHT    ,kingMoves_C2_4,_T("king MD_RIGHT from C2:D2"))
 ,Direction(MD_UPDIAG2  ,kingMoves_C2_5,_T("king MD_UPDIAG2 from C2:B3"))
 ,Direction(MD_UP       ,kingMoves_C2_6,_T("king MD_UP from C2:C3"))
 ,Direction(MD_UPDIAG1  ,kingMoves_C2_7,_T("king MD_UPDIAG1 from C2:D3"))
};
#endif
static const DirectionArray kingMoves_C2(8,kingMoveDirections_C2);

static const int kingMoves_D2_0[2] = { 1,C1 };
static const int kingMoves_D2_1[2] = { 1,D1 };
static const int kingMoves_D2_2[2] = { 1,E1 };
static const int kingMoves_D2_3[2] = { 1,C2 };
static const int kingMoves_D2_4[2] = { 1,E2 };
static const int kingMoves_D2_5[2] = { 1,C3 };
static const int kingMoves_D2_6[2] = { 1,D3 };
static const int kingMoves_D2_7[2] = { 1,E3 };
#ifndef _DEBUG
static const Direction kingMoveDirections_D2[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_D2_0)
 ,Direction(MD_DOWN     ,kingMoves_D2_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_D2_2)
 ,Direction(MD_LEFT     ,kingMoves_D2_3)
 ,Direction(MD_RIGHT    ,kingMoves_D2_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_D2_5)
 ,Direction(MD_UP       ,kingMoves_D2_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_D2_7)
};
#else
static const Direction kingMoveDirections_D2[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_D2_0,_T("king MD_DOWNDIAG1 from D2:C1"))
 ,Direction(MD_DOWN     ,kingMoves_D2_1,_T("king MD_DOWN from D2:D1"))
 ,Direction(MD_DOWNDIAG2,kingMoves_D2_2,_T("king MD_DOWNDIAG2 from D2:E1"))
 ,Direction(MD_LEFT     ,kingMoves_D2_3,_T("king MD_LEFT from D2:C2"))
 ,Direction(MD_RIGHT    ,kingMoves_D2_4,_T("king MD_RIGHT from D2:E2"))
 ,Direction(MD_UPDIAG2  ,kingMoves_D2_5,_T("king MD_UPDIAG2 from D2:C3"))
 ,Direction(MD_UP       ,kingMoves_D2_6,_T("king MD_UP from D2:D3"))
 ,Direction(MD_UPDIAG1  ,kingMoves_D2_7,_T("king MD_UPDIAG1 from D2:E3"))
};
#endif
static const DirectionArray kingMoves_D2(8,kingMoveDirections_D2);

static const int kingMoves_E2_0[2] = { 1,D1 };
static const int kingMoves_E2_1[2] = { 1,E1 };
static const int kingMoves_E2_2[2] = { 1,F1 };
static const int kingMoves_E2_3[2] = { 1,D2 };
static const int kingMoves_E2_4[2] = { 1,F2 };
static const int kingMoves_E2_5[2] = { 1,D3 };
static const int kingMoves_E2_6[2] = { 1,E3 };
static const int kingMoves_E2_7[2] = { 1,F3 };
#ifndef _DEBUG
static const Direction kingMoveDirections_E2[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_E2_0)
 ,Direction(MD_DOWN     ,kingMoves_E2_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_E2_2)
 ,Direction(MD_LEFT     ,kingMoves_E2_3)
 ,Direction(MD_RIGHT    ,kingMoves_E2_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_E2_5)
 ,Direction(MD_UP       ,kingMoves_E2_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_E2_7)
};
#else
static const Direction kingMoveDirections_E2[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_E2_0,_T("king MD_DOWNDIAG1 from E2:D1"))
 ,Direction(MD_DOWN     ,kingMoves_E2_1,_T("king MD_DOWN from E2:E1"))
 ,Direction(MD_DOWNDIAG2,kingMoves_E2_2,_T("king MD_DOWNDIAG2 from E2:F1"))
 ,Direction(MD_LEFT     ,kingMoves_E2_3,_T("king MD_LEFT from E2:D2"))
 ,Direction(MD_RIGHT    ,kingMoves_E2_4,_T("king MD_RIGHT from E2:F2"))
 ,Direction(MD_UPDIAG2  ,kingMoves_E2_5,_T("king MD_UPDIAG2 from E2:D3"))
 ,Direction(MD_UP       ,kingMoves_E2_6,_T("king MD_UP from E2:E3"))
 ,Direction(MD_UPDIAG1  ,kingMoves_E2_7,_T("king MD_UPDIAG1 from E2:F3"))
};
#endif
static const DirectionArray kingMoves_E2(8,kingMoveDirections_E2);

static const int kingMoves_F2_0[2] = { 1,E1 };
static const int kingMoves_F2_1[2] = { 1,F1 };
static const int kingMoves_F2_2[2] = { 1,G1 };
static const int kingMoves_F2_3[2] = { 1,E2 };
static const int kingMoves_F2_4[2] = { 1,G2 };
static const int kingMoves_F2_5[2] = { 1,E3 };
static const int kingMoves_F2_6[2] = { 1,F3 };
static const int kingMoves_F2_7[2] = { 1,G3 };
#ifndef _DEBUG
static const Direction kingMoveDirections_F2[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_F2_0)
 ,Direction(MD_DOWN     ,kingMoves_F2_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_F2_2)
 ,Direction(MD_LEFT     ,kingMoves_F2_3)
 ,Direction(MD_RIGHT    ,kingMoves_F2_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_F2_5)
 ,Direction(MD_UP       ,kingMoves_F2_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_F2_7)
};
#else
static const Direction kingMoveDirections_F2[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_F2_0,_T("king MD_DOWNDIAG1 from F2:E1"))
 ,Direction(MD_DOWN     ,kingMoves_F2_1,_T("king MD_DOWN from F2:F1"))
 ,Direction(MD_DOWNDIAG2,kingMoves_F2_2,_T("king MD_DOWNDIAG2 from F2:G1"))
 ,Direction(MD_LEFT     ,kingMoves_F2_3,_T("king MD_LEFT from F2:E2"))
 ,Direction(MD_RIGHT    ,kingMoves_F2_4,_T("king MD_RIGHT from F2:G2"))
 ,Direction(MD_UPDIAG2  ,kingMoves_F2_5,_T("king MD_UPDIAG2 from F2:E3"))
 ,Direction(MD_UP       ,kingMoves_F2_6,_T("king MD_UP from F2:F3"))
 ,Direction(MD_UPDIAG1  ,kingMoves_F2_7,_T("king MD_UPDIAG1 from F2:G3"))
};
#endif
static const DirectionArray kingMoves_F2(8,kingMoveDirections_F2);

static const int kingMoves_G2_0[2] = { 1,F1 };
static const int kingMoves_G2_1[2] = { 1,G1 };
static const int kingMoves_G2_2[2] = { 1,H1 };
static const int kingMoves_G2_3[2] = { 1,F2 };
static const int kingMoves_G2_4[2] = { 1,H2 };
static const int kingMoves_G2_5[2] = { 1,F3 };
static const int kingMoves_G2_6[2] = { 1,G3 };
static const int kingMoves_G2_7[2] = { 1,H3 };
#ifndef _DEBUG
static const Direction kingMoveDirections_G2[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_G2_0)
 ,Direction(MD_DOWN     ,kingMoves_G2_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_G2_2)
 ,Direction(MD_LEFT     ,kingMoves_G2_3)
 ,Direction(MD_RIGHT    ,kingMoves_G2_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_G2_5)
 ,Direction(MD_UP       ,kingMoves_G2_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_G2_7)
};
#else
static const Direction kingMoveDirections_G2[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_G2_0,_T("king MD_DOWNDIAG1 from G2:F1"))
 ,Direction(MD_DOWN     ,kingMoves_G2_1,_T("king MD_DOWN from G2:G1"))
 ,Direction(MD_DOWNDIAG2,kingMoves_G2_2,_T("king MD_DOWNDIAG2 from G2:H1"))
 ,Direction(MD_LEFT     ,kingMoves_G2_3,_T("king MD_LEFT from G2:F2"))
 ,Direction(MD_RIGHT    ,kingMoves_G2_4,_T("king MD_RIGHT from G2:H2"))
 ,Direction(MD_UPDIAG2  ,kingMoves_G2_5,_T("king MD_UPDIAG2 from G2:F3"))
 ,Direction(MD_UP       ,kingMoves_G2_6,_T("king MD_UP from G2:G3"))
 ,Direction(MD_UPDIAG1  ,kingMoves_G2_7,_T("king MD_UPDIAG1 from G2:H3"))
};
#endif
static const DirectionArray kingMoves_G2(8,kingMoveDirections_G2);

static const int kingMoves_H2_0[2] = { 1,G1 };
static const int kingMoves_H2_1[2] = { 1,H1 };
static const int kingMoves_H2_2[2] = { 1,G2 };
static const int kingMoves_H2_3[2] = { 1,G3 };
static const int kingMoves_H2_4[2] = { 1,H3 };
#ifndef _DEBUG
static const Direction kingMoveDirections_H2[5] = {
  Direction(MD_DOWNDIAG1,kingMoves_H2_0)
 ,Direction(MD_DOWN     ,kingMoves_H2_1)
 ,Direction(MD_LEFT     ,kingMoves_H2_2)
 ,Direction(MD_UPDIAG2  ,kingMoves_H2_3)
 ,Direction(MD_UP       ,kingMoves_H2_4)
};
#else
static const Direction kingMoveDirections_H2[5] = {
  Direction(MD_DOWNDIAG1,kingMoves_H2_0,_T("king MD_DOWNDIAG1 from H2:G1"))
 ,Direction(MD_DOWN     ,kingMoves_H2_1,_T("king MD_DOWN from H2:H1"))
 ,Direction(MD_LEFT     ,kingMoves_H2_2,_T("king MD_LEFT from H2:G2"))
 ,Direction(MD_UPDIAG2  ,kingMoves_H2_3,_T("king MD_UPDIAG2 from H2:G3"))
 ,Direction(MD_UP       ,kingMoves_H2_4,_T("king MD_UP from H2:H3"))
};
#endif
static const DirectionArray kingMoves_H2(5,kingMoveDirections_H2);

static const int kingMoves_A3_0[2] = { 1,A2 };
static const int kingMoves_A3_1[2] = { 1,B2 };
static const int kingMoves_A3_2[2] = { 1,B3 };
static const int kingMoves_A3_3[2] = { 1,A4 };
static const int kingMoves_A3_4[2] = { 1,B4 };
#ifndef _DEBUG
static const Direction kingMoveDirections_A3[5] = {
  Direction(MD_DOWN     ,kingMoves_A3_0)
 ,Direction(MD_DOWNDIAG2,kingMoves_A3_1)
 ,Direction(MD_RIGHT    ,kingMoves_A3_2)
 ,Direction(MD_UP       ,kingMoves_A3_3)
 ,Direction(MD_UPDIAG1  ,kingMoves_A3_4)
};
#else
static const Direction kingMoveDirections_A3[5] = {
  Direction(MD_DOWN     ,kingMoves_A3_0,_T("king MD_DOWN from A3:A2"))
 ,Direction(MD_DOWNDIAG2,kingMoves_A3_1,_T("king MD_DOWNDIAG2 from A3:B2"))
 ,Direction(MD_RIGHT    ,kingMoves_A3_2,_T("king MD_RIGHT from A3:B3"))
 ,Direction(MD_UP       ,kingMoves_A3_3,_T("king MD_UP from A3:A4"))
 ,Direction(MD_UPDIAG1  ,kingMoves_A3_4,_T("king MD_UPDIAG1 from A3:B4"))
};
#endif
static const DirectionArray kingMoves_A3(5,kingMoveDirections_A3);

static const int kingMoves_B3_0[2] = { 1,A2 };
static const int kingMoves_B3_1[2] = { 1,B2 };
static const int kingMoves_B3_2[2] = { 1,C2 };
static const int kingMoves_B3_3[2] = { 1,A3 };
static const int kingMoves_B3_4[2] = { 1,C3 };
static const int kingMoves_B3_5[2] = { 1,A4 };
static const int kingMoves_B3_6[2] = { 1,B4 };
static const int kingMoves_B3_7[2] = { 1,C4 };
#ifndef _DEBUG
static const Direction kingMoveDirections_B3[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_B3_0)
 ,Direction(MD_DOWN     ,kingMoves_B3_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_B3_2)
 ,Direction(MD_LEFT     ,kingMoves_B3_3)
 ,Direction(MD_RIGHT    ,kingMoves_B3_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_B3_5)
 ,Direction(MD_UP       ,kingMoves_B3_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_B3_7)
};
#else
static const Direction kingMoveDirections_B3[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_B3_0,_T("king MD_DOWNDIAG1 from B3:A2"))
 ,Direction(MD_DOWN     ,kingMoves_B3_1,_T("king MD_DOWN from B3:B2"))
 ,Direction(MD_DOWNDIAG2,kingMoves_B3_2,_T("king MD_DOWNDIAG2 from B3:C2"))
 ,Direction(MD_LEFT     ,kingMoves_B3_3,_T("king MD_LEFT from B3:A3"))
 ,Direction(MD_RIGHT    ,kingMoves_B3_4,_T("king MD_RIGHT from B3:C3"))
 ,Direction(MD_UPDIAG2  ,kingMoves_B3_5,_T("king MD_UPDIAG2 from B3:A4"))
 ,Direction(MD_UP       ,kingMoves_B3_6,_T("king MD_UP from B3:B4"))
 ,Direction(MD_UPDIAG1  ,kingMoves_B3_7,_T("king MD_UPDIAG1 from B3:C4"))
};
#endif
static const DirectionArray kingMoves_B3(8,kingMoveDirections_B3);

static const int kingMoves_C3_0[2] = { 1,B2 };
static const int kingMoves_C3_1[2] = { 1,C2 };
static const int kingMoves_C3_2[2] = { 1,D2 };
static const int kingMoves_C3_3[2] = { 1,B3 };
static const int kingMoves_C3_4[2] = { 1,D3 };
static const int kingMoves_C3_5[2] = { 1,B4 };
static const int kingMoves_C3_6[2] = { 1,C4 };
static const int kingMoves_C3_7[2] = { 1,D4 };
#ifndef _DEBUG
static const Direction kingMoveDirections_C3[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_C3_0)
 ,Direction(MD_DOWN     ,kingMoves_C3_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_C3_2)
 ,Direction(MD_LEFT     ,kingMoves_C3_3)
 ,Direction(MD_RIGHT    ,kingMoves_C3_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_C3_5)
 ,Direction(MD_UP       ,kingMoves_C3_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_C3_7)
};
#else
static const Direction kingMoveDirections_C3[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_C3_0,_T("king MD_DOWNDIAG1 from C3:B2"))
 ,Direction(MD_DOWN     ,kingMoves_C3_1,_T("king MD_DOWN from C3:C2"))
 ,Direction(MD_DOWNDIAG2,kingMoves_C3_2,_T("king MD_DOWNDIAG2 from C3:D2"))
 ,Direction(MD_LEFT     ,kingMoves_C3_3,_T("king MD_LEFT from C3:B3"))
 ,Direction(MD_RIGHT    ,kingMoves_C3_4,_T("king MD_RIGHT from C3:D3"))
 ,Direction(MD_UPDIAG2  ,kingMoves_C3_5,_T("king MD_UPDIAG2 from C3:B4"))
 ,Direction(MD_UP       ,kingMoves_C3_6,_T("king MD_UP from C3:C4"))
 ,Direction(MD_UPDIAG1  ,kingMoves_C3_7,_T("king MD_UPDIAG1 from C3:D4"))
};
#endif
static const DirectionArray kingMoves_C3(8,kingMoveDirections_C3);

static const int kingMoves_D3_0[2] = { 1,C2 };
static const int kingMoves_D3_1[2] = { 1,D2 };
static const int kingMoves_D3_2[2] = { 1,E2 };
static const int kingMoves_D3_3[2] = { 1,C3 };
static const int kingMoves_D3_4[2] = { 1,E3 };
static const int kingMoves_D3_5[2] = { 1,C4 };
static const int kingMoves_D3_6[2] = { 1,D4 };
static const int kingMoves_D3_7[2] = { 1,E4 };
#ifndef _DEBUG
static const Direction kingMoveDirections_D3[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_D3_0)
 ,Direction(MD_DOWN     ,kingMoves_D3_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_D3_2)
 ,Direction(MD_LEFT     ,kingMoves_D3_3)
 ,Direction(MD_RIGHT    ,kingMoves_D3_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_D3_5)
 ,Direction(MD_UP       ,kingMoves_D3_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_D3_7)
};
#else
static const Direction kingMoveDirections_D3[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_D3_0,_T("king MD_DOWNDIAG1 from D3:C2"))
 ,Direction(MD_DOWN     ,kingMoves_D3_1,_T("king MD_DOWN from D3:D2"))
 ,Direction(MD_DOWNDIAG2,kingMoves_D3_2,_T("king MD_DOWNDIAG2 from D3:E2"))
 ,Direction(MD_LEFT     ,kingMoves_D3_3,_T("king MD_LEFT from D3:C3"))
 ,Direction(MD_RIGHT    ,kingMoves_D3_4,_T("king MD_RIGHT from D3:E3"))
 ,Direction(MD_UPDIAG2  ,kingMoves_D3_5,_T("king MD_UPDIAG2 from D3:C4"))
 ,Direction(MD_UP       ,kingMoves_D3_6,_T("king MD_UP from D3:D4"))
 ,Direction(MD_UPDIAG1  ,kingMoves_D3_7,_T("king MD_UPDIAG1 from D3:E4"))
};
#endif
static const DirectionArray kingMoves_D3(8,kingMoveDirections_D3);

static const int kingMoves_E3_0[2] = { 1,D2 };
static const int kingMoves_E3_1[2] = { 1,E2 };
static const int kingMoves_E3_2[2] = { 1,F2 };
static const int kingMoves_E3_3[2] = { 1,D3 };
static const int kingMoves_E3_4[2] = { 1,F3 };
static const int kingMoves_E3_5[2] = { 1,D4 };
static const int kingMoves_E3_6[2] = { 1,E4 };
static const int kingMoves_E3_7[2] = { 1,F4 };
#ifndef _DEBUG
static const Direction kingMoveDirections_E3[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_E3_0)
 ,Direction(MD_DOWN     ,kingMoves_E3_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_E3_2)
 ,Direction(MD_LEFT     ,kingMoves_E3_3)
 ,Direction(MD_RIGHT    ,kingMoves_E3_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_E3_5)
 ,Direction(MD_UP       ,kingMoves_E3_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_E3_7)
};
#else
static const Direction kingMoveDirections_E3[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_E3_0,_T("king MD_DOWNDIAG1 from E3:D2"))
 ,Direction(MD_DOWN     ,kingMoves_E3_1,_T("king MD_DOWN from E3:E2"))
 ,Direction(MD_DOWNDIAG2,kingMoves_E3_2,_T("king MD_DOWNDIAG2 from E3:F2"))
 ,Direction(MD_LEFT     ,kingMoves_E3_3,_T("king MD_LEFT from E3:D3"))
 ,Direction(MD_RIGHT    ,kingMoves_E3_4,_T("king MD_RIGHT from E3:F3"))
 ,Direction(MD_UPDIAG2  ,kingMoves_E3_5,_T("king MD_UPDIAG2 from E3:D4"))
 ,Direction(MD_UP       ,kingMoves_E3_6,_T("king MD_UP from E3:E4"))
 ,Direction(MD_UPDIAG1  ,kingMoves_E3_7,_T("king MD_UPDIAG1 from E3:F4"))
};
#endif
static const DirectionArray kingMoves_E3(8,kingMoveDirections_E3);

static const int kingMoves_F3_0[2] = { 1,E2 };
static const int kingMoves_F3_1[2] = { 1,F2 };
static const int kingMoves_F3_2[2] = { 1,G2 };
static const int kingMoves_F3_3[2] = { 1,E3 };
static const int kingMoves_F3_4[2] = { 1,G3 };
static const int kingMoves_F3_5[2] = { 1,E4 };
static const int kingMoves_F3_6[2] = { 1,F4 };
static const int kingMoves_F3_7[2] = { 1,G4 };
#ifndef _DEBUG
static const Direction kingMoveDirections_F3[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_F3_0)
 ,Direction(MD_DOWN     ,kingMoves_F3_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_F3_2)
 ,Direction(MD_LEFT     ,kingMoves_F3_3)
 ,Direction(MD_RIGHT    ,kingMoves_F3_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_F3_5)
 ,Direction(MD_UP       ,kingMoves_F3_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_F3_7)
};
#else
static const Direction kingMoveDirections_F3[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_F3_0,_T("king MD_DOWNDIAG1 from F3:E2"))
 ,Direction(MD_DOWN     ,kingMoves_F3_1,_T("king MD_DOWN from F3:F2"))
 ,Direction(MD_DOWNDIAG2,kingMoves_F3_2,_T("king MD_DOWNDIAG2 from F3:G2"))
 ,Direction(MD_LEFT     ,kingMoves_F3_3,_T("king MD_LEFT from F3:E3"))
 ,Direction(MD_RIGHT    ,kingMoves_F3_4,_T("king MD_RIGHT from F3:G3"))
 ,Direction(MD_UPDIAG2  ,kingMoves_F3_5,_T("king MD_UPDIAG2 from F3:E4"))
 ,Direction(MD_UP       ,kingMoves_F3_6,_T("king MD_UP from F3:F4"))
 ,Direction(MD_UPDIAG1  ,kingMoves_F3_7,_T("king MD_UPDIAG1 from F3:G4"))
};
#endif
static const DirectionArray kingMoves_F3(8,kingMoveDirections_F3);

static const int kingMoves_G3_0[2] = { 1,F2 };
static const int kingMoves_G3_1[2] = { 1,G2 };
static const int kingMoves_G3_2[2] = { 1,H2 };
static const int kingMoves_G3_3[2] = { 1,F3 };
static const int kingMoves_G3_4[2] = { 1,H3 };
static const int kingMoves_G3_5[2] = { 1,F4 };
static const int kingMoves_G3_6[2] = { 1,G4 };
static const int kingMoves_G3_7[2] = { 1,H4 };
#ifndef _DEBUG
static const Direction kingMoveDirections_G3[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_G3_0)
 ,Direction(MD_DOWN     ,kingMoves_G3_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_G3_2)
 ,Direction(MD_LEFT     ,kingMoves_G3_3)
 ,Direction(MD_RIGHT    ,kingMoves_G3_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_G3_5)
 ,Direction(MD_UP       ,kingMoves_G3_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_G3_7)
};
#else
static const Direction kingMoveDirections_G3[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_G3_0,_T("king MD_DOWNDIAG1 from G3:F2"))
 ,Direction(MD_DOWN     ,kingMoves_G3_1,_T("king MD_DOWN from G3:G2"))
 ,Direction(MD_DOWNDIAG2,kingMoves_G3_2,_T("king MD_DOWNDIAG2 from G3:H2"))
 ,Direction(MD_LEFT     ,kingMoves_G3_3,_T("king MD_LEFT from G3:F3"))
 ,Direction(MD_RIGHT    ,kingMoves_G3_4,_T("king MD_RIGHT from G3:H3"))
 ,Direction(MD_UPDIAG2  ,kingMoves_G3_5,_T("king MD_UPDIAG2 from G3:F4"))
 ,Direction(MD_UP       ,kingMoves_G3_6,_T("king MD_UP from G3:G4"))
 ,Direction(MD_UPDIAG1  ,kingMoves_G3_7,_T("king MD_UPDIAG1 from G3:H4"))
};
#endif
static const DirectionArray kingMoves_G3(8,kingMoveDirections_G3);

static const int kingMoves_H3_0[2] = { 1,G2 };
static const int kingMoves_H3_1[2] = { 1,H2 };
static const int kingMoves_H3_2[2] = { 1,G3 };
static const int kingMoves_H3_3[2] = { 1,G4 };
static const int kingMoves_H3_4[2] = { 1,H4 };
#ifndef _DEBUG
static const Direction kingMoveDirections_H3[5] = {
  Direction(MD_DOWNDIAG1,kingMoves_H3_0)
 ,Direction(MD_DOWN     ,kingMoves_H3_1)
 ,Direction(MD_LEFT     ,kingMoves_H3_2)
 ,Direction(MD_UPDIAG2  ,kingMoves_H3_3)
 ,Direction(MD_UP       ,kingMoves_H3_4)
};
#else
static const Direction kingMoveDirections_H3[5] = {
  Direction(MD_DOWNDIAG1,kingMoves_H3_0,_T("king MD_DOWNDIAG1 from H3:G2"))
 ,Direction(MD_DOWN     ,kingMoves_H3_1,_T("king MD_DOWN from H3:H2"))
 ,Direction(MD_LEFT     ,kingMoves_H3_2,_T("king MD_LEFT from H3:G3"))
 ,Direction(MD_UPDIAG2  ,kingMoves_H3_3,_T("king MD_UPDIAG2 from H3:G4"))
 ,Direction(MD_UP       ,kingMoves_H3_4,_T("king MD_UP from H3:H4"))
};
#endif
static const DirectionArray kingMoves_H3(5,kingMoveDirections_H3);

static const int kingMoves_A4_0[2] = { 1,A3 };
static const int kingMoves_A4_1[2] = { 1,B3 };
static const int kingMoves_A4_2[2] = { 1,B4 };
static const int kingMoves_A4_3[2] = { 1,A5 };
static const int kingMoves_A4_4[2] = { 1,B5 };
#ifndef _DEBUG
static const Direction kingMoveDirections_A4[5] = {
  Direction(MD_DOWN     ,kingMoves_A4_0)
 ,Direction(MD_DOWNDIAG2,kingMoves_A4_1)
 ,Direction(MD_RIGHT    ,kingMoves_A4_2)
 ,Direction(MD_UP       ,kingMoves_A4_3)
 ,Direction(MD_UPDIAG1  ,kingMoves_A4_4)
};
#else
static const Direction kingMoveDirections_A4[5] = {
  Direction(MD_DOWN     ,kingMoves_A4_0,_T("king MD_DOWN from A4:A3"))
 ,Direction(MD_DOWNDIAG2,kingMoves_A4_1,_T("king MD_DOWNDIAG2 from A4:B3"))
 ,Direction(MD_RIGHT    ,kingMoves_A4_2,_T("king MD_RIGHT from A4:B4"))
 ,Direction(MD_UP       ,kingMoves_A4_3,_T("king MD_UP from A4:A5"))
 ,Direction(MD_UPDIAG1  ,kingMoves_A4_4,_T("king MD_UPDIAG1 from A4:B5"))
};
#endif
static const DirectionArray kingMoves_A4(5,kingMoveDirections_A4);

static const int kingMoves_B4_0[2] = { 1,A3 };
static const int kingMoves_B4_1[2] = { 1,B3 };
static const int kingMoves_B4_2[2] = { 1,C3 };
static const int kingMoves_B4_3[2] = { 1,A4 };
static const int kingMoves_B4_4[2] = { 1,C4 };
static const int kingMoves_B4_5[2] = { 1,A5 };
static const int kingMoves_B4_6[2] = { 1,B5 };
static const int kingMoves_B4_7[2] = { 1,C5 };
#ifndef _DEBUG
static const Direction kingMoveDirections_B4[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_B4_0)
 ,Direction(MD_DOWN     ,kingMoves_B4_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_B4_2)
 ,Direction(MD_LEFT     ,kingMoves_B4_3)
 ,Direction(MD_RIGHT    ,kingMoves_B4_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_B4_5)
 ,Direction(MD_UP       ,kingMoves_B4_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_B4_7)
};
#else
static const Direction kingMoveDirections_B4[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_B4_0,_T("king MD_DOWNDIAG1 from B4:A3"))
 ,Direction(MD_DOWN     ,kingMoves_B4_1,_T("king MD_DOWN from B4:B3"))
 ,Direction(MD_DOWNDIAG2,kingMoves_B4_2,_T("king MD_DOWNDIAG2 from B4:C3"))
 ,Direction(MD_LEFT     ,kingMoves_B4_3,_T("king MD_LEFT from B4:A4"))
 ,Direction(MD_RIGHT    ,kingMoves_B4_4,_T("king MD_RIGHT from B4:C4"))
 ,Direction(MD_UPDIAG2  ,kingMoves_B4_5,_T("king MD_UPDIAG2 from B4:A5"))
 ,Direction(MD_UP       ,kingMoves_B4_6,_T("king MD_UP from B4:B5"))
 ,Direction(MD_UPDIAG1  ,kingMoves_B4_7,_T("king MD_UPDIAG1 from B4:C5"))
};
#endif
static const DirectionArray kingMoves_B4(8,kingMoveDirections_B4);

static const int kingMoves_C4_0[2] = { 1,B3 };
static const int kingMoves_C4_1[2] = { 1,C3 };
static const int kingMoves_C4_2[2] = { 1,D3 };
static const int kingMoves_C4_3[2] = { 1,B4 };
static const int kingMoves_C4_4[2] = { 1,D4 };
static const int kingMoves_C4_5[2] = { 1,B5 };
static const int kingMoves_C4_6[2] = { 1,C5 };
static const int kingMoves_C4_7[2] = { 1,D5 };
#ifndef _DEBUG
static const Direction kingMoveDirections_C4[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_C4_0)
 ,Direction(MD_DOWN     ,kingMoves_C4_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_C4_2)
 ,Direction(MD_LEFT     ,kingMoves_C4_3)
 ,Direction(MD_RIGHT    ,kingMoves_C4_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_C4_5)
 ,Direction(MD_UP       ,kingMoves_C4_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_C4_7)
};
#else
static const Direction kingMoveDirections_C4[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_C4_0,_T("king MD_DOWNDIAG1 from C4:B3"))
 ,Direction(MD_DOWN     ,kingMoves_C4_1,_T("king MD_DOWN from C4:C3"))
 ,Direction(MD_DOWNDIAG2,kingMoves_C4_2,_T("king MD_DOWNDIAG2 from C4:D3"))
 ,Direction(MD_LEFT     ,kingMoves_C4_3,_T("king MD_LEFT from C4:B4"))
 ,Direction(MD_RIGHT    ,kingMoves_C4_4,_T("king MD_RIGHT from C4:D4"))
 ,Direction(MD_UPDIAG2  ,kingMoves_C4_5,_T("king MD_UPDIAG2 from C4:B5"))
 ,Direction(MD_UP       ,kingMoves_C4_6,_T("king MD_UP from C4:C5"))
 ,Direction(MD_UPDIAG1  ,kingMoves_C4_7,_T("king MD_UPDIAG1 from C4:D5"))
};
#endif
static const DirectionArray kingMoves_C4(8,kingMoveDirections_C4);

static const int kingMoves_D4_0[2] = { 1,C3 };
static const int kingMoves_D4_1[2] = { 1,D3 };
static const int kingMoves_D4_2[2] = { 1,E3 };
static const int kingMoves_D4_3[2] = { 1,C4 };
static const int kingMoves_D4_4[2] = { 1,E4 };
static const int kingMoves_D4_5[2] = { 1,C5 };
static const int kingMoves_D4_6[2] = { 1,D5 };
static const int kingMoves_D4_7[2] = { 1,E5 };
#ifndef _DEBUG
static const Direction kingMoveDirections_D4[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_D4_0)
 ,Direction(MD_DOWN     ,kingMoves_D4_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_D4_2)
 ,Direction(MD_LEFT     ,kingMoves_D4_3)
 ,Direction(MD_RIGHT    ,kingMoves_D4_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_D4_5)
 ,Direction(MD_UP       ,kingMoves_D4_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_D4_7)
};
#else
static const Direction kingMoveDirections_D4[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_D4_0,_T("king MD_DOWNDIAG1 from D4:C3"))
 ,Direction(MD_DOWN     ,kingMoves_D4_1,_T("king MD_DOWN from D4:D3"))
 ,Direction(MD_DOWNDIAG2,kingMoves_D4_2,_T("king MD_DOWNDIAG2 from D4:E3"))
 ,Direction(MD_LEFT     ,kingMoves_D4_3,_T("king MD_LEFT from D4:C4"))
 ,Direction(MD_RIGHT    ,kingMoves_D4_4,_T("king MD_RIGHT from D4:E4"))
 ,Direction(MD_UPDIAG2  ,kingMoves_D4_5,_T("king MD_UPDIAG2 from D4:C5"))
 ,Direction(MD_UP       ,kingMoves_D4_6,_T("king MD_UP from D4:D5"))
 ,Direction(MD_UPDIAG1  ,kingMoves_D4_7,_T("king MD_UPDIAG1 from D4:E5"))
};
#endif
static const DirectionArray kingMoves_D4(8,kingMoveDirections_D4);

static const int kingMoves_E4_0[2] = { 1,D3 };
static const int kingMoves_E4_1[2] = { 1,E3 };
static const int kingMoves_E4_2[2] = { 1,F3 };
static const int kingMoves_E4_3[2] = { 1,D4 };
static const int kingMoves_E4_4[2] = { 1,F4 };
static const int kingMoves_E4_5[2] = { 1,D5 };
static const int kingMoves_E4_6[2] = { 1,E5 };
static const int kingMoves_E4_7[2] = { 1,F5 };
#ifndef _DEBUG
static const Direction kingMoveDirections_E4[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_E4_0)
 ,Direction(MD_DOWN     ,kingMoves_E4_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_E4_2)
 ,Direction(MD_LEFT     ,kingMoves_E4_3)
 ,Direction(MD_RIGHT    ,kingMoves_E4_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_E4_5)
 ,Direction(MD_UP       ,kingMoves_E4_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_E4_7)
};
#else
static const Direction kingMoveDirections_E4[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_E4_0,_T("king MD_DOWNDIAG1 from E4:D3"))
 ,Direction(MD_DOWN     ,kingMoves_E4_1,_T("king MD_DOWN from E4:E3"))
 ,Direction(MD_DOWNDIAG2,kingMoves_E4_2,_T("king MD_DOWNDIAG2 from E4:F3"))
 ,Direction(MD_LEFT     ,kingMoves_E4_3,_T("king MD_LEFT from E4:D4"))
 ,Direction(MD_RIGHT    ,kingMoves_E4_4,_T("king MD_RIGHT from E4:F4"))
 ,Direction(MD_UPDIAG2  ,kingMoves_E4_5,_T("king MD_UPDIAG2 from E4:D5"))
 ,Direction(MD_UP       ,kingMoves_E4_6,_T("king MD_UP from E4:E5"))
 ,Direction(MD_UPDIAG1  ,kingMoves_E4_7,_T("king MD_UPDIAG1 from E4:F5"))
};
#endif
static const DirectionArray kingMoves_E4(8,kingMoveDirections_E4);

static const int kingMoves_F4_0[2] = { 1,E3 };
static const int kingMoves_F4_1[2] = { 1,F3 };
static const int kingMoves_F4_2[2] = { 1,G3 };
static const int kingMoves_F4_3[2] = { 1,E4 };
static const int kingMoves_F4_4[2] = { 1,G4 };
static const int kingMoves_F4_5[2] = { 1,E5 };
static const int kingMoves_F4_6[2] = { 1,F5 };
static const int kingMoves_F4_7[2] = { 1,G5 };
#ifndef _DEBUG
static const Direction kingMoveDirections_F4[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_F4_0)
 ,Direction(MD_DOWN     ,kingMoves_F4_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_F4_2)
 ,Direction(MD_LEFT     ,kingMoves_F4_3)
 ,Direction(MD_RIGHT    ,kingMoves_F4_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_F4_5)
 ,Direction(MD_UP       ,kingMoves_F4_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_F4_7)
};
#else
static const Direction kingMoveDirections_F4[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_F4_0,_T("king MD_DOWNDIAG1 from F4:E3"))
 ,Direction(MD_DOWN     ,kingMoves_F4_1,_T("king MD_DOWN from F4:F3"))
 ,Direction(MD_DOWNDIAG2,kingMoves_F4_2,_T("king MD_DOWNDIAG2 from F4:G3"))
 ,Direction(MD_LEFT     ,kingMoves_F4_3,_T("king MD_LEFT from F4:E4"))
 ,Direction(MD_RIGHT    ,kingMoves_F4_4,_T("king MD_RIGHT from F4:G4"))
 ,Direction(MD_UPDIAG2  ,kingMoves_F4_5,_T("king MD_UPDIAG2 from F4:E5"))
 ,Direction(MD_UP       ,kingMoves_F4_6,_T("king MD_UP from F4:F5"))
 ,Direction(MD_UPDIAG1  ,kingMoves_F4_7,_T("king MD_UPDIAG1 from F4:G5"))
};
#endif
static const DirectionArray kingMoves_F4(8,kingMoveDirections_F4);

static const int kingMoves_G4_0[2] = { 1,F3 };
static const int kingMoves_G4_1[2] = { 1,G3 };
static const int kingMoves_G4_2[2] = { 1,H3 };
static const int kingMoves_G4_3[2] = { 1,F4 };
static const int kingMoves_G4_4[2] = { 1,H4 };
static const int kingMoves_G4_5[2] = { 1,F5 };
static const int kingMoves_G4_6[2] = { 1,G5 };
static const int kingMoves_G4_7[2] = { 1,H5 };
#ifndef _DEBUG
static const Direction kingMoveDirections_G4[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_G4_0)
 ,Direction(MD_DOWN     ,kingMoves_G4_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_G4_2)
 ,Direction(MD_LEFT     ,kingMoves_G4_3)
 ,Direction(MD_RIGHT    ,kingMoves_G4_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_G4_5)
 ,Direction(MD_UP       ,kingMoves_G4_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_G4_7)
};
#else
static const Direction kingMoveDirections_G4[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_G4_0,_T("king MD_DOWNDIAG1 from G4:F3"))
 ,Direction(MD_DOWN     ,kingMoves_G4_1,_T("king MD_DOWN from G4:G3"))
 ,Direction(MD_DOWNDIAG2,kingMoves_G4_2,_T("king MD_DOWNDIAG2 from G4:H3"))
 ,Direction(MD_LEFT     ,kingMoves_G4_3,_T("king MD_LEFT from G4:F4"))
 ,Direction(MD_RIGHT    ,kingMoves_G4_4,_T("king MD_RIGHT from G4:H4"))
 ,Direction(MD_UPDIAG2  ,kingMoves_G4_5,_T("king MD_UPDIAG2 from G4:F5"))
 ,Direction(MD_UP       ,kingMoves_G4_6,_T("king MD_UP from G4:G5"))
 ,Direction(MD_UPDIAG1  ,kingMoves_G4_7,_T("king MD_UPDIAG1 from G4:H5"))
};
#endif
static const DirectionArray kingMoves_G4(8,kingMoveDirections_G4);

static const int kingMoves_H4_0[2] = { 1,G3 };
static const int kingMoves_H4_1[2] = { 1,H3 };
static const int kingMoves_H4_2[2] = { 1,G4 };
static const int kingMoves_H4_3[2] = { 1,G5 };
static const int kingMoves_H4_4[2] = { 1,H5 };
#ifndef _DEBUG
static const Direction kingMoveDirections_H4[5] = {
  Direction(MD_DOWNDIAG1,kingMoves_H4_0)
 ,Direction(MD_DOWN     ,kingMoves_H4_1)
 ,Direction(MD_LEFT     ,kingMoves_H4_2)
 ,Direction(MD_UPDIAG2  ,kingMoves_H4_3)
 ,Direction(MD_UP       ,kingMoves_H4_4)
};
#else
static const Direction kingMoveDirections_H4[5] = {
  Direction(MD_DOWNDIAG1,kingMoves_H4_0,_T("king MD_DOWNDIAG1 from H4:G3"))
 ,Direction(MD_DOWN     ,kingMoves_H4_1,_T("king MD_DOWN from H4:H3"))
 ,Direction(MD_LEFT     ,kingMoves_H4_2,_T("king MD_LEFT from H4:G4"))
 ,Direction(MD_UPDIAG2  ,kingMoves_H4_3,_T("king MD_UPDIAG2 from H4:G5"))
 ,Direction(MD_UP       ,kingMoves_H4_4,_T("king MD_UP from H4:H5"))
};
#endif
static const DirectionArray kingMoves_H4(5,kingMoveDirections_H4);

static const int kingMoves_A5_0[2] = { 1,A4 };
static const int kingMoves_A5_1[2] = { 1,B4 };
static const int kingMoves_A5_2[2] = { 1,B5 };
static const int kingMoves_A5_3[2] = { 1,A6 };
static const int kingMoves_A5_4[2] = { 1,B6 };
#ifndef _DEBUG
static const Direction kingMoveDirections_A5[5] = {
  Direction(MD_DOWN     ,kingMoves_A5_0)
 ,Direction(MD_DOWNDIAG2,kingMoves_A5_1)
 ,Direction(MD_RIGHT    ,kingMoves_A5_2)
 ,Direction(MD_UP       ,kingMoves_A5_3)
 ,Direction(MD_UPDIAG1  ,kingMoves_A5_4)
};
#else
static const Direction kingMoveDirections_A5[5] = {
  Direction(MD_DOWN     ,kingMoves_A5_0,_T("king MD_DOWN from A5:A4"))
 ,Direction(MD_DOWNDIAG2,kingMoves_A5_1,_T("king MD_DOWNDIAG2 from A5:B4"))
 ,Direction(MD_RIGHT    ,kingMoves_A5_2,_T("king MD_RIGHT from A5:B5"))
 ,Direction(MD_UP       ,kingMoves_A5_3,_T("king MD_UP from A5:A6"))
 ,Direction(MD_UPDIAG1  ,kingMoves_A5_4,_T("king MD_UPDIAG1 from A5:B6"))
};
#endif
static const DirectionArray kingMoves_A5(5,kingMoveDirections_A5);

static const int kingMoves_B5_0[2] = { 1,A4 };
static const int kingMoves_B5_1[2] = { 1,B4 };
static const int kingMoves_B5_2[2] = { 1,C4 };
static const int kingMoves_B5_3[2] = { 1,A5 };
static const int kingMoves_B5_4[2] = { 1,C5 };
static const int kingMoves_B5_5[2] = { 1,A6 };
static const int kingMoves_B5_6[2] = { 1,B6 };
static const int kingMoves_B5_7[2] = { 1,C6 };
#ifndef _DEBUG
static const Direction kingMoveDirections_B5[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_B5_0)
 ,Direction(MD_DOWN     ,kingMoves_B5_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_B5_2)
 ,Direction(MD_LEFT     ,kingMoves_B5_3)
 ,Direction(MD_RIGHT    ,kingMoves_B5_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_B5_5)
 ,Direction(MD_UP       ,kingMoves_B5_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_B5_7)
};
#else
static const Direction kingMoveDirections_B5[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_B5_0,_T("king MD_DOWNDIAG1 from B5:A4"))
 ,Direction(MD_DOWN     ,kingMoves_B5_1,_T("king MD_DOWN from B5:B4"))
 ,Direction(MD_DOWNDIAG2,kingMoves_B5_2,_T("king MD_DOWNDIAG2 from B5:C4"))
 ,Direction(MD_LEFT     ,kingMoves_B5_3,_T("king MD_LEFT from B5:A5"))
 ,Direction(MD_RIGHT    ,kingMoves_B5_4,_T("king MD_RIGHT from B5:C5"))
 ,Direction(MD_UPDIAG2  ,kingMoves_B5_5,_T("king MD_UPDIAG2 from B5:A6"))
 ,Direction(MD_UP       ,kingMoves_B5_6,_T("king MD_UP from B5:B6"))
 ,Direction(MD_UPDIAG1  ,kingMoves_B5_7,_T("king MD_UPDIAG1 from B5:C6"))
};
#endif
static const DirectionArray kingMoves_B5(8,kingMoveDirections_B5);

static const int kingMoves_C5_0[2] = { 1,B4 };
static const int kingMoves_C5_1[2] = { 1,C4 };
static const int kingMoves_C5_2[2] = { 1,D4 };
static const int kingMoves_C5_3[2] = { 1,B5 };
static const int kingMoves_C5_4[2] = { 1,D5 };
static const int kingMoves_C5_5[2] = { 1,B6 };
static const int kingMoves_C5_6[2] = { 1,C6 };
static const int kingMoves_C5_7[2] = { 1,D6 };
#ifndef _DEBUG
static const Direction kingMoveDirections_C5[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_C5_0)
 ,Direction(MD_DOWN     ,kingMoves_C5_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_C5_2)
 ,Direction(MD_LEFT     ,kingMoves_C5_3)
 ,Direction(MD_RIGHT    ,kingMoves_C5_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_C5_5)
 ,Direction(MD_UP       ,kingMoves_C5_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_C5_7)
};
#else
static const Direction kingMoveDirections_C5[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_C5_0,_T("king MD_DOWNDIAG1 from C5:B4"))
 ,Direction(MD_DOWN     ,kingMoves_C5_1,_T("king MD_DOWN from C5:C4"))
 ,Direction(MD_DOWNDIAG2,kingMoves_C5_2,_T("king MD_DOWNDIAG2 from C5:D4"))
 ,Direction(MD_LEFT     ,kingMoves_C5_3,_T("king MD_LEFT from C5:B5"))
 ,Direction(MD_RIGHT    ,kingMoves_C5_4,_T("king MD_RIGHT from C5:D5"))
 ,Direction(MD_UPDIAG2  ,kingMoves_C5_5,_T("king MD_UPDIAG2 from C5:B6"))
 ,Direction(MD_UP       ,kingMoves_C5_6,_T("king MD_UP from C5:C6"))
 ,Direction(MD_UPDIAG1  ,kingMoves_C5_7,_T("king MD_UPDIAG1 from C5:D6"))
};
#endif
static const DirectionArray kingMoves_C5(8,kingMoveDirections_C5);

static const int kingMoves_D5_0[2] = { 1,C4 };
static const int kingMoves_D5_1[2] = { 1,D4 };
static const int kingMoves_D5_2[2] = { 1,E4 };
static const int kingMoves_D5_3[2] = { 1,C5 };
static const int kingMoves_D5_4[2] = { 1,E5 };
static const int kingMoves_D5_5[2] = { 1,C6 };
static const int kingMoves_D5_6[2] = { 1,D6 };
static const int kingMoves_D5_7[2] = { 1,E6 };
#ifndef _DEBUG
static const Direction kingMoveDirections_D5[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_D5_0)
 ,Direction(MD_DOWN     ,kingMoves_D5_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_D5_2)
 ,Direction(MD_LEFT     ,kingMoves_D5_3)
 ,Direction(MD_RIGHT    ,kingMoves_D5_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_D5_5)
 ,Direction(MD_UP       ,kingMoves_D5_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_D5_7)
};
#else
static const Direction kingMoveDirections_D5[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_D5_0,_T("king MD_DOWNDIAG1 from D5:C4"))
 ,Direction(MD_DOWN     ,kingMoves_D5_1,_T("king MD_DOWN from D5:D4"))
 ,Direction(MD_DOWNDIAG2,kingMoves_D5_2,_T("king MD_DOWNDIAG2 from D5:E4"))
 ,Direction(MD_LEFT     ,kingMoves_D5_3,_T("king MD_LEFT from D5:C5"))
 ,Direction(MD_RIGHT    ,kingMoves_D5_4,_T("king MD_RIGHT from D5:E5"))
 ,Direction(MD_UPDIAG2  ,kingMoves_D5_5,_T("king MD_UPDIAG2 from D5:C6"))
 ,Direction(MD_UP       ,kingMoves_D5_6,_T("king MD_UP from D5:D6"))
 ,Direction(MD_UPDIAG1  ,kingMoves_D5_7,_T("king MD_UPDIAG1 from D5:E6"))
};
#endif
static const DirectionArray kingMoves_D5(8,kingMoveDirections_D5);

static const int kingMoves_E5_0[2] = { 1,D4 };
static const int kingMoves_E5_1[2] = { 1,E4 };
static const int kingMoves_E5_2[2] = { 1,F4 };
static const int kingMoves_E5_3[2] = { 1,D5 };
static const int kingMoves_E5_4[2] = { 1,F5 };
static const int kingMoves_E5_5[2] = { 1,D6 };
static const int kingMoves_E5_6[2] = { 1,E6 };
static const int kingMoves_E5_7[2] = { 1,F6 };
#ifndef _DEBUG
static const Direction kingMoveDirections_E5[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_E5_0)
 ,Direction(MD_DOWN     ,kingMoves_E5_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_E5_2)
 ,Direction(MD_LEFT     ,kingMoves_E5_3)
 ,Direction(MD_RIGHT    ,kingMoves_E5_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_E5_5)
 ,Direction(MD_UP       ,kingMoves_E5_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_E5_7)
};
#else
static const Direction kingMoveDirections_E5[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_E5_0,_T("king MD_DOWNDIAG1 from E5:D4"))
 ,Direction(MD_DOWN     ,kingMoves_E5_1,_T("king MD_DOWN from E5:E4"))
 ,Direction(MD_DOWNDIAG2,kingMoves_E5_2,_T("king MD_DOWNDIAG2 from E5:F4"))
 ,Direction(MD_LEFT     ,kingMoves_E5_3,_T("king MD_LEFT from E5:D5"))
 ,Direction(MD_RIGHT    ,kingMoves_E5_4,_T("king MD_RIGHT from E5:F5"))
 ,Direction(MD_UPDIAG2  ,kingMoves_E5_5,_T("king MD_UPDIAG2 from E5:D6"))
 ,Direction(MD_UP       ,kingMoves_E5_6,_T("king MD_UP from E5:E6"))
 ,Direction(MD_UPDIAG1  ,kingMoves_E5_7,_T("king MD_UPDIAG1 from E5:F6"))
};
#endif
static const DirectionArray kingMoves_E5(8,kingMoveDirections_E5);

static const int kingMoves_F5_0[2] = { 1,E4 };
static const int kingMoves_F5_1[2] = { 1,F4 };
static const int kingMoves_F5_2[2] = { 1,G4 };
static const int kingMoves_F5_3[2] = { 1,E5 };
static const int kingMoves_F5_4[2] = { 1,G5 };
static const int kingMoves_F5_5[2] = { 1,E6 };
static const int kingMoves_F5_6[2] = { 1,F6 };
static const int kingMoves_F5_7[2] = { 1,G6 };
#ifndef _DEBUG
static const Direction kingMoveDirections_F5[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_F5_0)
 ,Direction(MD_DOWN     ,kingMoves_F5_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_F5_2)
 ,Direction(MD_LEFT     ,kingMoves_F5_3)
 ,Direction(MD_RIGHT    ,kingMoves_F5_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_F5_5)
 ,Direction(MD_UP       ,kingMoves_F5_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_F5_7)
};
#else
static const Direction kingMoveDirections_F5[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_F5_0,_T("king MD_DOWNDIAG1 from F5:E4"))
 ,Direction(MD_DOWN     ,kingMoves_F5_1,_T("king MD_DOWN from F5:F4"))
 ,Direction(MD_DOWNDIAG2,kingMoves_F5_2,_T("king MD_DOWNDIAG2 from F5:G4"))
 ,Direction(MD_LEFT     ,kingMoves_F5_3,_T("king MD_LEFT from F5:E5"))
 ,Direction(MD_RIGHT    ,kingMoves_F5_4,_T("king MD_RIGHT from F5:G5"))
 ,Direction(MD_UPDIAG2  ,kingMoves_F5_5,_T("king MD_UPDIAG2 from F5:E6"))
 ,Direction(MD_UP       ,kingMoves_F5_6,_T("king MD_UP from F5:F6"))
 ,Direction(MD_UPDIAG1  ,kingMoves_F5_7,_T("king MD_UPDIAG1 from F5:G6"))
};
#endif
static const DirectionArray kingMoves_F5(8,kingMoveDirections_F5);

static const int kingMoves_G5_0[2] = { 1,F4 };
static const int kingMoves_G5_1[2] = { 1,G4 };
static const int kingMoves_G5_2[2] = { 1,H4 };
static const int kingMoves_G5_3[2] = { 1,F5 };
static const int kingMoves_G5_4[2] = { 1,H5 };
static const int kingMoves_G5_5[2] = { 1,F6 };
static const int kingMoves_G5_6[2] = { 1,G6 };
static const int kingMoves_G5_7[2] = { 1,H6 };
#ifndef _DEBUG
static const Direction kingMoveDirections_G5[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_G5_0)
 ,Direction(MD_DOWN     ,kingMoves_G5_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_G5_2)
 ,Direction(MD_LEFT     ,kingMoves_G5_3)
 ,Direction(MD_RIGHT    ,kingMoves_G5_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_G5_5)
 ,Direction(MD_UP       ,kingMoves_G5_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_G5_7)
};
#else
static const Direction kingMoveDirections_G5[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_G5_0,_T("king MD_DOWNDIAG1 from G5:F4"))
 ,Direction(MD_DOWN     ,kingMoves_G5_1,_T("king MD_DOWN from G5:G4"))
 ,Direction(MD_DOWNDIAG2,kingMoves_G5_2,_T("king MD_DOWNDIAG2 from G5:H4"))
 ,Direction(MD_LEFT     ,kingMoves_G5_3,_T("king MD_LEFT from G5:F5"))
 ,Direction(MD_RIGHT    ,kingMoves_G5_4,_T("king MD_RIGHT from G5:H5"))
 ,Direction(MD_UPDIAG2  ,kingMoves_G5_5,_T("king MD_UPDIAG2 from G5:F6"))
 ,Direction(MD_UP       ,kingMoves_G5_6,_T("king MD_UP from G5:G6"))
 ,Direction(MD_UPDIAG1  ,kingMoves_G5_7,_T("king MD_UPDIAG1 from G5:H6"))
};
#endif
static const DirectionArray kingMoves_G5(8,kingMoveDirections_G5);

static const int kingMoves_H5_0[2] = { 1,G4 };
static const int kingMoves_H5_1[2] = { 1,H4 };
static const int kingMoves_H5_2[2] = { 1,G5 };
static const int kingMoves_H5_3[2] = { 1,G6 };
static const int kingMoves_H5_4[2] = { 1,H6 };
#ifndef _DEBUG
static const Direction kingMoveDirections_H5[5] = {
  Direction(MD_DOWNDIAG1,kingMoves_H5_0)
 ,Direction(MD_DOWN     ,kingMoves_H5_1)
 ,Direction(MD_LEFT     ,kingMoves_H5_2)
 ,Direction(MD_UPDIAG2  ,kingMoves_H5_3)
 ,Direction(MD_UP       ,kingMoves_H5_4)
};
#else
static const Direction kingMoveDirections_H5[5] = {
  Direction(MD_DOWNDIAG1,kingMoves_H5_0,_T("king MD_DOWNDIAG1 from H5:G4"))
 ,Direction(MD_DOWN     ,kingMoves_H5_1,_T("king MD_DOWN from H5:H4"))
 ,Direction(MD_LEFT     ,kingMoves_H5_2,_T("king MD_LEFT from H5:G5"))
 ,Direction(MD_UPDIAG2  ,kingMoves_H5_3,_T("king MD_UPDIAG2 from H5:G6"))
 ,Direction(MD_UP       ,kingMoves_H5_4,_T("king MD_UP from H5:H6"))
};
#endif
static const DirectionArray kingMoves_H5(5,kingMoveDirections_H5);

static const int kingMoves_A6_0[2] = { 1,A5 };
static const int kingMoves_A6_1[2] = { 1,B5 };
static const int kingMoves_A6_2[2] = { 1,B6 };
static const int kingMoves_A6_3[2] = { 1,A7 };
static const int kingMoves_A6_4[2] = { 1,B7 };
#ifndef _DEBUG
static const Direction kingMoveDirections_A6[5] = {
  Direction(MD_DOWN     ,kingMoves_A6_0)
 ,Direction(MD_DOWNDIAG2,kingMoves_A6_1)
 ,Direction(MD_RIGHT    ,kingMoves_A6_2)
 ,Direction(MD_UP       ,kingMoves_A6_3)
 ,Direction(MD_UPDIAG1  ,kingMoves_A6_4)
};
#else
static const Direction kingMoveDirections_A6[5] = {
  Direction(MD_DOWN     ,kingMoves_A6_0,_T("king MD_DOWN from A6:A5"))
 ,Direction(MD_DOWNDIAG2,kingMoves_A6_1,_T("king MD_DOWNDIAG2 from A6:B5"))
 ,Direction(MD_RIGHT    ,kingMoves_A6_2,_T("king MD_RIGHT from A6:B6"))
 ,Direction(MD_UP       ,kingMoves_A6_3,_T("king MD_UP from A6:A7"))
 ,Direction(MD_UPDIAG1  ,kingMoves_A6_4,_T("king MD_UPDIAG1 from A6:B7"))
};
#endif
static const DirectionArray kingMoves_A6(5,kingMoveDirections_A6);

static const int kingMoves_B6_0[2] = { 1,A5 };
static const int kingMoves_B6_1[2] = { 1,B5 };
static const int kingMoves_B6_2[2] = { 1,C5 };
static const int kingMoves_B6_3[2] = { 1,A6 };
static const int kingMoves_B6_4[2] = { 1,C6 };
static const int kingMoves_B6_5[2] = { 1,A7 };
static const int kingMoves_B6_6[2] = { 1,B7 };
static const int kingMoves_B6_7[2] = { 1,C7 };
#ifndef _DEBUG
static const Direction kingMoveDirections_B6[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_B6_0)
 ,Direction(MD_DOWN     ,kingMoves_B6_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_B6_2)
 ,Direction(MD_LEFT     ,kingMoves_B6_3)
 ,Direction(MD_RIGHT    ,kingMoves_B6_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_B6_5)
 ,Direction(MD_UP       ,kingMoves_B6_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_B6_7)
};
#else
static const Direction kingMoveDirections_B6[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_B6_0,_T("king MD_DOWNDIAG1 from B6:A5"))
 ,Direction(MD_DOWN     ,kingMoves_B6_1,_T("king MD_DOWN from B6:B5"))
 ,Direction(MD_DOWNDIAG2,kingMoves_B6_2,_T("king MD_DOWNDIAG2 from B6:C5"))
 ,Direction(MD_LEFT     ,kingMoves_B6_3,_T("king MD_LEFT from B6:A6"))
 ,Direction(MD_RIGHT    ,kingMoves_B6_4,_T("king MD_RIGHT from B6:C6"))
 ,Direction(MD_UPDIAG2  ,kingMoves_B6_5,_T("king MD_UPDIAG2 from B6:A7"))
 ,Direction(MD_UP       ,kingMoves_B6_6,_T("king MD_UP from B6:B7"))
 ,Direction(MD_UPDIAG1  ,kingMoves_B6_7,_T("king MD_UPDIAG1 from B6:C7"))
};
#endif
static const DirectionArray kingMoves_B6(8,kingMoveDirections_B6);

static const int kingMoves_C6_0[2] = { 1,B5 };
static const int kingMoves_C6_1[2] = { 1,C5 };
static const int kingMoves_C6_2[2] = { 1,D5 };
static const int kingMoves_C6_3[2] = { 1,B6 };
static const int kingMoves_C6_4[2] = { 1,D6 };
static const int kingMoves_C6_5[2] = { 1,B7 };
static const int kingMoves_C6_6[2] = { 1,C7 };
static const int kingMoves_C6_7[2] = { 1,D7 };
#ifndef _DEBUG
static const Direction kingMoveDirections_C6[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_C6_0)
 ,Direction(MD_DOWN     ,kingMoves_C6_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_C6_2)
 ,Direction(MD_LEFT     ,kingMoves_C6_3)
 ,Direction(MD_RIGHT    ,kingMoves_C6_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_C6_5)
 ,Direction(MD_UP       ,kingMoves_C6_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_C6_7)
};
#else
static const Direction kingMoveDirections_C6[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_C6_0,_T("king MD_DOWNDIAG1 from C6:B5"))
 ,Direction(MD_DOWN     ,kingMoves_C6_1,_T("king MD_DOWN from C6:C5"))
 ,Direction(MD_DOWNDIAG2,kingMoves_C6_2,_T("king MD_DOWNDIAG2 from C6:D5"))
 ,Direction(MD_LEFT     ,kingMoves_C6_3,_T("king MD_LEFT from C6:B6"))
 ,Direction(MD_RIGHT    ,kingMoves_C6_4,_T("king MD_RIGHT from C6:D6"))
 ,Direction(MD_UPDIAG2  ,kingMoves_C6_5,_T("king MD_UPDIAG2 from C6:B7"))
 ,Direction(MD_UP       ,kingMoves_C6_6,_T("king MD_UP from C6:C7"))
 ,Direction(MD_UPDIAG1  ,kingMoves_C6_7,_T("king MD_UPDIAG1 from C6:D7"))
};
#endif
static const DirectionArray kingMoves_C6(8,kingMoveDirections_C6);

static const int kingMoves_D6_0[2] = { 1,C5 };
static const int kingMoves_D6_1[2] = { 1,D5 };
static const int kingMoves_D6_2[2] = { 1,E5 };
static const int kingMoves_D6_3[2] = { 1,C6 };
static const int kingMoves_D6_4[2] = { 1,E6 };
static const int kingMoves_D6_5[2] = { 1,C7 };
static const int kingMoves_D6_6[2] = { 1,D7 };
static const int kingMoves_D6_7[2] = { 1,E7 };
#ifndef _DEBUG
static const Direction kingMoveDirections_D6[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_D6_0)
 ,Direction(MD_DOWN     ,kingMoves_D6_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_D6_2)
 ,Direction(MD_LEFT     ,kingMoves_D6_3)
 ,Direction(MD_RIGHT    ,kingMoves_D6_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_D6_5)
 ,Direction(MD_UP       ,kingMoves_D6_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_D6_7)
};
#else
static const Direction kingMoveDirections_D6[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_D6_0,_T("king MD_DOWNDIAG1 from D6:C5"))
 ,Direction(MD_DOWN     ,kingMoves_D6_1,_T("king MD_DOWN from D6:D5"))
 ,Direction(MD_DOWNDIAG2,kingMoves_D6_2,_T("king MD_DOWNDIAG2 from D6:E5"))
 ,Direction(MD_LEFT     ,kingMoves_D6_3,_T("king MD_LEFT from D6:C6"))
 ,Direction(MD_RIGHT    ,kingMoves_D6_4,_T("king MD_RIGHT from D6:E6"))
 ,Direction(MD_UPDIAG2  ,kingMoves_D6_5,_T("king MD_UPDIAG2 from D6:C7"))
 ,Direction(MD_UP       ,kingMoves_D6_6,_T("king MD_UP from D6:D7"))
 ,Direction(MD_UPDIAG1  ,kingMoves_D6_7,_T("king MD_UPDIAG1 from D6:E7"))
};
#endif
static const DirectionArray kingMoves_D6(8,kingMoveDirections_D6);

static const int kingMoves_E6_0[2] = { 1,D5 };
static const int kingMoves_E6_1[2] = { 1,E5 };
static const int kingMoves_E6_2[2] = { 1,F5 };
static const int kingMoves_E6_3[2] = { 1,D6 };
static const int kingMoves_E6_4[2] = { 1,F6 };
static const int kingMoves_E6_5[2] = { 1,D7 };
static const int kingMoves_E6_6[2] = { 1,E7 };
static const int kingMoves_E6_7[2] = { 1,F7 };
#ifndef _DEBUG
static const Direction kingMoveDirections_E6[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_E6_0)
 ,Direction(MD_DOWN     ,kingMoves_E6_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_E6_2)
 ,Direction(MD_LEFT     ,kingMoves_E6_3)
 ,Direction(MD_RIGHT    ,kingMoves_E6_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_E6_5)
 ,Direction(MD_UP       ,kingMoves_E6_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_E6_7)
};
#else
static const Direction kingMoveDirections_E6[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_E6_0,_T("king MD_DOWNDIAG1 from E6:D5"))
 ,Direction(MD_DOWN     ,kingMoves_E6_1,_T("king MD_DOWN from E6:E5"))
 ,Direction(MD_DOWNDIAG2,kingMoves_E6_2,_T("king MD_DOWNDIAG2 from E6:F5"))
 ,Direction(MD_LEFT     ,kingMoves_E6_3,_T("king MD_LEFT from E6:D6"))
 ,Direction(MD_RIGHT    ,kingMoves_E6_4,_T("king MD_RIGHT from E6:F6"))
 ,Direction(MD_UPDIAG2  ,kingMoves_E6_5,_T("king MD_UPDIAG2 from E6:D7"))
 ,Direction(MD_UP       ,kingMoves_E6_6,_T("king MD_UP from E6:E7"))
 ,Direction(MD_UPDIAG1  ,kingMoves_E6_7,_T("king MD_UPDIAG1 from E6:F7"))
};
#endif
static const DirectionArray kingMoves_E6(8,kingMoveDirections_E6);

static const int kingMoves_F6_0[2] = { 1,E5 };
static const int kingMoves_F6_1[2] = { 1,F5 };
static const int kingMoves_F6_2[2] = { 1,G5 };
static const int kingMoves_F6_3[2] = { 1,E6 };
static const int kingMoves_F6_4[2] = { 1,G6 };
static const int kingMoves_F6_5[2] = { 1,E7 };
static const int kingMoves_F6_6[2] = { 1,F7 };
static const int kingMoves_F6_7[2] = { 1,G7 };
#ifndef _DEBUG
static const Direction kingMoveDirections_F6[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_F6_0)
 ,Direction(MD_DOWN     ,kingMoves_F6_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_F6_2)
 ,Direction(MD_LEFT     ,kingMoves_F6_3)
 ,Direction(MD_RIGHT    ,kingMoves_F6_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_F6_5)
 ,Direction(MD_UP       ,kingMoves_F6_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_F6_7)
};
#else
static const Direction kingMoveDirections_F6[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_F6_0,_T("king MD_DOWNDIAG1 from F6:E5"))
 ,Direction(MD_DOWN     ,kingMoves_F6_1,_T("king MD_DOWN from F6:F5"))
 ,Direction(MD_DOWNDIAG2,kingMoves_F6_2,_T("king MD_DOWNDIAG2 from F6:G5"))
 ,Direction(MD_LEFT     ,kingMoves_F6_3,_T("king MD_LEFT from F6:E6"))
 ,Direction(MD_RIGHT    ,kingMoves_F6_4,_T("king MD_RIGHT from F6:G6"))
 ,Direction(MD_UPDIAG2  ,kingMoves_F6_5,_T("king MD_UPDIAG2 from F6:E7"))
 ,Direction(MD_UP       ,kingMoves_F6_6,_T("king MD_UP from F6:F7"))
 ,Direction(MD_UPDIAG1  ,kingMoves_F6_7,_T("king MD_UPDIAG1 from F6:G7"))
};
#endif
static const DirectionArray kingMoves_F6(8,kingMoveDirections_F6);

static const int kingMoves_G6_0[2] = { 1,F5 };
static const int kingMoves_G6_1[2] = { 1,G5 };
static const int kingMoves_G6_2[2] = { 1,H5 };
static const int kingMoves_G6_3[2] = { 1,F6 };
static const int kingMoves_G6_4[2] = { 1,H6 };
static const int kingMoves_G6_5[2] = { 1,F7 };
static const int kingMoves_G6_6[2] = { 1,G7 };
static const int kingMoves_G6_7[2] = { 1,H7 };
#ifndef _DEBUG
static const Direction kingMoveDirections_G6[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_G6_0)
 ,Direction(MD_DOWN     ,kingMoves_G6_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_G6_2)
 ,Direction(MD_LEFT     ,kingMoves_G6_3)
 ,Direction(MD_RIGHT    ,kingMoves_G6_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_G6_5)
 ,Direction(MD_UP       ,kingMoves_G6_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_G6_7)
};
#else
static const Direction kingMoveDirections_G6[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_G6_0,_T("king MD_DOWNDIAG1 from G6:F5"))
 ,Direction(MD_DOWN     ,kingMoves_G6_1,_T("king MD_DOWN from G6:G5"))
 ,Direction(MD_DOWNDIAG2,kingMoves_G6_2,_T("king MD_DOWNDIAG2 from G6:H5"))
 ,Direction(MD_LEFT     ,kingMoves_G6_3,_T("king MD_LEFT from G6:F6"))
 ,Direction(MD_RIGHT    ,kingMoves_G6_4,_T("king MD_RIGHT from G6:H6"))
 ,Direction(MD_UPDIAG2  ,kingMoves_G6_5,_T("king MD_UPDIAG2 from G6:F7"))
 ,Direction(MD_UP       ,kingMoves_G6_6,_T("king MD_UP from G6:G7"))
 ,Direction(MD_UPDIAG1  ,kingMoves_G6_7,_T("king MD_UPDIAG1 from G6:H7"))
};
#endif
static const DirectionArray kingMoves_G6(8,kingMoveDirections_G6);

static const int kingMoves_H6_0[2] = { 1,G5 };
static const int kingMoves_H6_1[2] = { 1,H5 };
static const int kingMoves_H6_2[2] = { 1,G6 };
static const int kingMoves_H6_3[2] = { 1,G7 };
static const int kingMoves_H6_4[2] = { 1,H7 };
#ifndef _DEBUG
static const Direction kingMoveDirections_H6[5] = {
  Direction(MD_DOWNDIAG1,kingMoves_H6_0)
 ,Direction(MD_DOWN     ,kingMoves_H6_1)
 ,Direction(MD_LEFT     ,kingMoves_H6_2)
 ,Direction(MD_UPDIAG2  ,kingMoves_H6_3)
 ,Direction(MD_UP       ,kingMoves_H6_4)
};
#else
static const Direction kingMoveDirections_H6[5] = {
  Direction(MD_DOWNDIAG1,kingMoves_H6_0,_T("king MD_DOWNDIAG1 from H6:G5"))
 ,Direction(MD_DOWN     ,kingMoves_H6_1,_T("king MD_DOWN from H6:H5"))
 ,Direction(MD_LEFT     ,kingMoves_H6_2,_T("king MD_LEFT from H6:G6"))
 ,Direction(MD_UPDIAG2  ,kingMoves_H6_3,_T("king MD_UPDIAG2 from H6:G7"))
 ,Direction(MD_UP       ,kingMoves_H6_4,_T("king MD_UP from H6:H7"))
};
#endif
static const DirectionArray kingMoves_H6(5,kingMoveDirections_H6);

static const int kingMoves_A7_0[2] = { 1,A6 };
static const int kingMoves_A7_1[2] = { 1,B6 };
static const int kingMoves_A7_2[2] = { 1,B7 };
static const int kingMoves_A7_3[2] = { 1,A8 };
static const int kingMoves_A7_4[2] = { 1,B8 };
#ifndef _DEBUG
static const Direction kingMoveDirections_A7[5] = {
  Direction(MD_DOWN     ,kingMoves_A7_0)
 ,Direction(MD_DOWNDIAG2,kingMoves_A7_1)
 ,Direction(MD_RIGHT    ,kingMoves_A7_2)
 ,Direction(MD_UP       ,kingMoves_A7_3)
 ,Direction(MD_UPDIAG1  ,kingMoves_A7_4)
};
#else
static const Direction kingMoveDirections_A7[5] = {
  Direction(MD_DOWN     ,kingMoves_A7_0,_T("king MD_DOWN from A7:A6"))
 ,Direction(MD_DOWNDIAG2,kingMoves_A7_1,_T("king MD_DOWNDIAG2 from A7:B6"))
 ,Direction(MD_RIGHT    ,kingMoves_A7_2,_T("king MD_RIGHT from A7:B7"))
 ,Direction(MD_UP       ,kingMoves_A7_3,_T("king MD_UP from A7:A8"))
 ,Direction(MD_UPDIAG1  ,kingMoves_A7_4,_T("king MD_UPDIAG1 from A7:B8"))
};
#endif
static const DirectionArray kingMoves_A7(5,kingMoveDirections_A7);

static const int kingMoves_B7_0[2] = { 1,A6 };
static const int kingMoves_B7_1[2] = { 1,B6 };
static const int kingMoves_B7_2[2] = { 1,C6 };
static const int kingMoves_B7_3[2] = { 1,A7 };
static const int kingMoves_B7_4[2] = { 1,C7 };
static const int kingMoves_B7_5[2] = { 1,A8 };
static const int kingMoves_B7_6[2] = { 1,B8 };
static const int kingMoves_B7_7[2] = { 1,C8 };
#ifndef _DEBUG
static const Direction kingMoveDirections_B7[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_B7_0)
 ,Direction(MD_DOWN     ,kingMoves_B7_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_B7_2)
 ,Direction(MD_LEFT     ,kingMoves_B7_3)
 ,Direction(MD_RIGHT    ,kingMoves_B7_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_B7_5)
 ,Direction(MD_UP       ,kingMoves_B7_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_B7_7)
};
#else
static const Direction kingMoveDirections_B7[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_B7_0,_T("king MD_DOWNDIAG1 from B7:A6"))
 ,Direction(MD_DOWN     ,kingMoves_B7_1,_T("king MD_DOWN from B7:B6"))
 ,Direction(MD_DOWNDIAG2,kingMoves_B7_2,_T("king MD_DOWNDIAG2 from B7:C6"))
 ,Direction(MD_LEFT     ,kingMoves_B7_3,_T("king MD_LEFT from B7:A7"))
 ,Direction(MD_RIGHT    ,kingMoves_B7_4,_T("king MD_RIGHT from B7:C7"))
 ,Direction(MD_UPDIAG2  ,kingMoves_B7_5,_T("king MD_UPDIAG2 from B7:A8"))
 ,Direction(MD_UP       ,kingMoves_B7_6,_T("king MD_UP from B7:B8"))
 ,Direction(MD_UPDIAG1  ,kingMoves_B7_7,_T("king MD_UPDIAG1 from B7:C8"))
};
#endif
static const DirectionArray kingMoves_B7(8,kingMoveDirections_B7);

static const int kingMoves_C7_0[2] = { 1,B6 };
static const int kingMoves_C7_1[2] = { 1,C6 };
static const int kingMoves_C7_2[2] = { 1,D6 };
static const int kingMoves_C7_3[2] = { 1,B7 };
static const int kingMoves_C7_4[2] = { 1,D7 };
static const int kingMoves_C7_5[2] = { 1,B8 };
static const int kingMoves_C7_6[2] = { 1,C8 };
static const int kingMoves_C7_7[2] = { 1,D8 };
#ifndef _DEBUG
static const Direction kingMoveDirections_C7[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_C7_0)
 ,Direction(MD_DOWN     ,kingMoves_C7_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_C7_2)
 ,Direction(MD_LEFT     ,kingMoves_C7_3)
 ,Direction(MD_RIGHT    ,kingMoves_C7_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_C7_5)
 ,Direction(MD_UP       ,kingMoves_C7_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_C7_7)
};
#else
static const Direction kingMoveDirections_C7[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_C7_0,_T("king MD_DOWNDIAG1 from C7:B6"))
 ,Direction(MD_DOWN     ,kingMoves_C7_1,_T("king MD_DOWN from C7:C6"))
 ,Direction(MD_DOWNDIAG2,kingMoves_C7_2,_T("king MD_DOWNDIAG2 from C7:D6"))
 ,Direction(MD_LEFT     ,kingMoves_C7_3,_T("king MD_LEFT from C7:B7"))
 ,Direction(MD_RIGHT    ,kingMoves_C7_4,_T("king MD_RIGHT from C7:D7"))
 ,Direction(MD_UPDIAG2  ,kingMoves_C7_5,_T("king MD_UPDIAG2 from C7:B8"))
 ,Direction(MD_UP       ,kingMoves_C7_6,_T("king MD_UP from C7:C8"))
 ,Direction(MD_UPDIAG1  ,kingMoves_C7_7,_T("king MD_UPDIAG1 from C7:D8"))
};
#endif
static const DirectionArray kingMoves_C7(8,kingMoveDirections_C7);

static const int kingMoves_D7_0[2] = { 1,C6 };
static const int kingMoves_D7_1[2] = { 1,D6 };
static const int kingMoves_D7_2[2] = { 1,E6 };
static const int kingMoves_D7_3[2] = { 1,C7 };
static const int kingMoves_D7_4[2] = { 1,E7 };
static const int kingMoves_D7_5[2] = { 1,C8 };
static const int kingMoves_D7_6[2] = { 1,D8 };
static const int kingMoves_D7_7[2] = { 1,E8 };
#ifndef _DEBUG
static const Direction kingMoveDirections_D7[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_D7_0)
 ,Direction(MD_DOWN     ,kingMoves_D7_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_D7_2)
 ,Direction(MD_LEFT     ,kingMoves_D7_3)
 ,Direction(MD_RIGHT    ,kingMoves_D7_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_D7_5)
 ,Direction(MD_UP       ,kingMoves_D7_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_D7_7)
};
#else
static const Direction kingMoveDirections_D7[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_D7_0,_T("king MD_DOWNDIAG1 from D7:C6"))
 ,Direction(MD_DOWN     ,kingMoves_D7_1,_T("king MD_DOWN from D7:D6"))
 ,Direction(MD_DOWNDIAG2,kingMoves_D7_2,_T("king MD_DOWNDIAG2 from D7:E6"))
 ,Direction(MD_LEFT     ,kingMoves_D7_3,_T("king MD_LEFT from D7:C7"))
 ,Direction(MD_RIGHT    ,kingMoves_D7_4,_T("king MD_RIGHT from D7:E7"))
 ,Direction(MD_UPDIAG2  ,kingMoves_D7_5,_T("king MD_UPDIAG2 from D7:C8"))
 ,Direction(MD_UP       ,kingMoves_D7_6,_T("king MD_UP from D7:D8"))
 ,Direction(MD_UPDIAG1  ,kingMoves_D7_7,_T("king MD_UPDIAG1 from D7:E8"))
};
#endif
static const DirectionArray kingMoves_D7(8,kingMoveDirections_D7);

static const int kingMoves_E7_0[2] = { 1,D6 };
static const int kingMoves_E7_1[2] = { 1,E6 };
static const int kingMoves_E7_2[2] = { 1,F6 };
static const int kingMoves_E7_3[2] = { 1,D7 };
static const int kingMoves_E7_4[2] = { 1,F7 };
static const int kingMoves_E7_5[2] = { 1,D8 };
static const int kingMoves_E7_6[2] = { 1,E8 };
static const int kingMoves_E7_7[2] = { 1,F8 };
#ifndef _DEBUG
static const Direction kingMoveDirections_E7[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_E7_0)
 ,Direction(MD_DOWN     ,kingMoves_E7_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_E7_2)
 ,Direction(MD_LEFT     ,kingMoves_E7_3)
 ,Direction(MD_RIGHT    ,kingMoves_E7_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_E7_5)
 ,Direction(MD_UP       ,kingMoves_E7_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_E7_7)
};
#else
static const Direction kingMoveDirections_E7[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_E7_0,_T("king MD_DOWNDIAG1 from E7:D6"))
 ,Direction(MD_DOWN     ,kingMoves_E7_1,_T("king MD_DOWN from E7:E6"))
 ,Direction(MD_DOWNDIAG2,kingMoves_E7_2,_T("king MD_DOWNDIAG2 from E7:F6"))
 ,Direction(MD_LEFT     ,kingMoves_E7_3,_T("king MD_LEFT from E7:D7"))
 ,Direction(MD_RIGHT    ,kingMoves_E7_4,_T("king MD_RIGHT from E7:F7"))
 ,Direction(MD_UPDIAG2  ,kingMoves_E7_5,_T("king MD_UPDIAG2 from E7:D8"))
 ,Direction(MD_UP       ,kingMoves_E7_6,_T("king MD_UP from E7:E8"))
 ,Direction(MD_UPDIAG1  ,kingMoves_E7_7,_T("king MD_UPDIAG1 from E7:F8"))
};
#endif
static const DirectionArray kingMoves_E7(8,kingMoveDirections_E7);

static const int kingMoves_F7_0[2] = { 1,E6 };
static const int kingMoves_F7_1[2] = { 1,F6 };
static const int kingMoves_F7_2[2] = { 1,G6 };
static const int kingMoves_F7_3[2] = { 1,E7 };
static const int kingMoves_F7_4[2] = { 1,G7 };
static const int kingMoves_F7_5[2] = { 1,E8 };
static const int kingMoves_F7_6[2] = { 1,F8 };
static const int kingMoves_F7_7[2] = { 1,G8 };
#ifndef _DEBUG
static const Direction kingMoveDirections_F7[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_F7_0)
 ,Direction(MD_DOWN     ,kingMoves_F7_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_F7_2)
 ,Direction(MD_LEFT     ,kingMoves_F7_3)
 ,Direction(MD_RIGHT    ,kingMoves_F7_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_F7_5)
 ,Direction(MD_UP       ,kingMoves_F7_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_F7_7)
};
#else
static const Direction kingMoveDirections_F7[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_F7_0,_T("king MD_DOWNDIAG1 from F7:E6"))
 ,Direction(MD_DOWN     ,kingMoves_F7_1,_T("king MD_DOWN from F7:F6"))
 ,Direction(MD_DOWNDIAG2,kingMoves_F7_2,_T("king MD_DOWNDIAG2 from F7:G6"))
 ,Direction(MD_LEFT     ,kingMoves_F7_3,_T("king MD_LEFT from F7:E7"))
 ,Direction(MD_RIGHT    ,kingMoves_F7_4,_T("king MD_RIGHT from F7:G7"))
 ,Direction(MD_UPDIAG2  ,kingMoves_F7_5,_T("king MD_UPDIAG2 from F7:E8"))
 ,Direction(MD_UP       ,kingMoves_F7_6,_T("king MD_UP from F7:F8"))
 ,Direction(MD_UPDIAG1  ,kingMoves_F7_7,_T("king MD_UPDIAG1 from F7:G8"))
};
#endif
static const DirectionArray kingMoves_F7(8,kingMoveDirections_F7);

static const int kingMoves_G7_0[2] = { 1,F6 };
static const int kingMoves_G7_1[2] = { 1,G6 };
static const int kingMoves_G7_2[2] = { 1,H6 };
static const int kingMoves_G7_3[2] = { 1,F7 };
static const int kingMoves_G7_4[2] = { 1,H7 };
static const int kingMoves_G7_5[2] = { 1,F8 };
static const int kingMoves_G7_6[2] = { 1,G8 };
static const int kingMoves_G7_7[2] = { 1,H8 };
#ifndef _DEBUG
static const Direction kingMoveDirections_G7[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_G7_0)
 ,Direction(MD_DOWN     ,kingMoves_G7_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_G7_2)
 ,Direction(MD_LEFT     ,kingMoves_G7_3)
 ,Direction(MD_RIGHT    ,kingMoves_G7_4)
 ,Direction(MD_UPDIAG2  ,kingMoves_G7_5)
 ,Direction(MD_UP       ,kingMoves_G7_6)
 ,Direction(MD_UPDIAG1  ,kingMoves_G7_7)
};
#else
static const Direction kingMoveDirections_G7[8] = {
  Direction(MD_DOWNDIAG1,kingMoves_G7_0,_T("king MD_DOWNDIAG1 from G7:F6"))
 ,Direction(MD_DOWN     ,kingMoves_G7_1,_T("king MD_DOWN from G7:G6"))
 ,Direction(MD_DOWNDIAG2,kingMoves_G7_2,_T("king MD_DOWNDIAG2 from G7:H6"))
 ,Direction(MD_LEFT     ,kingMoves_G7_3,_T("king MD_LEFT from G7:F7"))
 ,Direction(MD_RIGHT    ,kingMoves_G7_4,_T("king MD_RIGHT from G7:H7"))
 ,Direction(MD_UPDIAG2  ,kingMoves_G7_5,_T("king MD_UPDIAG2 from G7:F8"))
 ,Direction(MD_UP       ,kingMoves_G7_6,_T("king MD_UP from G7:G8"))
 ,Direction(MD_UPDIAG1  ,kingMoves_G7_7,_T("king MD_UPDIAG1 from G7:H8"))
};
#endif
static const DirectionArray kingMoves_G7(8,kingMoveDirections_G7);

static const int kingMoves_H7_0[2] = { 1,G6 };
static const int kingMoves_H7_1[2] = { 1,H6 };
static const int kingMoves_H7_2[2] = { 1,G7 };
static const int kingMoves_H7_3[2] = { 1,G8 };
static const int kingMoves_H7_4[2] = { 1,H8 };
#ifndef _DEBUG
static const Direction kingMoveDirections_H7[5] = {
  Direction(MD_DOWNDIAG1,kingMoves_H7_0)
 ,Direction(MD_DOWN     ,kingMoves_H7_1)
 ,Direction(MD_LEFT     ,kingMoves_H7_2)
 ,Direction(MD_UPDIAG2  ,kingMoves_H7_3)
 ,Direction(MD_UP       ,kingMoves_H7_4)
};
#else
static const Direction kingMoveDirections_H7[5] = {
  Direction(MD_DOWNDIAG1,kingMoves_H7_0,_T("king MD_DOWNDIAG1 from H7:G6"))
 ,Direction(MD_DOWN     ,kingMoves_H7_1,_T("king MD_DOWN from H7:H6"))
 ,Direction(MD_LEFT     ,kingMoves_H7_2,_T("king MD_LEFT from H7:G7"))
 ,Direction(MD_UPDIAG2  ,kingMoves_H7_3,_T("king MD_UPDIAG2 from H7:G8"))
 ,Direction(MD_UP       ,kingMoves_H7_4,_T("king MD_UP from H7:H8"))
};
#endif
static const DirectionArray kingMoves_H7(5,kingMoveDirections_H7);

static const int kingMoves_A8_0[2] = { 1,A7 };
static const int kingMoves_A8_1[2] = { 1,B7 };
static const int kingMoves_A8_2[2] = { 1,B8 };
#ifndef _DEBUG
static const Direction kingMoveDirections_A8[3] = {
  Direction(MD_DOWN     ,kingMoves_A8_0)
 ,Direction(MD_DOWNDIAG2,kingMoves_A8_1)
 ,Direction(MD_RIGHT    ,kingMoves_A8_2)
};
#else
static const Direction kingMoveDirections_A8[3] = {
  Direction(MD_DOWN     ,kingMoves_A8_0,_T("king MD_DOWN from A8:A7"))
 ,Direction(MD_DOWNDIAG2,kingMoves_A8_1,_T("king MD_DOWNDIAG2 from A8:B7"))
 ,Direction(MD_RIGHT    ,kingMoves_A8_2,_T("king MD_RIGHT from A8:B8"))
};
#endif
static const DirectionArray kingMoves_A8(3,kingMoveDirections_A8);

static const int kingMoves_B8_0[2] = { 1,A7 };
static const int kingMoves_B8_1[2] = { 1,B7 };
static const int kingMoves_B8_2[2] = { 1,C7 };
static const int kingMoves_B8_3[2] = { 1,A8 };
static const int kingMoves_B8_4[2] = { 1,C8 };
#ifndef _DEBUG
static const Direction kingMoveDirections_B8[5] = {
  Direction(MD_DOWNDIAG1,kingMoves_B8_0)
 ,Direction(MD_DOWN     ,kingMoves_B8_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_B8_2)
 ,Direction(MD_LEFT     ,kingMoves_B8_3)
 ,Direction(MD_RIGHT    ,kingMoves_B8_4)
};
#else
static const Direction kingMoveDirections_B8[5] = {
  Direction(MD_DOWNDIAG1,kingMoves_B8_0,_T("king MD_DOWNDIAG1 from B8:A7"))
 ,Direction(MD_DOWN     ,kingMoves_B8_1,_T("king MD_DOWN from B8:B7"))
 ,Direction(MD_DOWNDIAG2,kingMoves_B8_2,_T("king MD_DOWNDIAG2 from B8:C7"))
 ,Direction(MD_LEFT     ,kingMoves_B8_3,_T("king MD_LEFT from B8:A8"))
 ,Direction(MD_RIGHT    ,kingMoves_B8_4,_T("king MD_RIGHT from B8:C8"))
};
#endif
static const DirectionArray kingMoves_B8(5,kingMoveDirections_B8);

static const int kingMoves_C8_0[2] = { 1,B7 };
static const int kingMoves_C8_1[2] = { 1,C7 };
static const int kingMoves_C8_2[2] = { 1,D7 };
static const int kingMoves_C8_3[2] = { 1,B8 };
static const int kingMoves_C8_4[2] = { 1,D8 };
#ifndef _DEBUG
static const Direction kingMoveDirections_C8[5] = {
  Direction(MD_DOWNDIAG1,kingMoves_C8_0)
 ,Direction(MD_DOWN     ,kingMoves_C8_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_C8_2)
 ,Direction(MD_LEFT     ,kingMoves_C8_3)
 ,Direction(MD_RIGHT    ,kingMoves_C8_4)
};
#else
static const Direction kingMoveDirections_C8[5] = {
  Direction(MD_DOWNDIAG1,kingMoves_C8_0,_T("king MD_DOWNDIAG1 from C8:B7"))
 ,Direction(MD_DOWN     ,kingMoves_C8_1,_T("king MD_DOWN from C8:C7"))
 ,Direction(MD_DOWNDIAG2,kingMoves_C8_2,_T("king MD_DOWNDIAG2 from C8:D7"))
 ,Direction(MD_LEFT     ,kingMoves_C8_3,_T("king MD_LEFT from C8:B8"))
 ,Direction(MD_RIGHT    ,kingMoves_C8_4,_T("king MD_RIGHT from C8:D8"))
};
#endif
static const DirectionArray kingMoves_C8(5,kingMoveDirections_C8);

static const int kingMoves_D8_0[2] = { 1,C7 };
static const int kingMoves_D8_1[2] = { 1,D7 };
static const int kingMoves_D8_2[2] = { 1,E7 };
static const int kingMoves_D8_3[2] = { 1,C8 };
static const int kingMoves_D8_4[2] = { 1,E8 };
#ifndef _DEBUG
static const Direction kingMoveDirections_D8[5] = {
  Direction(MD_DOWNDIAG1,kingMoves_D8_0)
 ,Direction(MD_DOWN     ,kingMoves_D8_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_D8_2)
 ,Direction(MD_LEFT     ,kingMoves_D8_3)
 ,Direction(MD_RIGHT    ,kingMoves_D8_4)
};
#else
static const Direction kingMoveDirections_D8[5] = {
  Direction(MD_DOWNDIAG1,kingMoves_D8_0,_T("king MD_DOWNDIAG1 from D8:C7"))
 ,Direction(MD_DOWN     ,kingMoves_D8_1,_T("king MD_DOWN from D8:D7"))
 ,Direction(MD_DOWNDIAG2,kingMoves_D8_2,_T("king MD_DOWNDIAG2 from D8:E7"))
 ,Direction(MD_LEFT     ,kingMoves_D8_3,_T("king MD_LEFT from D8:C8"))
 ,Direction(MD_RIGHT    ,kingMoves_D8_4,_T("king MD_RIGHT from D8:E8"))
};
#endif
static const DirectionArray kingMoves_D8(5,kingMoveDirections_D8);

static const int kingMoves_E8_0[2] = { 1,D7 };
static const int kingMoves_E8_1[2] = { 1,E7 };
static const int kingMoves_E8_2[2] = { 1,F7 };
static const int kingMoves_E8_3[2] = { 1,D8 };
static const int kingMoves_E8_4[2] = { 1,F8 };
#ifndef _DEBUG
static const Direction kingMoveDirections_E8[5] = {
  Direction(MD_DOWNDIAG1,kingMoves_E8_0)
 ,Direction(MD_DOWN     ,kingMoves_E8_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_E8_2)
 ,Direction(MD_LEFT     ,kingMoves_E8_3)
 ,Direction(MD_RIGHT    ,kingMoves_E8_4)
};
#else
static const Direction kingMoveDirections_E8[5] = {
  Direction(MD_DOWNDIAG1,kingMoves_E8_0,_T("king MD_DOWNDIAG1 from E8:D7"))
 ,Direction(MD_DOWN     ,kingMoves_E8_1,_T("king MD_DOWN from E8:E7"))
 ,Direction(MD_DOWNDIAG2,kingMoves_E8_2,_T("king MD_DOWNDIAG2 from E8:F7"))
 ,Direction(MD_LEFT     ,kingMoves_E8_3,_T("king MD_LEFT from E8:D8"))
 ,Direction(MD_RIGHT    ,kingMoves_E8_4,_T("king MD_RIGHT from E8:F8"))
};
#endif
static const DirectionArray kingMoves_E8(5,kingMoveDirections_E8);

static const int kingMoves_F8_0[2] = { 1,E7 };
static const int kingMoves_F8_1[2] = { 1,F7 };
static const int kingMoves_F8_2[2] = { 1,G7 };
static const int kingMoves_F8_3[2] = { 1,E8 };
static const int kingMoves_F8_4[2] = { 1,G8 };
#ifndef _DEBUG
static const Direction kingMoveDirections_F8[5] = {
  Direction(MD_DOWNDIAG1,kingMoves_F8_0)
 ,Direction(MD_DOWN     ,kingMoves_F8_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_F8_2)
 ,Direction(MD_LEFT     ,kingMoves_F8_3)
 ,Direction(MD_RIGHT    ,kingMoves_F8_4)
};
#else
static const Direction kingMoveDirections_F8[5] = {
  Direction(MD_DOWNDIAG1,kingMoves_F8_0,_T("king MD_DOWNDIAG1 from F8:E7"))
 ,Direction(MD_DOWN     ,kingMoves_F8_1,_T("king MD_DOWN from F8:F7"))
 ,Direction(MD_DOWNDIAG2,kingMoves_F8_2,_T("king MD_DOWNDIAG2 from F8:G7"))
 ,Direction(MD_LEFT     ,kingMoves_F8_3,_T("king MD_LEFT from F8:E8"))
 ,Direction(MD_RIGHT    ,kingMoves_F8_4,_T("king MD_RIGHT from F8:G8"))
};
#endif
static const DirectionArray kingMoves_F8(5,kingMoveDirections_F8);

static const int kingMoves_G8_0[2] = { 1,F7 };
static const int kingMoves_G8_1[2] = { 1,G7 };
static const int kingMoves_G8_2[2] = { 1,H7 };
static const int kingMoves_G8_3[2] = { 1,F8 };
static const int kingMoves_G8_4[2] = { 1,H8 };
#ifndef _DEBUG
static const Direction kingMoveDirections_G8[5] = {
  Direction(MD_DOWNDIAG1,kingMoves_G8_0)
 ,Direction(MD_DOWN     ,kingMoves_G8_1)
 ,Direction(MD_DOWNDIAG2,kingMoves_G8_2)
 ,Direction(MD_LEFT     ,kingMoves_G8_3)
 ,Direction(MD_RIGHT    ,kingMoves_G8_4)
};
#else
static const Direction kingMoveDirections_G8[5] = {
  Direction(MD_DOWNDIAG1,kingMoves_G8_0,_T("king MD_DOWNDIAG1 from G8:F7"))
 ,Direction(MD_DOWN     ,kingMoves_G8_1,_T("king MD_DOWN from G8:G7"))
 ,Direction(MD_DOWNDIAG2,kingMoves_G8_2,_T("king MD_DOWNDIAG2 from G8:H7"))
 ,Direction(MD_LEFT     ,kingMoves_G8_3,_T("king MD_LEFT from G8:F8"))
 ,Direction(MD_RIGHT    ,kingMoves_G8_4,_T("king MD_RIGHT from G8:H8"))
};
#endif
static const DirectionArray kingMoves_G8(5,kingMoveDirections_G8);

static const int kingMoves_H8_0[2] = { 1,G7 };
static const int kingMoves_H8_1[2] = { 1,H7 };
static const int kingMoves_H8_2[2] = { 1,G8 };
#ifndef _DEBUG
static const Direction kingMoveDirections_H8[3] = {
  Direction(MD_DOWNDIAG1,kingMoves_H8_0)
 ,Direction(MD_DOWN     ,kingMoves_H8_1)
 ,Direction(MD_LEFT     ,kingMoves_H8_2)
};
#else
static const Direction kingMoveDirections_H8[3] = {
  Direction(MD_DOWNDIAG1,kingMoves_H8_0,_T("king MD_DOWNDIAG1 from H8:G7"))
 ,Direction(MD_DOWN     ,kingMoves_H8_1,_T("king MD_DOWN from H8:H7"))
 ,Direction(MD_LEFT     ,kingMoves_H8_2,_T("king MD_LEFT from H8:G8"))
};
#endif
static const DirectionArray kingMoves_H8(3,kingMoveDirections_H8);

static const int queenMoves_A1_0[8] = { 7,B1,C1,D1,E1,F1,G1,H1 };
static const int queenMoves_A1_1[8] = { 7,A2,A3,A4,A5,A6,A7,A8 };
static const int queenMoves_A1_2[8] = { 7,B2,C3,D4,E5,F6,G7,H8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_A1[3] = {
  Direction(MD_RIGHT    ,queenMoves_A1_0)
 ,Direction(MD_UP       ,queenMoves_A1_1)
 ,Direction(MD_UPDIAG1  ,queenMoves_A1_2)
};
#else
static const Direction queenMoveDirections_A1[3] = {
  Direction(MD_RIGHT    ,queenMoves_A1_0,_T("queen MD_RIGHT from A1:B1 C1 D1 E1 F1 G1 H1"))
 ,Direction(MD_UP       ,queenMoves_A1_1,_T("queen MD_UP from A1:A2 A3 A4 A5 A6 A7 A8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_A1_2,_T("queen MD_UPDIAG1 from A1:B2 C3 D4 E5 F6 G7 H8"))
};
#endif
static const DirectionArray queenMoves_A1(3,queenMoveDirections_A1);

static const int queenMoves_B1_0[2] = { 1,A1 };
static const int queenMoves_B1_1[7] = { 6,C1,D1,E1,F1,G1,H1 };
static const int queenMoves_B1_2[2] = { 1,A2 };
static const int queenMoves_B1_3[8] = { 7,B2,B3,B4,B5,B6,B7,B8 };
static const int queenMoves_B1_4[7] = { 6,C2,D3,E4,F5,G6,H7 };
#ifndef _DEBUG
static const Direction queenMoveDirections_B1[5] = {
  Direction(MD_LEFT     ,queenMoves_B1_0)
 ,Direction(MD_RIGHT    ,queenMoves_B1_1)
 ,Direction(MD_UPDIAG2  ,queenMoves_B1_2)
 ,Direction(MD_UP       ,queenMoves_B1_3)
 ,Direction(MD_UPDIAG1  ,queenMoves_B1_4)
};
#else
static const Direction queenMoveDirections_B1[5] = {
  Direction(MD_LEFT     ,queenMoves_B1_0,_T("queen MD_LEFT from B1:A1"))
 ,Direction(MD_RIGHT    ,queenMoves_B1_1,_T("queen MD_RIGHT from B1:C1 D1 E1 F1 G1 H1"))
 ,Direction(MD_UPDIAG2  ,queenMoves_B1_2,_T("queen MD_UPDIAG2 from B1:A2"))
 ,Direction(MD_UP       ,queenMoves_B1_3,_T("queen MD_UP from B1:B2 B3 B4 B5 B6 B7 B8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_B1_4,_T("queen MD_UPDIAG1 from B1:C2 D3 E4 F5 G6 H7"))
};
#endif
static const DirectionArray queenMoves_B1(5,queenMoveDirections_B1);

static const int queenMoves_C1_0[3] = { 2,B1,A1 };
static const int queenMoves_C1_1[6] = { 5,D1,E1,F1,G1,H1 };
static const int queenMoves_C1_2[3] = { 2,B2,A3 };
static const int queenMoves_C1_3[8] = { 7,C2,C3,C4,C5,C6,C7,C8 };
static const int queenMoves_C1_4[6] = { 5,D2,E3,F4,G5,H6 };
#ifndef _DEBUG
static const Direction queenMoveDirections_C1[5] = {
  Direction(MD_LEFT     ,queenMoves_C1_0)
 ,Direction(MD_RIGHT    ,queenMoves_C1_1)
 ,Direction(MD_UPDIAG2  ,queenMoves_C1_2)
 ,Direction(MD_UP       ,queenMoves_C1_3)
 ,Direction(MD_UPDIAG1  ,queenMoves_C1_4)
};
#else
static const Direction queenMoveDirections_C1[5] = {
  Direction(MD_LEFT     ,queenMoves_C1_0,_T("queen MD_LEFT from C1:B1 A1"))
 ,Direction(MD_RIGHT    ,queenMoves_C1_1,_T("queen MD_RIGHT from C1:D1 E1 F1 G1 H1"))
 ,Direction(MD_UPDIAG2  ,queenMoves_C1_2,_T("queen MD_UPDIAG2 from C1:B2 A3"))
 ,Direction(MD_UP       ,queenMoves_C1_3,_T("queen MD_UP from C1:C2 C3 C4 C5 C6 C7 C8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_C1_4,_T("queen MD_UPDIAG1 from C1:D2 E3 F4 G5 H6"))
};
#endif
static const DirectionArray queenMoves_C1(5,queenMoveDirections_C1);

static const int queenMoves_D1_0[4] = { 3,C1,B1,A1 };
static const int queenMoves_D1_1[5] = { 4,E1,F1,G1,H1 };
static const int queenMoves_D1_2[4] = { 3,C2,B3,A4 };
static const int queenMoves_D1_3[8] = { 7,D2,D3,D4,D5,D6,D7,D8 };
static const int queenMoves_D1_4[5] = { 4,E2,F3,G4,H5 };
#ifndef _DEBUG
static const Direction queenMoveDirections_D1[5] = {
  Direction(MD_LEFT     ,queenMoves_D1_0)
 ,Direction(MD_RIGHT    ,queenMoves_D1_1)
 ,Direction(MD_UPDIAG2  ,queenMoves_D1_2)
 ,Direction(MD_UP       ,queenMoves_D1_3)
 ,Direction(MD_UPDIAG1  ,queenMoves_D1_4)
};
#else
static const Direction queenMoveDirections_D1[5] = {
  Direction(MD_LEFT     ,queenMoves_D1_0,_T("queen MD_LEFT from D1:C1 B1 A1"))
 ,Direction(MD_RIGHT    ,queenMoves_D1_1,_T("queen MD_RIGHT from D1:E1 F1 G1 H1"))
 ,Direction(MD_UPDIAG2  ,queenMoves_D1_2,_T("queen MD_UPDIAG2 from D1:C2 B3 A4"))
 ,Direction(MD_UP       ,queenMoves_D1_3,_T("queen MD_UP from D1:D2 D3 D4 D5 D6 D7 D8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_D1_4,_T("queen MD_UPDIAG1 from D1:E2 F3 G4 H5"))
};
#endif
static const DirectionArray queenMoves_D1(5,queenMoveDirections_D1);

static const int queenMoves_E1_0[5] = { 4,D1,C1,B1,A1 };
static const int queenMoves_E1_1[4] = { 3,F1,G1,H1 };
static const int queenMoves_E1_2[5] = { 4,D2,C3,B4,A5 };
static const int queenMoves_E1_3[8] = { 7,E2,E3,E4,E5,E6,E7,E8 };
static const int queenMoves_E1_4[4] = { 3,F2,G3,H4 };
#ifndef _DEBUG
static const Direction queenMoveDirections_E1[5] = {
  Direction(MD_LEFT     ,queenMoves_E1_0)
 ,Direction(MD_RIGHT    ,queenMoves_E1_1)
 ,Direction(MD_UPDIAG2  ,queenMoves_E1_2)
 ,Direction(MD_UP       ,queenMoves_E1_3)
 ,Direction(MD_UPDIAG1  ,queenMoves_E1_4)
};
#else
static const Direction queenMoveDirections_E1[5] = {
  Direction(MD_LEFT     ,queenMoves_E1_0,_T("queen MD_LEFT from E1:D1 C1 B1 A1"))
 ,Direction(MD_RIGHT    ,queenMoves_E1_1,_T("queen MD_RIGHT from E1:F1 G1 H1"))
 ,Direction(MD_UPDIAG2  ,queenMoves_E1_2,_T("queen MD_UPDIAG2 from E1:D2 C3 B4 A5"))
 ,Direction(MD_UP       ,queenMoves_E1_3,_T("queen MD_UP from E1:E2 E3 E4 E5 E6 E7 E8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_E1_4,_T("queen MD_UPDIAG1 from E1:F2 G3 H4"))
};
#endif
static const DirectionArray queenMoves_E1(5,queenMoveDirections_E1);

static const int queenMoves_F1_0[6] = { 5,E1,D1,C1,B1,A1 };
static const int queenMoves_F1_1[3] = { 2,G1,H1 };
static const int queenMoves_F1_2[6] = { 5,E2,D3,C4,B5,A6 };
static const int queenMoves_F1_3[8] = { 7,F2,F3,F4,F5,F6,F7,F8 };
static const int queenMoves_F1_4[3] = { 2,G2,H3 };
#ifndef _DEBUG
static const Direction queenMoveDirections_F1[5] = {
  Direction(MD_LEFT     ,queenMoves_F1_0)
 ,Direction(MD_RIGHT    ,queenMoves_F1_1)
 ,Direction(MD_UPDIAG2  ,queenMoves_F1_2)
 ,Direction(MD_UP       ,queenMoves_F1_3)
 ,Direction(MD_UPDIAG1  ,queenMoves_F1_4)
};
#else
static const Direction queenMoveDirections_F1[5] = {
  Direction(MD_LEFT     ,queenMoves_F1_0,_T("queen MD_LEFT from F1:E1 D1 C1 B1 A1"))
 ,Direction(MD_RIGHT    ,queenMoves_F1_1,_T("queen MD_RIGHT from F1:G1 H1"))
 ,Direction(MD_UPDIAG2  ,queenMoves_F1_2,_T("queen MD_UPDIAG2 from F1:E2 D3 C4 B5 A6"))
 ,Direction(MD_UP       ,queenMoves_F1_3,_T("queen MD_UP from F1:F2 F3 F4 F5 F6 F7 F8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_F1_4,_T("queen MD_UPDIAG1 from F1:G2 H3"))
};
#endif
static const DirectionArray queenMoves_F1(5,queenMoveDirections_F1);

static const int queenMoves_G1_0[7] = { 6,F1,E1,D1,C1,B1,A1 };
static const int queenMoves_G1_1[2] = { 1,H1 };
static const int queenMoves_G1_2[7] = { 6,F2,E3,D4,C5,B6,A7 };
static const int queenMoves_G1_3[8] = { 7,G2,G3,G4,G5,G6,G7,G8 };
static const int queenMoves_G1_4[2] = { 1,H2 };
#ifndef _DEBUG
static const Direction queenMoveDirections_G1[5] = {
  Direction(MD_LEFT     ,queenMoves_G1_0)
 ,Direction(MD_RIGHT    ,queenMoves_G1_1)
 ,Direction(MD_UPDIAG2  ,queenMoves_G1_2)
 ,Direction(MD_UP       ,queenMoves_G1_3)
 ,Direction(MD_UPDIAG1  ,queenMoves_G1_4)
};
#else
static const Direction queenMoveDirections_G1[5] = {
  Direction(MD_LEFT     ,queenMoves_G1_0,_T("queen MD_LEFT from G1:F1 E1 D1 C1 B1 A1"))
 ,Direction(MD_RIGHT    ,queenMoves_G1_1,_T("queen MD_RIGHT from G1:H1"))
 ,Direction(MD_UPDIAG2  ,queenMoves_G1_2,_T("queen MD_UPDIAG2 from G1:F2 E3 D4 C5 B6 A7"))
 ,Direction(MD_UP       ,queenMoves_G1_3,_T("queen MD_UP from G1:G2 G3 G4 G5 G6 G7 G8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_G1_4,_T("queen MD_UPDIAG1 from G1:H2"))
};
#endif
static const DirectionArray queenMoves_G1(5,queenMoveDirections_G1);

static const int queenMoves_H1_0[8] = { 7,G1,F1,E1,D1,C1,B1,A1 };
static const int queenMoves_H1_1[8] = { 7,G2,F3,E4,D5,C6,B7,A8 };
static const int queenMoves_H1_2[8] = { 7,H2,H3,H4,H5,H6,H7,H8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_H1[3] = {
  Direction(MD_LEFT     ,queenMoves_H1_0)
 ,Direction(MD_UPDIAG2  ,queenMoves_H1_1)
 ,Direction(MD_UP       ,queenMoves_H1_2)
};
#else
static const Direction queenMoveDirections_H1[3] = {
  Direction(MD_LEFT     ,queenMoves_H1_0,_T("queen MD_LEFT from H1:G1 F1 E1 D1 C1 B1 A1"))
 ,Direction(MD_UPDIAG2  ,queenMoves_H1_1,_T("queen MD_UPDIAG2 from H1:G2 F3 E4 D5 C6 B7 A8"))
 ,Direction(MD_UP       ,queenMoves_H1_2,_T("queen MD_UP from H1:H2 H3 H4 H5 H6 H7 H8"))
};
#endif
static const DirectionArray queenMoves_H1(3,queenMoveDirections_H1);

static const int queenMoves_A2_0[2] = { 1,A1 };
static const int queenMoves_A2_1[2] = { 1,B1 };
static const int queenMoves_A2_2[8] = { 7,B2,C2,D2,E2,F2,G2,H2 };
static const int queenMoves_A2_3[7] = { 6,A3,A4,A5,A6,A7,A8 };
static const int queenMoves_A2_4[7] = { 6,B3,C4,D5,E6,F7,G8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_A2[5] = {
  Direction(MD_DOWN     ,queenMoves_A2_0)
 ,Direction(MD_DOWNDIAG2,queenMoves_A2_1)
 ,Direction(MD_RIGHT    ,queenMoves_A2_2)
 ,Direction(MD_UP       ,queenMoves_A2_3)
 ,Direction(MD_UPDIAG1  ,queenMoves_A2_4)
};
#else
static const Direction queenMoveDirections_A2[5] = {
  Direction(MD_DOWN     ,queenMoves_A2_0,_T("queen MD_DOWN from A2:A1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_A2_1,_T("queen MD_DOWNDIAG2 from A2:B1"))
 ,Direction(MD_RIGHT    ,queenMoves_A2_2,_T("queen MD_RIGHT from A2:B2 C2 D2 E2 F2 G2 H2"))
 ,Direction(MD_UP       ,queenMoves_A2_3,_T("queen MD_UP from A2:A3 A4 A5 A6 A7 A8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_A2_4,_T("queen MD_UPDIAG1 from A2:B3 C4 D5 E6 F7 G8"))
};
#endif
static const DirectionArray queenMoves_A2(5,queenMoveDirections_A2);

static const int queenMoves_B2_0[2] = { 1,A1 };
static const int queenMoves_B2_1[2] = { 1,B1 };
static const int queenMoves_B2_2[2] = { 1,C1 };
static const int queenMoves_B2_3[2] = { 1,A2 };
static const int queenMoves_B2_4[7] = { 6,C2,D2,E2,F2,G2,H2 };
static const int queenMoves_B2_5[2] = { 1,A3 };
static const int queenMoves_B2_6[7] = { 6,B3,B4,B5,B6,B7,B8 };
static const int queenMoves_B2_7[7] = { 6,C3,D4,E5,F6,G7,H8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_B2[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_B2_0)
 ,Direction(MD_DOWN     ,queenMoves_B2_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_B2_2)
 ,Direction(MD_LEFT     ,queenMoves_B2_3)
 ,Direction(MD_RIGHT    ,queenMoves_B2_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_B2_5)
 ,Direction(MD_UP       ,queenMoves_B2_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_B2_7)
};
#else
static const Direction queenMoveDirections_B2[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_B2_0,_T("queen MD_DOWNDIAG1 from B2:A1"))
 ,Direction(MD_DOWN     ,queenMoves_B2_1,_T("queen MD_DOWN from B2:B1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_B2_2,_T("queen MD_DOWNDIAG2 from B2:C1"))
 ,Direction(MD_LEFT     ,queenMoves_B2_3,_T("queen MD_LEFT from B2:A2"))
 ,Direction(MD_RIGHT    ,queenMoves_B2_4,_T("queen MD_RIGHT from B2:C2 D2 E2 F2 G2 H2"))
 ,Direction(MD_UPDIAG2  ,queenMoves_B2_5,_T("queen MD_UPDIAG2 from B2:A3"))
 ,Direction(MD_UP       ,queenMoves_B2_6,_T("queen MD_UP from B2:B3 B4 B5 B6 B7 B8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_B2_7,_T("queen MD_UPDIAG1 from B2:C3 D4 E5 F6 G7 H8"))
};
#endif
static const DirectionArray queenMoves_B2(8,queenMoveDirections_B2);

static const int queenMoves_C2_0[2] = { 1,B1 };
static const int queenMoves_C2_1[2] = { 1,C1 };
static const int queenMoves_C2_2[2] = { 1,D1 };
static const int queenMoves_C2_3[3] = { 2,B2,A2 };
static const int queenMoves_C2_4[6] = { 5,D2,E2,F2,G2,H2 };
static const int queenMoves_C2_5[3] = { 2,B3,A4 };
static const int queenMoves_C2_6[7] = { 6,C3,C4,C5,C6,C7,C8 };
static const int queenMoves_C2_7[6] = { 5,D3,E4,F5,G6,H7 };
#ifndef _DEBUG
static const Direction queenMoveDirections_C2[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_C2_0)
 ,Direction(MD_DOWN     ,queenMoves_C2_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_C2_2)
 ,Direction(MD_LEFT     ,queenMoves_C2_3)
 ,Direction(MD_RIGHT    ,queenMoves_C2_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_C2_5)
 ,Direction(MD_UP       ,queenMoves_C2_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_C2_7)
};
#else
static const Direction queenMoveDirections_C2[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_C2_0,_T("queen MD_DOWNDIAG1 from C2:B1"))
 ,Direction(MD_DOWN     ,queenMoves_C2_1,_T("queen MD_DOWN from C2:C1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_C2_2,_T("queen MD_DOWNDIAG2 from C2:D1"))
 ,Direction(MD_LEFT     ,queenMoves_C2_3,_T("queen MD_LEFT from C2:B2 A2"))
 ,Direction(MD_RIGHT    ,queenMoves_C2_4,_T("queen MD_RIGHT from C2:D2 E2 F2 G2 H2"))
 ,Direction(MD_UPDIAG2  ,queenMoves_C2_5,_T("queen MD_UPDIAG2 from C2:B3 A4"))
 ,Direction(MD_UP       ,queenMoves_C2_6,_T("queen MD_UP from C2:C3 C4 C5 C6 C7 C8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_C2_7,_T("queen MD_UPDIAG1 from C2:D3 E4 F5 G6 H7"))
};
#endif
static const DirectionArray queenMoves_C2(8,queenMoveDirections_C2);

static const int queenMoves_D2_0[2] = { 1,C1 };
static const int queenMoves_D2_1[2] = { 1,D1 };
static const int queenMoves_D2_2[2] = { 1,E1 };
static const int queenMoves_D2_3[4] = { 3,C2,B2,A2 };
static const int queenMoves_D2_4[5] = { 4,E2,F2,G2,H2 };
static const int queenMoves_D2_5[4] = { 3,C3,B4,A5 };
static const int queenMoves_D2_6[7] = { 6,D3,D4,D5,D6,D7,D8 };
static const int queenMoves_D2_7[5] = { 4,E3,F4,G5,H6 };
#ifndef _DEBUG
static const Direction queenMoveDirections_D2[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_D2_0)
 ,Direction(MD_DOWN     ,queenMoves_D2_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_D2_2)
 ,Direction(MD_LEFT     ,queenMoves_D2_3)
 ,Direction(MD_RIGHT    ,queenMoves_D2_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_D2_5)
 ,Direction(MD_UP       ,queenMoves_D2_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_D2_7)
};
#else
static const Direction queenMoveDirections_D2[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_D2_0,_T("queen MD_DOWNDIAG1 from D2:C1"))
 ,Direction(MD_DOWN     ,queenMoves_D2_1,_T("queen MD_DOWN from D2:D1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_D2_2,_T("queen MD_DOWNDIAG2 from D2:E1"))
 ,Direction(MD_LEFT     ,queenMoves_D2_3,_T("queen MD_LEFT from D2:C2 B2 A2"))
 ,Direction(MD_RIGHT    ,queenMoves_D2_4,_T("queen MD_RIGHT from D2:E2 F2 G2 H2"))
 ,Direction(MD_UPDIAG2  ,queenMoves_D2_5,_T("queen MD_UPDIAG2 from D2:C3 B4 A5"))
 ,Direction(MD_UP       ,queenMoves_D2_6,_T("queen MD_UP from D2:D3 D4 D5 D6 D7 D8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_D2_7,_T("queen MD_UPDIAG1 from D2:E3 F4 G5 H6"))
};
#endif
static const DirectionArray queenMoves_D2(8,queenMoveDirections_D2);

static const int queenMoves_E2_0[2] = { 1,D1 };
static const int queenMoves_E2_1[2] = { 1,E1 };
static const int queenMoves_E2_2[2] = { 1,F1 };
static const int queenMoves_E2_3[5] = { 4,D2,C2,B2,A2 };
static const int queenMoves_E2_4[4] = { 3,F2,G2,H2 };
static const int queenMoves_E2_5[5] = { 4,D3,C4,B5,A6 };
static const int queenMoves_E2_6[7] = { 6,E3,E4,E5,E6,E7,E8 };
static const int queenMoves_E2_7[4] = { 3,F3,G4,H5 };
#ifndef _DEBUG
static const Direction queenMoveDirections_E2[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_E2_0)
 ,Direction(MD_DOWN     ,queenMoves_E2_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_E2_2)
 ,Direction(MD_LEFT     ,queenMoves_E2_3)
 ,Direction(MD_RIGHT    ,queenMoves_E2_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_E2_5)
 ,Direction(MD_UP       ,queenMoves_E2_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_E2_7)
};
#else
static const Direction queenMoveDirections_E2[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_E2_0,_T("queen MD_DOWNDIAG1 from E2:D1"))
 ,Direction(MD_DOWN     ,queenMoves_E2_1,_T("queen MD_DOWN from E2:E1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_E2_2,_T("queen MD_DOWNDIAG2 from E2:F1"))
 ,Direction(MD_LEFT     ,queenMoves_E2_3,_T("queen MD_LEFT from E2:D2 C2 B2 A2"))
 ,Direction(MD_RIGHT    ,queenMoves_E2_4,_T("queen MD_RIGHT from E2:F2 G2 H2"))
 ,Direction(MD_UPDIAG2  ,queenMoves_E2_5,_T("queen MD_UPDIAG2 from E2:D3 C4 B5 A6"))
 ,Direction(MD_UP       ,queenMoves_E2_6,_T("queen MD_UP from E2:E3 E4 E5 E6 E7 E8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_E2_7,_T("queen MD_UPDIAG1 from E2:F3 G4 H5"))
};
#endif
static const DirectionArray queenMoves_E2(8,queenMoveDirections_E2);

static const int queenMoves_F2_0[2] = { 1,E1 };
static const int queenMoves_F2_1[2] = { 1,F1 };
static const int queenMoves_F2_2[2] = { 1,G1 };
static const int queenMoves_F2_3[6] = { 5,E2,D2,C2,B2,A2 };
static const int queenMoves_F2_4[3] = { 2,G2,H2 };
static const int queenMoves_F2_5[6] = { 5,E3,D4,C5,B6,A7 };
static const int queenMoves_F2_6[7] = { 6,F3,F4,F5,F6,F7,F8 };
static const int queenMoves_F2_7[3] = { 2,G3,H4 };
#ifndef _DEBUG
static const Direction queenMoveDirections_F2[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_F2_0)
 ,Direction(MD_DOWN     ,queenMoves_F2_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_F2_2)
 ,Direction(MD_LEFT     ,queenMoves_F2_3)
 ,Direction(MD_RIGHT    ,queenMoves_F2_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_F2_5)
 ,Direction(MD_UP       ,queenMoves_F2_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_F2_7)
};
#else
static const Direction queenMoveDirections_F2[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_F2_0,_T("queen MD_DOWNDIAG1 from F2:E1"))
 ,Direction(MD_DOWN     ,queenMoves_F2_1,_T("queen MD_DOWN from F2:F1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_F2_2,_T("queen MD_DOWNDIAG2 from F2:G1"))
 ,Direction(MD_LEFT     ,queenMoves_F2_3,_T("queen MD_LEFT from F2:E2 D2 C2 B2 A2"))
 ,Direction(MD_RIGHT    ,queenMoves_F2_4,_T("queen MD_RIGHT from F2:G2 H2"))
 ,Direction(MD_UPDIAG2  ,queenMoves_F2_5,_T("queen MD_UPDIAG2 from F2:E3 D4 C5 B6 A7"))
 ,Direction(MD_UP       ,queenMoves_F2_6,_T("queen MD_UP from F2:F3 F4 F5 F6 F7 F8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_F2_7,_T("queen MD_UPDIAG1 from F2:G3 H4"))
};
#endif
static const DirectionArray queenMoves_F2(8,queenMoveDirections_F2);

static const int queenMoves_G2_0[2] = { 1,F1 };
static const int queenMoves_G2_1[2] = { 1,G1 };
static const int queenMoves_G2_2[2] = { 1,H1 };
static const int queenMoves_G2_3[7] = { 6,F2,E2,D2,C2,B2,A2 };
static const int queenMoves_G2_4[2] = { 1,H2 };
static const int queenMoves_G2_5[7] = { 6,F3,E4,D5,C6,B7,A8 };
static const int queenMoves_G2_6[7] = { 6,G3,G4,G5,G6,G7,G8 };
static const int queenMoves_G2_7[2] = { 1,H3 };
#ifndef _DEBUG
static const Direction queenMoveDirections_G2[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_G2_0)
 ,Direction(MD_DOWN     ,queenMoves_G2_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_G2_2)
 ,Direction(MD_LEFT     ,queenMoves_G2_3)
 ,Direction(MD_RIGHT    ,queenMoves_G2_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_G2_5)
 ,Direction(MD_UP       ,queenMoves_G2_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_G2_7)
};
#else
static const Direction queenMoveDirections_G2[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_G2_0,_T("queen MD_DOWNDIAG1 from G2:F1"))
 ,Direction(MD_DOWN     ,queenMoves_G2_1,_T("queen MD_DOWN from G2:G1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_G2_2,_T("queen MD_DOWNDIAG2 from G2:H1"))
 ,Direction(MD_LEFT     ,queenMoves_G2_3,_T("queen MD_LEFT from G2:F2 E2 D2 C2 B2 A2"))
 ,Direction(MD_RIGHT    ,queenMoves_G2_4,_T("queen MD_RIGHT from G2:H2"))
 ,Direction(MD_UPDIAG2  ,queenMoves_G2_5,_T("queen MD_UPDIAG2 from G2:F3 E4 D5 C6 B7 A8"))
 ,Direction(MD_UP       ,queenMoves_G2_6,_T("queen MD_UP from G2:G3 G4 G5 G6 G7 G8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_G2_7,_T("queen MD_UPDIAG1 from G2:H3"))
};
#endif
static const DirectionArray queenMoves_G2(8,queenMoveDirections_G2);

static const int queenMoves_H2_0[2] = { 1,G1 };
static const int queenMoves_H2_1[2] = { 1,H1 };
static const int queenMoves_H2_2[8] = { 7,G2,F2,E2,D2,C2,B2,A2 };
static const int queenMoves_H2_3[7] = { 6,G3,F4,E5,D6,C7,B8 };
static const int queenMoves_H2_4[7] = { 6,H3,H4,H5,H6,H7,H8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_H2[5] = {
  Direction(MD_DOWNDIAG1,queenMoves_H2_0)
 ,Direction(MD_DOWN     ,queenMoves_H2_1)
 ,Direction(MD_LEFT     ,queenMoves_H2_2)
 ,Direction(MD_UPDIAG2  ,queenMoves_H2_3)
 ,Direction(MD_UP       ,queenMoves_H2_4)
};
#else
static const Direction queenMoveDirections_H2[5] = {
  Direction(MD_DOWNDIAG1,queenMoves_H2_0,_T("queen MD_DOWNDIAG1 from H2:G1"))
 ,Direction(MD_DOWN     ,queenMoves_H2_1,_T("queen MD_DOWN from H2:H1"))
 ,Direction(MD_LEFT     ,queenMoves_H2_2,_T("queen MD_LEFT from H2:G2 F2 E2 D2 C2 B2 A2"))
 ,Direction(MD_UPDIAG2  ,queenMoves_H2_3,_T("queen MD_UPDIAG2 from H2:G3 F4 E5 D6 C7 B8"))
 ,Direction(MD_UP       ,queenMoves_H2_4,_T("queen MD_UP from H2:H3 H4 H5 H6 H7 H8"))
};
#endif
static const DirectionArray queenMoves_H2(5,queenMoveDirections_H2);

static const int queenMoves_A3_0[3] = { 2,A2,A1 };
static const int queenMoves_A3_1[3] = { 2,B2,C1 };
static const int queenMoves_A3_2[8] = { 7,B3,C3,D3,E3,F3,G3,H3 };
static const int queenMoves_A3_3[6] = { 5,A4,A5,A6,A7,A8 };
static const int queenMoves_A3_4[6] = { 5,B4,C5,D6,E7,F8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_A3[5] = {
  Direction(MD_DOWN     ,queenMoves_A3_0)
 ,Direction(MD_DOWNDIAG2,queenMoves_A3_1)
 ,Direction(MD_RIGHT    ,queenMoves_A3_2)
 ,Direction(MD_UP       ,queenMoves_A3_3)
 ,Direction(MD_UPDIAG1  ,queenMoves_A3_4)
};
#else
static const Direction queenMoveDirections_A3[5] = {
  Direction(MD_DOWN     ,queenMoves_A3_0,_T("queen MD_DOWN from A3:A2 A1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_A3_1,_T("queen MD_DOWNDIAG2 from A3:B2 C1"))
 ,Direction(MD_RIGHT    ,queenMoves_A3_2,_T("queen MD_RIGHT from A3:B3 C3 D3 E3 F3 G3 H3"))
 ,Direction(MD_UP       ,queenMoves_A3_3,_T("queen MD_UP from A3:A4 A5 A6 A7 A8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_A3_4,_T("queen MD_UPDIAG1 from A3:B4 C5 D6 E7 F8"))
};
#endif
static const DirectionArray queenMoves_A3(5,queenMoveDirections_A3);

static const int queenMoves_B3_0[2] = { 1,A2 };
static const int queenMoves_B3_1[3] = { 2,B2,B1 };
static const int queenMoves_B3_2[3] = { 2,C2,D1 };
static const int queenMoves_B3_3[2] = { 1,A3 };
static const int queenMoves_B3_4[7] = { 6,C3,D3,E3,F3,G3,H3 };
static const int queenMoves_B3_5[2] = { 1,A4 };
static const int queenMoves_B3_6[6] = { 5,B4,B5,B6,B7,B8 };
static const int queenMoves_B3_7[6] = { 5,C4,D5,E6,F7,G8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_B3[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_B3_0)
 ,Direction(MD_DOWN     ,queenMoves_B3_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_B3_2)
 ,Direction(MD_LEFT     ,queenMoves_B3_3)
 ,Direction(MD_RIGHT    ,queenMoves_B3_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_B3_5)
 ,Direction(MD_UP       ,queenMoves_B3_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_B3_7)
};
#else
static const Direction queenMoveDirections_B3[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_B3_0,_T("queen MD_DOWNDIAG1 from B3:A2"))
 ,Direction(MD_DOWN     ,queenMoves_B3_1,_T("queen MD_DOWN from B3:B2 B1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_B3_2,_T("queen MD_DOWNDIAG2 from B3:C2 D1"))
 ,Direction(MD_LEFT     ,queenMoves_B3_3,_T("queen MD_LEFT from B3:A3"))
 ,Direction(MD_RIGHT    ,queenMoves_B3_4,_T("queen MD_RIGHT from B3:C3 D3 E3 F3 G3 H3"))
 ,Direction(MD_UPDIAG2  ,queenMoves_B3_5,_T("queen MD_UPDIAG2 from B3:A4"))
 ,Direction(MD_UP       ,queenMoves_B3_6,_T("queen MD_UP from B3:B4 B5 B6 B7 B8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_B3_7,_T("queen MD_UPDIAG1 from B3:C4 D5 E6 F7 G8"))
};
#endif
static const DirectionArray queenMoves_B3(8,queenMoveDirections_B3);

static const int queenMoves_C3_0[3] = { 2,B2,A1 };
static const int queenMoves_C3_1[3] = { 2,C2,C1 };
static const int queenMoves_C3_2[3] = { 2,D2,E1 };
static const int queenMoves_C3_3[3] = { 2,B3,A3 };
static const int queenMoves_C3_4[6] = { 5,D3,E3,F3,G3,H3 };
static const int queenMoves_C3_5[3] = { 2,B4,A5 };
static const int queenMoves_C3_6[6] = { 5,C4,C5,C6,C7,C8 };
static const int queenMoves_C3_7[6] = { 5,D4,E5,F6,G7,H8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_C3[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_C3_0)
 ,Direction(MD_DOWN     ,queenMoves_C3_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_C3_2)
 ,Direction(MD_LEFT     ,queenMoves_C3_3)
 ,Direction(MD_RIGHT    ,queenMoves_C3_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_C3_5)
 ,Direction(MD_UP       ,queenMoves_C3_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_C3_7)
};
#else
static const Direction queenMoveDirections_C3[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_C3_0,_T("queen MD_DOWNDIAG1 from C3:B2 A1"))
 ,Direction(MD_DOWN     ,queenMoves_C3_1,_T("queen MD_DOWN from C3:C2 C1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_C3_2,_T("queen MD_DOWNDIAG2 from C3:D2 E1"))
 ,Direction(MD_LEFT     ,queenMoves_C3_3,_T("queen MD_LEFT from C3:B3 A3"))
 ,Direction(MD_RIGHT    ,queenMoves_C3_4,_T("queen MD_RIGHT from C3:D3 E3 F3 G3 H3"))
 ,Direction(MD_UPDIAG2  ,queenMoves_C3_5,_T("queen MD_UPDIAG2 from C3:B4 A5"))
 ,Direction(MD_UP       ,queenMoves_C3_6,_T("queen MD_UP from C3:C4 C5 C6 C7 C8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_C3_7,_T("queen MD_UPDIAG1 from C3:D4 E5 F6 G7 H8"))
};
#endif
static const DirectionArray queenMoves_C3(8,queenMoveDirections_C3);

static const int queenMoves_D3_0[3] = { 2,C2,B1 };
static const int queenMoves_D3_1[3] = { 2,D2,D1 };
static const int queenMoves_D3_2[3] = { 2,E2,F1 };
static const int queenMoves_D3_3[4] = { 3,C3,B3,A3 };
static const int queenMoves_D3_4[5] = { 4,E3,F3,G3,H3 };
static const int queenMoves_D3_5[4] = { 3,C4,B5,A6 };
static const int queenMoves_D3_6[6] = { 5,D4,D5,D6,D7,D8 };
static const int queenMoves_D3_7[5] = { 4,E4,F5,G6,H7 };
#ifndef _DEBUG
static const Direction queenMoveDirections_D3[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_D3_0)
 ,Direction(MD_DOWN     ,queenMoves_D3_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_D3_2)
 ,Direction(MD_LEFT     ,queenMoves_D3_3)
 ,Direction(MD_RIGHT    ,queenMoves_D3_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_D3_5)
 ,Direction(MD_UP       ,queenMoves_D3_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_D3_7)
};
#else
static const Direction queenMoveDirections_D3[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_D3_0,_T("queen MD_DOWNDIAG1 from D3:C2 B1"))
 ,Direction(MD_DOWN     ,queenMoves_D3_1,_T("queen MD_DOWN from D3:D2 D1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_D3_2,_T("queen MD_DOWNDIAG2 from D3:E2 F1"))
 ,Direction(MD_LEFT     ,queenMoves_D3_3,_T("queen MD_LEFT from D3:C3 B3 A3"))
 ,Direction(MD_RIGHT    ,queenMoves_D3_4,_T("queen MD_RIGHT from D3:E3 F3 G3 H3"))
 ,Direction(MD_UPDIAG2  ,queenMoves_D3_5,_T("queen MD_UPDIAG2 from D3:C4 B5 A6"))
 ,Direction(MD_UP       ,queenMoves_D3_6,_T("queen MD_UP from D3:D4 D5 D6 D7 D8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_D3_7,_T("queen MD_UPDIAG1 from D3:E4 F5 G6 H7"))
};
#endif
static const DirectionArray queenMoves_D3(8,queenMoveDirections_D3);

static const int queenMoves_E3_0[3] = { 2,D2,C1 };
static const int queenMoves_E3_1[3] = { 2,E2,E1 };
static const int queenMoves_E3_2[3] = { 2,F2,G1 };
static const int queenMoves_E3_3[5] = { 4,D3,C3,B3,A3 };
static const int queenMoves_E3_4[4] = { 3,F3,G3,H3 };
static const int queenMoves_E3_5[5] = { 4,D4,C5,B6,A7 };
static const int queenMoves_E3_6[6] = { 5,E4,E5,E6,E7,E8 };
static const int queenMoves_E3_7[4] = { 3,F4,G5,H6 };
#ifndef _DEBUG
static const Direction queenMoveDirections_E3[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_E3_0)
 ,Direction(MD_DOWN     ,queenMoves_E3_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_E3_2)
 ,Direction(MD_LEFT     ,queenMoves_E3_3)
 ,Direction(MD_RIGHT    ,queenMoves_E3_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_E3_5)
 ,Direction(MD_UP       ,queenMoves_E3_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_E3_7)
};
#else
static const Direction queenMoveDirections_E3[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_E3_0,_T("queen MD_DOWNDIAG1 from E3:D2 C1"))
 ,Direction(MD_DOWN     ,queenMoves_E3_1,_T("queen MD_DOWN from E3:E2 E1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_E3_2,_T("queen MD_DOWNDIAG2 from E3:F2 G1"))
 ,Direction(MD_LEFT     ,queenMoves_E3_3,_T("queen MD_LEFT from E3:D3 C3 B3 A3"))
 ,Direction(MD_RIGHT    ,queenMoves_E3_4,_T("queen MD_RIGHT from E3:F3 G3 H3"))
 ,Direction(MD_UPDIAG2  ,queenMoves_E3_5,_T("queen MD_UPDIAG2 from E3:D4 C5 B6 A7"))
 ,Direction(MD_UP       ,queenMoves_E3_6,_T("queen MD_UP from E3:E4 E5 E6 E7 E8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_E3_7,_T("queen MD_UPDIAG1 from E3:F4 G5 H6"))
};
#endif
static const DirectionArray queenMoves_E3(8,queenMoveDirections_E3);

static const int queenMoves_F3_0[3] = { 2,E2,D1 };
static const int queenMoves_F3_1[3] = { 2,F2,F1 };
static const int queenMoves_F3_2[3] = { 2,G2,H1 };
static const int queenMoves_F3_3[6] = { 5,E3,D3,C3,B3,A3 };
static const int queenMoves_F3_4[3] = { 2,G3,H3 };
static const int queenMoves_F3_5[6] = { 5,E4,D5,C6,B7,A8 };
static const int queenMoves_F3_6[6] = { 5,F4,F5,F6,F7,F8 };
static const int queenMoves_F3_7[3] = { 2,G4,H5 };
#ifndef _DEBUG
static const Direction queenMoveDirections_F3[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_F3_0)
 ,Direction(MD_DOWN     ,queenMoves_F3_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_F3_2)
 ,Direction(MD_LEFT     ,queenMoves_F3_3)
 ,Direction(MD_RIGHT    ,queenMoves_F3_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_F3_5)
 ,Direction(MD_UP       ,queenMoves_F3_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_F3_7)
};
#else
static const Direction queenMoveDirections_F3[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_F3_0,_T("queen MD_DOWNDIAG1 from F3:E2 D1"))
 ,Direction(MD_DOWN     ,queenMoves_F3_1,_T("queen MD_DOWN from F3:F2 F1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_F3_2,_T("queen MD_DOWNDIAG2 from F3:G2 H1"))
 ,Direction(MD_LEFT     ,queenMoves_F3_3,_T("queen MD_LEFT from F3:E3 D3 C3 B3 A3"))
 ,Direction(MD_RIGHT    ,queenMoves_F3_4,_T("queen MD_RIGHT from F3:G3 H3"))
 ,Direction(MD_UPDIAG2  ,queenMoves_F3_5,_T("queen MD_UPDIAG2 from F3:E4 D5 C6 B7 A8"))
 ,Direction(MD_UP       ,queenMoves_F3_6,_T("queen MD_UP from F3:F4 F5 F6 F7 F8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_F3_7,_T("queen MD_UPDIAG1 from F3:G4 H5"))
};
#endif
static const DirectionArray queenMoves_F3(8,queenMoveDirections_F3);

static const int queenMoves_G3_0[3] = { 2,F2,E1 };
static const int queenMoves_G3_1[3] = { 2,G2,G1 };
static const int queenMoves_G3_2[2] = { 1,H2 };
static const int queenMoves_G3_3[7] = { 6,F3,E3,D3,C3,B3,A3 };
static const int queenMoves_G3_4[2] = { 1,H3 };
static const int queenMoves_G3_5[6] = { 5,F4,E5,D6,C7,B8 };
static const int queenMoves_G3_6[6] = { 5,G4,G5,G6,G7,G8 };
static const int queenMoves_G3_7[2] = { 1,H4 };
#ifndef _DEBUG
static const Direction queenMoveDirections_G3[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_G3_0)
 ,Direction(MD_DOWN     ,queenMoves_G3_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_G3_2)
 ,Direction(MD_LEFT     ,queenMoves_G3_3)
 ,Direction(MD_RIGHT    ,queenMoves_G3_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_G3_5)
 ,Direction(MD_UP       ,queenMoves_G3_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_G3_7)
};
#else
static const Direction queenMoveDirections_G3[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_G3_0,_T("queen MD_DOWNDIAG1 from G3:F2 E1"))
 ,Direction(MD_DOWN     ,queenMoves_G3_1,_T("queen MD_DOWN from G3:G2 G1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_G3_2,_T("queen MD_DOWNDIAG2 from G3:H2"))
 ,Direction(MD_LEFT     ,queenMoves_G3_3,_T("queen MD_LEFT from G3:F3 E3 D3 C3 B3 A3"))
 ,Direction(MD_RIGHT    ,queenMoves_G3_4,_T("queen MD_RIGHT from G3:H3"))
 ,Direction(MD_UPDIAG2  ,queenMoves_G3_5,_T("queen MD_UPDIAG2 from G3:F4 E5 D6 C7 B8"))
 ,Direction(MD_UP       ,queenMoves_G3_6,_T("queen MD_UP from G3:G4 G5 G6 G7 G8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_G3_7,_T("queen MD_UPDIAG1 from G3:H4"))
};
#endif
static const DirectionArray queenMoves_G3(8,queenMoveDirections_G3);

static const int queenMoves_H3_0[3] = { 2,G2,F1 };
static const int queenMoves_H3_1[3] = { 2,H2,H1 };
static const int queenMoves_H3_2[8] = { 7,G3,F3,E3,D3,C3,B3,A3 };
static const int queenMoves_H3_3[6] = { 5,G4,F5,E6,D7,C8 };
static const int queenMoves_H3_4[6] = { 5,H4,H5,H6,H7,H8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_H3[5] = {
  Direction(MD_DOWNDIAG1,queenMoves_H3_0)
 ,Direction(MD_DOWN     ,queenMoves_H3_1)
 ,Direction(MD_LEFT     ,queenMoves_H3_2)
 ,Direction(MD_UPDIAG2  ,queenMoves_H3_3)
 ,Direction(MD_UP       ,queenMoves_H3_4)
};
#else
static const Direction queenMoveDirections_H3[5] = {
  Direction(MD_DOWNDIAG1,queenMoves_H3_0,_T("queen MD_DOWNDIAG1 from H3:G2 F1"))
 ,Direction(MD_DOWN     ,queenMoves_H3_1,_T("queen MD_DOWN from H3:H2 H1"))
 ,Direction(MD_LEFT     ,queenMoves_H3_2,_T("queen MD_LEFT from H3:G3 F3 E3 D3 C3 B3 A3"))
 ,Direction(MD_UPDIAG2  ,queenMoves_H3_3,_T("queen MD_UPDIAG2 from H3:G4 F5 E6 D7 C8"))
 ,Direction(MD_UP       ,queenMoves_H3_4,_T("queen MD_UP from H3:H4 H5 H6 H7 H8"))
};
#endif
static const DirectionArray queenMoves_H3(5,queenMoveDirections_H3);

static const int queenMoves_A4_0[4] = { 3,A3,A2,A1 };
static const int queenMoves_A4_1[4] = { 3,B3,C2,D1 };
static const int queenMoves_A4_2[8] = { 7,B4,C4,D4,E4,F4,G4,H4 };
static const int queenMoves_A4_3[5] = { 4,A5,A6,A7,A8 };
static const int queenMoves_A4_4[5] = { 4,B5,C6,D7,E8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_A4[5] = {
  Direction(MD_DOWN     ,queenMoves_A4_0)
 ,Direction(MD_DOWNDIAG2,queenMoves_A4_1)
 ,Direction(MD_RIGHT    ,queenMoves_A4_2)
 ,Direction(MD_UP       ,queenMoves_A4_3)
 ,Direction(MD_UPDIAG1  ,queenMoves_A4_4)
};
#else
static const Direction queenMoveDirections_A4[5] = {
  Direction(MD_DOWN     ,queenMoves_A4_0,_T("queen MD_DOWN from A4:A3 A2 A1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_A4_1,_T("queen MD_DOWNDIAG2 from A4:B3 C2 D1"))
 ,Direction(MD_RIGHT    ,queenMoves_A4_2,_T("queen MD_RIGHT from A4:B4 C4 D4 E4 F4 G4 H4"))
 ,Direction(MD_UP       ,queenMoves_A4_3,_T("queen MD_UP from A4:A5 A6 A7 A8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_A4_4,_T("queen MD_UPDIAG1 from A4:B5 C6 D7 E8"))
};
#endif
static const DirectionArray queenMoves_A4(5,queenMoveDirections_A4);

static const int queenMoves_B4_0[2] = { 1,A3 };
static const int queenMoves_B4_1[4] = { 3,B3,B2,B1 };
static const int queenMoves_B4_2[4] = { 3,C3,D2,E1 };
static const int queenMoves_B4_3[2] = { 1,A4 };
static const int queenMoves_B4_4[7] = { 6,C4,D4,E4,F4,G4,H4 };
static const int queenMoves_B4_5[2] = { 1,A5 };
static const int queenMoves_B4_6[5] = { 4,B5,B6,B7,B8 };
static const int queenMoves_B4_7[5] = { 4,C5,D6,E7,F8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_B4[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_B4_0)
 ,Direction(MD_DOWN     ,queenMoves_B4_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_B4_2)
 ,Direction(MD_LEFT     ,queenMoves_B4_3)
 ,Direction(MD_RIGHT    ,queenMoves_B4_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_B4_5)
 ,Direction(MD_UP       ,queenMoves_B4_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_B4_7)
};
#else
static const Direction queenMoveDirections_B4[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_B4_0,_T("queen MD_DOWNDIAG1 from B4:A3"))
 ,Direction(MD_DOWN     ,queenMoves_B4_1,_T("queen MD_DOWN from B4:B3 B2 B1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_B4_2,_T("queen MD_DOWNDIAG2 from B4:C3 D2 E1"))
 ,Direction(MD_LEFT     ,queenMoves_B4_3,_T("queen MD_LEFT from B4:A4"))
 ,Direction(MD_RIGHT    ,queenMoves_B4_4,_T("queen MD_RIGHT from B4:C4 D4 E4 F4 G4 H4"))
 ,Direction(MD_UPDIAG2  ,queenMoves_B4_5,_T("queen MD_UPDIAG2 from B4:A5"))
 ,Direction(MD_UP       ,queenMoves_B4_6,_T("queen MD_UP from B4:B5 B6 B7 B8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_B4_7,_T("queen MD_UPDIAG1 from B4:C5 D6 E7 F8"))
};
#endif
static const DirectionArray queenMoves_B4(8,queenMoveDirections_B4);

static const int queenMoves_C4_0[3] = { 2,B3,A2 };
static const int queenMoves_C4_1[4] = { 3,C3,C2,C1 };
static const int queenMoves_C4_2[4] = { 3,D3,E2,F1 };
static const int queenMoves_C4_3[3] = { 2,B4,A4 };
static const int queenMoves_C4_4[6] = { 5,D4,E4,F4,G4,H4 };
static const int queenMoves_C4_5[3] = { 2,B5,A6 };
static const int queenMoves_C4_6[5] = { 4,C5,C6,C7,C8 };
static const int queenMoves_C4_7[5] = { 4,D5,E6,F7,G8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_C4[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_C4_0)
 ,Direction(MD_DOWN     ,queenMoves_C4_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_C4_2)
 ,Direction(MD_LEFT     ,queenMoves_C4_3)
 ,Direction(MD_RIGHT    ,queenMoves_C4_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_C4_5)
 ,Direction(MD_UP       ,queenMoves_C4_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_C4_7)
};
#else
static const Direction queenMoveDirections_C4[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_C4_0,_T("queen MD_DOWNDIAG1 from C4:B3 A2"))
 ,Direction(MD_DOWN     ,queenMoves_C4_1,_T("queen MD_DOWN from C4:C3 C2 C1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_C4_2,_T("queen MD_DOWNDIAG2 from C4:D3 E2 F1"))
 ,Direction(MD_LEFT     ,queenMoves_C4_3,_T("queen MD_LEFT from C4:B4 A4"))
 ,Direction(MD_RIGHT    ,queenMoves_C4_4,_T("queen MD_RIGHT from C4:D4 E4 F4 G4 H4"))
 ,Direction(MD_UPDIAG2  ,queenMoves_C4_5,_T("queen MD_UPDIAG2 from C4:B5 A6"))
 ,Direction(MD_UP       ,queenMoves_C4_6,_T("queen MD_UP from C4:C5 C6 C7 C8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_C4_7,_T("queen MD_UPDIAG1 from C4:D5 E6 F7 G8"))
};
#endif
static const DirectionArray queenMoves_C4(8,queenMoveDirections_C4);

static const int queenMoves_D4_0[4] = { 3,C3,B2,A1 };
static const int queenMoves_D4_1[4] = { 3,D3,D2,D1 };
static const int queenMoves_D4_2[4] = { 3,E3,F2,G1 };
static const int queenMoves_D4_3[4] = { 3,C4,B4,A4 };
static const int queenMoves_D4_4[5] = { 4,E4,F4,G4,H4 };
static const int queenMoves_D4_5[4] = { 3,C5,B6,A7 };
static const int queenMoves_D4_6[5] = { 4,D5,D6,D7,D8 };
static const int queenMoves_D4_7[5] = { 4,E5,F6,G7,H8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_D4[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_D4_0)
 ,Direction(MD_DOWN     ,queenMoves_D4_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_D4_2)
 ,Direction(MD_LEFT     ,queenMoves_D4_3)
 ,Direction(MD_RIGHT    ,queenMoves_D4_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_D4_5)
 ,Direction(MD_UP       ,queenMoves_D4_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_D4_7)
};
#else
static const Direction queenMoveDirections_D4[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_D4_0,_T("queen MD_DOWNDIAG1 from D4:C3 B2 A1"))
 ,Direction(MD_DOWN     ,queenMoves_D4_1,_T("queen MD_DOWN from D4:D3 D2 D1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_D4_2,_T("queen MD_DOWNDIAG2 from D4:E3 F2 G1"))
 ,Direction(MD_LEFT     ,queenMoves_D4_3,_T("queen MD_LEFT from D4:C4 B4 A4"))
 ,Direction(MD_RIGHT    ,queenMoves_D4_4,_T("queen MD_RIGHT from D4:E4 F4 G4 H4"))
 ,Direction(MD_UPDIAG2  ,queenMoves_D4_5,_T("queen MD_UPDIAG2 from D4:C5 B6 A7"))
 ,Direction(MD_UP       ,queenMoves_D4_6,_T("queen MD_UP from D4:D5 D6 D7 D8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_D4_7,_T("queen MD_UPDIAG1 from D4:E5 F6 G7 H8"))
};
#endif
static const DirectionArray queenMoves_D4(8,queenMoveDirections_D4);

static const int queenMoves_E4_0[4] = { 3,D3,C2,B1 };
static const int queenMoves_E4_1[4] = { 3,E3,E2,E1 };
static const int queenMoves_E4_2[4] = { 3,F3,G2,H1 };
static const int queenMoves_E4_3[5] = { 4,D4,C4,B4,A4 };
static const int queenMoves_E4_4[4] = { 3,F4,G4,H4 };
static const int queenMoves_E4_5[5] = { 4,D5,C6,B7,A8 };
static const int queenMoves_E4_6[5] = { 4,E5,E6,E7,E8 };
static const int queenMoves_E4_7[4] = { 3,F5,G6,H7 };
#ifndef _DEBUG
static const Direction queenMoveDirections_E4[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_E4_0)
 ,Direction(MD_DOWN     ,queenMoves_E4_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_E4_2)
 ,Direction(MD_LEFT     ,queenMoves_E4_3)
 ,Direction(MD_RIGHT    ,queenMoves_E4_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_E4_5)
 ,Direction(MD_UP       ,queenMoves_E4_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_E4_7)
};
#else
static const Direction queenMoveDirections_E4[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_E4_0,_T("queen MD_DOWNDIAG1 from E4:D3 C2 B1"))
 ,Direction(MD_DOWN     ,queenMoves_E4_1,_T("queen MD_DOWN from E4:E3 E2 E1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_E4_2,_T("queen MD_DOWNDIAG2 from E4:F3 G2 H1"))
 ,Direction(MD_LEFT     ,queenMoves_E4_3,_T("queen MD_LEFT from E4:D4 C4 B4 A4"))
 ,Direction(MD_RIGHT    ,queenMoves_E4_4,_T("queen MD_RIGHT from E4:F4 G4 H4"))
 ,Direction(MD_UPDIAG2  ,queenMoves_E4_5,_T("queen MD_UPDIAG2 from E4:D5 C6 B7 A8"))
 ,Direction(MD_UP       ,queenMoves_E4_6,_T("queen MD_UP from E4:E5 E6 E7 E8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_E4_7,_T("queen MD_UPDIAG1 from E4:F5 G6 H7"))
};
#endif
static const DirectionArray queenMoves_E4(8,queenMoveDirections_E4);

static const int queenMoves_F4_0[4] = { 3,E3,D2,C1 };
static const int queenMoves_F4_1[4] = { 3,F3,F2,F1 };
static const int queenMoves_F4_2[3] = { 2,G3,H2 };
static const int queenMoves_F4_3[6] = { 5,E4,D4,C4,B4,A4 };
static const int queenMoves_F4_4[3] = { 2,G4,H4 };
static const int queenMoves_F4_5[5] = { 4,E5,D6,C7,B8 };
static const int queenMoves_F4_6[5] = { 4,F5,F6,F7,F8 };
static const int queenMoves_F4_7[3] = { 2,G5,H6 };
#ifndef _DEBUG
static const Direction queenMoveDirections_F4[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_F4_0)
 ,Direction(MD_DOWN     ,queenMoves_F4_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_F4_2)
 ,Direction(MD_LEFT     ,queenMoves_F4_3)
 ,Direction(MD_RIGHT    ,queenMoves_F4_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_F4_5)
 ,Direction(MD_UP       ,queenMoves_F4_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_F4_7)
};
#else
static const Direction queenMoveDirections_F4[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_F4_0,_T("queen MD_DOWNDIAG1 from F4:E3 D2 C1"))
 ,Direction(MD_DOWN     ,queenMoves_F4_1,_T("queen MD_DOWN from F4:F3 F2 F1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_F4_2,_T("queen MD_DOWNDIAG2 from F4:G3 H2"))
 ,Direction(MD_LEFT     ,queenMoves_F4_3,_T("queen MD_LEFT from F4:E4 D4 C4 B4 A4"))
 ,Direction(MD_RIGHT    ,queenMoves_F4_4,_T("queen MD_RIGHT from F4:G4 H4"))
 ,Direction(MD_UPDIAG2  ,queenMoves_F4_5,_T("queen MD_UPDIAG2 from F4:E5 D6 C7 B8"))
 ,Direction(MD_UP       ,queenMoves_F4_6,_T("queen MD_UP from F4:F5 F6 F7 F8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_F4_7,_T("queen MD_UPDIAG1 from F4:G5 H6"))
};
#endif
static const DirectionArray queenMoves_F4(8,queenMoveDirections_F4);

static const int queenMoves_G4_0[4] = { 3,F3,E2,D1 };
static const int queenMoves_G4_1[4] = { 3,G3,G2,G1 };
static const int queenMoves_G4_2[2] = { 1,H3 };
static const int queenMoves_G4_3[7] = { 6,F4,E4,D4,C4,B4,A4 };
static const int queenMoves_G4_4[2] = { 1,H4 };
static const int queenMoves_G4_5[5] = { 4,F5,E6,D7,C8 };
static const int queenMoves_G4_6[5] = { 4,G5,G6,G7,G8 };
static const int queenMoves_G4_7[2] = { 1,H5 };
#ifndef _DEBUG
static const Direction queenMoveDirections_G4[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_G4_0)
 ,Direction(MD_DOWN     ,queenMoves_G4_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_G4_2)
 ,Direction(MD_LEFT     ,queenMoves_G4_3)
 ,Direction(MD_RIGHT    ,queenMoves_G4_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_G4_5)
 ,Direction(MD_UP       ,queenMoves_G4_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_G4_7)
};
#else
static const Direction queenMoveDirections_G4[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_G4_0,_T("queen MD_DOWNDIAG1 from G4:F3 E2 D1"))
 ,Direction(MD_DOWN     ,queenMoves_G4_1,_T("queen MD_DOWN from G4:G3 G2 G1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_G4_2,_T("queen MD_DOWNDIAG2 from G4:H3"))
 ,Direction(MD_LEFT     ,queenMoves_G4_3,_T("queen MD_LEFT from G4:F4 E4 D4 C4 B4 A4"))
 ,Direction(MD_RIGHT    ,queenMoves_G4_4,_T("queen MD_RIGHT from G4:H4"))
 ,Direction(MD_UPDIAG2  ,queenMoves_G4_5,_T("queen MD_UPDIAG2 from G4:F5 E6 D7 C8"))
 ,Direction(MD_UP       ,queenMoves_G4_6,_T("queen MD_UP from G4:G5 G6 G7 G8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_G4_7,_T("queen MD_UPDIAG1 from G4:H5"))
};
#endif
static const DirectionArray queenMoves_G4(8,queenMoveDirections_G4);

static const int queenMoves_H4_0[4] = { 3,G3,F2,E1 };
static const int queenMoves_H4_1[4] = { 3,H3,H2,H1 };
static const int queenMoves_H4_2[8] = { 7,G4,F4,E4,D4,C4,B4,A4 };
static const int queenMoves_H4_3[5] = { 4,G5,F6,E7,D8 };
static const int queenMoves_H4_4[5] = { 4,H5,H6,H7,H8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_H4[5] = {
  Direction(MD_DOWNDIAG1,queenMoves_H4_0)
 ,Direction(MD_DOWN     ,queenMoves_H4_1)
 ,Direction(MD_LEFT     ,queenMoves_H4_2)
 ,Direction(MD_UPDIAG2  ,queenMoves_H4_3)
 ,Direction(MD_UP       ,queenMoves_H4_4)
};
#else
static const Direction queenMoveDirections_H4[5] = {
  Direction(MD_DOWNDIAG1,queenMoves_H4_0,_T("queen MD_DOWNDIAG1 from H4:G3 F2 E1"))
 ,Direction(MD_DOWN     ,queenMoves_H4_1,_T("queen MD_DOWN from H4:H3 H2 H1"))
 ,Direction(MD_LEFT     ,queenMoves_H4_2,_T("queen MD_LEFT from H4:G4 F4 E4 D4 C4 B4 A4"))
 ,Direction(MD_UPDIAG2  ,queenMoves_H4_3,_T("queen MD_UPDIAG2 from H4:G5 F6 E7 D8"))
 ,Direction(MD_UP       ,queenMoves_H4_4,_T("queen MD_UP from H4:H5 H6 H7 H8"))
};
#endif
static const DirectionArray queenMoves_H4(5,queenMoveDirections_H4);

static const int queenMoves_A5_0[5] = { 4,A4,A3,A2,A1 };
static const int queenMoves_A5_1[5] = { 4,B4,C3,D2,E1 };
static const int queenMoves_A5_2[8] = { 7,B5,C5,D5,E5,F5,G5,H5 };
static const int queenMoves_A5_3[4] = { 3,A6,A7,A8 };
static const int queenMoves_A5_4[4] = { 3,B6,C7,D8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_A5[5] = {
  Direction(MD_DOWN     ,queenMoves_A5_0)
 ,Direction(MD_DOWNDIAG2,queenMoves_A5_1)
 ,Direction(MD_RIGHT    ,queenMoves_A5_2)
 ,Direction(MD_UP       ,queenMoves_A5_3)
 ,Direction(MD_UPDIAG1  ,queenMoves_A5_4)
};
#else
static const Direction queenMoveDirections_A5[5] = {
  Direction(MD_DOWN     ,queenMoves_A5_0,_T("queen MD_DOWN from A5:A4 A3 A2 A1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_A5_1,_T("queen MD_DOWNDIAG2 from A5:B4 C3 D2 E1"))
 ,Direction(MD_RIGHT    ,queenMoves_A5_2,_T("queen MD_RIGHT from A5:B5 C5 D5 E5 F5 G5 H5"))
 ,Direction(MD_UP       ,queenMoves_A5_3,_T("queen MD_UP from A5:A6 A7 A8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_A5_4,_T("queen MD_UPDIAG1 from A5:B6 C7 D8"))
};
#endif
static const DirectionArray queenMoves_A5(5,queenMoveDirections_A5);

static const int queenMoves_B5_0[2] = { 1,A4 };
static const int queenMoves_B5_1[5] = { 4,B4,B3,B2,B1 };
static const int queenMoves_B5_2[5] = { 4,C4,D3,E2,F1 };
static const int queenMoves_B5_3[2] = { 1,A5 };
static const int queenMoves_B5_4[7] = { 6,C5,D5,E5,F5,G5,H5 };
static const int queenMoves_B5_5[2] = { 1,A6 };
static const int queenMoves_B5_6[4] = { 3,B6,B7,B8 };
static const int queenMoves_B5_7[4] = { 3,C6,D7,E8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_B5[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_B5_0)
 ,Direction(MD_DOWN     ,queenMoves_B5_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_B5_2)
 ,Direction(MD_LEFT     ,queenMoves_B5_3)
 ,Direction(MD_RIGHT    ,queenMoves_B5_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_B5_5)
 ,Direction(MD_UP       ,queenMoves_B5_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_B5_7)
};
#else
static const Direction queenMoveDirections_B5[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_B5_0,_T("queen MD_DOWNDIAG1 from B5:A4"))
 ,Direction(MD_DOWN     ,queenMoves_B5_1,_T("queen MD_DOWN from B5:B4 B3 B2 B1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_B5_2,_T("queen MD_DOWNDIAG2 from B5:C4 D3 E2 F1"))
 ,Direction(MD_LEFT     ,queenMoves_B5_3,_T("queen MD_LEFT from B5:A5"))
 ,Direction(MD_RIGHT    ,queenMoves_B5_4,_T("queen MD_RIGHT from B5:C5 D5 E5 F5 G5 H5"))
 ,Direction(MD_UPDIAG2  ,queenMoves_B5_5,_T("queen MD_UPDIAG2 from B5:A6"))
 ,Direction(MD_UP       ,queenMoves_B5_6,_T("queen MD_UP from B5:B6 B7 B8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_B5_7,_T("queen MD_UPDIAG1 from B5:C6 D7 E8"))
};
#endif
static const DirectionArray queenMoves_B5(8,queenMoveDirections_B5);

static const int queenMoves_C5_0[3] = { 2,B4,A3 };
static const int queenMoves_C5_1[5] = { 4,C4,C3,C2,C1 };
static const int queenMoves_C5_2[5] = { 4,D4,E3,F2,G1 };
static const int queenMoves_C5_3[3] = { 2,B5,A5 };
static const int queenMoves_C5_4[6] = { 5,D5,E5,F5,G5,H5 };
static const int queenMoves_C5_5[3] = { 2,B6,A7 };
static const int queenMoves_C5_6[4] = { 3,C6,C7,C8 };
static const int queenMoves_C5_7[4] = { 3,D6,E7,F8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_C5[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_C5_0)
 ,Direction(MD_DOWN     ,queenMoves_C5_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_C5_2)
 ,Direction(MD_LEFT     ,queenMoves_C5_3)
 ,Direction(MD_RIGHT    ,queenMoves_C5_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_C5_5)
 ,Direction(MD_UP       ,queenMoves_C5_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_C5_7)
};
#else
static const Direction queenMoveDirections_C5[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_C5_0,_T("queen MD_DOWNDIAG1 from C5:B4 A3"))
 ,Direction(MD_DOWN     ,queenMoves_C5_1,_T("queen MD_DOWN from C5:C4 C3 C2 C1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_C5_2,_T("queen MD_DOWNDIAG2 from C5:D4 E3 F2 G1"))
 ,Direction(MD_LEFT     ,queenMoves_C5_3,_T("queen MD_LEFT from C5:B5 A5"))
 ,Direction(MD_RIGHT    ,queenMoves_C5_4,_T("queen MD_RIGHT from C5:D5 E5 F5 G5 H5"))
 ,Direction(MD_UPDIAG2  ,queenMoves_C5_5,_T("queen MD_UPDIAG2 from C5:B6 A7"))
 ,Direction(MD_UP       ,queenMoves_C5_6,_T("queen MD_UP from C5:C6 C7 C8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_C5_7,_T("queen MD_UPDIAG1 from C5:D6 E7 F8"))
};
#endif
static const DirectionArray queenMoves_C5(8,queenMoveDirections_C5);

static const int queenMoves_D5_0[4] = { 3,C4,B3,A2 };
static const int queenMoves_D5_1[5] = { 4,D4,D3,D2,D1 };
static const int queenMoves_D5_2[5] = { 4,E4,F3,G2,H1 };
static const int queenMoves_D5_3[4] = { 3,C5,B5,A5 };
static const int queenMoves_D5_4[5] = { 4,E5,F5,G5,H5 };
static const int queenMoves_D5_5[4] = { 3,C6,B7,A8 };
static const int queenMoves_D5_6[4] = { 3,D6,D7,D8 };
static const int queenMoves_D5_7[4] = { 3,E6,F7,G8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_D5[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_D5_0)
 ,Direction(MD_DOWN     ,queenMoves_D5_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_D5_2)
 ,Direction(MD_LEFT     ,queenMoves_D5_3)
 ,Direction(MD_RIGHT    ,queenMoves_D5_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_D5_5)
 ,Direction(MD_UP       ,queenMoves_D5_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_D5_7)
};
#else
static const Direction queenMoveDirections_D5[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_D5_0,_T("queen MD_DOWNDIAG1 from D5:C4 B3 A2"))
 ,Direction(MD_DOWN     ,queenMoves_D5_1,_T("queen MD_DOWN from D5:D4 D3 D2 D1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_D5_2,_T("queen MD_DOWNDIAG2 from D5:E4 F3 G2 H1"))
 ,Direction(MD_LEFT     ,queenMoves_D5_3,_T("queen MD_LEFT from D5:C5 B5 A5"))
 ,Direction(MD_RIGHT    ,queenMoves_D5_4,_T("queen MD_RIGHT from D5:E5 F5 G5 H5"))
 ,Direction(MD_UPDIAG2  ,queenMoves_D5_5,_T("queen MD_UPDIAG2 from D5:C6 B7 A8"))
 ,Direction(MD_UP       ,queenMoves_D5_6,_T("queen MD_UP from D5:D6 D7 D8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_D5_7,_T("queen MD_UPDIAG1 from D5:E6 F7 G8"))
};
#endif
static const DirectionArray queenMoves_D5(8,queenMoveDirections_D5);

static const int queenMoves_E5_0[5] = { 4,D4,C3,B2,A1 };
static const int queenMoves_E5_1[5] = { 4,E4,E3,E2,E1 };
static const int queenMoves_E5_2[4] = { 3,F4,G3,H2 };
static const int queenMoves_E5_3[5] = { 4,D5,C5,B5,A5 };
static const int queenMoves_E5_4[4] = { 3,F5,G5,H5 };
static const int queenMoves_E5_5[4] = { 3,D6,C7,B8 };
static const int queenMoves_E5_6[4] = { 3,E6,E7,E8 };
static const int queenMoves_E5_7[4] = { 3,F6,G7,H8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_E5[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_E5_0)
 ,Direction(MD_DOWN     ,queenMoves_E5_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_E5_2)
 ,Direction(MD_LEFT     ,queenMoves_E5_3)
 ,Direction(MD_RIGHT    ,queenMoves_E5_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_E5_5)
 ,Direction(MD_UP       ,queenMoves_E5_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_E5_7)
};
#else
static const Direction queenMoveDirections_E5[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_E5_0,_T("queen MD_DOWNDIAG1 from E5:D4 C3 B2 A1"))
 ,Direction(MD_DOWN     ,queenMoves_E5_1,_T("queen MD_DOWN from E5:E4 E3 E2 E1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_E5_2,_T("queen MD_DOWNDIAG2 from E5:F4 G3 H2"))
 ,Direction(MD_LEFT     ,queenMoves_E5_3,_T("queen MD_LEFT from E5:D5 C5 B5 A5"))
 ,Direction(MD_RIGHT    ,queenMoves_E5_4,_T("queen MD_RIGHT from E5:F5 G5 H5"))
 ,Direction(MD_UPDIAG2  ,queenMoves_E5_5,_T("queen MD_UPDIAG2 from E5:D6 C7 B8"))
 ,Direction(MD_UP       ,queenMoves_E5_6,_T("queen MD_UP from E5:E6 E7 E8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_E5_7,_T("queen MD_UPDIAG1 from E5:F6 G7 H8"))
};
#endif
static const DirectionArray queenMoves_E5(8,queenMoveDirections_E5);

static const int queenMoves_F5_0[5] = { 4,E4,D3,C2,B1 };
static const int queenMoves_F5_1[5] = { 4,F4,F3,F2,F1 };
static const int queenMoves_F5_2[3] = { 2,G4,H3 };
static const int queenMoves_F5_3[6] = { 5,E5,D5,C5,B5,A5 };
static const int queenMoves_F5_4[3] = { 2,G5,H5 };
static const int queenMoves_F5_5[4] = { 3,E6,D7,C8 };
static const int queenMoves_F5_6[4] = { 3,F6,F7,F8 };
static const int queenMoves_F5_7[3] = { 2,G6,H7 };
#ifndef _DEBUG
static const Direction queenMoveDirections_F5[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_F5_0)
 ,Direction(MD_DOWN     ,queenMoves_F5_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_F5_2)
 ,Direction(MD_LEFT     ,queenMoves_F5_3)
 ,Direction(MD_RIGHT    ,queenMoves_F5_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_F5_5)
 ,Direction(MD_UP       ,queenMoves_F5_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_F5_7)
};
#else
static const Direction queenMoveDirections_F5[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_F5_0,_T("queen MD_DOWNDIAG1 from F5:E4 D3 C2 B1"))
 ,Direction(MD_DOWN     ,queenMoves_F5_1,_T("queen MD_DOWN from F5:F4 F3 F2 F1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_F5_2,_T("queen MD_DOWNDIAG2 from F5:G4 H3"))
 ,Direction(MD_LEFT     ,queenMoves_F5_3,_T("queen MD_LEFT from F5:E5 D5 C5 B5 A5"))
 ,Direction(MD_RIGHT    ,queenMoves_F5_4,_T("queen MD_RIGHT from F5:G5 H5"))
 ,Direction(MD_UPDIAG2  ,queenMoves_F5_5,_T("queen MD_UPDIAG2 from F5:E6 D7 C8"))
 ,Direction(MD_UP       ,queenMoves_F5_6,_T("queen MD_UP from F5:F6 F7 F8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_F5_7,_T("queen MD_UPDIAG1 from F5:G6 H7"))
};
#endif
static const DirectionArray queenMoves_F5(8,queenMoveDirections_F5);

static const int queenMoves_G5_0[5] = { 4,F4,E3,D2,C1 };
static const int queenMoves_G5_1[5] = { 4,G4,G3,G2,G1 };
static const int queenMoves_G5_2[2] = { 1,H4 };
static const int queenMoves_G5_3[7] = { 6,F5,E5,D5,C5,B5,A5 };
static const int queenMoves_G5_4[2] = { 1,H5 };
static const int queenMoves_G5_5[4] = { 3,F6,E7,D8 };
static const int queenMoves_G5_6[4] = { 3,G6,G7,G8 };
static const int queenMoves_G5_7[2] = { 1,H6 };
#ifndef _DEBUG
static const Direction queenMoveDirections_G5[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_G5_0)
 ,Direction(MD_DOWN     ,queenMoves_G5_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_G5_2)
 ,Direction(MD_LEFT     ,queenMoves_G5_3)
 ,Direction(MD_RIGHT    ,queenMoves_G5_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_G5_5)
 ,Direction(MD_UP       ,queenMoves_G5_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_G5_7)
};
#else
static const Direction queenMoveDirections_G5[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_G5_0,_T("queen MD_DOWNDIAG1 from G5:F4 E3 D2 C1"))
 ,Direction(MD_DOWN     ,queenMoves_G5_1,_T("queen MD_DOWN from G5:G4 G3 G2 G1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_G5_2,_T("queen MD_DOWNDIAG2 from G5:H4"))
 ,Direction(MD_LEFT     ,queenMoves_G5_3,_T("queen MD_LEFT from G5:F5 E5 D5 C5 B5 A5"))
 ,Direction(MD_RIGHT    ,queenMoves_G5_4,_T("queen MD_RIGHT from G5:H5"))
 ,Direction(MD_UPDIAG2  ,queenMoves_G5_5,_T("queen MD_UPDIAG2 from G5:F6 E7 D8"))
 ,Direction(MD_UP       ,queenMoves_G5_6,_T("queen MD_UP from G5:G6 G7 G8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_G5_7,_T("queen MD_UPDIAG1 from G5:H6"))
};
#endif
static const DirectionArray queenMoves_G5(8,queenMoveDirections_G5);

static const int queenMoves_H5_0[5] = { 4,G4,F3,E2,D1 };
static const int queenMoves_H5_1[5] = { 4,H4,H3,H2,H1 };
static const int queenMoves_H5_2[8] = { 7,G5,F5,E5,D5,C5,B5,A5 };
static const int queenMoves_H5_3[4] = { 3,G6,F7,E8 };
static const int queenMoves_H5_4[4] = { 3,H6,H7,H8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_H5[5] = {
  Direction(MD_DOWNDIAG1,queenMoves_H5_0)
 ,Direction(MD_DOWN     ,queenMoves_H5_1)
 ,Direction(MD_LEFT     ,queenMoves_H5_2)
 ,Direction(MD_UPDIAG2  ,queenMoves_H5_3)
 ,Direction(MD_UP       ,queenMoves_H5_4)
};
#else
static const Direction queenMoveDirections_H5[5] = {
  Direction(MD_DOWNDIAG1,queenMoves_H5_0,_T("queen MD_DOWNDIAG1 from H5:G4 F3 E2 D1"))
 ,Direction(MD_DOWN     ,queenMoves_H5_1,_T("queen MD_DOWN from H5:H4 H3 H2 H1"))
 ,Direction(MD_LEFT     ,queenMoves_H5_2,_T("queen MD_LEFT from H5:G5 F5 E5 D5 C5 B5 A5"))
 ,Direction(MD_UPDIAG2  ,queenMoves_H5_3,_T("queen MD_UPDIAG2 from H5:G6 F7 E8"))
 ,Direction(MD_UP       ,queenMoves_H5_4,_T("queen MD_UP from H5:H6 H7 H8"))
};
#endif
static const DirectionArray queenMoves_H5(5,queenMoveDirections_H5);

static const int queenMoves_A6_0[6] = { 5,A5,A4,A3,A2,A1 };
static const int queenMoves_A6_1[6] = { 5,B5,C4,D3,E2,F1 };
static const int queenMoves_A6_2[8] = { 7,B6,C6,D6,E6,F6,G6,H6 };
static const int queenMoves_A6_3[3] = { 2,A7,A8 };
static const int queenMoves_A6_4[3] = { 2,B7,C8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_A6[5] = {
  Direction(MD_DOWN     ,queenMoves_A6_0)
 ,Direction(MD_DOWNDIAG2,queenMoves_A6_1)
 ,Direction(MD_RIGHT    ,queenMoves_A6_2)
 ,Direction(MD_UP       ,queenMoves_A6_3)
 ,Direction(MD_UPDIAG1  ,queenMoves_A6_4)
};
#else
static const Direction queenMoveDirections_A6[5] = {
  Direction(MD_DOWN     ,queenMoves_A6_0,_T("queen MD_DOWN from A6:A5 A4 A3 A2 A1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_A6_1,_T("queen MD_DOWNDIAG2 from A6:B5 C4 D3 E2 F1"))
 ,Direction(MD_RIGHT    ,queenMoves_A6_2,_T("queen MD_RIGHT from A6:B6 C6 D6 E6 F6 G6 H6"))
 ,Direction(MD_UP       ,queenMoves_A6_3,_T("queen MD_UP from A6:A7 A8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_A6_4,_T("queen MD_UPDIAG1 from A6:B7 C8"))
};
#endif
static const DirectionArray queenMoves_A6(5,queenMoveDirections_A6);

static const int queenMoves_B6_0[2] = { 1,A5 };
static const int queenMoves_B6_1[6] = { 5,B5,B4,B3,B2,B1 };
static const int queenMoves_B6_2[6] = { 5,C5,D4,E3,F2,G1 };
static const int queenMoves_B6_3[2] = { 1,A6 };
static const int queenMoves_B6_4[7] = { 6,C6,D6,E6,F6,G6,H6 };
static const int queenMoves_B6_5[2] = { 1,A7 };
static const int queenMoves_B6_6[3] = { 2,B7,B8 };
static const int queenMoves_B6_7[3] = { 2,C7,D8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_B6[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_B6_0)
 ,Direction(MD_DOWN     ,queenMoves_B6_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_B6_2)
 ,Direction(MD_LEFT     ,queenMoves_B6_3)
 ,Direction(MD_RIGHT    ,queenMoves_B6_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_B6_5)
 ,Direction(MD_UP       ,queenMoves_B6_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_B6_7)
};
#else
static const Direction queenMoveDirections_B6[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_B6_0,_T("queen MD_DOWNDIAG1 from B6:A5"))
 ,Direction(MD_DOWN     ,queenMoves_B6_1,_T("queen MD_DOWN from B6:B5 B4 B3 B2 B1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_B6_2,_T("queen MD_DOWNDIAG2 from B6:C5 D4 E3 F2 G1"))
 ,Direction(MD_LEFT     ,queenMoves_B6_3,_T("queen MD_LEFT from B6:A6"))
 ,Direction(MD_RIGHT    ,queenMoves_B6_4,_T("queen MD_RIGHT from B6:C6 D6 E6 F6 G6 H6"))
 ,Direction(MD_UPDIAG2  ,queenMoves_B6_5,_T("queen MD_UPDIAG2 from B6:A7"))
 ,Direction(MD_UP       ,queenMoves_B6_6,_T("queen MD_UP from B6:B7 B8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_B6_7,_T("queen MD_UPDIAG1 from B6:C7 D8"))
};
#endif
static const DirectionArray queenMoves_B6(8,queenMoveDirections_B6);

static const int queenMoves_C6_0[3] = { 2,B5,A4 };
static const int queenMoves_C6_1[6] = { 5,C5,C4,C3,C2,C1 };
static const int queenMoves_C6_2[6] = { 5,D5,E4,F3,G2,H1 };
static const int queenMoves_C6_3[3] = { 2,B6,A6 };
static const int queenMoves_C6_4[6] = { 5,D6,E6,F6,G6,H6 };
static const int queenMoves_C6_5[3] = { 2,B7,A8 };
static const int queenMoves_C6_6[3] = { 2,C7,C8 };
static const int queenMoves_C6_7[3] = { 2,D7,E8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_C6[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_C6_0)
 ,Direction(MD_DOWN     ,queenMoves_C6_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_C6_2)
 ,Direction(MD_LEFT     ,queenMoves_C6_3)
 ,Direction(MD_RIGHT    ,queenMoves_C6_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_C6_5)
 ,Direction(MD_UP       ,queenMoves_C6_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_C6_7)
};
#else
static const Direction queenMoveDirections_C6[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_C6_0,_T("queen MD_DOWNDIAG1 from C6:B5 A4"))
 ,Direction(MD_DOWN     ,queenMoves_C6_1,_T("queen MD_DOWN from C6:C5 C4 C3 C2 C1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_C6_2,_T("queen MD_DOWNDIAG2 from C6:D5 E4 F3 G2 H1"))
 ,Direction(MD_LEFT     ,queenMoves_C6_3,_T("queen MD_LEFT from C6:B6 A6"))
 ,Direction(MD_RIGHT    ,queenMoves_C6_4,_T("queen MD_RIGHT from C6:D6 E6 F6 G6 H6"))
 ,Direction(MD_UPDIAG2  ,queenMoves_C6_5,_T("queen MD_UPDIAG2 from C6:B7 A8"))
 ,Direction(MD_UP       ,queenMoves_C6_6,_T("queen MD_UP from C6:C7 C8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_C6_7,_T("queen MD_UPDIAG1 from C6:D7 E8"))
};
#endif
static const DirectionArray queenMoves_C6(8,queenMoveDirections_C6);

static const int queenMoves_D6_0[4] = { 3,C5,B4,A3 };
static const int queenMoves_D6_1[6] = { 5,D5,D4,D3,D2,D1 };
static const int queenMoves_D6_2[5] = { 4,E5,F4,G3,H2 };
static const int queenMoves_D6_3[4] = { 3,C6,B6,A6 };
static const int queenMoves_D6_4[5] = { 4,E6,F6,G6,H6 };
static const int queenMoves_D6_5[3] = { 2,C7,B8 };
static const int queenMoves_D6_6[3] = { 2,D7,D8 };
static const int queenMoves_D6_7[3] = { 2,E7,F8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_D6[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_D6_0)
 ,Direction(MD_DOWN     ,queenMoves_D6_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_D6_2)
 ,Direction(MD_LEFT     ,queenMoves_D6_3)
 ,Direction(MD_RIGHT    ,queenMoves_D6_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_D6_5)
 ,Direction(MD_UP       ,queenMoves_D6_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_D6_7)
};
#else
static const Direction queenMoveDirections_D6[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_D6_0,_T("queen MD_DOWNDIAG1 from D6:C5 B4 A3"))
 ,Direction(MD_DOWN     ,queenMoves_D6_1,_T("queen MD_DOWN from D6:D5 D4 D3 D2 D1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_D6_2,_T("queen MD_DOWNDIAG2 from D6:E5 F4 G3 H2"))
 ,Direction(MD_LEFT     ,queenMoves_D6_3,_T("queen MD_LEFT from D6:C6 B6 A6"))
 ,Direction(MD_RIGHT    ,queenMoves_D6_4,_T("queen MD_RIGHT from D6:E6 F6 G6 H6"))
 ,Direction(MD_UPDIAG2  ,queenMoves_D6_5,_T("queen MD_UPDIAG2 from D6:C7 B8"))
 ,Direction(MD_UP       ,queenMoves_D6_6,_T("queen MD_UP from D6:D7 D8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_D6_7,_T("queen MD_UPDIAG1 from D6:E7 F8"))
};
#endif
static const DirectionArray queenMoves_D6(8,queenMoveDirections_D6);

static const int queenMoves_E6_0[5] = { 4,D5,C4,B3,A2 };
static const int queenMoves_E6_1[6] = { 5,E5,E4,E3,E2,E1 };
static const int queenMoves_E6_2[4] = { 3,F5,G4,H3 };
static const int queenMoves_E6_3[5] = { 4,D6,C6,B6,A6 };
static const int queenMoves_E6_4[4] = { 3,F6,G6,H6 };
static const int queenMoves_E6_5[3] = { 2,D7,C8 };
static const int queenMoves_E6_6[3] = { 2,E7,E8 };
static const int queenMoves_E6_7[3] = { 2,F7,G8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_E6[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_E6_0)
 ,Direction(MD_DOWN     ,queenMoves_E6_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_E6_2)
 ,Direction(MD_LEFT     ,queenMoves_E6_3)
 ,Direction(MD_RIGHT    ,queenMoves_E6_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_E6_5)
 ,Direction(MD_UP       ,queenMoves_E6_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_E6_7)
};
#else
static const Direction queenMoveDirections_E6[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_E6_0,_T("queen MD_DOWNDIAG1 from E6:D5 C4 B3 A2"))
 ,Direction(MD_DOWN     ,queenMoves_E6_1,_T("queen MD_DOWN from E6:E5 E4 E3 E2 E1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_E6_2,_T("queen MD_DOWNDIAG2 from E6:F5 G4 H3"))
 ,Direction(MD_LEFT     ,queenMoves_E6_3,_T("queen MD_LEFT from E6:D6 C6 B6 A6"))
 ,Direction(MD_RIGHT    ,queenMoves_E6_4,_T("queen MD_RIGHT from E6:F6 G6 H6"))
 ,Direction(MD_UPDIAG2  ,queenMoves_E6_5,_T("queen MD_UPDIAG2 from E6:D7 C8"))
 ,Direction(MD_UP       ,queenMoves_E6_6,_T("queen MD_UP from E6:E7 E8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_E6_7,_T("queen MD_UPDIAG1 from E6:F7 G8"))
};
#endif
static const DirectionArray queenMoves_E6(8,queenMoveDirections_E6);

static const int queenMoves_F6_0[6] = { 5,E5,D4,C3,B2,A1 };
static const int queenMoves_F6_1[6] = { 5,F5,F4,F3,F2,F1 };
static const int queenMoves_F6_2[3] = { 2,G5,H4 };
static const int queenMoves_F6_3[6] = { 5,E6,D6,C6,B6,A6 };
static const int queenMoves_F6_4[3] = { 2,G6,H6 };
static const int queenMoves_F6_5[3] = { 2,E7,D8 };
static const int queenMoves_F6_6[3] = { 2,F7,F8 };
static const int queenMoves_F6_7[3] = { 2,G7,H8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_F6[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_F6_0)
 ,Direction(MD_DOWN     ,queenMoves_F6_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_F6_2)
 ,Direction(MD_LEFT     ,queenMoves_F6_3)
 ,Direction(MD_RIGHT    ,queenMoves_F6_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_F6_5)
 ,Direction(MD_UP       ,queenMoves_F6_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_F6_7)
};
#else
static const Direction queenMoveDirections_F6[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_F6_0,_T("queen MD_DOWNDIAG1 from F6:E5 D4 C3 B2 A1"))
 ,Direction(MD_DOWN     ,queenMoves_F6_1,_T("queen MD_DOWN from F6:F5 F4 F3 F2 F1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_F6_2,_T("queen MD_DOWNDIAG2 from F6:G5 H4"))
 ,Direction(MD_LEFT     ,queenMoves_F6_3,_T("queen MD_LEFT from F6:E6 D6 C6 B6 A6"))
 ,Direction(MD_RIGHT    ,queenMoves_F6_4,_T("queen MD_RIGHT from F6:G6 H6"))
 ,Direction(MD_UPDIAG2  ,queenMoves_F6_5,_T("queen MD_UPDIAG2 from F6:E7 D8"))
 ,Direction(MD_UP       ,queenMoves_F6_6,_T("queen MD_UP from F6:F7 F8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_F6_7,_T("queen MD_UPDIAG1 from F6:G7 H8"))
};
#endif
static const DirectionArray queenMoves_F6(8,queenMoveDirections_F6);

static const int queenMoves_G6_0[6] = { 5,F5,E4,D3,C2,B1 };
static const int queenMoves_G6_1[6] = { 5,G5,G4,G3,G2,G1 };
static const int queenMoves_G6_2[2] = { 1,H5 };
static const int queenMoves_G6_3[7] = { 6,F6,E6,D6,C6,B6,A6 };
static const int queenMoves_G6_4[2] = { 1,H6 };
static const int queenMoves_G6_5[3] = { 2,F7,E8 };
static const int queenMoves_G6_6[3] = { 2,G7,G8 };
static const int queenMoves_G6_7[2] = { 1,H7 };
#ifndef _DEBUG
static const Direction queenMoveDirections_G6[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_G6_0)
 ,Direction(MD_DOWN     ,queenMoves_G6_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_G6_2)
 ,Direction(MD_LEFT     ,queenMoves_G6_3)
 ,Direction(MD_RIGHT    ,queenMoves_G6_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_G6_5)
 ,Direction(MD_UP       ,queenMoves_G6_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_G6_7)
};
#else
static const Direction queenMoveDirections_G6[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_G6_0,_T("queen MD_DOWNDIAG1 from G6:F5 E4 D3 C2 B1"))
 ,Direction(MD_DOWN     ,queenMoves_G6_1,_T("queen MD_DOWN from G6:G5 G4 G3 G2 G1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_G6_2,_T("queen MD_DOWNDIAG2 from G6:H5"))
 ,Direction(MD_LEFT     ,queenMoves_G6_3,_T("queen MD_LEFT from G6:F6 E6 D6 C6 B6 A6"))
 ,Direction(MD_RIGHT    ,queenMoves_G6_4,_T("queen MD_RIGHT from G6:H6"))
 ,Direction(MD_UPDIAG2  ,queenMoves_G6_5,_T("queen MD_UPDIAG2 from G6:F7 E8"))
 ,Direction(MD_UP       ,queenMoves_G6_6,_T("queen MD_UP from G6:G7 G8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_G6_7,_T("queen MD_UPDIAG1 from G6:H7"))
};
#endif
static const DirectionArray queenMoves_G6(8,queenMoveDirections_G6);

static const int queenMoves_H6_0[6] = { 5,G5,F4,E3,D2,C1 };
static const int queenMoves_H6_1[6] = { 5,H5,H4,H3,H2,H1 };
static const int queenMoves_H6_2[8] = { 7,G6,F6,E6,D6,C6,B6,A6 };
static const int queenMoves_H6_3[3] = { 2,G7,F8 };
static const int queenMoves_H6_4[3] = { 2,H7,H8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_H6[5] = {
  Direction(MD_DOWNDIAG1,queenMoves_H6_0)
 ,Direction(MD_DOWN     ,queenMoves_H6_1)
 ,Direction(MD_LEFT     ,queenMoves_H6_2)
 ,Direction(MD_UPDIAG2  ,queenMoves_H6_3)
 ,Direction(MD_UP       ,queenMoves_H6_4)
};
#else
static const Direction queenMoveDirections_H6[5] = {
  Direction(MD_DOWNDIAG1,queenMoves_H6_0,_T("queen MD_DOWNDIAG1 from H6:G5 F4 E3 D2 C1"))
 ,Direction(MD_DOWN     ,queenMoves_H6_1,_T("queen MD_DOWN from H6:H5 H4 H3 H2 H1"))
 ,Direction(MD_LEFT     ,queenMoves_H6_2,_T("queen MD_LEFT from H6:G6 F6 E6 D6 C6 B6 A6"))
 ,Direction(MD_UPDIAG2  ,queenMoves_H6_3,_T("queen MD_UPDIAG2 from H6:G7 F8"))
 ,Direction(MD_UP       ,queenMoves_H6_4,_T("queen MD_UP from H6:H7 H8"))
};
#endif
static const DirectionArray queenMoves_H6(5,queenMoveDirections_H6);

static const int queenMoves_A7_0[7] = { 6,A6,A5,A4,A3,A2,A1 };
static const int queenMoves_A7_1[7] = { 6,B6,C5,D4,E3,F2,G1 };
static const int queenMoves_A7_2[8] = { 7,B7,C7,D7,E7,F7,G7,H7 };
static const int queenMoves_A7_3[2] = { 1,A8 };
static const int queenMoves_A7_4[2] = { 1,B8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_A7[5] = {
  Direction(MD_DOWN     ,queenMoves_A7_0)
 ,Direction(MD_DOWNDIAG2,queenMoves_A7_1)
 ,Direction(MD_RIGHT    ,queenMoves_A7_2)
 ,Direction(MD_UP       ,queenMoves_A7_3)
 ,Direction(MD_UPDIAG1  ,queenMoves_A7_4)
};
#else
static const Direction queenMoveDirections_A7[5] = {
  Direction(MD_DOWN     ,queenMoves_A7_0,_T("queen MD_DOWN from A7:A6 A5 A4 A3 A2 A1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_A7_1,_T("queen MD_DOWNDIAG2 from A7:B6 C5 D4 E3 F2 G1"))
 ,Direction(MD_RIGHT    ,queenMoves_A7_2,_T("queen MD_RIGHT from A7:B7 C7 D7 E7 F7 G7 H7"))
 ,Direction(MD_UP       ,queenMoves_A7_3,_T("queen MD_UP from A7:A8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_A7_4,_T("queen MD_UPDIAG1 from A7:B8"))
};
#endif
static const DirectionArray queenMoves_A7(5,queenMoveDirections_A7);

static const int queenMoves_B7_0[2] = { 1,A6 };
static const int queenMoves_B7_1[7] = { 6,B6,B5,B4,B3,B2,B1 };
static const int queenMoves_B7_2[7] = { 6,C6,D5,E4,F3,G2,H1 };
static const int queenMoves_B7_3[2] = { 1,A7 };
static const int queenMoves_B7_4[7] = { 6,C7,D7,E7,F7,G7,H7 };
static const int queenMoves_B7_5[2] = { 1,A8 };
static const int queenMoves_B7_6[2] = { 1,B8 };
static const int queenMoves_B7_7[2] = { 1,C8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_B7[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_B7_0)
 ,Direction(MD_DOWN     ,queenMoves_B7_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_B7_2)
 ,Direction(MD_LEFT     ,queenMoves_B7_3)
 ,Direction(MD_RIGHT    ,queenMoves_B7_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_B7_5)
 ,Direction(MD_UP       ,queenMoves_B7_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_B7_7)
};
#else
static const Direction queenMoveDirections_B7[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_B7_0,_T("queen MD_DOWNDIAG1 from B7:A6"))
 ,Direction(MD_DOWN     ,queenMoves_B7_1,_T("queen MD_DOWN from B7:B6 B5 B4 B3 B2 B1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_B7_2,_T("queen MD_DOWNDIAG2 from B7:C6 D5 E4 F3 G2 H1"))
 ,Direction(MD_LEFT     ,queenMoves_B7_3,_T("queen MD_LEFT from B7:A7"))
 ,Direction(MD_RIGHT    ,queenMoves_B7_4,_T("queen MD_RIGHT from B7:C7 D7 E7 F7 G7 H7"))
 ,Direction(MD_UPDIAG2  ,queenMoves_B7_5,_T("queen MD_UPDIAG2 from B7:A8"))
 ,Direction(MD_UP       ,queenMoves_B7_6,_T("queen MD_UP from B7:B8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_B7_7,_T("queen MD_UPDIAG1 from B7:C8"))
};
#endif
static const DirectionArray queenMoves_B7(8,queenMoveDirections_B7);

static const int queenMoves_C7_0[3] = { 2,B6,A5 };
static const int queenMoves_C7_1[7] = { 6,C6,C5,C4,C3,C2,C1 };
static const int queenMoves_C7_2[6] = { 5,D6,E5,F4,G3,H2 };
static const int queenMoves_C7_3[3] = { 2,B7,A7 };
static const int queenMoves_C7_4[6] = { 5,D7,E7,F7,G7,H7 };
static const int queenMoves_C7_5[2] = { 1,B8 };
static const int queenMoves_C7_6[2] = { 1,C8 };
static const int queenMoves_C7_7[2] = { 1,D8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_C7[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_C7_0)
 ,Direction(MD_DOWN     ,queenMoves_C7_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_C7_2)
 ,Direction(MD_LEFT     ,queenMoves_C7_3)
 ,Direction(MD_RIGHT    ,queenMoves_C7_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_C7_5)
 ,Direction(MD_UP       ,queenMoves_C7_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_C7_7)
};
#else
static const Direction queenMoveDirections_C7[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_C7_0,_T("queen MD_DOWNDIAG1 from C7:B6 A5"))
 ,Direction(MD_DOWN     ,queenMoves_C7_1,_T("queen MD_DOWN from C7:C6 C5 C4 C3 C2 C1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_C7_2,_T("queen MD_DOWNDIAG2 from C7:D6 E5 F4 G3 H2"))
 ,Direction(MD_LEFT     ,queenMoves_C7_3,_T("queen MD_LEFT from C7:B7 A7"))
 ,Direction(MD_RIGHT    ,queenMoves_C7_4,_T("queen MD_RIGHT from C7:D7 E7 F7 G7 H7"))
 ,Direction(MD_UPDIAG2  ,queenMoves_C7_5,_T("queen MD_UPDIAG2 from C7:B8"))
 ,Direction(MD_UP       ,queenMoves_C7_6,_T("queen MD_UP from C7:C8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_C7_7,_T("queen MD_UPDIAG1 from C7:D8"))
};
#endif
static const DirectionArray queenMoves_C7(8,queenMoveDirections_C7);

static const int queenMoves_D7_0[4] = { 3,C6,B5,A4 };
static const int queenMoves_D7_1[7] = { 6,D6,D5,D4,D3,D2,D1 };
static const int queenMoves_D7_2[5] = { 4,E6,F5,G4,H3 };
static const int queenMoves_D7_3[4] = { 3,C7,B7,A7 };
static const int queenMoves_D7_4[5] = { 4,E7,F7,G7,H7 };
static const int queenMoves_D7_5[2] = { 1,C8 };
static const int queenMoves_D7_6[2] = { 1,D8 };
static const int queenMoves_D7_7[2] = { 1,E8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_D7[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_D7_0)
 ,Direction(MD_DOWN     ,queenMoves_D7_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_D7_2)
 ,Direction(MD_LEFT     ,queenMoves_D7_3)
 ,Direction(MD_RIGHT    ,queenMoves_D7_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_D7_5)
 ,Direction(MD_UP       ,queenMoves_D7_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_D7_7)
};
#else
static const Direction queenMoveDirections_D7[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_D7_0,_T("queen MD_DOWNDIAG1 from D7:C6 B5 A4"))
 ,Direction(MD_DOWN     ,queenMoves_D7_1,_T("queen MD_DOWN from D7:D6 D5 D4 D3 D2 D1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_D7_2,_T("queen MD_DOWNDIAG2 from D7:E6 F5 G4 H3"))
 ,Direction(MD_LEFT     ,queenMoves_D7_3,_T("queen MD_LEFT from D7:C7 B7 A7"))
 ,Direction(MD_RIGHT    ,queenMoves_D7_4,_T("queen MD_RIGHT from D7:E7 F7 G7 H7"))
 ,Direction(MD_UPDIAG2  ,queenMoves_D7_5,_T("queen MD_UPDIAG2 from D7:C8"))
 ,Direction(MD_UP       ,queenMoves_D7_6,_T("queen MD_UP from D7:D8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_D7_7,_T("queen MD_UPDIAG1 from D7:E8"))
};
#endif
static const DirectionArray queenMoves_D7(8,queenMoveDirections_D7);

static const int queenMoves_E7_0[5] = { 4,D6,C5,B4,A3 };
static const int queenMoves_E7_1[7] = { 6,E6,E5,E4,E3,E2,E1 };
static const int queenMoves_E7_2[4] = { 3,F6,G5,H4 };
static const int queenMoves_E7_3[5] = { 4,D7,C7,B7,A7 };
static const int queenMoves_E7_4[4] = { 3,F7,G7,H7 };
static const int queenMoves_E7_5[2] = { 1,D8 };
static const int queenMoves_E7_6[2] = { 1,E8 };
static const int queenMoves_E7_7[2] = { 1,F8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_E7[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_E7_0)
 ,Direction(MD_DOWN     ,queenMoves_E7_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_E7_2)
 ,Direction(MD_LEFT     ,queenMoves_E7_3)
 ,Direction(MD_RIGHT    ,queenMoves_E7_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_E7_5)
 ,Direction(MD_UP       ,queenMoves_E7_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_E7_7)
};
#else
static const Direction queenMoveDirections_E7[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_E7_0,_T("queen MD_DOWNDIAG1 from E7:D6 C5 B4 A3"))
 ,Direction(MD_DOWN     ,queenMoves_E7_1,_T("queen MD_DOWN from E7:E6 E5 E4 E3 E2 E1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_E7_2,_T("queen MD_DOWNDIAG2 from E7:F6 G5 H4"))
 ,Direction(MD_LEFT     ,queenMoves_E7_3,_T("queen MD_LEFT from E7:D7 C7 B7 A7"))
 ,Direction(MD_RIGHT    ,queenMoves_E7_4,_T("queen MD_RIGHT from E7:F7 G7 H7"))
 ,Direction(MD_UPDIAG2  ,queenMoves_E7_5,_T("queen MD_UPDIAG2 from E7:D8"))
 ,Direction(MD_UP       ,queenMoves_E7_6,_T("queen MD_UP from E7:E8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_E7_7,_T("queen MD_UPDIAG1 from E7:F8"))
};
#endif
static const DirectionArray queenMoves_E7(8,queenMoveDirections_E7);

static const int queenMoves_F7_0[6] = { 5,E6,D5,C4,B3,A2 };
static const int queenMoves_F7_1[7] = { 6,F6,F5,F4,F3,F2,F1 };
static const int queenMoves_F7_2[3] = { 2,G6,H5 };
static const int queenMoves_F7_3[6] = { 5,E7,D7,C7,B7,A7 };
static const int queenMoves_F7_4[3] = { 2,G7,H7 };
static const int queenMoves_F7_5[2] = { 1,E8 };
static const int queenMoves_F7_6[2] = { 1,F8 };
static const int queenMoves_F7_7[2] = { 1,G8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_F7[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_F7_0)
 ,Direction(MD_DOWN     ,queenMoves_F7_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_F7_2)
 ,Direction(MD_LEFT     ,queenMoves_F7_3)
 ,Direction(MD_RIGHT    ,queenMoves_F7_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_F7_5)
 ,Direction(MD_UP       ,queenMoves_F7_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_F7_7)
};
#else
static const Direction queenMoveDirections_F7[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_F7_0,_T("queen MD_DOWNDIAG1 from F7:E6 D5 C4 B3 A2"))
 ,Direction(MD_DOWN     ,queenMoves_F7_1,_T("queen MD_DOWN from F7:F6 F5 F4 F3 F2 F1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_F7_2,_T("queen MD_DOWNDIAG2 from F7:G6 H5"))
 ,Direction(MD_LEFT     ,queenMoves_F7_3,_T("queen MD_LEFT from F7:E7 D7 C7 B7 A7"))
 ,Direction(MD_RIGHT    ,queenMoves_F7_4,_T("queen MD_RIGHT from F7:G7 H7"))
 ,Direction(MD_UPDIAG2  ,queenMoves_F7_5,_T("queen MD_UPDIAG2 from F7:E8"))
 ,Direction(MD_UP       ,queenMoves_F7_6,_T("queen MD_UP from F7:F8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_F7_7,_T("queen MD_UPDIAG1 from F7:G8"))
};
#endif
static const DirectionArray queenMoves_F7(8,queenMoveDirections_F7);

static const int queenMoves_G7_0[7] = { 6,F6,E5,D4,C3,B2,A1 };
static const int queenMoves_G7_1[7] = { 6,G6,G5,G4,G3,G2,G1 };
static const int queenMoves_G7_2[2] = { 1,H6 };
static const int queenMoves_G7_3[7] = { 6,F7,E7,D7,C7,B7,A7 };
static const int queenMoves_G7_4[2] = { 1,H7 };
static const int queenMoves_G7_5[2] = { 1,F8 };
static const int queenMoves_G7_6[2] = { 1,G8 };
static const int queenMoves_G7_7[2] = { 1,H8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_G7[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_G7_0)
 ,Direction(MD_DOWN     ,queenMoves_G7_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_G7_2)
 ,Direction(MD_LEFT     ,queenMoves_G7_3)
 ,Direction(MD_RIGHT    ,queenMoves_G7_4)
 ,Direction(MD_UPDIAG2  ,queenMoves_G7_5)
 ,Direction(MD_UP       ,queenMoves_G7_6)
 ,Direction(MD_UPDIAG1  ,queenMoves_G7_7)
};
#else
static const Direction queenMoveDirections_G7[8] = {
  Direction(MD_DOWNDIAG1,queenMoves_G7_0,_T("queen MD_DOWNDIAG1 from G7:F6 E5 D4 C3 B2 A1"))
 ,Direction(MD_DOWN     ,queenMoves_G7_1,_T("queen MD_DOWN from G7:G6 G5 G4 G3 G2 G1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_G7_2,_T("queen MD_DOWNDIAG2 from G7:H6"))
 ,Direction(MD_LEFT     ,queenMoves_G7_3,_T("queen MD_LEFT from G7:F7 E7 D7 C7 B7 A7"))
 ,Direction(MD_RIGHT    ,queenMoves_G7_4,_T("queen MD_RIGHT from G7:H7"))
 ,Direction(MD_UPDIAG2  ,queenMoves_G7_5,_T("queen MD_UPDIAG2 from G7:F8"))
 ,Direction(MD_UP       ,queenMoves_G7_6,_T("queen MD_UP from G7:G8"))
 ,Direction(MD_UPDIAG1  ,queenMoves_G7_7,_T("queen MD_UPDIAG1 from G7:H8"))
};
#endif
static const DirectionArray queenMoves_G7(8,queenMoveDirections_G7);

static const int queenMoves_H7_0[7] = { 6,G6,F5,E4,D3,C2,B1 };
static const int queenMoves_H7_1[7] = { 6,H6,H5,H4,H3,H2,H1 };
static const int queenMoves_H7_2[8] = { 7,G7,F7,E7,D7,C7,B7,A7 };
static const int queenMoves_H7_3[2] = { 1,G8 };
static const int queenMoves_H7_4[2] = { 1,H8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_H7[5] = {
  Direction(MD_DOWNDIAG1,queenMoves_H7_0)
 ,Direction(MD_DOWN     ,queenMoves_H7_1)
 ,Direction(MD_LEFT     ,queenMoves_H7_2)
 ,Direction(MD_UPDIAG2  ,queenMoves_H7_3)
 ,Direction(MD_UP       ,queenMoves_H7_4)
};
#else
static const Direction queenMoveDirections_H7[5] = {
  Direction(MD_DOWNDIAG1,queenMoves_H7_0,_T("queen MD_DOWNDIAG1 from H7:G6 F5 E4 D3 C2 B1"))
 ,Direction(MD_DOWN     ,queenMoves_H7_1,_T("queen MD_DOWN from H7:H6 H5 H4 H3 H2 H1"))
 ,Direction(MD_LEFT     ,queenMoves_H7_2,_T("queen MD_LEFT from H7:G7 F7 E7 D7 C7 B7 A7"))
 ,Direction(MD_UPDIAG2  ,queenMoves_H7_3,_T("queen MD_UPDIAG2 from H7:G8"))
 ,Direction(MD_UP       ,queenMoves_H7_4,_T("queen MD_UP from H7:H8"))
};
#endif
static const DirectionArray queenMoves_H7(5,queenMoveDirections_H7);

static const int queenMoves_A8_0[8] = { 7,A7,A6,A5,A4,A3,A2,A1 };
static const int queenMoves_A8_1[8] = { 7,B7,C6,D5,E4,F3,G2,H1 };
static const int queenMoves_A8_2[8] = { 7,B8,C8,D8,E8,F8,G8,H8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_A8[3] = {
  Direction(MD_DOWN     ,queenMoves_A8_0)
 ,Direction(MD_DOWNDIAG2,queenMoves_A8_1)
 ,Direction(MD_RIGHT    ,queenMoves_A8_2)
};
#else
static const Direction queenMoveDirections_A8[3] = {
  Direction(MD_DOWN     ,queenMoves_A8_0,_T("queen MD_DOWN from A8:A7 A6 A5 A4 A3 A2 A1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_A8_1,_T("queen MD_DOWNDIAG2 from A8:B7 C6 D5 E4 F3 G2 H1"))
 ,Direction(MD_RIGHT    ,queenMoves_A8_2,_T("queen MD_RIGHT from A8:B8 C8 D8 E8 F8 G8 H8"))
};
#endif
static const DirectionArray queenMoves_A8(3,queenMoveDirections_A8);

static const int queenMoves_B8_0[2] = { 1,A7 };
static const int queenMoves_B8_1[8] = { 7,B7,B6,B5,B4,B3,B2,B1 };
static const int queenMoves_B8_2[7] = { 6,C7,D6,E5,F4,G3,H2 };
static const int queenMoves_B8_3[2] = { 1,A8 };
static const int queenMoves_B8_4[7] = { 6,C8,D8,E8,F8,G8,H8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_B8[5] = {
  Direction(MD_DOWNDIAG1,queenMoves_B8_0)
 ,Direction(MD_DOWN     ,queenMoves_B8_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_B8_2)
 ,Direction(MD_LEFT     ,queenMoves_B8_3)
 ,Direction(MD_RIGHT    ,queenMoves_B8_4)
};
#else
static const Direction queenMoveDirections_B8[5] = {
  Direction(MD_DOWNDIAG1,queenMoves_B8_0,_T("queen MD_DOWNDIAG1 from B8:A7"))
 ,Direction(MD_DOWN     ,queenMoves_B8_1,_T("queen MD_DOWN from B8:B7 B6 B5 B4 B3 B2 B1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_B8_2,_T("queen MD_DOWNDIAG2 from B8:C7 D6 E5 F4 G3 H2"))
 ,Direction(MD_LEFT     ,queenMoves_B8_3,_T("queen MD_LEFT from B8:A8"))
 ,Direction(MD_RIGHT    ,queenMoves_B8_4,_T("queen MD_RIGHT from B8:C8 D8 E8 F8 G8 H8"))
};
#endif
static const DirectionArray queenMoves_B8(5,queenMoveDirections_B8);

static const int queenMoves_C8_0[3] = { 2,B7,A6 };
static const int queenMoves_C8_1[8] = { 7,C7,C6,C5,C4,C3,C2,C1 };
static const int queenMoves_C8_2[6] = { 5,D7,E6,F5,G4,H3 };
static const int queenMoves_C8_3[3] = { 2,B8,A8 };
static const int queenMoves_C8_4[6] = { 5,D8,E8,F8,G8,H8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_C8[5] = {
  Direction(MD_DOWNDIAG1,queenMoves_C8_0)
 ,Direction(MD_DOWN     ,queenMoves_C8_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_C8_2)
 ,Direction(MD_LEFT     ,queenMoves_C8_3)
 ,Direction(MD_RIGHT    ,queenMoves_C8_4)
};
#else
static const Direction queenMoveDirections_C8[5] = {
  Direction(MD_DOWNDIAG1,queenMoves_C8_0,_T("queen MD_DOWNDIAG1 from C8:B7 A6"))
 ,Direction(MD_DOWN     ,queenMoves_C8_1,_T("queen MD_DOWN from C8:C7 C6 C5 C4 C3 C2 C1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_C8_2,_T("queen MD_DOWNDIAG2 from C8:D7 E6 F5 G4 H3"))
 ,Direction(MD_LEFT     ,queenMoves_C8_3,_T("queen MD_LEFT from C8:B8 A8"))
 ,Direction(MD_RIGHT    ,queenMoves_C8_4,_T("queen MD_RIGHT from C8:D8 E8 F8 G8 H8"))
};
#endif
static const DirectionArray queenMoves_C8(5,queenMoveDirections_C8);

static const int queenMoves_D8_0[4] = { 3,C7,B6,A5 };
static const int queenMoves_D8_1[8] = { 7,D7,D6,D5,D4,D3,D2,D1 };
static const int queenMoves_D8_2[5] = { 4,E7,F6,G5,H4 };
static const int queenMoves_D8_3[4] = { 3,C8,B8,A8 };
static const int queenMoves_D8_4[5] = { 4,E8,F8,G8,H8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_D8[5] = {
  Direction(MD_DOWNDIAG1,queenMoves_D8_0)
 ,Direction(MD_DOWN     ,queenMoves_D8_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_D8_2)
 ,Direction(MD_LEFT     ,queenMoves_D8_3)
 ,Direction(MD_RIGHT    ,queenMoves_D8_4)
};
#else
static const Direction queenMoveDirections_D8[5] = {
  Direction(MD_DOWNDIAG1,queenMoves_D8_0,_T("queen MD_DOWNDIAG1 from D8:C7 B6 A5"))
 ,Direction(MD_DOWN     ,queenMoves_D8_1,_T("queen MD_DOWN from D8:D7 D6 D5 D4 D3 D2 D1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_D8_2,_T("queen MD_DOWNDIAG2 from D8:E7 F6 G5 H4"))
 ,Direction(MD_LEFT     ,queenMoves_D8_3,_T("queen MD_LEFT from D8:C8 B8 A8"))
 ,Direction(MD_RIGHT    ,queenMoves_D8_4,_T("queen MD_RIGHT from D8:E8 F8 G8 H8"))
};
#endif
static const DirectionArray queenMoves_D8(5,queenMoveDirections_D8);

static const int queenMoves_E8_0[5] = { 4,D7,C6,B5,A4 };
static const int queenMoves_E8_1[8] = { 7,E7,E6,E5,E4,E3,E2,E1 };
static const int queenMoves_E8_2[4] = { 3,F7,G6,H5 };
static const int queenMoves_E8_3[5] = { 4,D8,C8,B8,A8 };
static const int queenMoves_E8_4[4] = { 3,F8,G8,H8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_E8[5] = {
  Direction(MD_DOWNDIAG1,queenMoves_E8_0)
 ,Direction(MD_DOWN     ,queenMoves_E8_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_E8_2)
 ,Direction(MD_LEFT     ,queenMoves_E8_3)
 ,Direction(MD_RIGHT    ,queenMoves_E8_4)
};
#else
static const Direction queenMoveDirections_E8[5] = {
  Direction(MD_DOWNDIAG1,queenMoves_E8_0,_T("queen MD_DOWNDIAG1 from E8:D7 C6 B5 A4"))
 ,Direction(MD_DOWN     ,queenMoves_E8_1,_T("queen MD_DOWN from E8:E7 E6 E5 E4 E3 E2 E1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_E8_2,_T("queen MD_DOWNDIAG2 from E8:F7 G6 H5"))
 ,Direction(MD_LEFT     ,queenMoves_E8_3,_T("queen MD_LEFT from E8:D8 C8 B8 A8"))
 ,Direction(MD_RIGHT    ,queenMoves_E8_4,_T("queen MD_RIGHT from E8:F8 G8 H8"))
};
#endif
static const DirectionArray queenMoves_E8(5,queenMoveDirections_E8);

static const int queenMoves_F8_0[6] = { 5,E7,D6,C5,B4,A3 };
static const int queenMoves_F8_1[8] = { 7,F7,F6,F5,F4,F3,F2,F1 };
static const int queenMoves_F8_2[3] = { 2,G7,H6 };
static const int queenMoves_F8_3[6] = { 5,E8,D8,C8,B8,A8 };
static const int queenMoves_F8_4[3] = { 2,G8,H8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_F8[5] = {
  Direction(MD_DOWNDIAG1,queenMoves_F8_0)
 ,Direction(MD_DOWN     ,queenMoves_F8_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_F8_2)
 ,Direction(MD_LEFT     ,queenMoves_F8_3)
 ,Direction(MD_RIGHT    ,queenMoves_F8_4)
};
#else
static const Direction queenMoveDirections_F8[5] = {
  Direction(MD_DOWNDIAG1,queenMoves_F8_0,_T("queen MD_DOWNDIAG1 from F8:E7 D6 C5 B4 A3"))
 ,Direction(MD_DOWN     ,queenMoves_F8_1,_T("queen MD_DOWN from F8:F7 F6 F5 F4 F3 F2 F1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_F8_2,_T("queen MD_DOWNDIAG2 from F8:G7 H6"))
 ,Direction(MD_LEFT     ,queenMoves_F8_3,_T("queen MD_LEFT from F8:E8 D8 C8 B8 A8"))
 ,Direction(MD_RIGHT    ,queenMoves_F8_4,_T("queen MD_RIGHT from F8:G8 H8"))
};
#endif
static const DirectionArray queenMoves_F8(5,queenMoveDirections_F8);

static const int queenMoves_G8_0[7] = { 6,F7,E6,D5,C4,B3,A2 };
static const int queenMoves_G8_1[8] = { 7,G7,G6,G5,G4,G3,G2,G1 };
static const int queenMoves_G8_2[2] = { 1,H7 };
static const int queenMoves_G8_3[7] = { 6,F8,E8,D8,C8,B8,A8 };
static const int queenMoves_G8_4[2] = { 1,H8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_G8[5] = {
  Direction(MD_DOWNDIAG1,queenMoves_G8_0)
 ,Direction(MD_DOWN     ,queenMoves_G8_1)
 ,Direction(MD_DOWNDIAG2,queenMoves_G8_2)
 ,Direction(MD_LEFT     ,queenMoves_G8_3)
 ,Direction(MD_RIGHT    ,queenMoves_G8_4)
};
#else
static const Direction queenMoveDirections_G8[5] = {
  Direction(MD_DOWNDIAG1,queenMoves_G8_0,_T("queen MD_DOWNDIAG1 from G8:F7 E6 D5 C4 B3 A2"))
 ,Direction(MD_DOWN     ,queenMoves_G8_1,_T("queen MD_DOWN from G8:G7 G6 G5 G4 G3 G2 G1"))
 ,Direction(MD_DOWNDIAG2,queenMoves_G8_2,_T("queen MD_DOWNDIAG2 from G8:H7"))
 ,Direction(MD_LEFT     ,queenMoves_G8_3,_T("queen MD_LEFT from G8:F8 E8 D8 C8 B8 A8"))
 ,Direction(MD_RIGHT    ,queenMoves_G8_4,_T("queen MD_RIGHT from G8:H8"))
};
#endif
static const DirectionArray queenMoves_G8(5,queenMoveDirections_G8);

static const int queenMoves_H8_0[8] = { 7,G7,F6,E5,D4,C3,B2,A1 };
static const int queenMoves_H8_1[8] = { 7,H7,H6,H5,H4,H3,H2,H1 };
static const int queenMoves_H8_2[8] = { 7,G8,F8,E8,D8,C8,B8,A8 };
#ifndef _DEBUG
static const Direction queenMoveDirections_H8[3] = {
  Direction(MD_DOWNDIAG1,queenMoves_H8_0)
 ,Direction(MD_DOWN     ,queenMoves_H8_1)
 ,Direction(MD_LEFT     ,queenMoves_H8_2)
};
#else
static const Direction queenMoveDirections_H8[3] = {
  Direction(MD_DOWNDIAG1,queenMoves_H8_0,_T("queen MD_DOWNDIAG1 from H8:G7 F6 E5 D4 C3 B2 A1"))
 ,Direction(MD_DOWN     ,queenMoves_H8_1,_T("queen MD_DOWN from H8:H7 H6 H5 H4 H3 H2 H1"))
 ,Direction(MD_LEFT     ,queenMoves_H8_2,_T("queen MD_LEFT from H8:G8 F8 E8 D8 C8 B8 A8"))
};
#endif
static const DirectionArray queenMoves_H8(3,queenMoveDirections_H8);

static const int rookMoves_A1_0[8] = { 7,B1,C1,D1,E1,F1,G1,H1 };
static const int rookMoves_A1_1[8] = { 7,A2,A3,A4,A5,A6,A7,A8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_A1[2] = {
  Direction(MD_RIGHT    ,rookMoves_A1_0)
 ,Direction(MD_UP       ,rookMoves_A1_1)
};
#else
static const Direction rookMoveDirections_A1[2] = {
  Direction(MD_RIGHT    ,rookMoves_A1_0,_T("rook MD_RIGHT from A1:B1 C1 D1 E1 F1 G1 H1"))
 ,Direction(MD_UP       ,rookMoves_A1_1,_T("rook MD_UP from A1:A2 A3 A4 A5 A6 A7 A8"))
};
#endif
static const DirectionArray rookMoves_A1(2,rookMoveDirections_A1);

static const int rookMoves_B1_0[2] = { 1,A1 };
static const int rookMoves_B1_1[7] = { 6,C1,D1,E1,F1,G1,H1 };
static const int rookMoves_B1_2[8] = { 7,B2,B3,B4,B5,B6,B7,B8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_B1[3] = {
  Direction(MD_LEFT     ,rookMoves_B1_0)
 ,Direction(MD_RIGHT    ,rookMoves_B1_1)
 ,Direction(MD_UP       ,rookMoves_B1_2)
};
#else
static const Direction rookMoveDirections_B1[3] = {
  Direction(MD_LEFT     ,rookMoves_B1_0,_T("rook MD_LEFT from B1:A1"))
 ,Direction(MD_RIGHT    ,rookMoves_B1_1,_T("rook MD_RIGHT from B1:C1 D1 E1 F1 G1 H1"))
 ,Direction(MD_UP       ,rookMoves_B1_2,_T("rook MD_UP from B1:B2 B3 B4 B5 B6 B7 B8"))
};
#endif
static const DirectionArray rookMoves_B1(3,rookMoveDirections_B1);

static const int rookMoves_C1_0[3] = { 2,B1,A1 };
static const int rookMoves_C1_1[6] = { 5,D1,E1,F1,G1,H1 };
static const int rookMoves_C1_2[8] = { 7,C2,C3,C4,C5,C6,C7,C8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_C1[3] = {
  Direction(MD_LEFT     ,rookMoves_C1_0)
 ,Direction(MD_RIGHT    ,rookMoves_C1_1)
 ,Direction(MD_UP       ,rookMoves_C1_2)
};
#else
static const Direction rookMoveDirections_C1[3] = {
  Direction(MD_LEFT     ,rookMoves_C1_0,_T("rook MD_LEFT from C1:B1 A1"))
 ,Direction(MD_RIGHT    ,rookMoves_C1_1,_T("rook MD_RIGHT from C1:D1 E1 F1 G1 H1"))
 ,Direction(MD_UP       ,rookMoves_C1_2,_T("rook MD_UP from C1:C2 C3 C4 C5 C6 C7 C8"))
};
#endif
static const DirectionArray rookMoves_C1(3,rookMoveDirections_C1);

static const int rookMoves_D1_0[4] = { 3,C1,B1,A1 };
static const int rookMoves_D1_1[5] = { 4,E1,F1,G1,H1 };
static const int rookMoves_D1_2[8] = { 7,D2,D3,D4,D5,D6,D7,D8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_D1[3] = {
  Direction(MD_LEFT     ,rookMoves_D1_0)
 ,Direction(MD_RIGHT    ,rookMoves_D1_1)
 ,Direction(MD_UP       ,rookMoves_D1_2)
};
#else
static const Direction rookMoveDirections_D1[3] = {
  Direction(MD_LEFT     ,rookMoves_D1_0,_T("rook MD_LEFT from D1:C1 B1 A1"))
 ,Direction(MD_RIGHT    ,rookMoves_D1_1,_T("rook MD_RIGHT from D1:E1 F1 G1 H1"))
 ,Direction(MD_UP       ,rookMoves_D1_2,_T("rook MD_UP from D1:D2 D3 D4 D5 D6 D7 D8"))
};
#endif
static const DirectionArray rookMoves_D1(3,rookMoveDirections_D1);

static const int rookMoves_E1_0[5] = { 4,D1,C1,B1,A1 };
static const int rookMoves_E1_1[4] = { 3,F1,G1,H1 };
static const int rookMoves_E1_2[8] = { 7,E2,E3,E4,E5,E6,E7,E8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_E1[3] = {
  Direction(MD_LEFT     ,rookMoves_E1_0)
 ,Direction(MD_RIGHT    ,rookMoves_E1_1)
 ,Direction(MD_UP       ,rookMoves_E1_2)
};
#else
static const Direction rookMoveDirections_E1[3] = {
  Direction(MD_LEFT     ,rookMoves_E1_0,_T("rook MD_LEFT from E1:D1 C1 B1 A1"))
 ,Direction(MD_RIGHT    ,rookMoves_E1_1,_T("rook MD_RIGHT from E1:F1 G1 H1"))
 ,Direction(MD_UP       ,rookMoves_E1_2,_T("rook MD_UP from E1:E2 E3 E4 E5 E6 E7 E8"))
};
#endif
static const DirectionArray rookMoves_E1(3,rookMoveDirections_E1);

static const int rookMoves_F1_0[6] = { 5,E1,D1,C1,B1,A1 };
static const int rookMoves_F1_1[3] = { 2,G1,H1 };
static const int rookMoves_F1_2[8] = { 7,F2,F3,F4,F5,F6,F7,F8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_F1[3] = {
  Direction(MD_LEFT     ,rookMoves_F1_0)
 ,Direction(MD_RIGHT    ,rookMoves_F1_1)
 ,Direction(MD_UP       ,rookMoves_F1_2)
};
#else
static const Direction rookMoveDirections_F1[3] = {
  Direction(MD_LEFT     ,rookMoves_F1_0,_T("rook MD_LEFT from F1:E1 D1 C1 B1 A1"))
 ,Direction(MD_RIGHT    ,rookMoves_F1_1,_T("rook MD_RIGHT from F1:G1 H1"))
 ,Direction(MD_UP       ,rookMoves_F1_2,_T("rook MD_UP from F1:F2 F3 F4 F5 F6 F7 F8"))
};
#endif
static const DirectionArray rookMoves_F1(3,rookMoveDirections_F1);

static const int rookMoves_G1_0[7] = { 6,F1,E1,D1,C1,B1,A1 };
static const int rookMoves_G1_1[2] = { 1,H1 };
static const int rookMoves_G1_2[8] = { 7,G2,G3,G4,G5,G6,G7,G8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_G1[3] = {
  Direction(MD_LEFT     ,rookMoves_G1_0)
 ,Direction(MD_RIGHT    ,rookMoves_G1_1)
 ,Direction(MD_UP       ,rookMoves_G1_2)
};
#else
static const Direction rookMoveDirections_G1[3] = {
  Direction(MD_LEFT     ,rookMoves_G1_0,_T("rook MD_LEFT from G1:F1 E1 D1 C1 B1 A1"))
 ,Direction(MD_RIGHT    ,rookMoves_G1_1,_T("rook MD_RIGHT from G1:H1"))
 ,Direction(MD_UP       ,rookMoves_G1_2,_T("rook MD_UP from G1:G2 G3 G4 G5 G6 G7 G8"))
};
#endif
static const DirectionArray rookMoves_G1(3,rookMoveDirections_G1);

static const int rookMoves_H1_0[8] = { 7,G1,F1,E1,D1,C1,B1,A1 };
static const int rookMoves_H1_1[8] = { 7,H2,H3,H4,H5,H6,H7,H8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_H1[2] = {
  Direction(MD_LEFT     ,rookMoves_H1_0)
 ,Direction(MD_UP       ,rookMoves_H1_1)
};
#else
static const Direction rookMoveDirections_H1[2] = {
  Direction(MD_LEFT     ,rookMoves_H1_0,_T("rook MD_LEFT from H1:G1 F1 E1 D1 C1 B1 A1"))
 ,Direction(MD_UP       ,rookMoves_H1_1,_T("rook MD_UP from H1:H2 H3 H4 H5 H6 H7 H8"))
};
#endif
static const DirectionArray rookMoves_H1(2,rookMoveDirections_H1);

static const int rookMoves_A2_0[2] = { 1,A1 };
static const int rookMoves_A2_1[8] = { 7,B2,C2,D2,E2,F2,G2,H2 };
static const int rookMoves_A2_2[7] = { 6,A3,A4,A5,A6,A7,A8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_A2[3] = {
  Direction(MD_DOWN     ,rookMoves_A2_0)
 ,Direction(MD_RIGHT    ,rookMoves_A2_1)
 ,Direction(MD_UP       ,rookMoves_A2_2)
};
#else
static const Direction rookMoveDirections_A2[3] = {
  Direction(MD_DOWN     ,rookMoves_A2_0,_T("rook MD_DOWN from A2:A1"))
 ,Direction(MD_RIGHT    ,rookMoves_A2_1,_T("rook MD_RIGHT from A2:B2 C2 D2 E2 F2 G2 H2"))
 ,Direction(MD_UP       ,rookMoves_A2_2,_T("rook MD_UP from A2:A3 A4 A5 A6 A7 A8"))
};
#endif
static const DirectionArray rookMoves_A2(3,rookMoveDirections_A2);

static const int rookMoves_B2_0[2] = { 1,B1 };
static const int rookMoves_B2_1[2] = { 1,A2 };
static const int rookMoves_B2_2[7] = { 6,C2,D2,E2,F2,G2,H2 };
static const int rookMoves_B2_3[7] = { 6,B3,B4,B5,B6,B7,B8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_B2[4] = {
  Direction(MD_DOWN     ,rookMoves_B2_0)
 ,Direction(MD_LEFT     ,rookMoves_B2_1)
 ,Direction(MD_RIGHT    ,rookMoves_B2_2)
 ,Direction(MD_UP       ,rookMoves_B2_3)
};
#else
static const Direction rookMoveDirections_B2[4] = {
  Direction(MD_DOWN     ,rookMoves_B2_0,_T("rook MD_DOWN from B2:B1"))
 ,Direction(MD_LEFT     ,rookMoves_B2_1,_T("rook MD_LEFT from B2:A2"))
 ,Direction(MD_RIGHT    ,rookMoves_B2_2,_T("rook MD_RIGHT from B2:C2 D2 E2 F2 G2 H2"))
 ,Direction(MD_UP       ,rookMoves_B2_3,_T("rook MD_UP from B2:B3 B4 B5 B6 B7 B8"))
};
#endif
static const DirectionArray rookMoves_B2(4,rookMoveDirections_B2);

static const int rookMoves_C2_0[2] = { 1,C1 };
static const int rookMoves_C2_1[3] = { 2,B2,A2 };
static const int rookMoves_C2_2[6] = { 5,D2,E2,F2,G2,H2 };
static const int rookMoves_C2_3[7] = { 6,C3,C4,C5,C6,C7,C8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_C2[4] = {
  Direction(MD_DOWN     ,rookMoves_C2_0)
 ,Direction(MD_LEFT     ,rookMoves_C2_1)
 ,Direction(MD_RIGHT    ,rookMoves_C2_2)
 ,Direction(MD_UP       ,rookMoves_C2_3)
};
#else
static const Direction rookMoveDirections_C2[4] = {
  Direction(MD_DOWN     ,rookMoves_C2_0,_T("rook MD_DOWN from C2:C1"))
 ,Direction(MD_LEFT     ,rookMoves_C2_1,_T("rook MD_LEFT from C2:B2 A2"))
 ,Direction(MD_RIGHT    ,rookMoves_C2_2,_T("rook MD_RIGHT from C2:D2 E2 F2 G2 H2"))
 ,Direction(MD_UP       ,rookMoves_C2_3,_T("rook MD_UP from C2:C3 C4 C5 C6 C7 C8"))
};
#endif
static const DirectionArray rookMoves_C2(4,rookMoveDirections_C2);

static const int rookMoves_D2_0[2] = { 1,D1 };
static const int rookMoves_D2_1[4] = { 3,C2,B2,A2 };
static const int rookMoves_D2_2[5] = { 4,E2,F2,G2,H2 };
static const int rookMoves_D2_3[7] = { 6,D3,D4,D5,D6,D7,D8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_D2[4] = {
  Direction(MD_DOWN     ,rookMoves_D2_0)
 ,Direction(MD_LEFT     ,rookMoves_D2_1)
 ,Direction(MD_RIGHT    ,rookMoves_D2_2)
 ,Direction(MD_UP       ,rookMoves_D2_3)
};
#else
static const Direction rookMoveDirections_D2[4] = {
  Direction(MD_DOWN     ,rookMoves_D2_0,_T("rook MD_DOWN from D2:D1"))
 ,Direction(MD_LEFT     ,rookMoves_D2_1,_T("rook MD_LEFT from D2:C2 B2 A2"))
 ,Direction(MD_RIGHT    ,rookMoves_D2_2,_T("rook MD_RIGHT from D2:E2 F2 G2 H2"))
 ,Direction(MD_UP       ,rookMoves_D2_3,_T("rook MD_UP from D2:D3 D4 D5 D6 D7 D8"))
};
#endif
static const DirectionArray rookMoves_D2(4,rookMoveDirections_D2);

static const int rookMoves_E2_0[2] = { 1,E1 };
static const int rookMoves_E2_1[5] = { 4,D2,C2,B2,A2 };
static const int rookMoves_E2_2[4] = { 3,F2,G2,H2 };
static const int rookMoves_E2_3[7] = { 6,E3,E4,E5,E6,E7,E8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_E2[4] = {
  Direction(MD_DOWN     ,rookMoves_E2_0)
 ,Direction(MD_LEFT     ,rookMoves_E2_1)
 ,Direction(MD_RIGHT    ,rookMoves_E2_2)
 ,Direction(MD_UP       ,rookMoves_E2_3)
};
#else
static const Direction rookMoveDirections_E2[4] = {
  Direction(MD_DOWN     ,rookMoves_E2_0,_T("rook MD_DOWN from E2:E1"))
 ,Direction(MD_LEFT     ,rookMoves_E2_1,_T("rook MD_LEFT from E2:D2 C2 B2 A2"))
 ,Direction(MD_RIGHT    ,rookMoves_E2_2,_T("rook MD_RIGHT from E2:F2 G2 H2"))
 ,Direction(MD_UP       ,rookMoves_E2_3,_T("rook MD_UP from E2:E3 E4 E5 E6 E7 E8"))
};
#endif
static const DirectionArray rookMoves_E2(4,rookMoveDirections_E2);

static const int rookMoves_F2_0[2] = { 1,F1 };
static const int rookMoves_F2_1[6] = { 5,E2,D2,C2,B2,A2 };
static const int rookMoves_F2_2[3] = { 2,G2,H2 };
static const int rookMoves_F2_3[7] = { 6,F3,F4,F5,F6,F7,F8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_F2[4] = {
  Direction(MD_DOWN     ,rookMoves_F2_0)
 ,Direction(MD_LEFT     ,rookMoves_F2_1)
 ,Direction(MD_RIGHT    ,rookMoves_F2_2)
 ,Direction(MD_UP       ,rookMoves_F2_3)
};
#else
static const Direction rookMoveDirections_F2[4] = {
  Direction(MD_DOWN     ,rookMoves_F2_0,_T("rook MD_DOWN from F2:F1"))
 ,Direction(MD_LEFT     ,rookMoves_F2_1,_T("rook MD_LEFT from F2:E2 D2 C2 B2 A2"))
 ,Direction(MD_RIGHT    ,rookMoves_F2_2,_T("rook MD_RIGHT from F2:G2 H2"))
 ,Direction(MD_UP       ,rookMoves_F2_3,_T("rook MD_UP from F2:F3 F4 F5 F6 F7 F8"))
};
#endif
static const DirectionArray rookMoves_F2(4,rookMoveDirections_F2);

static const int rookMoves_G2_0[2] = { 1,G1 };
static const int rookMoves_G2_1[7] = { 6,F2,E2,D2,C2,B2,A2 };
static const int rookMoves_G2_2[2] = { 1,H2 };
static const int rookMoves_G2_3[7] = { 6,G3,G4,G5,G6,G7,G8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_G2[4] = {
  Direction(MD_DOWN     ,rookMoves_G2_0)
 ,Direction(MD_LEFT     ,rookMoves_G2_1)
 ,Direction(MD_RIGHT    ,rookMoves_G2_2)
 ,Direction(MD_UP       ,rookMoves_G2_3)
};
#else
static const Direction rookMoveDirections_G2[4] = {
  Direction(MD_DOWN     ,rookMoves_G2_0,_T("rook MD_DOWN from G2:G1"))
 ,Direction(MD_LEFT     ,rookMoves_G2_1,_T("rook MD_LEFT from G2:F2 E2 D2 C2 B2 A2"))
 ,Direction(MD_RIGHT    ,rookMoves_G2_2,_T("rook MD_RIGHT from G2:H2"))
 ,Direction(MD_UP       ,rookMoves_G2_3,_T("rook MD_UP from G2:G3 G4 G5 G6 G7 G8"))
};
#endif
static const DirectionArray rookMoves_G2(4,rookMoveDirections_G2);

static const int rookMoves_H2_0[2] = { 1,H1 };
static const int rookMoves_H2_1[8] = { 7,G2,F2,E2,D2,C2,B2,A2 };
static const int rookMoves_H2_2[7] = { 6,H3,H4,H5,H6,H7,H8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_H2[3] = {
  Direction(MD_DOWN     ,rookMoves_H2_0)
 ,Direction(MD_LEFT     ,rookMoves_H2_1)
 ,Direction(MD_UP       ,rookMoves_H2_2)
};
#else
static const Direction rookMoveDirections_H2[3] = {
  Direction(MD_DOWN     ,rookMoves_H2_0,_T("rook MD_DOWN from H2:H1"))
 ,Direction(MD_LEFT     ,rookMoves_H2_1,_T("rook MD_LEFT from H2:G2 F2 E2 D2 C2 B2 A2"))
 ,Direction(MD_UP       ,rookMoves_H2_2,_T("rook MD_UP from H2:H3 H4 H5 H6 H7 H8"))
};
#endif
static const DirectionArray rookMoves_H2(3,rookMoveDirections_H2);

static const int rookMoves_A3_0[3] = { 2,A2,A1 };
static const int rookMoves_A3_1[8] = { 7,B3,C3,D3,E3,F3,G3,H3 };
static const int rookMoves_A3_2[6] = { 5,A4,A5,A6,A7,A8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_A3[3] = {
  Direction(MD_DOWN     ,rookMoves_A3_0)
 ,Direction(MD_RIGHT    ,rookMoves_A3_1)
 ,Direction(MD_UP       ,rookMoves_A3_2)
};
#else
static const Direction rookMoveDirections_A3[3] = {
  Direction(MD_DOWN     ,rookMoves_A3_0,_T("rook MD_DOWN from A3:A2 A1"))
 ,Direction(MD_RIGHT    ,rookMoves_A3_1,_T("rook MD_RIGHT from A3:B3 C3 D3 E3 F3 G3 H3"))
 ,Direction(MD_UP       ,rookMoves_A3_2,_T("rook MD_UP from A3:A4 A5 A6 A7 A8"))
};
#endif
static const DirectionArray rookMoves_A3(3,rookMoveDirections_A3);

static const int rookMoves_B3_0[3] = { 2,B2,B1 };
static const int rookMoves_B3_1[2] = { 1,A3 };
static const int rookMoves_B3_2[7] = { 6,C3,D3,E3,F3,G3,H3 };
static const int rookMoves_B3_3[6] = { 5,B4,B5,B6,B7,B8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_B3[4] = {
  Direction(MD_DOWN     ,rookMoves_B3_0)
 ,Direction(MD_LEFT     ,rookMoves_B3_1)
 ,Direction(MD_RIGHT    ,rookMoves_B3_2)
 ,Direction(MD_UP       ,rookMoves_B3_3)
};
#else
static const Direction rookMoveDirections_B3[4] = {
  Direction(MD_DOWN     ,rookMoves_B3_0,_T("rook MD_DOWN from B3:B2 B1"))
 ,Direction(MD_LEFT     ,rookMoves_B3_1,_T("rook MD_LEFT from B3:A3"))
 ,Direction(MD_RIGHT    ,rookMoves_B3_2,_T("rook MD_RIGHT from B3:C3 D3 E3 F3 G3 H3"))
 ,Direction(MD_UP       ,rookMoves_B3_3,_T("rook MD_UP from B3:B4 B5 B6 B7 B8"))
};
#endif
static const DirectionArray rookMoves_B3(4,rookMoveDirections_B3);

static const int rookMoves_C3_0[3] = { 2,C2,C1 };
static const int rookMoves_C3_1[3] = { 2,B3,A3 };
static const int rookMoves_C3_2[6] = { 5,D3,E3,F3,G3,H3 };
static const int rookMoves_C3_3[6] = { 5,C4,C5,C6,C7,C8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_C3[4] = {
  Direction(MD_DOWN     ,rookMoves_C3_0)
 ,Direction(MD_LEFT     ,rookMoves_C3_1)
 ,Direction(MD_RIGHT    ,rookMoves_C3_2)
 ,Direction(MD_UP       ,rookMoves_C3_3)
};
#else
static const Direction rookMoveDirections_C3[4] = {
  Direction(MD_DOWN     ,rookMoves_C3_0,_T("rook MD_DOWN from C3:C2 C1"))
 ,Direction(MD_LEFT     ,rookMoves_C3_1,_T("rook MD_LEFT from C3:B3 A3"))
 ,Direction(MD_RIGHT    ,rookMoves_C3_2,_T("rook MD_RIGHT from C3:D3 E3 F3 G3 H3"))
 ,Direction(MD_UP       ,rookMoves_C3_3,_T("rook MD_UP from C3:C4 C5 C6 C7 C8"))
};
#endif
static const DirectionArray rookMoves_C3(4,rookMoveDirections_C3);

static const int rookMoves_D3_0[3] = { 2,D2,D1 };
static const int rookMoves_D3_1[4] = { 3,C3,B3,A3 };
static const int rookMoves_D3_2[5] = { 4,E3,F3,G3,H3 };
static const int rookMoves_D3_3[6] = { 5,D4,D5,D6,D7,D8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_D3[4] = {
  Direction(MD_DOWN     ,rookMoves_D3_0)
 ,Direction(MD_LEFT     ,rookMoves_D3_1)
 ,Direction(MD_RIGHT    ,rookMoves_D3_2)
 ,Direction(MD_UP       ,rookMoves_D3_3)
};
#else
static const Direction rookMoveDirections_D3[4] = {
  Direction(MD_DOWN     ,rookMoves_D3_0,_T("rook MD_DOWN from D3:D2 D1"))
 ,Direction(MD_LEFT     ,rookMoves_D3_1,_T("rook MD_LEFT from D3:C3 B3 A3"))
 ,Direction(MD_RIGHT    ,rookMoves_D3_2,_T("rook MD_RIGHT from D3:E3 F3 G3 H3"))
 ,Direction(MD_UP       ,rookMoves_D3_3,_T("rook MD_UP from D3:D4 D5 D6 D7 D8"))
};
#endif
static const DirectionArray rookMoves_D3(4,rookMoveDirections_D3);

static const int rookMoves_E3_0[3] = { 2,E2,E1 };
static const int rookMoves_E3_1[5] = { 4,D3,C3,B3,A3 };
static const int rookMoves_E3_2[4] = { 3,F3,G3,H3 };
static const int rookMoves_E3_3[6] = { 5,E4,E5,E6,E7,E8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_E3[4] = {
  Direction(MD_DOWN     ,rookMoves_E3_0)
 ,Direction(MD_LEFT     ,rookMoves_E3_1)
 ,Direction(MD_RIGHT    ,rookMoves_E3_2)
 ,Direction(MD_UP       ,rookMoves_E3_3)
};
#else
static const Direction rookMoveDirections_E3[4] = {
  Direction(MD_DOWN     ,rookMoves_E3_0,_T("rook MD_DOWN from E3:E2 E1"))
 ,Direction(MD_LEFT     ,rookMoves_E3_1,_T("rook MD_LEFT from E3:D3 C3 B3 A3"))
 ,Direction(MD_RIGHT    ,rookMoves_E3_2,_T("rook MD_RIGHT from E3:F3 G3 H3"))
 ,Direction(MD_UP       ,rookMoves_E3_3,_T("rook MD_UP from E3:E4 E5 E6 E7 E8"))
};
#endif
static const DirectionArray rookMoves_E3(4,rookMoveDirections_E3);

static const int rookMoves_F3_0[3] = { 2,F2,F1 };
static const int rookMoves_F3_1[6] = { 5,E3,D3,C3,B3,A3 };
static const int rookMoves_F3_2[3] = { 2,G3,H3 };
static const int rookMoves_F3_3[6] = { 5,F4,F5,F6,F7,F8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_F3[4] = {
  Direction(MD_DOWN     ,rookMoves_F3_0)
 ,Direction(MD_LEFT     ,rookMoves_F3_1)
 ,Direction(MD_RIGHT    ,rookMoves_F3_2)
 ,Direction(MD_UP       ,rookMoves_F3_3)
};
#else
static const Direction rookMoveDirections_F3[4] = {
  Direction(MD_DOWN     ,rookMoves_F3_0,_T("rook MD_DOWN from F3:F2 F1"))
 ,Direction(MD_LEFT     ,rookMoves_F3_1,_T("rook MD_LEFT from F3:E3 D3 C3 B3 A3"))
 ,Direction(MD_RIGHT    ,rookMoves_F3_2,_T("rook MD_RIGHT from F3:G3 H3"))
 ,Direction(MD_UP       ,rookMoves_F3_3,_T("rook MD_UP from F3:F4 F5 F6 F7 F8"))
};
#endif
static const DirectionArray rookMoves_F3(4,rookMoveDirections_F3);

static const int rookMoves_G3_0[3] = { 2,G2,G1 };
static const int rookMoves_G3_1[7] = { 6,F3,E3,D3,C3,B3,A3 };
static const int rookMoves_G3_2[2] = { 1,H3 };
static const int rookMoves_G3_3[6] = { 5,G4,G5,G6,G7,G8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_G3[4] = {
  Direction(MD_DOWN     ,rookMoves_G3_0)
 ,Direction(MD_LEFT     ,rookMoves_G3_1)
 ,Direction(MD_RIGHT    ,rookMoves_G3_2)
 ,Direction(MD_UP       ,rookMoves_G3_3)
};
#else
static const Direction rookMoveDirections_G3[4] = {
  Direction(MD_DOWN     ,rookMoves_G3_0,_T("rook MD_DOWN from G3:G2 G1"))
 ,Direction(MD_LEFT     ,rookMoves_G3_1,_T("rook MD_LEFT from G3:F3 E3 D3 C3 B3 A3"))
 ,Direction(MD_RIGHT    ,rookMoves_G3_2,_T("rook MD_RIGHT from G3:H3"))
 ,Direction(MD_UP       ,rookMoves_G3_3,_T("rook MD_UP from G3:G4 G5 G6 G7 G8"))
};
#endif
static const DirectionArray rookMoves_G3(4,rookMoveDirections_G3);

static const int rookMoves_H3_0[3] = { 2,H2,H1 };
static const int rookMoves_H3_1[8] = { 7,G3,F3,E3,D3,C3,B3,A3 };
static const int rookMoves_H3_2[6] = { 5,H4,H5,H6,H7,H8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_H3[3] = {
  Direction(MD_DOWN     ,rookMoves_H3_0)
 ,Direction(MD_LEFT     ,rookMoves_H3_1)
 ,Direction(MD_UP       ,rookMoves_H3_2)
};
#else
static const Direction rookMoveDirections_H3[3] = {
  Direction(MD_DOWN     ,rookMoves_H3_0,_T("rook MD_DOWN from H3:H2 H1"))
 ,Direction(MD_LEFT     ,rookMoves_H3_1,_T("rook MD_LEFT from H3:G3 F3 E3 D3 C3 B3 A3"))
 ,Direction(MD_UP       ,rookMoves_H3_2,_T("rook MD_UP from H3:H4 H5 H6 H7 H8"))
};
#endif
static const DirectionArray rookMoves_H3(3,rookMoveDirections_H3);

static const int rookMoves_A4_0[4] = { 3,A3,A2,A1 };
static const int rookMoves_A4_1[8] = { 7,B4,C4,D4,E4,F4,G4,H4 };
static const int rookMoves_A4_2[5] = { 4,A5,A6,A7,A8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_A4[3] = {
  Direction(MD_DOWN     ,rookMoves_A4_0)
 ,Direction(MD_RIGHT    ,rookMoves_A4_1)
 ,Direction(MD_UP       ,rookMoves_A4_2)
};
#else
static const Direction rookMoveDirections_A4[3] = {
  Direction(MD_DOWN     ,rookMoves_A4_0,_T("rook MD_DOWN from A4:A3 A2 A1"))
 ,Direction(MD_RIGHT    ,rookMoves_A4_1,_T("rook MD_RIGHT from A4:B4 C4 D4 E4 F4 G4 H4"))
 ,Direction(MD_UP       ,rookMoves_A4_2,_T("rook MD_UP from A4:A5 A6 A7 A8"))
};
#endif
static const DirectionArray rookMoves_A4(3,rookMoveDirections_A4);

static const int rookMoves_B4_0[4] = { 3,B3,B2,B1 };
static const int rookMoves_B4_1[2] = { 1,A4 };
static const int rookMoves_B4_2[7] = { 6,C4,D4,E4,F4,G4,H4 };
static const int rookMoves_B4_3[5] = { 4,B5,B6,B7,B8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_B4[4] = {
  Direction(MD_DOWN     ,rookMoves_B4_0)
 ,Direction(MD_LEFT     ,rookMoves_B4_1)
 ,Direction(MD_RIGHT    ,rookMoves_B4_2)
 ,Direction(MD_UP       ,rookMoves_B4_3)
};
#else
static const Direction rookMoveDirections_B4[4] = {
  Direction(MD_DOWN     ,rookMoves_B4_0,_T("rook MD_DOWN from B4:B3 B2 B1"))
 ,Direction(MD_LEFT     ,rookMoves_B4_1,_T("rook MD_LEFT from B4:A4"))
 ,Direction(MD_RIGHT    ,rookMoves_B4_2,_T("rook MD_RIGHT from B4:C4 D4 E4 F4 G4 H4"))
 ,Direction(MD_UP       ,rookMoves_B4_3,_T("rook MD_UP from B4:B5 B6 B7 B8"))
};
#endif
static const DirectionArray rookMoves_B4(4,rookMoveDirections_B4);

static const int rookMoves_C4_0[4] = { 3,C3,C2,C1 };
static const int rookMoves_C4_1[3] = { 2,B4,A4 };
static const int rookMoves_C4_2[6] = { 5,D4,E4,F4,G4,H4 };
static const int rookMoves_C4_3[5] = { 4,C5,C6,C7,C8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_C4[4] = {
  Direction(MD_DOWN     ,rookMoves_C4_0)
 ,Direction(MD_LEFT     ,rookMoves_C4_1)
 ,Direction(MD_RIGHT    ,rookMoves_C4_2)
 ,Direction(MD_UP       ,rookMoves_C4_3)
};
#else
static const Direction rookMoveDirections_C4[4] = {
  Direction(MD_DOWN     ,rookMoves_C4_0,_T("rook MD_DOWN from C4:C3 C2 C1"))
 ,Direction(MD_LEFT     ,rookMoves_C4_1,_T("rook MD_LEFT from C4:B4 A4"))
 ,Direction(MD_RIGHT    ,rookMoves_C4_2,_T("rook MD_RIGHT from C4:D4 E4 F4 G4 H4"))
 ,Direction(MD_UP       ,rookMoves_C4_3,_T("rook MD_UP from C4:C5 C6 C7 C8"))
};
#endif
static const DirectionArray rookMoves_C4(4,rookMoveDirections_C4);

static const int rookMoves_D4_0[4] = { 3,D3,D2,D1 };
static const int rookMoves_D4_1[4] = { 3,C4,B4,A4 };
static const int rookMoves_D4_2[5] = { 4,E4,F4,G4,H4 };
static const int rookMoves_D4_3[5] = { 4,D5,D6,D7,D8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_D4[4] = {
  Direction(MD_DOWN     ,rookMoves_D4_0)
 ,Direction(MD_LEFT     ,rookMoves_D4_1)
 ,Direction(MD_RIGHT    ,rookMoves_D4_2)
 ,Direction(MD_UP       ,rookMoves_D4_3)
};
#else
static const Direction rookMoveDirections_D4[4] = {
  Direction(MD_DOWN     ,rookMoves_D4_0,_T("rook MD_DOWN from D4:D3 D2 D1"))
 ,Direction(MD_LEFT     ,rookMoves_D4_1,_T("rook MD_LEFT from D4:C4 B4 A4"))
 ,Direction(MD_RIGHT    ,rookMoves_D4_2,_T("rook MD_RIGHT from D4:E4 F4 G4 H4"))
 ,Direction(MD_UP       ,rookMoves_D4_3,_T("rook MD_UP from D4:D5 D6 D7 D8"))
};
#endif
static const DirectionArray rookMoves_D4(4,rookMoveDirections_D4);

static const int rookMoves_E4_0[4] = { 3,E3,E2,E1 };
static const int rookMoves_E4_1[5] = { 4,D4,C4,B4,A4 };
static const int rookMoves_E4_2[4] = { 3,F4,G4,H4 };
static const int rookMoves_E4_3[5] = { 4,E5,E6,E7,E8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_E4[4] = {
  Direction(MD_DOWN     ,rookMoves_E4_0)
 ,Direction(MD_LEFT     ,rookMoves_E4_1)
 ,Direction(MD_RIGHT    ,rookMoves_E4_2)
 ,Direction(MD_UP       ,rookMoves_E4_3)
};
#else
static const Direction rookMoveDirections_E4[4] = {
  Direction(MD_DOWN     ,rookMoves_E4_0,_T("rook MD_DOWN from E4:E3 E2 E1"))
 ,Direction(MD_LEFT     ,rookMoves_E4_1,_T("rook MD_LEFT from E4:D4 C4 B4 A4"))
 ,Direction(MD_RIGHT    ,rookMoves_E4_2,_T("rook MD_RIGHT from E4:F4 G4 H4"))
 ,Direction(MD_UP       ,rookMoves_E4_3,_T("rook MD_UP from E4:E5 E6 E7 E8"))
};
#endif
static const DirectionArray rookMoves_E4(4,rookMoveDirections_E4);

static const int rookMoves_F4_0[4] = { 3,F3,F2,F1 };
static const int rookMoves_F4_1[6] = { 5,E4,D4,C4,B4,A4 };
static const int rookMoves_F4_2[3] = { 2,G4,H4 };
static const int rookMoves_F4_3[5] = { 4,F5,F6,F7,F8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_F4[4] = {
  Direction(MD_DOWN     ,rookMoves_F4_0)
 ,Direction(MD_LEFT     ,rookMoves_F4_1)
 ,Direction(MD_RIGHT    ,rookMoves_F4_2)
 ,Direction(MD_UP       ,rookMoves_F4_3)
};
#else
static const Direction rookMoveDirections_F4[4] = {
  Direction(MD_DOWN     ,rookMoves_F4_0,_T("rook MD_DOWN from F4:F3 F2 F1"))
 ,Direction(MD_LEFT     ,rookMoves_F4_1,_T("rook MD_LEFT from F4:E4 D4 C4 B4 A4"))
 ,Direction(MD_RIGHT    ,rookMoves_F4_2,_T("rook MD_RIGHT from F4:G4 H4"))
 ,Direction(MD_UP       ,rookMoves_F4_3,_T("rook MD_UP from F4:F5 F6 F7 F8"))
};
#endif
static const DirectionArray rookMoves_F4(4,rookMoveDirections_F4);

static const int rookMoves_G4_0[4] = { 3,G3,G2,G1 };
static const int rookMoves_G4_1[7] = { 6,F4,E4,D4,C4,B4,A4 };
static const int rookMoves_G4_2[2] = { 1,H4 };
static const int rookMoves_G4_3[5] = { 4,G5,G6,G7,G8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_G4[4] = {
  Direction(MD_DOWN     ,rookMoves_G4_0)
 ,Direction(MD_LEFT     ,rookMoves_G4_1)
 ,Direction(MD_RIGHT    ,rookMoves_G4_2)
 ,Direction(MD_UP       ,rookMoves_G4_3)
};
#else
static const Direction rookMoveDirections_G4[4] = {
  Direction(MD_DOWN     ,rookMoves_G4_0,_T("rook MD_DOWN from G4:G3 G2 G1"))
 ,Direction(MD_LEFT     ,rookMoves_G4_1,_T("rook MD_LEFT from G4:F4 E4 D4 C4 B4 A4"))
 ,Direction(MD_RIGHT    ,rookMoves_G4_2,_T("rook MD_RIGHT from G4:H4"))
 ,Direction(MD_UP       ,rookMoves_G4_3,_T("rook MD_UP from G4:G5 G6 G7 G8"))
};
#endif
static const DirectionArray rookMoves_G4(4,rookMoveDirections_G4);

static const int rookMoves_H4_0[4] = { 3,H3,H2,H1 };
static const int rookMoves_H4_1[8] = { 7,G4,F4,E4,D4,C4,B4,A4 };
static const int rookMoves_H4_2[5] = { 4,H5,H6,H7,H8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_H4[3] = {
  Direction(MD_DOWN     ,rookMoves_H4_0)
 ,Direction(MD_LEFT     ,rookMoves_H4_1)
 ,Direction(MD_UP       ,rookMoves_H4_2)
};
#else
static const Direction rookMoveDirections_H4[3] = {
  Direction(MD_DOWN     ,rookMoves_H4_0,_T("rook MD_DOWN from H4:H3 H2 H1"))
 ,Direction(MD_LEFT     ,rookMoves_H4_1,_T("rook MD_LEFT from H4:G4 F4 E4 D4 C4 B4 A4"))
 ,Direction(MD_UP       ,rookMoves_H4_2,_T("rook MD_UP from H4:H5 H6 H7 H8"))
};
#endif
static const DirectionArray rookMoves_H4(3,rookMoveDirections_H4);

static const int rookMoves_A5_0[5] = { 4,A4,A3,A2,A1 };
static const int rookMoves_A5_1[8] = { 7,B5,C5,D5,E5,F5,G5,H5 };
static const int rookMoves_A5_2[4] = { 3,A6,A7,A8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_A5[3] = {
  Direction(MD_DOWN     ,rookMoves_A5_0)
 ,Direction(MD_RIGHT    ,rookMoves_A5_1)
 ,Direction(MD_UP       ,rookMoves_A5_2)
};
#else
static const Direction rookMoveDirections_A5[3] = {
  Direction(MD_DOWN     ,rookMoves_A5_0,_T("rook MD_DOWN from A5:A4 A3 A2 A1"))
 ,Direction(MD_RIGHT    ,rookMoves_A5_1,_T("rook MD_RIGHT from A5:B5 C5 D5 E5 F5 G5 H5"))
 ,Direction(MD_UP       ,rookMoves_A5_2,_T("rook MD_UP from A5:A6 A7 A8"))
};
#endif
static const DirectionArray rookMoves_A5(3,rookMoveDirections_A5);

static const int rookMoves_B5_0[5] = { 4,B4,B3,B2,B1 };
static const int rookMoves_B5_1[2] = { 1,A5 };
static const int rookMoves_B5_2[7] = { 6,C5,D5,E5,F5,G5,H5 };
static const int rookMoves_B5_3[4] = { 3,B6,B7,B8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_B5[4] = {
  Direction(MD_DOWN     ,rookMoves_B5_0)
 ,Direction(MD_LEFT     ,rookMoves_B5_1)
 ,Direction(MD_RIGHT    ,rookMoves_B5_2)
 ,Direction(MD_UP       ,rookMoves_B5_3)
};
#else
static const Direction rookMoveDirections_B5[4] = {
  Direction(MD_DOWN     ,rookMoves_B5_0,_T("rook MD_DOWN from B5:B4 B3 B2 B1"))
 ,Direction(MD_LEFT     ,rookMoves_B5_1,_T("rook MD_LEFT from B5:A5"))
 ,Direction(MD_RIGHT    ,rookMoves_B5_2,_T("rook MD_RIGHT from B5:C5 D5 E5 F5 G5 H5"))
 ,Direction(MD_UP       ,rookMoves_B5_3,_T("rook MD_UP from B5:B6 B7 B8"))
};
#endif
static const DirectionArray rookMoves_B5(4,rookMoveDirections_B5);

static const int rookMoves_C5_0[5] = { 4,C4,C3,C2,C1 };
static const int rookMoves_C5_1[3] = { 2,B5,A5 };
static const int rookMoves_C5_2[6] = { 5,D5,E5,F5,G5,H5 };
static const int rookMoves_C5_3[4] = { 3,C6,C7,C8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_C5[4] = {
  Direction(MD_DOWN     ,rookMoves_C5_0)
 ,Direction(MD_LEFT     ,rookMoves_C5_1)
 ,Direction(MD_RIGHT    ,rookMoves_C5_2)
 ,Direction(MD_UP       ,rookMoves_C5_3)
};
#else
static const Direction rookMoveDirections_C5[4] = {
  Direction(MD_DOWN     ,rookMoves_C5_0,_T("rook MD_DOWN from C5:C4 C3 C2 C1"))
 ,Direction(MD_LEFT     ,rookMoves_C5_1,_T("rook MD_LEFT from C5:B5 A5"))
 ,Direction(MD_RIGHT    ,rookMoves_C5_2,_T("rook MD_RIGHT from C5:D5 E5 F5 G5 H5"))
 ,Direction(MD_UP       ,rookMoves_C5_3,_T("rook MD_UP from C5:C6 C7 C8"))
};
#endif
static const DirectionArray rookMoves_C5(4,rookMoveDirections_C5);

static const int rookMoves_D5_0[5] = { 4,D4,D3,D2,D1 };
static const int rookMoves_D5_1[4] = { 3,C5,B5,A5 };
static const int rookMoves_D5_2[5] = { 4,E5,F5,G5,H5 };
static const int rookMoves_D5_3[4] = { 3,D6,D7,D8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_D5[4] = {
  Direction(MD_DOWN     ,rookMoves_D5_0)
 ,Direction(MD_LEFT     ,rookMoves_D5_1)
 ,Direction(MD_RIGHT    ,rookMoves_D5_2)
 ,Direction(MD_UP       ,rookMoves_D5_3)
};
#else
static const Direction rookMoveDirections_D5[4] = {
  Direction(MD_DOWN     ,rookMoves_D5_0,_T("rook MD_DOWN from D5:D4 D3 D2 D1"))
 ,Direction(MD_LEFT     ,rookMoves_D5_1,_T("rook MD_LEFT from D5:C5 B5 A5"))
 ,Direction(MD_RIGHT    ,rookMoves_D5_2,_T("rook MD_RIGHT from D5:E5 F5 G5 H5"))
 ,Direction(MD_UP       ,rookMoves_D5_3,_T("rook MD_UP from D5:D6 D7 D8"))
};
#endif
static const DirectionArray rookMoves_D5(4,rookMoveDirections_D5);

static const int rookMoves_E5_0[5] = { 4,E4,E3,E2,E1 };
static const int rookMoves_E5_1[5] = { 4,D5,C5,B5,A5 };
static const int rookMoves_E5_2[4] = { 3,F5,G5,H5 };
static const int rookMoves_E5_3[4] = { 3,E6,E7,E8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_E5[4] = {
  Direction(MD_DOWN     ,rookMoves_E5_0)
 ,Direction(MD_LEFT     ,rookMoves_E5_1)
 ,Direction(MD_RIGHT    ,rookMoves_E5_2)
 ,Direction(MD_UP       ,rookMoves_E5_3)
};
#else
static const Direction rookMoveDirections_E5[4] = {
  Direction(MD_DOWN     ,rookMoves_E5_0,_T("rook MD_DOWN from E5:E4 E3 E2 E1"))
 ,Direction(MD_LEFT     ,rookMoves_E5_1,_T("rook MD_LEFT from E5:D5 C5 B5 A5"))
 ,Direction(MD_RIGHT    ,rookMoves_E5_2,_T("rook MD_RIGHT from E5:F5 G5 H5"))
 ,Direction(MD_UP       ,rookMoves_E5_3,_T("rook MD_UP from E5:E6 E7 E8"))
};
#endif
static const DirectionArray rookMoves_E5(4,rookMoveDirections_E5);

static const int rookMoves_F5_0[5] = { 4,F4,F3,F2,F1 };
static const int rookMoves_F5_1[6] = { 5,E5,D5,C5,B5,A5 };
static const int rookMoves_F5_2[3] = { 2,G5,H5 };
static const int rookMoves_F5_3[4] = { 3,F6,F7,F8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_F5[4] = {
  Direction(MD_DOWN     ,rookMoves_F5_0)
 ,Direction(MD_LEFT     ,rookMoves_F5_1)
 ,Direction(MD_RIGHT    ,rookMoves_F5_2)
 ,Direction(MD_UP       ,rookMoves_F5_3)
};
#else
static const Direction rookMoveDirections_F5[4] = {
  Direction(MD_DOWN     ,rookMoves_F5_0,_T("rook MD_DOWN from F5:F4 F3 F2 F1"))
 ,Direction(MD_LEFT     ,rookMoves_F5_1,_T("rook MD_LEFT from F5:E5 D5 C5 B5 A5"))
 ,Direction(MD_RIGHT    ,rookMoves_F5_2,_T("rook MD_RIGHT from F5:G5 H5"))
 ,Direction(MD_UP       ,rookMoves_F5_3,_T("rook MD_UP from F5:F6 F7 F8"))
};
#endif
static const DirectionArray rookMoves_F5(4,rookMoveDirections_F5);

static const int rookMoves_G5_0[5] = { 4,G4,G3,G2,G1 };
static const int rookMoves_G5_1[7] = { 6,F5,E5,D5,C5,B5,A5 };
static const int rookMoves_G5_2[2] = { 1,H5 };
static const int rookMoves_G5_3[4] = { 3,G6,G7,G8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_G5[4] = {
  Direction(MD_DOWN     ,rookMoves_G5_0)
 ,Direction(MD_LEFT     ,rookMoves_G5_1)
 ,Direction(MD_RIGHT    ,rookMoves_G5_2)
 ,Direction(MD_UP       ,rookMoves_G5_3)
};
#else
static const Direction rookMoveDirections_G5[4] = {
  Direction(MD_DOWN     ,rookMoves_G5_0,_T("rook MD_DOWN from G5:G4 G3 G2 G1"))
 ,Direction(MD_LEFT     ,rookMoves_G5_1,_T("rook MD_LEFT from G5:F5 E5 D5 C5 B5 A5"))
 ,Direction(MD_RIGHT    ,rookMoves_G5_2,_T("rook MD_RIGHT from G5:H5"))
 ,Direction(MD_UP       ,rookMoves_G5_3,_T("rook MD_UP from G5:G6 G7 G8"))
};
#endif
static const DirectionArray rookMoves_G5(4,rookMoveDirections_G5);

static const int rookMoves_H5_0[5] = { 4,H4,H3,H2,H1 };
static const int rookMoves_H5_1[8] = { 7,G5,F5,E5,D5,C5,B5,A5 };
static const int rookMoves_H5_2[4] = { 3,H6,H7,H8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_H5[3] = {
  Direction(MD_DOWN     ,rookMoves_H5_0)
 ,Direction(MD_LEFT     ,rookMoves_H5_1)
 ,Direction(MD_UP       ,rookMoves_H5_2)
};
#else
static const Direction rookMoveDirections_H5[3] = {
  Direction(MD_DOWN     ,rookMoves_H5_0,_T("rook MD_DOWN from H5:H4 H3 H2 H1"))
 ,Direction(MD_LEFT     ,rookMoves_H5_1,_T("rook MD_LEFT from H5:G5 F5 E5 D5 C5 B5 A5"))
 ,Direction(MD_UP       ,rookMoves_H5_2,_T("rook MD_UP from H5:H6 H7 H8"))
};
#endif
static const DirectionArray rookMoves_H5(3,rookMoveDirections_H5);

static const int rookMoves_A6_0[6] = { 5,A5,A4,A3,A2,A1 };
static const int rookMoves_A6_1[8] = { 7,B6,C6,D6,E6,F6,G6,H6 };
static const int rookMoves_A6_2[3] = { 2,A7,A8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_A6[3] = {
  Direction(MD_DOWN     ,rookMoves_A6_0)
 ,Direction(MD_RIGHT    ,rookMoves_A6_1)
 ,Direction(MD_UP       ,rookMoves_A6_2)
};
#else
static const Direction rookMoveDirections_A6[3] = {
  Direction(MD_DOWN     ,rookMoves_A6_0,_T("rook MD_DOWN from A6:A5 A4 A3 A2 A1"))
 ,Direction(MD_RIGHT    ,rookMoves_A6_1,_T("rook MD_RIGHT from A6:B6 C6 D6 E6 F6 G6 H6"))
 ,Direction(MD_UP       ,rookMoves_A6_2,_T("rook MD_UP from A6:A7 A8"))
};
#endif
static const DirectionArray rookMoves_A6(3,rookMoveDirections_A6);

static const int rookMoves_B6_0[6] = { 5,B5,B4,B3,B2,B1 };
static const int rookMoves_B6_1[2] = { 1,A6 };
static const int rookMoves_B6_2[7] = { 6,C6,D6,E6,F6,G6,H6 };
static const int rookMoves_B6_3[3] = { 2,B7,B8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_B6[4] = {
  Direction(MD_DOWN     ,rookMoves_B6_0)
 ,Direction(MD_LEFT     ,rookMoves_B6_1)
 ,Direction(MD_RIGHT    ,rookMoves_B6_2)
 ,Direction(MD_UP       ,rookMoves_B6_3)
};
#else
static const Direction rookMoveDirections_B6[4] = {
  Direction(MD_DOWN     ,rookMoves_B6_0,_T("rook MD_DOWN from B6:B5 B4 B3 B2 B1"))
 ,Direction(MD_LEFT     ,rookMoves_B6_1,_T("rook MD_LEFT from B6:A6"))
 ,Direction(MD_RIGHT    ,rookMoves_B6_2,_T("rook MD_RIGHT from B6:C6 D6 E6 F6 G6 H6"))
 ,Direction(MD_UP       ,rookMoves_B6_3,_T("rook MD_UP from B6:B7 B8"))
};
#endif
static const DirectionArray rookMoves_B6(4,rookMoveDirections_B6);

static const int rookMoves_C6_0[6] = { 5,C5,C4,C3,C2,C1 };
static const int rookMoves_C6_1[3] = { 2,B6,A6 };
static const int rookMoves_C6_2[6] = { 5,D6,E6,F6,G6,H6 };
static const int rookMoves_C6_3[3] = { 2,C7,C8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_C6[4] = {
  Direction(MD_DOWN     ,rookMoves_C6_0)
 ,Direction(MD_LEFT     ,rookMoves_C6_1)
 ,Direction(MD_RIGHT    ,rookMoves_C6_2)
 ,Direction(MD_UP       ,rookMoves_C6_3)
};
#else
static const Direction rookMoveDirections_C6[4] = {
  Direction(MD_DOWN     ,rookMoves_C6_0,_T("rook MD_DOWN from C6:C5 C4 C3 C2 C1"))
 ,Direction(MD_LEFT     ,rookMoves_C6_1,_T("rook MD_LEFT from C6:B6 A6"))
 ,Direction(MD_RIGHT    ,rookMoves_C6_2,_T("rook MD_RIGHT from C6:D6 E6 F6 G6 H6"))
 ,Direction(MD_UP       ,rookMoves_C6_3,_T("rook MD_UP from C6:C7 C8"))
};
#endif
static const DirectionArray rookMoves_C6(4,rookMoveDirections_C6);

static const int rookMoves_D6_0[6] = { 5,D5,D4,D3,D2,D1 };
static const int rookMoves_D6_1[4] = { 3,C6,B6,A6 };
static const int rookMoves_D6_2[5] = { 4,E6,F6,G6,H6 };
static const int rookMoves_D6_3[3] = { 2,D7,D8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_D6[4] = {
  Direction(MD_DOWN     ,rookMoves_D6_0)
 ,Direction(MD_LEFT     ,rookMoves_D6_1)
 ,Direction(MD_RIGHT    ,rookMoves_D6_2)
 ,Direction(MD_UP       ,rookMoves_D6_3)
};
#else
static const Direction rookMoveDirections_D6[4] = {
  Direction(MD_DOWN     ,rookMoves_D6_0,_T("rook MD_DOWN from D6:D5 D4 D3 D2 D1"))
 ,Direction(MD_LEFT     ,rookMoves_D6_1,_T("rook MD_LEFT from D6:C6 B6 A6"))
 ,Direction(MD_RIGHT    ,rookMoves_D6_2,_T("rook MD_RIGHT from D6:E6 F6 G6 H6"))
 ,Direction(MD_UP       ,rookMoves_D6_3,_T("rook MD_UP from D6:D7 D8"))
};
#endif
static const DirectionArray rookMoves_D6(4,rookMoveDirections_D6);

static const int rookMoves_E6_0[6] = { 5,E5,E4,E3,E2,E1 };
static const int rookMoves_E6_1[5] = { 4,D6,C6,B6,A6 };
static const int rookMoves_E6_2[4] = { 3,F6,G6,H6 };
static const int rookMoves_E6_3[3] = { 2,E7,E8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_E6[4] = {
  Direction(MD_DOWN     ,rookMoves_E6_0)
 ,Direction(MD_LEFT     ,rookMoves_E6_1)
 ,Direction(MD_RIGHT    ,rookMoves_E6_2)
 ,Direction(MD_UP       ,rookMoves_E6_3)
};
#else
static const Direction rookMoveDirections_E6[4] = {
  Direction(MD_DOWN     ,rookMoves_E6_0,_T("rook MD_DOWN from E6:E5 E4 E3 E2 E1"))
 ,Direction(MD_LEFT     ,rookMoves_E6_1,_T("rook MD_LEFT from E6:D6 C6 B6 A6"))
 ,Direction(MD_RIGHT    ,rookMoves_E6_2,_T("rook MD_RIGHT from E6:F6 G6 H6"))
 ,Direction(MD_UP       ,rookMoves_E6_3,_T("rook MD_UP from E6:E7 E8"))
};
#endif
static const DirectionArray rookMoves_E6(4,rookMoveDirections_E6);

static const int rookMoves_F6_0[6] = { 5,F5,F4,F3,F2,F1 };
static const int rookMoves_F6_1[6] = { 5,E6,D6,C6,B6,A6 };
static const int rookMoves_F6_2[3] = { 2,G6,H6 };
static const int rookMoves_F6_3[3] = { 2,F7,F8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_F6[4] = {
  Direction(MD_DOWN     ,rookMoves_F6_0)
 ,Direction(MD_LEFT     ,rookMoves_F6_1)
 ,Direction(MD_RIGHT    ,rookMoves_F6_2)
 ,Direction(MD_UP       ,rookMoves_F6_3)
};
#else
static const Direction rookMoveDirections_F6[4] = {
  Direction(MD_DOWN     ,rookMoves_F6_0,_T("rook MD_DOWN from F6:F5 F4 F3 F2 F1"))
 ,Direction(MD_LEFT     ,rookMoves_F6_1,_T("rook MD_LEFT from F6:E6 D6 C6 B6 A6"))
 ,Direction(MD_RIGHT    ,rookMoves_F6_2,_T("rook MD_RIGHT from F6:G6 H6"))
 ,Direction(MD_UP       ,rookMoves_F6_3,_T("rook MD_UP from F6:F7 F8"))
};
#endif
static const DirectionArray rookMoves_F6(4,rookMoveDirections_F6);

static const int rookMoves_G6_0[6] = { 5,G5,G4,G3,G2,G1 };
static const int rookMoves_G6_1[7] = { 6,F6,E6,D6,C6,B6,A6 };
static const int rookMoves_G6_2[2] = { 1,H6 };
static const int rookMoves_G6_3[3] = { 2,G7,G8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_G6[4] = {
  Direction(MD_DOWN     ,rookMoves_G6_0)
 ,Direction(MD_LEFT     ,rookMoves_G6_1)
 ,Direction(MD_RIGHT    ,rookMoves_G6_2)
 ,Direction(MD_UP       ,rookMoves_G6_3)
};
#else
static const Direction rookMoveDirections_G6[4] = {
  Direction(MD_DOWN     ,rookMoves_G6_0,_T("rook MD_DOWN from G6:G5 G4 G3 G2 G1"))
 ,Direction(MD_LEFT     ,rookMoves_G6_1,_T("rook MD_LEFT from G6:F6 E6 D6 C6 B6 A6"))
 ,Direction(MD_RIGHT    ,rookMoves_G6_2,_T("rook MD_RIGHT from G6:H6"))
 ,Direction(MD_UP       ,rookMoves_G6_3,_T("rook MD_UP from G6:G7 G8"))
};
#endif
static const DirectionArray rookMoves_G6(4,rookMoveDirections_G6);

static const int rookMoves_H6_0[6] = { 5,H5,H4,H3,H2,H1 };
static const int rookMoves_H6_1[8] = { 7,G6,F6,E6,D6,C6,B6,A6 };
static const int rookMoves_H6_2[3] = { 2,H7,H8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_H6[3] = {
  Direction(MD_DOWN     ,rookMoves_H6_0)
 ,Direction(MD_LEFT     ,rookMoves_H6_1)
 ,Direction(MD_UP       ,rookMoves_H6_2)
};
#else
static const Direction rookMoveDirections_H6[3] = {
  Direction(MD_DOWN     ,rookMoves_H6_0,_T("rook MD_DOWN from H6:H5 H4 H3 H2 H1"))
 ,Direction(MD_LEFT     ,rookMoves_H6_1,_T("rook MD_LEFT from H6:G6 F6 E6 D6 C6 B6 A6"))
 ,Direction(MD_UP       ,rookMoves_H6_2,_T("rook MD_UP from H6:H7 H8"))
};
#endif
static const DirectionArray rookMoves_H6(3,rookMoveDirections_H6);

static const int rookMoves_A7_0[7] = { 6,A6,A5,A4,A3,A2,A1 };
static const int rookMoves_A7_1[8] = { 7,B7,C7,D7,E7,F7,G7,H7 };
static const int rookMoves_A7_2[2] = { 1,A8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_A7[3] = {
  Direction(MD_DOWN     ,rookMoves_A7_0)
 ,Direction(MD_RIGHT    ,rookMoves_A7_1)
 ,Direction(MD_UP       ,rookMoves_A7_2)
};
#else
static const Direction rookMoveDirections_A7[3] = {
  Direction(MD_DOWN     ,rookMoves_A7_0,_T("rook MD_DOWN from A7:A6 A5 A4 A3 A2 A1"))
 ,Direction(MD_RIGHT    ,rookMoves_A7_1,_T("rook MD_RIGHT from A7:B7 C7 D7 E7 F7 G7 H7"))
 ,Direction(MD_UP       ,rookMoves_A7_2,_T("rook MD_UP from A7:A8"))
};
#endif
static const DirectionArray rookMoves_A7(3,rookMoveDirections_A7);

static const int rookMoves_B7_0[7] = { 6,B6,B5,B4,B3,B2,B1 };
static const int rookMoves_B7_1[2] = { 1,A7 };
static const int rookMoves_B7_2[7] = { 6,C7,D7,E7,F7,G7,H7 };
static const int rookMoves_B7_3[2] = { 1,B8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_B7[4] = {
  Direction(MD_DOWN     ,rookMoves_B7_0)
 ,Direction(MD_LEFT     ,rookMoves_B7_1)
 ,Direction(MD_RIGHT    ,rookMoves_B7_2)
 ,Direction(MD_UP       ,rookMoves_B7_3)
};
#else
static const Direction rookMoveDirections_B7[4] = {
  Direction(MD_DOWN     ,rookMoves_B7_0,_T("rook MD_DOWN from B7:B6 B5 B4 B3 B2 B1"))
 ,Direction(MD_LEFT     ,rookMoves_B7_1,_T("rook MD_LEFT from B7:A7"))
 ,Direction(MD_RIGHT    ,rookMoves_B7_2,_T("rook MD_RIGHT from B7:C7 D7 E7 F7 G7 H7"))
 ,Direction(MD_UP       ,rookMoves_B7_3,_T("rook MD_UP from B7:B8"))
};
#endif
static const DirectionArray rookMoves_B7(4,rookMoveDirections_B7);

static const int rookMoves_C7_0[7] = { 6,C6,C5,C4,C3,C2,C1 };
static const int rookMoves_C7_1[3] = { 2,B7,A7 };
static const int rookMoves_C7_2[6] = { 5,D7,E7,F7,G7,H7 };
static const int rookMoves_C7_3[2] = { 1,C8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_C7[4] = {
  Direction(MD_DOWN     ,rookMoves_C7_0)
 ,Direction(MD_LEFT     ,rookMoves_C7_1)
 ,Direction(MD_RIGHT    ,rookMoves_C7_2)
 ,Direction(MD_UP       ,rookMoves_C7_3)
};
#else
static const Direction rookMoveDirections_C7[4] = {
  Direction(MD_DOWN     ,rookMoves_C7_0,_T("rook MD_DOWN from C7:C6 C5 C4 C3 C2 C1"))
 ,Direction(MD_LEFT     ,rookMoves_C7_1,_T("rook MD_LEFT from C7:B7 A7"))
 ,Direction(MD_RIGHT    ,rookMoves_C7_2,_T("rook MD_RIGHT from C7:D7 E7 F7 G7 H7"))
 ,Direction(MD_UP       ,rookMoves_C7_3,_T("rook MD_UP from C7:C8"))
};
#endif
static const DirectionArray rookMoves_C7(4,rookMoveDirections_C7);

static const int rookMoves_D7_0[7] = { 6,D6,D5,D4,D3,D2,D1 };
static const int rookMoves_D7_1[4] = { 3,C7,B7,A7 };
static const int rookMoves_D7_2[5] = { 4,E7,F7,G7,H7 };
static const int rookMoves_D7_3[2] = { 1,D8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_D7[4] = {
  Direction(MD_DOWN     ,rookMoves_D7_0)
 ,Direction(MD_LEFT     ,rookMoves_D7_1)
 ,Direction(MD_RIGHT    ,rookMoves_D7_2)
 ,Direction(MD_UP       ,rookMoves_D7_3)
};
#else
static const Direction rookMoveDirections_D7[4] = {
  Direction(MD_DOWN     ,rookMoves_D7_0,_T("rook MD_DOWN from D7:D6 D5 D4 D3 D2 D1"))
 ,Direction(MD_LEFT     ,rookMoves_D7_1,_T("rook MD_LEFT from D7:C7 B7 A7"))
 ,Direction(MD_RIGHT    ,rookMoves_D7_2,_T("rook MD_RIGHT from D7:E7 F7 G7 H7"))
 ,Direction(MD_UP       ,rookMoves_D7_3,_T("rook MD_UP from D7:D8"))
};
#endif
static const DirectionArray rookMoves_D7(4,rookMoveDirections_D7);

static const int rookMoves_E7_0[7] = { 6,E6,E5,E4,E3,E2,E1 };
static const int rookMoves_E7_1[5] = { 4,D7,C7,B7,A7 };
static const int rookMoves_E7_2[4] = { 3,F7,G7,H7 };
static const int rookMoves_E7_3[2] = { 1,E8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_E7[4] = {
  Direction(MD_DOWN     ,rookMoves_E7_0)
 ,Direction(MD_LEFT     ,rookMoves_E7_1)
 ,Direction(MD_RIGHT    ,rookMoves_E7_2)
 ,Direction(MD_UP       ,rookMoves_E7_3)
};
#else
static const Direction rookMoveDirections_E7[4] = {
  Direction(MD_DOWN     ,rookMoves_E7_0,_T("rook MD_DOWN from E7:E6 E5 E4 E3 E2 E1"))
 ,Direction(MD_LEFT     ,rookMoves_E7_1,_T("rook MD_LEFT from E7:D7 C7 B7 A7"))
 ,Direction(MD_RIGHT    ,rookMoves_E7_2,_T("rook MD_RIGHT from E7:F7 G7 H7"))
 ,Direction(MD_UP       ,rookMoves_E7_3,_T("rook MD_UP from E7:E8"))
};
#endif
static const DirectionArray rookMoves_E7(4,rookMoveDirections_E7);

static const int rookMoves_F7_0[7] = { 6,F6,F5,F4,F3,F2,F1 };
static const int rookMoves_F7_1[6] = { 5,E7,D7,C7,B7,A7 };
static const int rookMoves_F7_2[3] = { 2,G7,H7 };
static const int rookMoves_F7_3[2] = { 1,F8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_F7[4] = {
  Direction(MD_DOWN     ,rookMoves_F7_0)
 ,Direction(MD_LEFT     ,rookMoves_F7_1)
 ,Direction(MD_RIGHT    ,rookMoves_F7_2)
 ,Direction(MD_UP       ,rookMoves_F7_3)
};
#else
static const Direction rookMoveDirections_F7[4] = {
  Direction(MD_DOWN     ,rookMoves_F7_0,_T("rook MD_DOWN from F7:F6 F5 F4 F3 F2 F1"))
 ,Direction(MD_LEFT     ,rookMoves_F7_1,_T("rook MD_LEFT from F7:E7 D7 C7 B7 A7"))
 ,Direction(MD_RIGHT    ,rookMoves_F7_2,_T("rook MD_RIGHT from F7:G7 H7"))
 ,Direction(MD_UP       ,rookMoves_F7_3,_T("rook MD_UP from F7:F8"))
};
#endif
static const DirectionArray rookMoves_F7(4,rookMoveDirections_F7);

static const int rookMoves_G7_0[7] = { 6,G6,G5,G4,G3,G2,G1 };
static const int rookMoves_G7_1[7] = { 6,F7,E7,D7,C7,B7,A7 };
static const int rookMoves_G7_2[2] = { 1,H7 };
static const int rookMoves_G7_3[2] = { 1,G8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_G7[4] = {
  Direction(MD_DOWN     ,rookMoves_G7_0)
 ,Direction(MD_LEFT     ,rookMoves_G7_1)
 ,Direction(MD_RIGHT    ,rookMoves_G7_2)
 ,Direction(MD_UP       ,rookMoves_G7_3)
};
#else
static const Direction rookMoveDirections_G7[4] = {
  Direction(MD_DOWN     ,rookMoves_G7_0,_T("rook MD_DOWN from G7:G6 G5 G4 G3 G2 G1"))
 ,Direction(MD_LEFT     ,rookMoves_G7_1,_T("rook MD_LEFT from G7:F7 E7 D7 C7 B7 A7"))
 ,Direction(MD_RIGHT    ,rookMoves_G7_2,_T("rook MD_RIGHT from G7:H7"))
 ,Direction(MD_UP       ,rookMoves_G7_3,_T("rook MD_UP from G7:G8"))
};
#endif
static const DirectionArray rookMoves_G7(4,rookMoveDirections_G7);

static const int rookMoves_H7_0[7] = { 6,H6,H5,H4,H3,H2,H1 };
static const int rookMoves_H7_1[8] = { 7,G7,F7,E7,D7,C7,B7,A7 };
static const int rookMoves_H7_2[2] = { 1,H8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_H7[3] = {
  Direction(MD_DOWN     ,rookMoves_H7_0)
 ,Direction(MD_LEFT     ,rookMoves_H7_1)
 ,Direction(MD_UP       ,rookMoves_H7_2)
};
#else
static const Direction rookMoveDirections_H7[3] = {
  Direction(MD_DOWN     ,rookMoves_H7_0,_T("rook MD_DOWN from H7:H6 H5 H4 H3 H2 H1"))
 ,Direction(MD_LEFT     ,rookMoves_H7_1,_T("rook MD_LEFT from H7:G7 F7 E7 D7 C7 B7 A7"))
 ,Direction(MD_UP       ,rookMoves_H7_2,_T("rook MD_UP from H7:H8"))
};
#endif
static const DirectionArray rookMoves_H7(3,rookMoveDirections_H7);

static const int rookMoves_A8_0[8] = { 7,A7,A6,A5,A4,A3,A2,A1 };
static const int rookMoves_A8_1[8] = { 7,B8,C8,D8,E8,F8,G8,H8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_A8[2] = {
  Direction(MD_DOWN     ,rookMoves_A8_0)
 ,Direction(MD_RIGHT    ,rookMoves_A8_1)
};
#else
static const Direction rookMoveDirections_A8[2] = {
  Direction(MD_DOWN     ,rookMoves_A8_0,_T("rook MD_DOWN from A8:A7 A6 A5 A4 A3 A2 A1"))
 ,Direction(MD_RIGHT    ,rookMoves_A8_1,_T("rook MD_RIGHT from A8:B8 C8 D8 E8 F8 G8 H8"))
};
#endif
static const DirectionArray rookMoves_A8(2,rookMoveDirections_A8);

static const int rookMoves_B8_0[8] = { 7,B7,B6,B5,B4,B3,B2,B1 };
static const int rookMoves_B8_1[2] = { 1,A8 };
static const int rookMoves_B8_2[7] = { 6,C8,D8,E8,F8,G8,H8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_B8[3] = {
  Direction(MD_DOWN     ,rookMoves_B8_0)
 ,Direction(MD_LEFT     ,rookMoves_B8_1)
 ,Direction(MD_RIGHT    ,rookMoves_B8_2)
};
#else
static const Direction rookMoveDirections_B8[3] = {
  Direction(MD_DOWN     ,rookMoves_B8_0,_T("rook MD_DOWN from B8:B7 B6 B5 B4 B3 B2 B1"))
 ,Direction(MD_LEFT     ,rookMoves_B8_1,_T("rook MD_LEFT from B8:A8"))
 ,Direction(MD_RIGHT    ,rookMoves_B8_2,_T("rook MD_RIGHT from B8:C8 D8 E8 F8 G8 H8"))
};
#endif
static const DirectionArray rookMoves_B8(3,rookMoveDirections_B8);

static const int rookMoves_C8_0[8] = { 7,C7,C6,C5,C4,C3,C2,C1 };
static const int rookMoves_C8_1[3] = { 2,B8,A8 };
static const int rookMoves_C8_2[6] = { 5,D8,E8,F8,G8,H8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_C8[3] = {
  Direction(MD_DOWN     ,rookMoves_C8_0)
 ,Direction(MD_LEFT     ,rookMoves_C8_1)
 ,Direction(MD_RIGHT    ,rookMoves_C8_2)
};
#else
static const Direction rookMoveDirections_C8[3] = {
  Direction(MD_DOWN     ,rookMoves_C8_0,_T("rook MD_DOWN from C8:C7 C6 C5 C4 C3 C2 C1"))
 ,Direction(MD_LEFT     ,rookMoves_C8_1,_T("rook MD_LEFT from C8:B8 A8"))
 ,Direction(MD_RIGHT    ,rookMoves_C8_2,_T("rook MD_RIGHT from C8:D8 E8 F8 G8 H8"))
};
#endif
static const DirectionArray rookMoves_C8(3,rookMoveDirections_C8);

static const int rookMoves_D8_0[8] = { 7,D7,D6,D5,D4,D3,D2,D1 };
static const int rookMoves_D8_1[4] = { 3,C8,B8,A8 };
static const int rookMoves_D8_2[5] = { 4,E8,F8,G8,H8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_D8[3] = {
  Direction(MD_DOWN     ,rookMoves_D8_0)
 ,Direction(MD_LEFT     ,rookMoves_D8_1)
 ,Direction(MD_RIGHT    ,rookMoves_D8_2)
};
#else
static const Direction rookMoveDirections_D8[3] = {
  Direction(MD_DOWN     ,rookMoves_D8_0,_T("rook MD_DOWN from D8:D7 D6 D5 D4 D3 D2 D1"))
 ,Direction(MD_LEFT     ,rookMoves_D8_1,_T("rook MD_LEFT from D8:C8 B8 A8"))
 ,Direction(MD_RIGHT    ,rookMoves_D8_2,_T("rook MD_RIGHT from D8:E8 F8 G8 H8"))
};
#endif
static const DirectionArray rookMoves_D8(3,rookMoveDirections_D8);

static const int rookMoves_E8_0[8] = { 7,E7,E6,E5,E4,E3,E2,E1 };
static const int rookMoves_E8_1[5] = { 4,D8,C8,B8,A8 };
static const int rookMoves_E8_2[4] = { 3,F8,G8,H8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_E8[3] = {
  Direction(MD_DOWN     ,rookMoves_E8_0)
 ,Direction(MD_LEFT     ,rookMoves_E8_1)
 ,Direction(MD_RIGHT    ,rookMoves_E8_2)
};
#else
static const Direction rookMoveDirections_E8[3] = {
  Direction(MD_DOWN     ,rookMoves_E8_0,_T("rook MD_DOWN from E8:E7 E6 E5 E4 E3 E2 E1"))
 ,Direction(MD_LEFT     ,rookMoves_E8_1,_T("rook MD_LEFT from E8:D8 C8 B8 A8"))
 ,Direction(MD_RIGHT    ,rookMoves_E8_2,_T("rook MD_RIGHT from E8:F8 G8 H8"))
};
#endif
static const DirectionArray rookMoves_E8(3,rookMoveDirections_E8);

static const int rookMoves_F8_0[8] = { 7,F7,F6,F5,F4,F3,F2,F1 };
static const int rookMoves_F8_1[6] = { 5,E8,D8,C8,B8,A8 };
static const int rookMoves_F8_2[3] = { 2,G8,H8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_F8[3] = {
  Direction(MD_DOWN     ,rookMoves_F8_0)
 ,Direction(MD_LEFT     ,rookMoves_F8_1)
 ,Direction(MD_RIGHT    ,rookMoves_F8_2)
};
#else
static const Direction rookMoveDirections_F8[3] = {
  Direction(MD_DOWN     ,rookMoves_F8_0,_T("rook MD_DOWN from F8:F7 F6 F5 F4 F3 F2 F1"))
 ,Direction(MD_LEFT     ,rookMoves_F8_1,_T("rook MD_LEFT from F8:E8 D8 C8 B8 A8"))
 ,Direction(MD_RIGHT    ,rookMoves_F8_2,_T("rook MD_RIGHT from F8:G8 H8"))
};
#endif
static const DirectionArray rookMoves_F8(3,rookMoveDirections_F8);

static const int rookMoves_G8_0[8] = { 7,G7,G6,G5,G4,G3,G2,G1 };
static const int rookMoves_G8_1[7] = { 6,F8,E8,D8,C8,B8,A8 };
static const int rookMoves_G8_2[2] = { 1,H8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_G8[3] = {
  Direction(MD_DOWN     ,rookMoves_G8_0)
 ,Direction(MD_LEFT     ,rookMoves_G8_1)
 ,Direction(MD_RIGHT    ,rookMoves_G8_2)
};
#else
static const Direction rookMoveDirections_G8[3] = {
  Direction(MD_DOWN     ,rookMoves_G8_0,_T("rook MD_DOWN from G8:G7 G6 G5 G4 G3 G2 G1"))
 ,Direction(MD_LEFT     ,rookMoves_G8_1,_T("rook MD_LEFT from G8:F8 E8 D8 C8 B8 A8"))
 ,Direction(MD_RIGHT    ,rookMoves_G8_2,_T("rook MD_RIGHT from G8:H8"))
};
#endif
static const DirectionArray rookMoves_G8(3,rookMoveDirections_G8);

static const int rookMoves_H8_0[8] = { 7,H7,H6,H5,H4,H3,H2,H1 };
static const int rookMoves_H8_1[8] = { 7,G8,F8,E8,D8,C8,B8,A8 };
#ifndef _DEBUG
static const Direction rookMoveDirections_H8[2] = {
  Direction(MD_DOWN     ,rookMoves_H8_0)
 ,Direction(MD_LEFT     ,rookMoves_H8_1)
};
#else
static const Direction rookMoveDirections_H8[2] = {
  Direction(MD_DOWN     ,rookMoves_H8_0,_T("rook MD_DOWN from H8:H7 H6 H5 H4 H3 H2 H1"))
 ,Direction(MD_LEFT     ,rookMoves_H8_1,_T("rook MD_LEFT from H8:G8 F8 E8 D8 C8 B8 A8"))
};
#endif
static const DirectionArray rookMoves_H8(2,rookMoveDirections_H8);

static const int bishopMoves_A1_0[8] = { 7,B2,C3,D4,E5,F6,G7,H8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_A1[1] = {
  Direction(MD_UPDIAG1  ,bishopMoves_A1_0)
};
#else
static const Direction bishopMoveDirections_A1[1] = {
  Direction(MD_UPDIAG1  ,bishopMoves_A1_0,_T("bishop MD_UPDIAG1 from A1:B2 C3 D4 E5 F6 G7 H8"))
};
#endif
static const DirectionArray bishopMoves_A1(1,bishopMoveDirections_A1);

static const int bishopMoves_B1_0[2] = { 1,A2 };
static const int bishopMoves_B1_1[7] = { 6,C2,D3,E4,F5,G6,H7 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_B1[2] = {
  Direction(MD_UPDIAG2  ,bishopMoves_B1_0)
 ,Direction(MD_UPDIAG1  ,bishopMoves_B1_1)
};
#else
static const Direction bishopMoveDirections_B1[2] = {
  Direction(MD_UPDIAG2  ,bishopMoves_B1_0,_T("bishop MD_UPDIAG2 from B1:A2"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_B1_1,_T("bishop MD_UPDIAG1 from B1:C2 D3 E4 F5 G6 H7"))
};
#endif
static const DirectionArray bishopMoves_B1(2,bishopMoveDirections_B1);

static const int bishopMoves_C1_0[3] = { 2,B2,A3 };
static const int bishopMoves_C1_1[6] = { 5,D2,E3,F4,G5,H6 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_C1[2] = {
  Direction(MD_UPDIAG2  ,bishopMoves_C1_0)
 ,Direction(MD_UPDIAG1  ,bishopMoves_C1_1)
};
#else
static const Direction bishopMoveDirections_C1[2] = {
  Direction(MD_UPDIAG2  ,bishopMoves_C1_0,_T("bishop MD_UPDIAG2 from C1:B2 A3"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_C1_1,_T("bishop MD_UPDIAG1 from C1:D2 E3 F4 G5 H6"))
};
#endif
static const DirectionArray bishopMoves_C1(2,bishopMoveDirections_C1);

static const int bishopMoves_D1_0[4] = { 3,C2,B3,A4 };
static const int bishopMoves_D1_1[5] = { 4,E2,F3,G4,H5 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_D1[2] = {
  Direction(MD_UPDIAG2  ,bishopMoves_D1_0)
 ,Direction(MD_UPDIAG1  ,bishopMoves_D1_1)
};
#else
static const Direction bishopMoveDirections_D1[2] = {
  Direction(MD_UPDIAG2  ,bishopMoves_D1_0,_T("bishop MD_UPDIAG2 from D1:C2 B3 A4"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_D1_1,_T("bishop MD_UPDIAG1 from D1:E2 F3 G4 H5"))
};
#endif
static const DirectionArray bishopMoves_D1(2,bishopMoveDirections_D1);

static const int bishopMoves_E1_0[5] = { 4,D2,C3,B4,A5 };
static const int bishopMoves_E1_1[4] = { 3,F2,G3,H4 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_E1[2] = {
  Direction(MD_UPDIAG2  ,bishopMoves_E1_0)
 ,Direction(MD_UPDIAG1  ,bishopMoves_E1_1)
};
#else
static const Direction bishopMoveDirections_E1[2] = {
  Direction(MD_UPDIAG2  ,bishopMoves_E1_0,_T("bishop MD_UPDIAG2 from E1:D2 C3 B4 A5"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_E1_1,_T("bishop MD_UPDIAG1 from E1:F2 G3 H4"))
};
#endif
static const DirectionArray bishopMoves_E1(2,bishopMoveDirections_E1);

static const int bishopMoves_F1_0[6] = { 5,E2,D3,C4,B5,A6 };
static const int bishopMoves_F1_1[3] = { 2,G2,H3 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_F1[2] = {
  Direction(MD_UPDIAG2  ,bishopMoves_F1_0)
 ,Direction(MD_UPDIAG1  ,bishopMoves_F1_1)
};
#else
static const Direction bishopMoveDirections_F1[2] = {
  Direction(MD_UPDIAG2  ,bishopMoves_F1_0,_T("bishop MD_UPDIAG2 from F1:E2 D3 C4 B5 A6"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_F1_1,_T("bishop MD_UPDIAG1 from F1:G2 H3"))
};
#endif
static const DirectionArray bishopMoves_F1(2,bishopMoveDirections_F1);

static const int bishopMoves_G1_0[7] = { 6,F2,E3,D4,C5,B6,A7 };
static const int bishopMoves_G1_1[2] = { 1,H2 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_G1[2] = {
  Direction(MD_UPDIAG2  ,bishopMoves_G1_0)
 ,Direction(MD_UPDIAG1  ,bishopMoves_G1_1)
};
#else
static const Direction bishopMoveDirections_G1[2] = {
  Direction(MD_UPDIAG2  ,bishopMoves_G1_0,_T("bishop MD_UPDIAG2 from G1:F2 E3 D4 C5 B6 A7"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_G1_1,_T("bishop MD_UPDIAG1 from G1:H2"))
};
#endif
static const DirectionArray bishopMoves_G1(2,bishopMoveDirections_G1);

static const int bishopMoves_H1_0[8] = { 7,G2,F3,E4,D5,C6,B7,A8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_H1[1] = {
  Direction(MD_UPDIAG2  ,bishopMoves_H1_0)
};
#else
static const Direction bishopMoveDirections_H1[1] = {
  Direction(MD_UPDIAG2  ,bishopMoves_H1_0,_T("bishop MD_UPDIAG2 from H1:G2 F3 E4 D5 C6 B7 A8"))
};
#endif
static const DirectionArray bishopMoves_H1(1,bishopMoveDirections_H1);

static const int bishopMoves_A2_0[2] = { 1,B1 };
static const int bishopMoves_A2_1[7] = { 6,B3,C4,D5,E6,F7,G8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_A2[2] = {
  Direction(MD_DOWNDIAG2,bishopMoves_A2_0)
 ,Direction(MD_UPDIAG1  ,bishopMoves_A2_1)
};
#else
static const Direction bishopMoveDirections_A2[2] = {
  Direction(MD_DOWNDIAG2,bishopMoves_A2_0,_T("bishop MD_DOWNDIAG2 from A2:B1"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_A2_1,_T("bishop MD_UPDIAG1 from A2:B3 C4 D5 E6 F7 G8"))
};
#endif
static const DirectionArray bishopMoves_A2(2,bishopMoveDirections_A2);

static const int bishopMoves_B2_0[2] = { 1,A1 };
static const int bishopMoves_B2_1[2] = { 1,C1 };
static const int bishopMoves_B2_2[2] = { 1,A3 };
static const int bishopMoves_B2_3[7] = { 6,C3,D4,E5,F6,G7,H8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_B2[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_B2_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_B2_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_B2_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_B2_3)
};
#else
static const Direction bishopMoveDirections_B2[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_B2_0,_T("bishop MD_DOWNDIAG1 from B2:A1"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_B2_1,_T("bishop MD_DOWNDIAG2 from B2:C1"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_B2_2,_T("bishop MD_UPDIAG2 from B2:A3"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_B2_3,_T("bishop MD_UPDIAG1 from B2:C3 D4 E5 F6 G7 H8"))
};
#endif
static const DirectionArray bishopMoves_B2(4,bishopMoveDirections_B2);

static const int bishopMoves_C2_0[2] = { 1,B1 };
static const int bishopMoves_C2_1[2] = { 1,D1 };
static const int bishopMoves_C2_2[3] = { 2,B3,A4 };
static const int bishopMoves_C2_3[6] = { 5,D3,E4,F5,G6,H7 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_C2[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_C2_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_C2_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_C2_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_C2_3)
};
#else
static const Direction bishopMoveDirections_C2[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_C2_0,_T("bishop MD_DOWNDIAG1 from C2:B1"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_C2_1,_T("bishop MD_DOWNDIAG2 from C2:D1"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_C2_2,_T("bishop MD_UPDIAG2 from C2:B3 A4"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_C2_3,_T("bishop MD_UPDIAG1 from C2:D3 E4 F5 G6 H7"))
};
#endif
static const DirectionArray bishopMoves_C2(4,bishopMoveDirections_C2);

static const int bishopMoves_D2_0[2] = { 1,C1 };
static const int bishopMoves_D2_1[2] = { 1,E1 };
static const int bishopMoves_D2_2[4] = { 3,C3,B4,A5 };
static const int bishopMoves_D2_3[5] = { 4,E3,F4,G5,H6 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_D2[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_D2_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_D2_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_D2_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_D2_3)
};
#else
static const Direction bishopMoveDirections_D2[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_D2_0,_T("bishop MD_DOWNDIAG1 from D2:C1"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_D2_1,_T("bishop MD_DOWNDIAG2 from D2:E1"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_D2_2,_T("bishop MD_UPDIAG2 from D2:C3 B4 A5"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_D2_3,_T("bishop MD_UPDIAG1 from D2:E3 F4 G5 H6"))
};
#endif
static const DirectionArray bishopMoves_D2(4,bishopMoveDirections_D2);

static const int bishopMoves_E2_0[2] = { 1,D1 };
static const int bishopMoves_E2_1[2] = { 1,F1 };
static const int bishopMoves_E2_2[5] = { 4,D3,C4,B5,A6 };
static const int bishopMoves_E2_3[4] = { 3,F3,G4,H5 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_E2[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_E2_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_E2_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_E2_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_E2_3)
};
#else
static const Direction bishopMoveDirections_E2[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_E2_0,_T("bishop MD_DOWNDIAG1 from E2:D1"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_E2_1,_T("bishop MD_DOWNDIAG2 from E2:F1"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_E2_2,_T("bishop MD_UPDIAG2 from E2:D3 C4 B5 A6"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_E2_3,_T("bishop MD_UPDIAG1 from E2:F3 G4 H5"))
};
#endif
static const DirectionArray bishopMoves_E2(4,bishopMoveDirections_E2);

static const int bishopMoves_F2_0[2] = { 1,E1 };
static const int bishopMoves_F2_1[2] = { 1,G1 };
static const int bishopMoves_F2_2[6] = { 5,E3,D4,C5,B6,A7 };
static const int bishopMoves_F2_3[3] = { 2,G3,H4 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_F2[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_F2_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_F2_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_F2_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_F2_3)
};
#else
static const Direction bishopMoveDirections_F2[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_F2_0,_T("bishop MD_DOWNDIAG1 from F2:E1"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_F2_1,_T("bishop MD_DOWNDIAG2 from F2:G1"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_F2_2,_T("bishop MD_UPDIAG2 from F2:E3 D4 C5 B6 A7"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_F2_3,_T("bishop MD_UPDIAG1 from F2:G3 H4"))
};
#endif
static const DirectionArray bishopMoves_F2(4,bishopMoveDirections_F2);

static const int bishopMoves_G2_0[2] = { 1,F1 };
static const int bishopMoves_G2_1[2] = { 1,H1 };
static const int bishopMoves_G2_2[7] = { 6,F3,E4,D5,C6,B7,A8 };
static const int bishopMoves_G2_3[2] = { 1,H3 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_G2[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_G2_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_G2_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_G2_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_G2_3)
};
#else
static const Direction bishopMoveDirections_G2[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_G2_0,_T("bishop MD_DOWNDIAG1 from G2:F1"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_G2_1,_T("bishop MD_DOWNDIAG2 from G2:H1"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_G2_2,_T("bishop MD_UPDIAG2 from G2:F3 E4 D5 C6 B7 A8"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_G2_3,_T("bishop MD_UPDIAG1 from G2:H3"))
};
#endif
static const DirectionArray bishopMoves_G2(4,bishopMoveDirections_G2);

static const int bishopMoves_H2_0[2] = { 1,G1 };
static const int bishopMoves_H2_1[7] = { 6,G3,F4,E5,D6,C7,B8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_H2[2] = {
  Direction(MD_DOWNDIAG1,bishopMoves_H2_0)
 ,Direction(MD_UPDIAG2  ,bishopMoves_H2_1)
};
#else
static const Direction bishopMoveDirections_H2[2] = {
  Direction(MD_DOWNDIAG1,bishopMoves_H2_0,_T("bishop MD_DOWNDIAG1 from H2:G1"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_H2_1,_T("bishop MD_UPDIAG2 from H2:G3 F4 E5 D6 C7 B8"))
};
#endif
static const DirectionArray bishopMoves_H2(2,bishopMoveDirections_H2);

static const int bishopMoves_A3_0[3] = { 2,B2,C1 };
static const int bishopMoves_A3_1[6] = { 5,B4,C5,D6,E7,F8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_A3[2] = {
  Direction(MD_DOWNDIAG2,bishopMoves_A3_0)
 ,Direction(MD_UPDIAG1  ,bishopMoves_A3_1)
};
#else
static const Direction bishopMoveDirections_A3[2] = {
  Direction(MD_DOWNDIAG2,bishopMoves_A3_0,_T("bishop MD_DOWNDIAG2 from A3:B2 C1"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_A3_1,_T("bishop MD_UPDIAG1 from A3:B4 C5 D6 E7 F8"))
};
#endif
static const DirectionArray bishopMoves_A3(2,bishopMoveDirections_A3);

static const int bishopMoves_B3_0[2] = { 1,A2 };
static const int bishopMoves_B3_1[3] = { 2,C2,D1 };
static const int bishopMoves_B3_2[2] = { 1,A4 };
static const int bishopMoves_B3_3[6] = { 5,C4,D5,E6,F7,G8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_B3[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_B3_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_B3_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_B3_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_B3_3)
};
#else
static const Direction bishopMoveDirections_B3[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_B3_0,_T("bishop MD_DOWNDIAG1 from B3:A2"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_B3_1,_T("bishop MD_DOWNDIAG2 from B3:C2 D1"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_B3_2,_T("bishop MD_UPDIAG2 from B3:A4"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_B3_3,_T("bishop MD_UPDIAG1 from B3:C4 D5 E6 F7 G8"))
};
#endif
static const DirectionArray bishopMoves_B3(4,bishopMoveDirections_B3);

static const int bishopMoves_C3_0[3] = { 2,B2,A1 };
static const int bishopMoves_C3_1[3] = { 2,D2,E1 };
static const int bishopMoves_C3_2[3] = { 2,B4,A5 };
static const int bishopMoves_C3_3[6] = { 5,D4,E5,F6,G7,H8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_C3[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_C3_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_C3_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_C3_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_C3_3)
};
#else
static const Direction bishopMoveDirections_C3[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_C3_0,_T("bishop MD_DOWNDIAG1 from C3:B2 A1"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_C3_1,_T("bishop MD_DOWNDIAG2 from C3:D2 E1"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_C3_2,_T("bishop MD_UPDIAG2 from C3:B4 A5"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_C3_3,_T("bishop MD_UPDIAG1 from C3:D4 E5 F6 G7 H8"))
};
#endif
static const DirectionArray bishopMoves_C3(4,bishopMoveDirections_C3);

static const int bishopMoves_D3_0[3] = { 2,C2,B1 };
static const int bishopMoves_D3_1[3] = { 2,E2,F1 };
static const int bishopMoves_D3_2[4] = { 3,C4,B5,A6 };
static const int bishopMoves_D3_3[5] = { 4,E4,F5,G6,H7 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_D3[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_D3_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_D3_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_D3_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_D3_3)
};
#else
static const Direction bishopMoveDirections_D3[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_D3_0,_T("bishop MD_DOWNDIAG1 from D3:C2 B1"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_D3_1,_T("bishop MD_DOWNDIAG2 from D3:E2 F1"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_D3_2,_T("bishop MD_UPDIAG2 from D3:C4 B5 A6"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_D3_3,_T("bishop MD_UPDIAG1 from D3:E4 F5 G6 H7"))
};
#endif
static const DirectionArray bishopMoves_D3(4,bishopMoveDirections_D3);

static const int bishopMoves_E3_0[3] = { 2,D2,C1 };
static const int bishopMoves_E3_1[3] = { 2,F2,G1 };
static const int bishopMoves_E3_2[5] = { 4,D4,C5,B6,A7 };
static const int bishopMoves_E3_3[4] = { 3,F4,G5,H6 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_E3[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_E3_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_E3_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_E3_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_E3_3)
};
#else
static const Direction bishopMoveDirections_E3[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_E3_0,_T("bishop MD_DOWNDIAG1 from E3:D2 C1"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_E3_1,_T("bishop MD_DOWNDIAG2 from E3:F2 G1"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_E3_2,_T("bishop MD_UPDIAG2 from E3:D4 C5 B6 A7"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_E3_3,_T("bishop MD_UPDIAG1 from E3:F4 G5 H6"))
};
#endif
static const DirectionArray bishopMoves_E3(4,bishopMoveDirections_E3);

static const int bishopMoves_F3_0[3] = { 2,E2,D1 };
static const int bishopMoves_F3_1[3] = { 2,G2,H1 };
static const int bishopMoves_F3_2[6] = { 5,E4,D5,C6,B7,A8 };
static const int bishopMoves_F3_3[3] = { 2,G4,H5 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_F3[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_F3_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_F3_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_F3_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_F3_3)
};
#else
static const Direction bishopMoveDirections_F3[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_F3_0,_T("bishop MD_DOWNDIAG1 from F3:E2 D1"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_F3_1,_T("bishop MD_DOWNDIAG2 from F3:G2 H1"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_F3_2,_T("bishop MD_UPDIAG2 from F3:E4 D5 C6 B7 A8"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_F3_3,_T("bishop MD_UPDIAG1 from F3:G4 H5"))
};
#endif
static const DirectionArray bishopMoves_F3(4,bishopMoveDirections_F3);

static const int bishopMoves_G3_0[3] = { 2,F2,E1 };
static const int bishopMoves_G3_1[2] = { 1,H2 };
static const int bishopMoves_G3_2[6] = { 5,F4,E5,D6,C7,B8 };
static const int bishopMoves_G3_3[2] = { 1,H4 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_G3[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_G3_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_G3_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_G3_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_G3_3)
};
#else
static const Direction bishopMoveDirections_G3[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_G3_0,_T("bishop MD_DOWNDIAG1 from G3:F2 E1"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_G3_1,_T("bishop MD_DOWNDIAG2 from G3:H2"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_G3_2,_T("bishop MD_UPDIAG2 from G3:F4 E5 D6 C7 B8"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_G3_3,_T("bishop MD_UPDIAG1 from G3:H4"))
};
#endif
static const DirectionArray bishopMoves_G3(4,bishopMoveDirections_G3);

static const int bishopMoves_H3_0[3] = { 2,G2,F1 };
static const int bishopMoves_H3_1[6] = { 5,G4,F5,E6,D7,C8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_H3[2] = {
  Direction(MD_DOWNDIAG1,bishopMoves_H3_0)
 ,Direction(MD_UPDIAG2  ,bishopMoves_H3_1)
};
#else
static const Direction bishopMoveDirections_H3[2] = {
  Direction(MD_DOWNDIAG1,bishopMoves_H3_0,_T("bishop MD_DOWNDIAG1 from H3:G2 F1"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_H3_1,_T("bishop MD_UPDIAG2 from H3:G4 F5 E6 D7 C8"))
};
#endif
static const DirectionArray bishopMoves_H3(2,bishopMoveDirections_H3);

static const int bishopMoves_A4_0[4] = { 3,B3,C2,D1 };
static const int bishopMoves_A4_1[5] = { 4,B5,C6,D7,E8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_A4[2] = {
  Direction(MD_DOWNDIAG2,bishopMoves_A4_0)
 ,Direction(MD_UPDIAG1  ,bishopMoves_A4_1)
};
#else
static const Direction bishopMoveDirections_A4[2] = {
  Direction(MD_DOWNDIAG2,bishopMoves_A4_0,_T("bishop MD_DOWNDIAG2 from A4:B3 C2 D1"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_A4_1,_T("bishop MD_UPDIAG1 from A4:B5 C6 D7 E8"))
};
#endif
static const DirectionArray bishopMoves_A4(2,bishopMoveDirections_A4);

static const int bishopMoves_B4_0[2] = { 1,A3 };
static const int bishopMoves_B4_1[4] = { 3,C3,D2,E1 };
static const int bishopMoves_B4_2[2] = { 1,A5 };
static const int bishopMoves_B4_3[5] = { 4,C5,D6,E7,F8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_B4[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_B4_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_B4_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_B4_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_B4_3)
};
#else
static const Direction bishopMoveDirections_B4[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_B4_0,_T("bishop MD_DOWNDIAG1 from B4:A3"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_B4_1,_T("bishop MD_DOWNDIAG2 from B4:C3 D2 E1"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_B4_2,_T("bishop MD_UPDIAG2 from B4:A5"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_B4_3,_T("bishop MD_UPDIAG1 from B4:C5 D6 E7 F8"))
};
#endif
static const DirectionArray bishopMoves_B4(4,bishopMoveDirections_B4);

static const int bishopMoves_C4_0[3] = { 2,B3,A2 };
static const int bishopMoves_C4_1[4] = { 3,D3,E2,F1 };
static const int bishopMoves_C4_2[3] = { 2,B5,A6 };
static const int bishopMoves_C4_3[5] = { 4,D5,E6,F7,G8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_C4[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_C4_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_C4_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_C4_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_C4_3)
};
#else
static const Direction bishopMoveDirections_C4[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_C4_0,_T("bishop MD_DOWNDIAG1 from C4:B3 A2"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_C4_1,_T("bishop MD_DOWNDIAG2 from C4:D3 E2 F1"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_C4_2,_T("bishop MD_UPDIAG2 from C4:B5 A6"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_C4_3,_T("bishop MD_UPDIAG1 from C4:D5 E6 F7 G8"))
};
#endif
static const DirectionArray bishopMoves_C4(4,bishopMoveDirections_C4);

static const int bishopMoves_D4_0[4] = { 3,C3,B2,A1 };
static const int bishopMoves_D4_1[4] = { 3,E3,F2,G1 };
static const int bishopMoves_D4_2[4] = { 3,C5,B6,A7 };
static const int bishopMoves_D4_3[5] = { 4,E5,F6,G7,H8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_D4[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_D4_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_D4_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_D4_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_D4_3)
};
#else
static const Direction bishopMoveDirections_D4[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_D4_0,_T("bishop MD_DOWNDIAG1 from D4:C3 B2 A1"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_D4_1,_T("bishop MD_DOWNDIAG2 from D4:E3 F2 G1"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_D4_2,_T("bishop MD_UPDIAG2 from D4:C5 B6 A7"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_D4_3,_T("bishop MD_UPDIAG1 from D4:E5 F6 G7 H8"))
};
#endif
static const DirectionArray bishopMoves_D4(4,bishopMoveDirections_D4);

static const int bishopMoves_E4_0[4] = { 3,D3,C2,B1 };
static const int bishopMoves_E4_1[4] = { 3,F3,G2,H1 };
static const int bishopMoves_E4_2[5] = { 4,D5,C6,B7,A8 };
static const int bishopMoves_E4_3[4] = { 3,F5,G6,H7 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_E4[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_E4_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_E4_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_E4_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_E4_3)
};
#else
static const Direction bishopMoveDirections_E4[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_E4_0,_T("bishop MD_DOWNDIAG1 from E4:D3 C2 B1"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_E4_1,_T("bishop MD_DOWNDIAG2 from E4:F3 G2 H1"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_E4_2,_T("bishop MD_UPDIAG2 from E4:D5 C6 B7 A8"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_E4_3,_T("bishop MD_UPDIAG1 from E4:F5 G6 H7"))
};
#endif
static const DirectionArray bishopMoves_E4(4,bishopMoveDirections_E4);

static const int bishopMoves_F4_0[4] = { 3,E3,D2,C1 };
static const int bishopMoves_F4_1[3] = { 2,G3,H2 };
static const int bishopMoves_F4_2[5] = { 4,E5,D6,C7,B8 };
static const int bishopMoves_F4_3[3] = { 2,G5,H6 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_F4[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_F4_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_F4_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_F4_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_F4_3)
};
#else
static const Direction bishopMoveDirections_F4[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_F4_0,_T("bishop MD_DOWNDIAG1 from F4:E3 D2 C1"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_F4_1,_T("bishop MD_DOWNDIAG2 from F4:G3 H2"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_F4_2,_T("bishop MD_UPDIAG2 from F4:E5 D6 C7 B8"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_F4_3,_T("bishop MD_UPDIAG1 from F4:G5 H6"))
};
#endif
static const DirectionArray bishopMoves_F4(4,bishopMoveDirections_F4);

static const int bishopMoves_G4_0[4] = { 3,F3,E2,D1 };
static const int bishopMoves_G4_1[2] = { 1,H3 };
static const int bishopMoves_G4_2[5] = { 4,F5,E6,D7,C8 };
static const int bishopMoves_G4_3[2] = { 1,H5 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_G4[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_G4_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_G4_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_G4_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_G4_3)
};
#else
static const Direction bishopMoveDirections_G4[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_G4_0,_T("bishop MD_DOWNDIAG1 from G4:F3 E2 D1"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_G4_1,_T("bishop MD_DOWNDIAG2 from G4:H3"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_G4_2,_T("bishop MD_UPDIAG2 from G4:F5 E6 D7 C8"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_G4_3,_T("bishop MD_UPDIAG1 from G4:H5"))
};
#endif
static const DirectionArray bishopMoves_G4(4,bishopMoveDirections_G4);

static const int bishopMoves_H4_0[4] = { 3,G3,F2,E1 };
static const int bishopMoves_H4_1[5] = { 4,G5,F6,E7,D8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_H4[2] = {
  Direction(MD_DOWNDIAG1,bishopMoves_H4_0)
 ,Direction(MD_UPDIAG2  ,bishopMoves_H4_1)
};
#else
static const Direction bishopMoveDirections_H4[2] = {
  Direction(MD_DOWNDIAG1,bishopMoves_H4_0,_T("bishop MD_DOWNDIAG1 from H4:G3 F2 E1"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_H4_1,_T("bishop MD_UPDIAG2 from H4:G5 F6 E7 D8"))
};
#endif
static const DirectionArray bishopMoves_H4(2,bishopMoveDirections_H4);

static const int bishopMoves_A5_0[5] = { 4,B4,C3,D2,E1 };
static const int bishopMoves_A5_1[4] = { 3,B6,C7,D8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_A5[2] = {
  Direction(MD_DOWNDIAG2,bishopMoves_A5_0)
 ,Direction(MD_UPDIAG1  ,bishopMoves_A5_1)
};
#else
static const Direction bishopMoveDirections_A5[2] = {
  Direction(MD_DOWNDIAG2,bishopMoves_A5_0,_T("bishop MD_DOWNDIAG2 from A5:B4 C3 D2 E1"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_A5_1,_T("bishop MD_UPDIAG1 from A5:B6 C7 D8"))
};
#endif
static const DirectionArray bishopMoves_A5(2,bishopMoveDirections_A5);

static const int bishopMoves_B5_0[2] = { 1,A4 };
static const int bishopMoves_B5_1[5] = { 4,C4,D3,E2,F1 };
static const int bishopMoves_B5_2[2] = { 1,A6 };
static const int bishopMoves_B5_3[4] = { 3,C6,D7,E8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_B5[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_B5_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_B5_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_B5_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_B5_3)
};
#else
static const Direction bishopMoveDirections_B5[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_B5_0,_T("bishop MD_DOWNDIAG1 from B5:A4"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_B5_1,_T("bishop MD_DOWNDIAG2 from B5:C4 D3 E2 F1"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_B5_2,_T("bishop MD_UPDIAG2 from B5:A6"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_B5_3,_T("bishop MD_UPDIAG1 from B5:C6 D7 E8"))
};
#endif
static const DirectionArray bishopMoves_B5(4,bishopMoveDirections_B5);

static const int bishopMoves_C5_0[3] = { 2,B4,A3 };
static const int bishopMoves_C5_1[5] = { 4,D4,E3,F2,G1 };
static const int bishopMoves_C5_2[3] = { 2,B6,A7 };
static const int bishopMoves_C5_3[4] = { 3,D6,E7,F8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_C5[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_C5_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_C5_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_C5_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_C5_3)
};
#else
static const Direction bishopMoveDirections_C5[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_C5_0,_T("bishop MD_DOWNDIAG1 from C5:B4 A3"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_C5_1,_T("bishop MD_DOWNDIAG2 from C5:D4 E3 F2 G1"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_C5_2,_T("bishop MD_UPDIAG2 from C5:B6 A7"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_C5_3,_T("bishop MD_UPDIAG1 from C5:D6 E7 F8"))
};
#endif
static const DirectionArray bishopMoves_C5(4,bishopMoveDirections_C5);

static const int bishopMoves_D5_0[4] = { 3,C4,B3,A2 };
static const int bishopMoves_D5_1[5] = { 4,E4,F3,G2,H1 };
static const int bishopMoves_D5_2[4] = { 3,C6,B7,A8 };
static const int bishopMoves_D5_3[4] = { 3,E6,F7,G8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_D5[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_D5_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_D5_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_D5_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_D5_3)
};
#else
static const Direction bishopMoveDirections_D5[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_D5_0,_T("bishop MD_DOWNDIAG1 from D5:C4 B3 A2"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_D5_1,_T("bishop MD_DOWNDIAG2 from D5:E4 F3 G2 H1"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_D5_2,_T("bishop MD_UPDIAG2 from D5:C6 B7 A8"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_D5_3,_T("bishop MD_UPDIAG1 from D5:E6 F7 G8"))
};
#endif
static const DirectionArray bishopMoves_D5(4,bishopMoveDirections_D5);

static const int bishopMoves_E5_0[5] = { 4,D4,C3,B2,A1 };
static const int bishopMoves_E5_1[4] = { 3,F4,G3,H2 };
static const int bishopMoves_E5_2[4] = { 3,D6,C7,B8 };
static const int bishopMoves_E5_3[4] = { 3,F6,G7,H8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_E5[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_E5_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_E5_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_E5_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_E5_3)
};
#else
static const Direction bishopMoveDirections_E5[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_E5_0,_T("bishop MD_DOWNDIAG1 from E5:D4 C3 B2 A1"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_E5_1,_T("bishop MD_DOWNDIAG2 from E5:F4 G3 H2"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_E5_2,_T("bishop MD_UPDIAG2 from E5:D6 C7 B8"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_E5_3,_T("bishop MD_UPDIAG1 from E5:F6 G7 H8"))
};
#endif
static const DirectionArray bishopMoves_E5(4,bishopMoveDirections_E5);

static const int bishopMoves_F5_0[5] = { 4,E4,D3,C2,B1 };
static const int bishopMoves_F5_1[3] = { 2,G4,H3 };
static const int bishopMoves_F5_2[4] = { 3,E6,D7,C8 };
static const int bishopMoves_F5_3[3] = { 2,G6,H7 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_F5[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_F5_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_F5_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_F5_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_F5_3)
};
#else
static const Direction bishopMoveDirections_F5[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_F5_0,_T("bishop MD_DOWNDIAG1 from F5:E4 D3 C2 B1"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_F5_1,_T("bishop MD_DOWNDIAG2 from F5:G4 H3"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_F5_2,_T("bishop MD_UPDIAG2 from F5:E6 D7 C8"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_F5_3,_T("bishop MD_UPDIAG1 from F5:G6 H7"))
};
#endif
static const DirectionArray bishopMoves_F5(4,bishopMoveDirections_F5);

static const int bishopMoves_G5_0[5] = { 4,F4,E3,D2,C1 };
static const int bishopMoves_G5_1[2] = { 1,H4 };
static const int bishopMoves_G5_2[4] = { 3,F6,E7,D8 };
static const int bishopMoves_G5_3[2] = { 1,H6 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_G5[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_G5_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_G5_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_G5_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_G5_3)
};
#else
static const Direction bishopMoveDirections_G5[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_G5_0,_T("bishop MD_DOWNDIAG1 from G5:F4 E3 D2 C1"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_G5_1,_T("bishop MD_DOWNDIAG2 from G5:H4"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_G5_2,_T("bishop MD_UPDIAG2 from G5:F6 E7 D8"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_G5_3,_T("bishop MD_UPDIAG1 from G5:H6"))
};
#endif
static const DirectionArray bishopMoves_G5(4,bishopMoveDirections_G5);

static const int bishopMoves_H5_0[5] = { 4,G4,F3,E2,D1 };
static const int bishopMoves_H5_1[4] = { 3,G6,F7,E8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_H5[2] = {
  Direction(MD_DOWNDIAG1,bishopMoves_H5_0)
 ,Direction(MD_UPDIAG2  ,bishopMoves_H5_1)
};
#else
static const Direction bishopMoveDirections_H5[2] = {
  Direction(MD_DOWNDIAG1,bishopMoves_H5_0,_T("bishop MD_DOWNDIAG1 from H5:G4 F3 E2 D1"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_H5_1,_T("bishop MD_UPDIAG2 from H5:G6 F7 E8"))
};
#endif
static const DirectionArray bishopMoves_H5(2,bishopMoveDirections_H5);

static const int bishopMoves_A6_0[6] = { 5,B5,C4,D3,E2,F1 };
static const int bishopMoves_A6_1[3] = { 2,B7,C8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_A6[2] = {
  Direction(MD_DOWNDIAG2,bishopMoves_A6_0)
 ,Direction(MD_UPDIAG1  ,bishopMoves_A6_1)
};
#else
static const Direction bishopMoveDirections_A6[2] = {
  Direction(MD_DOWNDIAG2,bishopMoves_A6_0,_T("bishop MD_DOWNDIAG2 from A6:B5 C4 D3 E2 F1"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_A6_1,_T("bishop MD_UPDIAG1 from A6:B7 C8"))
};
#endif
static const DirectionArray bishopMoves_A6(2,bishopMoveDirections_A6);

static const int bishopMoves_B6_0[2] = { 1,A5 };
static const int bishopMoves_B6_1[6] = { 5,C5,D4,E3,F2,G1 };
static const int bishopMoves_B6_2[2] = { 1,A7 };
static const int bishopMoves_B6_3[3] = { 2,C7,D8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_B6[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_B6_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_B6_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_B6_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_B6_3)
};
#else
static const Direction bishopMoveDirections_B6[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_B6_0,_T("bishop MD_DOWNDIAG1 from B6:A5"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_B6_1,_T("bishop MD_DOWNDIAG2 from B6:C5 D4 E3 F2 G1"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_B6_2,_T("bishop MD_UPDIAG2 from B6:A7"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_B6_3,_T("bishop MD_UPDIAG1 from B6:C7 D8"))
};
#endif
static const DirectionArray bishopMoves_B6(4,bishopMoveDirections_B6);

static const int bishopMoves_C6_0[3] = { 2,B5,A4 };
static const int bishopMoves_C6_1[6] = { 5,D5,E4,F3,G2,H1 };
static const int bishopMoves_C6_2[3] = { 2,B7,A8 };
static const int bishopMoves_C6_3[3] = { 2,D7,E8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_C6[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_C6_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_C6_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_C6_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_C6_3)
};
#else
static const Direction bishopMoveDirections_C6[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_C6_0,_T("bishop MD_DOWNDIAG1 from C6:B5 A4"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_C6_1,_T("bishop MD_DOWNDIAG2 from C6:D5 E4 F3 G2 H1"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_C6_2,_T("bishop MD_UPDIAG2 from C6:B7 A8"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_C6_3,_T("bishop MD_UPDIAG1 from C6:D7 E8"))
};
#endif
static const DirectionArray bishopMoves_C6(4,bishopMoveDirections_C6);

static const int bishopMoves_D6_0[4] = { 3,C5,B4,A3 };
static const int bishopMoves_D6_1[5] = { 4,E5,F4,G3,H2 };
static const int bishopMoves_D6_2[3] = { 2,C7,B8 };
static const int bishopMoves_D6_3[3] = { 2,E7,F8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_D6[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_D6_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_D6_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_D6_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_D6_3)
};
#else
static const Direction bishopMoveDirections_D6[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_D6_0,_T("bishop MD_DOWNDIAG1 from D6:C5 B4 A3"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_D6_1,_T("bishop MD_DOWNDIAG2 from D6:E5 F4 G3 H2"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_D6_2,_T("bishop MD_UPDIAG2 from D6:C7 B8"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_D6_3,_T("bishop MD_UPDIAG1 from D6:E7 F8"))
};
#endif
static const DirectionArray bishopMoves_D6(4,bishopMoveDirections_D6);

static const int bishopMoves_E6_0[5] = { 4,D5,C4,B3,A2 };
static const int bishopMoves_E6_1[4] = { 3,F5,G4,H3 };
static const int bishopMoves_E6_2[3] = { 2,D7,C8 };
static const int bishopMoves_E6_3[3] = { 2,F7,G8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_E6[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_E6_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_E6_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_E6_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_E6_3)
};
#else
static const Direction bishopMoveDirections_E6[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_E6_0,_T("bishop MD_DOWNDIAG1 from E6:D5 C4 B3 A2"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_E6_1,_T("bishop MD_DOWNDIAG2 from E6:F5 G4 H3"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_E6_2,_T("bishop MD_UPDIAG2 from E6:D7 C8"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_E6_3,_T("bishop MD_UPDIAG1 from E6:F7 G8"))
};
#endif
static const DirectionArray bishopMoves_E6(4,bishopMoveDirections_E6);

static const int bishopMoves_F6_0[6] = { 5,E5,D4,C3,B2,A1 };
static const int bishopMoves_F6_1[3] = { 2,G5,H4 };
static const int bishopMoves_F6_2[3] = { 2,E7,D8 };
static const int bishopMoves_F6_3[3] = { 2,G7,H8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_F6[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_F6_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_F6_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_F6_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_F6_3)
};
#else
static const Direction bishopMoveDirections_F6[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_F6_0,_T("bishop MD_DOWNDIAG1 from F6:E5 D4 C3 B2 A1"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_F6_1,_T("bishop MD_DOWNDIAG2 from F6:G5 H4"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_F6_2,_T("bishop MD_UPDIAG2 from F6:E7 D8"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_F6_3,_T("bishop MD_UPDIAG1 from F6:G7 H8"))
};
#endif
static const DirectionArray bishopMoves_F6(4,bishopMoveDirections_F6);

static const int bishopMoves_G6_0[6] = { 5,F5,E4,D3,C2,B1 };
static const int bishopMoves_G6_1[2] = { 1,H5 };
static const int bishopMoves_G6_2[3] = { 2,F7,E8 };
static const int bishopMoves_G6_3[2] = { 1,H7 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_G6[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_G6_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_G6_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_G6_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_G6_3)
};
#else
static const Direction bishopMoveDirections_G6[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_G6_0,_T("bishop MD_DOWNDIAG1 from G6:F5 E4 D3 C2 B1"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_G6_1,_T("bishop MD_DOWNDIAG2 from G6:H5"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_G6_2,_T("bishop MD_UPDIAG2 from G6:F7 E8"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_G6_3,_T("bishop MD_UPDIAG1 from G6:H7"))
};
#endif
static const DirectionArray bishopMoves_G6(4,bishopMoveDirections_G6);

static const int bishopMoves_H6_0[6] = { 5,G5,F4,E3,D2,C1 };
static const int bishopMoves_H6_1[3] = { 2,G7,F8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_H6[2] = {
  Direction(MD_DOWNDIAG1,bishopMoves_H6_0)
 ,Direction(MD_UPDIAG2  ,bishopMoves_H6_1)
};
#else
static const Direction bishopMoveDirections_H6[2] = {
  Direction(MD_DOWNDIAG1,bishopMoves_H6_0,_T("bishop MD_DOWNDIAG1 from H6:G5 F4 E3 D2 C1"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_H6_1,_T("bishop MD_UPDIAG2 from H6:G7 F8"))
};
#endif
static const DirectionArray bishopMoves_H6(2,bishopMoveDirections_H6);

static const int bishopMoves_A7_0[7] = { 6,B6,C5,D4,E3,F2,G1 };
static const int bishopMoves_A7_1[2] = { 1,B8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_A7[2] = {
  Direction(MD_DOWNDIAG2,bishopMoves_A7_0)
 ,Direction(MD_UPDIAG1  ,bishopMoves_A7_1)
};
#else
static const Direction bishopMoveDirections_A7[2] = {
  Direction(MD_DOWNDIAG2,bishopMoves_A7_0,_T("bishop MD_DOWNDIAG2 from A7:B6 C5 D4 E3 F2 G1"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_A7_1,_T("bishop MD_UPDIAG1 from A7:B8"))
};
#endif
static const DirectionArray bishopMoves_A7(2,bishopMoveDirections_A7);

static const int bishopMoves_B7_0[2] = { 1,A6 };
static const int bishopMoves_B7_1[7] = { 6,C6,D5,E4,F3,G2,H1 };
static const int bishopMoves_B7_2[2] = { 1,A8 };
static const int bishopMoves_B7_3[2] = { 1,C8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_B7[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_B7_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_B7_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_B7_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_B7_3)
};
#else
static const Direction bishopMoveDirections_B7[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_B7_0,_T("bishop MD_DOWNDIAG1 from B7:A6"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_B7_1,_T("bishop MD_DOWNDIAG2 from B7:C6 D5 E4 F3 G2 H1"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_B7_2,_T("bishop MD_UPDIAG2 from B7:A8"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_B7_3,_T("bishop MD_UPDIAG1 from B7:C8"))
};
#endif
static const DirectionArray bishopMoves_B7(4,bishopMoveDirections_B7);

static const int bishopMoves_C7_0[3] = { 2,B6,A5 };
static const int bishopMoves_C7_1[6] = { 5,D6,E5,F4,G3,H2 };
static const int bishopMoves_C7_2[2] = { 1,B8 };
static const int bishopMoves_C7_3[2] = { 1,D8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_C7[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_C7_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_C7_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_C7_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_C7_3)
};
#else
static const Direction bishopMoveDirections_C7[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_C7_0,_T("bishop MD_DOWNDIAG1 from C7:B6 A5"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_C7_1,_T("bishop MD_DOWNDIAG2 from C7:D6 E5 F4 G3 H2"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_C7_2,_T("bishop MD_UPDIAG2 from C7:B8"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_C7_3,_T("bishop MD_UPDIAG1 from C7:D8"))
};
#endif
static const DirectionArray bishopMoves_C7(4,bishopMoveDirections_C7);

static const int bishopMoves_D7_0[4] = { 3,C6,B5,A4 };
static const int bishopMoves_D7_1[5] = { 4,E6,F5,G4,H3 };
static const int bishopMoves_D7_2[2] = { 1,C8 };
static const int bishopMoves_D7_3[2] = { 1,E8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_D7[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_D7_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_D7_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_D7_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_D7_3)
};
#else
static const Direction bishopMoveDirections_D7[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_D7_0,_T("bishop MD_DOWNDIAG1 from D7:C6 B5 A4"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_D7_1,_T("bishop MD_DOWNDIAG2 from D7:E6 F5 G4 H3"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_D7_2,_T("bishop MD_UPDIAG2 from D7:C8"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_D7_3,_T("bishop MD_UPDIAG1 from D7:E8"))
};
#endif
static const DirectionArray bishopMoves_D7(4,bishopMoveDirections_D7);

static const int bishopMoves_E7_0[5] = { 4,D6,C5,B4,A3 };
static const int bishopMoves_E7_1[4] = { 3,F6,G5,H4 };
static const int bishopMoves_E7_2[2] = { 1,D8 };
static const int bishopMoves_E7_3[2] = { 1,F8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_E7[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_E7_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_E7_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_E7_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_E7_3)
};
#else
static const Direction bishopMoveDirections_E7[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_E7_0,_T("bishop MD_DOWNDIAG1 from E7:D6 C5 B4 A3"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_E7_1,_T("bishop MD_DOWNDIAG2 from E7:F6 G5 H4"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_E7_2,_T("bishop MD_UPDIAG2 from E7:D8"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_E7_3,_T("bishop MD_UPDIAG1 from E7:F8"))
};
#endif
static const DirectionArray bishopMoves_E7(4,bishopMoveDirections_E7);

static const int bishopMoves_F7_0[6] = { 5,E6,D5,C4,B3,A2 };
static const int bishopMoves_F7_1[3] = { 2,G6,H5 };
static const int bishopMoves_F7_2[2] = { 1,E8 };
static const int bishopMoves_F7_3[2] = { 1,G8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_F7[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_F7_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_F7_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_F7_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_F7_3)
};
#else
static const Direction bishopMoveDirections_F7[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_F7_0,_T("bishop MD_DOWNDIAG1 from F7:E6 D5 C4 B3 A2"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_F7_1,_T("bishop MD_DOWNDIAG2 from F7:G6 H5"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_F7_2,_T("bishop MD_UPDIAG2 from F7:E8"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_F7_3,_T("bishop MD_UPDIAG1 from F7:G8"))
};
#endif
static const DirectionArray bishopMoves_F7(4,bishopMoveDirections_F7);

static const int bishopMoves_G7_0[7] = { 6,F6,E5,D4,C3,B2,A1 };
static const int bishopMoves_G7_1[2] = { 1,H6 };
static const int bishopMoves_G7_2[2] = { 1,F8 };
static const int bishopMoves_G7_3[2] = { 1,H8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_G7[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_G7_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_G7_1)
 ,Direction(MD_UPDIAG2  ,bishopMoves_G7_2)
 ,Direction(MD_UPDIAG1  ,bishopMoves_G7_3)
};
#else
static const Direction bishopMoveDirections_G7[4] = {
  Direction(MD_DOWNDIAG1,bishopMoves_G7_0,_T("bishop MD_DOWNDIAG1 from G7:F6 E5 D4 C3 B2 A1"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_G7_1,_T("bishop MD_DOWNDIAG2 from G7:H6"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_G7_2,_T("bishop MD_UPDIAG2 from G7:F8"))
 ,Direction(MD_UPDIAG1  ,bishopMoves_G7_3,_T("bishop MD_UPDIAG1 from G7:H8"))
};
#endif
static const DirectionArray bishopMoves_G7(4,bishopMoveDirections_G7);

static const int bishopMoves_H7_0[7] = { 6,G6,F5,E4,D3,C2,B1 };
static const int bishopMoves_H7_1[2] = { 1,G8 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_H7[2] = {
  Direction(MD_DOWNDIAG1,bishopMoves_H7_0)
 ,Direction(MD_UPDIAG2  ,bishopMoves_H7_1)
};
#else
static const Direction bishopMoveDirections_H7[2] = {
  Direction(MD_DOWNDIAG1,bishopMoves_H7_0,_T("bishop MD_DOWNDIAG1 from H7:G6 F5 E4 D3 C2 B1"))
 ,Direction(MD_UPDIAG2  ,bishopMoves_H7_1,_T("bishop MD_UPDIAG2 from H7:G8"))
};
#endif
static const DirectionArray bishopMoves_H7(2,bishopMoveDirections_H7);

static const int bishopMoves_A8_0[8] = { 7,B7,C6,D5,E4,F3,G2,H1 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_A8[1] = {
  Direction(MD_DOWNDIAG2,bishopMoves_A8_0)
};
#else
static const Direction bishopMoveDirections_A8[1] = {
  Direction(MD_DOWNDIAG2,bishopMoves_A8_0,_T("bishop MD_DOWNDIAG2 from A8:B7 C6 D5 E4 F3 G2 H1"))
};
#endif
static const DirectionArray bishopMoves_A8(1,bishopMoveDirections_A8);

static const int bishopMoves_B8_0[2] = { 1,A7 };
static const int bishopMoves_B8_1[7] = { 6,C7,D6,E5,F4,G3,H2 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_B8[2] = {
  Direction(MD_DOWNDIAG1,bishopMoves_B8_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_B8_1)
};
#else
static const Direction bishopMoveDirections_B8[2] = {
  Direction(MD_DOWNDIAG1,bishopMoves_B8_0,_T("bishop MD_DOWNDIAG1 from B8:A7"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_B8_1,_T("bishop MD_DOWNDIAG2 from B8:C7 D6 E5 F4 G3 H2"))
};
#endif
static const DirectionArray bishopMoves_B8(2,bishopMoveDirections_B8);

static const int bishopMoves_C8_0[3] = { 2,B7,A6 };
static const int bishopMoves_C8_1[6] = { 5,D7,E6,F5,G4,H3 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_C8[2] = {
  Direction(MD_DOWNDIAG1,bishopMoves_C8_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_C8_1)
};
#else
static const Direction bishopMoveDirections_C8[2] = {
  Direction(MD_DOWNDIAG1,bishopMoves_C8_0,_T("bishop MD_DOWNDIAG1 from C8:B7 A6"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_C8_1,_T("bishop MD_DOWNDIAG2 from C8:D7 E6 F5 G4 H3"))
};
#endif
static const DirectionArray bishopMoves_C8(2,bishopMoveDirections_C8);

static const int bishopMoves_D8_0[4] = { 3,C7,B6,A5 };
static const int bishopMoves_D8_1[5] = { 4,E7,F6,G5,H4 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_D8[2] = {
  Direction(MD_DOWNDIAG1,bishopMoves_D8_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_D8_1)
};
#else
static const Direction bishopMoveDirections_D8[2] = {
  Direction(MD_DOWNDIAG1,bishopMoves_D8_0,_T("bishop MD_DOWNDIAG1 from D8:C7 B6 A5"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_D8_1,_T("bishop MD_DOWNDIAG2 from D8:E7 F6 G5 H4"))
};
#endif
static const DirectionArray bishopMoves_D8(2,bishopMoveDirections_D8);

static const int bishopMoves_E8_0[5] = { 4,D7,C6,B5,A4 };
static const int bishopMoves_E8_1[4] = { 3,F7,G6,H5 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_E8[2] = {
  Direction(MD_DOWNDIAG1,bishopMoves_E8_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_E8_1)
};
#else
static const Direction bishopMoveDirections_E8[2] = {
  Direction(MD_DOWNDIAG1,bishopMoves_E8_0,_T("bishop MD_DOWNDIAG1 from E8:D7 C6 B5 A4"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_E8_1,_T("bishop MD_DOWNDIAG2 from E8:F7 G6 H5"))
};
#endif
static const DirectionArray bishopMoves_E8(2,bishopMoveDirections_E8);

static const int bishopMoves_F8_0[6] = { 5,E7,D6,C5,B4,A3 };
static const int bishopMoves_F8_1[3] = { 2,G7,H6 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_F8[2] = {
  Direction(MD_DOWNDIAG1,bishopMoves_F8_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_F8_1)
};
#else
static const Direction bishopMoveDirections_F8[2] = {
  Direction(MD_DOWNDIAG1,bishopMoves_F8_0,_T("bishop MD_DOWNDIAG1 from F8:E7 D6 C5 B4 A3"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_F8_1,_T("bishop MD_DOWNDIAG2 from F8:G7 H6"))
};
#endif
static const DirectionArray bishopMoves_F8(2,bishopMoveDirections_F8);

static const int bishopMoves_G8_0[7] = { 6,F7,E6,D5,C4,B3,A2 };
static const int bishopMoves_G8_1[2] = { 1,H7 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_G8[2] = {
  Direction(MD_DOWNDIAG1,bishopMoves_G8_0)
 ,Direction(MD_DOWNDIAG2,bishopMoves_G8_1)
};
#else
static const Direction bishopMoveDirections_G8[2] = {
  Direction(MD_DOWNDIAG1,bishopMoves_G8_0,_T("bishop MD_DOWNDIAG1 from G8:F7 E6 D5 C4 B3 A2"))
 ,Direction(MD_DOWNDIAG2,bishopMoves_G8_1,_T("bishop MD_DOWNDIAG2 from G8:H7"))
};
#endif
static const DirectionArray bishopMoves_G8(2,bishopMoveDirections_G8);

static const int bishopMoves_H8_0[8] = { 7,G7,F6,E5,D4,C3,B2,A1 };
#ifndef _DEBUG
static const Direction bishopMoveDirections_H8[1] = {
  Direction(MD_DOWNDIAG1,bishopMoves_H8_0)
};
#else
static const Direction bishopMoveDirections_H8[1] = {
  Direction(MD_DOWNDIAG1,bishopMoves_H8_0,_T("bishop MD_DOWNDIAG1 from H8:G7 F6 E5 D4 C3 B2 A1"))
};
#endif
static const DirectionArray bishopMoves_H8(1,bishopMoveDirections_H8);

static const int knightMoves_A1_0[2] = { 1,B3 };
static const int knightMoves_A1_1[2] = { 1,C2 };
#ifndef _DEBUG
static const Direction knightMoveDirections_A1[2] = {
  Direction(MD_NONE     ,knightMoves_A1_0)
 ,Direction(MD_NONE     ,knightMoves_A1_1)
};
#else
static const Direction knightMoveDirections_A1[2] = {
  Direction(MD_NONE     ,knightMoves_A1_0,_T("knight MD_NONE from A1:B3"))
 ,Direction(MD_NONE     ,knightMoves_A1_1,_T("knight MD_NONE from A1:C2"))
};
#endif
static const DirectionArray knightMoves_A1(2,knightMoveDirections_A1);

static const int knightMoves_B1_0[2] = { 1,C3 };
static const int knightMoves_B1_1[2] = { 1,D2 };
static const int knightMoves_B1_2[2] = { 1,A3 };
#ifndef _DEBUG
static const Direction knightMoveDirections_B1[3] = {
  Direction(MD_NONE     ,knightMoves_B1_0)
 ,Direction(MD_NONE     ,knightMoves_B1_1)
 ,Direction(MD_NONE     ,knightMoves_B1_2)
};
#else
static const Direction knightMoveDirections_B1[3] = {
  Direction(MD_NONE     ,knightMoves_B1_0,_T("knight MD_NONE from B1:C3"))
 ,Direction(MD_NONE     ,knightMoves_B1_1,_T("knight MD_NONE from B1:D2"))
 ,Direction(MD_NONE     ,knightMoves_B1_2,_T("knight MD_NONE from B1:A3"))
};
#endif
static const DirectionArray knightMoves_B1(3,knightMoveDirections_B1);

static const int knightMoves_C1_0[2] = { 1,D3 };
static const int knightMoves_C1_1[2] = { 1,E2 };
static const int knightMoves_C1_2[2] = { 1,B3 };
static const int knightMoves_C1_3[2] = { 1,A2 };
#ifndef _DEBUG
static const Direction knightMoveDirections_C1[4] = {
  Direction(MD_NONE     ,knightMoves_C1_0)
 ,Direction(MD_NONE     ,knightMoves_C1_1)
 ,Direction(MD_NONE     ,knightMoves_C1_2)
 ,Direction(MD_NONE     ,knightMoves_C1_3)
};
#else
static const Direction knightMoveDirections_C1[4] = {
  Direction(MD_NONE     ,knightMoves_C1_0,_T("knight MD_NONE from C1:D3"))
 ,Direction(MD_NONE     ,knightMoves_C1_1,_T("knight MD_NONE from C1:E2"))
 ,Direction(MD_NONE     ,knightMoves_C1_2,_T("knight MD_NONE from C1:B3"))
 ,Direction(MD_NONE     ,knightMoves_C1_3,_T("knight MD_NONE from C1:A2"))
};
#endif
static const DirectionArray knightMoves_C1(4,knightMoveDirections_C1);

static const int knightMoves_D1_0[2] = { 1,E3 };
static const int knightMoves_D1_1[2] = { 1,F2 };
static const int knightMoves_D1_2[2] = { 1,C3 };
static const int knightMoves_D1_3[2] = { 1,B2 };
#ifndef _DEBUG
static const Direction knightMoveDirections_D1[4] = {
  Direction(MD_NONE     ,knightMoves_D1_0)
 ,Direction(MD_NONE     ,knightMoves_D1_1)
 ,Direction(MD_NONE     ,knightMoves_D1_2)
 ,Direction(MD_NONE     ,knightMoves_D1_3)
};
#else
static const Direction knightMoveDirections_D1[4] = {
  Direction(MD_NONE     ,knightMoves_D1_0,_T("knight MD_NONE from D1:E3"))
 ,Direction(MD_NONE     ,knightMoves_D1_1,_T("knight MD_NONE from D1:F2"))
 ,Direction(MD_NONE     ,knightMoves_D1_2,_T("knight MD_NONE from D1:C3"))
 ,Direction(MD_NONE     ,knightMoves_D1_3,_T("knight MD_NONE from D1:B2"))
};
#endif
static const DirectionArray knightMoves_D1(4,knightMoveDirections_D1);

static const int knightMoves_E1_0[2] = { 1,F3 };
static const int knightMoves_E1_1[2] = { 1,G2 };
static const int knightMoves_E1_2[2] = { 1,D3 };
static const int knightMoves_E1_3[2] = { 1,C2 };
#ifndef _DEBUG
static const Direction knightMoveDirections_E1[4] = {
  Direction(MD_NONE     ,knightMoves_E1_0)
 ,Direction(MD_NONE     ,knightMoves_E1_1)
 ,Direction(MD_NONE     ,knightMoves_E1_2)
 ,Direction(MD_NONE     ,knightMoves_E1_3)
};
#else
static const Direction knightMoveDirections_E1[4] = {
  Direction(MD_NONE     ,knightMoves_E1_0,_T("knight MD_NONE from E1:F3"))
 ,Direction(MD_NONE     ,knightMoves_E1_1,_T("knight MD_NONE from E1:G2"))
 ,Direction(MD_NONE     ,knightMoves_E1_2,_T("knight MD_NONE from E1:D3"))
 ,Direction(MD_NONE     ,knightMoves_E1_3,_T("knight MD_NONE from E1:C2"))
};
#endif
static const DirectionArray knightMoves_E1(4,knightMoveDirections_E1);

static const int knightMoves_F1_0[2] = { 1,G3 };
static const int knightMoves_F1_1[2] = { 1,H2 };
static const int knightMoves_F1_2[2] = { 1,E3 };
static const int knightMoves_F1_3[2] = { 1,D2 };
#ifndef _DEBUG
static const Direction knightMoveDirections_F1[4] = {
  Direction(MD_NONE     ,knightMoves_F1_0)
 ,Direction(MD_NONE     ,knightMoves_F1_1)
 ,Direction(MD_NONE     ,knightMoves_F1_2)
 ,Direction(MD_NONE     ,knightMoves_F1_3)
};
#else
static const Direction knightMoveDirections_F1[4] = {
  Direction(MD_NONE     ,knightMoves_F1_0,_T("knight MD_NONE from F1:G3"))
 ,Direction(MD_NONE     ,knightMoves_F1_1,_T("knight MD_NONE from F1:H2"))
 ,Direction(MD_NONE     ,knightMoves_F1_2,_T("knight MD_NONE from F1:E3"))
 ,Direction(MD_NONE     ,knightMoves_F1_3,_T("knight MD_NONE from F1:D2"))
};
#endif
static const DirectionArray knightMoves_F1(4,knightMoveDirections_F1);

static const int knightMoves_G1_0[2] = { 1,H3 };
static const int knightMoves_G1_1[2] = { 1,F3 };
static const int knightMoves_G1_2[2] = { 1,E2 };
#ifndef _DEBUG
static const Direction knightMoveDirections_G1[3] = {
  Direction(MD_NONE     ,knightMoves_G1_0)
 ,Direction(MD_NONE     ,knightMoves_G1_1)
 ,Direction(MD_NONE     ,knightMoves_G1_2)
};
#else
static const Direction knightMoveDirections_G1[3] = {
  Direction(MD_NONE     ,knightMoves_G1_0,_T("knight MD_NONE from G1:H3"))
 ,Direction(MD_NONE     ,knightMoves_G1_1,_T("knight MD_NONE from G1:F3"))
 ,Direction(MD_NONE     ,knightMoves_G1_2,_T("knight MD_NONE from G1:E2"))
};
#endif
static const DirectionArray knightMoves_G1(3,knightMoveDirections_G1);

static const int knightMoves_H1_0[2] = { 1,G3 };
static const int knightMoves_H1_1[2] = { 1,F2 };
#ifndef _DEBUG
static const Direction knightMoveDirections_H1[2] = {
  Direction(MD_NONE     ,knightMoves_H1_0)
 ,Direction(MD_NONE     ,knightMoves_H1_1)
};
#else
static const Direction knightMoveDirections_H1[2] = {
  Direction(MD_NONE     ,knightMoves_H1_0,_T("knight MD_NONE from H1:G3"))
 ,Direction(MD_NONE     ,knightMoves_H1_1,_T("knight MD_NONE from H1:F2"))
};
#endif
static const DirectionArray knightMoves_H1(2,knightMoveDirections_H1);

static const int knightMoves_A2_0[2] = { 1,B4 };
static const int knightMoves_A2_1[2] = { 1,C3 };
static const int knightMoves_A2_2[2] = { 1,C1 };
#ifndef _DEBUG
static const Direction knightMoveDirections_A2[3] = {
  Direction(MD_NONE     ,knightMoves_A2_0)
 ,Direction(MD_NONE     ,knightMoves_A2_1)
 ,Direction(MD_NONE     ,knightMoves_A2_2)
};
#else
static const Direction knightMoveDirections_A2[3] = {
  Direction(MD_NONE     ,knightMoves_A2_0,_T("knight MD_NONE from A2:B4"))
 ,Direction(MD_NONE     ,knightMoves_A2_1,_T("knight MD_NONE from A2:C3"))
 ,Direction(MD_NONE     ,knightMoves_A2_2,_T("knight MD_NONE from A2:C1"))
};
#endif
static const DirectionArray knightMoves_A2(3,knightMoveDirections_A2);

static const int knightMoves_B2_0[2] = { 1,C4 };
static const int knightMoves_B2_1[2] = { 1,D3 };
static const int knightMoves_B2_2[2] = { 1,A4 };
static const int knightMoves_B2_3[2] = { 1,D1 };
#ifndef _DEBUG
static const Direction knightMoveDirections_B2[4] = {
  Direction(MD_NONE     ,knightMoves_B2_0)
 ,Direction(MD_NONE     ,knightMoves_B2_1)
 ,Direction(MD_NONE     ,knightMoves_B2_2)
 ,Direction(MD_NONE     ,knightMoves_B2_3)
};
#else
static const Direction knightMoveDirections_B2[4] = {
  Direction(MD_NONE     ,knightMoves_B2_0,_T("knight MD_NONE from B2:C4"))
 ,Direction(MD_NONE     ,knightMoves_B2_1,_T("knight MD_NONE from B2:D3"))
 ,Direction(MD_NONE     ,knightMoves_B2_2,_T("knight MD_NONE from B2:A4"))
 ,Direction(MD_NONE     ,knightMoves_B2_3,_T("knight MD_NONE from B2:D1"))
};
#endif
static const DirectionArray knightMoves_B2(4,knightMoveDirections_B2);

static const int knightMoves_C2_0[2] = { 1,D4 };
static const int knightMoves_C2_1[2] = { 1,E3 };
static const int knightMoves_C2_2[2] = { 1,B4 };
static const int knightMoves_C2_3[2] = { 1,E1 };
static const int knightMoves_C2_4[2] = { 1,A3 };
static const int knightMoves_C2_5[2] = { 1,A1 };
#ifndef _DEBUG
static const Direction knightMoveDirections_C2[6] = {
  Direction(MD_NONE     ,knightMoves_C2_0)
 ,Direction(MD_NONE     ,knightMoves_C2_1)
 ,Direction(MD_NONE     ,knightMoves_C2_2)
 ,Direction(MD_NONE     ,knightMoves_C2_3)
 ,Direction(MD_NONE     ,knightMoves_C2_4)
 ,Direction(MD_NONE     ,knightMoves_C2_5)
};
#else
static const Direction knightMoveDirections_C2[6] = {
  Direction(MD_NONE     ,knightMoves_C2_0,_T("knight MD_NONE from C2:D4"))
 ,Direction(MD_NONE     ,knightMoves_C2_1,_T("knight MD_NONE from C2:E3"))
 ,Direction(MD_NONE     ,knightMoves_C2_2,_T("knight MD_NONE from C2:B4"))
 ,Direction(MD_NONE     ,knightMoves_C2_3,_T("knight MD_NONE from C2:E1"))
 ,Direction(MD_NONE     ,knightMoves_C2_4,_T("knight MD_NONE from C2:A3"))
 ,Direction(MD_NONE     ,knightMoves_C2_5,_T("knight MD_NONE from C2:A1"))
};
#endif
static const DirectionArray knightMoves_C2(6,knightMoveDirections_C2);

static const int knightMoves_D2_0[2] = { 1,E4 };
static const int knightMoves_D2_1[2] = { 1,F3 };
static const int knightMoves_D2_2[2] = { 1,C4 };
static const int knightMoves_D2_3[2] = { 1,F1 };
static const int knightMoves_D2_4[2] = { 1,B3 };
static const int knightMoves_D2_5[2] = { 1,B1 };
#ifndef _DEBUG
static const Direction knightMoveDirections_D2[6] = {
  Direction(MD_NONE     ,knightMoves_D2_0)
 ,Direction(MD_NONE     ,knightMoves_D2_1)
 ,Direction(MD_NONE     ,knightMoves_D2_2)
 ,Direction(MD_NONE     ,knightMoves_D2_3)
 ,Direction(MD_NONE     ,knightMoves_D2_4)
 ,Direction(MD_NONE     ,knightMoves_D2_5)
};
#else
static const Direction knightMoveDirections_D2[6] = {
  Direction(MD_NONE     ,knightMoves_D2_0,_T("knight MD_NONE from D2:E4"))
 ,Direction(MD_NONE     ,knightMoves_D2_1,_T("knight MD_NONE from D2:F3"))
 ,Direction(MD_NONE     ,knightMoves_D2_2,_T("knight MD_NONE from D2:C4"))
 ,Direction(MD_NONE     ,knightMoves_D2_3,_T("knight MD_NONE from D2:F1"))
 ,Direction(MD_NONE     ,knightMoves_D2_4,_T("knight MD_NONE from D2:B3"))
 ,Direction(MD_NONE     ,knightMoves_D2_5,_T("knight MD_NONE from D2:B1"))
};
#endif
static const DirectionArray knightMoves_D2(6,knightMoveDirections_D2);

static const int knightMoves_E2_0[2] = { 1,F4 };
static const int knightMoves_E2_1[2] = { 1,G3 };
static const int knightMoves_E2_2[2] = { 1,D4 };
static const int knightMoves_E2_3[2] = { 1,G1 };
static const int knightMoves_E2_4[2] = { 1,C3 };
static const int knightMoves_E2_5[2] = { 1,C1 };
#ifndef _DEBUG
static const Direction knightMoveDirections_E2[6] = {
  Direction(MD_NONE     ,knightMoves_E2_0)
 ,Direction(MD_NONE     ,knightMoves_E2_1)
 ,Direction(MD_NONE     ,knightMoves_E2_2)
 ,Direction(MD_NONE     ,knightMoves_E2_3)
 ,Direction(MD_NONE     ,knightMoves_E2_4)
 ,Direction(MD_NONE     ,knightMoves_E2_5)
};
#else
static const Direction knightMoveDirections_E2[6] = {
  Direction(MD_NONE     ,knightMoves_E2_0,_T("knight MD_NONE from E2:F4"))
 ,Direction(MD_NONE     ,knightMoves_E2_1,_T("knight MD_NONE from E2:G3"))
 ,Direction(MD_NONE     ,knightMoves_E2_2,_T("knight MD_NONE from E2:D4"))
 ,Direction(MD_NONE     ,knightMoves_E2_3,_T("knight MD_NONE from E2:G1"))
 ,Direction(MD_NONE     ,knightMoves_E2_4,_T("knight MD_NONE from E2:C3"))
 ,Direction(MD_NONE     ,knightMoves_E2_5,_T("knight MD_NONE from E2:C1"))
};
#endif
static const DirectionArray knightMoves_E2(6,knightMoveDirections_E2);

static const int knightMoves_F2_0[2] = { 1,G4 };
static const int knightMoves_F2_1[2] = { 1,H3 };
static const int knightMoves_F2_2[2] = { 1,E4 };
static const int knightMoves_F2_3[2] = { 1,H1 };
static const int knightMoves_F2_4[2] = { 1,D3 };
static const int knightMoves_F2_5[2] = { 1,D1 };
#ifndef _DEBUG
static const Direction knightMoveDirections_F2[6] = {
  Direction(MD_NONE     ,knightMoves_F2_0)
 ,Direction(MD_NONE     ,knightMoves_F2_1)
 ,Direction(MD_NONE     ,knightMoves_F2_2)
 ,Direction(MD_NONE     ,knightMoves_F2_3)
 ,Direction(MD_NONE     ,knightMoves_F2_4)
 ,Direction(MD_NONE     ,knightMoves_F2_5)
};
#else
static const Direction knightMoveDirections_F2[6] = {
  Direction(MD_NONE     ,knightMoves_F2_0,_T("knight MD_NONE from F2:G4"))
 ,Direction(MD_NONE     ,knightMoves_F2_1,_T("knight MD_NONE from F2:H3"))
 ,Direction(MD_NONE     ,knightMoves_F2_2,_T("knight MD_NONE from F2:E4"))
 ,Direction(MD_NONE     ,knightMoves_F2_3,_T("knight MD_NONE from F2:H1"))
 ,Direction(MD_NONE     ,knightMoves_F2_4,_T("knight MD_NONE from F2:D3"))
 ,Direction(MD_NONE     ,knightMoves_F2_5,_T("knight MD_NONE from F2:D1"))
};
#endif
static const DirectionArray knightMoves_F2(6,knightMoveDirections_F2);

static const int knightMoves_G2_0[2] = { 1,H4 };
static const int knightMoves_G2_1[2] = { 1,F4 };
static const int knightMoves_G2_2[2] = { 1,E3 };
static const int knightMoves_G2_3[2] = { 1,E1 };
#ifndef _DEBUG
static const Direction knightMoveDirections_G2[4] = {
  Direction(MD_NONE     ,knightMoves_G2_0)
 ,Direction(MD_NONE     ,knightMoves_G2_1)
 ,Direction(MD_NONE     ,knightMoves_G2_2)
 ,Direction(MD_NONE     ,knightMoves_G2_3)
};
#else
static const Direction knightMoveDirections_G2[4] = {
  Direction(MD_NONE     ,knightMoves_G2_0,_T("knight MD_NONE from G2:H4"))
 ,Direction(MD_NONE     ,knightMoves_G2_1,_T("knight MD_NONE from G2:F4"))
 ,Direction(MD_NONE     ,knightMoves_G2_2,_T("knight MD_NONE from G2:E3"))
 ,Direction(MD_NONE     ,knightMoves_G2_3,_T("knight MD_NONE from G2:E1"))
};
#endif
static const DirectionArray knightMoves_G2(4,knightMoveDirections_G2);

static const int knightMoves_H2_0[2] = { 1,G4 };
static const int knightMoves_H2_1[2] = { 1,F3 };
static const int knightMoves_H2_2[2] = { 1,F1 };
#ifndef _DEBUG
static const Direction knightMoveDirections_H2[3] = {
  Direction(MD_NONE     ,knightMoves_H2_0)
 ,Direction(MD_NONE     ,knightMoves_H2_1)
 ,Direction(MD_NONE     ,knightMoves_H2_2)
};
#else
static const Direction knightMoveDirections_H2[3] = {
  Direction(MD_NONE     ,knightMoves_H2_0,_T("knight MD_NONE from H2:G4"))
 ,Direction(MD_NONE     ,knightMoves_H2_1,_T("knight MD_NONE from H2:F3"))
 ,Direction(MD_NONE     ,knightMoves_H2_2,_T("knight MD_NONE from H2:F1"))
};
#endif
static const DirectionArray knightMoves_H2(3,knightMoveDirections_H2);

static const int knightMoves_A3_0[2] = { 1,B5 };
static const int knightMoves_A3_1[2] = { 1,C4 };
static const int knightMoves_A3_2[2] = { 1,C2 };
static const int knightMoves_A3_3[2] = { 1,B1 };
#ifndef _DEBUG
static const Direction knightMoveDirections_A3[4] = {
  Direction(MD_NONE     ,knightMoves_A3_0)
 ,Direction(MD_NONE     ,knightMoves_A3_1)
 ,Direction(MD_NONE     ,knightMoves_A3_2)
 ,Direction(MD_NONE     ,knightMoves_A3_3)
};
#else
static const Direction knightMoveDirections_A3[4] = {
  Direction(MD_NONE     ,knightMoves_A3_0,_T("knight MD_NONE from A3:B5"))
 ,Direction(MD_NONE     ,knightMoves_A3_1,_T("knight MD_NONE from A3:C4"))
 ,Direction(MD_NONE     ,knightMoves_A3_2,_T("knight MD_NONE from A3:C2"))
 ,Direction(MD_NONE     ,knightMoves_A3_3,_T("knight MD_NONE from A3:B1"))
};
#endif
static const DirectionArray knightMoves_A3(4,knightMoveDirections_A3);

static const int knightMoves_B3_0[2] = { 1,C5 };
static const int knightMoves_B3_1[2] = { 1,D4 };
static const int knightMoves_B3_2[2] = { 1,A5 };
static const int knightMoves_B3_3[2] = { 1,D2 };
static const int knightMoves_B3_4[2] = { 1,C1 };
static const int knightMoves_B3_5[2] = { 1,A1 };
#ifndef _DEBUG
static const Direction knightMoveDirections_B3[6] = {
  Direction(MD_NONE     ,knightMoves_B3_0)
 ,Direction(MD_NONE     ,knightMoves_B3_1)
 ,Direction(MD_NONE     ,knightMoves_B3_2)
 ,Direction(MD_NONE     ,knightMoves_B3_3)
 ,Direction(MD_NONE     ,knightMoves_B3_4)
 ,Direction(MD_NONE     ,knightMoves_B3_5)
};
#else
static const Direction knightMoveDirections_B3[6] = {
  Direction(MD_NONE     ,knightMoves_B3_0,_T("knight MD_NONE from B3:C5"))
 ,Direction(MD_NONE     ,knightMoves_B3_1,_T("knight MD_NONE from B3:D4"))
 ,Direction(MD_NONE     ,knightMoves_B3_2,_T("knight MD_NONE from B3:A5"))
 ,Direction(MD_NONE     ,knightMoves_B3_3,_T("knight MD_NONE from B3:D2"))
 ,Direction(MD_NONE     ,knightMoves_B3_4,_T("knight MD_NONE from B3:C1"))
 ,Direction(MD_NONE     ,knightMoves_B3_5,_T("knight MD_NONE from B3:A1"))
};
#endif
static const DirectionArray knightMoves_B3(6,knightMoveDirections_B3);

static const int knightMoves_C3_0[2] = { 1,D5 };
static const int knightMoves_C3_1[2] = { 1,E4 };
static const int knightMoves_C3_2[2] = { 1,B5 };
static const int knightMoves_C3_3[2] = { 1,E2 };
static const int knightMoves_C3_4[2] = { 1,D1 };
static const int knightMoves_C3_5[2] = { 1,A4 };
static const int knightMoves_C3_6[2] = { 1,B1 };
static const int knightMoves_C3_7[2] = { 1,A2 };
#ifndef _DEBUG
static const Direction knightMoveDirections_C3[8] = {
  Direction(MD_NONE     ,knightMoves_C3_0)
 ,Direction(MD_NONE     ,knightMoves_C3_1)
 ,Direction(MD_NONE     ,knightMoves_C3_2)
 ,Direction(MD_NONE     ,knightMoves_C3_3)
 ,Direction(MD_NONE     ,knightMoves_C3_4)
 ,Direction(MD_NONE     ,knightMoves_C3_5)
 ,Direction(MD_NONE     ,knightMoves_C3_6)
 ,Direction(MD_NONE     ,knightMoves_C3_7)
};
#else
static const Direction knightMoveDirections_C3[8] = {
  Direction(MD_NONE     ,knightMoves_C3_0,_T("knight MD_NONE from C3:D5"))
 ,Direction(MD_NONE     ,knightMoves_C3_1,_T("knight MD_NONE from C3:E4"))
 ,Direction(MD_NONE     ,knightMoves_C3_2,_T("knight MD_NONE from C3:B5"))
 ,Direction(MD_NONE     ,knightMoves_C3_3,_T("knight MD_NONE from C3:E2"))
 ,Direction(MD_NONE     ,knightMoves_C3_4,_T("knight MD_NONE from C3:D1"))
 ,Direction(MD_NONE     ,knightMoves_C3_5,_T("knight MD_NONE from C3:A4"))
 ,Direction(MD_NONE     ,knightMoves_C3_6,_T("knight MD_NONE from C3:B1"))
 ,Direction(MD_NONE     ,knightMoves_C3_7,_T("knight MD_NONE from C3:A2"))
};
#endif
static const DirectionArray knightMoves_C3(8,knightMoveDirections_C3);

static const int knightMoves_D3_0[2] = { 1,E5 };
static const int knightMoves_D3_1[2] = { 1,F4 };
static const int knightMoves_D3_2[2] = { 1,C5 };
static const int knightMoves_D3_3[2] = { 1,F2 };
static const int knightMoves_D3_4[2] = { 1,E1 };
static const int knightMoves_D3_5[2] = { 1,B4 };
static const int knightMoves_D3_6[2] = { 1,C1 };
static const int knightMoves_D3_7[2] = { 1,B2 };
#ifndef _DEBUG
static const Direction knightMoveDirections_D3[8] = {
  Direction(MD_NONE     ,knightMoves_D3_0)
 ,Direction(MD_NONE     ,knightMoves_D3_1)
 ,Direction(MD_NONE     ,knightMoves_D3_2)
 ,Direction(MD_NONE     ,knightMoves_D3_3)
 ,Direction(MD_NONE     ,knightMoves_D3_4)
 ,Direction(MD_NONE     ,knightMoves_D3_5)
 ,Direction(MD_NONE     ,knightMoves_D3_6)
 ,Direction(MD_NONE     ,knightMoves_D3_7)
};
#else
static const Direction knightMoveDirections_D3[8] = {
  Direction(MD_NONE     ,knightMoves_D3_0,_T("knight MD_NONE from D3:E5"))
 ,Direction(MD_NONE     ,knightMoves_D3_1,_T("knight MD_NONE from D3:F4"))
 ,Direction(MD_NONE     ,knightMoves_D3_2,_T("knight MD_NONE from D3:C5"))
 ,Direction(MD_NONE     ,knightMoves_D3_3,_T("knight MD_NONE from D3:F2"))
 ,Direction(MD_NONE     ,knightMoves_D3_4,_T("knight MD_NONE from D3:E1"))
 ,Direction(MD_NONE     ,knightMoves_D3_5,_T("knight MD_NONE from D3:B4"))
 ,Direction(MD_NONE     ,knightMoves_D3_6,_T("knight MD_NONE from D3:C1"))
 ,Direction(MD_NONE     ,knightMoves_D3_7,_T("knight MD_NONE from D3:B2"))
};
#endif
static const DirectionArray knightMoves_D3(8,knightMoveDirections_D3);

static const int knightMoves_E3_0[2] = { 1,F5 };
static const int knightMoves_E3_1[2] = { 1,G4 };
static const int knightMoves_E3_2[2] = { 1,D5 };
static const int knightMoves_E3_3[2] = { 1,G2 };
static const int knightMoves_E3_4[2] = { 1,F1 };
static const int knightMoves_E3_5[2] = { 1,C4 };
static const int knightMoves_E3_6[2] = { 1,D1 };
static const int knightMoves_E3_7[2] = { 1,C2 };
#ifndef _DEBUG
static const Direction knightMoveDirections_E3[8] = {
  Direction(MD_NONE     ,knightMoves_E3_0)
 ,Direction(MD_NONE     ,knightMoves_E3_1)
 ,Direction(MD_NONE     ,knightMoves_E3_2)
 ,Direction(MD_NONE     ,knightMoves_E3_3)
 ,Direction(MD_NONE     ,knightMoves_E3_4)
 ,Direction(MD_NONE     ,knightMoves_E3_5)
 ,Direction(MD_NONE     ,knightMoves_E3_6)
 ,Direction(MD_NONE     ,knightMoves_E3_7)
};
#else
static const Direction knightMoveDirections_E3[8] = {
  Direction(MD_NONE     ,knightMoves_E3_0,_T("knight MD_NONE from E3:F5"))
 ,Direction(MD_NONE     ,knightMoves_E3_1,_T("knight MD_NONE from E3:G4"))
 ,Direction(MD_NONE     ,knightMoves_E3_2,_T("knight MD_NONE from E3:D5"))
 ,Direction(MD_NONE     ,knightMoves_E3_3,_T("knight MD_NONE from E3:G2"))
 ,Direction(MD_NONE     ,knightMoves_E3_4,_T("knight MD_NONE from E3:F1"))
 ,Direction(MD_NONE     ,knightMoves_E3_5,_T("knight MD_NONE from E3:C4"))
 ,Direction(MD_NONE     ,knightMoves_E3_6,_T("knight MD_NONE from E3:D1"))
 ,Direction(MD_NONE     ,knightMoves_E3_7,_T("knight MD_NONE from E3:C2"))
};
#endif
static const DirectionArray knightMoves_E3(8,knightMoveDirections_E3);

static const int knightMoves_F3_0[2] = { 1,G5 };
static const int knightMoves_F3_1[2] = { 1,H4 };
static const int knightMoves_F3_2[2] = { 1,E5 };
static const int knightMoves_F3_3[2] = { 1,H2 };
static const int knightMoves_F3_4[2] = { 1,G1 };
static const int knightMoves_F3_5[2] = { 1,D4 };
static const int knightMoves_F3_6[2] = { 1,E1 };
static const int knightMoves_F3_7[2] = { 1,D2 };
#ifndef _DEBUG
static const Direction knightMoveDirections_F3[8] = {
  Direction(MD_NONE     ,knightMoves_F3_0)
 ,Direction(MD_NONE     ,knightMoves_F3_1)
 ,Direction(MD_NONE     ,knightMoves_F3_2)
 ,Direction(MD_NONE     ,knightMoves_F3_3)
 ,Direction(MD_NONE     ,knightMoves_F3_4)
 ,Direction(MD_NONE     ,knightMoves_F3_5)
 ,Direction(MD_NONE     ,knightMoves_F3_6)
 ,Direction(MD_NONE     ,knightMoves_F3_7)
};
#else
static const Direction knightMoveDirections_F3[8] = {
  Direction(MD_NONE     ,knightMoves_F3_0,_T("knight MD_NONE from F3:G5"))
 ,Direction(MD_NONE     ,knightMoves_F3_1,_T("knight MD_NONE from F3:H4"))
 ,Direction(MD_NONE     ,knightMoves_F3_2,_T("knight MD_NONE from F3:E5"))
 ,Direction(MD_NONE     ,knightMoves_F3_3,_T("knight MD_NONE from F3:H2"))
 ,Direction(MD_NONE     ,knightMoves_F3_4,_T("knight MD_NONE from F3:G1"))
 ,Direction(MD_NONE     ,knightMoves_F3_5,_T("knight MD_NONE from F3:D4"))
 ,Direction(MD_NONE     ,knightMoves_F3_6,_T("knight MD_NONE from F3:E1"))
 ,Direction(MD_NONE     ,knightMoves_F3_7,_T("knight MD_NONE from F3:D2"))
};
#endif
static const DirectionArray knightMoves_F3(8,knightMoveDirections_F3);

static const int knightMoves_G3_0[2] = { 1,H5 };
static const int knightMoves_G3_1[2] = { 1,F5 };
static const int knightMoves_G3_2[2] = { 1,H1 };
static const int knightMoves_G3_3[2] = { 1,E4 };
static const int knightMoves_G3_4[2] = { 1,F1 };
static const int knightMoves_G3_5[2] = { 1,E2 };
#ifndef _DEBUG
static const Direction knightMoveDirections_G3[6] = {
  Direction(MD_NONE     ,knightMoves_G3_0)
 ,Direction(MD_NONE     ,knightMoves_G3_1)
 ,Direction(MD_NONE     ,knightMoves_G3_2)
 ,Direction(MD_NONE     ,knightMoves_G3_3)
 ,Direction(MD_NONE     ,knightMoves_G3_4)
 ,Direction(MD_NONE     ,knightMoves_G3_5)
};
#else
static const Direction knightMoveDirections_G3[6] = {
  Direction(MD_NONE     ,knightMoves_G3_0,_T("knight MD_NONE from G3:H5"))
 ,Direction(MD_NONE     ,knightMoves_G3_1,_T("knight MD_NONE from G3:F5"))
 ,Direction(MD_NONE     ,knightMoves_G3_2,_T("knight MD_NONE from G3:H1"))
 ,Direction(MD_NONE     ,knightMoves_G3_3,_T("knight MD_NONE from G3:E4"))
 ,Direction(MD_NONE     ,knightMoves_G3_4,_T("knight MD_NONE from G3:F1"))
 ,Direction(MD_NONE     ,knightMoves_G3_5,_T("knight MD_NONE from G3:E2"))
};
#endif
static const DirectionArray knightMoves_G3(6,knightMoveDirections_G3);

static const int knightMoves_H3_0[2] = { 1,G5 };
static const int knightMoves_H3_1[2] = { 1,F4 };
static const int knightMoves_H3_2[2] = { 1,G1 };
static const int knightMoves_H3_3[2] = { 1,F2 };
#ifndef _DEBUG
static const Direction knightMoveDirections_H3[4] = {
  Direction(MD_NONE     ,knightMoves_H3_0)
 ,Direction(MD_NONE     ,knightMoves_H3_1)
 ,Direction(MD_NONE     ,knightMoves_H3_2)
 ,Direction(MD_NONE     ,knightMoves_H3_3)
};
#else
static const Direction knightMoveDirections_H3[4] = {
  Direction(MD_NONE     ,knightMoves_H3_0,_T("knight MD_NONE from H3:G5"))
 ,Direction(MD_NONE     ,knightMoves_H3_1,_T("knight MD_NONE from H3:F4"))
 ,Direction(MD_NONE     ,knightMoves_H3_2,_T("knight MD_NONE from H3:G1"))
 ,Direction(MD_NONE     ,knightMoves_H3_3,_T("knight MD_NONE from H3:F2"))
};
#endif
static const DirectionArray knightMoves_H3(4,knightMoveDirections_H3);

static const int knightMoves_A4_0[2] = { 1,B6 };
static const int knightMoves_A4_1[2] = { 1,C5 };
static const int knightMoves_A4_2[2] = { 1,C3 };
static const int knightMoves_A4_3[2] = { 1,B2 };
#ifndef _DEBUG
static const Direction knightMoveDirections_A4[4] = {
  Direction(MD_NONE     ,knightMoves_A4_0)
 ,Direction(MD_NONE     ,knightMoves_A4_1)
 ,Direction(MD_NONE     ,knightMoves_A4_2)
 ,Direction(MD_NONE     ,knightMoves_A4_3)
};
#else
static const Direction knightMoveDirections_A4[4] = {
  Direction(MD_NONE     ,knightMoves_A4_0,_T("knight MD_NONE from A4:B6"))
 ,Direction(MD_NONE     ,knightMoves_A4_1,_T("knight MD_NONE from A4:C5"))
 ,Direction(MD_NONE     ,knightMoves_A4_2,_T("knight MD_NONE from A4:C3"))
 ,Direction(MD_NONE     ,knightMoves_A4_3,_T("knight MD_NONE from A4:B2"))
};
#endif
static const DirectionArray knightMoves_A4(4,knightMoveDirections_A4);

static const int knightMoves_B4_0[2] = { 1,C6 };
static const int knightMoves_B4_1[2] = { 1,D5 };
static const int knightMoves_B4_2[2] = { 1,A6 };
static const int knightMoves_B4_3[2] = { 1,D3 };
static const int knightMoves_B4_4[2] = { 1,C2 };
static const int knightMoves_B4_5[2] = { 1,A2 };
#ifndef _DEBUG
static const Direction knightMoveDirections_B4[6] = {
  Direction(MD_NONE     ,knightMoves_B4_0)
 ,Direction(MD_NONE     ,knightMoves_B4_1)
 ,Direction(MD_NONE     ,knightMoves_B4_2)
 ,Direction(MD_NONE     ,knightMoves_B4_3)
 ,Direction(MD_NONE     ,knightMoves_B4_4)
 ,Direction(MD_NONE     ,knightMoves_B4_5)
};
#else
static const Direction knightMoveDirections_B4[6] = {
  Direction(MD_NONE     ,knightMoves_B4_0,_T("knight MD_NONE from B4:C6"))
 ,Direction(MD_NONE     ,knightMoves_B4_1,_T("knight MD_NONE from B4:D5"))
 ,Direction(MD_NONE     ,knightMoves_B4_2,_T("knight MD_NONE from B4:A6"))
 ,Direction(MD_NONE     ,knightMoves_B4_3,_T("knight MD_NONE from B4:D3"))
 ,Direction(MD_NONE     ,knightMoves_B4_4,_T("knight MD_NONE from B4:C2"))
 ,Direction(MD_NONE     ,knightMoves_B4_5,_T("knight MD_NONE from B4:A2"))
};
#endif
static const DirectionArray knightMoves_B4(6,knightMoveDirections_B4);

static const int knightMoves_C4_0[2] = { 1,D6 };
static const int knightMoves_C4_1[2] = { 1,E5 };
static const int knightMoves_C4_2[2] = { 1,B6 };
static const int knightMoves_C4_3[2] = { 1,E3 };
static const int knightMoves_C4_4[2] = { 1,D2 };
static const int knightMoves_C4_5[2] = { 1,A5 };
static const int knightMoves_C4_6[2] = { 1,B2 };
static const int knightMoves_C4_7[2] = { 1,A3 };
#ifndef _DEBUG
static const Direction knightMoveDirections_C4[8] = {
  Direction(MD_NONE     ,knightMoves_C4_0)
 ,Direction(MD_NONE     ,knightMoves_C4_1)
 ,Direction(MD_NONE     ,knightMoves_C4_2)
 ,Direction(MD_NONE     ,knightMoves_C4_3)
 ,Direction(MD_NONE     ,knightMoves_C4_4)
 ,Direction(MD_NONE     ,knightMoves_C4_5)
 ,Direction(MD_NONE     ,knightMoves_C4_6)
 ,Direction(MD_NONE     ,knightMoves_C4_7)
};
#else
static const Direction knightMoveDirections_C4[8] = {
  Direction(MD_NONE     ,knightMoves_C4_0,_T("knight MD_NONE from C4:D6"))
 ,Direction(MD_NONE     ,knightMoves_C4_1,_T("knight MD_NONE from C4:E5"))
 ,Direction(MD_NONE     ,knightMoves_C4_2,_T("knight MD_NONE from C4:B6"))
 ,Direction(MD_NONE     ,knightMoves_C4_3,_T("knight MD_NONE from C4:E3"))
 ,Direction(MD_NONE     ,knightMoves_C4_4,_T("knight MD_NONE from C4:D2"))
 ,Direction(MD_NONE     ,knightMoves_C4_5,_T("knight MD_NONE from C4:A5"))
 ,Direction(MD_NONE     ,knightMoves_C4_6,_T("knight MD_NONE from C4:B2"))
 ,Direction(MD_NONE     ,knightMoves_C4_7,_T("knight MD_NONE from C4:A3"))
};
#endif
static const DirectionArray knightMoves_C4(8,knightMoveDirections_C4);

static const int knightMoves_D4_0[2] = { 1,E6 };
static const int knightMoves_D4_1[2] = { 1,F5 };
static const int knightMoves_D4_2[2] = { 1,C6 };
static const int knightMoves_D4_3[2] = { 1,F3 };
static const int knightMoves_D4_4[2] = { 1,E2 };
static const int knightMoves_D4_5[2] = { 1,B5 };
static const int knightMoves_D4_6[2] = { 1,C2 };
static const int knightMoves_D4_7[2] = { 1,B3 };
#ifndef _DEBUG
static const Direction knightMoveDirections_D4[8] = {
  Direction(MD_NONE     ,knightMoves_D4_0)
 ,Direction(MD_NONE     ,knightMoves_D4_1)
 ,Direction(MD_NONE     ,knightMoves_D4_2)
 ,Direction(MD_NONE     ,knightMoves_D4_3)
 ,Direction(MD_NONE     ,knightMoves_D4_4)
 ,Direction(MD_NONE     ,knightMoves_D4_5)
 ,Direction(MD_NONE     ,knightMoves_D4_6)
 ,Direction(MD_NONE     ,knightMoves_D4_7)
};
#else
static const Direction knightMoveDirections_D4[8] = {
  Direction(MD_NONE     ,knightMoves_D4_0,_T("knight MD_NONE from D4:E6"))
 ,Direction(MD_NONE     ,knightMoves_D4_1,_T("knight MD_NONE from D4:F5"))
 ,Direction(MD_NONE     ,knightMoves_D4_2,_T("knight MD_NONE from D4:C6"))
 ,Direction(MD_NONE     ,knightMoves_D4_3,_T("knight MD_NONE from D4:F3"))
 ,Direction(MD_NONE     ,knightMoves_D4_4,_T("knight MD_NONE from D4:E2"))
 ,Direction(MD_NONE     ,knightMoves_D4_5,_T("knight MD_NONE from D4:B5"))
 ,Direction(MD_NONE     ,knightMoves_D4_6,_T("knight MD_NONE from D4:C2"))
 ,Direction(MD_NONE     ,knightMoves_D4_7,_T("knight MD_NONE from D4:B3"))
};
#endif
static const DirectionArray knightMoves_D4(8,knightMoveDirections_D4);

static const int knightMoves_E4_0[2] = { 1,F6 };
static const int knightMoves_E4_1[2] = { 1,G5 };
static const int knightMoves_E4_2[2] = { 1,D6 };
static const int knightMoves_E4_3[2] = { 1,G3 };
static const int knightMoves_E4_4[2] = { 1,F2 };
static const int knightMoves_E4_5[2] = { 1,C5 };
static const int knightMoves_E4_6[2] = { 1,D2 };
static const int knightMoves_E4_7[2] = { 1,C3 };
#ifndef _DEBUG
static const Direction knightMoveDirections_E4[8] = {
  Direction(MD_NONE     ,knightMoves_E4_0)
 ,Direction(MD_NONE     ,knightMoves_E4_1)
 ,Direction(MD_NONE     ,knightMoves_E4_2)
 ,Direction(MD_NONE     ,knightMoves_E4_3)
 ,Direction(MD_NONE     ,knightMoves_E4_4)
 ,Direction(MD_NONE     ,knightMoves_E4_5)
 ,Direction(MD_NONE     ,knightMoves_E4_6)
 ,Direction(MD_NONE     ,knightMoves_E4_7)
};
#else
static const Direction knightMoveDirections_E4[8] = {
  Direction(MD_NONE     ,knightMoves_E4_0,_T("knight MD_NONE from E4:F6"))
 ,Direction(MD_NONE     ,knightMoves_E4_1,_T("knight MD_NONE from E4:G5"))
 ,Direction(MD_NONE     ,knightMoves_E4_2,_T("knight MD_NONE from E4:D6"))
 ,Direction(MD_NONE     ,knightMoves_E4_3,_T("knight MD_NONE from E4:G3"))
 ,Direction(MD_NONE     ,knightMoves_E4_4,_T("knight MD_NONE from E4:F2"))
 ,Direction(MD_NONE     ,knightMoves_E4_5,_T("knight MD_NONE from E4:C5"))
 ,Direction(MD_NONE     ,knightMoves_E4_6,_T("knight MD_NONE from E4:D2"))
 ,Direction(MD_NONE     ,knightMoves_E4_7,_T("knight MD_NONE from E4:C3"))
};
#endif
static const DirectionArray knightMoves_E4(8,knightMoveDirections_E4);

static const int knightMoves_F4_0[2] = { 1,G6 };
static const int knightMoves_F4_1[2] = { 1,H5 };
static const int knightMoves_F4_2[2] = { 1,E6 };
static const int knightMoves_F4_3[2] = { 1,H3 };
static const int knightMoves_F4_4[2] = { 1,G2 };
static const int knightMoves_F4_5[2] = { 1,D5 };
static const int knightMoves_F4_6[2] = { 1,E2 };
static const int knightMoves_F4_7[2] = { 1,D3 };
#ifndef _DEBUG
static const Direction knightMoveDirections_F4[8] = {
  Direction(MD_NONE     ,knightMoves_F4_0)
 ,Direction(MD_NONE     ,knightMoves_F4_1)
 ,Direction(MD_NONE     ,knightMoves_F4_2)
 ,Direction(MD_NONE     ,knightMoves_F4_3)
 ,Direction(MD_NONE     ,knightMoves_F4_4)
 ,Direction(MD_NONE     ,knightMoves_F4_5)
 ,Direction(MD_NONE     ,knightMoves_F4_6)
 ,Direction(MD_NONE     ,knightMoves_F4_7)
};
#else
static const Direction knightMoveDirections_F4[8] = {
  Direction(MD_NONE     ,knightMoves_F4_0,_T("knight MD_NONE from F4:G6"))
 ,Direction(MD_NONE     ,knightMoves_F4_1,_T("knight MD_NONE from F4:H5"))
 ,Direction(MD_NONE     ,knightMoves_F4_2,_T("knight MD_NONE from F4:E6"))
 ,Direction(MD_NONE     ,knightMoves_F4_3,_T("knight MD_NONE from F4:H3"))
 ,Direction(MD_NONE     ,knightMoves_F4_4,_T("knight MD_NONE from F4:G2"))
 ,Direction(MD_NONE     ,knightMoves_F4_5,_T("knight MD_NONE from F4:D5"))
 ,Direction(MD_NONE     ,knightMoves_F4_6,_T("knight MD_NONE from F4:E2"))
 ,Direction(MD_NONE     ,knightMoves_F4_7,_T("knight MD_NONE from F4:D3"))
};
#endif
static const DirectionArray knightMoves_F4(8,knightMoveDirections_F4);

static const int knightMoves_G4_0[2] = { 1,H6 };
static const int knightMoves_G4_1[2] = { 1,F6 };
static const int knightMoves_G4_2[2] = { 1,H2 };
static const int knightMoves_G4_3[2] = { 1,E5 };
static const int knightMoves_G4_4[2] = { 1,F2 };
static const int knightMoves_G4_5[2] = { 1,E3 };
#ifndef _DEBUG
static const Direction knightMoveDirections_G4[6] = {
  Direction(MD_NONE     ,knightMoves_G4_0)
 ,Direction(MD_NONE     ,knightMoves_G4_1)
 ,Direction(MD_NONE     ,knightMoves_G4_2)
 ,Direction(MD_NONE     ,knightMoves_G4_3)
 ,Direction(MD_NONE     ,knightMoves_G4_4)
 ,Direction(MD_NONE     ,knightMoves_G4_5)
};
#else
static const Direction knightMoveDirections_G4[6] = {
  Direction(MD_NONE     ,knightMoves_G4_0,_T("knight MD_NONE from G4:H6"))
 ,Direction(MD_NONE     ,knightMoves_G4_1,_T("knight MD_NONE from G4:F6"))
 ,Direction(MD_NONE     ,knightMoves_G4_2,_T("knight MD_NONE from G4:H2"))
 ,Direction(MD_NONE     ,knightMoves_G4_3,_T("knight MD_NONE from G4:E5"))
 ,Direction(MD_NONE     ,knightMoves_G4_4,_T("knight MD_NONE from G4:F2"))
 ,Direction(MD_NONE     ,knightMoves_G4_5,_T("knight MD_NONE from G4:E3"))
};
#endif
static const DirectionArray knightMoves_G4(6,knightMoveDirections_G4);

static const int knightMoves_H4_0[2] = { 1,G6 };
static const int knightMoves_H4_1[2] = { 1,F5 };
static const int knightMoves_H4_2[2] = { 1,G2 };
static const int knightMoves_H4_3[2] = { 1,F3 };
#ifndef _DEBUG
static const Direction knightMoveDirections_H4[4] = {
  Direction(MD_NONE     ,knightMoves_H4_0)
 ,Direction(MD_NONE     ,knightMoves_H4_1)
 ,Direction(MD_NONE     ,knightMoves_H4_2)
 ,Direction(MD_NONE     ,knightMoves_H4_3)
};
#else
static const Direction knightMoveDirections_H4[4] = {
  Direction(MD_NONE     ,knightMoves_H4_0,_T("knight MD_NONE from H4:G6"))
 ,Direction(MD_NONE     ,knightMoves_H4_1,_T("knight MD_NONE from H4:F5"))
 ,Direction(MD_NONE     ,knightMoves_H4_2,_T("knight MD_NONE from H4:G2"))
 ,Direction(MD_NONE     ,knightMoves_H4_3,_T("knight MD_NONE from H4:F3"))
};
#endif
static const DirectionArray knightMoves_H4(4,knightMoveDirections_H4);

static const int knightMoves_A5_0[2] = { 1,B7 };
static const int knightMoves_A5_1[2] = { 1,C6 };
static const int knightMoves_A5_2[2] = { 1,C4 };
static const int knightMoves_A5_3[2] = { 1,B3 };
#ifndef _DEBUG
static const Direction knightMoveDirections_A5[4] = {
  Direction(MD_NONE     ,knightMoves_A5_0)
 ,Direction(MD_NONE     ,knightMoves_A5_1)
 ,Direction(MD_NONE     ,knightMoves_A5_2)
 ,Direction(MD_NONE     ,knightMoves_A5_3)
};
#else
static const Direction knightMoveDirections_A5[4] = {
  Direction(MD_NONE     ,knightMoves_A5_0,_T("knight MD_NONE from A5:B7"))
 ,Direction(MD_NONE     ,knightMoves_A5_1,_T("knight MD_NONE from A5:C6"))
 ,Direction(MD_NONE     ,knightMoves_A5_2,_T("knight MD_NONE from A5:C4"))
 ,Direction(MD_NONE     ,knightMoves_A5_3,_T("knight MD_NONE from A5:B3"))
};
#endif
static const DirectionArray knightMoves_A5(4,knightMoveDirections_A5);

static const int knightMoves_B5_0[2] = { 1,C7 };
static const int knightMoves_B5_1[2] = { 1,D6 };
static const int knightMoves_B5_2[2] = { 1,A7 };
static const int knightMoves_B5_3[2] = { 1,D4 };
static const int knightMoves_B5_4[2] = { 1,C3 };
static const int knightMoves_B5_5[2] = { 1,A3 };
#ifndef _DEBUG
static const Direction knightMoveDirections_B5[6] = {
  Direction(MD_NONE     ,knightMoves_B5_0)
 ,Direction(MD_NONE     ,knightMoves_B5_1)
 ,Direction(MD_NONE     ,knightMoves_B5_2)
 ,Direction(MD_NONE     ,knightMoves_B5_3)
 ,Direction(MD_NONE     ,knightMoves_B5_4)
 ,Direction(MD_NONE     ,knightMoves_B5_5)
};
#else
static const Direction knightMoveDirections_B5[6] = {
  Direction(MD_NONE     ,knightMoves_B5_0,_T("knight MD_NONE from B5:C7"))
 ,Direction(MD_NONE     ,knightMoves_B5_1,_T("knight MD_NONE from B5:D6"))
 ,Direction(MD_NONE     ,knightMoves_B5_2,_T("knight MD_NONE from B5:A7"))
 ,Direction(MD_NONE     ,knightMoves_B5_3,_T("knight MD_NONE from B5:D4"))
 ,Direction(MD_NONE     ,knightMoves_B5_4,_T("knight MD_NONE from B5:C3"))
 ,Direction(MD_NONE     ,knightMoves_B5_5,_T("knight MD_NONE from B5:A3"))
};
#endif
static const DirectionArray knightMoves_B5(6,knightMoveDirections_B5);

static const int knightMoves_C5_0[2] = { 1,D7 };
static const int knightMoves_C5_1[2] = { 1,E6 };
static const int knightMoves_C5_2[2] = { 1,B7 };
static const int knightMoves_C5_3[2] = { 1,E4 };
static const int knightMoves_C5_4[2] = { 1,D3 };
static const int knightMoves_C5_5[2] = { 1,A6 };
static const int knightMoves_C5_6[2] = { 1,B3 };
static const int knightMoves_C5_7[2] = { 1,A4 };
#ifndef _DEBUG
static const Direction knightMoveDirections_C5[8] = {
  Direction(MD_NONE     ,knightMoves_C5_0)
 ,Direction(MD_NONE     ,knightMoves_C5_1)
 ,Direction(MD_NONE     ,knightMoves_C5_2)
 ,Direction(MD_NONE     ,knightMoves_C5_3)
 ,Direction(MD_NONE     ,knightMoves_C5_4)
 ,Direction(MD_NONE     ,knightMoves_C5_5)
 ,Direction(MD_NONE     ,knightMoves_C5_6)
 ,Direction(MD_NONE     ,knightMoves_C5_7)
};
#else
static const Direction knightMoveDirections_C5[8] = {
  Direction(MD_NONE     ,knightMoves_C5_0,_T("knight MD_NONE from C5:D7"))
 ,Direction(MD_NONE     ,knightMoves_C5_1,_T("knight MD_NONE from C5:E6"))
 ,Direction(MD_NONE     ,knightMoves_C5_2,_T("knight MD_NONE from C5:B7"))
 ,Direction(MD_NONE     ,knightMoves_C5_3,_T("knight MD_NONE from C5:E4"))
 ,Direction(MD_NONE     ,knightMoves_C5_4,_T("knight MD_NONE from C5:D3"))
 ,Direction(MD_NONE     ,knightMoves_C5_5,_T("knight MD_NONE from C5:A6"))
 ,Direction(MD_NONE     ,knightMoves_C5_6,_T("knight MD_NONE from C5:B3"))
 ,Direction(MD_NONE     ,knightMoves_C5_7,_T("knight MD_NONE from C5:A4"))
};
#endif
static const DirectionArray knightMoves_C5(8,knightMoveDirections_C5);

static const int knightMoves_D5_0[2] = { 1,E7 };
static const int knightMoves_D5_1[2] = { 1,F6 };
static const int knightMoves_D5_2[2] = { 1,C7 };
static const int knightMoves_D5_3[2] = { 1,F4 };
static const int knightMoves_D5_4[2] = { 1,E3 };
static const int knightMoves_D5_5[2] = { 1,B6 };
static const int knightMoves_D5_6[2] = { 1,C3 };
static const int knightMoves_D5_7[2] = { 1,B4 };
#ifndef _DEBUG
static const Direction knightMoveDirections_D5[8] = {
  Direction(MD_NONE     ,knightMoves_D5_0)
 ,Direction(MD_NONE     ,knightMoves_D5_1)
 ,Direction(MD_NONE     ,knightMoves_D5_2)
 ,Direction(MD_NONE     ,knightMoves_D5_3)
 ,Direction(MD_NONE     ,knightMoves_D5_4)
 ,Direction(MD_NONE     ,knightMoves_D5_5)
 ,Direction(MD_NONE     ,knightMoves_D5_6)
 ,Direction(MD_NONE     ,knightMoves_D5_7)
};
#else
static const Direction knightMoveDirections_D5[8] = {
  Direction(MD_NONE     ,knightMoves_D5_0,_T("knight MD_NONE from D5:E7"))
 ,Direction(MD_NONE     ,knightMoves_D5_1,_T("knight MD_NONE from D5:F6"))
 ,Direction(MD_NONE     ,knightMoves_D5_2,_T("knight MD_NONE from D5:C7"))
 ,Direction(MD_NONE     ,knightMoves_D5_3,_T("knight MD_NONE from D5:F4"))
 ,Direction(MD_NONE     ,knightMoves_D5_4,_T("knight MD_NONE from D5:E3"))
 ,Direction(MD_NONE     ,knightMoves_D5_5,_T("knight MD_NONE from D5:B6"))
 ,Direction(MD_NONE     ,knightMoves_D5_6,_T("knight MD_NONE from D5:C3"))
 ,Direction(MD_NONE     ,knightMoves_D5_7,_T("knight MD_NONE from D5:B4"))
};
#endif
static const DirectionArray knightMoves_D5(8,knightMoveDirections_D5);

static const int knightMoves_E5_0[2] = { 1,F7 };
static const int knightMoves_E5_1[2] = { 1,G6 };
static const int knightMoves_E5_2[2] = { 1,D7 };
static const int knightMoves_E5_3[2] = { 1,G4 };
static const int knightMoves_E5_4[2] = { 1,F3 };
static const int knightMoves_E5_5[2] = { 1,C6 };
static const int knightMoves_E5_6[2] = { 1,D3 };
static const int knightMoves_E5_7[2] = { 1,C4 };
#ifndef _DEBUG
static const Direction knightMoveDirections_E5[8] = {
  Direction(MD_NONE     ,knightMoves_E5_0)
 ,Direction(MD_NONE     ,knightMoves_E5_1)
 ,Direction(MD_NONE     ,knightMoves_E5_2)
 ,Direction(MD_NONE     ,knightMoves_E5_3)
 ,Direction(MD_NONE     ,knightMoves_E5_4)
 ,Direction(MD_NONE     ,knightMoves_E5_5)
 ,Direction(MD_NONE     ,knightMoves_E5_6)
 ,Direction(MD_NONE     ,knightMoves_E5_7)
};
#else
static const Direction knightMoveDirections_E5[8] = {
  Direction(MD_NONE     ,knightMoves_E5_0,_T("knight MD_NONE from E5:F7"))
 ,Direction(MD_NONE     ,knightMoves_E5_1,_T("knight MD_NONE from E5:G6"))
 ,Direction(MD_NONE     ,knightMoves_E5_2,_T("knight MD_NONE from E5:D7"))
 ,Direction(MD_NONE     ,knightMoves_E5_3,_T("knight MD_NONE from E5:G4"))
 ,Direction(MD_NONE     ,knightMoves_E5_4,_T("knight MD_NONE from E5:F3"))
 ,Direction(MD_NONE     ,knightMoves_E5_5,_T("knight MD_NONE from E5:C6"))
 ,Direction(MD_NONE     ,knightMoves_E5_6,_T("knight MD_NONE from E5:D3"))
 ,Direction(MD_NONE     ,knightMoves_E5_7,_T("knight MD_NONE from E5:C4"))
};
#endif
static const DirectionArray knightMoves_E5(8,knightMoveDirections_E5);

static const int knightMoves_F5_0[2] = { 1,G7 };
static const int knightMoves_F5_1[2] = { 1,H6 };
static const int knightMoves_F5_2[2] = { 1,E7 };
static const int knightMoves_F5_3[2] = { 1,H4 };
static const int knightMoves_F5_4[2] = { 1,G3 };
static const int knightMoves_F5_5[2] = { 1,D6 };
static const int knightMoves_F5_6[2] = { 1,E3 };
static const int knightMoves_F5_7[2] = { 1,D4 };
#ifndef _DEBUG
static const Direction knightMoveDirections_F5[8] = {
  Direction(MD_NONE     ,knightMoves_F5_0)
 ,Direction(MD_NONE     ,knightMoves_F5_1)
 ,Direction(MD_NONE     ,knightMoves_F5_2)
 ,Direction(MD_NONE     ,knightMoves_F5_3)
 ,Direction(MD_NONE     ,knightMoves_F5_4)
 ,Direction(MD_NONE     ,knightMoves_F5_5)
 ,Direction(MD_NONE     ,knightMoves_F5_6)
 ,Direction(MD_NONE     ,knightMoves_F5_7)
};
#else
static const Direction knightMoveDirections_F5[8] = {
  Direction(MD_NONE     ,knightMoves_F5_0,_T("knight MD_NONE from F5:G7"))
 ,Direction(MD_NONE     ,knightMoves_F5_1,_T("knight MD_NONE from F5:H6"))
 ,Direction(MD_NONE     ,knightMoves_F5_2,_T("knight MD_NONE from F5:E7"))
 ,Direction(MD_NONE     ,knightMoves_F5_3,_T("knight MD_NONE from F5:H4"))
 ,Direction(MD_NONE     ,knightMoves_F5_4,_T("knight MD_NONE from F5:G3"))
 ,Direction(MD_NONE     ,knightMoves_F5_5,_T("knight MD_NONE from F5:D6"))
 ,Direction(MD_NONE     ,knightMoves_F5_6,_T("knight MD_NONE from F5:E3"))
 ,Direction(MD_NONE     ,knightMoves_F5_7,_T("knight MD_NONE from F5:D4"))
};
#endif
static const DirectionArray knightMoves_F5(8,knightMoveDirections_F5);

static const int knightMoves_G5_0[2] = { 1,H7 };
static const int knightMoves_G5_1[2] = { 1,F7 };
static const int knightMoves_G5_2[2] = { 1,H3 };
static const int knightMoves_G5_3[2] = { 1,E6 };
static const int knightMoves_G5_4[2] = { 1,F3 };
static const int knightMoves_G5_5[2] = { 1,E4 };
#ifndef _DEBUG
static const Direction knightMoveDirections_G5[6] = {
  Direction(MD_NONE     ,knightMoves_G5_0)
 ,Direction(MD_NONE     ,knightMoves_G5_1)
 ,Direction(MD_NONE     ,knightMoves_G5_2)
 ,Direction(MD_NONE     ,knightMoves_G5_3)
 ,Direction(MD_NONE     ,knightMoves_G5_4)
 ,Direction(MD_NONE     ,knightMoves_G5_5)
};
#else
static const Direction knightMoveDirections_G5[6] = {
  Direction(MD_NONE     ,knightMoves_G5_0,_T("knight MD_NONE from G5:H7"))
 ,Direction(MD_NONE     ,knightMoves_G5_1,_T("knight MD_NONE from G5:F7"))
 ,Direction(MD_NONE     ,knightMoves_G5_2,_T("knight MD_NONE from G5:H3"))
 ,Direction(MD_NONE     ,knightMoves_G5_3,_T("knight MD_NONE from G5:E6"))
 ,Direction(MD_NONE     ,knightMoves_G5_4,_T("knight MD_NONE from G5:F3"))
 ,Direction(MD_NONE     ,knightMoves_G5_5,_T("knight MD_NONE from G5:E4"))
};
#endif
static const DirectionArray knightMoves_G5(6,knightMoveDirections_G5);

static const int knightMoves_H5_0[2] = { 1,G7 };
static const int knightMoves_H5_1[2] = { 1,F6 };
static const int knightMoves_H5_2[2] = { 1,G3 };
static const int knightMoves_H5_3[2] = { 1,F4 };
#ifndef _DEBUG
static const Direction knightMoveDirections_H5[4] = {
  Direction(MD_NONE     ,knightMoves_H5_0)
 ,Direction(MD_NONE     ,knightMoves_H5_1)
 ,Direction(MD_NONE     ,knightMoves_H5_2)
 ,Direction(MD_NONE     ,knightMoves_H5_3)
};
#else
static const Direction knightMoveDirections_H5[4] = {
  Direction(MD_NONE     ,knightMoves_H5_0,_T("knight MD_NONE from H5:G7"))
 ,Direction(MD_NONE     ,knightMoves_H5_1,_T("knight MD_NONE from H5:F6"))
 ,Direction(MD_NONE     ,knightMoves_H5_2,_T("knight MD_NONE from H5:G3"))
 ,Direction(MD_NONE     ,knightMoves_H5_3,_T("knight MD_NONE from H5:F4"))
};
#endif
static const DirectionArray knightMoves_H5(4,knightMoveDirections_H5);

static const int knightMoves_A6_0[2] = { 1,B8 };
static const int knightMoves_A6_1[2] = { 1,C7 };
static const int knightMoves_A6_2[2] = { 1,C5 };
static const int knightMoves_A6_3[2] = { 1,B4 };
#ifndef _DEBUG
static const Direction knightMoveDirections_A6[4] = {
  Direction(MD_NONE     ,knightMoves_A6_0)
 ,Direction(MD_NONE     ,knightMoves_A6_1)
 ,Direction(MD_NONE     ,knightMoves_A6_2)
 ,Direction(MD_NONE     ,knightMoves_A6_3)
};
#else
static const Direction knightMoveDirections_A6[4] = {
  Direction(MD_NONE     ,knightMoves_A6_0,_T("knight MD_NONE from A6:B8"))
 ,Direction(MD_NONE     ,knightMoves_A6_1,_T("knight MD_NONE from A6:C7"))
 ,Direction(MD_NONE     ,knightMoves_A6_2,_T("knight MD_NONE from A6:C5"))
 ,Direction(MD_NONE     ,knightMoves_A6_3,_T("knight MD_NONE from A6:B4"))
};
#endif
static const DirectionArray knightMoves_A6(4,knightMoveDirections_A6);

static const int knightMoves_B6_0[2] = { 1,C8 };
static const int knightMoves_B6_1[2] = { 1,D7 };
static const int knightMoves_B6_2[2] = { 1,A8 };
static const int knightMoves_B6_3[2] = { 1,D5 };
static const int knightMoves_B6_4[2] = { 1,C4 };
static const int knightMoves_B6_5[2] = { 1,A4 };
#ifndef _DEBUG
static const Direction knightMoveDirections_B6[6] = {
  Direction(MD_NONE     ,knightMoves_B6_0)
 ,Direction(MD_NONE     ,knightMoves_B6_1)
 ,Direction(MD_NONE     ,knightMoves_B6_2)
 ,Direction(MD_NONE     ,knightMoves_B6_3)
 ,Direction(MD_NONE     ,knightMoves_B6_4)
 ,Direction(MD_NONE     ,knightMoves_B6_5)
};
#else
static const Direction knightMoveDirections_B6[6] = {
  Direction(MD_NONE     ,knightMoves_B6_0,_T("knight MD_NONE from B6:C8"))
 ,Direction(MD_NONE     ,knightMoves_B6_1,_T("knight MD_NONE from B6:D7"))
 ,Direction(MD_NONE     ,knightMoves_B6_2,_T("knight MD_NONE from B6:A8"))
 ,Direction(MD_NONE     ,knightMoves_B6_3,_T("knight MD_NONE from B6:D5"))
 ,Direction(MD_NONE     ,knightMoves_B6_4,_T("knight MD_NONE from B6:C4"))
 ,Direction(MD_NONE     ,knightMoves_B6_5,_T("knight MD_NONE from B6:A4"))
};
#endif
static const DirectionArray knightMoves_B6(6,knightMoveDirections_B6);

static const int knightMoves_C6_0[2] = { 1,D8 };
static const int knightMoves_C6_1[2] = { 1,E7 };
static const int knightMoves_C6_2[2] = { 1,B8 };
static const int knightMoves_C6_3[2] = { 1,E5 };
static const int knightMoves_C6_4[2] = { 1,D4 };
static const int knightMoves_C6_5[2] = { 1,A7 };
static const int knightMoves_C6_6[2] = { 1,B4 };
static const int knightMoves_C6_7[2] = { 1,A5 };
#ifndef _DEBUG
static const Direction knightMoveDirections_C6[8] = {
  Direction(MD_NONE     ,knightMoves_C6_0)
 ,Direction(MD_NONE     ,knightMoves_C6_1)
 ,Direction(MD_NONE     ,knightMoves_C6_2)
 ,Direction(MD_NONE     ,knightMoves_C6_3)
 ,Direction(MD_NONE     ,knightMoves_C6_4)
 ,Direction(MD_NONE     ,knightMoves_C6_5)
 ,Direction(MD_NONE     ,knightMoves_C6_6)
 ,Direction(MD_NONE     ,knightMoves_C6_7)
};
#else
static const Direction knightMoveDirections_C6[8] = {
  Direction(MD_NONE     ,knightMoves_C6_0,_T("knight MD_NONE from C6:D8"))
 ,Direction(MD_NONE     ,knightMoves_C6_1,_T("knight MD_NONE from C6:E7"))
 ,Direction(MD_NONE     ,knightMoves_C6_2,_T("knight MD_NONE from C6:B8"))
 ,Direction(MD_NONE     ,knightMoves_C6_3,_T("knight MD_NONE from C6:E5"))
 ,Direction(MD_NONE     ,knightMoves_C6_4,_T("knight MD_NONE from C6:D4"))
 ,Direction(MD_NONE     ,knightMoves_C6_5,_T("knight MD_NONE from C6:A7"))
 ,Direction(MD_NONE     ,knightMoves_C6_6,_T("knight MD_NONE from C6:B4"))
 ,Direction(MD_NONE     ,knightMoves_C6_7,_T("knight MD_NONE from C6:A5"))
};
#endif
static const DirectionArray knightMoves_C6(8,knightMoveDirections_C6);

static const int knightMoves_D6_0[2] = { 1,E8 };
static const int knightMoves_D6_1[2] = { 1,F7 };
static const int knightMoves_D6_2[2] = { 1,C8 };
static const int knightMoves_D6_3[2] = { 1,F5 };
static const int knightMoves_D6_4[2] = { 1,E4 };
static const int knightMoves_D6_5[2] = { 1,B7 };
static const int knightMoves_D6_6[2] = { 1,C4 };
static const int knightMoves_D6_7[2] = { 1,B5 };
#ifndef _DEBUG
static const Direction knightMoveDirections_D6[8] = {
  Direction(MD_NONE     ,knightMoves_D6_0)
 ,Direction(MD_NONE     ,knightMoves_D6_1)
 ,Direction(MD_NONE     ,knightMoves_D6_2)
 ,Direction(MD_NONE     ,knightMoves_D6_3)
 ,Direction(MD_NONE     ,knightMoves_D6_4)
 ,Direction(MD_NONE     ,knightMoves_D6_5)
 ,Direction(MD_NONE     ,knightMoves_D6_6)
 ,Direction(MD_NONE     ,knightMoves_D6_7)
};
#else
static const Direction knightMoveDirections_D6[8] = {
  Direction(MD_NONE     ,knightMoves_D6_0,_T("knight MD_NONE from D6:E8"))
 ,Direction(MD_NONE     ,knightMoves_D6_1,_T("knight MD_NONE from D6:F7"))
 ,Direction(MD_NONE     ,knightMoves_D6_2,_T("knight MD_NONE from D6:C8"))
 ,Direction(MD_NONE     ,knightMoves_D6_3,_T("knight MD_NONE from D6:F5"))
 ,Direction(MD_NONE     ,knightMoves_D6_4,_T("knight MD_NONE from D6:E4"))
 ,Direction(MD_NONE     ,knightMoves_D6_5,_T("knight MD_NONE from D6:B7"))
 ,Direction(MD_NONE     ,knightMoves_D6_6,_T("knight MD_NONE from D6:C4"))
 ,Direction(MD_NONE     ,knightMoves_D6_7,_T("knight MD_NONE from D6:B5"))
};
#endif
static const DirectionArray knightMoves_D6(8,knightMoveDirections_D6);

static const int knightMoves_E6_0[2] = { 1,F8 };
static const int knightMoves_E6_1[2] = { 1,G7 };
static const int knightMoves_E6_2[2] = { 1,D8 };
static const int knightMoves_E6_3[2] = { 1,G5 };
static const int knightMoves_E6_4[2] = { 1,F4 };
static const int knightMoves_E6_5[2] = { 1,C7 };
static const int knightMoves_E6_6[2] = { 1,D4 };
static const int knightMoves_E6_7[2] = { 1,C5 };
#ifndef _DEBUG
static const Direction knightMoveDirections_E6[8] = {
  Direction(MD_NONE     ,knightMoves_E6_0)
 ,Direction(MD_NONE     ,knightMoves_E6_1)
 ,Direction(MD_NONE     ,knightMoves_E6_2)
 ,Direction(MD_NONE     ,knightMoves_E6_3)
 ,Direction(MD_NONE     ,knightMoves_E6_4)
 ,Direction(MD_NONE     ,knightMoves_E6_5)
 ,Direction(MD_NONE     ,knightMoves_E6_6)
 ,Direction(MD_NONE     ,knightMoves_E6_7)
};
#else
static const Direction knightMoveDirections_E6[8] = {
  Direction(MD_NONE     ,knightMoves_E6_0,_T("knight MD_NONE from E6:F8"))
 ,Direction(MD_NONE     ,knightMoves_E6_1,_T("knight MD_NONE from E6:G7"))
 ,Direction(MD_NONE     ,knightMoves_E6_2,_T("knight MD_NONE from E6:D8"))
 ,Direction(MD_NONE     ,knightMoves_E6_3,_T("knight MD_NONE from E6:G5"))
 ,Direction(MD_NONE     ,knightMoves_E6_4,_T("knight MD_NONE from E6:F4"))
 ,Direction(MD_NONE     ,knightMoves_E6_5,_T("knight MD_NONE from E6:C7"))
 ,Direction(MD_NONE     ,knightMoves_E6_6,_T("knight MD_NONE from E6:D4"))
 ,Direction(MD_NONE     ,knightMoves_E6_7,_T("knight MD_NONE from E6:C5"))
};
#endif
static const DirectionArray knightMoves_E6(8,knightMoveDirections_E6);

static const int knightMoves_F6_0[2] = { 1,G8 };
static const int knightMoves_F6_1[2] = { 1,H7 };
static const int knightMoves_F6_2[2] = { 1,E8 };
static const int knightMoves_F6_3[2] = { 1,H5 };
static const int knightMoves_F6_4[2] = { 1,G4 };
static const int knightMoves_F6_5[2] = { 1,D7 };
static const int knightMoves_F6_6[2] = { 1,E4 };
static const int knightMoves_F6_7[2] = { 1,D5 };
#ifndef _DEBUG
static const Direction knightMoveDirections_F6[8] = {
  Direction(MD_NONE     ,knightMoves_F6_0)
 ,Direction(MD_NONE     ,knightMoves_F6_1)
 ,Direction(MD_NONE     ,knightMoves_F6_2)
 ,Direction(MD_NONE     ,knightMoves_F6_3)
 ,Direction(MD_NONE     ,knightMoves_F6_4)
 ,Direction(MD_NONE     ,knightMoves_F6_5)
 ,Direction(MD_NONE     ,knightMoves_F6_6)
 ,Direction(MD_NONE     ,knightMoves_F6_7)
};
#else
static const Direction knightMoveDirections_F6[8] = {
  Direction(MD_NONE     ,knightMoves_F6_0,_T("knight MD_NONE from F6:G8"))
 ,Direction(MD_NONE     ,knightMoves_F6_1,_T("knight MD_NONE from F6:H7"))
 ,Direction(MD_NONE     ,knightMoves_F6_2,_T("knight MD_NONE from F6:E8"))
 ,Direction(MD_NONE     ,knightMoves_F6_3,_T("knight MD_NONE from F6:H5"))
 ,Direction(MD_NONE     ,knightMoves_F6_4,_T("knight MD_NONE from F6:G4"))
 ,Direction(MD_NONE     ,knightMoves_F6_5,_T("knight MD_NONE from F6:D7"))
 ,Direction(MD_NONE     ,knightMoves_F6_6,_T("knight MD_NONE from F6:E4"))
 ,Direction(MD_NONE     ,knightMoves_F6_7,_T("knight MD_NONE from F6:D5"))
};
#endif
static const DirectionArray knightMoves_F6(8,knightMoveDirections_F6);

static const int knightMoves_G6_0[2] = { 1,H8 };
static const int knightMoves_G6_1[2] = { 1,F8 };
static const int knightMoves_G6_2[2] = { 1,H4 };
static const int knightMoves_G6_3[2] = { 1,E7 };
static const int knightMoves_G6_4[2] = { 1,F4 };
static const int knightMoves_G6_5[2] = { 1,E5 };
#ifndef _DEBUG
static const Direction knightMoveDirections_G6[6] = {
  Direction(MD_NONE     ,knightMoves_G6_0)
 ,Direction(MD_NONE     ,knightMoves_G6_1)
 ,Direction(MD_NONE     ,knightMoves_G6_2)
 ,Direction(MD_NONE     ,knightMoves_G6_3)
 ,Direction(MD_NONE     ,knightMoves_G6_4)
 ,Direction(MD_NONE     ,knightMoves_G6_5)
};
#else
static const Direction knightMoveDirections_G6[6] = {
  Direction(MD_NONE     ,knightMoves_G6_0,_T("knight MD_NONE from G6:H8"))
 ,Direction(MD_NONE     ,knightMoves_G6_1,_T("knight MD_NONE from G6:F8"))
 ,Direction(MD_NONE     ,knightMoves_G6_2,_T("knight MD_NONE from G6:H4"))
 ,Direction(MD_NONE     ,knightMoves_G6_3,_T("knight MD_NONE from G6:E7"))
 ,Direction(MD_NONE     ,knightMoves_G6_4,_T("knight MD_NONE from G6:F4"))
 ,Direction(MD_NONE     ,knightMoves_G6_5,_T("knight MD_NONE from G6:E5"))
};
#endif
static const DirectionArray knightMoves_G6(6,knightMoveDirections_G6);

static const int knightMoves_H6_0[2] = { 1,G8 };
static const int knightMoves_H6_1[2] = { 1,F7 };
static const int knightMoves_H6_2[2] = { 1,G4 };
static const int knightMoves_H6_3[2] = { 1,F5 };
#ifndef _DEBUG
static const Direction knightMoveDirections_H6[4] = {
  Direction(MD_NONE     ,knightMoves_H6_0)
 ,Direction(MD_NONE     ,knightMoves_H6_1)
 ,Direction(MD_NONE     ,knightMoves_H6_2)
 ,Direction(MD_NONE     ,knightMoves_H6_3)
};
#else
static const Direction knightMoveDirections_H6[4] = {
  Direction(MD_NONE     ,knightMoves_H6_0,_T("knight MD_NONE from H6:G8"))
 ,Direction(MD_NONE     ,knightMoves_H6_1,_T("knight MD_NONE from H6:F7"))
 ,Direction(MD_NONE     ,knightMoves_H6_2,_T("knight MD_NONE from H6:G4"))
 ,Direction(MD_NONE     ,knightMoves_H6_3,_T("knight MD_NONE from H6:F5"))
};
#endif
static const DirectionArray knightMoves_H6(4,knightMoveDirections_H6);

static const int knightMoves_A7_0[2] = { 1,C8 };
static const int knightMoves_A7_1[2] = { 1,C6 };
static const int knightMoves_A7_2[2] = { 1,B5 };
#ifndef _DEBUG
static const Direction knightMoveDirections_A7[3] = {
  Direction(MD_NONE     ,knightMoves_A7_0)
 ,Direction(MD_NONE     ,knightMoves_A7_1)
 ,Direction(MD_NONE     ,knightMoves_A7_2)
};
#else
static const Direction knightMoveDirections_A7[3] = {
  Direction(MD_NONE     ,knightMoves_A7_0,_T("knight MD_NONE from A7:C8"))
 ,Direction(MD_NONE     ,knightMoves_A7_1,_T("knight MD_NONE from A7:C6"))
 ,Direction(MD_NONE     ,knightMoves_A7_2,_T("knight MD_NONE from A7:B5"))
};
#endif
static const DirectionArray knightMoves_A7(3,knightMoveDirections_A7);

static const int knightMoves_B7_0[2] = { 1,D8 };
static const int knightMoves_B7_1[2] = { 1,D6 };
static const int knightMoves_B7_2[2] = { 1,C5 };
static const int knightMoves_B7_3[2] = { 1,A5 };
#ifndef _DEBUG
static const Direction knightMoveDirections_B7[4] = {
  Direction(MD_NONE     ,knightMoves_B7_0)
 ,Direction(MD_NONE     ,knightMoves_B7_1)
 ,Direction(MD_NONE     ,knightMoves_B7_2)
 ,Direction(MD_NONE     ,knightMoves_B7_3)
};
#else
static const Direction knightMoveDirections_B7[4] = {
  Direction(MD_NONE     ,knightMoves_B7_0,_T("knight MD_NONE from B7:D8"))
 ,Direction(MD_NONE     ,knightMoves_B7_1,_T("knight MD_NONE from B7:D6"))
 ,Direction(MD_NONE     ,knightMoves_B7_2,_T("knight MD_NONE from B7:C5"))
 ,Direction(MD_NONE     ,knightMoves_B7_3,_T("knight MD_NONE from B7:A5"))
};
#endif
static const DirectionArray knightMoves_B7(4,knightMoveDirections_B7);

static const int knightMoves_C7_0[2] = { 1,E8 };
static const int knightMoves_C7_1[2] = { 1,E6 };
static const int knightMoves_C7_2[2] = { 1,D5 };
static const int knightMoves_C7_3[2] = { 1,A8 };
static const int knightMoves_C7_4[2] = { 1,B5 };
static const int knightMoves_C7_5[2] = { 1,A6 };
#ifndef _DEBUG
static const Direction knightMoveDirections_C7[6] = {
  Direction(MD_NONE     ,knightMoves_C7_0)
 ,Direction(MD_NONE     ,knightMoves_C7_1)
 ,Direction(MD_NONE     ,knightMoves_C7_2)
 ,Direction(MD_NONE     ,knightMoves_C7_3)
 ,Direction(MD_NONE     ,knightMoves_C7_4)
 ,Direction(MD_NONE     ,knightMoves_C7_5)
};
#else
static const Direction knightMoveDirections_C7[6] = {
  Direction(MD_NONE     ,knightMoves_C7_0,_T("knight MD_NONE from C7:E8"))
 ,Direction(MD_NONE     ,knightMoves_C7_1,_T("knight MD_NONE from C7:E6"))
 ,Direction(MD_NONE     ,knightMoves_C7_2,_T("knight MD_NONE from C7:D5"))
 ,Direction(MD_NONE     ,knightMoves_C7_3,_T("knight MD_NONE from C7:A8"))
 ,Direction(MD_NONE     ,knightMoves_C7_4,_T("knight MD_NONE from C7:B5"))
 ,Direction(MD_NONE     ,knightMoves_C7_5,_T("knight MD_NONE from C7:A6"))
};
#endif
static const DirectionArray knightMoves_C7(6,knightMoveDirections_C7);

static const int knightMoves_D7_0[2] = { 1,F8 };
static const int knightMoves_D7_1[2] = { 1,F6 };
static const int knightMoves_D7_2[2] = { 1,E5 };
static const int knightMoves_D7_3[2] = { 1,B8 };
static const int knightMoves_D7_4[2] = { 1,C5 };
static const int knightMoves_D7_5[2] = { 1,B6 };
#ifndef _DEBUG
static const Direction knightMoveDirections_D7[6] = {
  Direction(MD_NONE     ,knightMoves_D7_0)
 ,Direction(MD_NONE     ,knightMoves_D7_1)
 ,Direction(MD_NONE     ,knightMoves_D7_2)
 ,Direction(MD_NONE     ,knightMoves_D7_3)
 ,Direction(MD_NONE     ,knightMoves_D7_4)
 ,Direction(MD_NONE     ,knightMoves_D7_5)
};
#else
static const Direction knightMoveDirections_D7[6] = {
  Direction(MD_NONE     ,knightMoves_D7_0,_T("knight MD_NONE from D7:F8"))
 ,Direction(MD_NONE     ,knightMoves_D7_1,_T("knight MD_NONE from D7:F6"))
 ,Direction(MD_NONE     ,knightMoves_D7_2,_T("knight MD_NONE from D7:E5"))
 ,Direction(MD_NONE     ,knightMoves_D7_3,_T("knight MD_NONE from D7:B8"))
 ,Direction(MD_NONE     ,knightMoves_D7_4,_T("knight MD_NONE from D7:C5"))
 ,Direction(MD_NONE     ,knightMoves_D7_5,_T("knight MD_NONE from D7:B6"))
};
#endif
static const DirectionArray knightMoves_D7(6,knightMoveDirections_D7);

static const int knightMoves_E7_0[2] = { 1,G8 };
static const int knightMoves_E7_1[2] = { 1,G6 };
static const int knightMoves_E7_2[2] = { 1,F5 };
static const int knightMoves_E7_3[2] = { 1,C8 };
static const int knightMoves_E7_4[2] = { 1,D5 };
static const int knightMoves_E7_5[2] = { 1,C6 };
#ifndef _DEBUG
static const Direction knightMoveDirections_E7[6] = {
  Direction(MD_NONE     ,knightMoves_E7_0)
 ,Direction(MD_NONE     ,knightMoves_E7_1)
 ,Direction(MD_NONE     ,knightMoves_E7_2)
 ,Direction(MD_NONE     ,knightMoves_E7_3)
 ,Direction(MD_NONE     ,knightMoves_E7_4)
 ,Direction(MD_NONE     ,knightMoves_E7_5)
};
#else
static const Direction knightMoveDirections_E7[6] = {
  Direction(MD_NONE     ,knightMoves_E7_0,_T("knight MD_NONE from E7:G8"))
 ,Direction(MD_NONE     ,knightMoves_E7_1,_T("knight MD_NONE from E7:G6"))
 ,Direction(MD_NONE     ,knightMoves_E7_2,_T("knight MD_NONE from E7:F5"))
 ,Direction(MD_NONE     ,knightMoves_E7_3,_T("knight MD_NONE from E7:C8"))
 ,Direction(MD_NONE     ,knightMoves_E7_4,_T("knight MD_NONE from E7:D5"))
 ,Direction(MD_NONE     ,knightMoves_E7_5,_T("knight MD_NONE from E7:C6"))
};
#endif
static const DirectionArray knightMoves_E7(6,knightMoveDirections_E7);

static const int knightMoves_F7_0[2] = { 1,H8 };
static const int knightMoves_F7_1[2] = { 1,H6 };
static const int knightMoves_F7_2[2] = { 1,G5 };
static const int knightMoves_F7_3[2] = { 1,D8 };
static const int knightMoves_F7_4[2] = { 1,E5 };
static const int knightMoves_F7_5[2] = { 1,D6 };
#ifndef _DEBUG
static const Direction knightMoveDirections_F7[6] = {
  Direction(MD_NONE     ,knightMoves_F7_0)
 ,Direction(MD_NONE     ,knightMoves_F7_1)
 ,Direction(MD_NONE     ,knightMoves_F7_2)
 ,Direction(MD_NONE     ,knightMoves_F7_3)
 ,Direction(MD_NONE     ,knightMoves_F7_4)
 ,Direction(MD_NONE     ,knightMoves_F7_5)
};
#else
static const Direction knightMoveDirections_F7[6] = {
  Direction(MD_NONE     ,knightMoves_F7_0,_T("knight MD_NONE from F7:H8"))
 ,Direction(MD_NONE     ,knightMoves_F7_1,_T("knight MD_NONE from F7:H6"))
 ,Direction(MD_NONE     ,knightMoves_F7_2,_T("knight MD_NONE from F7:G5"))
 ,Direction(MD_NONE     ,knightMoves_F7_3,_T("knight MD_NONE from F7:D8"))
 ,Direction(MD_NONE     ,knightMoves_F7_4,_T("knight MD_NONE from F7:E5"))
 ,Direction(MD_NONE     ,knightMoves_F7_5,_T("knight MD_NONE from F7:D6"))
};
#endif
static const DirectionArray knightMoves_F7(6,knightMoveDirections_F7);

static const int knightMoves_G7_0[2] = { 1,H5 };
static const int knightMoves_G7_1[2] = { 1,E8 };
static const int knightMoves_G7_2[2] = { 1,F5 };
static const int knightMoves_G7_3[2] = { 1,E6 };
#ifndef _DEBUG
static const Direction knightMoveDirections_G7[4] = {
  Direction(MD_NONE     ,knightMoves_G7_0)
 ,Direction(MD_NONE     ,knightMoves_G7_1)
 ,Direction(MD_NONE     ,knightMoves_G7_2)
 ,Direction(MD_NONE     ,knightMoves_G7_3)
};
#else
static const Direction knightMoveDirections_G7[4] = {
  Direction(MD_NONE     ,knightMoves_G7_0,_T("knight MD_NONE from G7:H5"))
 ,Direction(MD_NONE     ,knightMoves_G7_1,_T("knight MD_NONE from G7:E8"))
 ,Direction(MD_NONE     ,knightMoves_G7_2,_T("knight MD_NONE from G7:F5"))
 ,Direction(MD_NONE     ,knightMoves_G7_3,_T("knight MD_NONE from G7:E6"))
};
#endif
static const DirectionArray knightMoves_G7(4,knightMoveDirections_G7);

static const int knightMoves_H7_0[2] = { 1,F8 };
static const int knightMoves_H7_1[2] = { 1,G5 };
static const int knightMoves_H7_2[2] = { 1,F6 };
#ifndef _DEBUG
static const Direction knightMoveDirections_H7[3] = {
  Direction(MD_NONE     ,knightMoves_H7_0)
 ,Direction(MD_NONE     ,knightMoves_H7_1)
 ,Direction(MD_NONE     ,knightMoves_H7_2)
};
#else
static const Direction knightMoveDirections_H7[3] = {
  Direction(MD_NONE     ,knightMoves_H7_0,_T("knight MD_NONE from H7:F8"))
 ,Direction(MD_NONE     ,knightMoves_H7_1,_T("knight MD_NONE from H7:G5"))
 ,Direction(MD_NONE     ,knightMoves_H7_2,_T("knight MD_NONE from H7:F6"))
};
#endif
static const DirectionArray knightMoves_H7(3,knightMoveDirections_H7);

static const int knightMoves_A8_0[2] = { 1,C7 };
static const int knightMoves_A8_1[2] = { 1,B6 };
#ifndef _DEBUG
static const Direction knightMoveDirections_A8[2] = {
  Direction(MD_NONE     ,knightMoves_A8_0)
 ,Direction(MD_NONE     ,knightMoves_A8_1)
};
#else
static const Direction knightMoveDirections_A8[2] = {
  Direction(MD_NONE     ,knightMoves_A8_0,_T("knight MD_NONE from A8:C7"))
 ,Direction(MD_NONE     ,knightMoves_A8_1,_T("knight MD_NONE from A8:B6"))
};
#endif
static const DirectionArray knightMoves_A8(2,knightMoveDirections_A8);

static const int knightMoves_B8_0[2] = { 1,D7 };
static const int knightMoves_B8_1[2] = { 1,C6 };
static const int knightMoves_B8_2[2] = { 1,A6 };
#ifndef _DEBUG
static const Direction knightMoveDirections_B8[3] = {
  Direction(MD_NONE     ,knightMoves_B8_0)
 ,Direction(MD_NONE     ,knightMoves_B8_1)
 ,Direction(MD_NONE     ,knightMoves_B8_2)
};
#else
static const Direction knightMoveDirections_B8[3] = {
  Direction(MD_NONE     ,knightMoves_B8_0,_T("knight MD_NONE from B8:D7"))
 ,Direction(MD_NONE     ,knightMoves_B8_1,_T("knight MD_NONE from B8:C6"))
 ,Direction(MD_NONE     ,knightMoves_B8_2,_T("knight MD_NONE from B8:A6"))
};
#endif
static const DirectionArray knightMoves_B8(3,knightMoveDirections_B8);

static const int knightMoves_C8_0[2] = { 1,E7 };
static const int knightMoves_C8_1[2] = { 1,D6 };
static const int knightMoves_C8_2[2] = { 1,B6 };
static const int knightMoves_C8_3[2] = { 1,A7 };
#ifndef _DEBUG
static const Direction knightMoveDirections_C8[4] = {
  Direction(MD_NONE     ,knightMoves_C8_0)
 ,Direction(MD_NONE     ,knightMoves_C8_1)
 ,Direction(MD_NONE     ,knightMoves_C8_2)
 ,Direction(MD_NONE     ,knightMoves_C8_3)
};
#else
static const Direction knightMoveDirections_C8[4] = {
  Direction(MD_NONE     ,knightMoves_C8_0,_T("knight MD_NONE from C8:E7"))
 ,Direction(MD_NONE     ,knightMoves_C8_1,_T("knight MD_NONE from C8:D6"))
 ,Direction(MD_NONE     ,knightMoves_C8_2,_T("knight MD_NONE from C8:B6"))
 ,Direction(MD_NONE     ,knightMoves_C8_3,_T("knight MD_NONE from C8:A7"))
};
#endif
static const DirectionArray knightMoves_C8(4,knightMoveDirections_C8);

static const int knightMoves_D8_0[2] = { 1,F7 };
static const int knightMoves_D8_1[2] = { 1,E6 };
static const int knightMoves_D8_2[2] = { 1,C6 };
static const int knightMoves_D8_3[2] = { 1,B7 };
#ifndef _DEBUG
static const Direction knightMoveDirections_D8[4] = {
  Direction(MD_NONE     ,knightMoves_D8_0)
 ,Direction(MD_NONE     ,knightMoves_D8_1)
 ,Direction(MD_NONE     ,knightMoves_D8_2)
 ,Direction(MD_NONE     ,knightMoves_D8_3)
};
#else
static const Direction knightMoveDirections_D8[4] = {
  Direction(MD_NONE     ,knightMoves_D8_0,_T("knight MD_NONE from D8:F7"))
 ,Direction(MD_NONE     ,knightMoves_D8_1,_T("knight MD_NONE from D8:E6"))
 ,Direction(MD_NONE     ,knightMoves_D8_2,_T("knight MD_NONE from D8:C6"))
 ,Direction(MD_NONE     ,knightMoves_D8_3,_T("knight MD_NONE from D8:B7"))
};
#endif
static const DirectionArray knightMoves_D8(4,knightMoveDirections_D8);

static const int knightMoves_E8_0[2] = { 1,G7 };
static const int knightMoves_E8_1[2] = { 1,F6 };
static const int knightMoves_E8_2[2] = { 1,D6 };
static const int knightMoves_E8_3[2] = { 1,C7 };
#ifndef _DEBUG
static const Direction knightMoveDirections_E8[4] = {
  Direction(MD_NONE     ,knightMoves_E8_0)
 ,Direction(MD_NONE     ,knightMoves_E8_1)
 ,Direction(MD_NONE     ,knightMoves_E8_2)
 ,Direction(MD_NONE     ,knightMoves_E8_3)
};
#else
static const Direction knightMoveDirections_E8[4] = {
  Direction(MD_NONE     ,knightMoves_E8_0,_T("knight MD_NONE from E8:G7"))
 ,Direction(MD_NONE     ,knightMoves_E8_1,_T("knight MD_NONE from E8:F6"))
 ,Direction(MD_NONE     ,knightMoves_E8_2,_T("knight MD_NONE from E8:D6"))
 ,Direction(MD_NONE     ,knightMoves_E8_3,_T("knight MD_NONE from E8:C7"))
};
#endif
static const DirectionArray knightMoves_E8(4,knightMoveDirections_E8);

static const int knightMoves_F8_0[2] = { 1,H7 };
static const int knightMoves_F8_1[2] = { 1,G6 };
static const int knightMoves_F8_2[2] = { 1,E6 };
static const int knightMoves_F8_3[2] = { 1,D7 };
#ifndef _DEBUG
static const Direction knightMoveDirections_F8[4] = {
  Direction(MD_NONE     ,knightMoves_F8_0)
 ,Direction(MD_NONE     ,knightMoves_F8_1)
 ,Direction(MD_NONE     ,knightMoves_F8_2)
 ,Direction(MD_NONE     ,knightMoves_F8_3)
};
#else
static const Direction knightMoveDirections_F8[4] = {
  Direction(MD_NONE     ,knightMoves_F8_0,_T("knight MD_NONE from F8:H7"))
 ,Direction(MD_NONE     ,knightMoves_F8_1,_T("knight MD_NONE from F8:G6"))
 ,Direction(MD_NONE     ,knightMoves_F8_2,_T("knight MD_NONE from F8:E6"))
 ,Direction(MD_NONE     ,knightMoves_F8_3,_T("knight MD_NONE from F8:D7"))
};
#endif
static const DirectionArray knightMoves_F8(4,knightMoveDirections_F8);

static const int knightMoves_G8_0[2] = { 1,H6 };
static const int knightMoves_G8_1[2] = { 1,F6 };
static const int knightMoves_G8_2[2] = { 1,E7 };
#ifndef _DEBUG
static const Direction knightMoveDirections_G8[3] = {
  Direction(MD_NONE     ,knightMoves_G8_0)
 ,Direction(MD_NONE     ,knightMoves_G8_1)
 ,Direction(MD_NONE     ,knightMoves_G8_2)
};
#else
static const Direction knightMoveDirections_G8[3] = {
  Direction(MD_NONE     ,knightMoves_G8_0,_T("knight MD_NONE from G8:H6"))
 ,Direction(MD_NONE     ,knightMoves_G8_1,_T("knight MD_NONE from G8:F6"))
 ,Direction(MD_NONE     ,knightMoves_G8_2,_T("knight MD_NONE from G8:E7"))
};
#endif
static const DirectionArray knightMoves_G8(3,knightMoveDirections_G8);

static const int knightMoves_H8_0[2] = { 1,G6 };
static const int knightMoves_H8_1[2] = { 1,F7 };
#ifndef _DEBUG
static const Direction knightMoveDirections_H8[2] = {
  Direction(MD_NONE     ,knightMoves_H8_0)
 ,Direction(MD_NONE     ,knightMoves_H8_1)
};
#else
static const Direction knightMoveDirections_H8[2] = {
  Direction(MD_NONE     ,knightMoves_H8_0,_T("knight MD_NONE from H8:G6"))
 ,Direction(MD_NONE     ,knightMoves_H8_1,_T("knight MD_NONE from H8:F7"))
};
#endif
static const DirectionArray knightMoves_H8(2,knightMoveDirections_H8);

static const DirectionArray whitePawnMoves_A1(0,NULL);

static const DirectionArray whitePawnMoves_B1(0,NULL);

static const DirectionArray whitePawnMoves_C1(0,NULL);

static const DirectionArray whitePawnMoves_D1(0,NULL);

static const DirectionArray whitePawnMoves_E1(0,NULL);

static const DirectionArray whitePawnMoves_F1(0,NULL);

static const DirectionArray whitePawnMoves_G1(0,NULL);

static const DirectionArray whitePawnMoves_H1(0,NULL);

static const int whitePawnMoves_A2_0[3] = { 2,A3,A4 };
static const int whitePawnMoves_A2_1[2] = { 1,B3 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_A2[2] = {
  Direction(MD_UP       ,whitePawnMoves_A2_0)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_A2_1)
};
#else
static const Direction whitePawnMoveDirections_A2[2] = {
  Direction(MD_UP       ,whitePawnMoves_A2_0,_T("whitePawn MD_UP from A2:A3 A4"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_A2_1,_T("whitePawn MD_UPDIAG1 from A2:B3"))
};
#endif
static const DirectionArray whitePawnMoves_A2(2,whitePawnMoveDirections_A2);

static const int whitePawnMoves_B2_0[2] = { 1,A3 };
static const int whitePawnMoves_B2_1[3] = { 2,B3,B4 };
static const int whitePawnMoves_B2_2[2] = { 1,C3 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_B2[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_B2_0)
 ,Direction(MD_UP       ,whitePawnMoves_B2_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_B2_2)
};
#else
static const Direction whitePawnMoveDirections_B2[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_B2_0,_T("whitePawn MD_UPDIAG2 from B2:A3"))
 ,Direction(MD_UP       ,whitePawnMoves_B2_1,_T("whitePawn MD_UP from B2:B3 B4"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_B2_2,_T("whitePawn MD_UPDIAG1 from B2:C3"))
};
#endif
static const DirectionArray whitePawnMoves_B2(3,whitePawnMoveDirections_B2);

static const int whitePawnMoves_C2_0[2] = { 1,B3 };
static const int whitePawnMoves_C2_1[3] = { 2,C3,C4 };
static const int whitePawnMoves_C2_2[2] = { 1,D3 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_C2[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_C2_0)
 ,Direction(MD_UP       ,whitePawnMoves_C2_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_C2_2)
};
#else
static const Direction whitePawnMoveDirections_C2[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_C2_0,_T("whitePawn MD_UPDIAG2 from C2:B3"))
 ,Direction(MD_UP       ,whitePawnMoves_C2_1,_T("whitePawn MD_UP from C2:C3 C4"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_C2_2,_T("whitePawn MD_UPDIAG1 from C2:D3"))
};
#endif
static const DirectionArray whitePawnMoves_C2(3,whitePawnMoveDirections_C2);

static const int whitePawnMoves_D2_0[2] = { 1,C3 };
static const int whitePawnMoves_D2_1[3] = { 2,D3,D4 };
static const int whitePawnMoves_D2_2[2] = { 1,E3 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_D2[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_D2_0)
 ,Direction(MD_UP       ,whitePawnMoves_D2_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_D2_2)
};
#else
static const Direction whitePawnMoveDirections_D2[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_D2_0,_T("whitePawn MD_UPDIAG2 from D2:C3"))
 ,Direction(MD_UP       ,whitePawnMoves_D2_1,_T("whitePawn MD_UP from D2:D3 D4"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_D2_2,_T("whitePawn MD_UPDIAG1 from D2:E3"))
};
#endif
static const DirectionArray whitePawnMoves_D2(3,whitePawnMoveDirections_D2);

static const int whitePawnMoves_E2_0[2] = { 1,D3 };
static const int whitePawnMoves_E2_1[3] = { 2,E3,E4 };
static const int whitePawnMoves_E2_2[2] = { 1,F3 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_E2[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_E2_0)
 ,Direction(MD_UP       ,whitePawnMoves_E2_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_E2_2)
};
#else
static const Direction whitePawnMoveDirections_E2[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_E2_0,_T("whitePawn MD_UPDIAG2 from E2:D3"))
 ,Direction(MD_UP       ,whitePawnMoves_E2_1,_T("whitePawn MD_UP from E2:E3 E4"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_E2_2,_T("whitePawn MD_UPDIAG1 from E2:F3"))
};
#endif
static const DirectionArray whitePawnMoves_E2(3,whitePawnMoveDirections_E2);

static const int whitePawnMoves_F2_0[2] = { 1,E3 };
static const int whitePawnMoves_F2_1[3] = { 2,F3,F4 };
static const int whitePawnMoves_F2_2[2] = { 1,G3 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_F2[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_F2_0)
 ,Direction(MD_UP       ,whitePawnMoves_F2_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_F2_2)
};
#else
static const Direction whitePawnMoveDirections_F2[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_F2_0,_T("whitePawn MD_UPDIAG2 from F2:E3"))
 ,Direction(MD_UP       ,whitePawnMoves_F2_1,_T("whitePawn MD_UP from F2:F3 F4"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_F2_2,_T("whitePawn MD_UPDIAG1 from F2:G3"))
};
#endif
static const DirectionArray whitePawnMoves_F2(3,whitePawnMoveDirections_F2);

static const int whitePawnMoves_G2_0[2] = { 1,F3 };
static const int whitePawnMoves_G2_1[3] = { 2,G3,G4 };
static const int whitePawnMoves_G2_2[2] = { 1,H3 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_G2[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_G2_0)
 ,Direction(MD_UP       ,whitePawnMoves_G2_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_G2_2)
};
#else
static const Direction whitePawnMoveDirections_G2[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_G2_0,_T("whitePawn MD_UPDIAG2 from G2:F3"))
 ,Direction(MD_UP       ,whitePawnMoves_G2_1,_T("whitePawn MD_UP from G2:G3 G4"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_G2_2,_T("whitePawn MD_UPDIAG1 from G2:H3"))
};
#endif
static const DirectionArray whitePawnMoves_G2(3,whitePawnMoveDirections_G2);

static const int whitePawnMoves_H2_0[2] = { 1,G3 };
static const int whitePawnMoves_H2_1[3] = { 2,H3,H4 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_H2[2] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_H2_0)
 ,Direction(MD_UP       ,whitePawnMoves_H2_1)
};
#else
static const Direction whitePawnMoveDirections_H2[2] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_H2_0,_T("whitePawn MD_UPDIAG2 from H2:G3"))
 ,Direction(MD_UP       ,whitePawnMoves_H2_1,_T("whitePawn MD_UP from H2:H3 H4"))
};
#endif
static const DirectionArray whitePawnMoves_H2(2,whitePawnMoveDirections_H2);

static const int whitePawnMoves_A3_0[2] = { 1,A4 };
static const int whitePawnMoves_A3_1[2] = { 1,B4 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_A3[2] = {
  Direction(MD_UP       ,whitePawnMoves_A3_0)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_A3_1)
};
#else
static const Direction whitePawnMoveDirections_A3[2] = {
  Direction(MD_UP       ,whitePawnMoves_A3_0,_T("whitePawn MD_UP from A3:A4"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_A3_1,_T("whitePawn MD_UPDIAG1 from A3:B4"))
};
#endif
static const DirectionArray whitePawnMoves_A3(2,whitePawnMoveDirections_A3);

static const int whitePawnMoves_B3_0[2] = { 1,A4 };
static const int whitePawnMoves_B3_1[2] = { 1,B4 };
static const int whitePawnMoves_B3_2[2] = { 1,C4 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_B3[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_B3_0)
 ,Direction(MD_UP       ,whitePawnMoves_B3_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_B3_2)
};
#else
static const Direction whitePawnMoveDirections_B3[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_B3_0,_T("whitePawn MD_UPDIAG2 from B3:A4"))
 ,Direction(MD_UP       ,whitePawnMoves_B3_1,_T("whitePawn MD_UP from B3:B4"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_B3_2,_T("whitePawn MD_UPDIAG1 from B3:C4"))
};
#endif
static const DirectionArray whitePawnMoves_B3(3,whitePawnMoveDirections_B3);

static const int whitePawnMoves_C3_0[2] = { 1,B4 };
static const int whitePawnMoves_C3_1[2] = { 1,C4 };
static const int whitePawnMoves_C3_2[2] = { 1,D4 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_C3[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_C3_0)
 ,Direction(MD_UP       ,whitePawnMoves_C3_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_C3_2)
};
#else
static const Direction whitePawnMoveDirections_C3[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_C3_0,_T("whitePawn MD_UPDIAG2 from C3:B4"))
 ,Direction(MD_UP       ,whitePawnMoves_C3_1,_T("whitePawn MD_UP from C3:C4"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_C3_2,_T("whitePawn MD_UPDIAG1 from C3:D4"))
};
#endif
static const DirectionArray whitePawnMoves_C3(3,whitePawnMoveDirections_C3);

static const int whitePawnMoves_D3_0[2] = { 1,C4 };
static const int whitePawnMoves_D3_1[2] = { 1,D4 };
static const int whitePawnMoves_D3_2[2] = { 1,E4 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_D3[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_D3_0)
 ,Direction(MD_UP       ,whitePawnMoves_D3_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_D3_2)
};
#else
static const Direction whitePawnMoveDirections_D3[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_D3_0,_T("whitePawn MD_UPDIAG2 from D3:C4"))
 ,Direction(MD_UP       ,whitePawnMoves_D3_1,_T("whitePawn MD_UP from D3:D4"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_D3_2,_T("whitePawn MD_UPDIAG1 from D3:E4"))
};
#endif
static const DirectionArray whitePawnMoves_D3(3,whitePawnMoveDirections_D3);

static const int whitePawnMoves_E3_0[2] = { 1,D4 };
static const int whitePawnMoves_E3_1[2] = { 1,E4 };
static const int whitePawnMoves_E3_2[2] = { 1,F4 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_E3[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_E3_0)
 ,Direction(MD_UP       ,whitePawnMoves_E3_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_E3_2)
};
#else
static const Direction whitePawnMoveDirections_E3[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_E3_0,_T("whitePawn MD_UPDIAG2 from E3:D4"))
 ,Direction(MD_UP       ,whitePawnMoves_E3_1,_T("whitePawn MD_UP from E3:E4"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_E3_2,_T("whitePawn MD_UPDIAG1 from E3:F4"))
};
#endif
static const DirectionArray whitePawnMoves_E3(3,whitePawnMoveDirections_E3);

static const int whitePawnMoves_F3_0[2] = { 1,E4 };
static const int whitePawnMoves_F3_1[2] = { 1,F4 };
static const int whitePawnMoves_F3_2[2] = { 1,G4 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_F3[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_F3_0)
 ,Direction(MD_UP       ,whitePawnMoves_F3_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_F3_2)
};
#else
static const Direction whitePawnMoveDirections_F3[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_F3_0,_T("whitePawn MD_UPDIAG2 from F3:E4"))
 ,Direction(MD_UP       ,whitePawnMoves_F3_1,_T("whitePawn MD_UP from F3:F4"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_F3_2,_T("whitePawn MD_UPDIAG1 from F3:G4"))
};
#endif
static const DirectionArray whitePawnMoves_F3(3,whitePawnMoveDirections_F3);

static const int whitePawnMoves_G3_0[2] = { 1,F4 };
static const int whitePawnMoves_G3_1[2] = { 1,G4 };
static const int whitePawnMoves_G3_2[2] = { 1,H4 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_G3[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_G3_0)
 ,Direction(MD_UP       ,whitePawnMoves_G3_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_G3_2)
};
#else
static const Direction whitePawnMoveDirections_G3[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_G3_0,_T("whitePawn MD_UPDIAG2 from G3:F4"))
 ,Direction(MD_UP       ,whitePawnMoves_G3_1,_T("whitePawn MD_UP from G3:G4"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_G3_2,_T("whitePawn MD_UPDIAG1 from G3:H4"))
};
#endif
static const DirectionArray whitePawnMoves_G3(3,whitePawnMoveDirections_G3);

static const int whitePawnMoves_H3_0[2] = { 1,G4 };
static const int whitePawnMoves_H3_1[2] = { 1,H4 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_H3[2] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_H3_0)
 ,Direction(MD_UP       ,whitePawnMoves_H3_1)
};
#else
static const Direction whitePawnMoveDirections_H3[2] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_H3_0,_T("whitePawn MD_UPDIAG2 from H3:G4"))
 ,Direction(MD_UP       ,whitePawnMoves_H3_1,_T("whitePawn MD_UP from H3:H4"))
};
#endif
static const DirectionArray whitePawnMoves_H3(2,whitePawnMoveDirections_H3);

static const int whitePawnMoves_A4_0[2] = { 1,A5 };
static const int whitePawnMoves_A4_1[2] = { 1,B5 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_A4[2] = {
  Direction(MD_UP       ,whitePawnMoves_A4_0)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_A4_1)
};
#else
static const Direction whitePawnMoveDirections_A4[2] = {
  Direction(MD_UP       ,whitePawnMoves_A4_0,_T("whitePawn MD_UP from A4:A5"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_A4_1,_T("whitePawn MD_UPDIAG1 from A4:B5"))
};
#endif
static const DirectionArray whitePawnMoves_A4(2,whitePawnMoveDirections_A4);

static const int whitePawnMoves_B4_0[2] = { 1,A5 };
static const int whitePawnMoves_B4_1[2] = { 1,B5 };
static const int whitePawnMoves_B4_2[2] = { 1,C5 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_B4[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_B4_0)
 ,Direction(MD_UP       ,whitePawnMoves_B4_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_B4_2)
};
#else
static const Direction whitePawnMoveDirections_B4[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_B4_0,_T("whitePawn MD_UPDIAG2 from B4:A5"))
 ,Direction(MD_UP       ,whitePawnMoves_B4_1,_T("whitePawn MD_UP from B4:B5"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_B4_2,_T("whitePawn MD_UPDIAG1 from B4:C5"))
};
#endif
static const DirectionArray whitePawnMoves_B4(3,whitePawnMoveDirections_B4);

static const int whitePawnMoves_C4_0[2] = { 1,B5 };
static const int whitePawnMoves_C4_1[2] = { 1,C5 };
static const int whitePawnMoves_C4_2[2] = { 1,D5 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_C4[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_C4_0)
 ,Direction(MD_UP       ,whitePawnMoves_C4_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_C4_2)
};
#else
static const Direction whitePawnMoveDirections_C4[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_C4_0,_T("whitePawn MD_UPDIAG2 from C4:B5"))
 ,Direction(MD_UP       ,whitePawnMoves_C4_1,_T("whitePawn MD_UP from C4:C5"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_C4_2,_T("whitePawn MD_UPDIAG1 from C4:D5"))
};
#endif
static const DirectionArray whitePawnMoves_C4(3,whitePawnMoveDirections_C4);

static const int whitePawnMoves_D4_0[2] = { 1,C5 };
static const int whitePawnMoves_D4_1[2] = { 1,D5 };
static const int whitePawnMoves_D4_2[2] = { 1,E5 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_D4[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_D4_0)
 ,Direction(MD_UP       ,whitePawnMoves_D4_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_D4_2)
};
#else
static const Direction whitePawnMoveDirections_D4[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_D4_0,_T("whitePawn MD_UPDIAG2 from D4:C5"))
 ,Direction(MD_UP       ,whitePawnMoves_D4_1,_T("whitePawn MD_UP from D4:D5"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_D4_2,_T("whitePawn MD_UPDIAG1 from D4:E5"))
};
#endif
static const DirectionArray whitePawnMoves_D4(3,whitePawnMoveDirections_D4);

static const int whitePawnMoves_E4_0[2] = { 1,D5 };
static const int whitePawnMoves_E4_1[2] = { 1,E5 };
static const int whitePawnMoves_E4_2[2] = { 1,F5 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_E4[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_E4_0)
 ,Direction(MD_UP       ,whitePawnMoves_E4_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_E4_2)
};
#else
static const Direction whitePawnMoveDirections_E4[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_E4_0,_T("whitePawn MD_UPDIAG2 from E4:D5"))
 ,Direction(MD_UP       ,whitePawnMoves_E4_1,_T("whitePawn MD_UP from E4:E5"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_E4_2,_T("whitePawn MD_UPDIAG1 from E4:F5"))
};
#endif
static const DirectionArray whitePawnMoves_E4(3,whitePawnMoveDirections_E4);

static const int whitePawnMoves_F4_0[2] = { 1,E5 };
static const int whitePawnMoves_F4_1[2] = { 1,F5 };
static const int whitePawnMoves_F4_2[2] = { 1,G5 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_F4[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_F4_0)
 ,Direction(MD_UP       ,whitePawnMoves_F4_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_F4_2)
};
#else
static const Direction whitePawnMoveDirections_F4[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_F4_0,_T("whitePawn MD_UPDIAG2 from F4:E5"))
 ,Direction(MD_UP       ,whitePawnMoves_F4_1,_T("whitePawn MD_UP from F4:F5"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_F4_2,_T("whitePawn MD_UPDIAG1 from F4:G5"))
};
#endif
static const DirectionArray whitePawnMoves_F4(3,whitePawnMoveDirections_F4);

static const int whitePawnMoves_G4_0[2] = { 1,F5 };
static const int whitePawnMoves_G4_1[2] = { 1,G5 };
static const int whitePawnMoves_G4_2[2] = { 1,H5 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_G4[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_G4_0)
 ,Direction(MD_UP       ,whitePawnMoves_G4_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_G4_2)
};
#else
static const Direction whitePawnMoveDirections_G4[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_G4_0,_T("whitePawn MD_UPDIAG2 from G4:F5"))
 ,Direction(MD_UP       ,whitePawnMoves_G4_1,_T("whitePawn MD_UP from G4:G5"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_G4_2,_T("whitePawn MD_UPDIAG1 from G4:H5"))
};
#endif
static const DirectionArray whitePawnMoves_G4(3,whitePawnMoveDirections_G4);

static const int whitePawnMoves_H4_0[2] = { 1,G5 };
static const int whitePawnMoves_H4_1[2] = { 1,H5 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_H4[2] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_H4_0)
 ,Direction(MD_UP       ,whitePawnMoves_H4_1)
};
#else
static const Direction whitePawnMoveDirections_H4[2] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_H4_0,_T("whitePawn MD_UPDIAG2 from H4:G5"))
 ,Direction(MD_UP       ,whitePawnMoves_H4_1,_T("whitePawn MD_UP from H4:H5"))
};
#endif
static const DirectionArray whitePawnMoves_H4(2,whitePawnMoveDirections_H4);

static const int whitePawnMoves_A5_0[2] = { 1,A6 };
static const int whitePawnMoves_A5_1[2] = { 1,B6 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_A5[2] = {
  Direction(MD_UP       ,whitePawnMoves_A5_0)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_A5_1)
};
#else
static const Direction whitePawnMoveDirections_A5[2] = {
  Direction(MD_UP       ,whitePawnMoves_A5_0,_T("whitePawn MD_UP from A5:A6"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_A5_1,_T("whitePawn MD_UPDIAG1 from A5:B6"))
};
#endif
static const DirectionArray whitePawnMoves_A5(2,whitePawnMoveDirections_A5);

static const int whitePawnMoves_B5_0[2] = { 1,A6 };
static const int whitePawnMoves_B5_1[2] = { 1,B6 };
static const int whitePawnMoves_B5_2[2] = { 1,C6 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_B5[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_B5_0)
 ,Direction(MD_UP       ,whitePawnMoves_B5_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_B5_2)
};
#else
static const Direction whitePawnMoveDirections_B5[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_B5_0,_T("whitePawn MD_UPDIAG2 from B5:A6"))
 ,Direction(MD_UP       ,whitePawnMoves_B5_1,_T("whitePawn MD_UP from B5:B6"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_B5_2,_T("whitePawn MD_UPDIAG1 from B5:C6"))
};
#endif
static const DirectionArray whitePawnMoves_B5(3,whitePawnMoveDirections_B5);

static const int whitePawnMoves_C5_0[2] = { 1,B6 };
static const int whitePawnMoves_C5_1[2] = { 1,C6 };
static const int whitePawnMoves_C5_2[2] = { 1,D6 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_C5[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_C5_0)
 ,Direction(MD_UP       ,whitePawnMoves_C5_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_C5_2)
};
#else
static const Direction whitePawnMoveDirections_C5[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_C5_0,_T("whitePawn MD_UPDIAG2 from C5:B6"))
 ,Direction(MD_UP       ,whitePawnMoves_C5_1,_T("whitePawn MD_UP from C5:C6"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_C5_2,_T("whitePawn MD_UPDIAG1 from C5:D6"))
};
#endif
static const DirectionArray whitePawnMoves_C5(3,whitePawnMoveDirections_C5);

static const int whitePawnMoves_D5_0[2] = { 1,C6 };
static const int whitePawnMoves_D5_1[2] = { 1,D6 };
static const int whitePawnMoves_D5_2[2] = { 1,E6 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_D5[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_D5_0)
 ,Direction(MD_UP       ,whitePawnMoves_D5_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_D5_2)
};
#else
static const Direction whitePawnMoveDirections_D5[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_D5_0,_T("whitePawn MD_UPDIAG2 from D5:C6"))
 ,Direction(MD_UP       ,whitePawnMoves_D5_1,_T("whitePawn MD_UP from D5:D6"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_D5_2,_T("whitePawn MD_UPDIAG1 from D5:E6"))
};
#endif
static const DirectionArray whitePawnMoves_D5(3,whitePawnMoveDirections_D5);

static const int whitePawnMoves_E5_0[2] = { 1,D6 };
static const int whitePawnMoves_E5_1[2] = { 1,E6 };
static const int whitePawnMoves_E5_2[2] = { 1,F6 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_E5[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_E5_0)
 ,Direction(MD_UP       ,whitePawnMoves_E5_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_E5_2)
};
#else
static const Direction whitePawnMoveDirections_E5[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_E5_0,_T("whitePawn MD_UPDIAG2 from E5:D6"))
 ,Direction(MD_UP       ,whitePawnMoves_E5_1,_T("whitePawn MD_UP from E5:E6"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_E5_2,_T("whitePawn MD_UPDIAG1 from E5:F6"))
};
#endif
static const DirectionArray whitePawnMoves_E5(3,whitePawnMoveDirections_E5);

static const int whitePawnMoves_F5_0[2] = { 1,E6 };
static const int whitePawnMoves_F5_1[2] = { 1,F6 };
static const int whitePawnMoves_F5_2[2] = { 1,G6 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_F5[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_F5_0)
 ,Direction(MD_UP       ,whitePawnMoves_F5_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_F5_2)
};
#else
static const Direction whitePawnMoveDirections_F5[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_F5_0,_T("whitePawn MD_UPDIAG2 from F5:E6"))
 ,Direction(MD_UP       ,whitePawnMoves_F5_1,_T("whitePawn MD_UP from F5:F6"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_F5_2,_T("whitePawn MD_UPDIAG1 from F5:G6"))
};
#endif
static const DirectionArray whitePawnMoves_F5(3,whitePawnMoveDirections_F5);

static const int whitePawnMoves_G5_0[2] = { 1,F6 };
static const int whitePawnMoves_G5_1[2] = { 1,G6 };
static const int whitePawnMoves_G5_2[2] = { 1,H6 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_G5[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_G5_0)
 ,Direction(MD_UP       ,whitePawnMoves_G5_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_G5_2)
};
#else
static const Direction whitePawnMoveDirections_G5[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_G5_0,_T("whitePawn MD_UPDIAG2 from G5:F6"))
 ,Direction(MD_UP       ,whitePawnMoves_G5_1,_T("whitePawn MD_UP from G5:G6"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_G5_2,_T("whitePawn MD_UPDIAG1 from G5:H6"))
};
#endif
static const DirectionArray whitePawnMoves_G5(3,whitePawnMoveDirections_G5);

static const int whitePawnMoves_H5_0[2] = { 1,G6 };
static const int whitePawnMoves_H5_1[2] = { 1,H6 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_H5[2] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_H5_0)
 ,Direction(MD_UP       ,whitePawnMoves_H5_1)
};
#else
static const Direction whitePawnMoveDirections_H5[2] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_H5_0,_T("whitePawn MD_UPDIAG2 from H5:G6"))
 ,Direction(MD_UP       ,whitePawnMoves_H5_1,_T("whitePawn MD_UP from H5:H6"))
};
#endif
static const DirectionArray whitePawnMoves_H5(2,whitePawnMoveDirections_H5);

static const int whitePawnMoves_A6_0[2] = { 1,A7 };
static const int whitePawnMoves_A6_1[2] = { 1,B7 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_A6[2] = {
  Direction(MD_UP       ,whitePawnMoves_A6_0)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_A6_1)
};
#else
static const Direction whitePawnMoveDirections_A6[2] = {
  Direction(MD_UP       ,whitePawnMoves_A6_0,_T("whitePawn MD_UP from A6:A7"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_A6_1,_T("whitePawn MD_UPDIAG1 from A6:B7"))
};
#endif
static const DirectionArray whitePawnMoves_A6(2,whitePawnMoveDirections_A6);

static const int whitePawnMoves_B6_0[2] = { 1,A7 };
static const int whitePawnMoves_B6_1[2] = { 1,B7 };
static const int whitePawnMoves_B6_2[2] = { 1,C7 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_B6[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_B6_0)
 ,Direction(MD_UP       ,whitePawnMoves_B6_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_B6_2)
};
#else
static const Direction whitePawnMoveDirections_B6[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_B6_0,_T("whitePawn MD_UPDIAG2 from B6:A7"))
 ,Direction(MD_UP       ,whitePawnMoves_B6_1,_T("whitePawn MD_UP from B6:B7"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_B6_2,_T("whitePawn MD_UPDIAG1 from B6:C7"))
};
#endif
static const DirectionArray whitePawnMoves_B6(3,whitePawnMoveDirections_B6);

static const int whitePawnMoves_C6_0[2] = { 1,B7 };
static const int whitePawnMoves_C6_1[2] = { 1,C7 };
static const int whitePawnMoves_C6_2[2] = { 1,D7 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_C6[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_C6_0)
 ,Direction(MD_UP       ,whitePawnMoves_C6_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_C6_2)
};
#else
static const Direction whitePawnMoveDirections_C6[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_C6_0,_T("whitePawn MD_UPDIAG2 from C6:B7"))
 ,Direction(MD_UP       ,whitePawnMoves_C6_1,_T("whitePawn MD_UP from C6:C7"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_C6_2,_T("whitePawn MD_UPDIAG1 from C6:D7"))
};
#endif
static const DirectionArray whitePawnMoves_C6(3,whitePawnMoveDirections_C6);

static const int whitePawnMoves_D6_0[2] = { 1,C7 };
static const int whitePawnMoves_D6_1[2] = { 1,D7 };
static const int whitePawnMoves_D6_2[2] = { 1,E7 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_D6[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_D6_0)
 ,Direction(MD_UP       ,whitePawnMoves_D6_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_D6_2)
};
#else
static const Direction whitePawnMoveDirections_D6[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_D6_0,_T("whitePawn MD_UPDIAG2 from D6:C7"))
 ,Direction(MD_UP       ,whitePawnMoves_D6_1,_T("whitePawn MD_UP from D6:D7"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_D6_2,_T("whitePawn MD_UPDIAG1 from D6:E7"))
};
#endif
static const DirectionArray whitePawnMoves_D6(3,whitePawnMoveDirections_D6);

static const int whitePawnMoves_E6_0[2] = { 1,D7 };
static const int whitePawnMoves_E6_1[2] = { 1,E7 };
static const int whitePawnMoves_E6_2[2] = { 1,F7 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_E6[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_E6_0)
 ,Direction(MD_UP       ,whitePawnMoves_E6_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_E6_2)
};
#else
static const Direction whitePawnMoveDirections_E6[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_E6_0,_T("whitePawn MD_UPDIAG2 from E6:D7"))
 ,Direction(MD_UP       ,whitePawnMoves_E6_1,_T("whitePawn MD_UP from E6:E7"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_E6_2,_T("whitePawn MD_UPDIAG1 from E6:F7"))
};
#endif
static const DirectionArray whitePawnMoves_E6(3,whitePawnMoveDirections_E6);

static const int whitePawnMoves_F6_0[2] = { 1,E7 };
static const int whitePawnMoves_F6_1[2] = { 1,F7 };
static const int whitePawnMoves_F6_2[2] = { 1,G7 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_F6[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_F6_0)
 ,Direction(MD_UP       ,whitePawnMoves_F6_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_F6_2)
};
#else
static const Direction whitePawnMoveDirections_F6[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_F6_0,_T("whitePawn MD_UPDIAG2 from F6:E7"))
 ,Direction(MD_UP       ,whitePawnMoves_F6_1,_T("whitePawn MD_UP from F6:F7"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_F6_2,_T("whitePawn MD_UPDIAG1 from F6:G7"))
};
#endif
static const DirectionArray whitePawnMoves_F6(3,whitePawnMoveDirections_F6);

static const int whitePawnMoves_G6_0[2] = { 1,F7 };
static const int whitePawnMoves_G6_1[2] = { 1,G7 };
static const int whitePawnMoves_G6_2[2] = { 1,H7 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_G6[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_G6_0)
 ,Direction(MD_UP       ,whitePawnMoves_G6_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_G6_2)
};
#else
static const Direction whitePawnMoveDirections_G6[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_G6_0,_T("whitePawn MD_UPDIAG2 from G6:F7"))
 ,Direction(MD_UP       ,whitePawnMoves_G6_1,_T("whitePawn MD_UP from G6:G7"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_G6_2,_T("whitePawn MD_UPDIAG1 from G6:H7"))
};
#endif
static const DirectionArray whitePawnMoves_G6(3,whitePawnMoveDirections_G6);

static const int whitePawnMoves_H6_0[2] = { 1,G7 };
static const int whitePawnMoves_H6_1[2] = { 1,H7 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_H6[2] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_H6_0)
 ,Direction(MD_UP       ,whitePawnMoves_H6_1)
};
#else
static const Direction whitePawnMoveDirections_H6[2] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_H6_0,_T("whitePawn MD_UPDIAG2 from H6:G7"))
 ,Direction(MD_UP       ,whitePawnMoves_H6_1,_T("whitePawn MD_UP from H6:H7"))
};
#endif
static const DirectionArray whitePawnMoves_H6(2,whitePawnMoveDirections_H6);

static const int whitePawnMoves_A7_0[2] = { 1,A8 };
static const int whitePawnMoves_A7_1[2] = { 1,B8 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_A7[2] = {
  Direction(MD_UP       ,whitePawnMoves_A7_0)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_A7_1)
};
#else
static const Direction whitePawnMoveDirections_A7[2] = {
  Direction(MD_UP       ,whitePawnMoves_A7_0,_T("whitePawn MD_UP from A7:A8"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_A7_1,_T("whitePawn MD_UPDIAG1 from A7:B8"))
};
#endif
static const DirectionArray whitePawnMoves_A7(2,whitePawnMoveDirections_A7);

static const int whitePawnMoves_B7_0[2] = { 1,A8 };
static const int whitePawnMoves_B7_1[2] = { 1,B8 };
static const int whitePawnMoves_B7_2[2] = { 1,C8 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_B7[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_B7_0)
 ,Direction(MD_UP       ,whitePawnMoves_B7_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_B7_2)
};
#else
static const Direction whitePawnMoveDirections_B7[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_B7_0,_T("whitePawn MD_UPDIAG2 from B7:A8"))
 ,Direction(MD_UP       ,whitePawnMoves_B7_1,_T("whitePawn MD_UP from B7:B8"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_B7_2,_T("whitePawn MD_UPDIAG1 from B7:C8"))
};
#endif
static const DirectionArray whitePawnMoves_B7(3,whitePawnMoveDirections_B7);

static const int whitePawnMoves_C7_0[2] = { 1,B8 };
static const int whitePawnMoves_C7_1[2] = { 1,C8 };
static const int whitePawnMoves_C7_2[2] = { 1,D8 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_C7[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_C7_0)
 ,Direction(MD_UP       ,whitePawnMoves_C7_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_C7_2)
};
#else
static const Direction whitePawnMoveDirections_C7[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_C7_0,_T("whitePawn MD_UPDIAG2 from C7:B8"))
 ,Direction(MD_UP       ,whitePawnMoves_C7_1,_T("whitePawn MD_UP from C7:C8"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_C7_2,_T("whitePawn MD_UPDIAG1 from C7:D8"))
};
#endif
static const DirectionArray whitePawnMoves_C7(3,whitePawnMoveDirections_C7);

static const int whitePawnMoves_D7_0[2] = { 1,C8 };
static const int whitePawnMoves_D7_1[2] = { 1,D8 };
static const int whitePawnMoves_D7_2[2] = { 1,E8 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_D7[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_D7_0)
 ,Direction(MD_UP       ,whitePawnMoves_D7_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_D7_2)
};
#else
static const Direction whitePawnMoveDirections_D7[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_D7_0,_T("whitePawn MD_UPDIAG2 from D7:C8"))
 ,Direction(MD_UP       ,whitePawnMoves_D7_1,_T("whitePawn MD_UP from D7:D8"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_D7_2,_T("whitePawn MD_UPDIAG1 from D7:E8"))
};
#endif
static const DirectionArray whitePawnMoves_D7(3,whitePawnMoveDirections_D7);

static const int whitePawnMoves_E7_0[2] = { 1,D8 };
static const int whitePawnMoves_E7_1[2] = { 1,E8 };
static const int whitePawnMoves_E7_2[2] = { 1,F8 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_E7[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_E7_0)
 ,Direction(MD_UP       ,whitePawnMoves_E7_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_E7_2)
};
#else
static const Direction whitePawnMoveDirections_E7[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_E7_0,_T("whitePawn MD_UPDIAG2 from E7:D8"))
 ,Direction(MD_UP       ,whitePawnMoves_E7_1,_T("whitePawn MD_UP from E7:E8"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_E7_2,_T("whitePawn MD_UPDIAG1 from E7:F8"))
};
#endif
static const DirectionArray whitePawnMoves_E7(3,whitePawnMoveDirections_E7);

static const int whitePawnMoves_F7_0[2] = { 1,E8 };
static const int whitePawnMoves_F7_1[2] = { 1,F8 };
static const int whitePawnMoves_F7_2[2] = { 1,G8 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_F7[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_F7_0)
 ,Direction(MD_UP       ,whitePawnMoves_F7_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_F7_2)
};
#else
static const Direction whitePawnMoveDirections_F7[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_F7_0,_T("whitePawn MD_UPDIAG2 from F7:E8"))
 ,Direction(MD_UP       ,whitePawnMoves_F7_1,_T("whitePawn MD_UP from F7:F8"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_F7_2,_T("whitePawn MD_UPDIAG1 from F7:G8"))
};
#endif
static const DirectionArray whitePawnMoves_F7(3,whitePawnMoveDirections_F7);

static const int whitePawnMoves_G7_0[2] = { 1,F8 };
static const int whitePawnMoves_G7_1[2] = { 1,G8 };
static const int whitePawnMoves_G7_2[2] = { 1,H8 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_G7[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_G7_0)
 ,Direction(MD_UP       ,whitePawnMoves_G7_1)
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_G7_2)
};
#else
static const Direction whitePawnMoveDirections_G7[3] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_G7_0,_T("whitePawn MD_UPDIAG2 from G7:F8"))
 ,Direction(MD_UP       ,whitePawnMoves_G7_1,_T("whitePawn MD_UP from G7:G8"))
 ,Direction(MD_UPDIAG1  ,whitePawnMoves_G7_2,_T("whitePawn MD_UPDIAG1 from G7:H8"))
};
#endif
static const DirectionArray whitePawnMoves_G7(3,whitePawnMoveDirections_G7);

static const int whitePawnMoves_H7_0[2] = { 1,G8 };
static const int whitePawnMoves_H7_1[2] = { 1,H8 };
#ifndef _DEBUG
static const Direction whitePawnMoveDirections_H7[2] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_H7_0)
 ,Direction(MD_UP       ,whitePawnMoves_H7_1)
};
#else
static const Direction whitePawnMoveDirections_H7[2] = {
  Direction(MD_UPDIAG2  ,whitePawnMoves_H7_0,_T("whitePawn MD_UPDIAG2 from H7:G8"))
 ,Direction(MD_UP       ,whitePawnMoves_H7_1,_T("whitePawn MD_UP from H7:H8"))
};
#endif
static const DirectionArray whitePawnMoves_H7(2,whitePawnMoveDirections_H7);

static const DirectionArray whitePawnMoves_A8(0,NULL);

static const DirectionArray whitePawnMoves_B8(0,NULL);

static const DirectionArray whitePawnMoves_C8(0,NULL);

static const DirectionArray whitePawnMoves_D8(0,NULL);

static const DirectionArray whitePawnMoves_E8(0,NULL);

static const DirectionArray whitePawnMoves_F8(0,NULL);

static const DirectionArray whitePawnMoves_G8(0,NULL);

static const DirectionArray whitePawnMoves_H8(0,NULL);

static const DirectionArray blackPawnMoves_A1(0,NULL);

static const DirectionArray blackPawnMoves_B1(0,NULL);

static const DirectionArray blackPawnMoves_C1(0,NULL);

static const DirectionArray blackPawnMoves_D1(0,NULL);

static const DirectionArray blackPawnMoves_E1(0,NULL);

static const DirectionArray blackPawnMoves_F1(0,NULL);

static const DirectionArray blackPawnMoves_G1(0,NULL);

static const DirectionArray blackPawnMoves_H1(0,NULL);

static const int blackPawnMoves_A2_0[2] = { 1,A1 };
static const int blackPawnMoves_A2_1[2] = { 1,B1 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_A2[2] = {
  Direction(MD_DOWN     ,blackPawnMoves_A2_0)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_A2_1)
};
#else
static const Direction blackPawnMoveDirections_A2[2] = {
  Direction(MD_DOWN     ,blackPawnMoves_A2_0,_T("blackPawn MD_DOWN from A2:A1"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_A2_1,_T("blackPawn MD_DOWNDIAG2 from A2:B1"))
};
#endif
static const DirectionArray blackPawnMoves_A2(2,blackPawnMoveDirections_A2);

static const int blackPawnMoves_B2_0[2] = { 1,A1 };
static const int blackPawnMoves_B2_1[2] = { 1,B1 };
static const int blackPawnMoves_B2_2[2] = { 1,C1 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_B2[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_B2_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_B2_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_B2_2)
};
#else
static const Direction blackPawnMoveDirections_B2[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_B2_0,_T("blackPawn MD_DOWNDIAG1 from B2:A1"))
 ,Direction(MD_DOWN     ,blackPawnMoves_B2_1,_T("blackPawn MD_DOWN from B2:B1"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_B2_2,_T("blackPawn MD_DOWNDIAG2 from B2:C1"))
};
#endif
static const DirectionArray blackPawnMoves_B2(3,blackPawnMoveDirections_B2);

static const int blackPawnMoves_C2_0[2] = { 1,B1 };
static const int blackPawnMoves_C2_1[2] = { 1,C1 };
static const int blackPawnMoves_C2_2[2] = { 1,D1 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_C2[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_C2_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_C2_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_C2_2)
};
#else
static const Direction blackPawnMoveDirections_C2[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_C2_0,_T("blackPawn MD_DOWNDIAG1 from C2:B1"))
 ,Direction(MD_DOWN     ,blackPawnMoves_C2_1,_T("blackPawn MD_DOWN from C2:C1"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_C2_2,_T("blackPawn MD_DOWNDIAG2 from C2:D1"))
};
#endif
static const DirectionArray blackPawnMoves_C2(3,blackPawnMoveDirections_C2);

static const int blackPawnMoves_D2_0[2] = { 1,C1 };
static const int blackPawnMoves_D2_1[2] = { 1,D1 };
static const int blackPawnMoves_D2_2[2] = { 1,E1 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_D2[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_D2_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_D2_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_D2_2)
};
#else
static const Direction blackPawnMoveDirections_D2[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_D2_0,_T("blackPawn MD_DOWNDIAG1 from D2:C1"))
 ,Direction(MD_DOWN     ,blackPawnMoves_D2_1,_T("blackPawn MD_DOWN from D2:D1"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_D2_2,_T("blackPawn MD_DOWNDIAG2 from D2:E1"))
};
#endif
static const DirectionArray blackPawnMoves_D2(3,blackPawnMoveDirections_D2);

static const int blackPawnMoves_E2_0[2] = { 1,D1 };
static const int blackPawnMoves_E2_1[2] = { 1,E1 };
static const int blackPawnMoves_E2_2[2] = { 1,F1 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_E2[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_E2_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_E2_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_E2_2)
};
#else
static const Direction blackPawnMoveDirections_E2[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_E2_0,_T("blackPawn MD_DOWNDIAG1 from E2:D1"))
 ,Direction(MD_DOWN     ,blackPawnMoves_E2_1,_T("blackPawn MD_DOWN from E2:E1"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_E2_2,_T("blackPawn MD_DOWNDIAG2 from E2:F1"))
};
#endif
static const DirectionArray blackPawnMoves_E2(3,blackPawnMoveDirections_E2);

static const int blackPawnMoves_F2_0[2] = { 1,E1 };
static const int blackPawnMoves_F2_1[2] = { 1,F1 };
static const int blackPawnMoves_F2_2[2] = { 1,G1 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_F2[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_F2_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_F2_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_F2_2)
};
#else
static const Direction blackPawnMoveDirections_F2[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_F2_0,_T("blackPawn MD_DOWNDIAG1 from F2:E1"))
 ,Direction(MD_DOWN     ,blackPawnMoves_F2_1,_T("blackPawn MD_DOWN from F2:F1"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_F2_2,_T("blackPawn MD_DOWNDIAG2 from F2:G1"))
};
#endif
static const DirectionArray blackPawnMoves_F2(3,blackPawnMoveDirections_F2);

static const int blackPawnMoves_G2_0[2] = { 1,F1 };
static const int blackPawnMoves_G2_1[2] = { 1,G1 };
static const int blackPawnMoves_G2_2[2] = { 1,H1 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_G2[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_G2_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_G2_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_G2_2)
};
#else
static const Direction blackPawnMoveDirections_G2[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_G2_0,_T("blackPawn MD_DOWNDIAG1 from G2:F1"))
 ,Direction(MD_DOWN     ,blackPawnMoves_G2_1,_T("blackPawn MD_DOWN from G2:G1"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_G2_2,_T("blackPawn MD_DOWNDIAG2 from G2:H1"))
};
#endif
static const DirectionArray blackPawnMoves_G2(3,blackPawnMoveDirections_G2);

static const int blackPawnMoves_H2_0[2] = { 1,G1 };
static const int blackPawnMoves_H2_1[2] = { 1,H1 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_H2[2] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_H2_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_H2_1)
};
#else
static const Direction blackPawnMoveDirections_H2[2] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_H2_0,_T("blackPawn MD_DOWNDIAG1 from H2:G1"))
 ,Direction(MD_DOWN     ,blackPawnMoves_H2_1,_T("blackPawn MD_DOWN from H2:H1"))
};
#endif
static const DirectionArray blackPawnMoves_H2(2,blackPawnMoveDirections_H2);

static const int blackPawnMoves_A3_0[2] = { 1,A2 };
static const int blackPawnMoves_A3_1[2] = { 1,B2 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_A3[2] = {
  Direction(MD_DOWN     ,blackPawnMoves_A3_0)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_A3_1)
};
#else
static const Direction blackPawnMoveDirections_A3[2] = {
  Direction(MD_DOWN     ,blackPawnMoves_A3_0,_T("blackPawn MD_DOWN from A3:A2"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_A3_1,_T("blackPawn MD_DOWNDIAG2 from A3:B2"))
};
#endif
static const DirectionArray blackPawnMoves_A3(2,blackPawnMoveDirections_A3);

static const int blackPawnMoves_B3_0[2] = { 1,A2 };
static const int blackPawnMoves_B3_1[2] = { 1,B2 };
static const int blackPawnMoves_B3_2[2] = { 1,C2 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_B3[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_B3_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_B3_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_B3_2)
};
#else
static const Direction blackPawnMoveDirections_B3[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_B3_0,_T("blackPawn MD_DOWNDIAG1 from B3:A2"))
 ,Direction(MD_DOWN     ,blackPawnMoves_B3_1,_T("blackPawn MD_DOWN from B3:B2"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_B3_2,_T("blackPawn MD_DOWNDIAG2 from B3:C2"))
};
#endif
static const DirectionArray blackPawnMoves_B3(3,blackPawnMoveDirections_B3);

static const int blackPawnMoves_C3_0[2] = { 1,B2 };
static const int blackPawnMoves_C3_1[2] = { 1,C2 };
static const int blackPawnMoves_C3_2[2] = { 1,D2 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_C3[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_C3_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_C3_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_C3_2)
};
#else
static const Direction blackPawnMoveDirections_C3[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_C3_0,_T("blackPawn MD_DOWNDIAG1 from C3:B2"))
 ,Direction(MD_DOWN     ,blackPawnMoves_C3_1,_T("blackPawn MD_DOWN from C3:C2"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_C3_2,_T("blackPawn MD_DOWNDIAG2 from C3:D2"))
};
#endif
static const DirectionArray blackPawnMoves_C3(3,blackPawnMoveDirections_C3);

static const int blackPawnMoves_D3_0[2] = { 1,C2 };
static const int blackPawnMoves_D3_1[2] = { 1,D2 };
static const int blackPawnMoves_D3_2[2] = { 1,E2 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_D3[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_D3_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_D3_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_D3_2)
};
#else
static const Direction blackPawnMoveDirections_D3[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_D3_0,_T("blackPawn MD_DOWNDIAG1 from D3:C2"))
 ,Direction(MD_DOWN     ,blackPawnMoves_D3_1,_T("blackPawn MD_DOWN from D3:D2"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_D3_2,_T("blackPawn MD_DOWNDIAG2 from D3:E2"))
};
#endif
static const DirectionArray blackPawnMoves_D3(3,blackPawnMoveDirections_D3);

static const int blackPawnMoves_E3_0[2] = { 1,D2 };
static const int blackPawnMoves_E3_1[2] = { 1,E2 };
static const int blackPawnMoves_E3_2[2] = { 1,F2 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_E3[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_E3_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_E3_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_E3_2)
};
#else
static const Direction blackPawnMoveDirections_E3[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_E3_0,_T("blackPawn MD_DOWNDIAG1 from E3:D2"))
 ,Direction(MD_DOWN     ,blackPawnMoves_E3_1,_T("blackPawn MD_DOWN from E3:E2"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_E3_2,_T("blackPawn MD_DOWNDIAG2 from E3:F2"))
};
#endif
static const DirectionArray blackPawnMoves_E3(3,blackPawnMoveDirections_E3);

static const int blackPawnMoves_F3_0[2] = { 1,E2 };
static const int blackPawnMoves_F3_1[2] = { 1,F2 };
static const int blackPawnMoves_F3_2[2] = { 1,G2 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_F3[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_F3_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_F3_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_F3_2)
};
#else
static const Direction blackPawnMoveDirections_F3[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_F3_0,_T("blackPawn MD_DOWNDIAG1 from F3:E2"))
 ,Direction(MD_DOWN     ,blackPawnMoves_F3_1,_T("blackPawn MD_DOWN from F3:F2"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_F3_2,_T("blackPawn MD_DOWNDIAG2 from F3:G2"))
};
#endif
static const DirectionArray blackPawnMoves_F3(3,blackPawnMoveDirections_F3);

static const int blackPawnMoves_G3_0[2] = { 1,F2 };
static const int blackPawnMoves_G3_1[2] = { 1,G2 };
static const int blackPawnMoves_G3_2[2] = { 1,H2 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_G3[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_G3_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_G3_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_G3_2)
};
#else
static const Direction blackPawnMoveDirections_G3[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_G3_0,_T("blackPawn MD_DOWNDIAG1 from G3:F2"))
 ,Direction(MD_DOWN     ,blackPawnMoves_G3_1,_T("blackPawn MD_DOWN from G3:G2"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_G3_2,_T("blackPawn MD_DOWNDIAG2 from G3:H2"))
};
#endif
static const DirectionArray blackPawnMoves_G3(3,blackPawnMoveDirections_G3);

static const int blackPawnMoves_H3_0[2] = { 1,G2 };
static const int blackPawnMoves_H3_1[2] = { 1,H2 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_H3[2] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_H3_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_H3_1)
};
#else
static const Direction blackPawnMoveDirections_H3[2] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_H3_0,_T("blackPawn MD_DOWNDIAG1 from H3:G2"))
 ,Direction(MD_DOWN     ,blackPawnMoves_H3_1,_T("blackPawn MD_DOWN from H3:H2"))
};
#endif
static const DirectionArray blackPawnMoves_H3(2,blackPawnMoveDirections_H3);

static const int blackPawnMoves_A4_0[2] = { 1,A3 };
static const int blackPawnMoves_A4_1[2] = { 1,B3 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_A4[2] = {
  Direction(MD_DOWN     ,blackPawnMoves_A4_0)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_A4_1)
};
#else
static const Direction blackPawnMoveDirections_A4[2] = {
  Direction(MD_DOWN     ,blackPawnMoves_A4_0,_T("blackPawn MD_DOWN from A4:A3"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_A4_1,_T("blackPawn MD_DOWNDIAG2 from A4:B3"))
};
#endif
static const DirectionArray blackPawnMoves_A4(2,blackPawnMoveDirections_A4);

static const int blackPawnMoves_B4_0[2] = { 1,A3 };
static const int blackPawnMoves_B4_1[2] = { 1,B3 };
static const int blackPawnMoves_B4_2[2] = { 1,C3 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_B4[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_B4_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_B4_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_B4_2)
};
#else
static const Direction blackPawnMoveDirections_B4[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_B4_0,_T("blackPawn MD_DOWNDIAG1 from B4:A3"))
 ,Direction(MD_DOWN     ,blackPawnMoves_B4_1,_T("blackPawn MD_DOWN from B4:B3"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_B4_2,_T("blackPawn MD_DOWNDIAG2 from B4:C3"))
};
#endif
static const DirectionArray blackPawnMoves_B4(3,blackPawnMoveDirections_B4);

static const int blackPawnMoves_C4_0[2] = { 1,B3 };
static const int blackPawnMoves_C4_1[2] = { 1,C3 };
static const int blackPawnMoves_C4_2[2] = { 1,D3 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_C4[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_C4_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_C4_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_C4_2)
};
#else
static const Direction blackPawnMoveDirections_C4[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_C4_0,_T("blackPawn MD_DOWNDIAG1 from C4:B3"))
 ,Direction(MD_DOWN     ,blackPawnMoves_C4_1,_T("blackPawn MD_DOWN from C4:C3"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_C4_2,_T("blackPawn MD_DOWNDIAG2 from C4:D3"))
};
#endif
static const DirectionArray blackPawnMoves_C4(3,blackPawnMoveDirections_C4);

static const int blackPawnMoves_D4_0[2] = { 1,C3 };
static const int blackPawnMoves_D4_1[2] = { 1,D3 };
static const int blackPawnMoves_D4_2[2] = { 1,E3 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_D4[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_D4_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_D4_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_D4_2)
};
#else
static const Direction blackPawnMoveDirections_D4[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_D4_0,_T("blackPawn MD_DOWNDIAG1 from D4:C3"))
 ,Direction(MD_DOWN     ,blackPawnMoves_D4_1,_T("blackPawn MD_DOWN from D4:D3"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_D4_2,_T("blackPawn MD_DOWNDIAG2 from D4:E3"))
};
#endif
static const DirectionArray blackPawnMoves_D4(3,blackPawnMoveDirections_D4);

static const int blackPawnMoves_E4_0[2] = { 1,D3 };
static const int blackPawnMoves_E4_1[2] = { 1,E3 };
static const int blackPawnMoves_E4_2[2] = { 1,F3 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_E4[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_E4_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_E4_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_E4_2)
};
#else
static const Direction blackPawnMoveDirections_E4[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_E4_0,_T("blackPawn MD_DOWNDIAG1 from E4:D3"))
 ,Direction(MD_DOWN     ,blackPawnMoves_E4_1,_T("blackPawn MD_DOWN from E4:E3"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_E4_2,_T("blackPawn MD_DOWNDIAG2 from E4:F3"))
};
#endif
static const DirectionArray blackPawnMoves_E4(3,blackPawnMoveDirections_E4);

static const int blackPawnMoves_F4_0[2] = { 1,E3 };
static const int blackPawnMoves_F4_1[2] = { 1,F3 };
static const int blackPawnMoves_F4_2[2] = { 1,G3 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_F4[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_F4_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_F4_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_F4_2)
};
#else
static const Direction blackPawnMoveDirections_F4[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_F4_0,_T("blackPawn MD_DOWNDIAG1 from F4:E3"))
 ,Direction(MD_DOWN     ,blackPawnMoves_F4_1,_T("blackPawn MD_DOWN from F4:F3"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_F4_2,_T("blackPawn MD_DOWNDIAG2 from F4:G3"))
};
#endif
static const DirectionArray blackPawnMoves_F4(3,blackPawnMoveDirections_F4);

static const int blackPawnMoves_G4_0[2] = { 1,F3 };
static const int blackPawnMoves_G4_1[2] = { 1,G3 };
static const int blackPawnMoves_G4_2[2] = { 1,H3 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_G4[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_G4_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_G4_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_G4_2)
};
#else
static const Direction blackPawnMoveDirections_G4[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_G4_0,_T("blackPawn MD_DOWNDIAG1 from G4:F3"))
 ,Direction(MD_DOWN     ,blackPawnMoves_G4_1,_T("blackPawn MD_DOWN from G4:G3"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_G4_2,_T("blackPawn MD_DOWNDIAG2 from G4:H3"))
};
#endif
static const DirectionArray blackPawnMoves_G4(3,blackPawnMoveDirections_G4);

static const int blackPawnMoves_H4_0[2] = { 1,G3 };
static const int blackPawnMoves_H4_1[2] = { 1,H3 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_H4[2] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_H4_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_H4_1)
};
#else
static const Direction blackPawnMoveDirections_H4[2] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_H4_0,_T("blackPawn MD_DOWNDIAG1 from H4:G3"))
 ,Direction(MD_DOWN     ,blackPawnMoves_H4_1,_T("blackPawn MD_DOWN from H4:H3"))
};
#endif
static const DirectionArray blackPawnMoves_H4(2,blackPawnMoveDirections_H4);

static const int blackPawnMoves_A5_0[2] = { 1,A4 };
static const int blackPawnMoves_A5_1[2] = { 1,B4 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_A5[2] = {
  Direction(MD_DOWN     ,blackPawnMoves_A5_0)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_A5_1)
};
#else
static const Direction blackPawnMoveDirections_A5[2] = {
  Direction(MD_DOWN     ,blackPawnMoves_A5_0,_T("blackPawn MD_DOWN from A5:A4"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_A5_1,_T("blackPawn MD_DOWNDIAG2 from A5:B4"))
};
#endif
static const DirectionArray blackPawnMoves_A5(2,blackPawnMoveDirections_A5);

static const int blackPawnMoves_B5_0[2] = { 1,A4 };
static const int blackPawnMoves_B5_1[2] = { 1,B4 };
static const int blackPawnMoves_B5_2[2] = { 1,C4 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_B5[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_B5_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_B5_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_B5_2)
};
#else
static const Direction blackPawnMoveDirections_B5[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_B5_0,_T("blackPawn MD_DOWNDIAG1 from B5:A4"))
 ,Direction(MD_DOWN     ,blackPawnMoves_B5_1,_T("blackPawn MD_DOWN from B5:B4"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_B5_2,_T("blackPawn MD_DOWNDIAG2 from B5:C4"))
};
#endif
static const DirectionArray blackPawnMoves_B5(3,blackPawnMoveDirections_B5);

static const int blackPawnMoves_C5_0[2] = { 1,B4 };
static const int blackPawnMoves_C5_1[2] = { 1,C4 };
static const int blackPawnMoves_C5_2[2] = { 1,D4 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_C5[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_C5_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_C5_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_C5_2)
};
#else
static const Direction blackPawnMoveDirections_C5[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_C5_0,_T("blackPawn MD_DOWNDIAG1 from C5:B4"))
 ,Direction(MD_DOWN     ,blackPawnMoves_C5_1,_T("blackPawn MD_DOWN from C5:C4"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_C5_2,_T("blackPawn MD_DOWNDIAG2 from C5:D4"))
};
#endif
static const DirectionArray blackPawnMoves_C5(3,blackPawnMoveDirections_C5);

static const int blackPawnMoves_D5_0[2] = { 1,C4 };
static const int blackPawnMoves_D5_1[2] = { 1,D4 };
static const int blackPawnMoves_D5_2[2] = { 1,E4 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_D5[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_D5_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_D5_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_D5_2)
};
#else
static const Direction blackPawnMoveDirections_D5[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_D5_0,_T("blackPawn MD_DOWNDIAG1 from D5:C4"))
 ,Direction(MD_DOWN     ,blackPawnMoves_D5_1,_T("blackPawn MD_DOWN from D5:D4"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_D5_2,_T("blackPawn MD_DOWNDIAG2 from D5:E4"))
};
#endif
static const DirectionArray blackPawnMoves_D5(3,blackPawnMoveDirections_D5);

static const int blackPawnMoves_E5_0[2] = { 1,D4 };
static const int blackPawnMoves_E5_1[2] = { 1,E4 };
static const int blackPawnMoves_E5_2[2] = { 1,F4 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_E5[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_E5_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_E5_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_E5_2)
};
#else
static const Direction blackPawnMoveDirections_E5[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_E5_0,_T("blackPawn MD_DOWNDIAG1 from E5:D4"))
 ,Direction(MD_DOWN     ,blackPawnMoves_E5_1,_T("blackPawn MD_DOWN from E5:E4"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_E5_2,_T("blackPawn MD_DOWNDIAG2 from E5:F4"))
};
#endif
static const DirectionArray blackPawnMoves_E5(3,blackPawnMoveDirections_E5);

static const int blackPawnMoves_F5_0[2] = { 1,E4 };
static const int blackPawnMoves_F5_1[2] = { 1,F4 };
static const int blackPawnMoves_F5_2[2] = { 1,G4 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_F5[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_F5_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_F5_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_F5_2)
};
#else
static const Direction blackPawnMoveDirections_F5[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_F5_0,_T("blackPawn MD_DOWNDIAG1 from F5:E4"))
 ,Direction(MD_DOWN     ,blackPawnMoves_F5_1,_T("blackPawn MD_DOWN from F5:F4"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_F5_2,_T("blackPawn MD_DOWNDIAG2 from F5:G4"))
};
#endif
static const DirectionArray blackPawnMoves_F5(3,blackPawnMoveDirections_F5);

static const int blackPawnMoves_G5_0[2] = { 1,F4 };
static const int blackPawnMoves_G5_1[2] = { 1,G4 };
static const int blackPawnMoves_G5_2[2] = { 1,H4 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_G5[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_G5_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_G5_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_G5_2)
};
#else
static const Direction blackPawnMoveDirections_G5[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_G5_0,_T("blackPawn MD_DOWNDIAG1 from G5:F4"))
 ,Direction(MD_DOWN     ,blackPawnMoves_G5_1,_T("blackPawn MD_DOWN from G5:G4"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_G5_2,_T("blackPawn MD_DOWNDIAG2 from G5:H4"))
};
#endif
static const DirectionArray blackPawnMoves_G5(3,blackPawnMoveDirections_G5);

static const int blackPawnMoves_H5_0[2] = { 1,G4 };
static const int blackPawnMoves_H5_1[2] = { 1,H4 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_H5[2] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_H5_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_H5_1)
};
#else
static const Direction blackPawnMoveDirections_H5[2] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_H5_0,_T("blackPawn MD_DOWNDIAG1 from H5:G4"))
 ,Direction(MD_DOWN     ,blackPawnMoves_H5_1,_T("blackPawn MD_DOWN from H5:H4"))
};
#endif
static const DirectionArray blackPawnMoves_H5(2,blackPawnMoveDirections_H5);

static const int blackPawnMoves_A6_0[2] = { 1,A5 };
static const int blackPawnMoves_A6_1[2] = { 1,B5 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_A6[2] = {
  Direction(MD_DOWN     ,blackPawnMoves_A6_0)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_A6_1)
};
#else
static const Direction blackPawnMoveDirections_A6[2] = {
  Direction(MD_DOWN     ,blackPawnMoves_A6_0,_T("blackPawn MD_DOWN from A6:A5"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_A6_1,_T("blackPawn MD_DOWNDIAG2 from A6:B5"))
};
#endif
static const DirectionArray blackPawnMoves_A6(2,blackPawnMoveDirections_A6);

static const int blackPawnMoves_B6_0[2] = { 1,A5 };
static const int blackPawnMoves_B6_1[2] = { 1,B5 };
static const int blackPawnMoves_B6_2[2] = { 1,C5 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_B6[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_B6_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_B6_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_B6_2)
};
#else
static const Direction blackPawnMoveDirections_B6[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_B6_0,_T("blackPawn MD_DOWNDIAG1 from B6:A5"))
 ,Direction(MD_DOWN     ,blackPawnMoves_B6_1,_T("blackPawn MD_DOWN from B6:B5"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_B6_2,_T("blackPawn MD_DOWNDIAG2 from B6:C5"))
};
#endif
static const DirectionArray blackPawnMoves_B6(3,blackPawnMoveDirections_B6);

static const int blackPawnMoves_C6_0[2] = { 1,B5 };
static const int blackPawnMoves_C6_1[2] = { 1,C5 };
static const int blackPawnMoves_C6_2[2] = { 1,D5 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_C6[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_C6_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_C6_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_C6_2)
};
#else
static const Direction blackPawnMoveDirections_C6[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_C6_0,_T("blackPawn MD_DOWNDIAG1 from C6:B5"))
 ,Direction(MD_DOWN     ,blackPawnMoves_C6_1,_T("blackPawn MD_DOWN from C6:C5"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_C6_2,_T("blackPawn MD_DOWNDIAG2 from C6:D5"))
};
#endif
static const DirectionArray blackPawnMoves_C6(3,blackPawnMoveDirections_C6);

static const int blackPawnMoves_D6_0[2] = { 1,C5 };
static const int blackPawnMoves_D6_1[2] = { 1,D5 };
static const int blackPawnMoves_D6_2[2] = { 1,E5 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_D6[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_D6_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_D6_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_D6_2)
};
#else
static const Direction blackPawnMoveDirections_D6[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_D6_0,_T("blackPawn MD_DOWNDIAG1 from D6:C5"))
 ,Direction(MD_DOWN     ,blackPawnMoves_D6_1,_T("blackPawn MD_DOWN from D6:D5"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_D6_2,_T("blackPawn MD_DOWNDIAG2 from D6:E5"))
};
#endif
static const DirectionArray blackPawnMoves_D6(3,blackPawnMoveDirections_D6);

static const int blackPawnMoves_E6_0[2] = { 1,D5 };
static const int blackPawnMoves_E6_1[2] = { 1,E5 };
static const int blackPawnMoves_E6_2[2] = { 1,F5 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_E6[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_E6_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_E6_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_E6_2)
};
#else
static const Direction blackPawnMoveDirections_E6[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_E6_0,_T("blackPawn MD_DOWNDIAG1 from E6:D5"))
 ,Direction(MD_DOWN     ,blackPawnMoves_E6_1,_T("blackPawn MD_DOWN from E6:E5"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_E6_2,_T("blackPawn MD_DOWNDIAG2 from E6:F5"))
};
#endif
static const DirectionArray blackPawnMoves_E6(3,blackPawnMoveDirections_E6);

static const int blackPawnMoves_F6_0[2] = { 1,E5 };
static const int blackPawnMoves_F6_1[2] = { 1,F5 };
static const int blackPawnMoves_F6_2[2] = { 1,G5 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_F6[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_F6_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_F6_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_F6_2)
};
#else
static const Direction blackPawnMoveDirections_F6[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_F6_0,_T("blackPawn MD_DOWNDIAG1 from F6:E5"))
 ,Direction(MD_DOWN     ,blackPawnMoves_F6_1,_T("blackPawn MD_DOWN from F6:F5"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_F6_2,_T("blackPawn MD_DOWNDIAG2 from F6:G5"))
};
#endif
static const DirectionArray blackPawnMoves_F6(3,blackPawnMoveDirections_F6);

static const int blackPawnMoves_G6_0[2] = { 1,F5 };
static const int blackPawnMoves_G6_1[2] = { 1,G5 };
static const int blackPawnMoves_G6_2[2] = { 1,H5 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_G6[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_G6_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_G6_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_G6_2)
};
#else
static const Direction blackPawnMoveDirections_G6[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_G6_0,_T("blackPawn MD_DOWNDIAG1 from G6:F5"))
 ,Direction(MD_DOWN     ,blackPawnMoves_G6_1,_T("blackPawn MD_DOWN from G6:G5"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_G6_2,_T("blackPawn MD_DOWNDIAG2 from G6:H5"))
};
#endif
static const DirectionArray blackPawnMoves_G6(3,blackPawnMoveDirections_G6);

static const int blackPawnMoves_H6_0[2] = { 1,G5 };
static const int blackPawnMoves_H6_1[2] = { 1,H5 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_H6[2] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_H6_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_H6_1)
};
#else
static const Direction blackPawnMoveDirections_H6[2] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_H6_0,_T("blackPawn MD_DOWNDIAG1 from H6:G5"))
 ,Direction(MD_DOWN     ,blackPawnMoves_H6_1,_T("blackPawn MD_DOWN from H6:H5"))
};
#endif
static const DirectionArray blackPawnMoves_H6(2,blackPawnMoveDirections_H6);

static const int blackPawnMoves_A7_0[3] = { 2,A6,A5 };
static const int blackPawnMoves_A7_1[2] = { 1,B6 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_A7[2] = {
  Direction(MD_DOWN     ,blackPawnMoves_A7_0)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_A7_1)
};
#else
static const Direction blackPawnMoveDirections_A7[2] = {
  Direction(MD_DOWN     ,blackPawnMoves_A7_0,_T("blackPawn MD_DOWN from A7:A6 A5"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_A7_1,_T("blackPawn MD_DOWNDIAG2 from A7:B6"))
};
#endif
static const DirectionArray blackPawnMoves_A7(2,blackPawnMoveDirections_A7);

static const int blackPawnMoves_B7_0[2] = { 1,A6 };
static const int blackPawnMoves_B7_1[3] = { 2,B6,B5 };
static const int blackPawnMoves_B7_2[2] = { 1,C6 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_B7[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_B7_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_B7_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_B7_2)
};
#else
static const Direction blackPawnMoveDirections_B7[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_B7_0,_T("blackPawn MD_DOWNDIAG1 from B7:A6"))
 ,Direction(MD_DOWN     ,blackPawnMoves_B7_1,_T("blackPawn MD_DOWN from B7:B6 B5"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_B7_2,_T("blackPawn MD_DOWNDIAG2 from B7:C6"))
};
#endif
static const DirectionArray blackPawnMoves_B7(3,blackPawnMoveDirections_B7);

static const int blackPawnMoves_C7_0[2] = { 1,B6 };
static const int blackPawnMoves_C7_1[3] = { 2,C6,C5 };
static const int blackPawnMoves_C7_2[2] = { 1,D6 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_C7[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_C7_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_C7_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_C7_2)
};
#else
static const Direction blackPawnMoveDirections_C7[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_C7_0,_T("blackPawn MD_DOWNDIAG1 from C7:B6"))
 ,Direction(MD_DOWN     ,blackPawnMoves_C7_1,_T("blackPawn MD_DOWN from C7:C6 C5"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_C7_2,_T("blackPawn MD_DOWNDIAG2 from C7:D6"))
};
#endif
static const DirectionArray blackPawnMoves_C7(3,blackPawnMoveDirections_C7);

static const int blackPawnMoves_D7_0[2] = { 1,C6 };
static const int blackPawnMoves_D7_1[3] = { 2,D6,D5 };
static const int blackPawnMoves_D7_2[2] = { 1,E6 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_D7[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_D7_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_D7_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_D7_2)
};
#else
static const Direction blackPawnMoveDirections_D7[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_D7_0,_T("blackPawn MD_DOWNDIAG1 from D7:C6"))
 ,Direction(MD_DOWN     ,blackPawnMoves_D7_1,_T("blackPawn MD_DOWN from D7:D6 D5"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_D7_2,_T("blackPawn MD_DOWNDIAG2 from D7:E6"))
};
#endif
static const DirectionArray blackPawnMoves_D7(3,blackPawnMoveDirections_D7);

static const int blackPawnMoves_E7_0[2] = { 1,D6 };
static const int blackPawnMoves_E7_1[3] = { 2,E6,E5 };
static const int blackPawnMoves_E7_2[2] = { 1,F6 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_E7[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_E7_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_E7_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_E7_2)
};
#else
static const Direction blackPawnMoveDirections_E7[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_E7_0,_T("blackPawn MD_DOWNDIAG1 from E7:D6"))
 ,Direction(MD_DOWN     ,blackPawnMoves_E7_1,_T("blackPawn MD_DOWN from E7:E6 E5"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_E7_2,_T("blackPawn MD_DOWNDIAG2 from E7:F6"))
};
#endif
static const DirectionArray blackPawnMoves_E7(3,blackPawnMoveDirections_E7);

static const int blackPawnMoves_F7_0[2] = { 1,E6 };
static const int blackPawnMoves_F7_1[3] = { 2,F6,F5 };
static const int blackPawnMoves_F7_2[2] = { 1,G6 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_F7[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_F7_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_F7_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_F7_2)
};
#else
static const Direction blackPawnMoveDirections_F7[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_F7_0,_T("blackPawn MD_DOWNDIAG1 from F7:E6"))
 ,Direction(MD_DOWN     ,blackPawnMoves_F7_1,_T("blackPawn MD_DOWN from F7:F6 F5"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_F7_2,_T("blackPawn MD_DOWNDIAG2 from F7:G6"))
};
#endif
static const DirectionArray blackPawnMoves_F7(3,blackPawnMoveDirections_F7);

static const int blackPawnMoves_G7_0[2] = { 1,F6 };
static const int blackPawnMoves_G7_1[3] = { 2,G6,G5 };
static const int blackPawnMoves_G7_2[2] = { 1,H6 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_G7[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_G7_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_G7_1)
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_G7_2)
};
#else
static const Direction blackPawnMoveDirections_G7[3] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_G7_0,_T("blackPawn MD_DOWNDIAG1 from G7:F6"))
 ,Direction(MD_DOWN     ,blackPawnMoves_G7_1,_T("blackPawn MD_DOWN from G7:G6 G5"))
 ,Direction(MD_DOWNDIAG2,blackPawnMoves_G7_2,_T("blackPawn MD_DOWNDIAG2 from G7:H6"))
};
#endif
static const DirectionArray blackPawnMoves_G7(3,blackPawnMoveDirections_G7);

static const int blackPawnMoves_H7_0[2] = { 1,G6 };
static const int blackPawnMoves_H7_1[3] = { 2,H6,H5 };
#ifndef _DEBUG
static const Direction blackPawnMoveDirections_H7[2] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_H7_0)
 ,Direction(MD_DOWN     ,blackPawnMoves_H7_1)
};
#else
static const Direction blackPawnMoveDirections_H7[2] = {
  Direction(MD_DOWNDIAG1,blackPawnMoves_H7_0,_T("blackPawn MD_DOWNDIAG1 from H7:G6"))
 ,Direction(MD_DOWN     ,blackPawnMoves_H7_1,_T("blackPawn MD_DOWN from H7:H6 H5"))
};
#endif
static const DirectionArray blackPawnMoves_H7(2,blackPawnMoveDirections_H7);

static const DirectionArray blackPawnMoves_A8(0,NULL);

static const DirectionArray blackPawnMoves_B8(0,NULL);

static const DirectionArray blackPawnMoves_C8(0,NULL);

static const DirectionArray blackPawnMoves_D8(0,NULL);

static const DirectionArray blackPawnMoves_E8(0,NULL);

static const DirectionArray blackPawnMoves_F8(0,NULL);

static const DirectionArray blackPawnMoves_G8(0,NULL);

static const DirectionArray blackPawnMoves_H8(0,NULL);


const DirectionArray MoveTable::kingMoves[64] = {
  kingMoves_A1 ,kingMoves_B1 ,kingMoves_C1 ,kingMoves_D1 ,kingMoves_E1 ,kingMoves_F1 ,kingMoves_G1 ,kingMoves_H1
 ,kingMoves_A2 ,kingMoves_B2 ,kingMoves_C2 ,kingMoves_D2 ,kingMoves_E2 ,kingMoves_F2 ,kingMoves_G2 ,kingMoves_H2
 ,kingMoves_A3 ,kingMoves_B3 ,kingMoves_C3 ,kingMoves_D3 ,kingMoves_E3 ,kingMoves_F3 ,kingMoves_G3 ,kingMoves_H3
 ,kingMoves_A4 ,kingMoves_B4 ,kingMoves_C4 ,kingMoves_D4 ,kingMoves_E4 ,kingMoves_F4 ,kingMoves_G4 ,kingMoves_H4
 ,kingMoves_A5 ,kingMoves_B5 ,kingMoves_C5 ,kingMoves_D5 ,kingMoves_E5 ,kingMoves_F5 ,kingMoves_G5 ,kingMoves_H5
 ,kingMoves_A6 ,kingMoves_B6 ,kingMoves_C6 ,kingMoves_D6 ,kingMoves_E6 ,kingMoves_F6 ,kingMoves_G6 ,kingMoves_H6
 ,kingMoves_A7 ,kingMoves_B7 ,kingMoves_C7 ,kingMoves_D7 ,kingMoves_E7 ,kingMoves_F7 ,kingMoves_G7 ,kingMoves_H7
 ,kingMoves_A8 ,kingMoves_B8 ,kingMoves_C8 ,kingMoves_D8 ,kingMoves_E8 ,kingMoves_F8 ,kingMoves_G8 ,kingMoves_H8
};

const DirectionArray MoveTable::queenMoves[64] = {
  queenMoves_A1 ,queenMoves_B1 ,queenMoves_C1 ,queenMoves_D1 ,queenMoves_E1 ,queenMoves_F1 ,queenMoves_G1 ,queenMoves_H1
 ,queenMoves_A2 ,queenMoves_B2 ,queenMoves_C2 ,queenMoves_D2 ,queenMoves_E2 ,queenMoves_F2 ,queenMoves_G2 ,queenMoves_H2
 ,queenMoves_A3 ,queenMoves_B3 ,queenMoves_C3 ,queenMoves_D3 ,queenMoves_E3 ,queenMoves_F3 ,queenMoves_G3 ,queenMoves_H3
 ,queenMoves_A4 ,queenMoves_B4 ,queenMoves_C4 ,queenMoves_D4 ,queenMoves_E4 ,queenMoves_F4 ,queenMoves_G4 ,queenMoves_H4
 ,queenMoves_A5 ,queenMoves_B5 ,queenMoves_C5 ,queenMoves_D5 ,queenMoves_E5 ,queenMoves_F5 ,queenMoves_G5 ,queenMoves_H5
 ,queenMoves_A6 ,queenMoves_B6 ,queenMoves_C6 ,queenMoves_D6 ,queenMoves_E6 ,queenMoves_F6 ,queenMoves_G6 ,queenMoves_H6
 ,queenMoves_A7 ,queenMoves_B7 ,queenMoves_C7 ,queenMoves_D7 ,queenMoves_E7 ,queenMoves_F7 ,queenMoves_G7 ,queenMoves_H7
 ,queenMoves_A8 ,queenMoves_B8 ,queenMoves_C8 ,queenMoves_D8 ,queenMoves_E8 ,queenMoves_F8 ,queenMoves_G8 ,queenMoves_H8
};

const DirectionArray MoveTable::rookMoves[64] = {
  rookMoves_A1 ,rookMoves_B1 ,rookMoves_C1 ,rookMoves_D1 ,rookMoves_E1 ,rookMoves_F1 ,rookMoves_G1 ,rookMoves_H1
 ,rookMoves_A2 ,rookMoves_B2 ,rookMoves_C2 ,rookMoves_D2 ,rookMoves_E2 ,rookMoves_F2 ,rookMoves_G2 ,rookMoves_H2
 ,rookMoves_A3 ,rookMoves_B3 ,rookMoves_C3 ,rookMoves_D3 ,rookMoves_E3 ,rookMoves_F3 ,rookMoves_G3 ,rookMoves_H3
 ,rookMoves_A4 ,rookMoves_B4 ,rookMoves_C4 ,rookMoves_D4 ,rookMoves_E4 ,rookMoves_F4 ,rookMoves_G4 ,rookMoves_H4
 ,rookMoves_A5 ,rookMoves_B5 ,rookMoves_C5 ,rookMoves_D5 ,rookMoves_E5 ,rookMoves_F5 ,rookMoves_G5 ,rookMoves_H5
 ,rookMoves_A6 ,rookMoves_B6 ,rookMoves_C6 ,rookMoves_D6 ,rookMoves_E6 ,rookMoves_F6 ,rookMoves_G6 ,rookMoves_H6
 ,rookMoves_A7 ,rookMoves_B7 ,rookMoves_C7 ,rookMoves_D7 ,rookMoves_E7 ,rookMoves_F7 ,rookMoves_G7 ,rookMoves_H7
 ,rookMoves_A8 ,rookMoves_B8 ,rookMoves_C8 ,rookMoves_D8 ,rookMoves_E8 ,rookMoves_F8 ,rookMoves_G8 ,rookMoves_H8
};

const DirectionArray MoveTable::bishopMoves[64] = {
  bishopMoves_A1 ,bishopMoves_B1 ,bishopMoves_C1 ,bishopMoves_D1 ,bishopMoves_E1 ,bishopMoves_F1 ,bishopMoves_G1 ,bishopMoves_H1
 ,bishopMoves_A2 ,bishopMoves_B2 ,bishopMoves_C2 ,bishopMoves_D2 ,bishopMoves_E2 ,bishopMoves_F2 ,bishopMoves_G2 ,bishopMoves_H2
 ,bishopMoves_A3 ,bishopMoves_B3 ,bishopMoves_C3 ,bishopMoves_D3 ,bishopMoves_E3 ,bishopMoves_F3 ,bishopMoves_G3 ,bishopMoves_H3
 ,bishopMoves_A4 ,bishopMoves_B4 ,bishopMoves_C4 ,bishopMoves_D4 ,bishopMoves_E4 ,bishopMoves_F4 ,bishopMoves_G4 ,bishopMoves_H4
 ,bishopMoves_A5 ,bishopMoves_B5 ,bishopMoves_C5 ,bishopMoves_D5 ,bishopMoves_E5 ,bishopMoves_F5 ,bishopMoves_G5 ,bishopMoves_H5
 ,bishopMoves_A6 ,bishopMoves_B6 ,bishopMoves_C6 ,bishopMoves_D6 ,bishopMoves_E6 ,bishopMoves_F6 ,bishopMoves_G6 ,bishopMoves_H6
 ,bishopMoves_A7 ,bishopMoves_B7 ,bishopMoves_C7 ,bishopMoves_D7 ,bishopMoves_E7 ,bishopMoves_F7 ,bishopMoves_G7 ,bishopMoves_H7
 ,bishopMoves_A8 ,bishopMoves_B8 ,bishopMoves_C8 ,bishopMoves_D8 ,bishopMoves_E8 ,bishopMoves_F8 ,bishopMoves_G8 ,bishopMoves_H8
};

const DirectionArray MoveTable::knightMoves[64] = {
  knightMoves_A1 ,knightMoves_B1 ,knightMoves_C1 ,knightMoves_D1 ,knightMoves_E1 ,knightMoves_F1 ,knightMoves_G1 ,knightMoves_H1
 ,knightMoves_A2 ,knightMoves_B2 ,knightMoves_C2 ,knightMoves_D2 ,knightMoves_E2 ,knightMoves_F2 ,knightMoves_G2 ,knightMoves_H2
 ,knightMoves_A3 ,knightMoves_B3 ,knightMoves_C3 ,knightMoves_D3 ,knightMoves_E3 ,knightMoves_F3 ,knightMoves_G3 ,knightMoves_H3
 ,knightMoves_A4 ,knightMoves_B4 ,knightMoves_C4 ,knightMoves_D4 ,knightMoves_E4 ,knightMoves_F4 ,knightMoves_G4 ,knightMoves_H4
 ,knightMoves_A5 ,knightMoves_B5 ,knightMoves_C5 ,knightMoves_D5 ,knightMoves_E5 ,knightMoves_F5 ,knightMoves_G5 ,knightMoves_H5
 ,knightMoves_A6 ,knightMoves_B6 ,knightMoves_C6 ,knightMoves_D6 ,knightMoves_E6 ,knightMoves_F6 ,knightMoves_G6 ,knightMoves_H6
 ,knightMoves_A7 ,knightMoves_B7 ,knightMoves_C7 ,knightMoves_D7 ,knightMoves_E7 ,knightMoves_F7 ,knightMoves_G7 ,knightMoves_H7
 ,knightMoves_A8 ,knightMoves_B8 ,knightMoves_C8 ,knightMoves_D8 ,knightMoves_E8 ,knightMoves_F8 ,knightMoves_G8 ,knightMoves_H8
};

const DirectionArray MoveTable::whitePawnMoves[64] = {
  whitePawnMoves_A1 ,whitePawnMoves_B1 ,whitePawnMoves_C1 ,whitePawnMoves_D1 ,whitePawnMoves_E1 ,whitePawnMoves_F1 ,whitePawnMoves_G1 ,whitePawnMoves_H1
 ,whitePawnMoves_A2 ,whitePawnMoves_B2 ,whitePawnMoves_C2 ,whitePawnMoves_D2 ,whitePawnMoves_E2 ,whitePawnMoves_F2 ,whitePawnMoves_G2 ,whitePawnMoves_H2
 ,whitePawnMoves_A3 ,whitePawnMoves_B3 ,whitePawnMoves_C3 ,whitePawnMoves_D3 ,whitePawnMoves_E3 ,whitePawnMoves_F3 ,whitePawnMoves_G3 ,whitePawnMoves_H3
 ,whitePawnMoves_A4 ,whitePawnMoves_B4 ,whitePawnMoves_C4 ,whitePawnMoves_D4 ,whitePawnMoves_E4 ,whitePawnMoves_F4 ,whitePawnMoves_G4 ,whitePawnMoves_H4
 ,whitePawnMoves_A5 ,whitePawnMoves_B5 ,whitePawnMoves_C5 ,whitePawnMoves_D5 ,whitePawnMoves_E5 ,whitePawnMoves_F5 ,whitePawnMoves_G5 ,whitePawnMoves_H5
 ,whitePawnMoves_A6 ,whitePawnMoves_B6 ,whitePawnMoves_C6 ,whitePawnMoves_D6 ,whitePawnMoves_E6 ,whitePawnMoves_F6 ,whitePawnMoves_G6 ,whitePawnMoves_H6
 ,whitePawnMoves_A7 ,whitePawnMoves_B7 ,whitePawnMoves_C7 ,whitePawnMoves_D7 ,whitePawnMoves_E7 ,whitePawnMoves_F7 ,whitePawnMoves_G7 ,whitePawnMoves_H7
 ,whitePawnMoves_A8 ,whitePawnMoves_B8 ,whitePawnMoves_C8 ,whitePawnMoves_D8 ,whitePawnMoves_E8 ,whitePawnMoves_F8 ,whitePawnMoves_G8 ,whitePawnMoves_H8
};

const DirectionArray MoveTable::blackPawnMoves[64] = {
  blackPawnMoves_A1 ,blackPawnMoves_B1 ,blackPawnMoves_C1 ,blackPawnMoves_D1 ,blackPawnMoves_E1 ,blackPawnMoves_F1 ,blackPawnMoves_G1 ,blackPawnMoves_H1
 ,blackPawnMoves_A2 ,blackPawnMoves_B2 ,blackPawnMoves_C2 ,blackPawnMoves_D2 ,blackPawnMoves_E2 ,blackPawnMoves_F2 ,blackPawnMoves_G2 ,blackPawnMoves_H2
 ,blackPawnMoves_A3 ,blackPawnMoves_B3 ,blackPawnMoves_C3 ,blackPawnMoves_D3 ,blackPawnMoves_E3 ,blackPawnMoves_F3 ,blackPawnMoves_G3 ,blackPawnMoves_H3
 ,blackPawnMoves_A4 ,blackPawnMoves_B4 ,blackPawnMoves_C4 ,blackPawnMoves_D4 ,blackPawnMoves_E4 ,blackPawnMoves_F4 ,blackPawnMoves_G4 ,blackPawnMoves_H4
 ,blackPawnMoves_A5 ,blackPawnMoves_B5 ,blackPawnMoves_C5 ,blackPawnMoves_D5 ,blackPawnMoves_E5 ,blackPawnMoves_F5 ,blackPawnMoves_G5 ,blackPawnMoves_H5
 ,blackPawnMoves_A6 ,blackPawnMoves_B6 ,blackPawnMoves_C6 ,blackPawnMoves_D6 ,blackPawnMoves_E6 ,blackPawnMoves_F6 ,blackPawnMoves_G6 ,blackPawnMoves_H6
 ,blackPawnMoves_A7 ,blackPawnMoves_B7 ,blackPawnMoves_C7 ,blackPawnMoves_D7 ,blackPawnMoves_E7 ,blackPawnMoves_F7 ,blackPawnMoves_G7 ,blackPawnMoves_H7
 ,blackPawnMoves_A8 ,blackPawnMoves_B8 ,blackPawnMoves_C8 ,blackPawnMoves_D8 ,blackPawnMoves_E8 ,blackPawnMoves_F8 ,blackPawnMoves_G8 ,blackPawnMoves_H8
};


