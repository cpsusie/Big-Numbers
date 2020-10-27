#include "stdafx.h"
#include <MyUtil.h>
#include "HelpFile.h"
#include <io.h>

TCHAR HelpFile::s_drive = _T('c');

void HelpFile::makeName() {
  TCHAR tmp[_MAX_FNAME+1];
  _stprintf(tmp, _T("%c:\\temp\\cXXXXXX"), s_drive);
  _tmktemp(tmp);
  m_name = tmp;
}

void HelpFile::destroy() {
  close();
  unlink(m_name);
}

void HelpFile::open(FileMode mode) {
  close();
  if(m_name.length() == 0) {
    makeName();
  }
  m_f = FOPEN(m_name,(mode == READMODE) ? _T("r") : _T("w"));
  if(setvbuf(m_f,nullptr,_IOFBF,BUF_SIZE) != 0) {
    throwException(_T("setvbuf"));
  }
  m_lineCount = 0;
}

void HelpFile::close() {
  if(m_f != nullptr) {
    fclose(m_f);
    m_f = nullptr;
  }
}
