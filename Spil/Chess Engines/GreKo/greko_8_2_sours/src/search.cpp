//  GREKO Chess Engine
//  (c) 2002-2011 Vladimir Medvedev <vrm@bk.ru>
//  http://greko.110mb.com

//  search.cpp: chess tree search
//  modified: 1-Aug-2011

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif

#include "book.h"
#include "commands.h"
#include "eval.h"
#include "moves.h"
#include "notation.h"
#include "search.h"
#include "utils.h"

extern Position   g_pos;
extern PROTOCOL_T g_protocol;
extern list<string> g_commandQueue;

inline bool IsUCI() { return g_protocol == UCI; }

void Search::AdjustKNPS()
{
  if (m_limitStrength)
  {
    int timeToSleep = 1000 * int(m_nodes) / m_npsLimit - GetSearchTime();
    if (m_restMillisec)
    {
      if (GetSearchTime() + timeToSleep > m_stHard)
        timeToSleep = m_stHard - GetSearchTime();
    }
    if (timeToSleep > 0)
      SleepMilliseconds(timeToSleep);
  }
}
////////////////////////////////////////////////////////////////////////////////

EVAL Search::AlphaBetaRoot(EVAL alpha, EVAL beta, const int depth)
{
  int ply = 0;
  bool inCheck = m_pos.InCheck();
  m_PV[ply].clear();
  m_nodes++;

  for (int j = 0; j < MAX_BRANCH; ++j)
  {
    m_multiPVs[j].m_score = -INFINITY_SCORE;
    m_multiPVs[j].m_pv.clear();
  }

  Move hash_mv = 0;
  U8 hash_type = HASH_ALPHA;
  HashEntry* pentry = ProbeHash();
  if (pentry)
  {
    hash_mv = pentry->m_mv;
  }

  MoveList& mvlist = m_lists[ply];
  if (inCheck)
    mvlist.GenCheckEvasions(m_pos);
  else
    mvlist.GenAllMoves(m_pos);
  UpdateScores(mvlist, hash_mv, ply);

  EVAL e = 0;
  int legalMoves = 0;
  Move best_mv = 0;

  for (int i = 0; i < mvlist.Size(); ++i)
  {
    AdjustKNPS();
    Move mv = mvlist.GetNthBest(i);
    if (m_pos.MakeMove(mv))
    {
      legalMoves++;
      m_histTry[mv.Piece()][mv.To()] += depth;

      if (IsUCI() && GetSearchTime() > 1000)
      {
        char mvbuf[16];
        out("info currmove %s", MoveToStrLong(mv, mvbuf));
        out(" currmovenumber %d\n", legalMoves);
      }

      int newDepth = depth - 1;
      bool givesCheck = m_pos.InCheck();

      //
      //   EXTENSIONS
      //

      if (givesCheck)
        ++newDepth;
      else if (mv.Piece() == PW && (mv.To() / 8) == 1)
        ++newDepth;
      else if (mv.Piece() == PB && (mv.To() / 8) == 6)
        ++newDepth;
      
      if (m_multiPV > 1)
        e = -AlphaBeta(-beta - VAL_Q, -alpha + VAL_Q, newDepth, ply + 1, false);
      else
      {
        if (legalMoves == 1)
          e = -AlphaBeta(-beta, -alpha, newDepth, ply + 1, false);
        else
        {
          e = -AlphaBeta(-alpha - 1, -alpha, newDepth, ply + 1, false);
          if (e > alpha && e < beta)
            e = -AlphaBeta(-beta, -alpha, newDepth, ply + 1, false);
        }
      }
      m_pos.UnmakeMove();

      if (m_flag)
        return alpha;

      if (legalMoves == 1)
      {
        best_mv = mv;
        m_PV[ply].clear();
        m_PV[ply].push_back(mv);
        m_PV[ply].insert(m_PV[ply].end(), m_PV[ply + 1].begin(), m_PV[ply + 1].end());
      }

      //
      //   Update multipv
      //

      if (legalMoves < MAX_BRANCH)
      {
        MultiPVEntry *mpv = &(m_multiPVs[legalMoves - 1]);
        
        mpv->m_pv.clear();
        mpv->m_score = e;
        mpv->m_pv.push_back(mv);
        mpv->m_pv.insert(mpv->m_pv.end(), m_PV[ply + 1].begin(), m_PV[ply + 1].end());
      }

      if (e > alpha)
      {
        best_mv = mv;
        if (!mv.Captured())
          m_histSuccess[mv.Piece()][mv.To()] += depth;
        hash_type = HASH_EXACT;

        m_PV[ply].clear();
        m_PV[ply].push_back(mv);
        m_PV[ply].insert(m_PV[ply].end(), m_PV[ply + 1].begin(), m_PV[ply + 1].end());

        alpha = e;

        if (alpha >= beta)
        {
          hash_type = HASH_BETA;
          if (!mv.Captured())
          {
            if (e > CHECKMATE_SCORE - MAX_PLY && e <= CHECKMATE_SCORE)
              m_matekillers[ply] = mv;
            else
              m_killers[ply] = mv;
          }
          break;
        }
      }
    }
  }
  if (legalMoves == 0)
  {
    if (inCheck)
      alpha = -CHECKMATE_SCORE + ply;
    else
      alpha = DRAW_SCORE;
  }
  RecordHash(best_mv, depth, alpha, hash_type, ply);
  return alpha;
}
////////////////////////////////////////////////////////////////////////////////

