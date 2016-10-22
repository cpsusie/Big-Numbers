//  GREKO Chess Engine
//  (c) 2002-2011 Vladimir Medvedev <vrm@bk.ru>
//  http://greko.110mb.com

//  moves.h: bitboard moves generator
//  modified: 1-Aug-2011

#ifndef MOVES_H
#define MOVES_H

#include "position.h"

struct MoveEntry
{
  MoveEntry() {}
  MoveEntry(Move mv) : m_mv(mv) {}
  ~MoveEntry() {}

  Move m_mv;
  EVAL m_value;
};

class MoveList
{
public:

  MoveList() {}
  ~MoveList() {}

  MoveEntry& operator[] (int n) { return m_data[n]; }
  const MoveEntry& operator[] (int n) const { return m_data[n]; }

  void Clear() { m_size = 0; }
  void AddSimpleChecks(const Position& pos);
  void GenAllMoves(const Position& pos);
  void GenCapturesAndPromotions(const Position& pos);
  void GenCheckEvasions(const Position& pos);
  Move GetNthBest(int n);
  int  Size() const { return m_size; }
  
private:

  inline void Add(FLD from, FLD to, PIECE piece)
  {
    m_data[m_size++].m_mv = Move(from, to, piece);
  }

  inline void Add(FLD from, FLD to, PIECE piece, PIECE captured)
  {
    m_data[m_size++].m_mv = Move(from, to, piece, captured);
  }

  inline void Add(FLD from, FLD to, PIECE piece, PIECE captured, PIECE promotion)
  {
    m_data[m_size++].m_mv = Move(from, to, piece, captured, promotion);
  }

  enum { MAX_SIZE = 256 };
  MoveEntry m_data[MAX_SIZE];
  int m_size;
};
////////////////////////////////////////////////////////////////////////////////

#endif

