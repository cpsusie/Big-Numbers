#include "stdafx.h"

static String srcDir = _T("c:\\mytools2015\\spil\\chess\\games.old");
static String dstDir = _T("c:\\mytools2015\\spil\\chess\\games");

static Options globalOptions;
Options &OptionsAccessor::getOptions() {
  return globalOptions;
}

int _tmain(int argc, TCHAR **argv) {
  DirList list = scandir(FileNameSplitter::getChildName(srcDir,_T("*.chs")));

  for(size_t i = 0; i < list.size(); i++) {
    const TCHAR *name = list[i].name;
    _tprintf(_T("%s\n"), name);
    Game game;
    Options options;
    const String srcName = FileNameSplitter::getChildName(srcDir, name);
    const String dstName = FileNameSplitter::getChildName(dstDir, name);

    FILE *in = NULL, *out = NULL;

    try {
      in = FOPEN(srcName, _T("r"));
      game.load(in);
      options.load(in);
      fclose(in);
      in = NULL;

      out = MKFOPEN(dstName, _T("w"));
      game.save(out);
      options.save(out);
      fclose(out);
      out = NULL;
    } catch(Exception e) {
      if(in) {
        fclose(in);
      }
      _tprintf(_T("Exception:%s\n"), e.what());
    }
  }
  return 0;
}