EVAL Search::AlphaBeta(EVAL alpha, EVAL beta, const int depth, int ply, bool isNull)
{
  m_PV[ply].clear();
  ++m_nodes;

  COLOR side = m_pos.Side();
  bool onPV = (beta - alpha > 1);
  bool inCheck = m_pos.InCheck();
  bool lateEndgame = m_pos.MatIndex(side) < 5;

  AdjustKNPS();
  CheckLimits();
  if (m_nodes % 8192 == 0)
    CheckInput();

  //
  //   DRAW DETECTION
  //

  if (ply >= MAX_PLY)
    return DRAW_SCORE;

  if (m_pos.IsDraw())
    return DRAW_SCORE;

  if (!isNull)
  {
    int rep_total = m_pos.GetRepetitions();
    if (rep_total >= 2)
      return DRAW_SCORE;
  }

  //
  //   PROBING HASH
  //

  Move hash_mv = 0;
  U8 hash_type = HASH_ALPHA;
  HashEntry* pentry = ProbeHash();

  if (pentry)
  {
    hash_mv = pentry->m_mv;
    if (pentry->m_depth >= depth)
    {
      EVAL hash_eval = pentry->m_eval;
      if (hash_eval > CHECKMATE_SCORE - 50 && hash_eval <= CHECKMATE_SCORE)
        hash_eval -= ply;
      else if (hash_eval < -CHECKMATE_SCORE + 50 && hash_eval >= -CHECKMATE_SCORE)
        hash_eval += ply;

      if (pentry->Type() == HASH_EXACT)
        return hash_eval;      
      else if (pentry->Type() == HASH_ALPHA && hash_eval <= alpha)
        return alpha;
      else if (pentry->Type() == HASH_BETA && hash_eval >= beta)
        return beta;
    }
  }

  //
  //   QSEARCH
  //

  if (depth <= 0 && !inCheck)
  {
    --m_nodes;
    return AlphaBetaQ(alpha, beta, ply, 0);
  }

  //
  //   PRUNING
  //

  EVAL MARGIN[4] = { INFINITY_SCORE, VAL_P, VAL_B, VAL_R };
  if (!inCheck && 
      !onPV && 
      (depth > 0) && 
      (depth < 4))
  {
    EVAL staticScore = Evaluate(m_pos, alpha - MARGIN[depth], beta + MARGIN[depth]);
    if (staticScore <= alpha - MARGIN[depth])
      return AlphaBetaQ(alpha, beta, ply, 0);
    if (staticScore >= beta + MARGIN[depth])
      return beta;
  }

  //
  //   NULLMOVE
  //

  const int R = 3;
  do
  {
    if (onPV) break;
    if (inCheck) break;
    if (isNull) break;
    if (lateEndgame) break;
    if (depth < 2) break;

    m_pos.MakeNullMove();
    EVAL nullEval;
    nullEval = -AlphaBeta(-beta, -beta + 1, depth - 1 - R, ply + 1, true);
    m_pos.UnmakeNullMove();

    if (nullEval >= beta)
      return beta;
  }
  while (0);

  MoveList& mvlist = m_lists[ply];
  if (inCheck)
    mvlist.GenCheckEvasions(m_pos);
  else
    mvlist.GenAllMoves(m_pos);
  UpdateScores(mvlist, hash_mv, ply);

  int legalMoves = 0;
  EVAL e = 0;
  Move best_mv = 0;

  for (int i = 0; i < mvlist.Size(); ++i)
  {
    Move mv = mvlist.GetNthBest(i);
    if (m_pos.MakeMove(mv))
    {
      legalMoves++;
      m_histTry[mv.Piece()][mv.To()] += depth;

      int newDepth = depth - 1;
      bool givesCheck = m_pos.InCheck();

      //
      //   EXTENSIONS
      //

      if (givesCheck)
        ++newDepth;
      else if (mv.Piece() == PW && (mv.To() / 8) == 1)
        ++newDepth;
      else if (mv.Piece() == PB && (mv.To() / 8) == 6)
        ++newDepth;

      //
      //   LMR
      //

      bool reduced = false;
      if (!onPV && 
          !inCheck && 
          !mv.Captured() && 
          !mv.Promotion())
      {
        if (legalMoves > 4 && depth > 4 &&
          m_histSuccess[mv.Piece()][mv.To()] <= m_histTry[mv.Piece()][mv.To()] * 0.75)
        {
          --newDepth;
          reduced = true;
        }
      }
      
      if (legalMoves == 1)
        e = -AlphaBeta(-beta, -alpha, newDepth, ply + 1, false);
      else
      {
        e = -AlphaBeta(-alpha - 1, -alpha, newDepth, ply + 1, false);
        if (reduced && e > alpha)
        {
          ++newDepth;
          e = -AlphaBeta(-alpha - 1, -alpha, newDepth, ply + 1, false);
        }
        if (e > alpha && e < beta)
          e = -AlphaBeta(-beta, -alpha, newDepth, ply + 1, false);
      }
      m_pos.UnmakeMove();

      if (m_flag)
        return alpha;

      if (e > alpha)
      {
        best_mv = mv;
        if (!mv.Captured())
          m_histSuccess[mv.Piece()][mv.To()] += depth;
        hash_type = HASH_EXACT;

        alpha = e;
        if (alpha >= beta)
        {
          hash_type = HASH_BETA;
          if (!mv.Captured())
          {
            if (e > CHECKMATE_SCORE - MAX_PLY && e <= CHECKMATE_SCORE)
              m_matekillers[ply] = mv;
            else
              m_killers[ply] = mv;
          }

          break;
        }

        m_PV[ply].clear();
        m_PV[ply].push_back(mv);
        m_PV[ply].insert(m_PV[ply].end(), m_PV[ply + 1].begin(), m_PV[ply + 1].end());
      }
    }
  }
  if (legalMoves == 0)
  {
    if (inCheck)
      alpha = -CHECKMATE_SCORE + ply;
    else
      alpha = DRAW_SCORE;
  }
  else if (m_pos.Fifty() >= 100)
    alpha = DRAW_SCORE;

  RecordHash(best_mv, depth, alpha, hash_type, ply);
  return alpha;
}
////////////////////////////////////////////////////////////////////////////////

