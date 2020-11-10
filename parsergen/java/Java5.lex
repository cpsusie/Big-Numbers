%{

 /* This part goes to javalex.h */

class Java5Lex : public Scanner {
  void init();
public:
  int getNextLexeme() override;
  static void findBestHashMapSize();
};
%}

%{

 /* This part goes to the first part of javalex.cpp */

#include "stdafx.h"
#include <MyUtil.h>
#include <CompactHashMap.h>
#include <TreeMap.h>
#include "Java5Symbol.h"
#include "Java5Lex.h"

static Java5InputSymbol nameOrKeyWord(const TCHAR *lexeme);

%}

letter                        [_a-zA-Z\x80-\xff]            /* Letter                                                                                 */
letterordigit                 [_a-zA-Z\x80-\xff0-9]         /* Alphanumeric character                                             */
white                         [\f\s\t\n\r]                  /* White space:                                                                           */
d                             [0-9]                         /* Decimal digit                                                                */
h                             [0-9a-fA-F]                   /* Hexadecimal digit                                                            */
o                             [0-7]                         /* Octal digit                              */

%%

{d}+[lL]?                     |
0{o}+[lL]?                    |
0[xX]{h}+[lL]?                return INTEGERLITERAL;

({d}|{d}+\.{d}*|{d}*\.{d}+)([eE][\-+]?{d}+)?[fFdD]? return FLOATLITERAL;

\"(\\.|[^\\\"])*\"                                  return STRINGLITERAL;

'([^'\\]|\\.)'                                      return CHARACTERLITERAL;


{letter}{letterordigit}*                            return nameOrKeyWord(getText());

"/*"                                                { int i;
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
                                                        error( startpos,_T("End of file in comment\n") );
                                                      }
                                                    }

"//"                                                { int i;
                                                      while(i = input()) {
                                                        if(i < 0) {
                                                          flushBuf();  /* Discard lexeme. */
                                                        } else if(i == '\n') {
                                                          break;
                                                        }
                                                      }
                                                    }

"("                 return LPAR;
")"                 return RPAR;
"{"                 return LC;
"}"                 return RC;
"["                 return LB;
"]"                 return RB;
";"                 return SEMICOLON;
","                 return COMMA;
"."                 return DOT;
"="                 return ASSIGN;
"<"                 return LT;
">"                 return GT;
"!"                 return NOT;
"~"                 return COMPLEMENT;
"?"                 return QUESTION;
":"                 return COLON;
"..."               return ELLIPSIS;
"@"                 return AT;
"=="                return EQ;
"<="                return LE;
">="                return GE;
"!="                return NEQ;
"&&"                return ANDAND;
"||"                return OROR;
"++"                return PLUSPLUS;
"--"                return MINUSMINUS;
"+"                 return PLUS;
"-"                 return MINUS;
"*"                 return STAR;
"/"                 return DIV;
"&"                 return AND;
"|"                 return OR;
"^"                 return XOR;
"%"                 return MOD;
"<<"                return SHL;
">>"                return SSHR;
">>>"               return USHR;
"+="                return PLUSASSIGN;
"-="                return MINUSASSIGN;
"*="                return STARASSIGN;
"/="                return DIVASSIGN;
"&="                return ANDASSIGN;
"|="                return ORASSIGN;
"^="                return XORASSIGN;
"%="                return MODASSIGN;
"<<="               return SHLASSIGN;
">>="               return SSHRASSIGN;
">>>="              return USHRASSIGN;

{white}+  ;

%%

typedef struct {
  const TCHAR     *m_name;
  Java5InputSymbol m_token;
} ReservedWords;

