/* DO NOT EDIT THIS FILE - it is machine generated */
#line 27 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
#include "stdafx.h"
#include <string.h>
#include <hashmap.h>
#include "cpplex.h"
#include "cppsymbol.h"

static int nameOrKeyWord(TCHAR *lexeme);

#line 7 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
// DFA State   0 [nonAccepting]
//   goto  1 on \x00\x01\x02\x03\x04\x05\x06\x07\b\t
//   goto  2 on \n
//   goto  1 on \x0b
//   goto  3 on \f
//   goto  4 on \r
//   goto  3 on \x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f
//   goto  1 on \s
//   goto  5 on !
//   goto 42 on "
//   goto  3 on #$
//   goto  6 on %
//   goto  7 on &
//   goto 50 on \'
//   goto  8 on (
//   goto  9 on )
//   goto 10 on *
//   goto 11 on +
//   goto 12 on ,
//   goto 13 on -
//   goto 14 on .
//   goto 44 on /
//   goto 15 on 0
//   goto 46 on 123456789
//   goto 16 on :
//   goto 17 on ;
//   goto 18 on <
//   goto 19 on =
//   goto 47 on >
//   goto 20 on ?
//   goto  3 on @
//   goto 21 on ABCDEFGHIJKLMNOPQRSTUVWXYZ
//   goto 22 on [
//   goto  3 on \\
//   goto 23 on ]
//   goto 24 on ^
//   goto 21 on _
//   goto  3 on `
//   goto 21 on abcdefghijklmnopqrstuvwxyz
//   goto 25 on {
//   goto 26 on |
//   goto 27 on }
//   goto 43 on ~
//   goto  3 on \x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97
//              \x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0
//              \xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9
//              \xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf\xe0\xe1\xe2
//              \xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb
//              \xfc\xfd\xfe\xff
// DFA State   1 [accepting, line 102 <;>]
//   goto  1 on \x00\x01\x02\x03\x04\x05\x06\x07\b\t\x0b\s
// DFA State   2 [accepting, line 103 <;>]
// DFA State   3 [accepting, line 132 <error( getPos(), _T("Illegal character <%s>\n"), g>]
// DFA State   4 [accepting, line 104 <;>]
// DFA State   5 [accepting, line  73 <ascii = *getText(); return UNOP;>]
//   goto 28 on =
// DFA State   6 [accepting, line  76 <ascii = *getText(); return DIVOP;>]
//   goto 31 on =
// DFA State   7 [accepting, line  89 <return AND;>]
//   goto 32 on &
//   goto 31 on =
// DFA State   8 [accepting, line  63 <return LPAR;>]
// DFA State   9 [accepting, line  64 <return RPAR;>]
// DFA State  10 [accepting, line  75 <return STAR;>]
//   goto 31 on =
// DFA State  11 [accepting, line  78 <return PLUS;>]
//   goto 33 on +
//   goto 31 on =
// DFA State  12 [accepting, line  97 <return COMMA;>]
// DFA State  13 [accepting, line  79 <return MINUS;>]
//   goto 33 on -
//   goto 31 on =
//   goto 45 on >
// DFA State  14 [accepting, line  69 <ascii = *getText(); return STRUCTOP;>]
//   goto 59 on .
//   goto 34 on 0123456789
// DFA State  15 [accepting, line  59 <return ICON ;>]
//   goto 34 on .
//   goto 51 on 01234567
//   goto 48 on 89
//   goto 61 on E
//   goto 53 on F
//   goto 55 on LU
//   goto 61 on e
//   goto 53 on f
//   goto 55 on lu
//   goto 62 on x
// DFA State  16 [accepting, line  95 <return COLON;>]
//   goto 37 on :
// DFA State  17 [accepting, line  98 <return SEMI;>]
// DFA State  18 [accepting, line  82 <ascii = getText()[1] ? (getText()[0]=='>' ? 'G' : >]
//   goto 38 on <
//   goto 52 on =
// DFA State  19 [accepting, line  88 <return EQUAL;>]
//   goto 28 on =
// DFA State  20 [accepting, line  94 <return QUEST;>]
// DFA State  21 [accepting, line 100 <return nameOrKeyWord( getText() );>]
//   goto 21 on 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz
// DFA State  22 [accepting, line  67 <return LB;>]
// DFA State  23 [accepting, line  68 <return RB;>]
// DFA State  24 [accepting, line  90 <return XOR;>]
//   goto 31 on =
// DFA State  25 [accepting, line  65 <return LC;>]
// DFA State  26 [accepting, line  91 <return OR;>]
//   goto 31 on =
//   goto 39 on |
// DFA State  27 [accepting, line  66 <return RC;>]
// DFA State  28 [accepting, line  84 <ascii = *getText(); return EQUOP;>]
// DFA State  29 [accepting, line  50 <error(getPos(),_T("Adding missing \" to string con>]
// DFA State  30 [accepting, line  48 <return STRING; // (((\r\n)*{white}*)*{strlit})*>]
// DFA State  31 [accepting, line  86 <ascii = *getText(); return ASSIGNOP;>]
// DFA State  32 [accepting, line  92 <return ANDAND;>]
// DFA State  33 [accepting, line  71 <ascii = *getText(); return INCOP;>]
// DFA State  34 [accepting, line  61 <return FCON ;>]
//   goto 34 on 0123456789
//   goto 61 on E
//   goto 53 on F
//   goto 61 on e
//   goto 53 on f
// DFA State  35 [accepting, line 106 <{   int i;               SourcePosition p = getPos>]
// DFA State  36 [accepting, line 122 <{ int i;   while( i = input()) {  if(i < 0)    flu>]
// DFA State  37 [accepting, line  96 <return COLONCOLON;>]
// DFA State  38 [accepting, line  80 <ascii = *getText(); return SHIFTOP;>]
//   goto 31 on =
// DFA State  39 [accepting, line  93 <return OROR;>]
// DFA State  40 [accepting, line  99 <return ELLIPSIS;>]
// DFA State  41 [nonAccepting]
//   goto 41 on \x00\x01\x02\x03\x04\x05\x06\x07\b\t\x0b\f
//   goto 29 on \r
//   goto 41 on \x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f\s!
//   goto 30 on "
//   goto 41 on #$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[
//   goto 49 on \\
//   goto 41 on ]^_`abcdefghijklmnopqrstuvwxyz{|}~\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f
//              \x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8
//              \xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xc0\xc1
//              \xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda
//              \xdb\xdc\xdd\xde\xdf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3
//              \xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff
// DFA State  42 [accepting, line 132 <error( getPos(), _T("Illegal character <%s>\n"), g>]
//   goto 41 on \x00\x01\x02\x03\x04\x05\x06\x07\b\t\x0b\f
//   goto 29 on \r
//   goto 41 on \x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f\s!
//   goto 30 on "
//   goto 41 on #$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[
//   goto 49 on \\
//   goto 41 on ]^_`abcdefghijklmnopqrstuvwxyz{|}~\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f
//              \x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8
//              \xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xc0\xc1
//              \xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda
//              \xdb\xdc\xdd\xde\xdf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3
//              \xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff
// DFA State  43 [accepting, line  73 <ascii = *getText(); return UNOP;>]
// DFA State  44 [accepting, line  76 <ascii = *getText(); return DIVOP;>]
//   goto 35 on *
//   goto 36 on /
//   goto 31 on =
// DFA State  45 [accepting, line  69 <ascii = *getText(); return STRUCTOP;>]
// DFA State  46 [accepting, line  59 <return ICON ;>]
//   goto 34 on .
//   goto 46 on 0123456789
//   goto 61 on E
//   goto 53 on F
//   goto 55 on LU
//   goto 61 on e
//   goto 53 on f
//   goto 55 on lu
// DFA State  47 [accepting, line  82 <ascii = getText()[1] ? (getText()[0]=='>' ? 'G' : >]
//   goto 52 on =
//   goto 38 on >
// DFA State  48 [accepting, line  61 <return FCON ;>]
//   goto 34 on .
//   goto 48 on 0123456789
//   goto 61 on E
//   goto 53 on F
//   goto 61 on e
//   goto 53 on f
// DFA State  49 [nonAccepting]
//   goto 41 on \x00\x01\x02\x03\x04\x05\x06\x07\b\t\x0b\f\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c
//              \x1d\x1e\x1f\s!"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuv
//              wxyz{|}~\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95
//              \x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae
//              \xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7
//              \xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf\xe0
//              \xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9
//              \xfa\xfb\xfc\xfd\xfe\xff
// DFA State  50 [accepting, line 132 <error( getPos(), _T("Illegal character <%s>\n"), g>]
//   goto 54 on \x00\x01\x02\x03\x04\x05\x06\x07\b\t\x0b\f\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c
//              \x1d\x1e\x1f\s!"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[
//   goto 57 on \\
//   goto 54 on ]^_`abcdefghijklmnopqrstuvwxyz{|}~\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f
//              \x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8
//              \xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xc0\xc1
//              \xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda
//              \xdb\xdc\xdd\xde\xdf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3
//              \xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff
// DFA State  51 [accepting, line  59 <return ICON ;>]
//   goto 34 on .
//   goto 51 on 01234567
//   goto 48 on 89
//   goto 61 on E
//   goto 53 on F
//   goto 55 on LU
//   goto 61 on e
//   goto 53 on f
//   goto 55 on lu
// DFA State  52 [accepting, line  82 <ascii = getText()[1] ? (getText()[0]=='>' ? 'G' : >]
// DFA State  53 [accepting, line  61 <return FCON ;>]
// DFA State  54 [nonAccepting]
//   goto 55 on \'
// DFA State  55 [accepting, line  59 <return ICON ;>]
// DFA State  56 [accepting, line  61 <return FCON ;>]
//   goto 56 on 0123456789
//   goto 53 on Ff
// DFA State  57 [nonAccepting]
//   goto 54 on \x00\x01\x02\x03\x04\x05\x06\x07\b\t\x0b\f\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c
//              \x1d\x1e\x1f\s!"#$%&
//   goto 58 on \'
//   goto 54 on ()*+,-./
//   goto 63 on 01234567
//   goto 54 on 89:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvw
//   goto 64 on x
//   goto 54 on yz{|}~\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96
//              \x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf
//              \xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8
//              \xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf\xe0\xe1
//              \xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa
//              \xfb\xfc\xfd\xfe\xff
// DFA State  58 [accepting, line  59 <return ICON ;>]
//   goto 55 on \'
// DFA State  59 [nonAccepting]
//   goto 40 on .
// DFA State  60 [accepting, line  59 <return ICON ;>]
//   goto 60 on 0123456789ABCDEF
//   goto 55 on LU
//   goto 60 on abcdef
//   goto 55 on lu
// DFA State  61 [nonAccepting]
//   goto 65 on +-
//   goto 56 on 0123456789
// DFA State  62 [nonAccepting]
//   goto 60 on 0123456789ABCDEFabcdef
// DFA State  63 [nonAccepting]
//   goto 55 on \'
//   goto 66 on 01234567
// DFA State  64 [nonAccepting]
//   goto 55 on \'
//   goto 68 on 0123456789ABCDEFabcdef
// DFA State  65 [nonAccepting]
//   goto 56 on 0123456789
// DFA State  66 [nonAccepting]
//   goto 55 on \'
//   goto 54 on 01234567
// DFA State  67 [nonAccepting]
//   goto 55 on \'
//   goto 54 on 0123456789ABCDEFabcdef
// DFA State  68 [nonAccepting]
//   goto 55 on \'
//   goto 67 on 0123456789ABCDEFabcdef

