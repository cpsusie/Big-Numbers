#include "stdafx.h"
#include "MediaFile.h"

void MediaFile::clear() {
  m_sourceURL = EMPTYSTRING;
  m_tag.clear();
}

static int countAlphaNum(const String &s) {
  int count = 0;
  for(const _TUCHAR *cp = (_TUCHAR*)s.cstr(); *cp; cp++) {
    if(_istalnum(*cp)) {
      count++;
    }
  }
  return count;
}

static EncodedString getStringFieldTextItem(const ID3_Field &field, size_t index) {
  const ID3_TextEnc encoding = field.GetEncoding();
  switch(encoding ) {
  case ID3TE_UTF8         :
  case ID3TE_UTF16        :
  case ID3TE_UTF16BE      :
    { unicode_t buf[1024]; memset(buf, 0, sizeof(buf));
      const size_t nc = field.Get(buf, ARRAYSIZE(buf), index);
      return EncodedString(encoding, (BYTE*)buf, nc * sizeof(buf[0]));
    }
    break;
  case ID3TE_ASCII        :
    { char buf[1024]; memset(buf, 0, sizeof(buf));
      const size_t nc = field.Get(buf, ARRAYSIZE(buf), index);
      return EncodedString(encoding, (BYTE*)buf, nc * sizeof(buf[0]));
    }
    break;
  }
  return EncodedString(ID3TE_NONE, NULL, 0);
}

static EncodedString getStringFieldText(const ID3_Field &field) {
  const ID3_TextEnc encoding = field.GetEncoding();
  switch (encoding) {
  case ID3TE_UTF8   :
  case ID3TE_UTF16  :
  case ID3TE_UTF16BE:
    { unicode_t buf[1024]; memset(buf, 0, sizeof(buf));
      const size_t nc = field.Get(buf, ARRAYSIZE(buf));
      return EncodedString(encoding, (BYTE*)buf, nc);
    }
    break;
  case ID3TE_ASCII:
    { char buf[1024]; memset(buf, 0, sizeof(buf));
      const size_t nc = field.Get(buf, ARRAYSIZE(buf));
      return EncodedString(encoding, (BYTE*)buf, nc * sizeof(buf[0]));
    }
    break;
  }
  return EncodedString(ID3TE_NONE, NULL, 0);
}

StringField::StringField(const ID3_Field *field) 
: m_encoding(field->GetEncoding())
{
  const size_t nItems = field->GetNumTextItems();
  if(nItems == 1) {
    m_esa.add(getStringFieldText(*field));
  } else {
    for(size_t i = 0; i < nItems; i++) {
      m_esa.add(getStringFieldTextItem(*field, i));
    }
  }
}

String StringField::toString() const {
  const size_t n = getNumItems();
  switch(n) {
  case 0: return EMPTYSTRING;
  case 1:
    return format(_T("\"%s\""), getStrings()[0].toString().cstr());
  default:
    { const Array<EncodedString> &ea = getStrings();
      String result;
      for(size_t i = 0; i < ea.size(); i++) {
        result += format(_T("\"%s\"\n"), ea[i].toString().cstr());
      }
      return result;
    }
  }
}

void FieldWithData::allocate(const ID3_Field *field) {
  m_fieldId   = field->GetID();
  m_fieldType = field->GetType();
  switch(m_fieldType) {
  case ID3FTY_INTEGER   :
    m_int32 = field->Get();
    break;
  case ID3FTY_BINARY    :
    m_binary = new ByteArray(field->GetRawBinary(), field->BinSize());
    break;
  case ID3FTY_TEXTSTRING:
    m_stringField = new StringField(field);
    break;
  case ID3FTY_NUMTYPES  :
  default               :
    fprintf(stderr, "Cannot yet get fields of type %d (fieldId:%d)", m_fieldType, field->GetID());
    break;
  }
}

void FieldWithData::allocate(const FieldWithData &field) {
  m_fieldId   = field.getId();
  m_fieldType = field.getType();
  switch(m_fieldType) {
  case ID3FTY_INTEGER   :
    m_int32 = field.getInt();
    break;
  case ID3FTY_BINARY    :
    m_binary = new ByteArray(field.getBinData());
    break;
  case ID3FTY_TEXTSTRING:
    m_stringField = new StringField(field.getStringData());
    break;
  case ID3FTY_NUMTYPES  :
  default               :
    /* ignore */
    ;
  }
}

FieldWithData::~FieldWithData() {
  cleanup();
}

void FieldWithData::cleanup() {
  switch(getType()) {
  case ID3FTY_INTEGER  :
    break;
  case ID3FTY_BINARY   :
    delete m_binary;
    break;
  case ID3FTY_TEXTSTRING:
    delete m_stringField;
    break;
  case ID3FTY_NUMTYPES :
  default              :
    /* ignore */
    ;
  }
}

