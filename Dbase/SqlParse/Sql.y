%{


  /* This part goes to SqlParser.h */

#include "Sqlsymbol.h"
#include "ParserTree.h"
#include "SqlSymbol.h"
#include "SqlLex.h"

class SqlParser : public LRparser {
private:
  static const AbstractParserTables *SqlTables;
  ParserTree &m_tree;
  Date      ttoDate(     const TCHAR *str);
  Time      ttoTime(     const TCHAR *str);
  Timestamp ttoTimestamp(const TCHAR *str);
  SyntaxNodeP m_leftSide,*m_stacktop,m_userstack[256];
  SyntaxNodeP getStackTop(int fromtop)            { return m_stacktop[-fromtop];            }
  int  userStackGetHeight() const                 { return (int)(m_stacktop - m_userstack); }
  void userStackInit()                   override { m_stacktop = m_userstack;               }
  void userStackShiftSymbol(UINT symbol) override { m_stacktop++;                           } // push 1 element (garbage) on userstack
  void userStackPopSymbols( UINT count ) override { m_stacktop      -= count;               } // pop count symbols from userstack
  void userStackShiftLeftSide()          override { *(++m_stacktop) = m_leftSide;           } // push($$) on userstack
  void defaultReduce(       UINT prod  ) override { m_leftSide      = getStackTop(getProductionLength(prod)?0:1);} // $$ = $1
  int  reduceAction(        UINT prod  ) override;
  SyntaxNode       *newNode(const SourcePosition &pos, int token, ...  );
public:
  SqlParser(ParserTree &tree, Scanner *lex = nullptr) : LRparser(*SqlTables,lex), m_tree(tree) {
  }
  void      parse( const SourcePosition &pos, const String &stmt); // parse stmt
  void      verror(const SourcePosition &pos, const TCHAR  *format, va_list argptr) override;
  static inline const AbstractParserTables &getTables() {
    return *SqlTables;
  }
};

%}

%term STRING
%term NUMBER
%term DATECONST TIMECONST TIMESTAMPCONST
%term PARAM
%term HOSTVAR

%term ACCESSS
%term ALL
%term AS
%term ASCENDING
%term BETWEEN
%term BROWSE
%term BY
%term CAST
%term COLUMN
%term COUNT
%term CREATE
%term CURRENTDATE CURRENTTIME CURRENTTIMESTAMP
%term CURSOR
%term DECLARE
%term DEFAULT
%term DELETESYM
%term DESCENDING
%term DISTINCT
%term DROP
%term EXCLUSIVE
%term EXISTS
%term EXPLAIN
%term FETCH
%term FOR
%term FROM
%term GROUP
%term HAVING
%term INSYM
%term INDEX
%term INSERT
%term INDICATOR
%term INNER
%term INTO
%term IS
%term JOIN
%term KEY
%term LEFT
%term LIKE
%term MAX
%term MIN
%term MODE
%term MULT  /* used as syntaxnode-token */
%term NULLVAL
%term ISNULL
%term ON
%term ORDER
%term PRIMARY
%term REPEATABLE
%term SELECT
%term SET
%term SHARE
%term SIGNED
%term STABLE
%term SUBSTRING
%term SUM
%term TABLE
%term UNIQUE
%term UNSIGNED
%term UPDATE
%term VALUES
%term WHERE
%term TYPECHAR TYPEWCHAR
%term TYPEVARCHAR
%term TYPEDOUBLE
%term TYPEFLOAT
%term TYPEINT
%term TYPELONG
%term TYPESHORT
%term TYPEDATE TYPETIME TYPETIMESTAMP

%left UNION SETDIFFERENCE /* lowest precedence of set-operators */
%left INTERSECT

%term COMMA DOT EQUAL NOTEQ RELOPLE RELOPLT RELOPGE RELOPGT

%left OR                /*  (lowest precedence)         */
%left AND               /*  (highest precedence)        */

