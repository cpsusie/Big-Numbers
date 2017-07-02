// Topologisk sortering

#include "stdafx.h"
#pragma warning(disable:4786)
// Denne pragma skal være her, efter #include <stdafx> og før #include <map> !!!

#include <iostream>
#include <fstream>
#include <String>
#include <list>
#include <map>

using namespace std;

typedef list<string> stringlist;

class token {
public:
  int predcount;
  stringlist succ;
  token() { predcount = 0; }
};

typedef map<string, token, less<string> > string2tokenmap;

int main(int argc, char **argv) {
  string2tokenmap m;
  string p,s;
  ifstream ifs;
  istream *input = &cin;

  argv++;
  if(*argv) {
    ifs.open(*argv);
    if(!ifs.good()) {
      perror(*argv);
      exit(-1);
    }
    input = &ifs;
  }
  while(*input >> p >> s) {
    if(p == s)
      m[p];
    else {
      m[s].predcount++;
      m[p].succ.push_back(s);
    }
  }

  stringlist zeroes;
  for(string2tokenmap::iterator i = m.begin(); i != m.end(); i++)
    if(i->second.predcount == 0)
      zeroes.push_back(i->first);
  int n = 0;
  while(!zeroes.empty()) {
    string p = zeroes.front();
    zeroes.pop_front();
    cout << p << "\n";
    n++;
    stringlist &t = m[p].succ;
    for(stringlist::iterator i = t.begin(); i != t.end(); i++)
      if(--m[*i].predcount == 0)
        zeroes.push_back(*i);
  }
  if(n != m.size()) {
    cerr << "the ordering contains a loop\n";
    return -1;
  }

  return 0;
}
