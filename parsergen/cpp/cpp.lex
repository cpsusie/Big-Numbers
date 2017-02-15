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
  void verror(const SourcePosition &pos, const TCHAR *format, va_list argptr);
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

static int nameOrKeyWord(TCHAR *lexeme);

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

\"(\\.|[^\\\"])*\r		error(getPos(),_T("Adding missing \" to string constant\n"));
						return STRING;

'.'						|
'\\.'					|
'\\{o}({o}{o}?)?'		|
'\\x{h}({h}{h}?)?'		|
0{o}*{suffix}?			|
0x{h}+{suffix}?			|
[1-9]{d}*{suffix}?		return ICON ;

({d}+|{d}+\.{d}*|{d}*\.{d}+)([eE][\-+]?{d}+)?[fF]?	return FCON ;

"("						return LPAR;
")"						return RPAR;
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
					error(p,_T("End of file in comment\n"));
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

.				error( getPos(), _T("Illegal character <%s>\n"), getText() );

%%

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
  if(m_parser==NULL) {
    Scanner::verror(pos, format, argptr);
  } else {
    m_parser->verror(pos, format, argptr);
  }
}
