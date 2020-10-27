#include "stdafx.h"
#include <DebugLog.h>
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
  return EncodedString(ID3TE_NONE, nullptr, 0);
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
  return EncodedString(ID3TE_NONE, nullptr, 0);
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

String FieldWithData::toString(bool hexdump) const {
  String result = format(_T("%-15s:%-10s"), ::toString(getId()).cstr(), ::toString(getType()).cstr());
  switch(getType()) {
  case ID3FTY_INTEGER   :
    result += format(_T(":v=%u"), getInt());
    break;
  case ID3FTY_BINARY    :
    { const ByteArray &ba = getBinData();
      result += format(_T(":size=%zu\n"), ba.size());
      if(hexdump) {
        result += format(_T("\n%s"), createHexdumpString(ba.getData(), ba.size()).cstr());
      }
    }
    break;
  case ID3FTY_TEXTSTRING:
    { const StringField          &f = getStringData();
      const Array<EncodedString> &a = f.getStrings();
      result += format(_T(", encoding:%s, n=%zu"), ::toString(f.getEncoding()).cstr(), a.size());
      result += f.getStrings().toString(HexStringifier<EncodedString>(hexdump), _T(" \n"));
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
  m_spec    = frame.GetSpec();
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
  return nullptr;
}

const FieldWithData *Frame::findFieldByType(ID3_FieldType type) const {
  const size_t n = m_fieldArray.size();
  for(size_t i = 0; i < n; i++) {
    const FieldWithData &f = m_fieldArray[i];
    if(f.getType() == type) {
      return &f;
    }
  }
  return nullptr;
}

String Frame::getTextFieldValue() const {
  const FieldWithData *field = findFieldById(ID3FN_TEXT);
  if(field && (field->getType() == ID3FTY_TEXTSTRING)) {
    const StringField &sf = field->getStringData();
    if(sf.getNumItems() == 1) {
      return sf.getStrings()[0].toString(false);
    }
  }
  return EMPTYSTRING;
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
  return nullptr;
}

UINT Tag::getFrameCount(const String &sourceURL, flags_t flags) { // static
  ID3_Tag tag;

  USES_ACONVERSION;
  char *aname = T2A(sourceURL.cstr());
  tag.Link(aname, flags);
  return tag.NumFrames();
}

MediaFile::MediaFile(const String &sourceURL, flags_t flags) : m_sourceURL(sourceURL) {
  ID3_Tag tag;
  USES_ACONVERSION;
  char *aname = T2A(m_sourceURL.cstr());
  tag.Link(aname, flags);
  m_fileSize = tag.GetFileSize();
  m_tag.load(tag);
}

bool operator==(const MediaFile &mf1,const MediaFile &mf2) {
  return mf1.getSourceURL().equalsIgnoreCase(mf2.getSourceURL());
}

static void setTextFrameValue(ID3_Frame &frame, const EncodedString &es) {
  ID3_Field *intfld = frame.GetField(ID3FN_TEXTENC);
  intfld->Set((uint32)es.getEncoding());
  ID3_Field *txtfld = frame.GetField(ID3FN_TEXT);
  txtfld->SetEncoding(es.getEncoding());
  ByteArray tmp(es);
  tmp.addZeroes(2);
  if(es.getEncoding() == ID3TE_ISO8859_1) {
    txtfld->Set((char*)tmp.getData());
  } else {
    txtfld->Set((unicode_t*)tmp.getData());
  }
}

static void setTextField(ID3_Tag &tag, ID3_FrameID frameId, const String &s) {
  if(s.isEmpty()) return;
  const EncodedString es(s);
  ID3_Frame *frame = new ID3_Frame(frameId);
  tag.AttachFrame(frame);
  setTextFrameValue(*frame, es);
}

MediaFile &MediaFile::removeAllFrames(flags_t flags) {
  ID3_Tag tag;

  USES_ACONVERSION;
  char *aname = T2A(m_sourceURL.cstr());
  tag.Link(aname, flags);
  ID3_Tag::Iterator *tagIt = tag.CreateIterator();
  const ID3_Frame *fr;
  int removeCount = 0;
  while(fr = tagIt->GetNext()) {
    tag.RemoveFrame(fr);
    removeCount++;
  }
  if(removeCount > 0) {
    tag.Update(flags);
  }
  return *this;
}

void MediaFile::updateMobileFrames(const MobileMediaFile &mmf) {
  const UINT frameCount = Tag::getFrameCount(m_sourceURL);
  if(frameCount > 0) {
    _ftprintf(stderr, _T("Skipping %s. Has %u frames\n"), m_sourceURL.cstr(), frameCount);
    return;
  }

  USES_CONVERSION;
  char *aname = T2A(m_sourceURL.cstr());
  ID3_Tag tag;
  tag.Link(aname, ID3TT_PREPENDED);
//  tag.SetSpec()
  // TPE1 */ ID3FID_LEADARTIST,        /**< Lead performer(s)/Soloist(s) */
  // TALB */ ID3FID_ALBUM,             /**< Album/Movie/Show title */
  // TRCK */ ID3FID_TRACKNUM,          /**< Track number/Position in set */
  // TIT2 */ ID3FID_TITLE,             /**< Title/songname/content description */
  // TYER */ ID3FID_YEAR,              /**< Year */
  // TCON */ ID3FID_CONTENTTYPE,       /**< Content type */

  setTextField(tag, ID3FID_LEADARTIST, mmf.getArtist());
  setTextField(tag, ID3FID_ALBUM     , mmf.getAlbum() );
  setTextField(tag, ID3FID_TITLE     , mmf.getTitle() );
  const int track = mmf.getTrack();
  if(track > 0) {
    setTextField(tag, ID3FID_TRACKNUM, format(_T("%d"), track));
  }
  const int year = mmf.getYear();
  if(year > 1900) {
    setTextField(tag, ID3FID_YEAR    , format(_T("%d"), year));
  }
  const String contentType = Tag::s_genreMap.getPackedText(trim(mmf.getContentType()));
  setTextField(tag, ID3FID_CONTENTTYPE, contentType);
  tag.Update(ID3TT_PREPENDED);
}
