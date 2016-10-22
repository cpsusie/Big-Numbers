//  GREKO Chess Engine
//  (c) 2002-2011 Vladimir Medvedev <vrm@bk.ru>
//  http://greko.110mb.com

//  utils.h: some utilities
//  modified: 1-Aug-2011

#ifndef UTILS_H
#define UTILS_H

#include "types.h"

extern FILE* g_log;

void  InitInput();
int   InputAvailable();
void  out(const char* s);
char* ReadInput(char* buf, int sz);
void  RandSeed32(U32 seed);
U32   Rand32();
U64   Rand64();
void  Highlight(int on);
void  SleepMilliseconds(int ms);

inline void out(const char* s)
{
  printf(s);
  if (g_log)
  {
    fprintf(g_log, s);
    fflush(g_log);
  }
}
////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline void out(const char* format, T arg)
{
  printf(format, arg);
  if (g_log)
  {
    fprintf(g_log, format, arg);
    fflush(g_log);
  }
}
////////////////////////////////////////////////////////////////////////////////

template <typename T1, typename T2>
inline void out(const char* format, T1 arg1, T2 arg2)
{
  printf(format, arg1, arg2);
  if (g_log)
  {
    fprintf(g_log, format, arg1, arg2);
    fflush(g_log);
  }
}
////////////////////////////////////////////////////////////////////////////////

#endif

