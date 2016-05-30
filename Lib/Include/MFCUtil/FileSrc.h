#pragma once

#include <stdio.h>
#include <pldatasrc.h>

// This is a class which takes a file as a source of picture data.
class FileSource : public PLDataSource {
private:
  PLBYTE *m_dataBuffer;
  int     m_bufferSize;
  int     m_currentPosition;
public:
  FileSource(FILE *f);
  ~FileSource();

  PLBYTE *ReadNBytes(int n);

  //! Read but don't advance file pointer.
  PLBYTE *GetBufferPtr(int MinBytesInBuffer);
  PLBYTE *ReadEverything();
  void Seek(int n);
};