EVAL Search::AlphaBetaQ(EVAL alpha, EVAL beta, int ply, int qply)
{
  m_PV[ply].clear();
  ++m_nodes;

  AdjustKNPS();
  CheckLimits();
  if (m_nodes % 8192 == 0)
    CheckInput();

  if (ply >= MAX_PLY)
    return DRAW_SCORE;

  bool inCheck = m_pos.InCheck();
  EVAL staticEval = Evaluate(m_pos, alpha, beta);
  if (!inCheck)
  {
    if (staticEval > alpha)
      alpha = staticEval;
  }

  if (alpha >= beta)
    return beta;

  MoveList& mvlist = m_lists[ply];
  if (inCheck)
    mvlist.GenCheckEvasions(m_pos);
  else
  {
    mvlist.GenCapturesAndPromotions(m_pos);
    if (qply < 2)
      mvlist.AddSimpleChecks(m_pos);
  }
  UpdateScores(mvlist, 0, ply);

  EVAL e = 0;
  int legalMoves = 0;
  for (int i = 0; i < mvlist.Size(); ++i)
  {
    Move mv = mvlist.GetNthBest(i);    
    if (!inCheck && SEE(m_pos, mv) < 0)
      continue;

    if (m_pos.MakeMove(mv))
    {
      legalMoves++;
      e = -AlphaBetaQ(-beta, -alpha, ply + 1, qply + 1);
      m_pos.UnmakeMove();

      if (m_flag)
        return alpha;

      if (e > alpha)
      {
        alpha = e;
        if (alpha >= beta)
          break;

        m_PV[ply].clear();
        m_PV[ply].push_back(mv);
        m_PV[ply].insert(m_PV[ply].end(), m_PV[ply + 1].begin(), m_PV[ply + 1].end());
      }
    }
  }

  if (inCheck && legalMoves == 0)
    alpha = -CHECKMATE_SCORE + ply;

  return alpha;
}
////////////////////////////////////////////////////////////////////////////////

