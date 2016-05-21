#include "pch.h"
#include <zlib.h>
#include <CompressFilter.h>

#ifdef _DEBUG
#pragma comment(lib,"c:/mytools2015/lib/MTDebug32/zlib.lib")
#else
#pragma comment(lib,"c:/mytools2015/lib/MTRelease32/zlib.lib")
#endif

#define MAX_BUFFERSIZE 8192

CompressFilter::CompressFilter(ByteOutputStream &dst, CompressionLevel level) : m_dst(dst) {
  m_zStreamp  = NULL;
  m_buffer    = NULL;

  z_stream *zStream = new z_stream;
  zStream->zalloc   = NULL;
  zStream->zfree    = NULL;
  zStream->opaque   = (voidpf)0;
  
  int err = deflateInit(zStream, (int)level);
  if (err != Z_OK) {
    delete zStream;
    throwException(_T("CompressFilter:deflateInit:returncode=%d"), err);
  }
  m_zStreamp  = zStream;
  m_buffer    = new BYTE[MAX_BUFFERSIZE*2];

  setAvailableOut();
};

CompressFilter::~CompressFilter() {
  finalFlush();
  delete (z_streamp)m_zStreamp;
  delete[] m_buffer;
}

void CompressFilter::putByte(BYTE b) {
  putBytes(&b, 1);
}

void CompressFilter::putBytes(const BYTE *src, unsigned int n) {
  while(n > 0) {
    const int size = min(n, MAX_BUFFERSIZE - m_inputBuffer.size());
    if(m_inputBuffer.append(src, size).size() >= MAX_BUFFERSIZE) {
      flushInput();
    }
    n   -= size;
    src += size;
  }
}

void CompressFilter::flush() {
  flushInput();
  int err = deflate((z_streamp)m_zStreamp, Z_PARTIAL_FLUSH);
  switch(err) {
  case Z_OK        :
    getCompressedData();
    flushOutput();
    break;
  default:
    throwException(_T("flush:deflate:returncode:%d"), err);
  }
}

void CompressFilter::flushInput() {
  z_streamp zStreamp = (z_streamp)m_zStreamp;
  zStreamp->next_in  = (BYTE*)m_inputBuffer.getData();
  zStreamp->avail_in = m_inputBuffer.size();

  while(zStreamp->avail_in > 0) {
    int err = deflate(zStreamp, Z_NO_FLUSH);
    if(err != Z_OK) {
      throwException(_T("deflate:returncode:%d"), err);
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
    int err = deflate((z_streamp)m_zStreamp, Z_FINISH);
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
      throwException(_T("finalFlush:deflate:returncode:%d"), err);
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
