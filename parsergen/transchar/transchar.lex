%{

class TranscharLex : public Scanner {
public:
  int getNextLexeme() override;
};

#define YYBADINP

%}

%{

 /* This part goes to the first part of exprlex.cpp */

#include "stdafx.h"
#include "transcharlex.h"

%}

let     [_a-zA-Z\x80-\xff]		/* Letter					*/
alnum   [_a-zA-Z0-9\x80-\xff]	/* Alphanumeric character	*/
d       [0-9]					/* Decimal digit			*/
white   [\x00-\x09\x0b\s\n]		/* White space:				*/

%%

"&AElig;"	return 146; /* � */
"&aelig;"	return 145; /* � */
"&Aring;"	return 143; /* � */
"&aring;"	return 134; /* � */
"&Agrave;"	return 183; /* � */
"&agrave;"	return 133; /* � */
"&Aacute;"	return 181; /* � */
"&aacute;"	return 160; /* � */
"&Auml;"	return 142; /* � */
"&auml;"	return 132; /* � */
"&acirc;"	return 131; /* � */
"&Egrave;"	return 212; /* � */
"&egrave;"	return 138; /* � */
"&Eacute;"	return 144; /* � */
"&eacute;"	return 130; /* � */
"&Euml;"	return 211; /* � */
"&euml;"	return 137; /* � */
"&Ecirc;"	return 210; /* � */
"&ecirc;"	return 136; /* � */
"&Igrave;"	return 222; /* � */
"&igrave;"	return 141; /* � */
"&Iacute;"	return 214; /* � */
"&iacute;"	return 161; /* � */
"&Iuml;"	return 216; /* � */
"&iuml;"	return 139; /* � */
"&icirc;"	return 140; /* � */
"&Ntilde;"	return 165; /* � */
"&ntilde;"	return 164; /* � */
"&Oslash;"	return 157; /* � */
"&oslash;"	return 155; /* � */
"&Ograve;"	return 227; /* � */
"&ograve;"	return 149; /* � */
"&Oacute;"	return 224; /* � */
"&oacute;"	return 162; /* � */
"&Ouml;"	return 153; /* � */
"&ouml;"	return 148; /* � */
"&ocirc;"	return 147; /* � */
"&Ugrave;"	return 235; /* � */
"&ugrave;"	return 151; /* � */
"&Uacute;"	return 233; /* � */
"&uacute;"	return 163; /* � */
"&Uuml;"	return 154; /* � */
"&uuml;"	return 129; /* � */
"&ucirc;"	return 150; /* � */
"&Yacute;"	return 237; /* � */
"&yacute;"	return 236; /* � */
"&szlig;"	return 225; /* ss */
"&ccedil;"	return 135; /* c */
"&Ccedil;"	return 128; /* C */
.           return getText()[0];
{white}		return getText()[0];

%%

