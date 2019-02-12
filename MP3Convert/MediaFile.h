#pragma once

#include <HashMap.h>
#include <ByteStream.h>
#include <Date.h>
#include "mp3tag.h"
#include "EncodedString.h"

class GenreMap : public StringIHashMap<int> {
private:
  // return true, if str is in format "(<ddd>)", and set index=ddd; else return false;
  static inline bool isIndexStr(const String &str, int &index) {
    return _stscanf(str.cstr(), _T("(%u)"), &index) == 1;
  }
  // return index, if str exist as display text, comparing ignoring case. else return -1
  inline int  getIndex(const String &genre) const {
    const int *result = __super::get(genre);
    return result ? *result : -1;
  }
public:
  GenreMap();
  // if(isIndexStr(str, index) && ISVALIDGENREINDEX(index)) then return ID3_V1GENRE2DESCRIPTION(index); else return str
  String getDisplayText(const String &str) const;
  // if(isIndexStr(str, index) || ((index = getIndex(str)) && ISVALIDGENREINDEX(index)) then return "(<index>)"; else return str;
  String getPackedText( const String &str) const;
};

template<class T> class HexStringifier : public AbstractStringifier<T> {
private:
  const bool m_hexdump;
public:
  HexStringifier(bool hexdump) : m_hexdump(hexdump) {
  }
  String toString(const T &e) {
    return e.toString(m_hexdump);
  };
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
  String toString(bool hexdump) const;
};

class Frame {
private:
  ID3_FrameID          m_frameId;
  String               m_desc;
  ID3_V2Spec           m_spec;
  Array<FieldWithData> m_fieldArray;
public:
  Frame(const ID3_Frame &frame);
  inline ID3_FrameID getId() const {
    return m_frameId;
  }
  inline const String &getDescription() const {
    return m_desc;
  }
  inline ID3_V2Spec getSpec() const {
    return m_spec;
  }
  inline const Array<FieldWithData> &getFieldArray() const {
    return m_fieldArray;
  }
  inline size_t getFieldCount() const {
    return m_fieldArray.size();
  }
  const FieldWithData *findFieldById(  ID3_FieldID   id)   const;
  const FieldWithData *findFieldByType(ID3_FieldType type) const;

  String toString(bool hexdump) const {
    return format(_T("Frame(Id=%d(%s), desc=%s, spec=%s)\n")
                 ,m_frameId, ::toString(getId()).cstr(), m_desc.cstr(), ::toString(m_spec).cstr())
         + indentString(m_fieldArray.toString(HexStringifier<FieldWithData>(hexdump),_T("\n ")), 3);
  }
  // return value of first field with getId == ID3FN_TEXT if any; else EMPTYSTRING
  String getTextFieldValue() const;
  // return value of first field with getType == ID3FTY_BINARY if any; else NULL
  inline const ByteArray *getBinaryFieldValue() const {
    const FieldWithData *field = findFieldByType(ID3FTY_BINARY);
    return field ? &field->getBinData() : NULL;
  }
};

class Tag {
private:
  Array<Frame> m_frameArray;
  ID3_V2Spec   m_spec;

public:
  static const GenreMap s_genreMap;
  inline Tag() {
  }
  inline Tag(const ID3_Tag &tag) {
    m_spec = tag.GetSpec();
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
  const Frame *getFrame(ID3_FrameID id) const;

  inline String getFrameText(ID3_FrameID id) const {
    const Frame *frame = getFrame(id);
    return frame ? frame->getTextFieldValue() : EMPTYSTRING;
  }
  inline const ByteArray *getFrameBinary(ID3_FrameID id) const {
    const Frame *frame = getFrame(id);
    return frame ? frame->getBinaryFieldValue() : NULL;
  }
  static UINT getFrameCount(const String &sourceURL, flags_t flags = (flags_t)ID3TT_ALL);
  inline String toString(bool hexdump) const {
    return format(_T("Tag. Spec:%s\n"), ::toString(m_spec).cstr())
         + indentString(m_frameArray.toString(HexStringifier<Frame>(hexdump),_T("\n ")), 3);
  }
};

#define SELECT_READONLY  0x01
#define SELECT_READWRITE 0x02
#define SELECT_EMPTY     0x04

class MobileMediaFile;

class MediaFile {
private:
  String            m_sourceURL;
  Tag               m_tag;
  size_t            m_fileSize;
  bool              m_protected;
public:
  MediaFile();
  MediaFile(const String &sourceURL, flags_t flags = (flags_t)ID3TT_ALL);
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
  inline const Tag &getTags() const {
    return m_tag;
  }
  inline String toString(bool hexdump) const {
    return format(_T("%s\n%s"), getSourceURL().cstr(), getTags().toString(hexdump).cstr());
  }

  MediaFile &removeAllFrames(flags_t flags = (flags_t)ID3TT_ALL);
  void updateMobileFrames(const MobileMediaFile &mmf);
};

bool operator==(const MediaFile &f1,const MediaFile &f2);

// -----------------------------------------------------------------------------------------

typedef enum {
  TAG_ARTIST
 ,TAG_ALBUM
 ,TAG_TRACK
 ,TAG_TITLE
 ,TAG_YEAR
 ,TAG_CONTENTTYPE
 ,TAG_LASTFIELD = TAG_CONTENTTYPE
} MobileMediaField;

class MobileMediaFile {
private:
  String            m_sourceURL;
  String            m_artist;
  String            m_album;
  UINT              m_track;
  String            m_title;
  UINT              m_year;
  String            m_contentType;
  static const UINT s_defaultColumnWidth[6];
public:
  MobileMediaFile(const MediaFile &mf);
  MobileMediaFile(const String &line);
  const String &getSourceURL() const {
    return m_sourceURL;
  }
  const String &getArtist() const {
    return m_artist;
  }
  const String &getAlbum() const {
    return m_album;
  }
  UINT getTrack() const {
    return m_track;
  }
  const String &getTitle() const {
    return m_title;
  }
  UINT getYear() const {
    return m_year;
  }
  const String &getContentType() const {
    return m_contentType;
  }
  String buildPath() const;
  String toString(MobileMediaField field, bool addQuotes) const;
  String toString(bool addQuotes=true, const UINT columnWidth[6] = s_defaultColumnWidth) const;
  static const UINT *getDefaultColumnWidth() {
    return s_defaultColumnWidth;
  }
};

class MobileMediaFileComparator : public Comparator<MobileMediaFile> {
private:
  CompactArray<MobileMediaField> m_compareOrder;
public:
  void setDefault();
  inline void clear() {
    m_compareOrder.clear();
  }
  inline bool isEmpty() const {
    return m_compareOrder.isEmpty();
  }
  inline void addField(MobileMediaField f) {
    m_compareOrder.add(f);
  }
  int compare(const MobileMediaFile &e1, const MobileMediaFile &e2);
  AbstractComparator *clone() const {
    return new MobileMediaFileComparator(*this);
  }
};