// The lexCharMap[] and lexStateMap arrays are used as follows:
//
// nextState = lexNext[lexStateMap[currentState]][lexCharMap[inputChar]];
//
// Character positions in the lexCharMap Array are:
//
//\x00 \x01 \x02 \x03 \x04 \x05 \x06 \x07 \b   \t   \n   \x0b \f   \r   \x0e \x0f
//\x10 \x11 \x12 \x13 \x14 \x15 \x16 \x17 \x18 \x19 \x1a \x1b \x1c \x1d \x1e \x1f
//\s   !    "    #    $    %    &    \'   (    )    *    +    ,    -    .    /
//0    1    2    3    4    5    6    7    8    9    :    ;    <    =    >    ?
//@    A    B    C    D    E    F    G    H    I    J    K    L    M    N    O
//P    Q    R    S    T    U    V    W    X    Y    Z    [    \\   ]    ^    _
//`    a    b    c    d    e    f    g    h    i    j    k    l    m    n    o
//p    q    r    s    t    u    v    w    x    y    z    {    |    }    ~    \x7f
//\x80 \x81 \x82 \x83 \x84 \x85 \x86 \x87 \x88 \x89 \x8a \x8b \x8c \x8d \x8e \x8f
//\x90 \x91 \x92 \x93 \x94 \x95 \x96 \x97 \x98 \x99 \x9a \x9b \x9c \x9d \x9e \x9f
//\xa0 \xa1 \xa2 \xa3 \xa4 \xa5 \xa6 \xa7 \xa8 \xa9 \xaa \xab \xac \xad \xae \xaf
//\xb0 \xb1 \xb2 \xb3 \xb4 \xb5 \xb6 \xb7 \xb8 \xb9 \xba \xbb \xbc \xbd \xbe \xbf
//\xc0 \xc1 \xc2 \xc3 \xc4 \xc5 \xc6 \xc7 \xc8 \xc9 \xca \xcb \xcc \xcd \xce \xcf
//\xd0 \xd1 \xd2 \xd3 \xd4 \xd5 \xd6 \xd7 \xd8 \xd9 \xda \xdb \xdc \xdd \xde \xdf
//\xe0 \xe1 \xe2 \xe3 \xe4 \xe5 \xe6 \xe7 \xe8 \xe9 \xea \xeb \xec \xed \xee \xef
//\xf0 \xf1 \xf2 \xf3 \xf4 \xf5 \xf6 \xf7 \xf8 \xf9 \xfa \xfb \xfc \xfd \xfe \xff

