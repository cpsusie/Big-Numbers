#include "stdafx.h"
#include "MediaDatabase.h"

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

void Tag::load(const ID3_Tag &tag) {
  m_frameArray.clear();
  ID3_Tag::ConstIterator *tagIt = tag.CreateIterator();
  const ID3_Frame *fr;
  while(fr = tagIt->GetNext()) {
    m_frameArray.add(Frame(*fr));
  }
}

MediaFile::MediaFile(const String &sourceURL) : m_sourceURL(sourceURL) {
  ID3_Tag tag;
  USES_ACONVERSION;
  char *aname = T2A(m_sourceURL.cstr());
  tag.Link(aname);
  m_fileSize = tag.GetFileSize();
  m_tag.load(tag);
  _tprintf(_T("%s"), m_tag.toString().cstr());
}

bool operator==(const MediaFile &mf1,const MediaFile &mf2) {
  return mf1.getSourceURL().equalsIgnoreCase(mf2.getSourceURL());
}
