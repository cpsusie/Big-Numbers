%{

  /* This part goes to sqlparser.h */

#include <LrParser.h>

%}

%term STRING
%term NUMBER
%term DATECONST TIMECONST TIMESTAMPCONST
%term ALL
%term AS
%term ASCENDING
%left BETWEEN
%term BOOLFALSE
%term BOOLTRUE
%term BY
%term CAST
%term COUNT
%term CURRENTDATE CURRENTTIME CURRENTTIMESTAMP
%term DELETESYM
%term DESCENDING
%term DISTINCT
%term DISTINCTROW
%term DROP
%term EXISTS
%term FROM
%term GROUp
%term HAVING
%term IIF
%left INSYM
%term INDEX
%term INSERT
%term INNER
%term OUTER
%term INTO
%term IS
%term JOIN
%term LEFT
%term LIKE
%term NULLVAL
%term ON
%term ORDER
%term PARAM
%term PARAMETERS
%term PERCENT
%term RIGHT
%term SELECT
%term SET
%term TABLE
%term TOP
%term UPDATE
%term VALUES
%term WHERE
%term TRANSFORM
%term PIVOT
%term TYPEBIT
%term TYPETEXT
%term TYPEVALUE
%term TYPEDOUBLE
%term TYPELONG
%term TYPEDATETIME
%term SEMI COMMA
%left UNION SETDIFFERENCE							/* lowest precedence of set-operators	*/
%left INTERSECT
%left EQUAL NOTEQ RELOPLE RELOPLT RELOPGE RELOPGT	/*  (lowest precedence)					*/
%left OR                  
%left AND											/*  (highest precedence)				*/
%right NOT
%left CONCAT
%left PLUS MINUS									/*  + - (lowest precedence)				*/
%left STAR DIVOP MODOP								/*  * / %								*/
%left EXPO											/*  **									*/
%left LPAR RPAR										/*  ( ) (highest precedence)			*/
%left DOT EXCLAMATION
%nonassoc NAME

%{

#include "stdafx.h"
#include "SqlParser.h"
%}

%%

start					: stmt_list
						;

stmt_list				: stmt_list stmt
						| stmt
						;

stmt					: update_stmt SEMI
						| insert_stmt 
						| delete_stmt SEMI
						| select_stmt 
						| transform_stmt SEMI
						| drop_stmt SEMI
						| parameter_stmt SEMI
						;

parameter_stmt			: PARAMETERS parameter_list
						;

parameter_list			: parameter_list COMMA parameter
						| parameter
						;

parameter				: name type
						;

type					: TYPEDATETIME
						| TYPELONG
						| TYPETEXT
						| TYPETEXT LPAR number_const RPAR
						| TYPEBIT
						| TYPEDOUBLE
						| TYPEVALUE
						;

update_stmt 			: UPDATE from_table_list SET assign_list opt_where
						;

assign_list 			: assign_list COMMA assign
						| assign
						;

assign					: tab_col EQUAL expr
						;


insert_stmt 			: INSERT INTO name LPAR insert_elem_list RPAR VALUES LPAR opt_expr_list RPAR SEMI
						| INSERT INTO name VALUES LPAR opt_expr_list RPAR SEMI
						| INSERT INTO name LPAR insert_elem_list RPAR LPAR select_stmt RPAR SEMI
						| INSERT INTO name LPAR select_stmt RPAR SEMI
						| INSERT INTO name LPAR insert_elem_list RPAR select_stmt
						| INSERT INTO name select_stmt
						;

opt_expr_list			: /* eps */
						| expr_list
						;

delete_stmt 			: DELETESYM opt_alldistinct select_list FROM name opt_where
						;

select_stmt 			: select_stmt UNION opt_all select_stmt
						| select_stmt INTERSECT 	select_stmt
						| select_stmt SETDIFFERENCE select_stmt
						| simple_select opt_semi
						;

simple_select			: SELECT 
							opt_alldistinct 
							opt_select_list
							opt_from
							opt_where
							opt_groupby
							opt_having
							opt_orderby
						;

transform_stmt			: TRANSFORM select_list select_stmt PIVOT expr opt_pivotlist
						;

opt_pivotlist			: /* eps */
						| INSYM LPAR const_list RPAR
						;

opt_all 				: /* eps */
						| ALL
						;

opt_alldistinct			: /* eps */
						| ALL
						| DISTINCT
						| DISTINCTROW
						| TOP number_const
						| TOP number_const PERCENT
						;

opt_select_list			: /* eps */
						| select_list
						;

select_list 			: select_list COMMA select_elem
						| select_elem
						;

