#pragma once

#include <stdio.h>
#include <pldatasink.h>
#include "ByteStream.h"

// This is a data sink class which takes a file as a destination of picture data.
class FileSink : public PLDataSink {
private:
  FILE *m_file;
  PLBYTE *m_dataBuffer;

public:
  FileSink(FILE *file);
  ~FileSink();
};

class ByteStreamSink : public PLDataSink {
private:
  ByteOutputStream &m_stream;
  PLBYTE           *m_dataBuffer;
public:
  ByteStreamSink(ByteOutputStream &stream);
  ~ByteStreamSink();
};
