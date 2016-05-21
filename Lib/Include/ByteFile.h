#pragma once

#include "MyUtil.h"
#include "ByteStream.h"

class ByteInputFile : public ResetableByteInputStream {
private:
  FILE            *m_file;
  unsigned __int64 m_startPos;
  int              m_oldMode;
  String           m_name;

  void init(const String &name);
public:
  ByteInputFile();
  ByteInputFile(const String &name);
  ByteInputFile(FILE *f);
  virtual ~ByteInputFile();

  ByteInputFile(const ByteInputFile &src);            // not defined. ByteInputFile not cloneable
  ByteInputFile &operator=(const ByteInputFile &src); // not defined. ByteInputFile not cloneable
  void open(const String &name);
  virtual void close();

  const String &getName() const {
    return m_name;
  }

  int getByte();
  int getBytes(BYTE *dst, unsigned int n);
  void reset();

  __int64 getPos() {
    return GETPOS(m_file);
  }

  void seek(__int64 pos);

  bool isOpen() const {
    return m_file != NULL;
  }
};

class ByteOutputFile : public ByteOutputStream {
private:
  FILE  *m_file;
  int    m_oldMode;
  String m_name;
  void init(const String &name);
public:
  ByteOutputFile();
  ByteOutputFile(const String &name);
  ByteOutputFile(FILE *f);
  virtual ~ByteOutputFile();

  ByteOutputFile(const ByteOutputFile &src);            // not defined. ByteOutputFile not cloneable
  ByteOutputFile &operator=(const ByteOutputFile &src); // not defined. ByteOutputFile not cloneable

  void open(const String &name);
  virtual void close();

  const String &getName() const {
    return m_name;
  }

  void putByte(BYTE c);
  void putBytes(const BYTE *src, unsigned int n);

  __int64 getPos() {
    return GETPOS(m_file);
  }

  void seek(__int64 pos);

  bool isOpen() const {
    return m_file != NULL;
  }
};

typedef enum {
  READMODE
 ,WRITEMODE
} FileMode;

class ByteFile : public ResetableByteInputOutputStream {
private:
  FILE            *m_file;
  unsigned __int64 m_startPos;
  int              m_oldMode;
  String           m_name;
  FileMode         m_mode;

  void init(const String &name, FileMode mode);
public:
  ByteFile();
  ByteFile(const String &name, FileMode mode);
  ByteFile(FILE *f, FileMode mode);
  virtual ~ByteFile();

  ByteFile(const ByteFile &src);            // not defined. ByteFile not cloneable
  ByteFile &operator=(const ByteFile &src); // not defined. ByteFile not cloneable

  void open(const String &name, FileMode mode);
  virtual void close();

  void putByte(BYTE c);
  void putBytes(const BYTE *src, unsigned int n);

  int getByte();
  int getBytes(BYTE *dst, unsigned int n);
  void reset();

  __int64 getPos() {
    return GETPOS(m_file);
  }

  void seek(__int64 pos);

  bool isOpen() const {
    return m_file != NULL;
  }

  FileMode getMode() const {
    return m_mode;
  }
};
