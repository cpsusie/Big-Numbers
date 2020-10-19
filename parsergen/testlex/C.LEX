%{
#pragma once

 /* This part goes to clex.h */

#include <Scanner.h>
#include "csymbol.h"

class CLex : public Scanner {
public:
  int getNextLexeme() override;
};

extern void gotstructop();
%}

%{

 /* This part goes to the first part of clex.cpp */

#include "stdafx.h"
#include "clex.h"
#include <ctype.h>

//extern bool nextfile(Scanner &);
//#define yywrap() nextfile(*this)

%}

let     [_a-zA-Z]			/* Letter 								*/
alnum   [_a-zA-Z0-9]		/* Alphanumeric character				*/
h       [0-9a-fA-F]			/* Hexadecimal digit					*/
o       [0-7]				/* Octal digit							*/
d       [0-9]				/* Decimal digit						*/
suffix  [UuLl]				/* Suffix in integral numeric constant	*/
white   [\x00-\x09\x0b\s]	/* White space: all control chars but \n*/
strlit	\"(\\.|[^\\\"])*\"
%%

{let}{alnum}*           return NAME;

"/*"            {
                  int i;

                  while( i = input() ) {
                    if( i < 0 )
                      flushBuf();        // Discard lexeme.
                    else if( i == '*' && look(1) == '/' ) {
                      input();
                      break;             // Recognized comment
                    }
                  }

                  if( i == 0 )
                    printf("End of file in comment\n" );
                  else
                    printf("/* .. */ comment\n");
                }

"//"            { int i;
                  while( i = input()) {
                    if(i < 0)
                      flushBuf();
                    else
                      if(i == '\r')
                    break;
                  }
                  printf("// comment\n");
                }

{strlit}        return STRING; // (((\r\n)*{white}*)*{strlit})*

\"(\\.|[^\\\"])*\r error(getPos(),_T("Adding missing \" to string constant\n"));
                        return STRING;

'.'                     |
'\\.'                   |
'\\{o}({o}{o}?)?'       |
'\\x{h}({h}{h}?)?'      |
0{o}*{suffix}?          |
0x{h}+{suffix}?         |
[1-9]{d}*{suffix}?      return ICON ;

({d}+|{d}+\.{d}*|{d}*\.{d}+)([eE][\-+]?{d}+)?[fF]?  return FCON ;

"("                     return LP;
")"                     return RP;
"{"                     return LC;
"}"                     return RC;
"["                     return LB;
"]"                     return RB;
"->"                    |
"."                     gotstructop(); return STRUCTOP;
"++"                    |
"--"                    return INCOP;
[~!]                    return UNOP;
"*"                     return STAR;
[/%]                    return DIVOP;
"+"                     return PLUS;
"-"                     return MINUS;
<<|>>                   return SHIFTOP;
[<>]=?                  return RELOP;

[!=]=                   return EQUOP;

[*/%+\-&|^]=            |
(<<|>>)=                return ASSIGNOP;

"="                     return EQUAL;
"&"                     return AND;
"^"                     return XOR;
"|"                     return OR;
"&&"                    return ANDAND;
"||"                    return OROR;
"?"                     return QUEST;
":"                     return COLON;
","                     return COMMA;
";"                     return SEMI;
"..."                   return ELLIPSIS;

^{white}*#              return HASHMARK;
#define                 return HASHMARK;

\n          ;
\r          ;
{white}+                ;
.                       _tprintf( _T("Illegal character <%c>\n"), *getText());
%%

  /* This part goes to the last part of clex.cpp */
void gotstructop() {
  printf("Nu er vi i structop\n");
}