static const unsigned char lexCharMap[256] = {
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   0,   2,   3,   2,   2,
     2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
     0,   4,   5,   2,   2,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,  16,
    17,  18,  18,  18,  18,  18,  18,  18,  19,  19,  20,  21,  22,  23,  24,  25,
     2,  26,  26,  26,  26,  27,  28,  29,  29,  29,  29,  29,  30,  29,  29,  29,
    29,  29,  29,  29,  29,  30,  29,  29,  29,  29,  29,  31,  32,  33,  34,  29,
     2,  26,  26,  26,  26,  27,  28,  29,  29,  29,  29,  29,  30,  29,  29,  29,
    29,  29,  29,  29,  29,  30,  29,  29,  35,  29,  29,  36,  37,  38,  39,   2,
     2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
     2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
     2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
     2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
     2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
     2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
     2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
     2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2
};

static const unsigned char lexStateMap[69] = {
  /*   0 */ 0, 1, 2, 2, 2, 3, 4, 5, 2, 2, 4, 6, 2, 7, 8, 9,10, 2,11, 3,
  /*  20 */ 2,12, 2, 2, 4, 2,13, 2, 2, 2, 2, 2, 2, 2,14, 2, 2, 2, 4, 2,
  /*  40 */ 2,15,15, 2,16, 2,17,18,19,20,21,22, 2, 2,23, 2,24,25,23,26,
  /*  60 */27,28,29,30,31,32,33,34,35
};

