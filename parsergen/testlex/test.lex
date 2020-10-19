%{
#pragma once

 /* This part goes to clex.h */

#include <MyUtil.h>
#include <Scanner.h>
#include "csymbol.h"

class TestLex : public Scanner {
public:
  int getNextLexeme() override;
};

%}

%{

 /* This part goes to the first part of clex.cpp */

#include "stdafx.h"
#include "testlex.h"

%}

let     [_a-zA-Zזרו]			/* Letter 								*/
alnum	[_a-zA-Z\x80-\xff0-9]	/* Alphanumeric character			    */
h       [0-9a-fA-F]			    /* Hexadecimal digit					*/
o       [0-7]				    /* Octal digit							*/
d       [0-9]				    /* Decimal digit						*/
suffix  [UuLl]				    /* Suffix in integral numeric constant	*/
white   [\s\t\^a]
strlit	\"(\\.|[^\\\"])*\"
infin	a{infin1}b
infin1	c{infin2}d
infin2	e{infin}f

%%
{let}{alnum}*	return NAME;
{strlit}$		return STRING;
^#				return 1;
{white}+		;
.				printf( "Illegal character <%c>\n", *getText());
%%
