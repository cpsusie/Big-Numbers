#include "stdafx.h"
#include <ExternProcess.h>
#include <MD5.h>
#include "HighScore.h"
#include <CompressFilter.h>
#include <ByteFile.h>
#include <ByteMemoryStream.h>

Options::Options() {
  load();
}

Options::~Options() {
}

static void copyStream(ByteOutputStream &out, ByteInputStream &in) {
  BYTE buffer[4096];
  intptr_t n;
  while((n = in.getBytes(buffer, sizeof(buffer))) > 0) {
    out.putBytes(buffer, n);
  }
}

void copyFile(const String &to, const String &from) {
  copyStream(ByteOutputFile(to), ByteInputFile(from));
}

bool Options::save() const {
  const String fileName = getModuleFileName();
  FileNameSplitter info(fileName);
  info.setFileName(info.getFileName() + _T("1"));
  const String outFileName = info.getAbsolutePath();

  try {
    copyFile(outFileName, fileName);
  } catch(Exception e) {
    showWarning(getLastErrorText());
    return false;
  }

  HANDLE exeHandle = BeginUpdateResource(outFileName.cstr(), FALSE);
  if(exeHandle == nullptr) {
    unlink(outFileName);
    showWarning(getLastErrorText());
    return false;
  }

  ByteArray buffer;
  save(CompressFilter(ByteMemoryOutputStream(buffer)));

  if(!UpdateResource(exeHandle, _T("OPTIONS"), MAKEINTRESOURCE(IDR_OPTIONS), MAKELANGID(LANG_DANISH,SUBLANG_DEFAULT), (BYTE*)buffer.getData(), (DWORD)buffer.size())) {
    showWarning(getLastErrorText());
    return false;
  }

  if(!EndUpdateResource(exeHandle, FALSE)) {
    showWarning(getLastErrorText());
    return false;
  }

  if(_tspawnl(_P_NOWAIT, outFileName.cstr(), outFileName.cstr(), _T("-c"), getMD5Password().cstr(), fileName.cstr(), nullptr) < 0) {
    showWarning(_T("spawn failed:%s"), getErrnoText().cstr());
    return false;
  }
/*
  FileNameSplitter info(__FILE__);
  info.setFileName(_T("options")).setExtension(_T("bin"));
  info.setDir(FileNameSplitter::getChildName(info.getDir(),_T("res")));
  save(CompressFilter(ByteOutputFile(info.getAbsolutePath())));
*/
  exit(0);
  return true;
}

void Options::load() {
  load(DecompressFilter(ByteMemoryInputStream(ByteArray().loadFromResource(IDR_OPTIONS, _T("OPTIONS")))));
}

void Options::save(ByteOutputStream &s) const {
  Packer p;
  p << m_password
    << m_highScoreFileName;
  p.write(s);
}

void Options::load(ByteInputStream &s) {
  Packer p;
  p.read(s);
  p >> m_password
    >> m_highScoreFileName;
}

String Options::getMD5Password() const {
  USES_CONVERSION;
  char *apasswrd = T2A(m_password.cstr());
  MD5HashCode code;
  return MD5::getHashCode(code, ByteArray((BYTE*)apasswrd, strlen(apasswrd))).toString(false);
}
