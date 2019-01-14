#pragma once

#include <HashMap.h>
#include <ByteStream.h>
#include <Date.h>
#include "mp3tag.h"
#include "EncodedString.h"

String toString(ID3_FrameID   id  );
String toString(ID3_FieldID   id  );
String toString(ID3_FieldType type);
String toString(ID3_TextEnc   enc );

class GenreMap : public StringIHashMap<int> {
public:
  GenreMap();
  inline int getIndex(const String &genre) const {
    const int *result = __super::get(genre);
    return result ? *result : -1;
  }
};

class StringField {
private:
private:
  ID3_TextEnc          m_encoding;
  Array<EncodedString> m_esa;
public:
  StringField(const ID3_Field *field);
  StringField(const StringField &src) 
    : m_encoding(src.m_encoding)
    , m_esa(src.m_esa)
  {
  }
  inline const Array<EncodedString> &getStrings() const {
    return m_esa;
  }
  inline ID3_TextEnc getEncoding() const {
    return m_encoding;
  }
  inline size_t getNumItems() const {
    return m_esa.size();
  }
  String toString() const;
};

class FieldWithData {
private:
  ID3_FieldID   m_fieldId;
  ID3_FieldType m_fieldType;

  union {
    uint32       m_int32;
    ByteArray   *m_binary;
    StringField *m_stringField;
  };
  void allocate(const ID3_Field *field);
  void allocate(const FieldWithData &field);
  void cleanup();
  inline void checkType(const TCHAR *method, ID3_FieldType et) const {
    if(et != m_fieldType) {
      throwInvalidArgumentException(method, _T("type mismatch. fieldType=%d, expectedType=%d"), method, m_fieldType, et);
    }
  }
public:
  inline FieldWithData(const ID3_Field *field) {
    allocate(field);
  }
  inline FieldWithData(const FieldWithData &src) {
    allocate(src);
  }
  FieldWithData &operator=(const FieldWithData &src);
  ~FieldWithData();
  inline ID3_FieldID   getId() const {
    return m_fieldId;
  }
  inline ID3_FieldType getType() const {
    return m_fieldType;
  }
  inline uint32 getInt() const {
    checkType(__TFUNCTION__, ID3FTY_INTEGER);
    return m_int32;
  }
  inline const ByteArray &getBinData() const {
    checkType(__TFUNCTION__, ID3FTY_BINARY);
    return *m_binary;
  }
  inline const StringField &getStringData() const {
    checkType(__TFUNCTION__, ID3FTY_TEXTSTRING);
    return *m_stringField;
  }
  String toString() const;
};

class Frame {
private:
  ID3_FrameID          m_frameId;
  String               m_desc;
  Array<FieldWithData> m_fieldArray;
public:
  Frame(const ID3_Frame &frame);
  inline ID3_FrameID getId() const {
    return m_frameId;
  }
  inline const String &getDescription() const {
    return m_desc;
  }
  inline const Array<FieldWithData> &getFieldArray() const {
    return m_fieldArray;
  }
  inline size_t getFieldCount() const {
    return m_fieldArray.size();
  }
  String toString() const {
    return format(_T("Frame(Id=%d(%s), desc=%s)\n")
                 ,m_frameId, ::toString(getId()).cstr(), m_desc.cstr())
         + indentString(m_fieldArray.toString(_T("\n")),2);
  }
};

class Tag {
private:
  Array<Frame> m_frameArray;
public:
  inline Tag() {
  }
  inline Tag(const ID3_Tag &tag) {
    load(tag);
  }
  void load(const ID3_Tag &tag);
  inline const Array<Frame> &getFrameArray() const {
    return m_frameArray;
  }
  inline size_t getFrameCount() const {
    return m_frameArray.size();
  }
  inline void clear() {
    m_frameArray.clear();
  }
  inline String toString() const {
    return String(_T("Tag:\n")) + indentString(m_frameArray.toString(_T("\n")), 2);
  }
};

#define SELECT_READONLY  0x01
#define SELECT_READWRITE 0x02
#define SELECT_EMPTY     0x04

class MediaFile {
private:
  static GenreMap s_genreMap;

  String            m_sourceURL;
  Tag               m_tag;
  size_t            m_fileSize;
  bool              m_protected;
public:
  MediaFile();
  MediaFile(const String &sourceURL);
  inline size_t  getFileSize() const {
    return m_fileSize;
  }
  void clear();
  inline bool isDefined() const {
    return m_sourceURL.length() > 0;
  }
  inline const String &getSourceURL() const {
    return m_sourceURL;
  }
};

bool operator==(const MediaFile &f1,const MediaFile &f2);
