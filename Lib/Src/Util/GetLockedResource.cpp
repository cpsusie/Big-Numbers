#include "pch.h"
#include <WinUser.h>
#include <ByteArray.h>

static String typeNameToString(const TCHAR *typeName) {
  switch((int)((size_t)((ULONG_PTR)typeName))) {

#define caseStr(s) case ((int)((size_t)(ULONG_PTR)s)): return _T(#s)

  caseStr(RT_CURSOR      );
  caseStr(RT_BITMAP      );
  caseStr(RT_ICON        );
  caseStr(RT_MENU        );
  caseStr(RT_DIALOG      );
  caseStr(RT_STRING      );
  caseStr(RT_FONTDIR     );
  caseStr(RT_FONT        );
  caseStr(RT_ACCELERATOR );
  caseStr(RT_RCDATA      );
  caseStr(RT_MESSAGETABLE);
  caseStr(RT_GROUP_CURSOR);
  caseStr(RT_GROUP_ICON  );
  caseStr(RT_VERSION     );
  caseStr(RT_DLGINCLUDE  );
  caseStr(RT_PLUGPLAY    );
  caseStr(RT_VXD         );
  caseStr(RT_ANICURSOR   );
  caseStr(RT_ANIICON     );
  caseStr(RT_HTML        );
  default:
    try {
      return typeName;
    } catch(...) {
      return format(_T("%p"), typeName);
    }
  }
}


ByteArray &ByteArray::loadFromResource(int resId, const TCHAR *typeName) {
  DEFINEMETHODNAME;
  HRSRC hSource = FindResource(NULL, MAKEINTRESOURCE(resId), typeName);

  if(hSource == NULL) {
    throwException(_T("%s(%d,%s) failed:%s"), method, resId, typeNameToString(typeName).cstr(), getLastErrorText().cstr());
  }

  HGLOBAL hGlobal = LoadResource(NULL, hSource);
  if(hGlobal == NULL) {
    throwException(_T("%s(%d,%s) failed:%s"), method, resId, typeNameToString(typeName).cstr(), getLastErrorText().cstr());
  }

  try {
    void *r = LockResource(hGlobal);
    if(r == NULL) {
      throwException(_T("%s(%d,%s):LockResource failed"), method, resId, typeNameToString(typeName).cstr());
    }

    const int size = (UINT)SizeofResource(NULL, hSource);

    clear(size);
    append((BYTE*)r, size);

    UnlockResource(hGlobal); // 16Bit Windows Needs This
    FreeResource(hGlobal);   // 16Bit Windows Needs This (32Bit - Automatic Release)
    hGlobal = NULL;
    return *this;
  } catch(...) {
    UnlockResource(hGlobal); // 16Bit Windows Needs This
    FreeResource(hGlobal);   // 16Bit Windows Needs This (32Bit - Automatic Release)
    throw;
  }
}

