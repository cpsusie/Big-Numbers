//  GREKO Chess Engine
//  (c) 2002-2011 Vladimir Medvedev <vrm@bk.ru>
//  http://greko.110mb.com

//  eval.cpp: static position evaluation
//  modified: 1-Aug-2011

#include "eval.h"

EVAL VAL_P = 100;
EVAL VAL_N = 400;
EVAL VAL_B = 400;
EVAL VAL_R = 600;
EVAL VAL_Q = 1200;

EVAL LazyEval         = 150;

EVAL PawnDoubled      = -10;
EVAL PawnIsolated     = -10;
EVAL PawnBackwards    = -10;
EVAL PawnCenter       =  10;
EVAL PawnPassed       = 100;
EVAL PassedKingDist   =   5;
EVAL PawnPassedSquare =  50;
EVAL KnightMobility   =  20;
EVAL KnightCenter     =  10;
EVAL KnightOutpost    =  10;
EVAL BishopPair       =  50;
EVAL BishopCenter     =  10;
EVAL BishopMobility   =  60;
EVAL BishopTrapped    = -50;
EVAL RookMobility     =  40;
EVAL Rook7th          =  20;
EVAL RookOpen         =  10;
EVAL QueenKingTropism =  60;
EVAL KingCenterMid    = -40;
EVAL KingCenterEnd    =  40;
EVAL KingPawnShield   = 100;

EVAL PSQ_P[64];
EVAL PSQ_N[64];
EVAL PSQ_B[64];
EVAL PSQ_K_MID[64];
EVAL PSQ_K_END[64];
EVAL KNIGHT_MOBILITY[9];
EVAL BISHOP_MOBILITY[14];
EVAL ROOK_MOBILITY[15];
EVAL QUEEN_KING_TROPISM[8];
EVAL KING_PAWN_SHIELD[10];

struct PawnEntry
{
  U32  m_pawnHash;
  int  m_ranks[10][2];
  EVAL m_score;
  U64  m_passed[2];

  void Read(const Position& pos);
};

const int g_pawnHashSize = 8192;
PawnEntry g_pawnHash[g_pawnHashSize];

inline int Dist(FLD f1, FLD f2)
{
  int drow = Row(f1) - Row(f2);
  if (drow < 0) drow = -drow;
  int dcol = Col(f1) - Col(f2);
  if (dcol < 0) dcol = -dcol;

  return (drow > dcol)? drow : dcol;
}
////////////////////////////////////////////////////////////////////////////////

int PawnShieldWhite(const PawnEntry& pentry, FLD K)
{
  int r = 0;
  int file = Col(K) + 1;
  for (int i = file - 1; i <= file + 1; ++i)
  {
    int rank = pentry.m_ranks[i][WHITE];
    if (rank == 6)
      ;
    else if (rank == 5)
      r += 1;
    else if (rank == 4)
      r += 2;
    else
      r += 3;
  }
  return r;
}
////////////////////////////////////////////////////////////////////////////////

int PawnShieldBlack(const PawnEntry& pentry, FLD K)
{
  int r = 0;
  int file = Col(K) + 1;
  for (int i = file - 1; i <= file + 1; ++i)
  {
    int rank = pentry.m_ranks[i][BLACK];
    if (rank == 1)
      ;
    else if (rank == 2)
      r += 1;
    else if (rank == 3)
      r += 2;
    else
      r += 3;
  }
  return r;
}
////////////////////////////////////////////////////////////////////////////////

