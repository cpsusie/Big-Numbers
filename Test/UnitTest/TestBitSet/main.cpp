#include "stdafx.h"
#include <MyUtil.h>
#include <Console.h>
#include <random.h>
#include <istream>
#include <ostream>
#include "BitSet.h"
#include "TestBitSet.h"

#define DUMPSETS

static int getInteger(TCHAR *prompt) {
  for(;;) {
    Console::printf(0,23,_T("%-78.78s"),prompt);
    Console::setCursorPos(_tcslen(prompt)+1,23);
    TCHAR line[20];
    GETS(line);
    int x;
    if(_stscanf(line,_T("%d"),&x) == 1)
      return x;
  }
}

static int getch(TCHAR *prompt) {
  Console::printf(0,23,_T("%-78.78s"),prompt);
  Console::setCursorPos(_tcslen(prompt)+1,23);
  return Console::getKey();
}

static BitSet genQFilledSet(int size, double q) {
  BitSet result(size);
  int c = (int)(q*size);
  for(int i = 0; i < c; i++) {
    result += i;
  }
  return result;
}

static void findTimes(BitSet &s, double &oldtime, double &newtime) {
  double start;
  int i,cold,cnew;
#define LOOPCOUNT 400000
  start = getProcessTime();
  for(i = 0; i < LOOPCOUNT; i++)
    cold = s.oldsize();
  oldtime = getProcessTime() - start;
  start = getProcessTime();
  for(i = 0; i < LOOPCOUNT; i++)
    cnew = s.size();
  newtime = getProcessTime() - start;
  if(cold != cnew) {
    _ftprintf(stderr,_T("FEJL i newcount\n"));
    exit(-1);
  }
}

class CountTimes {
public:
  double oldtime,newtime;
  int n;
};

static void testCount() {
  randomize();

  for(int c = 0; c < 100; c++) {
    CountTimes t;
    t.n = 0; t.oldtime = t.newtime = 0;
    for(int k = 0; k < 10; k++) {
      BitSet s = genRandomSet(100,c);
      double oldtime,newtime;
      findTimes(s,oldtime,newtime);
      t.n++; t.oldtime += oldtime; t.newtime += newtime;
    }
    _tprintf(_T("%d %lf %lf\n"),c,t.oldtime/t.n, t.newtime/t.n);
  }

}