void Search::CheckInput()
{
  if (m_rootPV.empty())
    return;

  if (InputAvailable())
  {
    char s[4096];
    ReadInput(s, sizeof(s));

    if (m_mode == ANALYZE)
    {
      Move mv = StrToMove(s, g_pos);
      if (mv)
      {
        m_flag = true;
        g_commandQueue.push_back("force");
        g_commandQueue.push_back(s);
        g_commandQueue.push_back("analyze");
      }
      else if (Is(s, "board", 1))
        g_pos.Print();
      else if (Is(s, "quit", 1))
        exit(0);
      else if (Is(s, "isready", 7))
        out("readyok\n");
      else if (Is(s, "exit", 2))
        m_flag = true;
      else if (Is(s, "stop", 2))
        m_flag = true;
      else if (Is(s, "new", 3))
        m_flag = true;
      else if (Is(s, "setoption", 8))
      {
        char *token = strtok(s, " ");
        token = strtok(NULL, " ");
        token = strtok(NULL, " ");
        if (token && !strcmp(token, "MultiPV"))
        {
          token = strtok(NULL, " ");
          token = strtok(NULL, " ");
          m_multiPV = atoi(token);
        }
      }
      else if (Is(s, "undo", 4))
      {
        m_flag = true;
        g_commandQueue.push_back("undo");
        g_commandQueue.push_back("analyze");
      }
      else if (Is(s, "remove", 6))
      {
        m_flag = true;
        g_commandQueue.push_back("undo");
        g_commandQueue.push_back("analyze");
      }
    }

    else if (m_mode == THINKING)
    {
      if (Is(s, "quit", 1))
        exit(0);
      else if (Is(s, "isready", 7))
        out("readyok\n");
      else if (Is(s, "?", 1))
        m_flag = true;
      else if (Is(s, "stop", 4))
        m_flag = true;
      else if (Is(s, "new", 3))
        m_flag = true;
      else if (Is(s, "result", 6))
        m_flag = true;
      else if (Is(s, "setoption", 8))
      {
        char *token = strtok(s, " ");
        token = strtok(NULL, " ");
        token = strtok(NULL, " ");
        if (token && !strcmp(token, "MultiPV"))
        {
          token = strtok(NULL, " ");
          token = strtok(NULL, " ");
          m_multiPV = atoi(token);
        }
      }
    }

    else if (m_mode == EPDTEST)
    {
      if (Is(s, "board", 1))
        g_pos.Print();
      else if (Is(s, "quit", 1))
        exit(0);
      else if (Is(s, "exit", 2))
        m_flag = true;
      else if (Is(s, "new", 3))
        m_flag = true;
    }
  }
}
////////////////////////////////////////////////////////////////////////////////

void Search::CheckLimits()
{
  if (m_rootPV.empty())
    return;

  if (m_stHard)
  {
    if (GetSearchTime() >= m_stHard)
    {
      if (m_mode == THINKING || m_mode == EPDTEST)
        m_flag = true;
    }
  }

  if (m_sn && m_nodes >= m_sn)
  {
    if (m_mode == THINKING || m_mode == EPDTEST)
      m_flag = true;
  }
}
////////////////////////////////////////////////////////////////////////////////

void Search::ClearHash()
{
  memset(m_hash, 0, m_hashSize * sizeof(HashEntry));
}
////////////////////////////////////////////////////////////////////////////////

void Search::ClearHistory()
{  
  memset(m_histTry, 0, 64 * 14 * sizeof(m_histTry[0][0]));
  memset(m_histSuccess, 0, 64 * 14 * sizeof(m_histSuccess[0][0]));
}
////////////////////////////////////////////////////////////////////////////////

void Search::ClearKillers()
{
  memset(m_killers, 0, MAX_PLY * sizeof(Move));
  memset(m_matekillers, 0, MAX_PLY * sizeof(Move));
}
////////////////////////////////////////////////////////////////////////////////

void Search::Epdtest(FILE* psrc, double time_in_seconds, int reps)
{
  char fen[256];
  Position tmp = g_pos;

  int dt = int(1000 * time_in_seconds);
  SetLimits(0, 0, 0, dt, dt);

  int total = 0, solved = 0;
  double sqtime = 0.0, sec = 0.0;

  while (fgets(fen, 256, psrc))
  {
    int res = g_pos.SetFEN(fen);
    out(fen);
    out("\n");

    if (!res)
    {
      g_pos = tmp;
      return;
    }

    if (!StartEpd(g_pos, fen, reps, &total, &solved, &sec))
    {
      g_pos = tmp;
      return;
    }

    sqtime += sec * sec;
    out("\nScore: %d / ", solved);
    out("%d\n", total);
    out("<Sq. time> = %lf\n\n", sqtime / total);
  }

  g_pos = tmp;
}
////////////////////////////////////////////////////////////////////////////////

