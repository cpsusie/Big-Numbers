//  GREKO Chess Engine
//  (c) 2002-2011 Vladimir Medvedev <vrm@bk.ru>
//  http://greko.110mb.com

//  commands.cpp: command line interface, uci and winboard protocols
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
#include "version.h"

Search g_search;
extern Position g_pos;

PROTOCOL_T g_protocol = CONSOLE;
bool g_force = false;
list<string> g_commandQueue;

void SetTimeLimits(int restMillisec)
{
  // all values in milliseconds

  int dt = restMillisec / 20 + g_search.GetIncrement();
  if (dt > restMillisec - 1000)
    dt = restMillisec / 20;

  int stHard = dt;
  int stSoft = dt / 2;

  g_search.SetLimits(restMillisec, 0, 0, stHard, stSoft);
}
////////////////////////////////////////////////////////////////////////////////

int Is(const char *s, const char *pattern, size_t min_length)
{
  //
  //   Check if string is command (may be abbreviated)
  //

  char buf[4096];
  strncpy(buf, s, sizeof(buf));
  char* p = buf;
  while (*p)
  {
    if (*p == ' ')
    {
      *p = 0;
      break;
    }
    p++;
  }

  if (strstr(pattern, buf) == pattern)
  {
    if (strlen(buf) >= min_length)
      return 1;
  }
  return 0;
}
////////////////////////////////////////////////////////////////////////////////

void OnBk()
{
  char buf[256];
  g_book.GetMove(g_pos, buf);
  out(" %s\n\n", buf);
}
////////////////////////////////////////////////////////////////////////////////

void OnBook(char* s)
{
  if (!strcmp(s, "book clean"))
    g_book.Clean();
  else if (strstr(s, "book import") == s)
    g_book.Import(s + 12);
  else if (strstr(s, "book load") == s)
    g_book.Load(s + 10);
  else if (strstr(s, "book save") == s)
    g_book.Save(s + 10);
}
////////////////////////////////////////////////////////////////////////////////