EVAL Evaluate(const Position& pos, EVAL alpha, EVAL beta)
{
  EVAL matScore = pos.Material(WHITE) - pos.Material(BLACK);
  EVAL posScore = 0;

  if (pos.Count(BW) == 2)
    matScore += BishopPair;
  if (pos.Count(BB) == 2)
    matScore -= BishopPair;

  EVAL lazy = (pos.Side() == WHITE)? matScore : -matScore;
  if (lazy <= alpha - LazyEval)
    return alpha;
  if (lazy >= beta + LazyEval)
    return beta;

  U64 x, y, occ = pos.BitsAll();
  U64 court[2] = { BB_KING_ATTACKS[pos.King(WHITE)], BB_KING_ATTACKS[pos.King(BLACK)] };
  FLD f;

  //
  //   PAWNS
  //

  int index = pos.PawnHash() % g_pawnHashSize;
  PawnEntry& pentry = g_pawnHash[index];
  if (pentry.m_pawnHash != pos.PawnHash())
    pentry.Read(pos);

  posScore += pentry.m_score;

  x = pentry.m_passed[WHITE];
  while (x)
  {
    f = PopLSB(x);
    posScore += PawnPassed * (7 - Row(f)) / 6;
    if (pos.MatIndex(BLACK) == 0)
    {
      FLD f1 = f;
      if (pos.Side() == BLACK)
        f1 += 8;
      if ((BB_PAWN_SQUARE[f1][WHITE] & pos.Bits(KB)) == 0)
        posScore += PawnPassedSquare * (7 - Row(f1)) / 6;
    }
    else if (pos.MatIndex(BLACK) < 10)
      posScore += PassedKingDist * Dist(f - 8, pos.King(BLACK));
  }

  x = pentry.m_passed[BLACK];
  while (x)
  {
    f = PopLSB(x);
    posScore -= PawnPassed * Row(f) / 6;
    if (pos.MatIndex(WHITE) == 0)
    {
      FLD f1 = f;
      if (pos.Side() == WHITE)
        f1 -= 8;
      if ((BB_PAWN_SQUARE[f1][BLACK] & pos.Bits(KW)) == 0)
        posScore -= PawnPassedSquare * Row(f1) / 6;
    }
    else if (pos.MatIndex(WHITE) < 10)
      posScore -= PassedKingDist * Dist(f + 8, pos.King(WHITE));
  }

  //
  //   KNIGHTS
  //

  static const int outpost[64] =
  {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 0, 0, 0,
    0, 0, 1, 1, 1, 1, 0, 0,
    0, 1, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
  };

  x = pos.Bits(NW);
  while (x)
  {
    f = PopLSB(x);
    posScore += PSQ_N[f];
    y = BB_KNIGHT_ATTACKS[f] & ~occ;
    posScore += KNIGHT_MOBILITY[CountBits(y)];
    if (outpost[f])
    {
      if (BB_PAWN_ATTACKS[f][BLACK] & pos.Bits(PW))
        posScore += KnightOutpost;
    }
  }

  x = pos.Bits(NB);
  while (x)
  {
    f = PopLSB(x);
    posScore -= PSQ_N[FLIP[f]];
    y = BB_KNIGHT_ATTACKS[f] & ~occ;
    posScore -= KNIGHT_MOBILITY[CountBits(y)];
    if (outpost[FLIP[f]])
    {
      if (BB_PAWN_ATTACKS[f][WHITE] & pos.Bits(PB))
        posScore -= KnightOutpost;
    }
  }

  //
  //   BISHOPS
  //
  
  x = pos.Bits(BW);
  while (x)
  {
    f = PopLSB(x);
    posScore += PSQ_B[f];
    if ((f == A7 || f == B8) && pos[B6] == PB && pos[C7] == PB)
      posScore += BishopTrapped;
    if ((f == H7 || f == G8) && pos[G6] == PB && pos[F7] == PB)
      posScore += BishopTrapped;
    y = BishopAttacks(f, occ);
    posScore += BISHOP_MOBILITY[CountBits(y)];
  }

  x = pos.Bits(BB);
  while (x)
  {
    f = PopLSB(x);
    posScore -= PSQ_B[FLIP[f]];
    if ((f == A2 || f == B1) && pos[B3] == PW && pos[C2] == PW)
      posScore -= BishopTrapped;
    if ((f == H2 || f == G1) && pos[G3] == PW && pos[F2] == PW)
      posScore -= BishopTrapped;
    y = BishopAttacks(f, occ);
    posScore -= BISHOP_MOBILITY[CountBits(y)];
  }

  //
  //   ROOKS
  //

  x = pos.Bits(RW);
  while (x)
  {
    f = PopLSB(x);
    y = RookAttacks(f, occ ^ pos.Bits(RW));
    posScore += ROOK_MOBILITY[CountBits(y)];
    if (Row(f) == 1)
      posScore += Rook7th;
    int file = Col(f) + 1;
    if (pentry.m_ranks[file][WHITE] == 0)
      posScore += RookOpen;
  }

  x = pos.Bits(RB);
  while (x)
  {
    f = PopLSB(x);
    y = RookAttacks(f, occ ^ pos.Bits(RB));
    posScore -= ROOK_MOBILITY[CountBits(y)];
    if (Row(f) == 6)
      posScore -= Rook7th;
    int file = Col(f) + 1;
    if (pentry.m_ranks[file][BLACK] == 7)
      posScore -= RookOpen;
  }

  //
  //   QUEENS
  //

  x = pos.Bits(QW);
  while (x)
  {
    f = PopLSB(x);
    posScore += QUEEN_KING_TROPISM[Dist(f, pos.King(BLACK))];
  }

  x = pos.Bits(QB);
  while (x)
  {
    f = PopLSB(x);
    posScore -= QUEEN_KING_TROPISM[Dist(f, pos.King(WHITE))];
  }

  //
  //   KINGS
  //

  {
    f = pos.King(WHITE);
    posScore += PSQ_K_MID[f] * pos.MatIndex(BLACK) / 32;
    posScore += PSQ_K_END[f] * (32 - pos.MatIndex(BLACK)) / 32;

    int penalty = PawnShieldWhite(pentry, f);
    posScore += KING_PAWN_SHIELD[penalty] * pos.MatIndex(BLACK) / 32;
  }

  {
    f = pos.King(BLACK);
    posScore -= PSQ_K_MID[FLIP[f]] * pos.MatIndex(WHITE) / 32;
    posScore -= PSQ_K_END[FLIP[f]] * (32 - pos.MatIndex(WHITE)) / 32;

    int penalty = PawnShieldBlack(pentry, f);
    posScore -= KING_PAWN_SHIELD[penalty] * pos.MatIndex(WHITE) / 32;
  }

  EVAL e = matScore + posScore;
  if (pos.Count(PW) == 0 && pos.MatIndex(WHITE) < 5 && e > 0)
    e = 0;
  if (pos.Count(PB) == 0 && pos.MatIndex(BLACK) < 5 && e < 0)
    e = 0;

  return (pos.Side() == WHITE)? e : -e;
}
////////////////////////////////////////////////////////////////////////////////

