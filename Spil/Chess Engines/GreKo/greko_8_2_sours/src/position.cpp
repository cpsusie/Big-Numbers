//  GREKO Chess Engine
//  (c) 2002-2011 Vladimir Medvedev <vrm@bk.ru>
//  http://greko.110mb.com

//  position.cpp: position and game representation
//  modified: 1-Aug-2011

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif

#include "eval.h"
#include "moves.h"
#include "notation.h"
#include "utils.h"

U64 Position::s_Zobrist0 = 0;
U64 Position::s_Zobrist[64][14];
U32 Position::s_ZobristPawn[64][14];

void Position::Clear()
{
  m_castlings = 0;
  m_ep = NF;
  m_fifty = 0;
  m_hash = 0;
  m_hashPawn = 0;
  m_Kings[WHITE] = m_Kings[BLACK] = 0;
  m_material[WHITE] = m_material[BLACK] = 0;
  m_matIndex[WHITE] = m_matIndex[BLACK] = 0;
  m_ply = 0;
  m_side = WHITE;
  m_undoCnt = 0;

  for (int f = 0; f < 64; f++)
  {
    m_board[f] = NOPIECE;
  }

  for (int p = 0; p < 14; p++)
  {
    m_bits[p] = 0;
    m_count[p] = 0;
  }

  m_bitsAll[WHITE] = m_bitsAll[BLACK] = 0;
}
////////////////////////////////////////////////////////////////////////////////

bool Position::GameOver(char* comment) const
{
  MoveList mvlist;
  mvlist.GenAllMoves(*this);

  if (mvlist.Size() == 0)
  {
    if (InCheck())
    {
      if (m_side == WHITE)
        strcpy(comment, "0-1 {Black mates}\n");
      else
        strcpy(comment, "1-0 {White mates}\n");
    }
    else
    {
      strcpy(comment, "1/2-1/2 {Draw: stalemate}\n");
    }

    return true;
  }

  if (IsDraw())
  {
    strcpy(comment, "1/2-1/2 {Draw: material}\n");
    return true;
  }

  return false;
}
////////////////////////////////////////////////////////////////////////////////

U64 Position::GetAttacks(FLD to, COLOR side, U64 occupied) const
{
  U64 att = 0;

  att |= BB_PAWN_ATTACKS[to][side ^ 1] & Bits(PW | side);
  att |= BB_KNIGHT_ATTACKS[to] & Bits(NW | side);
  att |= BB_KING_ATTACKS[to] & Bits(KW | side);
  att |= BishopAttacks(to, occupied) & (Bits(BW | side) | Bits(QW | side));
  att |= RookAttacks(to, occupied) & (Bits(RW | side) | Bits(QW | side));

  return att;
}
////////////////////////////////////////////////////////////////////////////////

char* Position::FEN(char* buf) const
{
  static const char* names[14] = {"", "", "P", "p", "N", "n", "B", "b", "R", "r", "Q", "q", "K", "k"};
  strcpy(buf, "");
  int empty = 0;
  for (int f = 0; f < 64; ++f)
  {
    PIECE p = m_board[f];
    if (p)
    {
      if (empty)
      {
        char t1[3];
        sprintf(t1, "%d", empty);
        strcat(buf, t1);
        empty = 0;
      }
      strcat(buf, names[p]);
    }
    else
      ++empty;

    if (Col(f) == 7)
    {
      if (empty)
      {
        char t1[3];
        sprintf(t1, "%d", empty);
        strcat(buf, t1);
        empty = 0;
      }
      if (f != 63)
        strcat(buf, "/");
    }
  }

  if (m_side == WHITE)
    strcat(buf, " w");
  else
    strcat(buf, " b");

  if (m_castlings & 0x33)
  {
    strcat(buf, " ");
    if (m_castlings & WHITE_CAN_O_O)   strcat(buf, "K");
    if (m_castlings & WHITE_CAN_O_O_O) strcat(buf, "Q");
    if (m_castlings & BLACK_CAN_O_O)   strcat(buf, "k");
    if (m_castlings & BLACK_CAN_O_O_O) strcat(buf, "q");
  }
  else
    strcat(buf, " -");

  if (m_ep == NF)
    strcat(buf, " -");
  else
  {
    strcat(buf, " ");
    char t1[3];
    strcat(buf, FldToStr(m_ep, t1));
  }

  return buf;
}
////////////////////////////////////////////////////////////////////////////////

