%{
#pragma once

 /* This part goes to javalex.h */

#include <Scanner.h>

class Java5Lex : public Scanner {
  void init();
public:
  int getNextLexeme();
  static void findBestHashMapSize();
};
%}

%{

 /* This part goes to the first part of javalex.cpp */

#include <MyUtil.h>
#include <HashMap.h>
#include <TreeMap.h>
#include "Java5Symbol.h"

static int nameOrKeyWord(const unsigned char *lexeme);

%}

letter			[_a-zA-Z\x80-\xff]		/* Letter									*/
letterordigit	[_a-zA-Z\x80-\xff0-9]	/* Alphanumeric character					*/
white			[\f\s\t\n\r]			/* White space:								*/
d				[0-9]					/* Decimal digit							*/
h				[0-9a-fA-F]				/* Hexadecimal digit						*/
o				[0-7]                   /* Octal digit                              */

%%

{d}+[lL]?					|
0{o}+[lL]?					|
0[xX]{h}+[lL]?				return INTEGERLITERAL;

({d}|{d}+\.{d}*|{d}*\.{d}+)([eE][\-+]?{d}+)?[fFdD]? return FLOATLITERAL;

\"(\\.|[^\\\"])*\"			return STRINGLITERAL;

'([^'\\]|\\.)'				return CHARACTERLITERAL;


{letter}{letterordigit}*	return nameOrKeyWord(getText());

"/*"						{ int i;
							  SourcePosition startpos = getPos();
							  while(i = input()) {
							    if(i < 0) {
							      flushBuf();  /* Discard lexeme. */
							    } else if(i == '*' && look(1) == '/') {
							     input();
							     break;       /* Recognized comment.*/
							    }
							  }
							  if(i == 0) {
							    error( startpos,"End of file in comment\n" );
                              }
							}

"//"						{ int i;
							  while(i = input()) {
							    if(i < 0) {
							      flushBuf();  /* Discard lexeme. */
							    } else if(i == '\n') {
							      break;
							    }
                              }
							}

"("		return LP;
")"		return RP;
"{"		return LC;
"}"		return RC;
"["		return LB;
"]"		return RB;
";"		return SEMICOLON;
","		return COMMA;
"."		return DOT;
"="		return ASSIGN;
"<"		return LT;
">"		return GT;
"!"		return NOT;
"~"		return COMPLEMENT;
"?"		return QUESTION;
":"		return COLON;
"..."	return ELLIPSIS;
"@"		return AT;
"=="	return EQ;
"<="	return LE;
">="	return GE;
"!="	return NEQ;
"&&"	return ANDAND;
"||"	return OROR;
"++"	return PLUSPLUS;
"--"	return MINUSMINUS;
"+"		return PLUS;
"-"		return MINUS;
"*"		return STAR;
"/"		return DIV;
"&"		return AND;
"|"		return OR;
"^"		return XOR;
"%"		return MOD;
"<<"	return SHL;
">>"	return SSHR;
">>>"	return USHR;
"+="	return PLUSASSIGN;
"-="	return MINUSASSIGN;
"*="	return STARASSIGN;
"/="	return DIVASSIGN;
"&="	return ANDASSIGN;
"|="	return ORASSIGN;
"^="	return XORASSIGN;
"%="	return MODASSIGN;
"<<="	return SHLASSIGN;
">>="	return SSHRASSIGN;
">>>="	return USHRASSIGN;

{white}+	;

%%

typedef struct {
  char *m_name;
  int   m_token;
} ReservedWords;

static ReservedWords wordTable[] = {
  "abstract"     ,ABSTRACT
 ,"assert"       ,ASSERT
 ,"boolean"      ,TYPEBOOLEAN
 ,"break"        ,BREAK
 ,"byte"         ,TYPEBYTE
 ,"case"         ,CASE
 ,"catch"        ,CATCH
 ,"char"         ,TYPECHAR
 ,"class"        ,CLASS
 ,"continue"     ,CONTINUE
 ,"default"      ,DEFAULT
 ,"do"           ,DO
 ,"double"       ,TYPEDOUBLE
 ,"else"         ,ELSE
 ,"enum"         ,ENUM
 ,"extends"      ,EXTENDS
 ,"false"        ,BOOLEANLITERAL
 ,"final"        ,FINAL
 ,"finally"      ,FINALLY
 ,"float"        ,TYPEFLOAT
 ,"for"          ,FOR
 ,"if"           ,IF
 ,"implements"   ,IMPLEMENTS
 ,"import"       ,IMPORT
 ,"instanceof"   ,INSTANCEOF
 ,"int"          ,TYPEINT
 ,"interface"    ,INTERFACE
 ,"long"         ,TYPELONG
 ,"native"       ,NATIVE
 ,"new"          ,NEW
 ,"null"         ,NULLLITERAL
 ,"package"      ,PACKAGE
 ,"private"      ,PRIVATE
 ,"protected"    ,PROTECTED
 ,"public"       ,PUBLIC
 ,"return"       ,RETURN
 ,"short"        ,TYPESHORT
 ,"static"       ,STATIC
 ,"strictfp"     ,STRICTFP
 ,"super"        ,SUPER
 ,"switch"       ,SWITCH
 ,"synchronized" ,SYNCHRONIZED
 ,"this"         ,THISLITERAL
 ,"throw"        ,THROW
 ,"throws"       ,THROWS
 ,"transient"    ,TRANSIENT
 ,"true"         ,BOOLEANLITERAL
 ,"try"          ,TRYTOKEN
 ,"void"         ,TYPEVOID
 ,"volatile"     ,VOLATILE
 ,"while"        ,WHILE
};

typedef StrHashMap<int> HashMapType;

class ReservedWordMap : public HashMapType {
public:
  ReservedWordMap(int capacity) : HashMapType(capacity) {
    for(int i = 0; i < ARRAYSIZE(wordTable); i++) {
      put(wordTable[i].m_name,wordTable[i].m_token);
	}
  }
};

static ReservedWordMap reservedWordMap(249);

static int nameOrKeyWord(const unsigned char *lexeme) {
  int *p = reservedWordMap.get((char*)lexeme);
  return p ? *p : IDENTIFIER;
}

void Java5Lex::findBestHashMapSize() {
  IntTreeMap<CompactIntArray> cl;
  int bestCapacity;
  for(int capacity = 3; capacity < 2000; capacity++) {
    ReservedWordMap ht(capacity);
	cl.put(ht.getCapacity(), ht.getLength());
	if(ht.getMaxChainLength() == 1) {
	  bestCapacity = capacity;
	  break;
	}
  }

  for(Iterator<Entry<int, CompactIntArray> > it = cl.entrySet().getIterator(); it.hasNext();) {
    Entry<int, CompactIntArray> &e = it.next();
    int capacity = e.getKey();
	const CompactIntArray &chainLength = e.getValue();
	printf("Capacity %4d:",capacity);
	for(int l = 0; l < chainLength.size(); l++) {
	  printf(" (%d,%3d)",l,chainLength[l]);
	}
	printf("\n");
  }
  printf("Capacity:%d gives best hashmap\n",bestCapacity);
}
