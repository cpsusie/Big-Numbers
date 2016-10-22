//  GREKO Chess Engine
//  (c) 2002-2011 Vladimir Medvedev <vrm@bk.ru>
//  http://greko.110mb.com

//  main.cpp: initialize and start engine
//  modified: 1-Aug-2011

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif

#include "book.h"
#include "commands.h"
#include "config.h"
#include "eval.h"
#include "moves.h"
#include "search.h"
#include "utils.h"
#include "version.h"

FILE *g_log = NULL;
Position g_pos;

extern Search g_search;

int main(int argc, char *argv[])
{
  g_config.Read();

  if (g_config.GetInt("WriteLog", 0))
    g_log = fopen("greko.log", "at");

  g_search.LimitStrength(g_config.GetInt("UCI_LimitStrength", 0) != 0);
  g_search.SetElo(g_config.GetInt("UCI_Elo", 2500));

  int knps = g_config.GetInt("LimitKNPS", 9999);
  if (knps != 9999)
  {
    // overrides UCI settings
    g_search.LimitStrength(true);
    g_search.SetNPS(1000 * knps);
  }

  Highlight(1);
  out("\n%s (%s)\n\n", VERSION, RELEASE_DATE);
  Highlight(0);

  InitInput();
  InitBitboards();
  Position::InitHashNumbers();

  g_search.SetHashMB(g_config.GetDouble("HashMB", 32.0));

  g_book.Init();
  RandSeed32(U32(time(0)));

  g_pos.SetInitial();
  InitEval();

  RunCommandLine();

  if (g_log)
    fclose(g_log);

  return 0;
}
////////////////////////////////////////////////////////////////////////////////

