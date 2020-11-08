#include "pch.h"
#include <zlib.h>
#include <CompressFilter.h>

#pragma comment(lib, LIB_VERSION "zlib.lib")

#define MAX_BUFFERSIZE 8192

CompressFilter::CompressFilter(ByteOutputStream &dst, CompressionLevel level) : m_dst(dst) {
  m_zStreamp  = nullptr;
  m_buffer    = nullptr;

  z_stream *zStream = new z_stream; TRACE_NEW(zStream);
  zStream->zalloc   = nullptr;
  zStream->zfree    = nullptr;
  zStream->opaque   = (voidpf)0;

  const int err = deflateInit(zStream, (int)level);
  if(err != Z_OK) {
    SAFEDELETE(zStream);
    throwException(_T("%s:deflateInit:returncode=%d"), __TFUNCTION__, err);
  }
  m_zStreamp  = zStream;
  m_buffer    = new BYTE[MAX_BUFFERSIZE*2]; TRACE_NEW(m_buffer);

  setAvailableOut();
};

CompressFilter::~CompressFilter() {
  finalFlush();
  z_streamp streamp = (z_streamp)m_zStreamp;
  SAFEDELETE(streamp);
  SAFEDELETEARRAY(m_buffer);
}

void CompressFilter::putByte(BYTE b) {
  putBytes(&b, 1);
}

void CompressFilter::putBytes(const BYTE *src, size_t n) {
  while(n > 0) {
    const size_t size = min(n, MAX_BUFFERSIZE - m_inputBuffer.size());
    if(m_inputBuffer.append(src, size).size() >= MAX_BUFFERSIZE) {
      flushInput();
    }
    n   -= size;
    src += size;
  }
}

void CompressFilter::flush() {
  flushInput();
  const int err = deflate((z_streamp)m_zStreamp, Z_PARTIAL_FLUSH);
  switch(err) {
  case Z_OK        :
    getCompressedData();
    flushOutput();
    break;
  default:
    throwException(_T("%s:deflate:returncode:%d"), __TFUNCTION__, err);
  }
}

void CompressFilter::flushInput() {
  z_streamp zStreamp = (z_streamp)m_zStreamp;
  zStreamp->next_in  = (BYTE*)m_inputBuffer.getData();
  zStreamp->avail_in = (UINT)m_inputBuffer.size();

  while(zStreamp->avail_in > 0) {
    const int err = deflate(zStreamp, Z_NO_FLUSH);
    if(err != Z_OK) {
      throwException(_T("%s:deflate:returncode:%d"), __TFUNCTION__, err);
    }
    if(zStreamp->avail_out < MAX_BUFFERSIZE) {
      getCompressedData();
      if(m_outputBuffer.size() >= MAX_BUFFERSIZE) {
        flushOutput();
      }
    }
  }
  m_inputBuffer.clear();
}

void CompressFilter::finalFlush() {
  flushInput();
  for(;;) {
    const int err = deflate((z_streamp)m_zStreamp, Z_FINISH);
    switch(err) {
    case Z_OK        :
      getCompressedData();
      flushOutput();
      break;
    case Z_STREAM_END:
      getCompressedData();
      flushOutput();
      return;
    default:
      throwException(_T("%s:deflate:returncode:%d"), __TFUNCTION__, err);
    }
  }
}

void CompressFilter::flushOutput() {
  m_dst.putBytes(m_outputBuffer.getData(), m_outputBuffer.size());
  m_outputBuffer.clear();
}

void CompressFilter::getCompressedData() {
  z_streamp zStreamp = (z_streamp)m_zStreamp;
  m_outputBuffer.append(m_buffer, MAX_BUFFERSIZE*2 - zStreamp->avail_out);
  setAvailableOut();
}

void CompressFilter::setAvailableOut() {
  z_streamp zStreamp  = (z_streamp)m_zStreamp;
  zStreamp->next_out  = m_buffer;
  zStreamp->avail_out = MAX_BUFFERSIZE*2;
}
