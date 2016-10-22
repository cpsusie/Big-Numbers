//  GREKO Chess Engine
//  (c) 2002-2011 Vladimir Medvedev <vrm@bk.ru>
//  http://greko.110mb.com

//  bitboards.cpp: 64-bit position representation
//  modified: 1-Aug-2011


#include "bitboards.h"
#include "position.h"
#include "utils.h"

int g_dist[64][64];

U64 BB_SINGLE[64];
U64 BB_DIR[64][8];
U64 BB_PAWN_ATTACKS[64][2];
U64 BB_KNIGHT_ATTACKS[64];
U64 BB_BISHOP_ATTACKS[64];
U64 BB_ROOK_ATTACKS[64];
U64 BB_QUEEN_ATTACKS[64];
U64 BB_KING_ATTACKS[64];
U64 BB_BETWEEN[64][64];
U64 BB_PAWN_SQUARE[64][2];
U8  DIRS[64][64];

#ifdef MAGIC

int B_OFFSET[64];
U64* B_DATA = 0;

int R_OFFSET[64];
U64* R_DATA = 0;

#endif

U8 FIRST_IN_16[65536];

U64 BB_VERTICAL[8] =
{
  LL(0x8080808080808080),
  LL(0x4040404040404040),
  LL(0x2020202020202020),
  LL(0x1010101010101010),
  LL(0x0808080808080808),
  LL(0x0404040404040404),
  LL(0x0202020202020202),
  LL(0x0101010101010101)
};

U64 EnumBits(U64 mask, U64 n)
{
  U64 x = 0;
  while (mask != 0 && n != 0)
  {
    int f = PopLSB(mask);
    int digit = int(n & 1);
    n >>= 1;
    x |= digit * BB_SINGLE[f];    
  }
  return x; 
}
////////////////////////////////////////////////////////////////////////////////

#define TRACE(Shift)        \
   x = Shift(BB_SINGLE[f]); \
   while (x)                \
   {                        \
      att |= x;             \
      if (x & occupied)     \
         break;             \
                            \
      x = Shift(x);         \
   }

U64 BishopAttacksTrace(int f, const U64& occupied)
{
  U64 att = 0;
  U64 x = 0;

  TRACE(UpRight);
  TRACE(UpLeft);
  TRACE(DownLeft);
  TRACE(DownRight);

  return att;
}
////////////////////////////////////////////////////////////////////////////////

U64 RookAttacksTrace(int f, const U64& occupied)
{
  U64 att = 0;
  U64 x = 0;

  TRACE(Right);
  TRACE(Up);
  TRACE(Left);
  TRACE(Down);

  return att;
}
////////////////////////////////////////////////////////////////////////////////

#undef TRACE