FieldWithData &FieldWithData::operator=(const FieldWithData &src) {
  if(this == &src) {
    return *this;
  }
  cleanup();
  allocate(src);
  return *this;
}

String FieldWithData::toString() const {
  String result = format(_T("%-15s:%-10s"), ::toString(getId()).cstr(), ::toString(getType()).cstr());
  switch(getType()) {
  case ID3FTY_INTEGER   :
    result += format(_T(":v=%u"), getInt());
    break;
  case ID3FTY_BINARY    :
    result += format(_T(":size=%zu"), getBinData().size());
    break;
  case ID3FTY_TEXTSTRING:
    { const StringField &f = getStringData();
      const size_t       n = f.getNumItems();
      result += format(_T(", encoding:%s, n=%zu"), ::toString(f.getEncoding()).cstr(), n);
      if(n <= 1) {
        result += _T(", ");
        result += f.toString();
      } else {
        result += _T("\n");
        result += indentString(f.toString(), 10);
      }
    }
    break;
  case ID3FTY_NUMTYPES  :
  default               :
    break;
  }
  return result;
}

Frame::Frame(const ID3_Frame &frame) {
  m_frameId = frame.GetID();
  m_desc    = frame.GetDescription();
  ID3_Frame::ConstIterator *it = frame.CreateIterator();
  const ID3_Field *field;
  while(field = it->GetNext()) {
    m_fieldArray.add(FieldWithData(field));
  }
};

const FieldWithData *Frame::findFieldById(ID3_FieldID id) const {
  const size_t n = m_fieldArray.size();
  for(size_t i = 0; i < n; i++) {
    const FieldWithData &f = m_fieldArray[i];
    if(f.getId() == id) {
      return &f;
    }
  }
  return NULL;
}

const FieldWithData *Frame::findFieldByType(ID3_FieldType type) const {
  const size_t n = m_fieldArray.size();
  for(size_t i = 0; i < n; i++) {
    const FieldWithData &f = m_fieldArray[i];
    if(f.getType() == type) {
      return &f;
    }
  }
  return NULL;
}

const GenreMap Tag::s_genreMap;

void Tag::load(const ID3_Tag &tag) {
  m_frameArray.clear();
  ID3_Tag::ConstIterator *tagIt = tag.CreateIterator();
  const ID3_Frame *fr;
  while(fr = tagIt->GetNext()) {
    m_frameArray.add(Frame(*fr));
  }
}

const Frame *Tag::getFrame(ID3_FrameID id) const {
  const size_t n = m_frameArray.size();
  for(size_t i = 0; i < n; i++) {
    const Frame &frame = m_frameArray[i];
    if(frame.getId() == id) {
      return &frame;
    }
  }
  return NULL;
}

typedef struct {
  ID3_FrameID m_id;
  int         m_len;
} FrameIdTextField;

String Tag::toStringMobileTags() const {
  static const FrameIdTextField frameIdArray[] = {
    ID3FID_LEADARTIST  , 40
   ,ID3FID_ALBUM       , 45
   ,ID3FID_TRACKNUM    , 2
   ,ID3FID_TITLE       , 35
   ,ID3FID_YEAR        , 4
   ,ID3FID_CONTENTTYPE , 15
  };
  String result;
  const TCHAR *delim = NULL;
  for(size_t i = 0; i < ARRAYSIZE(frameIdArray); i++) {
    const FrameIdTextField &ftf = frameIdArray[i];
    const Frame *frame = getFrame(ftf.m_id);
    String text;
    if(frame) {
      const FieldWithData *field = frame->findFieldById(ID3FN_TEXT);
      if(field && (field->getType() == ID3FTY_TEXTSTRING)) {
        const StringField &sf = field->getStringData();
        if(sf.getNumItems() == 1) {
          text = sf.getStrings()[0].toString();
          if((ftf.m_id == ID3FID_CONTENTTYPE)) {
            text = s_genreMap.getDisplayText(text);
          }
        }
      }
    }
    if(delim) result += delim; else delim = _T(",");
    result += format(_T("%-*s"), ftf.m_len, text.cstr());
  }
  return result;
}

MediaFile::MediaFile(const String &sourceURL) : m_sourceURL(sourceURL) {
  ID3_Tag tag;
  USES_ACONVERSION;
  char *aname = T2A(m_sourceURL.cstr());
  tag.Link(aname);
  m_fileSize = tag.GetFileSize();
  m_tag.load(tag);
}

bool operator==(const MediaFile &mf1,const MediaFile &mf2) {
  return mf1.getSourceURL().equalsIgnoreCase(mf2.getSourceURL());
}
