#include "stdafx.h"
#include <MyUtil.h>
#include <MFCUtil/FileSink.h>
#include <pldebug.h>

#define DATABUFSIZE 10000000

FileSink::FileSink(FILE *file) {
  m_file = file;
  m_dataBuffer = new PLBYTE[DATABUFSIZE];
  PLDataSink::Open("file", m_dataBuffer, DATABUFSIZE);
}

FileSink::~FileSink() {
  const int towrite = (int)GetDataSize();
  const int written = (int)fwrite(m_pStartData, 1, towrite, m_file );
  PLASSERT(written == towrite );
  PLDataSink::Close();
  delete[] m_dataBuffer;
}

ByteStreamSink::ByteStreamSink(ByteOutputStream &stream) : m_stream(stream) {
  m_dataBuffer = new PLBYTE[DATABUFSIZE];
  PLDataSink::Open("stream", m_dataBuffer, DATABUFSIZE);
}

ByteStreamSink::~ByteStreamSink() {
  const int towrite = (int)GetDataSize();
  m_stream.putBytes(m_dataBuffer, towrite);
  PLDataSink::Close();
  delete[] m_dataBuffer;
}

