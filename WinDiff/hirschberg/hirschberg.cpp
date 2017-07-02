// hirschberg.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <MyUtil.h>
#include <Array.h>
#include <MyString.h>

void getFile(char *fname, StringArray &s) {
  FILE *f = FOPEN(fname,"r");
  char line[1000];
  while(fgets(line,sizeof(line),f))
    s.add(line);
  fclose(f);
}

int Min(int x, int y) {
  return (x < y) ? x : y;
}

class diff {
private:
  const StringArray &s1,&s2;
  int *fwd[2],*rev[2];
  void fwdDPA(int p1, int p2, int q1, int q2, int *m[2]);
  void revDPA(int p1, int p2, int q1, int q2, int *m[2]);
  void align( int p1, int p2, int q1, int q2);
public:
  diff(const StringArray &s1, const StringArray &s2);
  ~diff();
  void hirschberg();
  StringArray result;
};

diff::diff(const StringArray &_s1, const StringArray &_s2) : s1(_s1), s2(_s2) {
  int l2 = s2.size();
  for(int i = 0; i < 2; i++) {
    fwd[i] = new int[l2+1];
    rev[i] = new int[l2+1];
  }
}

diff::~diff() {
  for(int i = 0; i < 2; i++) {
    delete[] fwd[i];
    delete[] rev[i];
  }
}

void diff::fwdDPA(int p1, int p2, int q1, int q2, int *m[2]) {
  int i, j;

  m[p1%2][q1] = 0; // boundary conditions
  for(j=q1+1; j <= q2; j++)
    m[p1%2][j] = m[p1%2][j-1] + 1;

  for(i=p1+1; i <= p2; i++) {    // outer loop
    m[i%2][q1] = m[(i-1)%2][q1] + 1; // boundary conditions

    for(j=q1+1; j <= q2; j++) { // inner loop
      int diag = m[(i-1)%2][j-1];
      if(s1[i-1] != s2[j-1] )
        diag += 1;

      m[i%2][j] = Min( diag,
                     Min( m[(i-1)%2][j]+1,
                          m[i%2][j-1]+1
                     )
                  );
    }//for j
  }//for i
}//fwdDPA

void diff::revDPA( int p1, int p2, int q1, int q2, int *m[2]) {
  int i, j;

  m[p2%2][q2] = 0; // boundary conditions
  for(j=q2-1; j >= q1; j--)
    m[p2%2][j] = m[p2%2][j+1]+1;

  for(i=p2-1; i >= p1; i--) {
    m[i%2][q2] = m[(i+1)%2][q2] + 1;

    for(j=q2-1; j >= q1; j--) {
      int diag = m[(i+1)%2][j+1];
      if( s1[i] != s2[j] )
        diag += 1;

      m[i%2][j] = Min( diag,
                     Min( m[(i+1)%2][j]+1,
                          m[i%2][j+1]+1
                     )
                  );
    }//for j
  }//for i
}//revDPA

void diff::align(int p1, int p2, int q1, int q2) {
  int i;

  if( p2-0 <= p1 ) { // s1 is empty String
    for(i=q1; i < q2; i++) {
      result.add(String("<") + s2[i]);
    }
  }
  else if( q2-0 <= q1 ) { // s2 is empty String
    for(i=p1; i < p2; i++) {
      result.add(String(">") + s1[i]);
    }
  }
  else if( p2-1 == p1 ) { // s1 is one character and s2 is not empty
    const String &tmp = s1[p1];
    int   memo = q1;
    for(i=q1+1; i < q2; i++)
      if(s2[i] == tmp)
        memo=i;
    for(i=q1; i < q2; i++) {
      if(i == memo) {
        if(s2[i] != tmp) {
          result.add(String(">") + tmp);
          result.add(String("<") + s2[i]);
        }
      }
      else {
        result.add(String("<") + s2[i]);
      }
    }
  }
/*
      for(i=q1; i < q2; i++)
       { if(i == memo) {
           if(s2.charAt(i) == ch) {
             row1 += ch;
             row2 += '|';
             row3 += s2.charAt(i);
           }
           else {
             row1 += ch;
             row2 += ' ';
             row3 += s2.charAt(i);
           }
         else {
           row1 += '-';
           row2 += ' ';
           row3 += s2.charAt(i);
         }
       }
  } // a b [l=2] mid=1, a b c [l=3] mid=1, a b c d [l=4] mid=2
*/
  else { // p2 > p1 + 1, s1 has at least 2 chars,  divide s1 and conquer
    int mid = (p1+p2) / 2;
    fprintf(stderr,"split %d\n",mid);
    fwdDPA(p1, mid, q1, q2, fwd);
    revDPA(mid, p2, q1, q2, rev);
    int s2mid = q1;
    int best = -1;
    for(i = q1; i <= q2; i++) { // look for cheapest split of s2
      int sum = fwd[mid%2][i] + rev[mid%2][i];
      if( best == -1 || sum < best ) {
        best = sum;
        s2mid= i;
      }
    }
    align(p1,mid, q1,s2mid);
    align(mid,p2, s2mid,q2);
  }
}//align

void diff::hirschberg() {
  align(0,s1.size(), 0,s2.size());
}

static void usage() {
  fprintf(stderr,"Usage:hirschberg file1 file2\n");
  exit(-1);
}

int main(int argc, char **argv)
{
  try {
    StringArray s1,s2;

    argv++;
    if(!*argv) usage();
    getFile(*argv,s1);
    argv++;
    if(!*argv) usage();
    getFile(*argv,s2);
    diff d(s1,s2);
    d.hirschberg();
    for(unsigned int i = 0; i < d.result.size(); i++)
      printf("%s",d.result[i].cstr());
  } catch(Exception e) {
    fprintf(stderr,"%s\n",e.what());
    exit(-1);
  }
  return 0;
}