NODES Search::Perft(int depth)
{
  if (depth == 0)
    return 1;

  MoveList& mvlist = m_lists[depth];
  mvlist.GenAllMoves(m_pos);
  NODES sum = 0, delta = 0;

  for (int i = 0; i < mvlist.Size(); ++i)
  {
    Move mv = mvlist[i].m_mv;
    if (m_pos.MakeMove(mv))
    {
      delta = Perft(depth - 1);
      sum += delta;
      m_pos.UnmakeMove();
    }
  }

  return sum;
}
////////////////////////////////////////////////////////////////////////////////

void Search::PrintPV(int iter, EVAL e, const char* comment)
{
  if (m_limitStrength)
  {
    int timeToSleep = 1000 * int(m_nodes) / m_npsLimit - GetSearchTime();
    bool canSleep = timeToSleep > 0;
    if (m_restMillisec)
    {
      if (GetSearchTime() + timeToSleep < m_restMillisec / 2)
        canSleep = false;
    }
    if (canSleep)
      SleepMilliseconds(timeToSleep);
  }

  char buf[16];
  Move mv = 0;

  if (IsUCI())
  {
    for (int j = 0; j < MAX_BRANCH; ++j)
      m_multiPVs[j].m_seen = false;

    for (int j = 1; j <= m_multiPV; ++j)
    {
      EVAL best_score = -INFINITY_SCORE;
      MultiPVEntry *best_mpv = NULL;  
      MultiPVEntry *mpv = NULL;

      for (int k = 0; k < MAX_BRANCH; ++k)
      {
        mpv = &(m_multiPVs[k]);
        if (mpv->m_seen)
          continue;
        if (mpv->m_pv.empty())
          break;
        if (mpv->m_score > best_score)
        {
          best_score = mpv->m_score;
          best_mpv = mpv;
        }
      }

      if (best_mpv)
      {
        best_mpv->m_seen = true;

        mpv = best_mpv;
        out("info multipv %d ", j);
        out("depth %d ", iter);
        out("score");
        if (mpv->m_score > CHECKMATE_SCORE - 50)
        {
          out(" mate %d", 1 + (CHECKMATE_SCORE - mpv->m_score) / 2);
        }
        else if (mpv->m_score < -CHECKMATE_SCORE + 50)
        {
          out(" mate -%d", (mpv->m_score + CHECKMATE_SCORE) / 2);
        }
        else
          out(" cp %d", mpv->m_score);

        out(" time %ld", GetSearchTime());
        out(" nodes %ld", m_nodes);
        if (GetSearchTime() > 0)
        {
          long knps = long(m_nodes / GetSearchTime());
          printf (" nps %ld", (1000 * knps));
        }
        out(" pv ");
        for (size_t m = 0; m < mpv->m_pv.size(); ++m)
        {
          mv = mpv->m_pv[m];
          out("%s ", MoveToStrLong(mv, buf));
        }
        out("\n");
      }
    }
    if (GetSearchTime() > 0)
    {
      out("info time %ld", GetSearchTime());
      out(" nodes %ld", m_nodes);
      long knps = long(m_nodes / GetSearchTime());
      out (" nps %ld\n", (1000 * knps));
    }
    return;
  }
  else
  {
    out(" %2d", iter);
    out(" %9d ", e);
    out(" %7d ", GetSearchTime() / 10);
    out(" %12d ", m_nodes);
    out(" ");
  }

  Position tmp = m_pos;

  int movenum = tmp.Ply() / 2 + 1;
  for (size_t m = 0; m < m_rootPV.size(); ++m)
  {
    mv = m_rootPV[m];
    if (tmp.Side() == WHITE)
    {
      out("%d. ", movenum++);
    }
    else if (m == 0)
    {
      out("%d. ... ", movenum++);
    }

    MoveList mvlist;
    mvlist.GenAllMoves(tmp);
    out("%s", MoveToStrShort(mv, tmp, buf));
    tmp.MakeMove(mv);

    if (tmp.InCheck())
    {
      if (int(e + m + 1) == CHECKMATE_SCORE || int(e - m - 1) == -CHECKMATE_SCORE)
      {
        out("#");

        if (e > 0)
          out(" {+");
        else
          out(" {-");

        out("Mate in %d}", m / 2 + 1);
      }
      else
        out("+");
    }

    if (m == 0)
      out(comment);

    out(" ");
  }

  out("\n");
}
////////////////////////////////////////////////////////////////////////////////

HashEntry* Search::ProbeHash()
{
  long index = long(m_pos.Hash() % m_hashSize);
  if (m_hash[index].m_hash == m_pos.Hash())
    return m_hash + index;
  else
    return NULL;
}
////////////////////////////////////////////////////////////////////////////////