select_elem 			: STAR
						| search_condition opt_alias
						;

opt_alias				: /* eps */
						| AS name
						| AS number_const
						;

opt_from				: /* eps */
						| FROM from_table_list
						;

from_table_list			: from_table_list COMMA from_table
						| from_table
						;

from_table				: name
						| name AS name
						| name name
						| number_const
						| from_table opt_join_type JOIN from_table ON search_condition
						| LPAR from_table opt_join_type JOIN from_table ON search_condition RPAR
						;

opt_join_type			: /* eps */
						| OUTER
						| INNER
						| RIGHT
						| LEFT
						| LEFT INNER
						| LEFT OUTER
						| RIGHT INNER
						| RIGHT OUTER
						;

opt_where				: /* eps */
						| WHERE search_condition
						;

opt_having				: /* eps */
						| HAVING search_condition
						;

opt_groupby 			: /* eps */
						| GROUp BY groupby_list
						;

groupby_list			: groupby_list COMMA group_elem
						| group_elem
						;

group_elem				: search_condition
						;

opt_orderby 			: /* eps */
						| ORDER BY order_elem_list
						;

order_elem_list			: order_elem_list COMMA order_elem
						| order_elem
						;

order_elem				: expr opt_ascdesc
						;

opt_ascdesc 			: /* eps */
						| ASCENDING
						| DESCENDING
						;

search_condition		: search_condition OR  search_condition
						| search_condition AND search_condition
						| NOT search_condition
						| EXISTS LPAR select_stmt RPAR
						| between_condition
						| expr INSYM LPAR set_expr RPAR
						| expr NOT INSYM LPAR set_expr RPAR
						| expr LIKE expr
						| expr NOT LIKE expr
						| expr IS NULLVAL
						| expr IS NOT NULLVAL
						| bool_expr
						;

between_condition		: expr     BETWEEN expr AND expr
						| expr NOT BETWEEN expr AND expr
						;

bool_expr				: bool_expr RELOPLT bool_expr
						| bool_expr RELOPLE bool_expr
						| bool_expr RELOPGT bool_expr
						| bool_expr RELOPGE bool_expr
						| bool_expr EQUAL	bool_expr
						| bool_expr NOTEQ	bool_expr
						| expr
						;

expr					: expr PLUS    expr
						| expr MINUS   expr
						| expr CONCAT  expr
						| expr STAR    expr
						| expr DIVOP   expr
						| expr MODOP   expr
						| expr EXPO    expr
						| MINUS unary
						| PLUS	unary
						| unary
						;

unary					: string_const
						| date_const
						| time_const
						| timestamp_const
						| bool_const
						| tab_col
						| CURRENTDATE
						| CURRENTTIME
						| CURRENTTIMESTAMP
						| NULLVAL
						| LPAR search_condition RPAR
						| CAST LPAR expr AS type RPAR
						| LPAR select_stmt RPAR
						| COUNT LPAR count_arg RPAR
						| funcname LPAR opt_funcparameter_list RPAR
						| iif_expression
						;

iif_expression			: IIF LPAR search_condition COMMA search_condition opt_falsepart RPAR
						;

opt_funcparameter_list	: funcparameter
						| opt_funcparameter_list COMMA funcparameter
						;

funcparameter			: /* eps */
						| expr
						;

opt_falsepart			: /* eps */
						| COMMA search_condition
						;

set_expr				: select_stmt
						| set_expr_list
						;

set_expr_list			: expr_list
						;

opt_semi				: /* eps */
						| SEMI
						;

expr_list				: expr_list COMMA expr
						| expr					 
						;

const_list				: const_list COMMA constsymbol
						| constsymbol
						;

funcname				: name
						| LEFT
						| RIGHT
						;

count_arg				: opt_alldistinct expr
						| STAR
						;

tab_col 				: tab_col DOT name
						| tab_col DOT STAR
						| tab_col EXCLAMATION name
						| name
						| PARAM
						| number_const
						;

insert_elem_list		: insert_elem
						| insert_elem_list COMMA insert_elem
						;

insert_elem 			: tab_col
						;

drop_stmt				: DROP TABLE name
						| DROP INDEX name
						;

constsymbol 			: number_const
						| MINUS number_const
						| string_const
						| date_const
						| time_const
						| timestamp_const
						| bool_const
						;

name					: NAME
						;

number_const			: NUMBER
						;

string_const			: STRING
						;

date_const				: DATECONST
						;

time_const				: TIMECONST
						;

timestamp_const			: TIMESTAMPCONST
						;

bool_const				: BOOLTRUE
						| BOOLFALSE
						;

%%
