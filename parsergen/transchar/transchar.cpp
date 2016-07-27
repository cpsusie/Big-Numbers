// transchar.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <String.h>
#include <MyUtil.h>
#include "transcharlex.h"

static char *translateLine(char *line) {
  char tmp[100];
  int i = 0;
  LexStringStream stream(line);
  TranscharLex lex;
  lex.newStream(&stream);
  char c;
  while(c = lex.getNextLexeme())
    tmp[i++] = c;

  tmp[i] = 0;
  strcpy(line,tmp);
  return line;
}

static void translateCharacter(char *src, char *pat, unsigned char c) {
  char *s;
  int l = strlen(pat);
  while((s = strstr(src,pat)) != NULL) {
    *s = c;
    strcpy(s+1,s+l);
  }
}

/* 
 * translate any special characters in String s. Note this is not (yet) a complete list of special characters used in HTML
 */
static char *translateCharacters(char *s) {
  /* optimization. no need to do anything if we havent got a '&' */
  if(strchr(s,'&') == NULL) return s; 
  translateCharacter(s,"&AElig;" ,146); /* Æ */
  translateCharacter(s,"&aelig;" ,145); /* æ */
  translateCharacter(s,"&Aring;" ,143); /* Å */
  translateCharacter(s,"&aring;" ,134); /* å */
  translateCharacter(s,"&Agrave;",183); /* À */
  translateCharacter(s,"&agrave;",133); /* à */
  translateCharacter(s,"&Aacute;",181); /* Á */
  translateCharacter(s,"&aacute;",160); /* á */
  translateCharacter(s,"&Auml;"  ,142); /* Ä */
  translateCharacter(s,"&auml;"  ,132); /* ä */
  translateCharacter(s,"&acirc;" ,131); /* â */
  translateCharacter(s,"&Egrave;",212); /* È */
  translateCharacter(s,"&egrave;",138); /* è */
  translateCharacter(s,"&Eacute;",144); /* É */
  translateCharacter(s,"&eacute;",130); /* é */
  translateCharacter(s,"&Euml;"  ,211); /* Ë */
  translateCharacter(s,"&euml;"  ,137); /* ë */
  translateCharacter(s,"&Ecirc;" ,210); /* Ê */
  translateCharacter(s,"&ecirc;" ,136); /* ê */
  translateCharacter(s,"&Igrave;",222); /* Ì */
  translateCharacter(s,"&igrave;",141); /* ì */
  translateCharacter(s,"&Iacute;",214); /* Í */
  translateCharacter(s,"&iacute;",161); /* í */
  translateCharacter(s,"&Iuml;"  ,216); /* Ï */
  translateCharacter(s,"&iuml;"  ,139); /* ï */
  translateCharacter(s,"&icirc;" ,140); /* î */
  translateCharacter(s,"&Ntilde;",165); /* Ñ */
  translateCharacter(s,"&ntilde;",164); /* ñ */
  translateCharacter(s,"&Oslash;",157); /* Ø */
  translateCharacter(s,"&oslash;",155); /* ø */
  translateCharacter(s,"&Ograve;",227); /* Ò */
  translateCharacter(s,"&ograve;",149); /* ò */
  translateCharacter(s,"&Oacute;",224); /* Ó */
  translateCharacter(s,"&oacute;",162); /* ó */
  translateCharacter(s,"&Ouml;"  ,153); /* Ö */
  translateCharacter(s,"&ouml;"  ,148); /* ö */
  translateCharacter(s,"&ocirc;" ,147); /* ô */
  translateCharacter(s,"&Ugrave;",235); /* Ù */
  translateCharacter(s,"&ugrave;",151); /* ù */
  translateCharacter(s,"&Uacute;",233); /* Ú */
  translateCharacter(s,"&uacute;",163); /* ú */
  translateCharacter(s,"&Uuml;"  ,154); /* Ü */
  translateCharacter(s,"&uuml;"  ,129); /* ü */
  translateCharacter(s,"&ucirc;" ,150); /* û */
  translateCharacter(s,"&Yacute;",237); /* Ý */
  translateCharacter(s,"&yacute;",236); /* ý */
  translateCharacter(s,"&szlig;" ,225); /* ss */
  translateCharacter(s,"&ccedil;",135); /* c */
  translateCharacter(s,"&Ccedil;",128); /* C */
  return s;
}

int main(int argc, char* argv[])
{
  char line[100];
  char *s;
  double start;
  int i;
  strcpy(line,"sdfgdfgkjh&aelig;srdgkljh");
  start = getProcessTime();
  for(i = 0; i < 50000; i++) {
    strcpy(line,"sdfgdfgkjh&aelig;srdgkljh");
    s = translateCharacters(line);
  }
  printf("%lf\n",getProcessTime() - start);

  start = getProcessTime();
  for(i = 0; i < 50000; i++) {
    strcpy(line,"sdfgdfgkjh&aelig;srdgkljh");
    s = translateLine(line);
  }
  printf("%lf\n",getProcessTime() - start);
  return 0;
}
