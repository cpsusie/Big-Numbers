%{
#pragma once
 /* This part goes to cpplex.h */

#include <Scanner.h>
#include <lrparser.h>


class CppLex : public Scanner {
private:
  LRparser *m_parser;
public:
  CppLex() { m_parser = NULL; }
  void setParser(LRparser *parser) { m_parser = parser; }
  int  getNextLexeme();
  void verror(const SourcePosition &pos, const char *format, va_list argptr);
  int m_token;
  int ascii;
};

%}

%{

 /* This part goes to the first part of cpplex.cpp */

#include "stdafx.h"
#include <string.h>
#include <hashmap.h>
#include "cpplex.h"
#include "cppsymbol.h"

static int nameOrKeyWord(unsigned char *lexeme);

%}

let     [_a-zA-Z]           /* Letter                               */
alnum   [_a-zA-Z0-9]        /* Alphanumeric character               */
d       [0-9]		        /* Decimal digit				        */
h       [0-9a-fA-F]			/* Hexadecimal digit					*/
o       [0-7]				/* Octal digit							*/
suffix  [UuLl]				/* Suffix in integral numeric constant	*/
white   [\x00-\x09\x0b\s]      /* White space: all control chars but \n */
strlit	\"(\\.|[^\\\"])*\"

%%

{strlit}	return STRING; // (((\r\n)*{white}*)*{strlit})*

\"(\\.|[^\\\"])*\r		error(getPos(),"Adding missing \" to string constant\n");
						return STRING;

'.'						|
'\\.'					|
'\\{o}({o}{o}?)?'		|
'\\x{h}({h}{h}?)?'		|
0{o}*{suffix}?			|
0x{h}+{suffix}?			|
[1-9]{d}*{suffix}?		return ICON ;

({d}+|{d}+\.{d}*|{d}*\.{d}+)([eE][\-+]?{d}+)?[fF]?	return FCON ;

"("						return LP;
")"						return RP;
"{"						return LC;
"}"						return RC;
"["						return LB;
"]"						return RB;
"->"|"."				ascii = *getText();
						return STRUCTOP;
"++"|"--"				ascii = *getText();
						return INCOP;
[~!]					ascii = *getText();
						return UNOP;
"*"						return STAR;
[/%]					ascii = *getText();
						return DIVOP;
"+"						return PLUS;
"-"						return MINUS;
<<|>>					ascii = *getText();
						return SHIFTOP;
[<>]=?					ascii = getText()[1] ? (getText()[0]=='>' ? 'G' : 'L') : (getText()[0] );
						return RELOP;
[!=]=					ascii = *getText();
						return EQUOP;
[*/%+\-&|^]=|(<<|>>)=	ascii = *getText();
						return ASSIGNOP;
"="						return EQUAL;
"&"						return AND;
"^"						return XOR;
"|"						return OR;
"&&"					return ANDAND;
"||"					return OROR;
"?"						return QUEST;
":"						return COLON;
"::"                    return COLONCOLON;
","						return COMMA;
";"						return SEMI;
"..."					return ELLIPSIS;
{let}{alnum}*			return nameOrKeyWord( getText() );

{white}+	;
\n		    ;
\r		    ;

"/*"			{
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
					error(p,"End of file in comment\n" );
				}

"//"		    { int i;
				  while( i = input()) {
					if(i < 0)
					  flushBuf();
					else
					  if(i == '\r')
					break;
				  }
				}

.				error( getPos(), "Illegal character <%s>\n", getText() );

%%

  /* This part goes to the last part of cpplex.cpp */

typedef struct {
  char *m_name;
  int   m_token;
} keyword;

static keyword keywordtable[] = {
 "new"       ,NEWOP
,"delete"    ,DELETEOP
,"sizeof"    ,SIZEOF
,"class"	 ,STRUCT
,"struct"    ,STRUCT
,"union"     ,STRUCT
,"enum"      ,ENUM
,"auto"      ,CLASS
,"extern"    ,CLASS
,"register"  ,CLASS
,"static"    ,CLASS
,"const"     ,CLASS
,"typedef"   ,CLASS
,"friend"    ,CLASS
,"inline"    ,CLASS
,"overload"  ,CLASS
,"operator"  ,OPERATOR
,"virtual"   ,CLASS
,"char"      ,TYPE
,"short"     ,TYPE
,"int"       ,TYPE
,"long"      ,TYPE
,"unsigned"  ,TYPE
,"signed"    ,SIGNED
,"float"     ,TYPE
,"double"    ,TYPE
,"void "     ,TYPE
,"private"   ,PRIVATE
,"public"    ,PUBLIC
,"protected" ,PROTECTED
,"while"     ,WHILE
,"if"        ,IF
,"else"      ,ELSE
,"switch"    ,SWITCH
,"case"      ,CASE
,"default"   ,DEFAULT
,"break"     ,BREAK
,"continue"  ,CONTINUE
,"return"    ,RETURN
,"goto"      ,GOTO
,"for"       ,FOR
,"throw"     ,THROW
,"try"       ,TRYSTMT
,"catch"     ,CATCH
,"template"  ,TEMPLATE
,"define"    ,DEFINE
,"endif"     ,ENDIF
,"ifdef"     ,IFDEF
,"ifndef"    ,IFNDEF
,"include"   ,INCLUDE
,"undef"     ,UNDEF
};

/*
static unsigned long keyword_hash( char * const &s ) {
  register unsigned int i = 0;
  register const char *cp = s;

  while( *cp ) i = ( i * 3 ) ^ *(cp++);
  return i;
}

static int keyword_cmp( char * const &e1, char * const &e2 ) {
  int ret = strcmp( e1, e2 );
  return ret;
}
*/

typedef StrHashMap<int> HashMapType;

class CppKeyWordMap : public HashMapType {
public:
  CppKeyWordMap() : HashMapType(23) {
    for(int i = 0; i < ARRAYSIZE(keywordtable); i++)
      put(keywordtable[i].m_name,keywordtable[i].m_token);
  }
};

static CppKeyWordMap keywords;

static int nameOrKeyWord(unsigned char *lexeme) {
  int *p = keywords.get((char*)lexeme);
  return p ? *p : NAME;
}

void CppLex::verror(const SourcePosition &pos, const char *format, va_list argptr) {
  if(m_parser==NULL)
    Scanner::verror(pos,format,argptr);
  else
    m_parser->verror(pos,format,argptr);
}