%left  PLUS MINUS       /*  + - (lowest precedence)     */
%left  STAR DIVOP MODOP /*  * / %                       */
%left  EXPO             /*  **                          */
%right NOT
%left  LPAR RPAR        /*  ( ) (highest precedence)    */

%term CONCAT

%nonassoc NAME

%{
#include "stdafx.h"
#include "SqlParser.h"
#include <SqlApi.h>
#include <io.h>

%}

%%

start           : stmt                                          { m_tree.m_root = $1; }
                ;

stmt            : update_stmt
                | insert_stmt
                | delete_stmt
                | select_stmt
                | declare_stmt
                | fetch_stmt
                | create_stmt
                | drop_stmt
                | explain_stmt
                ;

update_stmt     : UPDATE name SET assign_list opt_where         { $$ = newNode( getPos(1), UPDATE, $2, $4, $5, nullptr ); }
                ;

assign_list     : assign_list COMMA assign                      { $$ = newNode( getPos(2), COMMA, $1, $3, nullptr ); }
                | assign
                ;

assign          : name EQUAL expr                               { $$ = newNode( getPos(2), EQUAL, $1, $3, nullptr ); }
                ;

insert_stmt     : INSERT INTO name LPAR name_list RPAR VALUES LPAR opt_expr_list RPAR opt_access
/* 1 */
                                                                { $$ = newNode( getPos(1), INSERT,
                                                                        $3,
                                                                        newNode( getPos(5), COLUMN, $5, nullptr ),
                                                                        newNode( getPos(7), VALUES, $9, nullptr ),
                                                                        $11,
                                                                        nullptr );
                                                                }

                | INSERT INTO name VALUES LPAR opt_expr_list RPAR opt_access
/* 2 */
                                                                { $$ = newNode( getPos(1), INSERT,
                                                                        $3,
                                                                        newNode( getPos(4), COLUMN, nullptr     ),
                                                                        newNode( getPos(4), VALUES, $6, nullptr ),
                                                                        $8,
                                                                        nullptr );
                                                                }

                | INSERT INTO name LPAR name_list RPAR LPAR select_stmt RPAR opt_access
/* 3 */
                                                                { $$ = newNode( getPos(1), INSERT,
                                                                        $3,
                                                                        newNode( getPos(5), COLUMN, $5, nullptr ),
                                                                        $8,
                                                                        $10,
                                                                        nullptr );
                                                                }

                | INSERT INTO name LPAR select_stmt RPAR opt_access
/* 4 */
                                                                { $$ = newNode( getPos(1), INSERT,
                                                                        $3, 
                                                                        newNode( getPos(4), COLUMN, nullptr ),
                                                                        $5,
                                                                        $7,
                                                                        nullptr );
                                                                }

                | INSERT INTO name LPAR name_list RPAR select_stmt
/* 5 NB no opt_access here. will cause shift/reduce conflict */
                                                                { $$ = newNode( getPos(1), INSERT,
                                                                        $3,
                                                                        newNode( getPos(5), COLUMN, $5, nullptr ),
                                                                        $7,
                                                                        newNode( getPos(), ACCESSS, nullptr         ),
                                                                        nullptr );
                                                                }

                | INSERT INTO name select_stmt
/* 6 NB no opt_access here. will cause shift/reduce conflict */
                                                                { $$ = newNode( getPos(1), INSERT,
                                                                        $3, 
                                                                        newNode( getPos(3), COLUMN, nullptr   ),
                                                                        $4, 
                                                                        newNode( getPos(), ACCESSS, nullptr       ),
                                                                        nullptr );
                                                                }
                ;

name_list       : name           
                | name_list COMMA name                          { $$ = newNode( getPos(2), COMMA, $1, $3, nullptr );       }
                ;

opt_expr_list   : /* eps */                                     { $$ = nullptr; }
                | expr_list
                ;

