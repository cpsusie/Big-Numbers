// testregexold.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <MyUtil.h>
#include <Regex.h>


/* Indexed by a character, gives the upper case equivalent of the character */

static unsigned char upcase[0400] =
  { 000, 001, 002, 003, 004, 005, 006, 007,
    010, 011, 012, 013, 014, 015, 016, 017,
    020, 021, 022, 023, 024, 025, 026, 027,
    030, 031, 032, 033, 034, 035, 036, 037,
    040, 041, 042, 043, 044, 045, 046, 047,
    050, 051, 052, 053, 054, 055, 056, 057,
    060, 061, 062, 063, 064, 065, 066, 067,
    070, 071, 072, 073, 074, 075, 076, 077,
    0100, 0101, 0102, 0103, 0104, 0105, 0106, 0107,
    0110, 0111, 0112, 0113, 0114, 0115, 0116, 0117,
    0120, 0121, 0122, 0123, 0124, 0125, 0126, 0127,
    0130, 0131, 0132, 0133, 0134, 0135, 0136, 0137,
    0140, 0101, 0102, 0103, 0104, 0105, 0106, 0107,
    0110, 0111, 0112, 0113, 0114, 0115, 0116, 0117,
    0120, 0121, 0122, 0123, 0124, 0125, 0126, 0127,
    0130, 0131, 0132, 0173, 0174, 0175, 0176, 0177,
    0200, 0201, 0202, 0203, 0204, 0205, 0206, 0207,
    0210, 0211, 0212, 0213, 0214, 0215, 0216, 0217,
    0220, 0221, 0222, 0223, 0224, 0225, 0226, 0227,
    0230, 0231, 0232, 0233, 0234, 0235, 0236, 0237,
    0240, 0241, 0242, 0243, 0244, 0245, 0246, 0247,
    0250, 0251, 0252, 0253, 0254, 0255, 0256, 0257,
    0260, 0261, 0262, 0263, 0264, 0265, 0266, 0267,
    0270, 0271, 0272, 0273, 0274, 0275, 0276, 0277,
    0300, 0301, 0302, 0303, 0304, 0305, 0306, 0307,
    0310, 0311, 0312, 0313, 0314, 0315, 0316, 0317,
    0320, 0321, 0322, 0323, 0324, 0325, 0326, 0327,
    0330, 0331, 0332, 0333, 0334, 0335, 0336, 0337,
    0340, 0341, 0342, 0343, 0344, 0345, 0346, 0347,
    0350, 0351, 0352, 0353, 0354, 0355, 0356, 0357,
    0360, 0361, 0362, 0363, 0364, 0365, 0366, 0367,
    0370, 0371, 0372, 0373, 0374, 0375, 0376, 0377
  };


#ifdef NOTDEF
static void print_buf ( struct re_pattern_buffer *bufp ) {
  int i;

  printf ("buf is :\n----------------\n");
  for (i = 0; i < bufp->used; i++)
    printchar (bufp->buffer[i]);

  printf ("\n%d allocated, %d used.\n", bufp->allocated, bufp->used);

  printf ("Allowed by fastmap: ");
  for (i = 0; i < (1 << BYTEWIDTH); i++)
    if (bufp->fastmap[i])
      printchar (i);
  printf ("\nAllowed by translate: ");
  if (bufp->translate)
    for (i = 0; i < (1 << BYTEWIDTH); i++)
      if (bufp->translate[i])
   printchar (i);
  printf ("\nfastmap is%s accurate\n", bufp->fastmap_accurate ? "" : "n't");
  printf ("can %s be null\n----------", bufp->can_be_null ? "" : "not");
}
#endif

static void printchar (char c) {
  if (c < 041 || c >= 0177) {
      putchar ('\\');
      putchar (((c >> 6) & 3) + '0');
      putchar (((c >> 3) & 7) + '0');
      putchar ((c & 7) + '0');
    }
  else
    putchar (c);
}

/*
int main () {
  char pat[80];
  struct re_pattern_buffer buf;
  int i;
  char fastmap[(1 << BYTEWIDTH)];

  buf.allocated = 40;
  buf.buffer = MALLOC(char,buf.allocated);
  buf.fastmap = fastmap;
  buf.translate = upcase;

  printf("enter pattern:"); gets (pat);
  if(!*pat) exit(0);
  re_compile_pattern (pat, strlen(pat), &buf);
  for (i = 0; i < buf.used; i++)
    printchar (buf.buffer[i]);
  putchar ('\n');

  printf ("%d allocated, %d used.\n", buf.allocated, buf.used);
  re_compile_fastmap (&buf);
  printf ("Allowed by fastmap: ");
  for (i = 0; i < (1 << BYTEWIDTH); i++)
    if (fastmap[i]) printchar (i);
  putchar ('\n');

  for(;;) {
    printf("Enter String:"); gets (pat);

    i = re_match (&buf, pat, strlen (pat), 0, 0);
    printf ("Match value %d.\n", i);
  }
}
*/

