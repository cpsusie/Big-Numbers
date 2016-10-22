//  GREKO Chess Engine
//  (c) 2002-2011 Vladimir Medvedev <vrm@bk.ru>
//  http://greko.110mb.com

//  book.h: opening book
//  modified: 1-Aug-2011

#ifndef BOOK_H
#define BOOK_H

#include <map>
#include <vector>
using namespace std;

#include "moves.h"

class Book
{
public:

  Book() {}
  ~Book() {}

  void Clean() 
  { 
    m_data.clear();
    m_pos.SetInitial();
    ++m_data[m_pos.Hash()];
  }

  Move GetMove(const Position& pos, char* buf);
  bool Import(char* s);
  void Init();
  bool Load(const char* path);
  bool Save(const char* path);

private:

  void ProcessLine(const char* str);

  map<U64, int> m_data;
  Position m_pos;
};
extern Book g_book;

#endif