void Search::RecordHash(Move best_mv, I8 depth, EVAL eval, U8 type, int ply)
{
  long index = long(m_pos.Hash() % m_hashSize);
  HashEntry* pentry = m_hash + index;

  bool canWrite = false;
  if (pentry->Age() != m_hashAge)
    canWrite = true;
  else
  {
    switch (pentry->Type())
    {
      case HASH_UNKNOWN:
        canWrite = true;
        break;
      case HASH_ALPHA:
        if (type == HASH_ALPHA) canWrite = (depth >= pentry->m_depth);
        if (type == HASH_BETA)  canWrite = true;
        if (type == HASH_EXACT) canWrite = true;
        break;
      case HASH_BETA:
        if (type == HASH_ALPHA) canWrite = false;
        if (type == HASH_BETA)  canWrite = (depth >= pentry->m_depth);
        if (type == HASH_EXACT) canWrite = true;
        break;
      case HASH_EXACT:
        if (type == HASH_ALPHA) canWrite = false;
        if (type == HASH_BETA)  canWrite = false;
        if (type == HASH_EXACT) canWrite = (depth >= pentry->m_depth);
        break;
      default:
        break;
    }
  }

  if (!canWrite)
    return;

  if (eval > CHECKMATE_SCORE - 50 && eval <= CHECKMATE_SCORE)
    eval += ply;
  else if (eval < -CHECKMATE_SCORE + 50 && eval >= -CHECKMATE_SCORE)
    eval -= ply;

  pentry->m_depth = depth;
  pentry->m_eval = eval;
  pentry->m_hash = m_pos.Hash();
  pentry->m_mv = best_mv;
  pentry->m_flags = type | m_hashAge;
}
////////////////////////////////////////////////////////////////////////////////

void Search::SetElo(int rating)
{
  int R0 = 2100;
  m_npsLimit = static_cast<int> (200000 * pow(10, (rating - R0) / 400.));
}
////////////////////////////////////////////////////////////////////////////////

void Search::SetHashMB(double mb)
{
  if (m_hash)
    delete[] m_hash;

  m_hashSize = long (1024 * 1024 * mb / sizeof(HashEntry));
  if (m_hashSize <= 0)
    m_hashSize = 1;

  m_hash = new HashEntry[m_hashSize];

  if (!IsUCI())
  {
    out("main hash: %8ld nodes = ", m_hashSize);
    out("%lf MB\n", m_hashSize * sizeof(HashEntry) / (1024. * 1024.));
  }
}
////////////////////////////////////////////////////////////////////////////////

void Search::SetLimits(int restMillisec, int sd, NODES sn, int stHard, int stSoft)
{
  m_restMillisec = restMillisec;
  m_sd = sd;
  m_sn = sn;
  m_stHard = stHard;
  m_stSoft = stSoft;
}
////////////////////////////////////////////////////////////////////////////////

void Search::StartAnalyze(const Position& pos)
{
  m_hashAge = (m_hashAge + 1) & 0x0f;

  SetLimits(0, 0, 0, 0, 0);
  m_mode = ANALYZE;
  m_flag = false;

  m_nodes = 0;
  m_start_time = clock();

  m_pos = pos;
  ClearHash();
  ClearHistory();
  ClearKillers();

  if (!IsUCI())
  {
    char buf[4096];
    out("\n%s\n\n", m_pos.FEN(buf));
  }

  EVAL alpha = -INFINITY_SCORE;
  EVAL beta = INFINITY_SCORE;

  for (m_iter = 1; m_iter < MAX_PLY; m_iter++)
  {
    int print_iter = m_iter;
    EVAL e = AlphaBetaRoot(alpha, beta, m_iter);

    if (m_flag)
      break;

    if (e > alpha)
    {
      m_rootPV = m_PV[0];
    }

    if (e > alpha && e < beta)
    {
      PrintPV(print_iter, e, "");

      alpha = e - m_rootWindow / 2;
      beta = e + m_rootWindow / 2;
    }
    else
    {
      PrintPV(print_iter, e, (e <= alpha)? "?" : "!");

      alpha = -INFINITY_SCORE;
      beta = INFINITY_SCORE;
      m_iter--;
    }
  }
}
////////////////////////////////////////////////////////////////////////////////