delete_stmt     : DELETESYM FROM name opt_where opt_access      { $$ = newNode( getPos(1), DELETESYM, $3, $4, $5, nullptr );     }
                ;

select_stmt     : select_stmt UNION opt_all select_stmt         { $$ = newNode( getPos(2), UNION        , $1, $4, $3, nullptr ); }
                | select_stmt INTERSECT     select_stmt         { $$ = newNode( getPos(2), INTERSECT    , $1, $3, nullptr );     }
                | select_stmt SETDIFFERENCE select_stmt         { $$ = newNode( getPos(2), SETDIFFERENCE, $1, $3, nullptr );     }
                | LPAR select_stmt RPAR                         { $$ = $2;                                                    }
                | simple_select
                ;

/* NB - the grammar allows more into-clauses when using union
   and also in insert...select and exist (select...
   it also allows orderby for all selects in a union,intersect..
   these errors should be checked in sqlcompiler
*/
simple_select   :
                SELECT opt_alldistinct select_list
                opt_into
                FROM from_table_list 
                opt_where
                opt_groupby
                opt_having
                opt_orderby
                opt_access
                opt_lockmode                                    { $$ = newNode( getPos(1), SELECT,
                                                                        $2,
                                                                        $3,
                                                                        $4,
                                                                        newNode( getPos(5), FROM, $6, nullptr ),
                                                                        $7,
                                                                        $8,
                                                                        $9,
                                                                        $10,
                                                                        $11,
                                                                        $12,
                                                                        nullptr );
                                                                }
                ;

opt_all         : /* eps */                                     { $$ = nullptr;                                      }
                | ALL                                           { $$ = newNode( getPos(1), ALL, nullptr );           }
                ;

opt_alldistinct : /* eps */                                     { $$ = newNode( getPos(0), ALL, nullptr );           }
                | ALL                                           { $$ = newNode( getPos(1), ALL, nullptr );           }
                | DISTINCT                                      { $$ = newNode( getPos(1), DISTINCT, nullptr );      }
                ;

select_list     : select_list COMMA select_elem                 { $$ = newNode( getPos(2), COMMA, $1, $3, nullptr ); }
                | select_elem
                ;

select_elem     : STAR                                          { $$ = newNode( getPos(1), STAR, nullptr );          }
                | name DOT STAR                                 { $$ = newNode( getPos(2), DOT,
                                                                        $1,
                                                                        newNode( getPos(3), STAR, nullptr ),
                                                                        nullptr );
                                                                }
                | expr
                ;

opt_into        : /* eps */                                     { $$ = newNode( getPos(0), INTO,nullptr );           }
                | INTO hostvar_list                             { $$ = newNode( getPos(1), INTO, $2, nullptr );      }
                ;

hostvar_list    : hostvar_list COMMA hostindi_var               { $$ = newNode( getPos(2), COMMA, $1, $3, nullptr ); }
                | hostindi_var
                ;

/* we need only the hostvar, not the indicator in the syntax_tree, but in the grammar the indicator has to be there */
hostindi_var    : hostvar opt_indicator                         { $$ = $1; } /*  newNode( HOSTVAR, $1, $2, nullptr ); } */
                ;
                
opt_indicator   :
                /* eps */                                       /*{ $$ = newNode( getPos(0), INDICATOR, nullptr );    }*/
                | hostvar                                       /*{ $$ = newNode( getPos(1), INDICATOR, $1, nullptr ); }*/
                | INDICATOR hostvar                             /*{ $$ = newNode( getPos(1), INDICATOR, $2, nullptr ); }*/
                ;

from_table_list : from_table_list COMMA from_table              { $$ = newNode( getPos(2), COMMA, $1, $3, nullptr ); }
                | from_table
                ;

from_table      : name
                | name name                                     { $$ = newNode( getPos(1), DOT, $1, $2, nullptr );   }
                | LPAR from_table opt_join_type JOIN from_table ON search_condition RPAR
