#include "stdafx.h"
#include <String.h>
#include <ProcessTools.h>
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
  size_t l = strlen(pat);
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
  translateCharacter(s,"&AElig;" ,146); /* � */
  translateCharacter(s,"&aelig;" ,145); /* � */
  translateCharacter(s,"&Aring;" ,143); /* � */
  translateCharacter(s,"&aring;" ,134); /* � */
  translateCharacter(s,"&Agrave;",183); /* � */
  translateCharacter(s,"&agrave;",133); /* � */
  translateCharacter(s,"&Aacute;",181); /* � */
  translateCharacter(s,"&aacute;",160); /* � */
  translateCharacter(s,"&Auml;"  ,142); /* � */
  translateCharacter(s,"&auml;"  ,132); /* � */
  translateCharacter(s,"&acirc;" ,131); /* � */
  translateCharacter(s,"&Egrave;",212); /* � */
  translateCharacter(s,"&egrave;",138); /* � */
  translateCharacter(s,"&Eacute;",144); /* � */
  translateCharacter(s,"&eacute;",130); /* � */
  translateCharacter(s,"&Euml;"  ,211); /* � */
  translateCharacter(s,"&euml;"  ,137); /* � */
  translateCharacter(s,"&Ecirc;" ,210); /* � */
  translateCharacter(s,"&ecirc;" ,136); /* � */
  translateCharacter(s,"&Igrave;",222); /* � */
  translateCharacter(s,"&igrave;",141); /* � */
  translateCharacter(s,"&Iacute;",214); /* � */
  translateCharacter(s,"&iacute;",161); /* � */
  translateCharacter(s,"&Iuml;"  ,216); /* � */
  translateCharacter(s,"&iuml;"  ,139); /* � */
  translateCharacter(s,"&icirc;" ,140); /* � */
  translateCharacter(s,"&Ntilde;",165); /* � */
  translateCharacter(s,"&ntilde;",164); /* � */
  translateCharacter(s,"&Oslash;",157); /* � */
  translateCharacter(s,"&oslash;",155); /* � */
  translateCharacter(s,"&Ograve;",227); /* � */
  translateCharacter(s,"&ograve;",149); /* � */
  translateCharacter(s,"&Oacute;",224); /* � */
  translateCharacter(s,"&oacute;",162); /* � */
  translateCharacter(s,"&Ouml;"  ,153); /* � */
  translateCharacter(s,"&ouml;"  ,148); /* � */
  translateCharacter(s,"&ocirc;" ,147); /* � */
  translateCharacter(s,"&Ugrave;",235); /* � */
  translateCharacter(s,"&ugrave;",151); /* � */
  translateCharacter(s,"&Uacute;",233); /* � */
  translateCharacter(s,"&uacute;",163); /* � */
  translateCharacter(s,"&Uuml;"  ,154); /* � */
  translateCharacter(s,"&uuml;"  ,129); /* � */
  translateCharacter(s,"&ucirc;" ,150); /* � */
  translateCharacter(s,"&Yacute;",237); /* � */
  translateCharacter(s,"&yacute;",236); /* � */
  translateCharacter(s,"&szlig;" ,225); /* ss */
  translateCharacter(s,"&ccedil;",135); /* c */
  translateCharacter(s,"&Ccedil;",128); /* C */
  return s;
}

int main(int argc, char **argv) {
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
