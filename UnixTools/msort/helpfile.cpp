#include "stdafx.h"
#include <MyUtil.h>
#include "HelpFile.h"
#include <io.h>

TCHAR HelpFile::s_drive = _T('c');

HelpFile::HelpFile() {
  _tcscpy(m_name, EMPTYSTRING);
  m_f = NULL;
}

HelpFile::~HelpFile() { 
  close();
}

void HelpFile::makeName() {
  _stprintf(m_name,_T("%c:\\cXXXXXX"),s_drive);
  _tmktemp(m_name);
}

void HelpFile::destroy() {
  close();
  unlink(m_name);
}

void HelpFile::open(FileMode mode) {
  close();
  if(_tcslen(m_name) == 0) {
    makeName();
  }
  m_f = FOPEN(m_name,(mode == READMODE) ? _T("r") : _T("w"));
  if(setvbuf(m_f,NULL,_IOFBF,BUF_SIZE) != 0) {
    throwException(_T("setvbuf"));
  }
}

void HelpFile::close() {
  if(m_f != NULL) {
    fclose(m_f);
    m_f = NULL;
  }
}

void HelpFile::writeLine(Line s) {
  _fputts(s, m_f);
}

TCHAR *HelpFile::readLine(Line s) {
  return _fgetts(s, LINE_SIZE, m_f);
}