const int center[64] =
{
  -3, -2, -1,  0,  0, -1, -2, -3,
  -2, -1,  0,  1,  1,  0, -1, -2,
  -1,  0,  1,  2,  2,  1,  0, -1,
   0,  1,  2,  3,  3,  2,  1,  0,
   0,  1,  2,  3,  3,  2,  1,  0,
  -1,  0,  1,  2,  2,  1,  0, -1,
  -2, -1,  0,  1,  1,  0, -1, -2,
  -3, -2, -1,  0,  0, -1, -2, -3
};

const int center_p[64] =
{
   0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  2,  2,  0,  0,  0,
   0,  0,  1,  2,  2,  1,  0,  0,
   0,  0,  1,  2,  2,  1,  0,  0,
   0,  0,  1,  1,  1,  1,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,  0,  0
};

const int center_k[64] =
{
   4,  4,  4,  4,  4,  4,  4,  4, 
   4,  4,  4,  4,  4,  4,  4,  4,
   4,  4,  4,  4,  4,  4,  4,  4,
   4,  4,  4,  4,  4,  4,  4,  4,
   4,  4,  4,  4,  4,  4,  4,  4,
   4,  4,  4,  4,  4,  4,  4,  4,
   2,  2,  2,  2,  2,  2,  2,  2,
   1,  0,  0,  1,  0,  1,  0,  1
};

