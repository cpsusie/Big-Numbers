#include "stdafx.h"
#include "MediaFile.h"

static UINT strToUint(const String &s) {
  UINT value;
  if(_stscanf(s.cstr(), _T("%u"), &value) != 1) {
    value = 0;
  }
  return value;
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

int mobileMediaFileCmp(const MobileMediaFile &f1, const MobileMediaFile &f2) {
  int c = f1.getArtist().compareIgnoreCase(f2.getArtist()); if(c) return c;
      c = f1.getAlbum().compareIgnoreCase( f2.getAlbum() ); if(c) return c;
      c = (int)f1.getTrack() - (int)f2.getTrack();          if(c) return c;
      c = f1.getTitle().compareIgnoreCase( f2.getTitle() ); if(c) return c;
  return f1.getSourceURL().compare(f2.getSourceURL());
}

String addEscape(const String &s) {
  return String(s).replace('\\', _T("\\")).replace(_T('"'), _T("\\\""));
}

String quotedString(const String &s) {
  return format(_T("\"%s\""), addEscape(s).cstr());
}

#define Q(f) addQuotes ? quotedString(f).cstr() : (f).cstr()

String MobileMediaFile::toString(bool addQuotes) const {
  return format(_T("%-40s,%-45s,%2u,%-35s,%4u,%-15s")
               ,Q(m_artist )
               ,Q(m_album  )
               ,m_track
               ,Q(m_title  )
               ,m_year
               ,Q(m_contentType)
               );
};
