//  GREKO Chess Engine
//  (c) 2002-2011 Vladimir Medvedev <vrm@bk.ru>
//  http://greko.110mb.com

//  search.h: chess tree search
//  modified: 1-Aug-2011

#ifndef SEARCH_H
#define SEARCH_H

#include <math.h>
#include "position.h"

enum
{
  HASH_UNKNOWN = 0x00, 
  HASH_ALPHA   = 0x10, 
  HASH_BETA    = 0x20, 
  HASH_EXACT   = 0x40
};

struct HashEntry
{
  I8   m_depth;
  EVAL m_eval;
  U64  m_hash;
  Move m_mv;
  U8   m_flags;

  U8 Age() const { return m_flags & 0x0f; }
  U8 Type() const { return m_flags & 0xf0; }
};

const EVAL SORT_HASH       = 0x40000000;
const EVAL SORT_CAPTURE    = 0x20000000;
const EVAL SORT_MATEKILLER = 0x18000000;
const EVAL SORT_KILLER     = 0x10000000;

class Search
{
public:
  Search() : 
      m_mode(IDLE),
      m_hash(NULL),
      m_hashAge(0),
      m_hashSize(0),
      m_inc(0),
      m_limitStrength(false),
      m_multiPV(1),
      m_rootWindow(VAL_P),
      m_sd(0), 
      m_sn(0), 
      m_stHard(2000), 
      m_stSoft(2000) {}

  void  AdjustKNPS();
  EVAL  AlphaBetaRoot(EVAL alpha, EVAL beta, const int depth);
  EVAL  AlphaBeta(EVAL alpha, EVAL beta, const int depth, int ply, bool isNull);
  EVAL  AlphaBetaQ(EVAL alpha, EVAL beta, int ply, int qply);
  void  CheckInput();
  void  CheckLimits();
  void  ClearHash();
  void  ClearHistory();
  void  ClearKillers();
  void  Epdtest(FILE* psrc, double time_in_seconds, int reps);
  int   GetIncrement() const { return m_inc; }
  int   GetSearchTime() { return 1000 * (clock() - m_start_time) / CLOCKS_PER_SEC; } // milliseconds
  void  LimitStrength(bool x) { m_limitStrength = x; }
  NODES Perft(int depth);
  void  PrintPV(int iter, EVAL e, const char* comment);
  HashEntry* ProbeHash();
  void  RecordHash(Move best_mv, I8 depth, EVAL eval, U8 type, int ply);
  void  SetElo(int rating);
  void  SetHashMB(double mb);
  void  SetIncrement(int inc) { m_inc = inc; }
  void  SetLimits(int restMillisec, int sd, NODES sn, int stHard, int stSoft);
  void  SetMultiPV(int n) { m_multiPV = n; }
  void  SetNPS(int nps) { m_npsLimit = nps; }
  void  StartAnalyze(const Position& pos);
  int   StartEpd(const Position& pos, const char* fen, int reps, int* ptotal, int* psolved, double* psec);
  void  StartPerft(const Position& pos, int depth);
  void  StartThinking(Position& pos);
  void  UpdateScores(MoveList& mvlist, Move hashmv, int ply);

  static EVAL SEE(const Position& pos, Move mv);
  static EVAL SEE_Exchange(const Position& pos, FLD to, COLOR side, EVAL currScore, EVAL target, U64 occupied);

private:

  enum { MAX_PLY = 64 };
  enum { MAX_BRANCH = 128 };
  enum { IDLE, ANALYZE, THINKING, EPDTEST } m_mode;

  struct MultiPVEntry
  {
    vector<Move> m_pv;
    EVAL m_score;
    bool m_seen;
  };

  bool         m_flag;
  HashEntry*   m_hash;
  U8           m_hashAge;
  long         m_hashSize;
  int          m_histTry[14][64];
  int          m_histSuccess[14][64];
  int          m_inc;
  int          m_iter;
  Move         m_killers[MAX_PLY];
  bool         m_limitStrength;
  MoveList     m_lists[MAX_PLY];
  Move         m_matekillers[MAX_PLY];
  int          m_multiPV;
  MultiPVEntry m_multiPVs[MAX_BRANCH];
  NODES        m_nodes;
  int          m_npsLimit;
  Position     m_pos;
  vector<Move> m_PV[MAX_PLY];
  int          m_restMillisec;
  vector<Move> m_rootPV;
  EVAL         m_rootWindow;
  clock_t      m_start_time;
  int          m_sd;
  NODES        m_sn;
  int          m_stHard;
  int          m_stSoft;
};

#endif

