//  GREKO Chess Engine
//  (c) 2002-2011 Vladimir Medvedev <vrm@bk.ru>
//  http://greko.110mb.com

//  commands.h: command line interface, uci and winboard protocols
//  modified: 1-Aug-2011

#ifndef COMMANDS_H
#define COMMANDS_H

int  Is(const char *s, const char *pattern, size_t min_length);
void RunCommandLine();

#endif

