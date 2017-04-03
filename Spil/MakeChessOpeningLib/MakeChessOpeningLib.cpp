#include "stdafx.h"
#include <Scandir.h>
#include <OpeningLibrary.h>

int main(int argc, char **argv) {

  try {
    CHDIR(_T("c:\\mytools2015\\spil\\chess\\Openings"));

    OpeningLibrary lib;

    DirList list = scandir(_T("*.chs"));
    for(int i = 0; i < list.size(); i++) {
      DirListEntry &name = list[i];
      _tprintf(_T("Adding <%s>\n"), name.name);
      lib.addGame(name.name);
    }
    lib.reduceEmptyStates();

    const TCHAR *libName = _T("c:\\mytools2015\\spil\\chess\\res\\OpenLib.dat");
    _tprintf(_T("Saving %s\n"), libName);
    lib.save(libName);
    FILE *f = FOPEN(_T("openLib.txt"),_T("w"));
    _ftprintf(f, _T("%s"), lib.toString().cstr());
    fclose(f);
  } catch(Exception e) {
    _tprintf(_T("Error:%s\n"), e.what());
    return -1;
  }
  return 0;
}
