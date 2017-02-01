#include "stdafx.h"
#include "mp3tag.h"

GenreMap MediaFile::m_genreMap;

GenreMap::GenreMap() {
#ifdef HAS_ID3TAGS
  for(int i = 0; i < ID3_NR_OF_V1_GENRES; i++) {
    put(toLowerCase(ID3_V1GENRE2DESCRIPTION(i)),i);
  }
#endif
}

int *GenreMap::get(const String &genre) {
  return StringHashMap<int>::get(toLowerCase(genre));
}

void MediaFile::update(const AttributeArray &attributes) {
  bool changed = false;
  CWMPMedia media = getMedia();
  for(size_t i = 0; i < attributes.size(); i++) {
    const MediaAttribute &attr = attributes[i];
    if(attr.isReadOnly()) {
      continue;
    }
    CString old = media.getItemInfo(attr.getName());
    if(_tcscmp(attr.getValue(), old) != 0) {
      media.setItemInfo(attr.getName(), attr.getValue());
      changed = true;
    }
  }
  if(changed) {
    init();
    MediaDatabase::update(*this);
  }
}

CWMPMedia MediaFile::getMedia() const {
  if(m_media == NULL) {
    if(m_sourceURL.length() != 0) {
      CWMPPlaylist list = MediaDatabase::getMediaCollection().getByAttribute(_T("SourceURL"), m_sourceURL.cstr());
      if(list.GetCount() > 0) {
        m_media = list.GetItem(0);
      } else {
        m_media = MediaDatabase::getMediaCollection().add(m_sourceURL.cstr());
      }
    }
    if(m_media == NULL) {
      throwException(_T("Error reading %s"), m_sourceURL.cstr());
    }
  }
  return m_media;
}

MediaFile::MediaFile(const TCHAR *fileName, time_t modified) {
  m_sourceURL = fileName;

  init();

  m_modified  = modified;
  m_hasPlayed = false;
}

MediaFile::MediaFile() {
  m_media       = NULL;
  m_modified    = 0;
  m_duration    = 0;
  m_protected   = true;
  m_trackNumber = 0;
  m_hasPlayed   = false;
}

MediaFile::MediaFile(CWMPMedia media) {
  m_media     = media;
  m_sourceURL = media.GetSourceURL();
  struct _stat st;
  if(stat(m_sourceURL,st) == 0) {
    m_modified = st.st_mtime;
  } else {
    m_modified = 0;
  }
  m_hasPlayed = false;
  init();
}

void MediaFile::clear() {
  m_media       = NULL;
  m_sourceURL   = _T("");
  m_title       = _T("");
  m_artist      = _T("");
  m_composer    = _T("");
  m_genre       = _T("");
  m_album       = _T("");
  m_duration    = 0;
  m_protected   = true;
  m_trackNumber = 0;
  m_hasPlayed   = false;
}

static int countAlphaNum(const String &s) {
  int count = 0;
  for(const _TUCHAR *cp = (_TUCHAR*)s.cstr(); *cp; cp++) {
    if(isalnum(*cp)) {
      count++;
    }
  }
  return count;
}

static bool isGarbage(const String &s) {
  const size_t len = s.length();
  return (len == 0) || ((double)countAlphaNum(s)/len < 0.2);
}

void MediaFile::init() {
  CWMPMedia media = getMedia();
  m_title    = media.getItemInfo(_T("Title"));
  m_artist   = media.getItemInfo(_T("WM/AlbumArtist"));
  m_composer = media.getItemInfo(_T("Author"));
  if(isGarbage(m_composer)) {
    m_composer = media.getItemInfo(_T("WM/Composer"));
  }
  if(isGarbage(m_artist)) {
    m_artist = media.getItemInfo(_T("DisplayArtist"));
    if(isGarbage(m_artist)) {
      if(!isGarbage(m_composer)) {
        m_artist = m_composer;
      } else {
        FileNameSplitter info(m_sourceURL);
        String   last = info.getFileName();
        TCHAR    ch   = _T('-');
        intptr_t dash = last.find(ch);
        if(dash < 0) {
          ch   = _T('_');
          dash = last.find(ch);
        }
        if(dash < 0) {
          m_title  = last;
        } else {
          m_title  = trim(substr(last,dash+1,100)).replace(ch, _T(' '));
          m_artist = trim(substr(last,0,dash)).replace(ch,_T(' '));
        }
      }
    }
  }
  m_genre     = media.getItemInfo(_T("WM/Genre"));
  m_album     = media.getItemInfo(_T("WM/AlbumTitle"));
  m_duration  = media.GetDuration();
  m_protected = media.getItemInfo(_T("is_protected"))==_T("True");
  const CString track = media.getItemInfo(_T("WM/TrackNumber"));
  if(track.GetLength() == 0 || _stscanf((LPCTSTR)track,_T("%hd"),&m_trackNumber) != 1) {
    m_trackNumber = 0;
  }
}

AttributeArray MediaFile::getAttributes(int flags) const {
  AttributeArray result;

  CWMPMedia media = getMedia();
  if(media == NULL) {
    return result;
  }

  const long n = media.GetAttributeCount();
  for(long i = 0; i < n; i++) {
    const CString name     = media.getAttributeName(i);
    const bool    readOnly = media.isReadOnlyItem(name) ? true : false;
    if(readOnly && ((flags & SELECT_READONLY) == 0)) {
      continue;
    }
    if((!readOnly) && ((flags & SELECT_READWRITE) == 0)) {
      continue;
    }
    const CString value = media.getItemInfo(name);
    if(((flags & SELECT_EMPTY) == 0) && value.GetLength() == 0) {
      continue;
    }

    result.add(MediaAttribute((LPCTSTR)name, (LPCTSTR)value, readOnly));
  }
  return result;
}