int Position::GetRepetitions() const
{ 
  int total = 1;

  for (int i = m_undoCnt - 1; i >= 0; --i)
  {
    if (m_undos[i].m_hash == m_hash) 
    {
      ++total;
    }

    if (m_undos[i].m_mv == 0)
      return 0;

    if (m_undos[i].m_mv.Captured())
      break;

    if (m_undos[i].m_mv.Piece() == PW)
      break;

    if (m_undos[i].m_mv.Piece() == PB)
      break;
   }

  return total;
}
////////////////////////////////////////////////////////////////////////////////

void Position::InitHashNumbers()
{
  RandSeed32(42);
  for (int f = 0; f < 64; f++)
  {
    for (int p = 0; p < 14; p++)
    {
      s_Zobrist[f][p] = Rand64();
      if (p == PW || p == PB)
        s_ZobristPawn[f][p] = Rand32();
      else
        s_ZobristPawn[f][p] = 0;
    }
  }
  s_Zobrist0 = Rand64();
}
////////////////////////////////////////////////////////////////////////////////

bool Position::IsAttacked(FLD to, COLOR side) const
{
  U64 x;
  FLD from;

  if (BB_PAWN_ATTACKS[to][side ^ 1] & Bits(PW | side))
    return true;

  if (BB_KNIGHT_ATTACKS[to] & Bits(NW | side))
    return true;

  if (BB_KING_ATTACKS[to] & Bits(KW | side))
    return true;

  U64 occupied = m_bitsAll[WHITE] | m_bitsAll[BLACK];

  x = BB_BISHOP_ATTACKS[to] & 
      (Bits(QW | side) | Bits(BW | side));

  while (x)
  {
    from = PopLSB(x);
    if ((BB_BETWEEN[from][to] & occupied) == 0)
      return true;
  }

  x = BB_ROOK_ATTACKS[to] & 
      (Bits(QW | side) | Bits(RW | side));

  while (x)
  {
    from = PopLSB(x);
    if ((BB_BETWEEN[from][to] & occupied) == 0)
      return true;
  }

  return false;
}
////////////////////////////////////////////////////////////////////////////////

bool Position::IsDraw() const
{
  if (Material(WHITE) == 0 && Material(BLACK) == 0)
    return 1;

  if (Count(PW) || Count(PB))
    return 0;

  if (Material(WHITE) == 0)                            
  {
    if (Material(BLACK) < VAL_R)
      return 1;
  }

  if (Material(BLACK) == 0)
  {
    if (Material(WHITE) < VAL_R)
      return 1;
  }

  return 0;
}
////////////////////////////////////////////////////////////////////////////////

static unsigned char castling_effect[64] =
{
  0xf7, 0xff, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xfb,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xfd, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xfe
};

