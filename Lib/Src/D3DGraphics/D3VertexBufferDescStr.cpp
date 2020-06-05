#include "pch.h"
#include <D3DGraphics/D3ToString.h>
#include "D3NameLookupTemplate.h"

String toString(const D3DVERTEXBUFFER_DESC &desc, bool multiLines) {
  const TCHAR *formatStr = multiLines
    ? _T("Type  :%s\n"
         "Format:%s\n"
         "Pool  :%s\n"
         "Usage :%s\n"
         "FVF   :%s\n"
         "Size  :%s bytes\n")
    : _T("Type:%s,Format:%s,Pool:%s,Usage:%s,FVF:%s,Size:%s bytes");
  return format(formatStr
               ,resourceTypeToString(desc.Type  ).cstr()
               ,formatToString(      desc.Format).cstr()
               ,poolToString(        desc.Pool  ).cstr()
               ,usageToString(       desc.Usage ).cstr()
               ,FVFToString(         desc.FVF   ).cstr()
               ,format1000(          desc.Size  ).cstr()
               );
}