UINT MediaFile::getFileSize() const {
  CString v = getMedia().getItemInfo(_T("FileSize"));
  UINT result = 0;
  _stscanf((LPCTSTR)v, _T("%lu"), &result);
  return result;
}

String MediaFile::getDurationString() const {
  int isec = (int)m_duration;
  if(isec >= 3600) {
    return format(_T("%d:%02d:%02d"),isec / 3600, (isec % 3600) / 60,isec % 60);
  } else {
    return format(_T("%02d:%02d"),isec / 60,isec % 60);
  }
}

void MediaFile::read(ByteInputStream &s, String &str) { // static
  UINT l;
  s.getBytesForced((BYTE*)&l, sizeof(l));
  if(l == 0) {
    str = _T("");
    return;
  }

  TCHAR tmp[4000];
  TCHAR *buf = tmp;
  if(l >= ARRAYSIZE(tmp)) {
    buf = new TCHAR[l+1];
  }
  try {
    s.getBytesForced((BYTE*)buf, l*sizeof(TCHAR));
    buf[l] = _T('\0');
    str = buf;
    if(buf != tmp) {
      delete[] buf;
    }
  } catch(...) {
    if(buf != tmp) {
      delete[] buf;
    }
    throw;
  }
}

void MediaFile::write(ByteOutputStream &s, const String &str) { // static
  const UINT l = (UINT)str.length();
  s.putBytes((BYTE*)&l, sizeof(l));
  if(l == 0) {
    return;
  }
  s.putBytes((BYTE*)str.cstr(), l*sizeof(TCHAR));
}

void MediaFile::read(ByteInputStream &s) {
  read(s, m_sourceURL);
  read(s, m_title    );
  read(s, m_artist   );
  read(s, m_album    );
  read(s, m_composer );
  read(s, m_genre    );

  s.getBytesForced((BYTE*)&m_trackNumber, sizeof(m_trackNumber));
  s.getBytesForced((BYTE*)&m_duration   , sizeof(m_duration   ));
  s.getBytesForced((BYTE*)&m_protected  , sizeof(m_protected  ));
  s.getBytesForced((BYTE*)&m_modified   , sizeof(m_modified   ));
}

void MediaFile::write(ByteOutputStream &s) const {
  write(s, m_sourceURL);
  write(s, m_title    );
  write(s, m_artist   );
  write(s, m_album    );
  write(s, m_composer );
  write(s, m_genre    );

  s.putBytes((BYTE*)&m_trackNumber, sizeof(m_trackNumber));
  s.putBytes((BYTE*)&m_duration   , sizeof(m_duration   ));
  s.putBytes((BYTE*)&m_protected  , sizeof(m_protected  ));
  s.putBytes((BYTE*)&m_modified   , sizeof(m_modified   ));
}

bool operator==(const MediaFile &mf1,const MediaFile &mf2) {
  return _tcsicmp(mf1.getSourceURL(),mf2.getSourceURL()) == 0;
}

int MediaFileWithQueueId::idCounter = 0;

AttributeArray::AttributeArray() {
  m_maxNameLength = m_maxValueLength = 0;
}

void AttributeArray::refreshMaxWidth() {
}

void AttributeArray::add(const MediaAttribute &attr) {
  Array<MediaAttribute>::add(attr);
  int l = (int)attr.m_name.length();
  if(l > m_maxNameLength) {
    m_maxNameLength = l;
  }
  l = (int)attr.m_value.length();
  if(l > m_maxValueLength) {
    m_maxValueLength = l;
  }
}

int AttributeArray::findByName(const String &name) const {
  for(size_t i = 0; i < size(); i++) {
    if(name.equalsIgnoreCase((*this)[i].getName())) {
      return (int)i;
    }
  }
  return -1;
}

void AttributeArray::removeAttribute(const String &name) {
  for(size_t i = 0; i < size(); i++) {
    MediaAttribute &attr = (*this)[i];
    if(name.equalsIgnoreCase(attr.getName())) {
      removeIndex(i);
      if(attr.m_name.length() == m_maxNameLength || attr.m_value.length() == m_maxValueLength) {
        refreshMaxWidth();
      }
      return;
    }
  }
}

MediaAttribute::MediaAttribute(const TCHAR *name, const TCHAR *value, bool readOnly)
: m_name(name)
, m_value(value)
, m_readOnly(readOnly)
{
}

void MediaAttribute::setValue(const String &value) {
  if(m_readOnly) {
    return;
  }
  m_value = value;
}

ULONG MediaAttribute::hashCode() const {
  return m_name.hashCode() + m_value.hashCode() + (m_readOnly ? 113 : 117);
}

bool operator==(const MediaAttribute &a1, const MediaAttribute &a2) {
  return (a1.m_value == a2.m_value) && (a1.m_name == a2.m_name);
}

bool operator!=(const MediaAttribute &a1, const MediaAttribute &a2) {
  return !(a1 == a2);
}