bool Position::MakeMove(Move mv)
{
  Undo& undo = m_undos[m_undoCnt++];
  undo.m_castlings = m_castlings;
  undo.m_ep = m_ep;
  undo.m_fifty = m_fifty;
  undo.m_hash = m_hash;
  undo.m_mv = mv;

  FLD from = mv.From();
  FLD to = mv.To();
  PIECE piece = mv.Piece();
  PIECE captured = mv.Captured();
  PIECE promotion = mv.Promotion();

  assert(piece != NOPIECE);

  COLOR side = GetColor(piece);
  COLOR opp = 1 - side;
  assert(side == m_side);

  if (captured)
  {
    m_material[opp] -= VALUE[captured];
    --m_count[captured];
  }

  bool isEP = false;
  bool isCastling = false;

  switch (piece)
  {
  case PW:

    m_fifty = 0;
    RemovePiece(from);

    if (to == m_ep)
    {
      isEP = true;
      RemovePiece(to + 8);
      PutPiece(to, PW);
    }
    else
    {
      if (captured)
        RemovePiece(to);

      if (promotion == NOPIECE)
        PutPiece(to, PW);
      else
      {
        PutPiece(to, promotion);
        m_material[side] += VALUE[promotion] - VALUE[piece];
        m_count[promotion]++;
        m_count[piece]--;
      }
    }

    // Set enpassant field:
    if (to - from == - 16)       // long pawn move
      m_ep = from - 8;
    else
      m_ep = NF;

    break;

  case PB:

    m_fifty = 0;
    RemovePiece(from);

    if (to == m_ep)
    {
      isEP = true;
      RemovePiece(to - 8);
      PutPiece(to, PB);
    }
    else
    {
      if (captured)
        RemovePiece(to);

      if (promotion == NOPIECE)
        PutPiece(to, PB);
      else
      {
        PutPiece(to, promotion);
        m_material[side] += VALUE[promotion] - VALUE[piece];
        ++m_count[promotion];
        --m_count[piece];
      }
    }

    // Set enpassant field:
    if (to - from == 16)       // long pawn move
      m_ep = from + 8;
    else
      m_ep = NF;

    break;

  case NW:
  case NB:
  case BW:
  case BB:                               
  case RW:
  case RB:
  case QW:
  case QB:

    ++m_fifty;
    m_ep = NF;

    RemovePiece(from);

    if (captured)
    {
      m_fifty = 0;
      RemovePiece(to);
    }

    PutPiece(to, piece);

    break;

  case KW:

    ++m_fifty;
    m_ep = NF;

    RemovePiece(from);

    if (captured)
    {
      m_fifty = 0;
      RemovePiece(to);
    }
    else if (from == E1)
    {
      if (to == G1)
      {
        isCastling = true;
        RemovePiece(H1);
        PutPiece(F1, RW);
        m_castlings |= WHITE_DID_O_O; // 0001 0000
      }
      else if (to == C1)
      {
        isCastling = true;
        RemovePiece(A1);
        PutPiece(D1, RW);
        m_castlings |= WHITE_DID_O_O_O; // 0010 0000
      }
    }

    PutPiece(to, piece);
    m_Kings[WHITE] = to;
    break;

  case KB:

    ++m_fifty;
    m_ep = NF;

    RemovePiece(from);

    if (captured)
    {
      m_fifty = 0;
      RemovePiece(to);
    }
    else if (from == E8)
    {
      if (to == G8)
      {
        isCastling = true;
        RemovePiece(H8);
        PutPiece(F8, RB);
        m_castlings |= BLACK_DID_O_O; // 0100 0000
      }
      else if (to == C8)
      {
        isCastling = true;
        RemovePiece(A8);
        PutPiece(D8, RB);
        m_castlings |= BLACK_DID_O_O_O; // 1000 0000
      }
    }

    PutPiece(to, piece);
    m_Kings[BLACK] = to;

    break;
  default:
    assert(0);
  }

  m_castlings &= castling_effect[from];
  m_castlings &= castling_effect[to];

  ++m_ply;
  m_side ^= 1;
  m_hash ^= s_Zobrist0;
  
  if (IsAttacked(m_Kings[side], opp))
  {
    UnmakeMove();
    return false;
  }

  return true;
}
////////////////////////////////////////////////////////////////////////////////

void Position::MakeNullMove()
{
  Undo& undo = m_undos[m_undoCnt++];
  undo.m_castlings = m_castlings;
  undo.m_ep = m_ep;
  undo.m_fifty = m_fifty;
//  undo.m_hash = m_hash;
  undo.m_mv = 0;

  m_ep = NF;
  m_side ^= 1;
  m_hash ^= s_Zobrist0;
  ++m_ply;
}
////////////////////////////////////////////////////////////////////////////////

void Position::Mirror()
{
  Position old = *this;

  Clear();
  for (int f = 0; f < 64; f++)
  {
    PIECE p = old[f];
    if (p == NOPIECE)
      continue;

    PutPiece(FLIP[f], p ^ 1);

    if (p == KW)
      m_Kings[BLACK] = FLIP[f];
    else if (p == KB)
      m_Kings[WHITE] = FLIP[f];
  }

  m_side = old.Side() ^ 1;
  m_fifty = old.Fifty();
  if (old.EP() != NF)
    m_ep = FLIP[old.EP()];

  m_ply = old.Ply();

  for (int f1 = 0; f1 < 64; f1++)
  {
    PIECE p1 = m_board[f1];
    COLOR c1 = GetColor(p1);
    if (p1)
    {
      m_material[c1] += VALUE[p1];
      m_count[p1]++;
    }
  }

  int bit0 = old.m_castlings & 0x01;
  int bit1 = (old.m_castlings & 0x02) >> 1;
  int bit2 = (old.m_castlings & 0x04) >> 2;
  int bit3 = (old.m_castlings & 0x08) >> 3;
  int bit4 = (old.m_castlings & 0x10) >> 4;
  int bit5 = (old.m_castlings & 0x20) >> 5;
  int bit6 = (old.m_castlings & 0x40) >> 6;
  int bit7 = (old.m_castlings & 0x80) >> 7;

  m_castlings = (bit5 << 7) |
                   (bit4 << 6) |
                   (bit7 << 5) |
                   (bit6 << 4) |
                   (bit1 << 3) |
                   (bit0 << 2) |
                   (bit3 << 1) |
                   (bit2 << 0);
}
////////////////////////////////////////////////////////////////////////////////