/* paranteses around search_condition to avoid shift/reduce conflict */
                                                                { $$ = newNode( getPos(2), JOIN, $2, $3, $5, $7, nullptr );
                                                                }
                ;

opt_join_type   : /* eps */                                     { $$ = newNode( getPos(0), INNER, nullptr );         }
                | INNER                                         { $$ = newNode( getPos(1), INNER, nullptr );         }
                | LEFT                                          { $$ = newNode( getPos(1), LEFT , nullptr );         }
                ;

opt_where       : /* eps */                                     { $$ = newNode( getPos(0), WHERE, nullptr );         }
                | WHERE search_condition                        { $$ = newNode( getPos(1), WHERE, $2, nullptr );     }
                ;

opt_having      : /* eps */                                     { $$ = newNode( getPos(0), HAVING, nullptr );        }
                | HAVING search_condition                       { $$ = newNode( getPos(1), HAVING, $2, nullptr );    }
                ;

opt_groupby     : /* eps */                                     { $$ = newNode( getPos(0), GROUP, nullptr );         }
                | GROUP BY expr_list                            { $$ = newNode( getPos(1), GROUP, $3, nullptr );     }
                ;

opt_orderby     : /* eps */                                     { $$ = newNode( getPos(0), ORDER, nullptr );         }
                | ORDER BY order_elem_list                      { $$ = newNode( getPos(1), ORDER, $3, nullptr );     }
                ;

order_elem_list : order_elem_list COMMA order_elem              { $$ = newNode( getPos(2), COMMA, $1, $3, nullptr ); }
                | order_elem
                ;

order_elem      : expr opt_ascdesc                              { $$ = newNode( getPos(1), BY, $1, $2, nullptr );    }
                ;

opt_ascdesc     : /* eps */                                     { $$ = newNode( getPos(0), ASCENDING , nullptr );    }
                | ASCENDING                                     { $$ = newNode( getPos(1), ASCENDING , nullptr );    }
                | DESCENDING                                    { $$ = newNode( getPos(1), DESCENDING, nullptr );    }
                ;

opt_access      : /* eps */                                     { $$ = newNode( getPos(0), ACCESSS, nullptr      );  }
                | FOR  access_mode ACCESSS                      { $$ = newNode( getPos(2), ACCESSS, $2 , nullptr );  }
                |      access_mode ACCESSS                      { $$ = newNode( getPos(1), ACCESSS, $1 , nullptr );  }
                ;

access_mode     : BROWSE                                        { $$ = newNode( getPos(1), BROWSE    , nullptr );    }
                | STABLE                                        { $$ = newNode( getPos(1), STABLE    , nullptr );    }
                | REPEATABLE                                    { $$ = newNode( getPos(1), REPEATABLE, nullptr );    }
                ;

opt_lockmode    : /* eps */                                     { $$ = newNode ( getPos(0), MODE, nullptr     );     }
                | INSYM lockmode MODE                           { $$ = newNode ( getPos(2), MODE, $2, nullptr );     }
                |       lockmode MODE                           { $$ = newNode ( getPos(1), MODE, $1, nullptr );     }
                ;

lockmode        : SHARE                                         { $$ = newNode( getPos(1), SHARE     , nullptr );    }
                | EXCLUSIVE                                     { $$ = newNode( getPos(1), EXCLUSIVE , nullptr );    }
                ;

search_condition: search_condition OR  search_condition         { $$ = newNode( getPos(2), OR , $1, $3, nullptr);    }
                | search_condition AND search_condition         { $$ = newNode( getPos(2), AND, $1, $3, nullptr);    }
                | NOT boolean_primary                           { $$ = newNode( getPos(1), NOT, $2, nullptr );       }
                | boolean_primary
                ;

boolean_primary : predicate      
                | LPAR search_condition RPAR                    { $$ = $2; }
                ;

