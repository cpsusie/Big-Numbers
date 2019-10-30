#include "stdafx.h"
#include <CPUInfo.h>
#include <Console.h>
#include <Math/BigReal.h>
#include <Math/MRisprime.h>
#include <Random.h>
#include <BitSet.h>
#include <Date.h>

using namespace std;

double _poly(int degree, double *p, double x) {
  double res = p[degree];
  for(int i = degree - 1; i >= 0; i--)
    res = res * x + p[i];
  return res;
}
static double _c[] = {
  -2.6336104921498027e-001,
  +3.4865633991741085e-003,
  -4.4526493660875375e-006,
  +1.0445218132346279e-008
};
double timeestimate(double x) { return _poly(3,_c,x); }

static void usage() {
  _ftprintf(stderr,_T("Usage:millerrabin [-v] [-c[BigReal] | -nCount[,digits]] [-tThreadCount]\n"
                      "                  -c[BigReal]: Check if the specified number is prime or composite.\n"
                      "                    If number not specified, it will be read from stdin.\n"
                      "                  -p:Calculate a^n mod n\n."
                      "                  -v:Verbose. Show whats going on.\n"
                      "                  -nCount[,digits]: Generate count random primes with the specified number of decimal digits.\n"
                      "                    Default number of digits is 100.\n"
                      "                  -tThreadCount : Run the specified threadCount threads in parallel, with each sequence\n"
                      "                    of numbers to check, returning the first that arrives at a prime\n"
                      "                    Default threadCount = 1. Maximum of threads that will be started is the number of cores\n"
                      "                    in the CPU.\n")
           );
  exit(-1);
}

class MRHandler : public MillerRabinHandler {
private:
  Semaphore m_gate;
  int       m_n;
  int       m_threadCount;
  int       m_winHeight;
public:
  void handleData(const MillerRabinCheck &data);
  MRHandler(int n, int threadCount, int winHeight);
};

MRHandler::MRHandler(int n, int threadCount, int winHeight) {
  m_n           = n;
  m_threadCount = threadCount;
  m_winHeight   = winHeight;
}

String shortString(const BigInt &n) {
  const size_t maxLength = 60;
  String       s         = toString(n);
  const size_t len = s.length();
  if(len < maxLength) {
    return s;
  } else {
    const int sl = (int)(maxLength-6)/2;
    const int mp = (int)maxLength - 2*sl;
    return left(s, sl) + spaceString(mp, _T('_')) + right(s, sl);
  }
}

void MRHandler::handleData(const MillerRabinCheck &data) {
  m_gate.wait();
  int y = data.m_threadId*(m_winHeight-7)/m_threadCount;
  Console::printf(0, y++, _T("Searching prime %3d"), m_n);
  Console::printf(0, y++, _T("%-50s"), data.m_msg.cstr());
  Console::printf(0, y++, _T("%s"), shortString(data.m_number).cstr());
  m_gate.signal();
}

typedef enum {
  CMD_CHECKISPRIME
 ,CMD_FINDPRIME
 ,CMD_POWMOD
} Command;

int main(int argc, char **argv) {
  char    *cp;
  BigInt   numberToCheck;
  bool     verbose      = false;
  Command  cmd          = CMD_FINDPRIME;
  int      threadCount  = 1;
  UINT     count        = 1;
  UINT     digits       = 100;

  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++;*cp; cp++) {
      switch(*cp) {
      case 'v':
        verbose = true;
        continue;
      case 'c':
        cmd = CMD_CHECKISPRIME;
        if(isdigit(cp[1])) {
          numberToCheck = BigInt(cp+1);
          break;
        } else {
          continue;
        }
      case 'n':
        if(sscanf(cp+1,"%u,%u",&count,&digits) != 2) {
          if(sscanf(cp+1,"%u",&count) != 1) {
            usage();
          } else {
            digits = 100;
          }
        }
        if(digits < 1 || digits > 100000) {
          usage();
        }
        break;
      case 't':
        if((sscanf(cp+1, "%u", &threadCount) != 1) || (threadCount < 1)) {
          usage();
        }
        break;
      case 'p':
        cmd = CMD_POWMOD;
        break;
      default:
        usage();
      }
      break;
    }
  }

  const int processors = getProcessorCount();
  if(threadCount > processors) {
    threadCount = processors;
  }

  try {
    switch(cmd) {
    case CMD_CHECKISPRIME:
      if(numberToCheck.isZero()) {
        tcout << _T("Enter BigReal:");
        tcin >> numberToCheck;
      }
      if(MRisprime(0, numberToCheck)) {
        tcout << numberToCheck << _T(" is prime\n");
      } else {
        tcout << numberToCheck << _T(" is composite\n");
      }
      break;
    case CMD_FINDPRIME:
      { randomize();
        int winW, winH;
        if(verbose) {
          Console::getLargestConsoleWindowSize(winW, winH);
          winW -= 10;
          winH --;
          Console::setWindowAndBufferSize(0,0,winW, winH);
          Console::clear();
        }
        for(UINT i = 1; i <= count; i++) {
          BigInt n = findRandomPrime(digits, threadCount, NULL, verbose?&MRHandler(i,threadCount, winH):NULL);
          tcout << n << endl;
        }
      }
      break;
    case CMD_POWMOD:
      { tcout << _T("Calculate a^r mod n.");
        for(;;) {
          tcout << _T("Enter a,r,n (separated with whitespace):");
          BigInt a,r,n;
          tcin >> a >> r >> n;
          BigInt p = powmod(a, r, n, verbose);
          tcout << _T("powmod(a,r,n)=") << p << endl;
        }
      }
    }
  } catch(Exception e) {
    _tprintf(_T("Exception:%s\n"), e.what());
    return -1;
  }
  return 0;
}