void InitEval()
{
  for (int f = 0; f < 64; ++f)
  {
    PSQ_P[f] = PawnCenter * center_p[f] / 2;
    PSQ_N[f] = KnightCenter * center[f] / 3;
    PSQ_B[f] = BishopCenter * center[f] / 3;
    PSQ_K_MID[f] = KingCenterMid * center_k[f] / 4;
    PSQ_K_END[f] = KingCenterEnd * center[f] / 3;
  }

  for (int m = 0; m < 9; ++m)
    KNIGHT_MOBILITY[m] = (EVAL) (KnightMobility * (m - 4) / 4);
  for (int m = 0; m < 14; ++m)
    BISHOP_MOBILITY[m] = (EVAL) (BishopMobility * (m - 6.5) / 6.5);
  for (int m = 0; m < 15; ++m)    
    ROOK_MOBILITY[m] = (EVAL) (RookMobility * (m - 7) / 7);
  for (int d = 0; d < 8; ++d)
    QUEEN_KING_TROPISM[d] = QueenKingTropism * (7 - d) / 7;
  for (int p = 0; p < 10; ++p)
    KING_PAWN_SHIELD[p] = -KingPawnShield * p / 9;
}
////////////////////////////////////////////////////////////////////////////////

void PawnEntry::Read(const Position& pos)
{
  U64 x;
  FLD f;
  int file, rank;

  m_pawnHash = pos.PawnHash();
  m_score = 0;
  m_passed[WHITE] = m_passed[BLACK] = 0;

  for (file = 0; file < 10; ++file)
  {
    m_ranks[file][WHITE] = 0;
    m_ranks[file][BLACK] = 7;
  }

  x = pos.Bits(PW);
  while (x)
  {
    f = PopLSB(x);
    file = Col(f) + 1;
    rank = Row(f);
    if (rank > m_ranks[file][WHITE])
      m_ranks[file][WHITE] = rank;
  }

  x = pos.Bits(PB);
  while (x)
  {
    f = PopLSB(x);
    file = Col(f) + 1;
    rank = Row(f);
    if (rank < m_ranks[file][BLACK])
      m_ranks[file][BLACK] = rank;
  }

  x = pos.Bits(PW);
  while (x)
  {
    f = PopLSB(x);
    file = Col(f) + 1;
    rank = Row(f);

    m_score += PSQ_P[f];
    if (m_ranks[file][BLACK] == 7)
    {
      if (m_ranks[file - 1][BLACK] >= rank && m_ranks[file + 1][BLACK] >= rank)
        m_passed[WHITE] |= BB_SINGLE[f];
    }
    if (rank != m_ranks[file][WHITE])
      m_score += PawnDoubled;
    if (m_ranks[file - 1][WHITE] == 0 && m_ranks[file + 1][WHITE] == 0)
      m_score += PawnIsolated;
    else if (m_ranks[file - 1][WHITE] < rank && m_ranks[file + 1][WHITE] < rank)
      m_score += PawnBackwards;
  }

  x = pos.Bits(PB);
  while (x)
  {
    f = PopLSB(x);
    file = Col(f) + 1;
    rank = Row(f);

    m_score -= PSQ_P[FLIP[f]];
    if (m_ranks[file][WHITE] == 0)
    {
      if (m_ranks[file - 1][WHITE] <= rank && m_ranks[file + 1][WHITE] <= rank)
        m_passed[BLACK] |= BB_SINGLE[f];
    }
    if (rank != m_ranks[file][BLACK])
      m_score -= PawnDoubled;
    if (m_ranks[file - 1][BLACK] == 7 && m_ranks[file + 1][BLACK] == 7)
      m_score -= PawnIsolated;
    else if (m_ranks[file - 1][BLACK] > rank && m_ranks[file + 1][BLACK] > rank)
      m_score -= PawnBackwards;
  }
}
////////////////////////////////////////////////////////////////////////////////

EVAL VALUE[14] =
{ 0, 0, VAL_P, VAL_P, VAL_N, VAL_N, VAL_B, VAL_B, VAL_R, VAL_R, VAL_Q, VAL_Q, 0, 0 };

