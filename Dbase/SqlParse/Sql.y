%{


  /* This part goes to SqlParser.h */

#include "Sqlsymbol.h"
#include "ParserTree.h"
#include "SqlSymbol.h"
#include "SqlLex.h"

class SqlParser : public LRparser {
private:
  ParserTree &m_tree;
  Date      ttoDate(     const TCHAR *str);
  Time      ttoTime(     const TCHAR *str);
  Timestamp ttoTimestamp(const TCHAR *str);
  SyntaxNodeP m_leftSide,*m_stacktop,m_userstack[256];
  SyntaxNodeP getStackTop(int fromtop)           { return m_stacktop[-fromtop];            }
  int  userStackGetHeight() const                { return (int)(m_stacktop - m_userstack); }
  void userStackInit()		                     { m_stacktop = m_userstack;		       }
  void userStackShiftSymbol(unsigned int symbol) { m_stacktop++;					       } // push 1 element (garbage) on userstack
  void userStackPopSymbols( unsigned int count ) { m_stacktop      -= count; 	           } // pop count symbols from userstack
  void userStackShiftLeftSide()                  { *(++m_stacktop) = m_leftSide;           } // push($$) on userstack
  void defaultReduce(       unsigned int prod)   { m_leftSide      = getStackTop(getProductionLength(prod)?0:1);} // $$ = $1
  int  reduceAction(        unsigned int prod);
  SyntaxNode       *newNode(const SourcePosition &pos, int token, ...  );
public:
  SqlParser(ParserTree &tree, Scanner *lex = NULL) : LRparser(*SqlTables,lex), m_tree(tree) {
  }
  void      parse( const SourcePosition &pos, const String &stmt); // parse stmt
  void      verror(const SourcePosition &pos, const TCHAR  *format, va_list argptr);
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
%term MULT	/* used as syntaxnode-token */
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

%left OR				/*  (lowest precedence)		    */
%left AND				/*  (highest precedence)		*/

%left  PLUS MINUS		/*  + - (lowest precedence)		*/
%left  STAR DIVOP MODOP /*  * / %						*/
%left  EXPO 			/*  **							*/
%right NOT
%left  LPAR RPAR		/*  ( ) (highest precedence)	*/

%term CONCAT

%nonassoc NAME

%{
#include "stdafx.h"
#include "SqlParser.h"
#include <SqlApi.h>
#include <io.h>

%}

%%

start			: stmt											{ m_tree.m_root = $1; }
				;

stmt			: update_stmt
				| insert_stmt
				| delete_stmt
				| select_stmt
				| declare_stmt
                | fetch_stmt
				| create_stmt
				| drop_stmt
				| explain_stmt
				;

update_stmt 	: UPDATE name SET assign_list opt_where 		{ $$ = newNode( getPos(1), UPDATE, $2, $4, $5, NULL ); }
				;

assign_list 	: assign_list COMMA assign						{ $$ = newNode( getPos(2), COMMA, $1, $3, NULL ); }
				| assign
				;

assign			: name EQUAL expr								{ $$ = newNode( getPos(2), EQUAL, $1, $3, NULL ); }
				;

insert_stmt 	: INSERT INTO name LPAR name_list RPAR VALUES LPAR opt_expr_list RPAR opt_access
/* 1 */
																{ $$ = newNode( getPos(1), INSERT,
																		$3,
																		newNode( getPos(5), COLUMN, $5, NULL ),
																		newNode( getPos(7), VALUES, $9, NULL ),
																		$11,
																		NULL );
																}

				| INSERT INTO name VALUES LPAR opt_expr_list RPAR opt_access
/* 2 */
																{ $$ = newNode( getPos(1), INSERT,
																		$3,
																		newNode( getPos(4), COLUMN, NULL     ),
																		newNode( getPos(4), VALUES, $6, NULL ),
																		$8,
																		NULL );
																}

				| INSERT INTO name LPAR name_list RPAR LPAR select_stmt RPAR opt_access
/* 3 */
																{ $$ = newNode( getPos(1), INSERT,
																		$3,
																		newNode( getPos(5), COLUMN, $5, NULL ),
																		$8,
																		$10,
																		NULL );
																}

				| INSERT INTO name LPAR select_stmt RPAR opt_access
/* 4 */
																{ $$ = newNode( getPos(1), INSERT,
																		$3, 
																		newNode( getPos(4), COLUMN, NULL ),
																		$5,
																		$7,
																		NULL );
																}

				| INSERT INTO name LPAR name_list RPAR select_stmt
/* 5 NB no opt_access here. will cause shift/reduce conflict */
																{ $$ = newNode( getPos(1), INSERT,
																		$3,
																		newNode( getPos(5), COLUMN, $5, NULL ),
																		$7,
																		newNode( getPos(), ACCESSS, NULL	     ),
																		NULL );
																}

				| INSERT INTO name select_stmt
/* 6 NB no opt_access here. will cause shift/reduce conflict */
																{ $$ = newNode( getPos(1), INSERT,
																		$3, 
																		newNode( getPos(3), COLUMN, NULL   ),
																		$4, 
																		newNode( getPos(), ACCESSS, NULL	   ),
																		NULL );
																}
				;

name_list		: name			 
				| name_list COMMA name							{ $$ = newNode( getPos(2), COMMA, $1, $3, NULL );       }
				;

opt_expr_list	: /* eps */										{ $$ = NULL; }
				| expr_list
				;

delete_stmt 	: DELETESYM FROM name opt_where opt_access		{ $$ = newNode( getPos(1), DELETESYM, $3, $4, $5, NULL );     }
				;

select_stmt 	: select_stmt UNION opt_all select_stmt         { $$ = newNode( getPos(2), UNION        , $1, $4, $3, NULL ); }
				| select_stmt INTERSECT     select_stmt         { $$ = newNode( getPos(2), INTERSECT    , $1, $3, NULL );     }
				| select_stmt SETDIFFERENCE select_stmt         { $$ = newNode( getPos(2), SETDIFFERENCE, $1, $3, NULL );     }
                | LPAR select_stmt RPAR                         { $$ = $2;                                                    }
				| simple_select
				;

/* NB - the grammar allows more into-clauses when using union
   and also in insert...select and exist (select...
   it also allows orderby for all selects in a union,intersect..
   these errors should be checked in sqlcompiler
*/
simple_select	:
				SELECT opt_alldistinct select_list
				opt_into
				FROM from_table_list 
				opt_where
				opt_groupby
				opt_having
				opt_orderby
				opt_access
				opt_lockmode									{ $$ = newNode( getPos(1), SELECT,
																		$2,
																		$3,
																		$4,
																		newNode( getPos(5), FROM, $6, NULL ),
																		$7,
																		$8,
																		$9,
																		$10,
																		$11,
																		$12,
																		NULL );
																}
				;

opt_all			: /* eps */										{ $$ = NULL;                                     }
				| ALL											{ $$ = newNode( getPos(1), ALL, NULL );           }
				;

opt_alldistinct : /* eps */ 									{ $$ = newNode( getPos(0), ALL, NULL );	         }
				| ALL											{ $$ = newNode( getPos(1), ALL, NULL );	         }
				| DISTINCT										{ $$ = newNode( getPos(1), DISTINCT, NULL );      }
				;

select_list		: select_list COMMA select_elem 				{ $$ = newNode( getPos(2), COMMA, $1, $3, NULL ); }
				| select_elem
				;

select_elem 	: STAR											{ $$ = newNode( getPos(1), STAR, NULL );          }
				| name DOT STAR 								{ $$ = newNode( getPos(2), DOT,
																		$1,
																		newNode( getPos(3), STAR, NULL ),
																		NULL );
																}
				| expr
				;

opt_into		: /* eps */ 									{ $$ = newNode( getPos(0), INTO,NULL );           }
				| INTO hostvar_list 							{ $$ = newNode( getPos(1), INTO, $2, NULL );      }
				;

hostvar_list	: hostvar_list COMMA hostindi_var				{ $$ = newNode( getPos(2), COMMA, $1, $3, NULL ); }
				| hostindi_var
				;

/* we need only the hostvar, not the indicator in the syntax_tree, but in the grammar the indicator has to be there */
hostindi_var	: hostvar opt_indicator 						{ $$ = $1; } /*  newNode( HOSTVAR, $1, $2, NULL ); } */
				;
				
opt_indicator	:
				/* eps */										/*{ $$ = newNode( getPos(0), INDICATOR, NULL );	   }*/
				| hostvar										/*{ $$ = newNode( getPos(1), INDICATOR, $1, NULL ); }*/
				| INDICATOR hostvar 							/*{ $$ = newNode( getPos(1), INDICATOR, $2, NULL ); }*/
				;

from_table_list : from_table_list COMMA from_table				{ $$ = newNode( getPos(2), COMMA, $1, $3, NULL ); }
				| from_table
				;

from_table		: name
				| name name 									{ $$ = newNode( getPos(1), DOT, $1, $2, NULL );   }
				| LPAR from_table opt_join_type JOIN from_table ON search_condition RPAR
/* paranteses around search_condition to avoid shift/reduce conflict */
																{ $$ = newNode( getPos(2), JOIN, $2, $3, $5, $7, NULL );
																}
				;

opt_join_type	: /* eps */ 									{ $$ = newNode( getPos(0), INNER, NULL ); 	     }
				| INNER 										{ $$ = newNode( getPos(1), INNER, NULL ); 	     }
				| LEFT											{ $$ = newNode( getPos(1), LEFT , NULL ); 	     }
				;

opt_where		: /* eps */ 									{ $$ = newNode( getPos(0), WHERE, NULL ); 	     }
				| WHERE search_condition						{ $$ = newNode( getPos(1), WHERE, $2, NULL );     }
				;

opt_having		: /* eps */ 									{ $$ = newNode( getPos(0), HAVING, NULL );	     }
				| HAVING search_condition						{ $$ = newNode( getPos(1), HAVING, $2, NULL );    }
				;

opt_groupby		: /* eps */ 									{ $$ = newNode( getPos(0), GROUP, NULL ); 	     }
				| GROUP BY expr_list        					{ $$ = newNode( getPos(1), GROUP, $3, NULL );     }
				;

opt_orderby		: /* eps */ 									{ $$ = newNode( getPos(0), ORDER, NULL ); 	     }
				| ORDER BY order_elem_list						{ $$ = newNode( getPos(1), ORDER, $3, NULL );     }
				;

order_elem_list	: order_elem_list COMMA order_elem				{ $$ = newNode( getPos(2), COMMA, $1, $3, NULL ); }
				| order_elem
				;

order_elem		: expr opt_ascdesc								{ $$ = newNode( getPos(1), BY, $1, $2, NULL );    }
				;

opt_ascdesc		: /* eps */ 									{ $$ = newNode( getPos(0), ASCENDING , NULL );    }
				| ASCENDING 									{ $$ = newNode( getPos(1), ASCENDING , NULL );    }
				| DESCENDING									{ $$ = newNode( getPos(1), DESCENDING, NULL );    }
				;

opt_access		: /* eps */ 									{ $$ = newNode( getPos(0), ACCESSS, NULL	 );      }
				| FOR  access_mode ACCESSS						{ $$ = newNode( getPos(2), ACCESSS, $2 , NULL );   }
				|	   access_mode ACCESSS						{ $$ = newNode( getPos(1), ACCESSS, $1 , NULL );   }
				;

access_mode 	: BROWSE										{ $$ = newNode( getPos(1), BROWSE    , NULL );    }
				| STABLE										{ $$ = newNode( getPos(1), STABLE    , NULL );    }
				| REPEATABLE									{ $$ = newNode( getPos(1), REPEATABLE, NULL );    }
				;

opt_lockmode	: /* eps */ 									{ $$ = newNode ( getPos(0), MODE, NULL	 );      }
				| INSYM lockmode MODE							{ $$ = newNode ( getPos(2), MODE, $2, NULL );     }
				|		lockmode MODE							{ $$ = newNode ( getPos(1), MODE, $1, NULL );     }
				;

lockmode		: SHARE 										{ $$ = newNode( getPos(1), SHARE	, NULL );        }
				| EXCLUSIVE 									{ $$ = newNode( getPos(1), EXCLUSIVE , NULL );    }
				;

search_condition: search_condition OR  search_condition			{ $$ = newNode( getPos(2), OR , $1, $3, NULL);    }
				| search_condition AND search_condition			{ $$ = newNode( getPos(2), AND, $1, $3, NULL);    }
				| NOT boolean_primary							{ $$ = newNode( getPos(1), NOT, $2, NULL );	     }
				| boolean_primary
				;

boolean_primary : predicate 	 
				| LPAR search_condition RPAR					{ $$ = $2; }
				;

predicate		: par_expr_list rel_op par_expr_list			{ $$ = newNode( getPos(2), $2->token(), $1, $3, NULL );
																}
				| EXISTS LPAR select_stmt RPAR					{ $$ = newNode( getPos(1), EXISTS, $3, NULL );
																}
				| par_expr_list BETWEEN par_expr_list AND par_expr_list
																{ $$ = newNode( getPos(2), BETWEEN, $1, $3, $5, NULL );
																}
				| par_expr_list NOT BETWEEN par_expr_list AND par_expr_list
																{ $$ = newNode( getPos(2), NOT,
																		newNode( getPos(3), BETWEEN, $1, $4, $6, NULL ),
																		NULL );
																}
/* actually only 1 single expr. but this gives shift/reduce-conflicts. We check it in the compiler */
				| par_expr_list INSYM LPAR set_expr RPAR		{ $$ = newNode( getPos(2), INSYM, $1, $4, NULL ); }

				| par_expr_list NOT INSYM LPAR set_expr RPAR	{ $$ = newNode( getPos(2), NOT,
																		newNode( getPos(3), INSYM, $1, $5, NULL ),
																		NULL );
																}
/* actually only 1 single expr. but this gives shift/reduce-conflicts. We check it in the compiler */
				| par_expr_list LIKE expr						{ $$ = newNode( getPos(2), LIKE, $1, $3, NULL );  }
				| par_expr_list NOT LIKE expr					{ $$ = newNode( getPos(2), NOT,
																		newNode( getPos(3), LIKE, $1, $4, NULL),
																		NULL );
																}
/* actually only 1 single expr. but this gives shift/reduce-conflicts. We check it in the compiler */
				| par_expr_list IS NULLVAL			            { $$ = newNode( getPos(2), ISNULL, $1, NULL );    }
				| par_expr_list IS NOT NULLVAL					{ $$ = newNode( getPos(3), NOT,
																		newNode( getPos(2), ISNULL, $1, NULL ),
																		NULL );
																}
				;

set_expr		: select_stmt
				| expr_list
				;

par_expr_list	: expr_list
//    			| LPAR expr_list RPAR 							{ $$ = $2; }
// this production gives shift/reduce conflict
				;

rel_op			: RELOPLE										{ $$ = newNode(getPos(1), RELOPLE,NULL);				}
				| RELOPLT										{ $$ = newNode(getPos(1), RELOPLT,NULL);				}
				| RELOPGE										{ $$ = newNode(getPos(1), RELOPGE,NULL);				}
				| RELOPGT										{ $$ = newNode(getPos(1), RELOPGT,NULL);				}
				| EQUAL 										{ $$ = newNode(getPos(1), EQUAL  ,NULL);				}
				| NOTEQ 										{ $$ = newNode(getPos(1), NOTEQ  ,NULL);				}
				;

expr_list		: expr_list COMMA expr							{ $$ = newNode( getPos(2), COMMA, $1, $3, NULL );	}
				| expr					 
				;

expr			: expr PLUS  expr								{ $$ = newNode( getPos(2), PLUS  , $1, $3, NULL );	}
				| expr MINUS expr								{ $$ = newNode( getPos(2), MINUS , $1, $3, NULL );	}
				| expr CONCAT expr								{ $$ = newNode( getPos(2), CONCAT, $1, $3, NULL);	}
/* need to be here to avoid shift/reduce-conflicts */
				| expr STAR  expr	 							{ $$ = newNode( getPos(2), MULT  , $1, $3, NULL );	}
				| expr DIVOP expr 								{ $$ = newNode( getPos(2), DIVOP , $1, $3, NULL );	}
				| expr MODOP expr 								{ $$ = newNode( getPos(2), MODOP , $1, $3, NULL );	}     			
				| expr EXPO expr								{ $$ = newNode( getPos(2), EXPO  , $1, $3, NULL );	}
				| MINUS unary									{ $$ = newNode( getPos(1), MINUS , $2, NULL );		}
				| PLUS	unary									{ $$ = $2; }
				| unary
				;

unary			: number_const
                | date_const
                | time_const
                | timestamp_const
				| hostindi_var
				| tab_col
				| param
				| string_const
                | CURRENTDATE                                   { $$ = newNode( getPos(1), CURRENTDATE, NULL );      }
                | CURRENTTIME                                   { $$ = newNode( getPos(1), CURRENTTIME, NULL );      }
                | CURRENTTIMESTAMP                              { $$ = newNode( getPos(1), CURRENTTIMESTAMP, NULL);  }
				| NULLVAL										{ $$ = newNode( getPos(1), NULLVAL, NULL );		    }
				| LPAR expr RPAR								{ $$ = $2;                                          }
                | CAST LPAR expr AS type_def RPAR               { $$ = newNode( getPos(1), CAST, $3, $5, NULL );     }
                | LPAR select_stmt RPAR                         { $$ = $2;                                          }
				| SUBSTRING LPAR expr FROM  expr FOR expr RPAR	{ $$ = newNode(  getPos(1), SUBSTRING, $3, $5, $7, NULL ); }
				| SUBSTRING LPAR expr COMMA expr COMMA expr RPAR {$$ = newNode(  getPos(1), SUBSTRING, $3, $5, $7, NULL ); }
                | TYPEDATE  LPAR expr COMMA expr COMMA expr RPAR { $$ = newNode( getPos(1), TYPEDATE , $3, $5, $7, NULL ); }
                | TYPETIME  LPAR expr COMMA expr COMMA expr RPAR { $$ = newNode( getPos(1), TYPETIME , $3, $5, $7, NULL ); }
                | TYPETIMESTAMP LPAR expr COMMA expr COMMA 
                                     expr COMMA expr COMMA 
                                     expr COMMA expr RPAR       { $$ = newNode( getPos(1), TYPETIMESTAMP, $3, $5, $7, $9, $11, $13, NULL );
                                                                }
				| COUNT LPAR count_arg RPAR						{ $$ = newNode( getPos(1), COUNT, $3, NULL); }
				| min_max_sum LPAR opt_alldistinct expr RPAR	{ $$ = newNode( getPos(1), $1->token(), $3, $4, NULL ); }
				;

min_max_sum 	: MIN											{ $$ = newNode(getPos(1), MIN,NULL); }
				| MAX											{ $$ = newNode(getPos(1), MAX,NULL); }
				| SUM											{ $$ = newNode(getPos(1), SUM,NULL); }
				;

count_arg		: opt_alldistinct expr							{ $$ = newNode(getPos(1), $1->token(), $2, NULL );    }
				| STAR											{ $$ = newNode(getPos(1), STAR,NULL);				 }
				;

tab_col 		: name					 
				| name DOT name 								{ $$ = newNode ( getPos(2), DOT, $1, $3, NULL );      }
				;

declare_stmt	: DECLARE name CURSOR FOR select_stmt			{ $$ = newNode ( getPos(1), DECLARE, $2, $5, NULL );  }
				;

fetch_stmt      : FETCH name INTO hostvar_list                  { $$ = newNode( getPos(1), FETCH, $2, $4, NULL );     }
                ;

create_stmt 	: CREATE TABLE name LIKE name					{ $$ = newNode( getPos(1), CREATE,
																		newNode(getPos(2), TABLE,NULL),
																		newNode(getPos(4), LIKE,NULL),
																		$3,
																		$5,
																		NULL );
																}
				| CREATE TABLE name LPAR col_def_list COMMA
									  PRIMARY KEY LPAR keyfield_list RPAR
									RPAR
																{ $$ = newNode( getPos(1), CREATE,
																		newNode(getPos(2), TABLE,NULL),
																		newNode(getPos(7), KEY, $10, NULL ),
																		$3,
																		$5,
																		NULL );
																}
				| CREATE opt_unique INDEX name ON name LPAR keyfield_list RPAR
																{ $$ = newNode( getPos(1), CREATE,
																		newNode(getPos(3), INDEX,NULL),
																		$2,
																		$4,
																		$6,
																		$8,
																		NULL );
																}
				;

opt_unique		: /* eps */ 									{ $$ = newNode( getPos(0), NOT,
																		newNode(getPos(0), UNIQUE,NULL),
																		NULL);
																}
				| UNIQUE										{ $$ = newNode( getPos(1), UNIQUE,NULL);
																}
				;

keyfield_list	: keyfield_list COMMA keyfield					{ $$ = newNode( getPos(2), COMMA, $1, $3, NULL );       }
				| keyfield
				;

keyfield		: name opt_ascdesc								{ $$ = newNode( getPos(1), COLUMN, $1, $2, NULL );      }
				;

col_def_list	: col_def_list COMMA col_def					{ $$ = newNode( getPos(2), COMMA, $1, $3, NULL );       }
				| col_def				 
				;

col_def 		: name type_def opt_not_null opt_defaultvalue	{ $$ = newNode( getPos(1), COLUMN, $1, $2, $3, $4, NULL ); }
				;

type_def		: opt_sign TYPECHAR 							{ $$ = newNode(getPos(2), TYPECHAR	   , $1,     NULL);  }
                | opt_sign TYPEWCHAR 							{ $$ = newNode(getPos(2), TYPEWCHAR	   , $1,     NULL);  }
				| opt_sign TYPECHAR LPAR number_const RPAR		{ $$ = newNode(getPos(2), TYPECHAR	   , $1, $4, NULL);  }
				| opt_sign TYPEWCHAR LPAR number_const RPAR		{ $$ = newNode(getPos(2), TYPEWCHAR	   , $1, $4, NULL);  }
				| opt_sign TYPESHORT							{ $$ = newNode(getPos(2), TYPESHORT	   , $1,     NULL);  }
				| opt_sign TYPEINT								{ $$ = newNode(getPos(2), TYPEINT 	   , $1,     NULL);  }
				| opt_sign TYPELONG 							{ $$ = newNode(getPos(2), TYPELONG	   , $1,     NULL);  }
				| TYPEFLOAT 									{ $$ = newNode(getPos(1), TYPEFLOAT	   ,         NULL);  }
				| TYPEDOUBLE									{ $$ = newNode(getPos(1), TYPEDOUBLE   ,         NULL);  }
				| TYPEDATE   									{ $$ = newNode(getPos(1), TYPEDATE	   ,         NULL);  }
				| TYPETIME   									{ $$ = newNode(getPos(1), TYPETIME	   ,         NULL);  }
				| TYPETIMESTAMP 								{ $$ = newNode(getPos(1), TYPETIMESTAMP,         NULL);  }
				| TYPEVARCHAR LPAR number_const RPAR			{ $$ = newNode(getPos(1), TYPEVARCHAR  , $3,     NULL);	 }
				;

opt_sign		: /* eps */ 									{ $$ = newNode(getPos(0), SIGNED,NULL);	                 }
				| SIGNED										{ $$ = newNode(getPos(1), SIGNED,NULL);	                 }
				| UNSIGNED										{ $$ = newNode(getPos(1), UNSIGNED,NULL);                }
				;

opt_not_null	: /* eps */ 									{ $$ = newNode(getPos(0), NULLVAL,NULL);                 } /* default null allowed */
				| NOT NULLVAL									{ $$ = newNode(getPos(1), NOT,
																		newNode(getPos(2), NULLVAL,NULL),
																		NULL );
																}
				;

opt_defaultvalue: /* eps */ 									{ $$ = newNode(getPos(0), DEFAULT, NULL);                }
				| DEFAULT constant								{ $$ = newNode(getPos(1), DEFAULT, $2, NULL);            }
				;

constant		: string_const
				| number_const
				| PLUS number_const								{ $$ = $2;                                              }
				| MINUS number_const							{ $$ = newNode( getPos(1), NUMBER, -$2->number());       }
                | date_const
                | time_const
                | timestamp_const
				;

drop_stmt		: DROP TABLE name								{ $$ = newNode( getPos(1), DROP,
																		newNode(getPos(2), TABLE	, NULL),
																		$3, NULL );
																}
				| DROP INDEX name								{ $$ = newNode( getPos(1), DROP,
																		newNode(getPos(2), INDEX	, NULL),
																		$3, NULL );
																}
				;

explain_stmt	: EXPLAIN stmt									{ $$ = newNode( getPos(1), EXPLAIN, $2, NULL	);           }
				;

name			: NAME											{ $$ = newNode( getPos(1), NAME, getText());               }
				;

hostvar 		: HOSTVAR										{ $$ = newNode( getPos(1), HOSTVAR, getText());            }
				;

param			: PARAM 										{ $$ = newNode( getPos(1), PARAM, getText());              }
				;

number_const	: NUMBER										{ $$ = newNode( getPos(1), NUMBER, _ttof(getText()));      }
				;

string_const	: STRING										{ $$ = newNode( getPos(1), STRING, getText());             }
				;

date_const	    : DATECONST										{ $$ = newNode( getPos(1), DATECONST, ttoDate(getText())); }
                ;

time_const	    : TIMECONST										{ $$ = newNode( getPos(1), TIMECONST, ttoTime(getText())); }
                ;

timestamp_const : TIMESTAMPCONST								{ $$ = newNode( getPos(1), TIMESTAMPCONST, ttoTimestamp(getText())); }
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