int Search::StartEpd(const Position& pos, const char* fen, int reps, int* ptotal, int* psolved, double* psec)
{
  m_hashAge = (m_hashAge + 1) & 0x0f;
  
  m_mode = EPDTEST;
  m_flag = false;
  m_nodes = 0;
  m_start_time = clock();

  m_pos = pos;
  ClearHash();
  ClearHistory();
  ClearKillers();

  int good_iters = 0;
  int sufficient_iters = reps;
  int prev_iter = 0;

  EVAL alpha = -INFINITY_SCORE;
  EVAL beta = INFINITY_SCORE;

  *psec = m_stHard / 1000.;
  for (m_iter = 1; m_iter < MAX_PLY; m_iter++)
  {
    int print_iter = m_iter;

    int move_found = 0;
    EVAL score = AlphaBetaRoot(alpha, beta, m_iter);

    if (m_flag)
      break;

    if (score > alpha)
    {
      m_rootPV = m_PV[0];
    }

    if (!m_rootPV.empty())
    {
      Move mv = m_rootPV[0];
      char mvstr[16];
      MoveToStrShort(mv, m_pos, mvstr);

      if (strstr(fen, mvstr) != NULL)
        move_found = 1;
      else
      {
        MoveToStrLong(mv, mvstr);
        if (strstr(fen, mvstr) != NULL)
          move_found = 1;
      }

      if (strstr(fen, "am") && !strstr(fen, "bm"))
        move_found = 1 - move_found;
    }

    char comment[3] = "";

    if (score > alpha && score < beta)
    {
      strcpy(comment, "");
      alpha = score - m_rootWindow / 2;
      beta = score + m_rootWindow / 2;
    }
    else
    {
      strcpy(comment, (score <= alpha)? "?" : "!");
      alpha = -INFINITY_SCORE;
      beta = INFINITY_SCORE;
      m_iter--;
    }

    if (!move_found)
    {
      good_iters = 0;
    }
    else if (m_iter > prev_iter)
    {
      good_iters++;
      prev_iter = m_iter;
    }

    if (move_found)
    {
      Highlight(1);
      out(" yes ");
      Highlight(0);

      if (good_iters == 1)
        *psec = GetSearchTime() / 1000.;
    }
    else
    {
      out("  no ");
      *psec = m_stHard / 1000.;
    }

    int complete_iter = (score > alpha && score < beta);

    if (good_iters >= sufficient_iters && complete_iter)
      Highlight(1);

    PrintPV(print_iter, score, comment);
    Highlight(0);

    if (good_iters >= sufficient_iters && complete_iter)
      break;
  }

  (*ptotal)++;
  if (good_iters > 0)
    (*psolved)++;

  return 1;
}
////////////////////////////////////////////////////////////////////////////////

void Search::StartThinking(Position& pos)
{
  m_hashAge = (m_hashAge + 1) & 0x0f;
  
  m_rootPV.clear();
  EVAL e = 0;
  int print_iter = 0;
  char comment[32];

  if (pos.GameOver(comment))
  {
    out(comment);
    return;
  }

  m_mode = THINKING;
  m_flag = false;
  m_nodes = 0;
  m_start_time = clock();

  m_pos = pos;
  // ClearHash();
  ClearHistory();
  ClearKillers();

  if (!IsUCI())
  {
    char buf[4096];
    out("\n%s\n\n", m_pos.FEN(buf));
  }

  EVAL alpha = -INFINITY_SCORE;
  EVAL beta = INFINITY_SCORE;
  Move best_move = 0;

  // Book

  char buf[256];
  Move book_move = g_book.GetMove(pos, buf);
  if (book_move)
  {
    best_move = book_move;
    if (!IsUCI())
    {
      out(" 0 0 0 0      (");
      out(buf);
      out(")\n");
    }
    m_flag = true;
    goto MAKE_MOVE;
  }

  for (m_iter = 1; m_iter < MAX_PLY; ++m_iter)
  {
    e = AlphaBetaRoot(alpha, beta, m_iter);
    if (m_flag)
      break;

    if (e > alpha)
    {
      m_rootPV = m_PV[0];
    }

    if (e > alpha && e < beta)
    {
      print_iter = m_iter;
      strcpy(comment, "");
      
      alpha = e - m_rootWindow / 2;
      beta = e + m_rootWindow / 2;

      if (m_stSoft)
      {
        if (GetSearchTime() >= m_stSoft)
          m_flag = true;
      }
      if (m_sd)
      {
        if (m_iter >= m_sd)
          m_flag = true;
      }
    }
    else
    {
      strcpy(comment, (e <= alpha)? "?" : "!");
      
      // Additional time
      if (m_restMillisec)
      {
        if (m_restMillisec > 10 * m_stHard)
        {
          int delta = m_stHard / 2;
          m_stHard += delta;
          m_stSoft += delta;
        }
      }
 
      // Opening window
      if (e <= alpha)
        alpha = -INFINITY_SCORE;
      else
        beta = INFINITY_SCORE;
      --m_iter;
    }

    PrintPV(print_iter, e, comment);
    best_move = m_rootPV[0];

    if (best_move && (m_iter + e >= CHECKMATE_SCORE))
      m_flag = true;
  }

  if (m_iter == MAX_PLY && best_move)
    m_flag = true;

MAKE_MOVE:

  if (m_flag)
  {
    char buf[16];
    if (IsUCI())
    {
      out("\nbestmove %s\n", MoveToStrLong(best_move, buf));
    }
    else
    {
      Highlight(1);
      out("\nmove %s\n\n", MoveToStrLong(best_move, buf));
      Highlight(0);
    }

    pos.MakeMove(best_move);
  }
}
////////////////////////////////////////////////////////////////////////////////

