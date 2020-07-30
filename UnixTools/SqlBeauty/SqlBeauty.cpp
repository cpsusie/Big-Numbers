#include "stdafx.h"
#include <FileNameSplitter.h>
#include <MFCUtil/Clipboard.h>

#include "ParserTree.h"

static String getStmt() {
  String result;
  for(;;) {
    printf(">");
    TCHAR line[1024];
    GETS(line);
    TCHAR trimmet[1024];
    strTrim(_tcscpy(trimmet,line));
    const size_t l = _tcslen(trimmet);
    if(l > 0 && trimmet[l-1] == ';') {
      trimmet[l-1] = '\0';
      result += trimmet;
      break;
    }
    result += line;
    result += _T("\n\r");
  }
  return result;
}

static void testParser(const TCHAR *fileName) {
  String stmt = (fileName == NULL) ? getStmt() : readTextFile(fileName);

  const TCHAR *dispFileName = fileName != NULL ? fileName : _T("stdin");

  SqlParser parser;
  _tprintf(_T("Now parsing <%s>                            \r"), dispFileName);
  parser.parse(dispFileName, 1, stmt.cstr());

  if(!parser.ok()) {
    _tprintf(_T("Errors in <%s>                            \n"), dispFileName);
    parser.listErrors();
    pause();
  } else {
    FILE *out = (fileName == NULL) ? stdout : fopen(FileNameSplitter(fileName).setExtension("dmp").getFullPath(),"w");
    if(out) {
      _ftprintf(out, _T("%s\n"), parser.beautify().cstr());
      if(out != stdout)
        fclose(out);
    }
  }
}

static void testScanner(const TCHAR *fileName) {
  String stmt = (fileName == NULL) ? getStmt() : readTextFile(fileName);

  _tprintf(_T("Now scanning <%s>\n"), stmt.cstr());

  LexStringStream stream(stmt.cstr());
  SqlParser       parser;
  SqlLex          lex(&parser);

  parser.setScanner(&lex);
  lex.newStream(&stream,1);
  int s;
  while(s = lex.getNextLexeme()) {
    TCHAR *str = lex.getText();
    int    l   = 25 - (int)_tcslen(str);
    if(l < 0) l = 0;
    _tprintf(_T("<%s>%*.*s\tsym:%s\n"),str,l,l,EMPTYSTRING,parser.getSymbolName(s));
  }
  parser.listErrors();
}

static void runTest(TCHAR **argv, void (*tester)(const TCHAR *)) {
  if(*argv == NULL) {
    for(;;) tester(NULL);
  } else {
    for(;*argv;argv++) tester(*argv);
  }
}

static int beautify(const TCHAR *fileName, const String &outFileName) {
  SqlParser parser;
  parser.parse(fileName, 1, readTextFile(fileName).cstr());

  if(!parser.ok())
    return -1;

  FILE *f = fopen(outFileName,_T("w"));
  if(f) {
    String tmp(parser.beautify());
    tmp.replace('\n', _T("\r\n"));
    _ftprintf(f,_T("%s"), tmp.cstr());
    fclose(f);
  }
  return 0;
}

static int beautifyClipboard() {
  SqlParser parser;
  parser.parse(EMPTYSTRING, 1, getClipboardText().cstr());

  if(!parser.ok()) {
    return -1;
  }
  putClipboard(NULL,parser.beautify());
  return 0;
}

static void usage() {
  _ftprintf(stderr, _T("testpars [-l] [-ofile] [-c] [inputfiles]\n"
                       "  -l:Test lexical scanner\n"
                       "  -ofile:Specify outputfile. only one inputfile can be specified\n"
                       "  -c:Read input and write output from/to clipboard\n")
           );
  exit(-1);
}

int _tmain(int argc, TCHAR **argv) {
  TCHAR *cp;
  bool   testLex      = false;
  TCHAR *outFileName  = NULL;
  bool   useClipboard = false;

  argvExpand(argc,argv);

//  extern void findbesthashmapsize();
//findbesthashmapsize();

  for(argv++;*argv && *(cp = *argv) == '-'; argv++) {
    for(cp++;*cp;cp++) {
      switch(*cp) {
      case 'l':
        testLex = true;
        continue;
      case 'o':
        outFileName = cp+1;
        break;
      case 'c':
        useClipboard = true;
        continue;
      default:
        usage();
      }
      break;
    }
  }

  if(useClipboard) {
    return beautifyClipboard();
  }

  if(outFileName != NULL && *argv) {
    return beautify(*argv, outFileName);
  }

  if(testLex) {
    runTest(argv, testScanner);
  } else {
    runTest(argv, testParser);
  }
  return 0;
}
