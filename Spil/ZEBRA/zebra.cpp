#include "stdafx.h"
#include <stdio.h>

/*
   Programmet løser følgende opgave ved at gennemløbe samtlige
   placeringer af personer, farver, husdyr, drikke og cigaretter
   og checke at betingelserne er opfyldt :

  1. Der er 5 huse på række
  2. Englænderen bor i det røde hus.
  3. Svenskeren har hund.
  4. Der drikkes kaffe i det grønne hus.
  5. Manden der ryger Long holder fugl.
  6. Det grønne hus ligger til højre for det hvide.
  7. Danskeren drikker te.
  8. I det gule hus ryger man Kings.
  9. I det midterste hus drikker man mælk.
 10. Nordmanden bor i det første hus.
 11. Manden, der ryger Cecil bor ved siden af huset med kat.
 12. Man ryger Kings i huset ved siden af huset med hest.
 13. Manden, der ryger Northstate drikker øl.
 14. Tyskeren ryger Prince.
 15. Nordmanden bor ved siden af det blå hus.
 16. Man drikker vand i huset ved siden af huset hvor man ryger Cecil.

  Hvert hus har sin farve, hver sit husdyr hver sin drik og hver sin røg.

  Hvem holder Zebra ?.

*/

unsigned long tryCount = 0;
unsigned int solutionCount = 0;

#define swap(p1, p2) { char tmp = *p1; *p1 = *p2; *p2 = tmp; }

// Kalder f med samtlige permutationer af a.

void permuter(char *a, int nelem, void(*f)()) {
  if(nelem <= 1) {
    tryCount++;
    f();
  } else {
    nelem--;
    permuter(a, nelem, f);
    int i = 0;
    for(char *p1 = a, *p2 = a + nelem; i < nelem; i++, p1++) {
      swap(p1, p2);
      permuter(a, nelem, f);
      swap(p1, p2);
    }
  }
}

/* de enkelte arrays repræsenterer husnumrene. F.eks. NORSK angiver
   hvor nordmanden bor. GUL angiver hvilket hus, der er gult osv.
*/

char nation[5];
char farve[5];
char husdyr[5];
char drik[5];
char ryger[5];

#define SVENSK     nation[0]
#define ENGELSK    nation[1]
#define DANSK      nation[2]
#define NORSK      nation[3]
#define TYSK       nation[4]

#define ROED       farve[0]
#define GROEN      farve[1]
#define HVID       farve[2]
#define GUL        farve[3]
#define BLAA       farve[4]

#define HUND       husdyr[0]
#define FUGL       husdyr[1]
#define KAT        husdyr[2]
#define HEST       husdyr[3]
#define ZEBRA      husdyr[4]

#define KAFFE      drik[0]
#define TE         drik[1]
#define MAELK      drik[2]
#define OEL        drik[3]
#define VAND       drik[4]

#define LONGS      ryger[0]
#define KINGS      ryger[1]
#define CECIL      ryger[2]
#define NORDSTATE  ryger[3]
#define PRINCE     ryger[4]


char *nation_str[] = {
  "svensk      ",
  "engelsk     ",
  "dansk       ",
  "norsk       ",
  "tysk        "
};

char *farve_str[] = {
  "roed        ",
  "groen       ",
  "hvid        ",
  "gul         ",
  "blaa        "
};

char *husdyr_str[] = {
  "hund        ",
  "fugl        ",
  "kat         ",
  "hest        ",
  "zebra       "
};

char *drik_str[] = {
  "kaffe       ",
  "te          ",
  "maelk       ",
  "oel         ",
  "vand        "
};

char *ryger_str[] = {
  "longs       ",
  "kings       ",
  "cecil       ",
  "nordstate   ",
  "prince      "
};

static void listAttributes(char *a, char **str) {
  for(int i = 0; i < 5; i++) {
    for(int j = 0; j < 5; j++) {
      if(a[j] == i) {
         printf( "%s", str[j] );
         break;
      }
    }
  }
  printf( "\n" );
}

static void solution() {
  solutionCount++;
  printf("Løsning %d efter %6ld kombinationer\n", solutionCount, tryCount);

#define listProperty(a) listAttributes(a, a##_str)

  listProperty(nation);
  listProperty(farve );
  listProperty(husdyr);
  listProperty(drik  );
  listProperty(ryger );
}

void checkRyger() {

  if( LONGS != FUGL )              return;    // 5
  if( GUL != KINGS )               return;    // 8

  if( CECIL != KAT - 1 &&
      CECIL != KAT + 1 )           return;    // 11

  if( KINGS != HEST - 1 &&
      KINGS != HEST + 1 )          return;    // 12


  if( NORDSTATE != OEL )           return;    // 13

  if( PRINCE != TYSK )             return;    // 14

  if( CECIL != VAND - 1 &&
      CECIL != VAND + 1 )          return;    // 16

  solution();

}

void checkDrik() {
  if( KAFFE != GROEN )             return;    // 4
  if( TE    != DANSK )             return;    // 7
  if( MAELK != 2 )                 return;    // 9
  permuter(ryger, 5, checkRyger);
}

void checkHusdyr() {
  if( SVENSK != HUND )             return;    // 3

  permuter(drik, 5, checkDrik);
}

void checkFarve() {
  if( ENGELSK != ROED )            return;    // 2
  if( GROEN   <= HVID )            return;    // 6

  if( NORSK != BLAA - 1 &&
      NORSK != BLAA + 1 )          return;    // 15

  permuter(husdyr, 5, checkHusdyr);
}

void checkNation() {
  if( NORSK != 0 )                 return;    // 10

  permuter(farve, 5, checkFarve);
}

int main(int argc, char **argv) {
  for(int i = 0; i < 5; i++) {
    farve[i] = husdyr[i] = drik[i] = ryger[i] = nation[i] = i;
  }

  permuter(nation, 5, checkNation);

  printf("Prøvet %ld kombinationer. Fundet %d %s\n", tryCount, solutionCount, (solutionCount==1)?"løsning":"løsninger");

  return 0;
}