static const ReservedWords wordTable[] = {
  _T("abstract"    ) ,ABSTRACT
 ,_T("assert"      ) ,ASSERT
 ,_T("boolean"     ) ,TYPEBOOLEAN
 ,_T("break"       ) ,BREAK
 ,_T("byte"        ) ,TYPEBYTE
 ,_T("case"        ) ,CASE
 ,_T("catch"       ) ,CATCH
 ,_T("char"        ) ,TYPECHAR
 ,_T("class"       ) ,CLASS
 ,_T("continue"    ) ,CONTINUE
 ,_T("default"     ) ,DEFAULT
 ,_T("do"          ) ,DO
 ,_T("double"      ) ,TYPEDOUBLE
 ,_T("else"        ) ,ELSE
 ,_T("enum"        ) ,ENUM
 ,_T("extends"     ) ,EXTENDS
 ,_T("false"       ) ,BOOLEANLITERAL
 ,_T("final"       ) ,FINAL
 ,_T("finally"     ) ,FINALLY
 ,_T("float"       ) ,TYPEFLOAT
 ,_T("for"         ) ,FOR
 ,_T("if"          ) ,IF
 ,_T("implements"  ) ,IMPLEMENTS
 ,_T("import"      ) ,IMPORT
 ,_T("instanceof"  ) ,INSTANCEOF
 ,_T("int"         ) ,TYPEINT
 ,_T("interface"   ) ,INTERFACE
 ,_T("long"        ) ,TYPELONG
 ,_T("native"      ) ,NATIVE
 ,_T("new"         ) ,NEW
 ,_T("null"        ) ,NULLLITERAL
 ,_T("package"     ) ,PACKAGE
 ,_T("private"     ) ,PRIVATE
 ,_T("protected"   ) ,PROTECTED
 ,_T("public"      ) ,PUBLIC
 ,_T("return"      ) ,RETURN
 ,_T("short"       ) ,TYPESHORT
 ,_T("static"      ) ,STATIC
 ,_T("strictfp"    ) ,STRICTFP
 ,_T("super"       ) ,SUPER
 ,_T("switch"      ) ,SWITCH
 ,_T("synchronized") ,SYNCHRONIZED
 ,_T("this"        ) ,THISLITERAL
 ,_T("throw"       ) ,THROW
 ,_T("throws"      ) ,THROWS
 ,_T("transient"   ) ,TRANSIENT
 ,_T("true"        ) ,BOOLEANLITERAL
 ,_T("try"         ) ,TRYTOKEN
 ,_T("void"        ) ,TYPEVOID
 ,_T("volatile"    ) ,VOLATILE
 ,_T("while"       ) ,WHILE
};

typedef CompactStrHashMap<Java5InputSymbol,ARRAYSIZE(wordTable) > HashMapType;

class ReservedWordMap : public HashMapType {
public:
  ReservedWordMap(UINT capacity) : HashMapType(capacity) {
    for(ReservedWords w : wordTable) {
      put(w.m_name,w.m_token);
    }
  }
};

static const ReservedWordMap reservedWordMap(249);

static Java5InputSymbol nameOrKeyWord(const TCHAR *lexeme) {
  const Java5InputSymbol *p = reservedWordMap.get(lexeme);
  return p ? *p : IDENTIFIER;
}

void Java5Lex::findBestHashMapSize() {
  UIntTreeMap<CompactUIntArray> cl;
  UINT bestCapacity;
  for(UINT capacity = 3; capacity < 2000; capacity++) {
    ReservedWordMap ht(capacity);
    cl.put((UINT)ht.getCapacity(), ht.getLength());
    if(ht.getMaxChainLength() == 1) {
      bestCapacity = capacity;
      break;
    }
  }

  for(auto it = cl.getIterator(); it.hasNext();) {
    Entry<UINT, CompactUIntArray> &e = it.next();
    UINT capacity = e.getKey();
    const CompactUIntArray &chainLength = e.getValue();
    printf("Capacity %4u:",capacity);
    for(size_t l = 0; l < chainLength.size(); l++) {
      printf(" (%zu,%3u)",l,chainLength[l]);
    }
    printf("\n");
  }
  printf("Capacity:%u gives best hashmap\n",bestCapacity);
}
