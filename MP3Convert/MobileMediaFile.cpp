#include "stdafx.h"
#include "MediaFile.h"

int mobileMediaFileCmp(const MobileMediaFile &f1, const MobileMediaFile &f2) {
  int c = f1.getArtist().compareIgnoreCase(f2.getArtist()); if(c) return c;
      c = f1.getAlbum().compareIgnoreCase( f2.getAlbum() ); if(c) return c;
      c = (int)f1.getTrack() - (int)f2.getTrack();          if(c) return c;
      c = f1.getTitle().compareIgnoreCase( f2.getTitle() ); if(c) return c;
  return f1.getSourceURL().compare(f2.getSourceURL());
}

static UINT strToUint(const String &s) {
  UINT value;
  if(_stscanf(s.cstr(), _T("%u"), &value) != 1) {
    value = 0;
  }
  return value;
}

static String addEscape(const String &s) {
  return String(s).replace('\\', _T("\\")).replace(_T('"'), _T("\\\""));
}

String quoteString(const String &s) {
  return format(_T("\"%s\""), addEscape(s).cstr());
}

static String unQuoteString(const String &s) {
  String tmp(s);
  String result;
  tmp.trim();
  Tokenizer::parseCstring(tmp.cstr(), &result);
  return result;
}

MobileMediaFile::MobileMediaFile(const MediaFile &mf) {
  m_sourceURL    = mf.getSourceURL();
  m_artist       = mf.getTags().getFrameText(ID3FID_LEADARTIST           );
  m_album        = mf.getTags().getFrameText(ID3FID_ALBUM                );
  m_track        = strToUint(mf.getTags().getFrameText(ID3FID_TRACKNUM   ));
  m_title        = mf.getTags().getFrameText(ID3FID_TITLE                );
  m_year         = strToUint(mf.getTags().getFrameText(ID3FID_YEAR       ));
  m_contentType  = Tag::s_genreMap.getPackedText(mf.getTags().getFrameText(ID3FID_CONTENTTYPE          ));
}

MobileMediaFile::MobileMediaFile(const String &line) {
  const intptr_t colonPos = line.find(':');
  if(colonPos < 0) {
    throwException(_T("Invalid input:missing ':'. <%s>"), line.cstr());
  };
  m_sourceURL = left(line, colonPos);
  StringArray a(Tokenizer(substr(line, colonPos+1, 1000), _T(","), 0, TOK_CSTRING));
  if(a.size() != 6) {
    throwException(_T("Invalid input:Expected 6 comma-separated fields. got %zu. <%s>"), a.size(), line.cstr());
  }
  m_artist      = unQuoteString(a[0]);
  m_album       = unQuoteString(a[1]);
  m_track       = strToUint(a[2]);
  m_title       = unQuoteString(a[3]);
  m_year        = strToUint(a[4]);
  m_contentType = unQuoteString(a[5]);
}

#define Q(f) addQuotes ? quoteString(f).cstr() : (f).cstr()

String MobileMediaFile::toString(bool addQuotes) const {
  return format(_T("%-30s:%-40s,%-45s,%2u,%-35s,%4u,%-15s")
               ,getSourceURL().cstr()
               ,Q(m_artist )
               ,Q(m_album  )
               ,m_track
               ,Q(m_title  )
               ,m_year
               ,Q(m_contentType)
               );
};
