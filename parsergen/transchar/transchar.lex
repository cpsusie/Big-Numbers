%{

class TranscharLex : public Scanner {
public:
  int getNextLexeme();
};

#define YYBADINP

%}

%{

 /* This part goes to the first part of exprlex.cpp */

#include "stdafx.h"
#include <string.h>
#include <MyUtil.h>
#include "transcharlex.h"

%}

let     [_a-zA-Z\x80-\xff]		/* Letter					*/
alnum   [_a-zA-Z0-9\x80-\xff]	/* Alphanumeric character	*/
d       [0-9]					/* Decimal digit			*/
white   [\x00-\x09\x0b\s\n]		/* White space:				*/

%%

"&AElig;"	return 146; /* Æ */
"&aelig;"	return 145; /* æ */
"&Aring;"	return 143; /* Å */
"&aring;"	return 134; /* å */
"&Agrave;"	return 183; /* À */
"&agrave;"	return 133; /* à */
"&Aacute;"	return 181; /* Á */
"&aacute;"	return 160; /* á */
"&Auml;"	return 142; /* Ä */
"&auml;"	return 132; /* ä */
"&acirc;"	return 131; /* â */
"&Egrave;"	return 212; /* È */
"&egrave;"	return 138; /* è */
"&Eacute;"	return 144; /* É */
"&eacute;"	return 130; /* é */
"&Euml;"	return 211; /* Ë */
"&euml;"	return 137; /* ë */
"&Ecirc;"	return 210; /* Ê */
"&ecirc;"	return 136; /* ê */
"&Igrave;"	return 222; /* Ì */
"&igrave;"	return 141; /* ì */
"&Iacute;"	return 214; /* Í */
"&iacute;"	return 161; /* í */
"&Iuml;"	return 216; /* Ï */
"&iuml;"	return 139; /* ï */
"&icirc;"	return 140; /* î */
"&Ntilde;"	return 165; /* Ñ */
"&ntilde;"	return 164; /* ñ */
"&Oslash;"	return 157; /* Ø */
"&oslash;"	return 155; /* ø */
"&Ograve;"	return 227; /* Ò */
"&ograve;"	return 149; /* ò */
"&Oacute;"	return 224; /* Ó */
"&oacute;"	return 162; /* ó */
"&Ouml;"	return 153; /* Ö */
"&ouml;"	return 148; /* ö */
"&ocirc;"	return 147; /* ô */
"&Ugrave;"	return 235; /* Ù */
"&ugrave;"	return 151; /* ù */
"&Uacute;"	return 233; /* Ú */
"&uacute;"	return 163; /* ú */
"&Uuml;"	return 154; /* Ü */
"&uuml;"	return 129; /* ü */
"&ucirc;"	return 150; /* û */
"&Yacute;"	return 237; /* Ý */
"&yacute;"	return 236; /* ý */
"&szlig;"	return 225; /* ss */
"&ccedil;"	return 135; /* c */
"&Ccedil;"	return 128; /* C */
.           return getText()[0];
{white}		return getText()[0];

%%