predicate       : par_expr_list rel_op par_expr_list            { $$ = newNode( getPos(2), $2->token(), $1, $3, nullptr );
                                                                }
                | EXISTS LPAR select_stmt RPAR                  { $$ = newNode( getPos(1), EXISTS, $3, nullptr );
                                                                }
                | par_expr_list BETWEEN par_expr_list AND par_expr_list
                                                                { $$ = newNode( getPos(2), BETWEEN, $1, $3, $5, nullptr );
                                                                }
                | par_expr_list NOT BETWEEN par_expr_list AND par_expr_list
                                                                { $$ = newNode( getPos(2), NOT,
                                                                        newNode( getPos(3), BETWEEN, $1, $4, $6, nullptr ),
                                                                        nullptr );
                                                                }
/* actually only 1 single expr. but this gives shift/reduce-conflicts. We check it in the compiler */
                | par_expr_list INSYM LPAR set_expr RPAR        { $$ = newNode( getPos(2), INSYM, $1, $4, nullptr ); }

                | par_expr_list NOT INSYM LPAR set_expr RPAR    { $$ = newNode( getPos(2), NOT,
                                                                        newNode( getPos(3), INSYM, $1, $5, nullptr ),
                                                                        nullptr );
                                                                }
/* actually only 1 single expr. but this gives shift/reduce-conflicts. We check it in the compiler */
                | par_expr_list LIKE expr                       { $$ = newNode( getPos(2), LIKE, $1, $3, nullptr );  }
                | par_expr_list NOT LIKE expr                   { $$ = newNode( getPos(2), NOT,
                                                                        newNode( getPos(3), LIKE, $1, $4, nullptr),
                                                                        nullptr );
                                                                }
/* actually only 1 single expr. but this gives shift/reduce-conflicts. We check it in the compiler */
                | par_expr_list IS NULLVAL                      { $$ = newNode( getPos(2), ISNULL, $1, nullptr );    }
                | par_expr_list IS NOT NULLVAL                  { $$ = newNode( getPos(3), NOT,
                                                                        newNode( getPos(2), ISNULL, $1, nullptr ),
                                                                        nullptr );
                                                                }
                ;

set_expr        : select_stmt
                | expr_list
                ;

par_expr_list   : expr_list
//              | LPAR expr_list RPAR                           { $$ = $2; }
// this production gives shift/reduce conflict
                ;

rel_op          : RELOPLE                                       { $$ = newNode(getPos(1), RELOPLE,nullptr);                }
                | RELOPLT                                       { $$ = newNode(getPos(1), RELOPLT,nullptr);                }
                | RELOPGE                                       { $$ = newNode(getPos(1), RELOPGE,nullptr);                }
                | RELOPGT                                       { $$ = newNode(getPos(1), RELOPGT,nullptr);                }
                | EQUAL                                         { $$ = newNode(getPos(1), EQUAL  ,nullptr);                }
                | NOTEQ                                         { $$ = newNode(getPos(1), NOTEQ  ,nullptr);                }
                ;

expr_list       : expr_list COMMA expr                          { $$ = newNode( getPos(2), COMMA, $1, $3, nullptr );   }
                | expr                   
                ;

expr            : expr PLUS  expr                               { $$ = newNode( getPos(2), PLUS  , $1, $3, nullptr );  }
                | expr MINUS expr                               { $$ = newNode( getPos(2), MINUS , $1, $3, nullptr );  }
                | expr CONCAT expr                              { $$ = newNode( getPos(2), CONCAT, $1, $3, nullptr);   }
/* need to be here to avoid shift/reduce-conflicts */
                | expr STAR  expr                               { $$ = newNode( getPos(2), MULT  , $1, $3, nullptr );  }
                | expr DIVOP expr                               { $$ = newNode( getPos(2), DIVOP , $1, $3, nullptr );  }
                | expr MODOP expr                               { $$ = newNode( getPos(2), MODOP , $1, $3, nullptr );  }               
                | expr EXPO expr                                { $$ = newNode( getPos(2), EXPO  , $1, $3, nullptr );  }
                | MINUS unary                                   { $$ = newNode( getPos(1), MINUS , $2, nullptr );      }
                | PLUS  unary                                   { $$ = $2; }
                | unary
                ;