static const char lexNext[36][40] = {
  /*  0 */ { 1, 2, 3, 4, 5,42, 6, 7,50, 8, 9,10,11,12,13,14,44,15,46,46,16,17,18,19,47,20,21,21,21,21,21,22, 3,23,24,21,25,26,27,43},
  /*  1 */ { 1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
  /*  2 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
  /*  3 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,28,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
  /*  4 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,31,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
  /*  5 */ {-1,-1,-1,-1,-1,-1,-1,32,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,31,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
  /*  6 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,33,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,31,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
  /*  7 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,33,-1,-1,-1,-1,-1,-1,-1,-1,31,45,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
  /*  8 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,59,-1,34,34,34,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
  /*  9 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,34,-1,51,51,48,-1,-1,-1,-1,-1,-1,-1,61,53,-1,55,-1,-1,-1,-1,62,-1,-1,-1,-1},
  /* 10 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,37,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
  /* 11 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,38,52,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
  /* 12 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,21,21,21,-1,-1,-1,-1,-1,-1,21,21,21,21,21,-1,-1,-1,-1,21,-1,-1,-1,-1},
  /* 13 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,31,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,39,-1,-1},
  /* 14 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,34,34,34,-1,-1,-1,-1,-1,-1,-1,61,53,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
  /* 15 */ {41,-1,41,29,41,30,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,49,41,41,41,41,41,41,41},
  /* 16 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,35,-1,-1,-1,-1,36,-1,-1,-1,-1,-1,-1,31,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
  /* 17 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,34,-1,46,46,46,-1,-1,-1,-1,-1,-1,-1,61,53,-1,55,-1,-1,-1,-1,-1,-1,-1,-1,-1},
  /* 18 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,52,38,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
  /* 19 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,34,-1,48,48,48,-1,-1,-1,-1,-1,-1,-1,61,53,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
  /* 20 */ {41,-1,41,-1,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41},
  /* 21 */ {54,-1,54,-1,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,57,54,54,54,54,54,54,54},
  /* 22 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,34,-1,51,51,48,-1,-1,-1,-1,-1,-1,-1,61,53,-1,55,-1,-1,-1,-1,-1,-1,-1,-1,-1},
  /* 23 */ {-1,-1,-1,-1,-1,-1,-1,-1,55,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
  /* 24 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,56,56,56,-1,-1,-1,-1,-1,-1,-1,-1,53,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
  /* 25 */ {54,-1,54,-1,54,54,54,54,58,54,54,54,54,54,54,54,54,63,63,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,54,64,54,54,54,54},
  /* 26 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,40,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
  /* 27 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,60,60,60,-1,-1,-1,-1,-1,-1,60,60,60,-1,55,-1,-1,-1,-1,-1,-1,-1,-1,-1},
  /* 28 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,65,-1,65,-1,-1,56,56,56,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
  /* 29 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,60,60,60,-1,-1,-1,-1,-1,-1,60,60,60,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
  /* 30 */ {-1,-1,-1,-1,-1,-1,-1,-1,55,-1,-1,-1,-1,-1,-1,-1,-1,66,66,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
  /* 31 */ {-1,-1,-1,-1,-1,-1,-1,-1,55,-1,-1,-1,-1,-1,-1,-1,-1,68,68,68,-1,-1,-1,-1,-1,-1,68,68,68,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
  /* 32 */ {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,56,56,56,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
  /* 33 */ {-1,-1,-1,-1,-1,-1,-1,-1,55,-1,-1,-1,-1,-1,-1,-1,-1,54,54,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
  /* 34 */ {-1,-1,-1,-1,-1,-1,-1,-1,55,-1,-1,-1,-1,-1,-1,-1,-1,54,54,54,-1,-1,-1,-1,-1,-1,54,54,54,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
  /* 35 */ {-1,-1,-1,-1,-1,-1,-1,-1,55,-1,-1,-1,-1,-1,-1,-1,-1,67,67,67,-1,-1,-1,-1,-1,-1,67,67,67,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}
};

static const char lexAccept[69] = {
  /*   0 */ 0,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4
  /*  20 */,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4
  /*  40 */,4,0,4,4,4,4,4,4,4,0,4,4,4,4,0,4,4,0,4,0
  /*  60 */,4,0,0,0,0,0,0,0,0
};

#line 9 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"

// nextState(state,c) is given the current state number and input
// character and evaluates to the next state.
#define nextState(state,c) (lexNext[lexStateMap[state]][lexCharMap[c]])

int CppLex::getNextLexeme() {
  int  state           = 0;     // Current state
  int  lastAcceptState = 0;     // Most recently seen accept state
  int  prevState;               // State before lastAcceptState
  int  nextState;               // Next state
  unsigned int  lookahead;      // Lookahead character
  int  anchor;                  // Anchor mode for most recently seen accepting state

  unTerminateLexeme();
  initMore();
  markStart();

  for(;;) {
    // Check end of file. If there's an unprocessed accepting state,
    // lastAcceptState will be nonzero. In this case, ignore EOF for now so
    // that you can do the accepting action; otherwise, try to open another
    // file and return if you can't.

    for(;;) {
      if(((int)(lookahead = look(1))) != EOF) {
        assert(lookahead < 256);
        nextState = nextState(state, lookahead);
        break;
      } else if(lastAcceptState != 0) {   // still something to do
        nextState = -1;
        break;
      } else if(isWrap()) {               // another file?
        terminateLexeme();
        return 0;                         // EOI
      }
    }
    if(m_debug) {
      debugState(_T("--------"), state, lookahead);
    }

    if(nextState != -1) {
      if(advance() < 0) {                 // Buffer full
        const TCHAR *tooLongMessage = _T("Lexeme too long. Discarding extra characters.");
        error(getPos(), tooLongMessage);

        flush(true);

        if(m_debug) {
          debug(tooLongMessage);
          debugState(_T("--------"), state, look(1));
        }
      }

      if(anchor = lexAccept[nextState]) { // Is this an accept state
        prevState       = state;
        lastAcceptState = nextState;
        markEnd();                        // Mark input at current character
      }                                   // A subsequent gotoMark() returns us to this position.
      state = nextState;
    } else if(lastAcceptState == 0) {     // illegal input
      error(getPos(), _istprint(lookahead)?_T("Ignore bad input:'%c'"):_T("Ignore bad input:%#x"),lookahead);

      if(m_debug) {
        debug(_T("Ignore bad input:'%c'"), lookahead);
      }

      advance();
    } else {
      if(m_debug) {
        debugState(_T("accept--"), lastAcceptState, lookahead);
      }

      gotoMark();                         // Back up to previous accept state
      if(anchor & ANCHOR_END) {           // If end anchor is active
        pushback(1);                      // Push back the CR or LF
      }
      if(anchor & ANCHOR_START) {         // If start anchor is active
        moveStart();                      // Skip the leading newline
      }
      terminateLexeme();                  // Null-terminate the string

      switch(lastAcceptState) {
      case 1:
#line 102 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
        ;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 2:
#line 103 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
        ;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 3:
      case 42:
      case 50:
#line 132 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
        error( getPos(), _T("Illegal character <%s>\n"), getText() );
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 4:
#line 104 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
        ;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 5:
      case 43:
#line 73 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
           ascii = *getText();
        return UNOP;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 6:
      case 44:
#line 76 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
           ascii = *getText();
        return DIVOP;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 7:
#line 89 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
        return AND;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 8:
#line 63 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
        return LPAR;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 9:
#line 64 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
        return RPAR;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 10:
#line 75 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
        return STAR;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 11:
#line 78 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
        return PLUS;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 12:
#line 97 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
        return COMMA;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 13:
#line 79 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
        return MINUS;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 14:
      case 45:
#line 69 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
              ascii = *getText();
        return STRUCTOP;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 15:
      case 46:
      case 51:
      case 55:
      case 58:
      case 60:
#line 59 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
        return ICON ;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 16:
#line 95 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
        return COLON;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 17:
#line 98 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
        return SEMI;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 18:
      case 47:
      case 52:
#line 82 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
             ascii = getText()[1] ? (getText()[0]=='>' ? 'G' : 'L') : (getText()[0] );
        return RELOP;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 19:
#line 88 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
        return EQUAL;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 20:
#line 94 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
        return QUEST;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 21:
#line 100 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
        return nameOrKeyWord( getText() );
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 22:
#line 67 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
        return LB;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 23:
#line 68 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
        return RB;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 24:
#line 90 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
        return XOR;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 25:
#line 65 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
        return LC;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 26:
#line 91 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
        return OR;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 27:
#line 66 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
        return RC;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 28:
#line 84 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
            ascii = *getText();
        return EQUOP;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 29:
#line 50 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
                      error(getPos(),_T("Adding missing \" to string constant\n"));
        return STRING;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 30:
#line 48 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
        return STRING; // (((\r\n)*{white}*)*{strlit})*
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 31:
#line 86 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
                        ascii = *getText();
        return ASSIGNOP;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 32:
#line 92 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
        return ANDAND;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 33:
#line 71 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
               ascii = *getText();
        return INCOP;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 34:
      case 48:
      case 53:
      case 56:
#line 61 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
        return FCON ;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 35:
#line 106 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
           {
          int i;
                      SourcePosition p = getPos();
          while( i = input() ) {
        	if( i < 0 )
        	  flushBuf();   	/* Discard lexeme.		*/
        	else if( i == '*' && look(1) == '/' ) {
                          input();
        	  break;          	/* Recognized comment.*/
        	}
          }
          if( i == 0 )
        	error(p,_T("End of file in comment\n"));
        }
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 36:
#line 122 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
              { int i;
          while( i = input()) {
        	if(i < 0)
        	  flushBuf();
        	else
        	  if(i == '\r')
        	break;
          }
        }
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 37:
#line 96 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
        return COLONCOLON;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 38:
#line 80 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
            ascii = *getText();
        return SHIFTOP;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 39:
#line 93 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
        return OROR;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
      case 40:
#line 99 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
        return ELLIPSIS;
#line 91 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
        break;
#line 93 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
      default:
        throwException(_T("%s:Unknown accept state:%d, text=<%s>"), __TFUNCTION__, lastAcceptState,getText());
        break;
      }

      unTerminateLexeme();
      lastAcceptState = 0;

      if(isMore()) {
        state = prevState;                // Back up
        initMore();
      } else {
        state = 0;
        markStart();
      }
    }
  }
}

#line 135 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"

  /* This part goes to the last part of cpplex.cpp */

typedef struct {
  TCHAR *m_name;
  int    m_token;
} keyword;

static keyword keywordtable[] = {
 _T("new")       ,NEWOP
,_T("delete")    ,DELETEOP
,_T("sizeof")    ,SIZEOF
,_T("class")	 ,STRUCT
,_T("struct")    ,STRUCT
,_T("union")     ,STRUCT
,_T("enum")      ,ENUM
,_T("auto")      ,CLASS
,_T("extern")    ,CLASS
,_T("register")  ,CLASS
,_T("static")    ,CLASS
,_T("const")     ,CLASS
,_T("typedef")   ,CLASS
,_T("friend")    ,CLASS
,_T("inline")    ,CLASS
,_T("overload")  ,CLASS
,_T("operator")  ,OPERATOR
,_T("virtual")   ,CLASS
,_T("char")      ,TYPE
,_T("short")     ,TYPE
,_T("int")       ,TYPE
,_T("long")      ,TYPE
,_T("unsigned")  ,TYPE
,_T("signed")    ,SIGNED
,_T("float")     ,TYPE
,_T("double")    ,TYPE
,_T("void")      ,TYPE
,_T("private")   ,PRIVATE
,_T("public")    ,PUBLIC
,_T("protected") ,PROTECTED
,_T("while")     ,WHILE
,_T("if")        ,IF
,_T("else")      ,ELSE
,_T("switch")    ,SWITCH
,_T("case")      ,CASE
,_T("default")   ,DEFAULT
,_T("break")     ,BREAK
,_T("continue")  ,CONTINUE
,_T("return")    ,RETURN
,_T("goto")      ,GOTO
,_T("for")       ,FOR
,_T("throw")     ,THROW
,_T("try")       ,TRYSTMT
,_T("catch")     ,CATCH
,_T("template")  ,TEMPLATE
,_T("define")    ,DEFINE
,_T("endif")     ,ENDIF
,_T("ifdef")     ,IFDEF
,_T("ifndef")    ,IFNDEF
,_T("include")   ,INCLUDE
,_T("undef")     ,UNDEF
};

typedef StrHashMap<int> HashMapType;

class CppKeyWordMap : public HashMapType {
public:
  CppKeyWordMap() : HashMapType(23) {
    for(int i = 0; i < ARRAYSIZE(keywordtable); i++)
      put(keywordtable[i].m_name,keywordtable[i].m_token);
  }
};

static CppKeyWordMap keywords;

static int nameOrKeyWord(TCHAR *lexeme) {
  int *p = keywords.get((TCHAR*)lexeme);
  return p ? *p : NAME;
}

void CppLex::verror(const SourcePosition &pos, const TCHAR *format, va_list argptr) {
  if(m_parser==nullptr) {
    Scanner::verror(pos, format, argptr);
  } else {
    m_parser->verror(pos, format, argptr);
  }
}
#line 112 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