void OnEpdtest(char* s)
{
  char* token = strtok(s, " ");
  token = strtok(NULL, " ");
  if (!token)
  {
    out("usage: epdtest <file.epd> <seconds>\n");
  }
  else
  {
    FILE *psrc = fopen(token, "rt");
    if (!psrc)
    {
      out("Unable to open file: %s\n", token);
    }
    else
    {
      double tm = 1.0;
      token = strtok(NULL, " ");
      if (token)
        tm = atof(token);

      int reps = 3;
      token = strtok(NULL, " ");
      if (token)
        reps = atoi(token);

      g_search.Epdtest(psrc, tm, reps);
      fclose(psrc);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////

void OnFEN()
{
  char buf[256];
  out("%s\n\n", g_pos.FEN(buf));
}
////////////////////////////////////////////////////////////////////////////////

void OnGo(char* s)
{
  char *token = strtok(s, " ");
  while (token)
  {
    if (!strcmp(token, "infinite"))
    {
      g_search.SetLimits(0, 0, 0, 0, 0);
      break;
    }
    else if(!strcmp(token, "nodes"))
    {
      token = strtok(NULL, " ");
      if (token)
        g_search.SetLimits(0, 0, atol(token), 0, 0);
    }
    else if(!strcmp(token, "movetime"))
    {
      token = strtok(NULL, " ");
      if (token)
        g_search.SetLimits(atol(token), 0, 0, atol(token), atol(token));
    }
    else if(!strcmp(token, "wtime") && g_pos.Side() == WHITE)
    {
      token = strtok(NULL, " ");
      if (token)
        SetTimeLimits(atoi(token));
    }
    else if(!strcmp(token, "btime") && g_pos.Side() == BLACK)
    {
      token = strtok(NULL, " ");
      if (token)
        SetTimeLimits(atoi(token));
    }
    else if(!strcmp(token, "winc") && g_pos.Side() == WHITE)
    {
      // in UCI time comes in milliseconds
      token = strtok(NULL, " ");
      if (token)
        g_search.SetIncrement(atoi(token));
    }
    else if(!strcmp(token, "binc") && g_pos.Side() == BLACK)
    {
      // in UCI time comes in milliseconds
      token = strtok(NULL, " ");
      if (token)
        g_search.SetIncrement(atoi(token));
    }
    token = strtok(NULL, " ");
  }

  g_force = false;
  g_search.StartThinking(g_pos);
}
////////////////////////////////////////////////////////////////////////////////

void OnEval()
{
  EVAL e = Evaluate(g_pos, -INFINITY_SCORE, INFINITY_SCORE);
  out("eval = %d\n", e);
}
////////////////////////////////////////////////////////////////////////////////

void OnLimit(char* s)
{
  char* token = strtok(s, " "); // "level"
  token = strtok(NULL, " "); // mps
  token = strtok(NULL, " "); // base
  token = strtok(NULL, " "); // inc
  if (token)
    g_search.SetIncrement(1000 * atoi(token)); // in WB increment comes in seconds
}
////////////////////////////////////////////////////////////////////////////////

void OnList()
{
  MoveList mvlist;
  mvlist.GenAllMoves(g_pos);
  for (int i = 0; i < mvlist.Size(); ++i)
  {
    char buf[16];
    out("%s ", MoveToStrLong(mvlist[i].m_mv, buf));
  }
  out(" -- total %d moves\n", mvlist.Size());
}
////////////////////////////////////////////////////////////////////////////////

void OnLoad(const char* s)
{
  //
  //   Load position with given number from file
  //

  char buf[4096];
  strncpy(buf, s, sizeof(buf));

  char* ptoken = strtok(buf, " ");
  ptoken = strtok(NULL, " ");

  FILE* psrc = fopen(ptoken, "rt");
  if (psrc == NULL)
  {
    out("Can't open file: %s\n", ptoken);
    return;
  }

  int line_req = 1;
  int line_read = 0;

  ptoken = strtok(NULL, " ");
  if (ptoken)
    line_req = atoi(ptoken);

  while (fgets(buf, sizeof(buf), psrc))
  {
    line_read++;
    if (line_read == line_req)
      break;
  }

  if (line_read == line_req)
  {
    if (buf[strlen(buf) - 1] == '\n' || buf[strlen(buf) - 1] == '\r')
      buf[strlen(buf) - 1] = 0;

    out("%s\n\n", buf);
    g_pos.SetFEN(buf);
  }
  else
  {
    out("Incorrect line number\n");
  }

  fclose(psrc);
}
////////////////////////////////////////////////////////////////////////////////

void OnMT(char* s)
{
  Position tmp = g_pos;

  char* token = strtok(s, " ");
  token = strtok(NULL, " ");
  if (!token) return;

  FILE* psrc = fopen(token, "rt");
  if (psrc == NULL)
  {
    out("Can't open file: %s\n", token);
    return;
  }

  char buf[4096];
  while (fgets(buf, sizeof(buf), psrc))
  {
    if (buf[strlen(buf) - 1] == '\n' || buf[strlen(buf) - 1] == '\r')
      buf[strlen(buf) - 1] = 0;

    out("%s\n", buf);
    g_pos.SetFEN(buf);

    EVAL e1 = Evaluate(g_pos, -INFINITY_SCORE, INFINITY_SCORE);
    g_pos.Mirror();
    EVAL e2 = Evaluate(g_pos, -INFINITY_SCORE, INFINITY_SCORE);

    if (e1 != e2)
    {
      out("Incorrect evaluation:\n");
      out("e1 = %d\n", e1);
      out("e2 = %d\n", e2);
      break;
    }
  }

  fclose(psrc);

  g_pos = tmp;
  out("\n");
}
////////////////////////////////////////////////////////////////////////////////

void OnNew()
{
  g_force = false;
  g_pos.SetInitial();
}
////////////////////////////////////////////////////////////////////////////////

void OnPerft(char* s)
{
  int depth = 1;
  char *token = strtok(s, " ");
  token = strtok(NULL, " ");
  if (token)
  {
    depth = atoi(token);
    if (depth <= 0)
      depth = 1;
  }
  g_search.StartPerft(g_pos, depth);
}
////////////////////////////////////////////////////////////////////////////////

void OnPing(char* s)
{
  s[1] = 'o';
  out("%s\n", s);
}
////////////////////////////////////////////////////////////////////////////////

void OnPosition(char* s)
{
  char uci_buf[8192];
  strcpy(uci_buf, s);

  char *token = strtok(s, " ");
  token = strtok(NULL, " ");
  if (!strcmp(token, "startpos"))
  {
    g_pos.SetInitial();

    token = strtok(uci_buf, " "); // "position"
    token = strtok(NULL, " ");    // "startpos"
    token = strtok(NULL, " ");    // "moves"
    token = strtok(NULL, " ");    // first move

    while (token)
    {
      Move mv = StrToMove(token, g_pos);
      if (mv)
        g_pos.MakeMove(mv);
      else
        break;

      token = strtok(NULL, " ");
    }
  }
  else if (!strcmp(token, "fen"))
  {
    g_pos.SetFEN(s + 13);

    char *p = strstr(uci_buf, "moves");
    if (p)
    {
      token = strtok(p, " ");    // "moves"
      token = strtok(NULL, " "); // first move

      while (token)
      {
        Move mv = StrToMove(token, g_pos);
        g_pos.MakeMove(mv);
        token = strtok(NULL, " ");
      }
    }
  }
}
////////////////////////////////////////////////////////////////////////////////

void OnProtover()
{
  out("\nfeature myname=\"%s\"", VERSION);
  out(" setboard=1 analyze=1 colors=0 san=0 ping=1 name=1 done=1\n\n");
}
////////////////////////////////////////////////////////////////////////////////

void OnRemove()
{
  g_pos.UnmakeMove();
  g_pos.UnmakeMove();
}
////////////////////////////////////////////////////////////////////////////////

void OnSD(char* s)
{
  char *token = strtok(s, " ");
  token = strtok(NULL, " ");
  if (token)
  {
    int sd = atoi(token);
    g_search.SetLimits(0, sd, 0, 0, 0);
  }
}
////////////////////////////////////////////////////////////////////////////////

void OnSetoption(char* s)
{
  char *token = strtok(s, " ");

  token = strtok(NULL, " ");
  token = strtok(NULL, " ");
  if (!token) return;

  if (!strcmp(token, "MultiPV"))
  {
    token = strtok(NULL, " ");
    token = strtok(NULL, " ");
    if (token) g_search.SetMultiPV(atoi(token));
  }
  else if (!strcmp(token, "Hash"))
  {
    token = strtok(NULL, " ");
    token = strtok(NULL, " ");
    if (token) g_search.SetHashMB(atof(token));
  }
  else if (!strcmp(token, "UCI_LimitStrength"))
  {
    token = strtok(NULL, " ");
    token = strtok(NULL, " ");
    if (token)
    {
      if (!strcmp(token, "false") || !strcmp(token, "0"))
        g_search.LimitStrength(false);
      else
        g_search.LimitStrength(true);
    }
  }
  else if (!strcmp(token, "UCI_Elo"))
  {
    token = strtok(NULL, " ");
    token = strtok(NULL, " ");
    if (token) g_search.SetElo(atoi(token));
  }
  else if (!strcmp(token, "LimitKNPS"))
  {
    token = strtok(NULL, " ");
    token = strtok(NULL, " ");
    if (token)
    {
      int knps = atoi(token);
      if (knps != 9999)
      {
        g_search.LimitStrength(true);
        g_search.SetNPS(1000 * knps);
      }
    }
  }
}
////////////////////////////////////////////////////////////////////////////////

void OnSN(char* s)
{
  char *token = strtok(s, " ");
  token = strtok(NULL, " ");
  if (token)
  {
    NODES sn = atol(token);
    g_search.SetLimits(0, 0, sn, 0, 0);
  }
}
////////////////////////////////////////////////////////////////////////////////

void OnST(char* s)
{
  char *token = strtok(s, " ");
  token = strtok(NULL, " ");
  if (token)
  {
    int dt = int(1000 * atof(token));
    g_search.SetLimits(dt, 0, 0, dt, dt);
  }
}
////////////////////////////////////////////////////////////////////////////////

void OnTime(char* s)
{
  char *token = strtok(s, " ");
  token = strtok(NULL, " ");
  if (token)
    SetTimeLimits(10 * atoi(token)); // in WB time comes in centiseconds
}
////////////////////////////////////////////////////////////////////////////////

void OnUCI()
{
  g_protocol = UCI;

  out("id name ");
  out(VERSION);
  out("\n");
  out("id author Vladimir Medvedev\n");
  out("option name Hash type spin default 1 min 1 max 1024\n");
  out("option name MultiPV type spin default 1 min 1 max 64\n");
  out("option name UCI_LimitStrength type check default false\n");
  out("option name UCI_Elo type spin default 2100 min 1200 max 2400\n");
  out("option name LimitKNPS type spin default 9999 min 1 max 9999\n");
  out("uciok\n");
}
////////////////////////////////////////////////////////////////////////////////

void RunCommandLine()
{
  //
  //   Read user input and process commands
  //

  out("\n");
  char s[4096];
  const char* sn[2] = { "White", "Black" };

  while (1)
  {
    if (g_commandQueue.empty())
    {
      if (g_protocol == CONSOLE)
        out("%s(%d): ", sn[g_pos.Side()], g_pos.Ply() / 2 + 1);

      ReadInput(s, sizeof(s));
    }
    else
    {
      string cmd = g_commandQueue.front();
      g_commandQueue.pop_front();
      strncpy(s, cmd.c_str(), sizeof(s));
    }

    Move mv = StrToMove(s, g_pos);
    if (mv)
    {
      g_pos.MakeMove(mv);
      if (!g_force)
        g_search.StartThinking(g_pos);

      continue;
    }

#define ON_CMD(pattern, minLen, action) \
   if (Is(s, #pattern, minLen))         \
   {                                    \
      action;                           \
      continue;                         \
   }

    ON_CMD (analyze,   2, g_search.StartAnalyze(g_pos))
    ON_CMD (bk,        2, OnBk())
    ON_CMD (board,     1, g_pos.Print())
    ON_CMD (book,      4, OnBook(s))
    ON_CMD (epdtest,   3, OnEpdtest(s))
    ON_CMD (eval,      1, OnEval())
    ON_CMD (fen,       3, OnFEN())
    ON_CMD (force,     5, g_force = true)
    ON_CMD (go,        2, OnGo(s))
    ON_CMD (isready,   7, out("readyok\n"))
    ON_CMD (limit,     2, OnLimit(s))
    ON_CMD (list,      2, OnList())
    ON_CMD (load,      2, OnLoad(s))
    ON_CMD (mirror,    2, g_pos.Mirror())
    ON_CMD (mt,        2, OnMT(s))
    ON_CMD (new,       3, OnNew())
    ON_CMD (perft,     3, OnPerft(s))
    ON_CMD (ping,      4, OnPing(s))
    ON_CMD (position,  8, OnPosition(s))
    ON_CMD (protover,  8, OnProtover())
    ON_CMD (quit,      1, exit(0))
    ON_CMD (remove,    6, OnRemove())
    ON_CMD (sd,        2, OnSD(s))
    ON_CMD (setboard,  8, g_pos.SetFEN(s + 9))
    ON_CMD (setoption, 8, OnSetoption(s))
    ON_CMD (sn,        2, OnSN(s))
    ON_CMD (st,        2, OnST(s))
    ON_CMD (time,      2, OnTime(s))
    ON_CMD (xboard,    6, g_protocol = WINBOARD)
    ON_CMD (uci,       3, OnUCI())
    ON_CMD (undo,      1, g_pos.UnmakeMove())

    if (g_protocol == CONSOLE)
      out("Unknown command: %s\n", s);
  }
}
////////////////////////////////////////////////////////////////////////////////

