#include "pch.h"
#include <Math/Int128.h>

StrStream &operator<<(StrStream &stream, const _int128 &n) {
  tostrstream tts;
  tts.precision(stream.getPrecision());
  tts.width(stream.getWidth());
  tts.setf(stream.getFlags());
  tts << n;
  stream.append(tts.str().c_str());
  return stream;
}

StrStream &operator<<(StrStream &stream, const _uint128 &n) {
  tostrstream tts;
  tts.precision(stream.getPrecision());
  tts.width(stream.getWidth());
  tts.setf(stream.getFlags());
  tts << n;
  stream.append(tts.str().c_str());
  return stream;
}

String toString(const _int128  &n, StreamSize precision, StreamSize width, FormatFlags flags) {
  StrStream stream(precision,width,flags);
  stream << n;
  return stream;
}

String toString(const _uint128 &n, StreamSize precision, StreamSize width, FormatFlags flags) {
  StrStream stream(precision,width,flags);
  stream << n;
  return stream;
}
