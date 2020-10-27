#include "stdafx.h"
#include "SaveAsjob.h"

SaveAsJob::SaveAsJob(const String &newName, ByteContainer &src)
: m_newName(newName)
, m_src(src)
, m_size(src.getSize())
, m_fileIndex(0)
{
}

UINT SaveAsJob::safeRun() {
  doSave();
  return 0;
}

void SaveAsJob::doSave() {
  FILE *dst = MKFOPEN(m_newName, _T("wb"));
  try {
    ByteArray buffer;
    for(m_fileIndex = 0; (m_fileIndex < m_size) && !isInterrupted(); m_fileIndex += buffer.size()) {
      m_src.getBytes(m_fileIndex, 0x4000, buffer);
      if(buffer.size()) {
        FWRITE(buffer.getData(), 1, buffer.size(), dst);
      }
    }
    if(isInterrupted()) {
      throwException(_T("Save cancelled"));
    }
    fclose(dst);
    dst = nullptr;
  } catch(...) {
    if(dst) {
      fclose(dst);
    }
    unlink(m_newName);
    throw;
  }
}
