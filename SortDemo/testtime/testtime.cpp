// testtime.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <MyUtil.h>
#include <qsortnorec.h>

int cmpcounter;
int intcmp(int *i1, int *i2) {
  cmpcounter++;
  return *i1 - *i2;
}
typedef int  (__cdecl *sortcomparator)(const void*, const void*); 
typedef void (*sortmethod)(void *, size_t, size_t, sortcomparator);

void dosort(int *a, int n, sortmethod sortproc, char *name) {
  cmpcounter = 0;
  double starttime = getProcessTime();
  sortproc(a,n,sizeof(int),(sortcomparator)intcmp);
  printf("%s=(%8d %8d %.5lf) ",name,n,cmpcounter,(getProcessTime()-starttime)/10000000);
}

#define MAXN 300000
#define STEP 1000
typedef int list[MAXN];
static list a,save;
#define M_PI 3.1415926535897932384626433

typedef struct {
  char data[13];
  int key;
} somestruct;
int somestructcmp(somestruct *i1, somestruct *i2) {
  return i1->key - i2->key;
}

inline int sign(double d) {
  return d < 0 ? -1 : d > 0 ? 1 : 0;
}

int doublecmp(double *d1, double *d2) {
  return sign(*d1 - *d2);
}

#define QSORT_PERFORM_OK
int main(int argc, char* argv[]) {

#ifdef QSORT_PERFORM_OK
  for(int i = 0; i < MAXN;i++) 
      save[i] = rand();
#else
  for(int i = 0; i < MAXN; i++)
    save[i] = (int)(150 * (1.0+sin(M_PI*2*i / (MAXN-1)*(MAXN-2))));
#endif
  somestruct v[20];
  for(i = 0; i < 20; i++) {
    v[i].key = rand();
    sprintf(v[i].data,"v=%d",v[i].key);
  }
  quickSort(v,20,sizeof(v[0]),(sortcomparator)somestructcmp);
  for(i = 0; i < 20;i++) {
    printf("v[%d]={%s %d}\n",i,v[i].data,v[i].key);
  }

  double dv[20];
  for(i = 0; i < 20; i++)
    dv[i] = rand();
  quickSort(dv,20,sizeof(dv[0]),(sortcomparator)doublecmp);
  for(i = 0; i < 20;i++) {
    printf("dv[%d]=%le\n",i,dv[i]);
  }

  for(int n = MAXN-10*STEP;n < MAXN;n+=STEP) {
    memcpy(a,save,sizeof(a));
    dosort(a,n,qsort,"qsort");
    memcpy(a,save,sizeof(a));
    dosort(a,n,quickSort,"quickSort");
    printf("\n");
  }
  
  return 0;
}