static void testregex(char *pattern, char *target, int expected, bool expectexception = false) {
  Regex re_pat;
  try {
    re_pat.compile(pattern);
  } catch(Exception e) {
    if(!expectexception) 
      printf("error:pattern:<%s> return Exception:<%s>. No Exception expected\n",pattern,e.what());
    return;
  }
  if(expectexception) {
    printf("error:pattern:<%s> expected Exception. No Exception returned\n",pattern);
    return;
  }
  int ret;
  if((ret=re_pat.exec(target)) != expected)
    printf("error:pattern:<%s> target:<%s> return:%d expected:%d\n",pattern,target,ret,expected);
}

void testregex() {
  testregex("j","rrj",2);
  testregex("j","rr",-1);
  testregex("j.","rrj",-1);
  testregex("j.","rrjq",2);
  testregex("j.r","rrjeq",-1);
  testregex("j.r","rrjer",2);
  testregex("j.r","rrjeer",-1);
  testregex("j.*","rrj",2);
  testregex("j.*r","rrj",-1);
  testregex("j.*r","rrjr",2);
  testregex("j.*r","rrjeer",2);
  testregex("j.+","rrj",-1);
  testregex("j.+","rrjr",2);
  testregex("j.+r","rrjr",-1);
  testregex("j.+r","rrjer",2);
  testregex("j.+r","rrjeer",2);
  testregex("j.?r","rrje",-1);
  testregex("j.?r","rrjr",2);
  testregex("j.?r","rrjer",2);
  testregex("j.?r","rrjeer",-1);
  testregex("jr?","rrj",2);
  testregex("jr?","rrjr",2);
  testregex("jr?","rrjrr",2);
  testregex("jr?e","rrje",2);
  testregex("jr?e","rrjre",2);
  testregex("jr?e","rrjrre",-1);
  testregex("j[rt]","rrje",-1);
  testregex("j[rt]","rrjr",2);
  testregex("j[rt]*","rrj",2);
  testregex("j[rt]+","rrjt",2);
  testregex("j[rt]+","rrj",-1);
  testregex("j[^t]","rrjt",-1);
  testregex("j[^t]","rrjr",2);
  testregex("j\\(rt\\|re\\)r","rrjrr",-1);
  testregex("j\\(rt\\|re\\)r","rrjrtr",2);
  testregex("j\\(rt\\|re\\)r","rrjrer",2);
  testregex("j\\(rt\\|re\\)+r","rrjrerertr",2);
  testregex("j\\(rt\\|re\\)+r","rrjr",-1);
  testregex("j\\(rt\\|re\\)*r","rrjr",2);
  testregex("j\\(rt\\|re\\)?r","rrjr",2);
  testregex("je$","qjer",-1);
  testregex("je$","qje",1);
  testregex("^je","qje",-1);
  testregex("^je","jeq",0);
  testregex("^je$","jeq",-1);
  testregex("^je$","je",0);
  testregex("j\\w","rrj ",-1);
  testregex("j\\w","rrje",2);
  testregex("j\\W","rrj ",2);
  testregex("j\\W","rrje",-1);
  testregex("\\<je","rrje",-1);
  testregex("\\<je"," je",1);
  testregex("\\<je\\>"," je",1);
  testregex("\\<je\\>"," je ",1);
  testregex("\\<je\\>"," jer",-1);
  testregex("\\bje\\b"," jer",-1);
  testregex("\\bje\\b"," je r",1);
  testregex("\\Bje\\B"," jer", -1);
  testregex("\\Bje\\B"," je r",-1);
  testregex("\\Bje\\B"," rjer", 2);
  testregex("je\\","jeq",0,true);
  testregex("je\\(","jeq",0,true);
  testregex("je\\)","jeq",0,true);
  testregex("je[","jeq",0,true);
  testregex("je]","je]",0);
}

main() {
//  testregex();
//  exit(0);
  char target[256];
  char pat[64];
  Regex re_pat;

  for(;;) {
    for(;;) {
      printf("Enter pattern:"); gets(pat);
      try {
        re_pat.compile(pat);
      } catch(Exception e) {
        printf("recomp(%s) : %s\n", pat, e.what());
        continue;
      }
      re_pat.dump();
    }

    for(;;) {
      printf("Enter target ('!'=new pattern):"); gets(target);
      if(target[0] == '!') break;
      printf("re_exec(%s) : %d\n", target, re_pat.exec( target ));
    }
  }
  return 0;
}