void Position::Print() const
{
  out("\n");
  for (int f = 0; f < 64; f++)
  {
    char sym = '-';
    PIECE piece = m_board[f];
    switch (piece)
    {
    case PW:   sym = 'P'; break;
    case PB:   sym = 'p'; break;
    case NW: sym = 'N'; break;
    case NB: sym = 'n'; break;
    case BW: sym = 'B'; break;
    case BB: sym = 'b'; break;
    case RW:   sym = 'R'; break;
    case RB:   sym = 'r'; break;
    case QW:  sym = 'Q'; break;
    case QB:  sym = 'q'; break;
    case KW:   sym = 'K'; break;
    case KB:   sym = 'k'; break;
    default:      sym = '-'; break;
    }

    if (piece != NOPIECE && GetColor(piece) == WHITE)
      Highlight(1);

    out(" %c", sym);
    Highlight(0);

    if (Col(f) == 7)
      out("\n");
  }

  if (m_undoCnt)
  {
    out("\n ");
    for (int m = 0; m < m_undoCnt; m++)
    {
      char buf1[16];
      out("%s ", MoveToStrLong(m_undos[m].m_mv, buf1));
    }
    out("\n ");
  }
  out("\n");
}
////////////////////////////////////////////////////////////////////////////////

bool Position::SetFEN(const char* fen)
{
  if (strlen(fen) < 5)
    return 0;

  Clear();

  char buf[4096];
  strncpy(buf, fen, sizeof(buf));

  char* token = strtok(buf, " ");
  char* p = token;
  int incorrect_fen = 0;

  FLD f = A8;

  //
  //   1. Pieces
  //

  while (*p)
  {
    if (f > H1)
      break;

    PIECE piece = NOPIECE;

    switch (*p)
    {
    case 'P': piece = PW;   break;
    case 'p': piece = PB;   break;
    case 'N': piece = NW; break;
    case 'n': piece = NB; break;
    case 'B': piece = BW; break;
    case 'b': piece = BB; break;
    case 'R': piece = RW;   break;
    case 'r': piece = RB;   break;
    case 'Q': piece = QW;  break;
    case 'q': piece = QB;  break;
    case 'K': piece = KW; m_Kings[WHITE] = f; break;
    case 'k': piece = KB; m_Kings[BLACK] = f; break;

    case '1': f += 1; break;
    case '2': f += 2; break;
    case '3': f += 3; break;
    case '4': f += 4; break;
    case '5': f += 5; break;
    case '6': f += 6; break;
    case '7': f += 7; break;
    case '8': f += 8; break;

    case '/':
      if (Col(f) != 0)
        f = 8 * (Row(f) + 1);
      break;

    default:
      break;
    }

    if (piece)
    {
      PutPiece(f, piece);
      f++;
    }

    p++;
  }

  //
  //   2. Side to move
  //

  token = strtok(NULL, " ");
  if (token == NULL)
    goto FINALIZE_SETFEN;

  if (token[0] == 'w')
    m_side = WHITE;
  else if (token[0] == 'b')
  {
    m_side = BLACK;
    m_hash ^= s_Zobrist0;
  }
  else
  {
    incorrect_fen = 1;
    goto FINALIZE_SETFEN;
  }

  //
  //   3. Castling rights
  //

  token = strtok(NULL, " ");
  if (token == NULL)
    goto FINALIZE_SETFEN;

  m_castlings = 0;

  if (strstr(token, "K") != NULL) 
    m_castlings |= WHITE_CAN_O_O;

  if (strstr(token, "Q") != NULL) 
    m_castlings |= WHITE_CAN_O_O_O;
    
  if (strstr(token, "k") != NULL) 
    m_castlings |= BLACK_CAN_O_O;
    
  if (strstr(token, "q") != NULL) 
    m_castlings |= BLACK_CAN_O_O_O;

  //
  //   4. En-passant
  //

  token = strtok(NULL, " ");
  if (token == NULL)
    goto FINALIZE_SETFEN;

  m_ep = StrToFld(token);

  //
  //   5. Counters
  //

  token = strtok(NULL, " ");
  if (token == NULL)
    goto FINALIZE_SETFEN;

  m_fifty = atoi(token);
  if (m_fifty < 0)
    m_fifty = 0;

  token = strtok(NULL, " ");
  if (token == NULL)
    goto FINALIZE_SETFEN;

  m_ply = (atoi(token) - 1) * 2;
  if (m_side == BLACK)
    ++m_ply;

  if (m_ply < 0)
    m_ply = 0;

FINALIZE_SETFEN:

  for (int f1 = 0; f1 < 64; f1++)
  {
    PIECE p1 = m_board[f1];
    COLOR c1 = GetColor(p1);
    if (p1)
    {
      m_material[c1] += VALUE[p1];
      ++m_count[p1];
    }
  }

  return true;
}
////////////////////////////////////////////////////////////////////////////////

