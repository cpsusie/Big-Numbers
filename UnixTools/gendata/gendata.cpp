#include "stdafx.h"

static void printExpressionList(double x, Array<Expression> &expressionList) {
  for(size_t v = 0; v < expressionList.size(); v++) {
    Expression &e = expressionList[v];
    e.setValue(_T("x"), x);
    if(v > 0) {
      _tprintf(_T(" "));
    }
    _tprintf(_T("%.15le"), getDouble(e.evaluate()));
  }
  _tprintf(_T("\n"));
}

static void showHelp() {
  const int n = ExpressionDescription::getHelpListSize();
  for(int i = 0; i < n; i++) {
    const ExpressionDescription &desc = ExpressionDescription::getHelpList()[i];
    _tprintf(_T("%-45s%s\n"), desc.getSyntax(), desc.getDescription());
  }
}

static void usage() {
  _ftprintf(stderr,_T("Usage:gendata [-E] [-istart,end] [-ncount] -eexpression1 [-eexpresion2...]\n"
                      "        -ncount     : x will step through count equidistant values from start to stop. Default count=10\n"
                      "        -istart,end : Specify the interval that x will step through. Default interval is [0..count].\n"
                      "        -eexpression: Specify an expression to evaluate for each value of x.\n"
                      "        -E          : Use exponential growing steps instead of equal steps for x.\n"
                      "        -help       : Print a list of syntax for expressions.\n")
           );
  exit(-1);
}

int main(int argc, char **argv) {
  UINT              count               = 10;
  double            xStart, xEnd;
  Array<Expression> expressionList;
  bool              intervalSpecified   = false;
  bool              useExponentielSteps = false;
  char             *cp;

  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++; *cp; cp++) {
      switch(*cp) {
      case 'e':
        { Expression e;
          e.compile(cp+1,true);
          if(!e.isOk()) {
            e.listErrors(stderr);
            exit(-1);
          }
          expressionList.add(e);
        }
        break;

      case 'i':
        if(sscanf(cp+1,"%le,%le",&xStart,&xEnd) != 2) {
          usage();
        }
        intervalSpecified = true;
        break;

      case 'n':
        if(sscanf(cp+1,"%lu", &count) != 1) {
          usage();
        }
        if(count == 0) {
          fprintf(stderr,"Count must be > 0. (=%lu).\n",count);
          usage();
        }
        break;
      case 'E':
        useExponentielSteps = true;
        break;

      default :
        if(strcmp(cp,"help") == 0) {
          showHelp();
        }
        usage();
      }
      break;
    }
  }

  if(expressionList.size() == 0) {
    _ftprintf(stderr,_T("No expression specified. Use -e\n"));
    usage();
  }

  if(!intervalSpecified) {
    xStart = 0;
    xEnd  = count;
  }

  if(count == 1) {
    printExpressionList((xStart+xEnd)/2, expressionList);
  } else if(useExponentielSteps) {
    double t = dsign(xEnd-xStart);
    const double startMt = xStart-t;
    const double stepFactor = root(fabs(xEnd-startMt),dmax(1,(int)count-1)); // >= 0
    for(UINT i = 1; i <= count; i++, t *= stepFactor) {
      printExpressionList((i<count)?(startMt+t):xEnd, expressionList);
    }
  } else {
    const double step = (xEnd-xStart) / (count-1);
    double x = xStart;
    for(UINT i = 1; i <= count; i++, x += step) {
      printExpressionList((i<count)?x:xEnd, expressionList);
    }
  }
  return 0;
}
