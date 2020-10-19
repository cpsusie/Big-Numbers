%{
#pragma once

 /* This part goes to the h file */

typedef enum {
  _EOI
 ,OLDSTRING
 ,OLDCHAR
 ,NEWSTRING
 ,NEWCHAR
 ,DIRECTIVE
 ,OTHER
} InputToken;

class FindStringsLex : public Scanner {
public:
  FindStringsLex(LexStream *inputStream = NULL) : Scanner(inputStream) {
  }
  int getNextLexeme() override;
};

%}

%{

 /* This part goes to the first part of the cpp file */

#include "stdafx.h"
#include "FindStringsLex.h"

%}

let     [_a-zA-Z]              /* Letter                                */
alnum   [_a-zA-Z0-9]           /* Alphanumeric character                */
d       [0-9]                  /* Decimal digit                         */
white   [\x00-\x09\x0b\s\r\n]  /* White space: all control chars        */
strlit  \"(\\.|[^\\\"])*\"
%%

{strlit}({white}*{strlit})*                 return OLDSTRING;

'([^'\\]|\\.)'                              return OLDCHAR;

L{strlit}({white}*{strlit})* |              
_T\({white}*({white}*{strlit})+{white}*\)   return NEWSTRING;

_T\({white}*'([^'\\]|\\.)'{white}*\)        return NEWCHAR;

#include                    return DIRECTIVE;
#line{white}+{d}+           return DIRECTIVE;
extern                      return DIRECTIVE;

"/*"           { int i;
                 const SourcePosition startpos = getPos();
                 while(i = input()) {
                   if(i < 0) {
                     flushBuf();  /* Discard lexeme.    */
                   } else if(i == '*' && look(1) == '/') {
                    input();
                    break;        /* Recognized comment.*/
                   }
                 }
                 if(i == 0) {
                   error( startpos,_T("End of file in comment\n") );
                 }
               }

"//"           { int i;
                 while(i = input()) {
                   if(i < 0) {
                     flushBuf();  /* Discard lexeme. */
                   } else if(i == '\n') {
                     break;
                   }
                 }
               }

{white}+	;
.         return OTHER;
%%