unary           : number_const
                | date_const
                | time_const
                | timestamp_const
                | hostindi_var
                | tab_col
                | param
                | string_const
                | CURRENTDATE                                   { $$ = newNode( getPos(1), CURRENTDATE, nullptr );      }
                | CURRENTTIME                                   { $$ = newNode( getPos(1), CURRENTTIME, nullptr );      }
                | CURRENTTIMESTAMP                              { $$ = newNode( getPos(1), CURRENTTIMESTAMP, nullptr);  }
                | NULLVAL                                       { $$ = newNode( getPos(1), NULLVAL, nullptr );          }
                | LPAR expr RPAR                                { $$ = $2;                                              }
                | CAST LPAR expr AS type_def RPAR               { $$ = newNode( getPos(1), CAST, $3, $5, nullptr );     }
                | LPAR select_stmt RPAR                         { $$ = $2;                                              }
                | SUBSTRING LPAR expr FROM  expr FOR expr RPAR  { $$ = newNode(  getPos(1), SUBSTRING, $3, $5, $7, nullptr ); }
                | SUBSTRING LPAR expr COMMA expr COMMA expr RPAR {$$ = newNode(  getPos(1), SUBSTRING, $3, $5, $7, nullptr ); }
                | TYPEDATE  LPAR expr COMMA expr COMMA expr RPAR { $$ = newNode( getPos(1), TYPEDATE , $3, $5, $7, nullptr ); }
                | TYPETIME  LPAR expr COMMA expr COMMA expr RPAR { $$ = newNode( getPos(1), TYPETIME , $3, $5, $7, nullptr ); }
                | TYPETIMESTAMP LPAR expr COMMA expr COMMA 
                                     expr COMMA expr COMMA 
                                     expr COMMA expr RPAR       { $$ = newNode( getPos(1), TYPETIMESTAMP, $3, $5, $7, $9, $11, $13, nullptr );
                                                                }
                | COUNT LPAR count_arg RPAR                     { $$ = newNode( getPos(1), COUNT, $3, nullptr); }
                | min_max_sum LPAR opt_alldistinct expr RPAR    { $$ = newNode( getPos(1), $1->token(), $3, $4, nullptr ); }
                ;

min_max_sum     : MIN                                           { $$ = newNode(getPos(1), MIN,nullptr); }
                | MAX                                           { $$ = newNode(getPos(1), MAX,nullptr); }
                | SUM                                           { $$ = newNode(getPos(1), SUM,nullptr); }
                ;

count_arg       : opt_alldistinct expr                          { $$ = newNode(getPos(1), $1->token(), $2, nullptr );    }
                | STAR                                          { $$ = newNode(getPos(1), STAR,nullptr);                 }
                ;

tab_col         : name                   
                | name DOT name                                 { $$ = newNode ( getPos(2), DOT, $1, $3, nullptr );      }
                ;

declare_stmt    : DECLARE name CURSOR FOR select_stmt           { $$ = newNode ( getPos(1), DECLARE, $2, $5, nullptr );  }
                ;

fetch_stmt      : FETCH name INTO hostvar_list                  { $$ = newNode( getPos(1), FETCH, $2, $4, nullptr );     }
                ;

