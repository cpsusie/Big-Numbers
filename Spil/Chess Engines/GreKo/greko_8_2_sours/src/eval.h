//  GREKO Chess Engine
//  (c) 2002-2011 Vladimir Medvedev <vrm@bk.ru>
//  http://greko.110mb.com

//  eval.h: static position evaluation
//  modified: 1-Aug-2011

#ifndef EVAL_H
#define EVAL_H

#include "position.h"

const EVAL INFINITY_SCORE  = 50000;
const EVAL CHECKMATE_SCORE = 32768;
const EVAL DRAW_SCORE      = 0;

extern EVAL VAL_P;
extern EVAL VAL_N;
extern EVAL VAL_B;
extern EVAL VAL_R;
extern EVAL VAL_Q;

extern EVAL VALUE[14];

void InitEval();
EVAL Evaluate(const Position& pos, EVAL alpha, EVAL beta);

#endif

