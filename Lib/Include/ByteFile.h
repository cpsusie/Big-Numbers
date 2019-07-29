#pragma once

#include "BasicIncludes.h"
#include "ByteStream.h"

class ByteInputFile : public ResetableByteInputStream {
private:
  FILE            *m_file;
  unsigned __int64 m_startPos;
  int              m_oldMode;
  String           m_name;

  ByteInputFile(const ByteInputFile &src);            // Not defined. Class not cloneable
  ByteInputFile &operator=(const ByteInputFile &src); // Not defined. Class not cloneable
  void init(const String &name);
public:
  ByteInputFile();
  ByteInputFile(const String &name);
  ByteInputFile(FILE *f);
  virtual ~ByteInputFile();

  void open(const String &name);
  virtual void close();

  inline const String &getName() const {
    return m_name;
  }

  int getByte();
  intptr_t getBytes(BYTE *dst, size_t n);
  void reset();

  __int64 getPos();

  void seek(__int64 pos);

  inline bool isOpen() const {
    return m_file != NULL;
  }
};

class ByteOutputFile : public ByteOutputStream {
private:
  FILE  *m_file;
  int    m_oldMode;
  String m_name;

  ByteOutputFile(const ByteOutputFile &src);            // Not defined. Class not cloneable
  ByteOutputFile &operator=(const ByteOutputFile &src); // Not defined. Class not cloneable
  void init(const String &name);
public:
  ByteOutputFile();
  ByteOutputFile(const String &name);
  ByteOutputFile(FILE *f);
  virtual ~ByteOutputFile();


  void open(const String &name);
  virtual void close();

  inline const String &getName() const {
    return m_name;
  }

  void putByte(BYTE c);
  void putBytes(const BYTE *src, size_t n);

  __int64 getPos();

  void seek(__int64 pos);

  inline bool isOpen() const {
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

  ByteFile(const ByteFile &src);            // Not defined. Class not cloneable
  ByteFile &operator=(const ByteFile &src); // Not defined. Class not cloneable

  void init(const String &name, FileMode mode);
public:
  ByteFile();
  ByteFile(const String &name, FileMode mode);
  ByteFile(FILE *f, FileMode mode);
  virtual ~ByteFile();

  void open(const String &name, FileMode mode);
  virtual void close();

  void putByte(BYTE c);
  void putBytes(const BYTE *src, size_t n);

  int getByte();
  intptr_t getBytes(BYTE *dst, size_t n);
  void reset();

  inline __int64 getPos();

  void seek(__int64 pos);

  inline bool isOpen() const {
    return m_file != NULL;
  }

  inline FileMode getMode() const {
    return m_mode;
  }
};
