#include "stdafx.h"
#include <stdio.h>

/*
   Programmet løser følgende opgave ved at gennemløbe samtlige kombinationer
   af hovedstæder, hoteller og planer, og checke at betingelserne
   er opfyldt:

   1 Fem amerikanere på vej i fly fra USA til Frankfurt faldt i snak ved baren og
   fortalte hinanden om deres planer for Europarejsen. Det viste sig at de
   alle skulle skifte fly i Frankfurt og derfra flyve videre til hver sin
   europæiske hovedstad.

   1. En af dem ville starte i Bruxelles, fordi hun simpelthen ikke kunne
      vente med at se Grand Place.
   2. Mrs. Gilbert skulle skifte til SAS-flyet til København.
   3. Turisten, der skulle til London, skulle bo på Regent Palace.
   4. Den, der skulle bo på Ritz, glædede sig til at besøge venner i byen.
   5. Mr. Lundberg, der havde bestilt værelse på Grand Hotel, var en
      stilfærdig type, der ikke rejste ud for at nyde det muntre natteliv.
   6. Ingen af damernes hotel hed Sheraton.
   7. Miss Monson glædede sig specielt til at gå i teatret i sin foretrukne
      hovedstad.
   8. Turisten, der skulle til Stockholm, håbede om muligt at finde sine
      rødder. Han stammer nemlig fra Sverige.
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

/* De enkelte arrays repræsenterer nummeret på personen. hovedstad[0] er
   således personen der skal til Bruxelles, hotel[1] er personen der skal
   bo på Grandhotel. osv.
*/

char navn[5]; // NB - permuteres ikke, men holdes fast. Faktisk unødvendig
char hovedstad[5];
char hotel[5];
char planer[5];

#define GILBERT     navn[0]
#define HARDING     navn[1]
#define LUNDBERG    navn[2]
#define MONSON      navn[3]
#define SWANSON     navn[4]

#define BRUXELLES   hovedstad[0]
#define COPENHAGEN  hovedstad[1]
#define LONDON      hovedstad[2]
#define PARIS       hovedstad[3]
#define STOCKHOLM   hovedstad[4]

#define AMBASSADOR  hotel[0]
#define GRANDHOTEL  hotel[1]
#define REGENTHOTEL hotel[2]
#define RITZ        hotel[3]
#define SHERATON    hotel[4]

#define NATTELIV    planer[0]
#define ROOTS       planer[1]
#define VENNER      planer[2]
#define TEATER      planer[3]
#define GRANDPLACE  planer[4]

char *navn_str[] = {
  "Mrs. Gilbert", "Mr. Harding","Mr. Lundberg","Miss Monson","Mr. Swanson"
};
char *hovedstad_str[] = {
  "Bruxelles","København","London","Paris","Stockholm"
};
char *hotel_str[] = {
  "Ambassador","Grand Hotel","Regent Hotel","Ritz","Sheraton"
};
char *planer_str[] = {
  "Nyde nattelivet","Søge rødder","Besøge venner","Gå i teater","Se Grand Place"
};

static void listAttributes(char *a, char **str) {
  for(int i = 0; i < 5; i++) {
    for(int j = 0; j < 5; j++) {
      if(a[j] == i) {
        printf("%-16s ", str[j]);
        break;
      }
    }
  }
  printf("\n");
}

static void solution() {
  solutionCount++;
  printf("Løsning %d efter %6ld kombinationer\n", solutionCount, tryCount);

#define listProperty(a) listAttributes(a, a##_str)

  listProperty(navn     );
  listProperty(hovedstad);
  listProperty(hotel    );
  listProperty(planer   );
}

void checkplaner() {
  if(GILBERT   != BRUXELLES && HARDING != BRUXELLES && MONSON != BRUXELLES)
    return;                                   // 1
  if(BRUXELLES != GRANDPLACE )  return;       // 1
  if(GILBERT   != COPENHAGEN )  return;       // 2
  if(LONDON    != REGENTHOTEL)  return;       // 3
  if(RITZ      != VENNER     )  return;       // 4
  if(LUNDBERG  != GRANDHOTEL )  return;       // 5
  if(LUNDBERG == NATTELIV    )  return;       // 5
  if(GILBERT  == SHERATON || HARDING == SHERATON || MONSON == SHERATON)
    return;                                   // 6
  if(MONSON    != TEATER     )  return;       // 7
  if(STOCKHOLM != ROOTS      )  return;       // 8
  solution();
}

void checkHotel() {
  permuter(planer, 5, checkplaner);
}

void checkHovedstad() {
  permuter(hotel, 5, checkHotel);
}

void checkNavn() {
  permuter(hovedstad, 5, checkHovedstad);
}

int main( int argc, char **argv ) {
  for(int i = 0; i < 5; i++ ) {
    navn[i] = hovedstad[i] = hotel[i] = planer[i] = i;
  }

  checkNavn();

  printf("Prøvet %ld kombinationer. Fundet %d %s\n", tryCount, solutionCount, (solutionCount==1)?"løsning":"løsninger");

  return 0;
}
