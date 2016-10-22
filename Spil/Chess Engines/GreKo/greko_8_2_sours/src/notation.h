//  GREKO Chess Engine
//  (c) 2002-2011 Vladimir Medvedev <vrm@bk.ru>
//  http://greko.110mb.com

//  notation.h: full and short algebraic notation
//  modified: 1-Aug-2011

#ifndef NOTATION_H
#define NOTATION_H

#include "position.h"

char* FldToStr(FLD f, char* str);
char* MoveToStrLong(Move mv, char* str);
char* MoveToStrShort(Move mv, Position& pos, char* str);
FLD   StrToFld(const char* str);
Move  StrToMove(const char* str, Position& pos);

#endif