create_stmt     : CREATE TABLE name LIKE name                   { $$ = newNode( getPos(1), CREATE,
                                                                        newNode(getPos(2), TABLE,nullptr),
                                                                        newNode(getPos(4), LIKE,nullptr),
                                                                        $3,
                                                                        $5,
                                                                        nullptr );
                                                                }
                | CREATE TABLE name LPAR col_def_list COMMA
                                      PRIMARY KEY LPAR keyfield_list RPAR
                                    RPAR
                                                                { $$ = newNode( getPos(1), CREATE,
                                                                        newNode(getPos(2), TABLE,nullptr),
                                                                        newNode(getPos(7), KEY, $10, nullptr ),
                                                                        $3,
                                                                        $5,
                                                                        nullptr );
                                                                }
                | CREATE opt_unique INDEX name ON name LPAR keyfield_list RPAR
                                                                { $$ = newNode( getPos(1), CREATE,
                                                                        newNode(getPos(3), INDEX,nullptr),
                                                                        $2,
                                                                        $4,
                                                                        $6,
                                                                        $8,
                                                                        nullptr );
                                                                }
                ;

opt_unique      : /* eps */                                     { $$ = newNode( getPos(0), NOT,
                                                                        newNode(getPos(0), UNIQUE,nullptr),
                                                                        nullptr);
                                                                }
                | UNIQUE                                        { $$ = newNode( getPos(1), UNIQUE,nullptr);
                                                                }
                ;

keyfield_list   : keyfield_list COMMA keyfield                  { $$ = newNode( getPos(2), COMMA, $1, $3, nullptr );       }
                | keyfield
                ;

keyfield        : name opt_ascdesc                              { $$ = newNode( getPos(1), COLUMN, $1, $2, nullptr );      }
                ;

col_def_list    : col_def_list COMMA col_def                    { $$ = newNode( getPos(2), COMMA, $1, $3, nullptr );       }
                | col_def                
                ;

col_def         : name type_def opt_not_null opt_defaultvalue   { $$ = newNode( getPos(1), COLUMN, $1, $2, $3, $4, nullptr ); }
                ;

type_def        : opt_sign TYPECHAR                             { $$ = newNode(getPos(2), TYPECHAR     , $1,     nullptr);  }
                | opt_sign TYPEWCHAR                            { $$ = newNode(getPos(2), TYPEWCHAR    , $1,     nullptr);  }
                | opt_sign TYPECHAR LPAR number_const RPAR      { $$ = newNode(getPos(2), TYPECHAR     , $1, $4, nullptr);  }
                | opt_sign TYPEWCHAR LPAR number_const RPAR     { $$ = newNode(getPos(2), TYPEWCHAR    , $1, $4, nullptr);  }
                | opt_sign TYPESHORT                            { $$ = newNode(getPos(2), TYPESHORT    , $1,     nullptr);  }
                | opt_sign TYPEINT                              { $$ = newNode(getPos(2), TYPEINT      , $1,     nullptr);  }
                | opt_sign TYPELONG                             { $$ = newNode(getPos(2), TYPELONG     , $1,     nullptr);  }
                | TYPEFLOAT                                     { $$ = newNode(getPos(1), TYPEFLOAT    ,         nullptr);  }
                | TYPEDOUBLE                                    { $$ = newNode(getPos(1), TYPEDOUBLE   ,         nullptr);  }
                | TYPEDATE                                      { $$ = newNode(getPos(1), TYPEDATE     ,         nullptr);  }
                | TYPETIME                                      { $$ = newNode(getPos(1), TYPETIME     ,         nullptr);  }
                | TYPETIMESTAMP                                 { $$ = newNode(getPos(1), TYPETIMESTAMP,         nullptr);  }
                | TYPEVARCHAR LPAR number_const RPAR            { $$ = newNode(getPos(1), TYPEVARCHAR  , $3,     nullptr);  }
                ;

opt_sign        : /* eps */                                     { $$ = newNode(getPos(0), SIGNED,nullptr);                  }
                | SIGNED                                        { $$ = newNode(getPos(1), SIGNED,nullptr);                  }
                | UNSIGNED                                      { $$ = newNode(getPos(1), UNSIGNED,nullptr);                }
                ;

