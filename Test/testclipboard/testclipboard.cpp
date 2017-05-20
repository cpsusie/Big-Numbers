#include "stdafx.h"

typedef struct {
  int    m_format;
  TCHAR *m_name;
#define fn(f) f,_T(#f)
} FormatName;

static FormatName formats[] = {
  fn(CF_TEXT             ),
  fn(CF_BITMAP           ),
  fn(CF_METAFILEPICT     ),
  fn(CF_SYLK             ),
  fn(CF_DIF              ),
  fn(CF_TIFF             ),
  fn(CF_OEMTEXT          ),
  fn(CF_DIB              ),
  fn(CF_PALETTE          ),
  fn(CF_PENDATA          ),
  fn(CF_RIFF             ),
  fn(CF_WAVE             ),
  fn(CF_UNICODETEXT      ),
  fn(CF_ENHMETAFILE      ),
  fn(CF_HDROP            ),
  fn(CF_LOCALE           ),
  fn(CF_MAX              )
};

TCHAR *findFormatName(int cf) {
  for(int i = 0; i < ARRAYSIZE(formats); i++) {
    if(cf == formats[i].m_format) {
      return formats[i].m_name;
    }
  }
  return NULL;
}

static void usage() {
  fprintf(stderr,"testclipboard [-r]\n"
                 "        -r:reset clipboard\n"
         );
  exit(-1);
}

int main(int argc, char* argv[]) {
  bool resetClipboard = false;
  char *cp;
  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++; *cp; cp++) {
      switch(*cp) {
      case 'r': resetClipboard = true; continue;
      default: usage();
      }
      break;
    }
  }

  if(OpenClipboard(NULL) == 0) {
    _ftprintf(stderr,_T("%s\n"), getLastErrorText().cstr());
    return -1;
  }
  if(resetClipboard) {
    if(EmptyClipboard() == 0) {
      _ftprintf(stderr,_T("%s\n"), getLastErrorText().cstr());
      return -1;
    }
  } else {
    for(int cf = EnumClipboardFormats(0); cf; cf = EnumClipboardFormats(cf)) {
      TCHAR name[256];
      GetClipboardFormatName(cf,name,ARRAYSIZE(name));
      TCHAR *cfName = findFormatName(cf);
      if(cfName != NULL) {
        _tprintf(_T("format:%-20s:"), cfName);
      } else {
        _tprintf(_T("format:%-20s:"), name);
      }
      HANDLE t = GetClipboardData(cf);
      if(t == NULL) {
        _tprintf(_T("GetClipboardData failed:%s\n"), getLastErrorText().cstr());
      } else {
        TCHAR *str = (TCHAR*)t;
        if(cf == CF_TEXT) {
          _tprintf(_T("<%s>\n"), str);
        }
        try {
          hexdump(str,40,stdout);
        } catch(...) {
        }
      }
    }
  }
  CloseClipboard();
  return 0;
}
