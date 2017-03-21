#include "stdafx.h"
#include <Scandir.h>
#include <OpeningLibrary.h>

int main(int argc, char **argv) {

  try {
    CHDIR("c:\\mytools\\spil\\chess\\Openings");

    OpeningLibrary lib;

    DirList list = scandir("*.chs");
    for(int i = 0; i < list.size(); i++) {
      _finddata_t &name = list[i];
      printf("Adding <%s>\n", name.name);
      lib.addGame(name.name);
    }
    lib.reduceEmptyStates();

    const char *libName = "c:\\mytools\\spil\\chess\\res\\OpenLib.dat";
    printf("Saving %s\n", libName);
    lib.save(libName);
    FILE *f = FOPEN("openLib.txt","w");
    fprintf(f,"%s",lib.toString().cstr());
    fclose(f);
  } catch(Exception e) {
    printf("Error:%s\n",e.what());
    return -1;
  }

  return 0;
}