opt_not_null    : /* eps */                                     { $$ = newNode(getPos(0), NULLVAL,nullptr);                 } /* default null allowed */
                | NOT NULLVAL                                   { $$ = newNode(getPos(1), NOT,
                                                                        newNode(getPos(2), NULLVAL,nullptr),
                                                                        nullptr );
                                                                }
                ;

opt_defaultvalue: /* eps */                                     { $$ = newNode(getPos(0), DEFAULT, nullptr);                }
                | DEFAULT constant                              { $$ = newNode(getPos(1), DEFAULT, $2, nullptr);            }
                ;

constant        : string_const
                | number_const
                | PLUS number_const                             { $$ = $2;                                               }
                | MINUS number_const                            { $$ = newNode( getPos(1), NUMBER, -$2->number());       }
                | date_const
                | time_const
                | timestamp_const
                ;

drop_stmt       : DROP TABLE name                               { $$ = newNode( getPos(1), DROP,
                                                                        newNode(getPos(2), TABLE    , nullptr),
                                                                        $3, nullptr );
                                                                }
                | DROP INDEX name                               { $$ = newNode( getPos(1), DROP,
                                                                        newNode(getPos(2), INDEX    , nullptr),
                                                                        $3, nullptr );
                                                                }
                ;

explain_stmt    : EXPLAIN stmt                                  { $$ = newNode( getPos(1), EXPLAIN, $2, nullptr    );      }
                ;

name            : NAME                                          { $$ = newNode( getPos(1), NAME, getText());               }
                ;

hostvar         : HOSTVAR                                       { $$ = newNode( getPos(1), HOSTVAR, getText());            }
                ;

param           : PARAM                                         { $$ = newNode( getPos(1), PARAM, getText());              }
                ;

number_const    : NUMBER                                        { $$ = newNode( getPos(1), NUMBER, _ttof(getText()));      }
                ;

string_const    : STRING                                        { $$ = newNode( getPos(1), STRING, getText());             }
                ;

date_const      : DATECONST                                     { $$ = newNode( getPos(1), DATECONST, ttoDate(getText())); }
                ;

time_const      : TIMECONST                                     { $$ = newNode( getPos(1), TIMECONST, ttoTime(getText())); }
                ;

timestamp_const : TIMESTAMPCONST                                { $$ = newNode( getPos(1), TIMESTAMPCONST, ttoTimestamp(getText())); }
                ;
%%
/*----------------------------------------------------------------------*/

Date SqlParser::ttoDate(const TCHAR *str) {
  try {
    return Date(str+1);
  } catch(Exception e) {
    error(getPos(),_T("%s"), e.what());
    return Date();
  }
}

Time SqlParser::ttoTime(const TCHAR *str) {
  try {
    return Time(str+1);
  } catch(Exception e) {
    error(getPos(), _T("%s"), e.what());
    return Time();
  }
}

Timestamp SqlParser::ttoTimestamp(const TCHAR *str) {
  try {
    return Timestamp(str+1);
  } catch(Exception e) {
    error(getPos(), _T("%s"), e.what());
    return Timestamp();
  }
}

void SqlParser::verror(const SourcePosition &pos, const TCHAR *format, va_list argptr) {
  m_tree.vAppendError(pos,SQL_SYNTAX_ERROR, format, argptr);
}

void SqlParser::parse(const SourcePosition &pos, const String &stmt) {
  LexStringStream stream(stmt);
  SqlLex lex(this);
  lex.newStream(&stream,pos);
  setScanner(&lex);
//  setdebug(true);
//  lex.setdebug(true);
  LRparser::parse();
}

SyntaxNode *SqlParser::newNode(const SourcePosition &pos, int token, ...) {
  va_list argptr;
  va_start(argptr,token);
  SyntaxNode *p = m_tree.vFetchTokenNode(token,argptr);
  va_end(argptr);
  p->setpos(pos);
  return p;
}
