#include "stdafx.h"
#include <MyUtil.h>
#include <Console.h>
#include <fcntl.h>
#include <io.h>
#include <comdef.h>
#include <atlconv.h>

#define LINESIZE 4096

void headTail(FILE *f, __int64 n, bool seekable) {
  String line;

  setFileMode(f,_O_TEXT);

  if(seekable) {
    FSEEK(f, 0);
  }
  for(__int64 i = 0; i < n; i++) { /* skip the first n lines */
    if(!readLine(f,line)) {
      return;
    }
  }
  while(readLine(f,line)) {
    _tprintf(_T("%s\n"),line.cstr());
  }
}

__int64 _max(__int64 a,__int64 b) { return a > b ? a : b; }
#define BUFSIZE 4096

static __int64 findStartOfTail(FILE *f, __int64 n) { // find start of n'th last line
  __int64 fileLength = GETSIZE(f);
  __int64 nlCount = 0; // Count newlines backwards from the end of file
  for(__int64 endPos = fileLength, startPos = max(fileLength - BUFSIZE,0); endPos != startPos;
    endPos = startPos, startPos = max(startPos - BUFSIZE,0)) {

    FSEEK(f, startPos);
    char buffer[BUFSIZE];
    size_t c = FREAD(buffer, 1, (size_t)(endPos-startPos),f);
    if(endPos == fileLength && (c > 0) && (buffer[c-1] != _T('\n'))) {
      nlCount++; // Treat the last line as a full line even though there is no '\n' at the end
    }

    for(char *cp = buffer + c - 1; cp >= buffer; cp--) { // Search backwards for '\n'
      if(*cp == _T('\n')) {
        if(nlCount++ >= n) {
          return startPos + (cp - buffer) + 1;
        }
      }
    }
  }
  return 0;
}

typedef QueueList<String> StringQueue;

static void fillQueue(FILE *f, __int64 n, StringQueue &lastLines) {
  String line;
  while(readLine(f, line)) {
    if(lastLines.size() == n) {
      lastLines.get();
    }
    lastLines.put(line);
  }
}

void endTail(FILE *f, __int64 n, bool seekable) {
  if(seekable) {
    FSEEK(f, findStartOfTail(f, n));
    setFileMode(f,_O_TEXT);
    String line;
    while(readLine(f,line)) {
      _tprintf(_T("%s\n"), line.cstr());
    }
  } else {
    StringQueue lastLines;
    fillQueue(f, n, lastLines);
    while(!lastLines.isEmpty()) {
      _tprintf(_T("%s\n"), lastLines.get().cstr());
    }
  }
}

static void usage() {
  _ftprintf(stderr,_T("Usage:tail [-f[delay]] [[-|+]count] file\n"
                      "            -f[delay]: Continously print out the tail of input, with the specified timedelay in seconds.\n"
                      "                       Default delay is 1 second.\n"
                      "           [-|+]count: If count < 0 ('-' is prefix), the last count lines will be printed\n"
                      "                       If count > 0 ('+' is prefix), tail will skip the first count lines and then print the rest of the file.\n"
                      "                       Default count = -10.\n")
           );
  exit(-1);
}

static bool tail(const TCHAR *name, bool tailFromEnd, __int64 lineCount) { // return file is seekable
  FILE *f = name ? FOPEN(name,_T("r")) : stdin;
  if(isatty(f)) {
    usage();
  }
  setFileMode(f,_O_BINARY);

  const bool seekable = (FSTAT(f).st_mode & _S_IFIFO) == 0; // cannot seek if input is a pipe
  if(tailFromEnd) {
    endTail(f, lineCount, seekable);
  } else {
    headTail(f, lineCount, seekable);
  }
  if(f != stdin) {
    fclose(f);
  }
  return seekable;
}

int main(int argc, char **argv) {
  try {
    __int64 lineCount   = -10;
    bool    tailFromEnd = true;
    bool    monitoring  = false;
    UINT    delay       = 1;

    char *cp;
    for(argv++; *argv && (*(cp = *argv) == '-' || *cp == '+'); argv++) {
      if(*cp == '+') {
        if(sscanf(cp,"%I64d",&lineCount) != 1) {
          usage();
        }
        continue;
      } else if(sscanf(cp,"%I64d",&lineCount) == 1) {
        continue;
      }
      // *cp == '-' and *cp is not a number
      for(cp++; *cp; cp++) {
        switch(*cp) {
        case 'f':
          monitoring = true;
          if(sscanf(cp+1,"%lu",&delay) != 1) {
            delay = 1;
          }
          break;
        default :
          usage();
        }
        break;
      }
    }

    if(lineCount < 0) {
      lineCount = -lineCount;
    } else {
      tailFromEnd = false;
    }
    USES_CONVERSION;
    if(monitoring) {
      for(;;) {
        Console::clear();
        Console::setCursorPos(0,0);
        if(!tail(A2T(*argv), tailFromEnd, lineCount)) {
          throwException(_T("Cannot monitor pipe"));
        }
        Sleep(1000*delay);
      }
    } else {
      tail(A2T(*argv), tailFromEnd, lineCount);
    }
  } catch(Exception e) {
    _ftprintf(stderr, _T("tail:%s\n"), e.what());
    return -1;
  }
  return 0;
}