void Search::StartPerft(const Position& pos, int depth)
{
  m_pos = pos;
  clock_t t0 = clock();
  NODES sum = 0, delta = 0;

  MoveList& mvlist = m_lists[depth];
  mvlist.GenAllMoves(pos);

  out("\n");
  int i = 0;
  for (i = 0; i < mvlist.Size(); ++i)
  {
    Move mv = mvlist[i].m_mv;

    if (m_pos.MakeMove(mv))
    {
      delta = Perft(depth - 1);
      sum += delta;
      m_pos.UnmakeMove();

      char buf[16];
      out(" %s - ", MoveToStrLong(mv, buf));
      out("%d\n", delta);
      if (InputAvailable())
      {
        char s[4096];
        ReadInput(s, sizeof(s));

        if (Is(s, "exit", 2))
          break;
      }
    }
  }

  if (i == mvlist.Size())
  {
    clock_t t1 = clock();

    out("\n Nodes: %d\n", sum);
    out(" Time:  %lf\n", GetSearchTime() / 1000.);
    out(" Knps:  %lf\n\n", 0.001 * (double) sum * CLOCKS_PER_SEC / (t1 - t0));
  }
}
////////////////////////////////////////////////////////////////////////////////

const EVAL SEE_VALUE[14] =
{ 0, 0, 100, 100, 400, 400, 400, 400, 600, 600, 1200, 1200, 20000, 20000 };

EVAL Search::SEE_Exchange(const Position& pos, FLD to, COLOR side, EVAL currScore, EVAL target, U64 occupied)
{
  U64 att = pos.GetAttacks(to, side, occupied) & occupied;
  if (att == 0)
    return currScore;

  FLD from = NF;
  PIECE piece;
  EVAL newTarget = SEE_VALUE[KW] + 1;

  while (att)
  {
    FLD f = PopLSB(att);
    piece = pos[f];
    if (SEE_VALUE[piece] < newTarget) 
    {
      from = f;
      newTarget = SEE_VALUE[piece];
    }
  }

  occupied ^= BB_SINGLE[from];
  EVAL score = - SEE_Exchange(pos, to, side ^ 1, -(currScore + target), newTarget, occupied);
  return (score > currScore)? score : currScore;
}
////////////////////////////////////////////////////////////////////////////////

EVAL Search::SEE(const Position& pos, Move mv)
{
  FLD from = mv.From();
  FLD to = mv.To();
  PIECE piece = mv.Piece();
  PIECE captured = mv.Captured();
  PIECE promotion = mv.Promotion();
  COLOR side = GetColor(piece);

  EVAL score0 = SEE_VALUE[captured];
  if (promotion)
  {
    score0 += SEE_VALUE[promotion] - SEE_VALUE[PW];
    piece = promotion;
  }

  U64 occupied = pos.BitsAll() ^ BB_SINGLE[from];
  EVAL score = - SEE_Exchange(pos, to, side ^ 1, -score0, SEE_VALUE[piece], occupied);

  return score;
}
////////////////////////////////////////////////////////////////////////////////

void Search::UpdateScores(MoveList& mvlist, Move hashmv, int ply)
{
  for (int i = 0; i < mvlist.Size(); ++i)
  {
    Move mv = mvlist[i].m_mv;
    if (mv == hashmv)
    {
      mvlist[i].m_value = SORT_HASH;
      continue;
    }

    PIECE piece = mv.Piece();
    PIECE captured = mv.Captured();
    PIECE promotion = mv.Promotion();
    FLD to = mv.To();

    mvlist[i].m_value = 0;

    if (captured || promotion)
    {
      mvlist[i].m_value = SORT_CAPTURE + SEE(m_pos, mv);
    }
    else
    {
      if (mv == m_matekillers[ply])
        mvlist[i].m_value = SORT_MATEKILLER;
      else if (mv == m_killers[ply])
        mvlist[i].m_value = SORT_KILLER;
      else
      {
        if (m_histTry[piece][to])
          mvlist[i].m_value = 100 * m_histSuccess[piece][to] / m_histTry[piece][to];
      }
    }
  }
}
////////////////////////////////////////////////////////////////////////////////