void Position::UnmakeMove()
{
  if (m_undoCnt <= 0)
    return;

  Undo& undo = m_undos[--m_undoCnt];
  Move mv = undo.m_mv;
  m_ep = undo.m_ep;
  m_castlings = undo.m_castlings;
  m_fifty = undo.m_fifty;

  FLD from = mv.From();
  FLD to = mv.To();
  PIECE piece = mv.Piece();
  PIECE captured = mv.Captured();
  PIECE promotion = mv.Promotion();
  
  --m_ply;
  m_side ^= 1;
  m_hash ^= s_Zobrist0;

  COLOR side = m_side;
  COLOR opp = side ^ 1;

  if (captured)
  {
    m_material[opp] += VALUE[captured];
    ++m_count[captured];
  }

  if (promotion)
  {
    m_material[side] -= (VALUE[promotion] - VALUE[piece]);
    --m_count[promotion];
    ++m_count[piece];
  }
  
  if (piece == KW)
  {
    m_Kings[WHITE] = from;

    if (from == E1 && to == G1)
    {
      RemovePiece(G1);
      RemovePiece(F1);
      PutPiece(E1, KW);
      PutPiece(H1, RW);
    }
    else if (from == E1 && to == C1)
    {
      RemovePiece(C1);
      RemovePiece(D1);
      PutPiece(E1, KW);
      PutPiece(A1, RW);
    }
    else
    {
      RemovePiece(to);
      PutPiece(from, piece);

      if (captured)
        PutPiece(to, captured);
    }
  }
  else if (piece == KB)
  {
    m_Kings[BLACK] = from;

    if (from == E8 && to == G8)
    {
      RemovePiece(G8);
      RemovePiece(F8);
      PutPiece(E8, KB);
      PutPiece(H8, RB);
    }
    else if (from == E8 && to == C8)
    {
      RemovePiece(C8);
      RemovePiece(D8);
      PutPiece(E8, KB);
      PutPiece(A8, RB);
    }
    else
    {
      RemovePiece(to);
      PutPiece(from, piece);

      if (captured)
        PutPiece(to, captured);
    }
  }
  else if (piece == PW && to == m_ep)
  {
    RemovePiece(to);
    PutPiece(from, PW);
    PutPiece(to + 8, PB);
  }
  else if (piece == PB && to == m_ep)
  {
    RemovePiece(to);
    PutPiece(from, PB);
    PutPiece(to - 8, PW);
  }
  else
  {
    RemovePiece(to);
    PutPiece(from, piece);

    if (captured)
      PutPiece(to, captured);
  }
}
////////////////////////////////////////////////////////////////////////////////

void Position::UnmakeNullMove()
{
  if (m_undoCnt <= 0)
    return;

  Undo& undo = m_undos[--m_undoCnt];
  m_ep = undo.m_ep;
  m_castlings = undo.m_castlings;
  m_fifty = undo.m_fifty;

  m_side ^= 1;
  m_hash ^= s_Zobrist0;
  --m_ply;
}
////////////////////////////////////////////////////////////////////////////////

