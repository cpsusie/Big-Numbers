#include "stdafx.h"
#include "MediaDatabase.h"

GenreMap MediaFile::s_genreMap;

GenreMap::GenreMap() {
  for(int i = 0; i < ID3_NR_OF_V1_GENRES; i++) {
    put(ID3_V1GENRE2DESCRIPTION(i),i);
  }
}

String toString(ID3_FrameID id) {
  switch(id) {
#define CASESTR(id) case ID3FID_##id : return _T(#id);
  CASESTR(NOFRAME            ) /**< No known frame */
  CASESTR(AUDIOCRYPTO        ) /**< Audio encryption */
  CASESTR(PICTURE            ) /**< Attached picture */
  CASESTR(AUDIOSEEKPOINT     ) /**< Audio seek point index */
  CASESTR(COMMENT            ) /**< Comments */
  CASESTR(COMMERCIAL         ) /**< Commercial frame */
  CASESTR(CRYPTOREG          ) /**< Encryption method registration */
  CASESTR(EQUALIZATION2      ) /**< Equalisation (2) */
  CASESTR(EQUALIZATION       ) /**< Equalization */
  CASESTR(EVENTTIMING        ) /**< Event timing codes */
  CASESTR(GENERALOBJECT      ) /**< General encapsulated object */
  CASESTR(GROUPINGREG        ) /**< Group identification registration */
  CASESTR(INVOLVEDPEOPLE     ) /**< Involved people list */
  CASESTR(LINKEDINFO         ) /**< Linked information */
  CASESTR(CDID               ) /**< Music CD identifier */
  CASESTR(MPEGLOOKUP         ) /**< MPEG location lookup table */
  CASESTR(OWNERSHIP          ) /**< Ownership frame */
  CASESTR(PRIVATE            ) /**< Private frame */
  CASESTR(PLAYCOUNTER        ) /**< Play counter */
  CASESTR(POPULARIMETER      ) /**< Popularimeter */
  CASESTR(POSITIONSYNC       ) /**< Position synchronisation frame */
  CASESTR(BUFFERSIZE         ) /**< Recommended buffer size */
  CASESTR(VOLUMEADJ2         ) /**< Relative volume adjustment (2) */
  CASESTR(VOLUMEADJ          ) /**< Relative volume adjustment */
  CASESTR(REVERB             ) /**< Reverb */
  CASESTR(SEEKFRAME          ) /**< Seek frame */
  CASESTR(SIGNATURE          ) /**< Signature frame */
  CASESTR(SYNCEDLYRICS       ) /**< Synchronized lyric/text */
  CASESTR(SYNCEDTEMPO        ) /**< Synchronized tempo codes */
  CASESTR(ALBUM              ) /**< Album/Movie/Show title */
  CASESTR(BPM                ) /**< BPM (beats per minute) */
  CASESTR(COMPOSER           ) /**< Composer */
  CASESTR(CONTENTTYPE        ) /**< Content type */
  CASESTR(COPYRIGHT          ) /**< Copyright message */
  CASESTR(DATE               ) /**< Date */
  CASESTR(ENCODINGTIME       ) /**< Encoding time */
  CASESTR(PLAYLISTDELAY      ) /**< Playlist delay */
  CASESTR(ORIGRELEASETIME    ) /**< Original release time */
  CASESTR(RECORDINGTIME      ) /**< Recording time */
  CASESTR(RELEASETIME        ) /**< Release time */
  CASESTR(TAGGINGTIME        ) /**< Tagging time */
  CASESTR(INVOLVEDPEOPLE2    ) /**< Involved people list */
  CASESTR(ENCODEDBY          ) /**< Encoded by */
  CASESTR(LYRICIST           ) /**< Lyricist/Text writer */
  CASESTR(FILETYPE           ) /**< File type */
  CASESTR(TIME               ) /**< Time */
  CASESTR(CONTENTGROUP       ) /**< Content group description */
  CASESTR(TITLE              ) /**< Title/songname/content description */
  CASESTR(SUBTITLE           ) /**< Subtitle/Description refinement */
  CASESTR(INITIALKEY         ) /**< Initial key */
  CASESTR(LANGUAGE           ) /**< Language(s) */
  CASESTR(SONGLEN            ) /**< Length */
  CASESTR(MUSICIANCREDITLIST ) /**< Musician credits list */
  CASESTR(MEDIATYPE          ) /**< Media type */
  CASESTR(MOOD               ) /**< Mood */
  CASESTR(ORIGALBUM          ) /**< Original album/movie/show title */
  CASESTR(ORIGFILENAME       ) /**< Original filename */
  CASESTR(ORIGLYRICIST       ) /**< Original lyricist(s)/text writer(s) */
  CASESTR(ORIGARTIST         ) /**< Original artist(s)/performer(s) */
  CASESTR(ORIGYEAR           ) /**< Original release year */
  CASESTR(FILEOWNER          ) /**< File owner/licensee */
  CASESTR(LEADARTIST         ) /**< Lead performer(s)/Soloist(s) */
  CASESTR(BAND               ) /**< Band/orchestra/accompaniment */
  CASESTR(CONDUCTOR          ) /**< Conductor/performer refinement */
  CASESTR(MIXARTIST          ) /**< Interpreted, remixed, or otherwise modified by */
  CASESTR(PARTINSET          ) /**< Part of a set */
  CASESTR(PRODUCEDNOTICE     ) /**< Produced notice */
  CASESTR(PUBLISHER          ) /**< Publisher */
  CASESTR(TRACKNUM           ) /**< Track number/Position in set */
  CASESTR(RECORDINGDATES     ) /**< Recording dates */
  CASESTR(NETRADIOSTATION    ) /**< Internet radio station name */
  CASESTR(NETRADIOOWNER      ) /**< Internet radio station owner */
  CASESTR(SIZE               ) /**< Size */
  CASESTR(ALBUMSORTORDER     ) /**< Album sort order */
  CASESTR(PERFORMERSORTORDER ) /**< Performer sort order */
  CASESTR(TITLESORTORDER     ) /**< Title sort order */
  CASESTR(ISRC               ) /**< ISRC (international standard recording code) */
  CASESTR(ENCODERSETTINGS    ) /**< Software/Hardware and settings used for encoding */
  CASESTR(SETSUBTITLE        ) /**< Set subtitle */
  CASESTR(USERTEXT           ) /**< User defined text information */
  CASESTR(YEAR               ) /**< Year */
  CASESTR(UNIQUEFILEID       ) /**< Unique file identifier */
  CASESTR(TERMSOFUSE         ) /**< Terms of use */
  CASESTR(UNSYNCEDLYRICS     ) /**< Unsynchronized lyric/text transcription */
  CASESTR(WWWCOMMERCIALINFO  ) /**< Commercial information */
  CASESTR(WWWCOPYRIGHT       ) /**< Copyright/Legal infromation */
  CASESTR(WWWAUDIOFILE       ) /**< Official audio file webpage */
  CASESTR(WWWARTIST          ) /**< Official artist/performer webpage */
  CASESTR(WWWAUDIOSOURCE     ) /**< Official audio source webpage */
  CASESTR(WWWRADIOPAGE       ) /**< Official internet radio station homepage */
  CASESTR(WWWPAYMENT         ) /**< Payment */
  CASESTR(WWWPUBLISHER       ) /**< Official publisher webpage */
  CASESTR(WWWUSER            ) /**< User defined URL link */
  CASESTR(METACRYPTO         ) /**< Encrypted meta frame (id3v2.2.x) */
  CASESTR(METACOMPRESSION    ) /**< Compressed meta frame (id3v2.2.1) */
  CASESTR(LASTFRAMEID        )  /**< Last field placeholder */
  default:return format(_T("Unknown frameId:%d"), id);
  }
#undef CASESTR
}

