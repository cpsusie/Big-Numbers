#include "stdafx.h"
#include "lex.h"

CollectScanner::CollectScanner(const String &fname, int size) : Scanner(fname) {
  collect_buffer_size = size;
  collect_buffer      = new TCHAR[size];
  collect_count       = 0;
  collect_head        = 0;
  collect_tail        = 0;
  collect_lastsym     = 0;
}

CollectScanner::~CollectScanner() {
  delete[] collect_buffer;
}

void CollectScanner::collectChar(TCHAR ch) {
  if( collect_count >= collect_buffer_size) {
    warning( SQL_BUFFER_OVERFLOW, _T("Internal error, Collect_buffer flushed"));
    flushCollected();
  }
  collect_count++;
  collect_buffer[collect_tail] = ch;
  incmod(collect_tail);
}

int CollectScanner::getUntilLastSym(TCHAR *dst, bool inc_space) {
  int i,j;
  int n = inc_space ? collect_lastsym_inc_space : collect_lastsym;
  for(i=0,j=collect_head; j != n;i++,incmod(j))
    dst[i] = collect_buffer[j];
  dst[i] = 0;
  return collect_count;
}

void CollectScanner::flushUntilLastSym(bool inc_space) {
  int n = inc_space ? collect_lastsym_inc_space : collect_lastsym;
  while(collect_head != n) {
    fputc(collect_buffer[collect_head],&m_listfile);
    incmod(collect_head);
    collect_count--;
  }
}

void CollectScanner::flushLastSym() {
  while(collect_lastsym != collect_tail) {
    fputc(collect_buffer[collect_lastsym],&m_listfile);
    incmod(collect_lastsym);
  }
}

void CollectScanner::skipUntilLastSym(bool inc_space) {
  int n = inc_space ? collect_lastsym_inc_space : collect_lastsym;
  while(collect_head != n) {
    incmod(collect_head);
    collect_count--;
  }
}

void CollectScanner::skipCollected() {
  collect_head = collect_lastsym = collect_lastsym_inc_space = collect_tail;
  collect_count = 0;
}

void CollectScanner::flushCollected() {
  flushUntilLastSym(false);
  flushLastSym();
  skipCollected();
}

void CollectScanner::advance() {
  if(currentChar()) collectChar(currentChar());
  Scanner::advance();
}

void CollectScanner::nextToken() {
  do {
    collect_lastsym_inc_space = collect_tail;
    skipSpace();
    collect_lastsym = collect_tail;
    Scanner::nextToken();
  } while(token == SYM_COMMENT);
}
