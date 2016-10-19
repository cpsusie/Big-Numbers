#include "pch.h"
#include <zlib.h>
#include <CompressFilter.h>

#pragma comment(lib, LIB_VERSION "zlib.lib")

#define MAX_BUFFERSIZE 8192

DecompressFilter::DecompressFilter(ByteInputStream &src) : m_src(src) {
  m_zStreamp  = NULL;
  m_buffer    = NULL;

  z_stream *zStream = new z_stream;
  zStream->zalloc   = NULL;
  zStream->zfree    = NULL;
  zStream->opaque   = (voidpf)0;
  zStream->avail_in = 0;
  int err = inflateInit(zStream);
  if(err != Z_OK) {
    delete zStream;
    throwException(_T("%s:inflateInit:returncode=%d"), __TFUNCTION__, err);
  }
  m_zStreamp  = zStream;
  m_buffer    = new BYTE[MAX_BUFFERSIZE];
  m_pos       = 0;
  m_eos       = false;
  m_eoz       = false;
  setAvailableOut();
}

DecompressFilter::~DecompressFilter() {
  delete (z_streamp)m_zStreamp;
  delete[] m_buffer;
}

int DecompressFilter::getByte() {
  BYTE b;
  return (getBytes(&b,1) == 1) ? b : EOF;
}

intptr_t DecompressFilter::getBytes(BYTE *dst, size_t n) {
  size_t bytesDone = 0;
  while(bytesDone < n && !m_eos) {
    if(m_pos == (int)m_outputBuffer.size()) {
      fillOutputBuffer();
    }
    const size_t toCopy = min(m_outputBuffer.size() - m_pos, n - bytesDone);
    if(toCopy > 0) {
      memcpy(dst + bytesDone, m_outputBuffer.getData() + m_pos, toCopy);
      m_pos     += toCopy;
      bytesDone += toCopy;
    } else {
      m_eos = true;
    }
  }
  return bytesDone;
}

void DecompressFilter::fillOutputBuffer() {
  z_streamp zStreamp = (z_streamp)m_zStreamp;

  m_outputBuffer.clear();
  m_pos = 0;
  while(m_outputBuffer.size() < MAX_BUFFERSIZE) {
    if(zStreamp->avail_in == 0) {
      fillInputBuffer();
    }
    if(zStreamp->avail_in > 0) {
      decompress();
    }
    if(zStreamp->avail_out < MAX_BUFFERSIZE) {
      getDecompressedData();
    } else if(m_eoz) {
      break;
    }
  }
}

void DecompressFilter::fillInputBuffer() {
  m_inputBuffer.clear();
  while(m_inputBuffer.size() < MAX_BUFFERSIZE) {
    BYTE tmp[MAX_BUFFERSIZE];
    const intptr_t n = m_src.getBytes(tmp,sizeof(tmp));
    if(n > 0) {
      m_inputBuffer.append(tmp,n);
    }
    if(n < sizeof(tmp)) {
      break;
    }
  }
  z_streamp zStreamp = (z_streamp)m_zStreamp;
  zStreamp->next_in  = (BYTE*)m_inputBuffer.getData();
  zStreamp->avail_in = (UINT)m_inputBuffer.size();
}

void DecompressFilter::decompress() {
  const int err = inflate((z_streamp)m_zStreamp, Z_NO_FLUSH);
  if(err == Z_STREAM_END) {
    m_eoz = true;
  } else if(err != Z_OK) {
    throwException(_T("%s:inflate returncode:%d"), __TFUNCTION__, err);
  }
}

void DecompressFilter::getDecompressedData() {
  z_streamp zStreamp = (z_streamp)m_zStreamp;
  m_outputBuffer.append(m_buffer, MAX_BUFFERSIZE - zStreamp->avail_out);
  setAvailableOut();
}

void DecompressFilter::setAvailableOut() {
  z_streamp zStreamp  = (z_streamp)m_zStreamp;
  zStreamp->next_out  = m_buffer;
  zStreamp->avail_out = MAX_BUFFERSIZE;
}