static void session() {
  BitSet c(32);
  BitSet a(31);

  a.add(10);

  BitSet b(40);
  BitSet *currentSet = &a;
  for(;;) {
    try {
      Console::clear(); // rect(0,0,80,3);
      tcout.flush(); 
      Console::setCursorPos(0,0);
      tcout << _T("A:") << a << _T("\n");
      tcout << _T("B:") << b << _T("\n");
      tcout << _T("C:") << c << _T("\n");
      tcout.flush();
  #ifdef DUMPSETS
      tcout << _T("dump(A):");
      a.dump(tcout); 
      tcout << _T("\n");

      tcout << _T("dump(B):");
      b.dump(tcout); 
      tcout << _T("\n");

      tcout << _T("dump(C):");
      c.dump(tcout);
      tcout << _T("\n");
  #endif
      tcout << _T("A.size() :") << std::ios_base::dec << a.size() << _T(" ");
      tcout << _T("B.size() :") << std::ios_base::dec << b.size() << _T(" ");
      tcout << _T("C.size() :") << std::ios_base::dec << c.size() << _T("\n");
      tcout << _T("A == B  :") << ((a == b) ? _T("true") : _T("false")) << _T(" ");
      tcout << _T("A <= B  :") << ((a <= b) ? _T("true") : _T("false")) << _T(" ");
      tcout << _T("B <= A  :") << ((b <= a) ? _T("true") : _T("false")) << _T(" ");
      tcout << _T("A <  B  :") << ((a <  b) ? _T("true") : _T("false")) << _T(" ");
      tcout << _T("B <  A  :") << ((b <  a) ? _T("true") : _T("false")) << _T("\n");
      tcout << _T("A.isEmpty():") << (a.isEmpty() ? _T("true") : _T("false")) << _T(" ");
      tcout << _T("B.isEmpty():") << (b.isEmpty() ? _T("true") : _T("false")) << _T(" ");
      tcout << _T("C.isEmpty():") << (c.isEmpty() ? _T("true") : _T("false")) << _T("  \n");
      tcout << _T("bitSetCmp(A,B):") << bitSetCmp(a,b) << _T("\n");
      tcout << _T("tester (a-b) + (b-a) == a^b: ") << (((a-b) + (b-a) == (a^b)) ? _T("ens") : _T("FORSKELLIGE!!!!!!")) << _T("   \n");
      tcout << _T("tester count(a+b) == count(a)+count(b)-count(a*b):") << (((a+b).size() == a.size()+b.size()-(a*b).size()) ? _T("ens") : _T("FORSKELLIGE!!!!!!")) << _T("  \n");
      tcout << _T("Iterator:");
      const TCHAR *delim = _T("");
      for(Iterator<unsigned int> it = currentSet->getIterator(); it.hasNext(); delim = _T(",")) {
        tcout << delim << it.next();
      }
      tcout.flush();
      delim = _T("");
      tcout << _T("\nReverse iterator:");
      for(Iterator<unsigned int> itr = currentSet->getReverseIterator(); itr.hasNext(); delim = _T(",")) {
        tcout << delim << itr.next();
      }
      tcout << _T("\n");
      tcout.flush();

      if(currentSet == &a) {
        tcout << _T("currentSet:A\n");
      } else if(currentSet == &b) {
        tcout << _T("currentSet:B\n");
      } else {
        tcout << _T("currentSet:C\n");
      }
      Console::setCursorPos(0,21);
      tcout << _T("(Q)uit/(=)/(A)/(B)/(C)/(i)ndsaet/(s)let/(M)edlem/(K)omplement/(R)eset/(+)/(-)/(*)/(^)\n");
      tcout.flush();
      unsigned long x,y;
      BitSet *assignto = NULL;
      switch(Console::getKey()) {
      case 'a':
      case 'A':
        currentSet = &a;
        break;
      case 'b':
      case 'B':
        currentSet = &b;
        break;
      case 'c':
      case 'C':
        currentSet = &c;
        break;
      case '=':
        switch(getch(_T("Assign til (A),(B) eller (C):"))) {
        case 'a': assignto = &a; break;
        case 'b': assignto = &b; break;
        case 'c': assignto = &c; break;
        }
        if(assignto) *assignto = *currentSet;
        break;
      case 'r':
        currentSet->clear();
        break;
      case 'i':
        x = getInteger(_T("Indtast tal:"));
        *currentSet += x;
        break;
      case 's':
        x = getInteger(_T("Indtast tal:"));
        *currentSet -= x;
        break;
      case 'I':
        x = getInteger(_T("Indtast interval start:"));
        y = getInteger(_T("Indtast interval slut:"));
        currentSet->add(x,y);
        break;
      case 'S':
        x = getInteger(_T("Indtast interval start:"));
        y = getInteger(_T("Indtast interval slut:"));
        currentSet->remove(x,y);
        break;
      case 'm':
        x = getInteger(_T("Indtast tal:"));
        tcout << _T("currentSet.contains(")<<x<<_T("):") << boolToStr(currentSet->contains(x)) << _T("\n");
        break;
      case 'k':
        currentSet->invert();
        break;
      case '+':
        switch(getch(_T("(A) = B + C, (B) = A + C eller (C) = A + B:"))) {
        case 'a': a = b + c; break;
        case 'b': b = a + c; break;
        case 'c': c = b + a; break;
        }
        break;
      case '-':
        switch(getch(_T("(A) = B - C, (B) = A - C eller (C) = A - B:"))) {
        case 'a': a = b - c; break;
        case 'b': b = a - c; break;
        case 'c': c = a - b; break;
        }
        break;
      case '*':
        switch(getch(_T("(A) = B * C, (B) = A * C eller (C) = A * B:"))) {
        case 'a': a = b * c; break;
        case 'b': b = a * c; break;
        case 'c': c = a * b; break;
        }
        break;
      case '^':
        switch(getch(_T("(A) = B ^ C, (B) = A ^ C eller (C) = A ^ B:"))) {
        case 'a': a = b ^ c; break;
        case 'b': b = a ^ c; break;
        case 'c': c = a ^ b; break;
        }
        break;
      case '#':
        { for(unsigned int i = 0; i < a.size(); i++) {
            if(rand() % 2) a.add(i);
          }
          for(unsigned int i = 0; i < b.size(); i++) {
            if(rand() % 2) b.add(i);
          }
        }
        break;
      case 'q':
        return;
      default:
        Console::setCursorPos(0,23);
        tcout << _T("Forkert kommando!\n");
        tcout.flush();
        pause();
        break;
      }
    } catch(Exception e) {
      Console::setCursorPos(0,23);
      tcout << _T("Exception:") << e.what() << _T("\n");
      tcout.flush();
      pause();
    }
  }
}

static void usage() {
  _ftprintf(stderr, _T("testbitset [-ic]\n"));
  exit(-1);
}

int _tmain(int argc, TCHAR **argv) {
  try {
    testBitSet();
    testTinyBitSet();
    testBitMatrix();

    TCHAR *cp;
    for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
      for(cp++; *cp; cp++) {
        switch(*cp) {
        case 'i': session(); break;
        case 'c': testCount(); break;
        default : usage();
        }
      }
    }
  } catch(Exception e) {
    _ftprintf(stderr, _T("Exception:%s\n"),e.what());
    return -1;
  }
  return 0;
}