void InitBitboards()
{
  int i, j, from, to;
  U64 x, y;

  // Bitscan
  for (i = 1; i < 65536; ++i)
  {
    int x = 0x8000;
    for (j = 0; j < 16; ++j)
    {
      if (i & x)
      {
        FIRST_IN_16[i] = j;
        break;
      }
      x >>= 1;
    }
  }

  x = ((U64) 1) << 63;
  for (i = 0; i < 64; i++)
  {
    BB_SINGLE[i] = x;
    x >>= 1;
  }

  for (from = 0; from < 64; from++)
  {
    for (to = 0; to < 64; ++to)
      {
      BB_BETWEEN[from][to] = 0;
      DIRS[from][to] = DIR_NO;

      int drow = Row(from) - Row(to);
      int dcol = Col(from) - Col(to);

      if (drow < 0) drow = -drow;
      if (dcol < 0) dcol = -dcol;

      g_dist[from][to] = (drow > dcol)? drow : dcol;
      }

#define TRACE_DIR(dir, Shift, delta) \
   x = Shift(BB_SINGLE[from]);       \
   y = 0;                            \
   to = from + (delta);              \
   while (x)                         \
   {                                 \
      BB_BETWEEN[from][to] = y;      \
      DIRS[from][to] = dir;          \
      y |= x;                        \
      x = Shift(x);                  \
      to += (delta);                 \
   }                                 \
   BB_DIR[from][dir] = y;
  
    TRACE_DIR (DIR_R,  Right,     1)
    TRACE_DIR (DIR_UR, UpRight,  -7)
    TRACE_DIR (DIR_U,  Up,       -8)
    TRACE_DIR (DIR_UL, UpLeft,   -9)
    TRACE_DIR (DIR_L,  Left,     -1)
    TRACE_DIR (DIR_DL, DownLeft,  7)
    TRACE_DIR (DIR_D,  Down,      8)
    TRACE_DIR (DIR_DR, DownRight, 9)

    x = BB_SINGLE[from];
    y = 0;
    y |= Right(UpRight(x));
    y |= Up(UpRight(x));
    y |= Up(UpLeft(x));
    y |= Left(UpLeft(x));
    y |= Left(DownLeft(x));
    y |= Down(DownLeft(x));
    y |= Down(DownRight(x));
    y |= Right(DownRight(x));
    BB_KNIGHT_ATTACKS[from] = y;

    x = BB_SINGLE[from];
    y = 0;
    y |= UpRight(x);
    y |= Up(x);
    y |= UpLeft(x);
    y |= Left(x);
    y |= DownLeft(x);
    y |= Down(x);
    y |= DownRight(x);
    y |= Right(x);
    BB_KING_ATTACKS[from] = y;

    x = BB_SINGLE[from];
    y = 0;
    y |= UpRight(x);
    y |= UpLeft(x);
    BB_PAWN_ATTACKS[from][WHITE] = y;

    x = BB_SINGLE[from];
    y = 0;
    y |= DownRight(x);
    y |= DownLeft(x);
    BB_PAWN_ATTACKS[from][BLACK] = y;

    BB_BISHOP_ATTACKS[from] =
      BB_DIR[from][DIR_UR] |
      BB_DIR[from][DIR_UL] |
      BB_DIR[from][DIR_DL] |
      BB_DIR[from][DIR_DR];

    BB_ROOK_ATTACKS[from] =
      BB_DIR[from][DIR_R] |
      BB_DIR[from][DIR_U] |
      BB_DIR[from][DIR_L] |
      BB_DIR[from][DIR_D];

    BB_QUEEN_ATTACKS[from] = BB_ROOK_ATTACKS[from] | BB_BISHOP_ATTACKS[from];
  }

  // pawn squares

  for (int f = 0; f < 64; f++)
  {
    x = BB_DIR[f][DIR_U] | BB_SINGLE[f];
    for (j = 0; j < Row(f); j++)
    {
      x |= Right(x);
      x |= Left(x);
    }

    BB_PAWN_SQUARE[f][WHITE] = x;

    x = BB_DIR[f][DIR_D] | BB_SINGLE[f];
    for (j = 0; j < 7 - Row(f); j++)
    {
      x |= Right(x);
      x |= Left(x);
    }

    BB_PAWN_SQUARE[f][BLACK] = x;
  }

#ifdef MAGIC

  int offset;

  offset = 0;
  for (int f = 0; f < 64; ++f)
  {
    B_OFFSET[f] = offset;
    offset += (1 << B_BITS[f]);
  }
  B_DATA = new U64[offset];

  for (int f = 0; f < 64; ++f)
  {
    U64 mask = B_MASK[f];
    int bits = B_BITS[f];

    for (int n = 0; n < (1 << bits); ++n)
    {
      U64 occupied = EnumBits(mask, n);
      U64 att = BishopAttacksTrace(f, occupied);
      int index = B_OFFSET[f];
      index += int((occupied * B_MULT[f]) >> (64 - bits));

      B_DATA[index] = att;
    }
  }

  offset = 0;
  for (int f = 0; f < 64; ++f)
  {
    R_OFFSET[f] = offset;
    offset += (1 << R_BITS[f]);
  }
  R_DATA = new U64[offset];

  for (int f = 0; f < 64; ++f)
  {
    U64 mask = R_MASK[f];
    int bits = R_BITS[f];

    for (int n = 0; n < (1 << bits); ++n)
    {
      U64 occupied = EnumBits(mask, n);
      U64 att = RookAttacksTrace(f, occupied);
      int index = R_OFFSET[f];
      index += int((occupied * R_MULT[f]) >> (64 - bits));

      R_DATA[index] = att;
    }
  }

#endif

}
////////////////////////////////////////////////////////////////////////////////

void Print(U64 b)
{
  out("\n");
  for (int f = 0; f < 64; f++)
  {
    if (b & BB_SINGLE[f])
      out(" 1");
    else
      out(" -");

    if (Col(f) == 7)
      out("\n");
  }
  out("\n");
}
////////////////////////////////////////////////////////////////////////////////