String toString(ID3_FieldID id) {
#define CASESTR(id) case ID3FN_##id: return _T(#id);
  switch(id) {
  CASESTR(NOFIELD         ) /**< No field */
  CASESTR(TEXTENC         ) /**< Text encoding (unicode or ASCII) */
  CASESTR(TEXT            ) /**< Text field */
  CASESTR(URL             ) /**< A URL */
  CASESTR(DATA            ) /**< Data field */
  CASESTR(DESCRIPTION     ) /**< Description field */
  CASESTR(OWNER           ) /**< Owner field */
  CASESTR(EMAIL           ) /**< Email field */
  CASESTR(RATING          ) /**< Rating field */
  CASESTR(FILENAME        ) /**< Filename field */
  CASESTR(LANGUAGE        ) /**< Language field */
  CASESTR(PICTURETYPE     ) /**< Picture type field */
  CASESTR(IMAGEFORMAT     ) /**< Image format field */
  CASESTR(MIMETYPE        ) /**< Mimetype field */
  CASESTR(COUNTER         ) /**< Counter field */
  CASESTR(ID              ) /**< Identifier/Symbol field */
  CASESTR(VOLUMEADJ       ) /**< Volume adjustment field */
  CASESTR(NUMBITS         ) /**< Number of bits field */
  CASESTR(VOLCHGRIGHT     ) /**< Volume chage on the right channel */
  CASESTR(VOLCHGLEFT      ) /**< Volume chage on the left channel */
  CASESTR(PEAKVOLRIGHT    ) /**< Peak volume on the right channel */
  CASESTR(PEAKVOLLEFT     ) /**< Peak volume on the left channel */
  CASESTR(TIMESTAMPFORMAT ) /**< SYLT Timestamp Format */
  CASESTR(CONTENTTYPE     ) /**< SYLT content type */
  CASESTR(LASTFIELDID     ) /**< Last field placeholder */
  default:return format(_T("Unknown fieldId:%d"), id);
}
#undef CASESTR
}

String toString(ID3_FieldType type) {
#define CASESTR(type) case ID3FTY_##type: return _T(#type);
  switch(type) {
  CASESTR(NONE      )
  CASESTR(INTEGER   )
  CASESTR(BINARY    )
  CASESTR(TEXTSTRING)
  CASESTR(NUMTYPES  )
  default:return format(_T("Unknown fieldType:%d"), type);
  }
#undef CASESTR
}

String toString(ID3_TextEnc enc) {
#define CASESTR(enc) case ID3TE_##enc: return _T(#enc);
  switch(enc) {
  CASESTR(NONE        );
  CASESTR(UTF16       );
  CASESTR(UTF16BE     );
  CASESTR(UTF8        );
  CASESTR(NUMENCODINGS);
  CASESTR(ASCII       );
  default:return format(_T("Unknown encoding:%d"), enc);
  }
}
