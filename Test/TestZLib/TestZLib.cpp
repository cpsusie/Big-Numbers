#include "stdafx.h"
#include <MyUtil.h>
#include <ByteFile.h>
#include <FileContent.h>
#include <comdef.h>
#include <atlconv.h>
#include <zlib.h>

#define ZLIB_PATH LIBROOT _PLATFORM_ _CONFIGURATION_

#ifdef GIVE_LINK_MESSAGE
#pragma message("link with " ZLIB_PATH)
#endif
#pragma comment(lib, ZLIB_PATH "zlib.lib")

typedef enum {
  NOCOMMAND
 ,COMPRESSFILES
 ,DECOMPRESSFILES
} Command;

static void usage() {
  fprintf(stderr,"Usage:testzlib -C [-v] libfile files\n"
                 "      testzlib -D [-v] libfile files\n"
         );
  exit(-1);
}

int main(int argc, char **argv) {
  char *cp;
  Command command = NOCOMMAND;
  bool verbose = false;

  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++; *cp; cp++) {
      switch(*cp) {
      case 'C': command = COMPRESSFILES;   continue;
      case 'D': command = DECOMPRESSFILES; continue;
      case 'v': verbose = true;            continue;
      default : usage();
      }
      break;
    }
  }

  if(command == NOCOMMAND) {
    usage();
  }

  if(!*argv) {
    usage();
  }
  String libName = *argv;
  FileNameSplitter info(libName);
  if(!info.getExtension().equalsIgnoreCase(_T(".zip"))) {
    info.setExtension(_T("zip"));
    libName = info.getFullPath();
  }
  USES_CONVERSION;
  const char *libNameA = T2A(libName.cstr());

  argv++;
  argvExpand(argc, argv);
  try {
    switch(command) {
    case COMPRESSFILES:
      { gzFile lib = gzopen(libNameA, "wb");
        if(lib == NULL) {
          if(errno == NULL) {
            throwException(_T("Out of memory"));
          } else {
            throwErrNoOnSysCallException(libName.cstr());
          }
        }
        for(;*argv; argv++) {
          const char *name = *argv;
          if(verbose) {
            fprintf(stderr,"compressing %-50s\r", name);
          }
          FileContent content(name);
          BYTE nameLength = (BYTE)strlen(name);
          gzwrite(lib,&nameLength,1);
          gzwrite(lib,name,nameLength);
          ULONG fileSize = (ULONG)content.size();
          gzwrite(lib,&fileSize,sizeof(fileSize));
          gzwrite(lib,content.getData(),fileSize);
        }
        gzclose(lib);
      }
      break;
    case DECOMPRESSFILES:
      { gzFile lib = gzopen(libNameA, "rb");
        if(lib == NULL) {
          if(errno == NULL) {
            throwException("out of memory");
          } else {
            throwErrNoOnSysCallException(libName.cstr());
          }
        }
        while(!gzeof(lib)) {
          BYTE nameLength = 0;
          if(gzread(lib,&nameLength,1) != 1) {
            break;
          }
          char name[257];
          gzread(lib,name,nameLength);
          name[nameLength] = '\0';
          ByteFile out(name,WRITEMODE);
          if(verbose) {
            fprintf(stderr,"decompressing %-50s\r", name);
          }
          ULONG fileSize;
          gzread(lib,&fileSize,sizeof(fileSize));
          BYTE *buffer = new BYTE[fileSize];
          gzread(lib,buffer,fileSize);
          out.putBytes(buffer,fileSize);
          delete[] buffer;
        }
        gzclose(lib);
      }
      break;
    }
  } catch(Exception e) {
    _ftprintf(stderr,_T("%s\n"),e.what());
    return -1;
  }
  return 0;
}
