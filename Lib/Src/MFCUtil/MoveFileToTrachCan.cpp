#include "pch.h"
#include <ShellApi.h>

bool moveFileToTrashCan(HWND hwnd, const String &fileName, bool confirm) {
  StringArray nameArray;
  nameArray.add(fileName);
  return moveFilesToTrashCan(hwnd, nameArray, confirm);
}

bool moveFilesToTrashCan(HWND hwnd, const StringArray &fileNames, bool confirm) {
  String zzpFrom = fileNames.getAsDoubleNullTerminatedString();
  TCHAR zzpTo[] = {0,0};

  SHFILEOPSTRUCT shf;
  shf.hwnd          = hwnd;
  shf.wFunc         = FO_DELETE;
  shf.pFrom         = zzpFrom.cstr();
  shf.pTo           = zzpTo;
  shf.fFlags        = FOF_ALLOWUNDO;
  if(!confirm) {
    shf.fFlags |= FOF_NOCONFIRMATION;
  }
  shf.hNameMappings = nullptr;
  return SHFileOperation(&shf) == 0;

}
