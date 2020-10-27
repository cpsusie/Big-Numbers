#include "stdafx.h"

#if defined(HAVE_CONFIG_H)
#  include <config.h>
#endif

//#include <iostream.h>
#include <id3/tag.h>
#include <id3/misc_support.h>
#include <id3/field.h>
#include <stdio.h>

const char *gettag(ID3_Tag &tag, ID3_FrameID id) {
  ID3_Frame *frame = tag.Find(id);
  if(frame != nullptr)
    return frame->GetField(ID3FN_TEXT)->GetRawText();
  else
    return "";
}

static void settag(ID3_Tag &tag, ID3_FrameID id, const char *value) {
  ID3_Frame* frame = tag.Find(id);
  if(frame == nullptr) {
    ID3_Frame newframe;
    newframe.SetID(id);
    tag.AddFrame(newframe);
    frame = tag.Find(id);
  }
  *frame->GetField(ID3FN_TEXT) = value;
}

typedef struct {
  ID3_FrameID  tag;
  char        *tagname;
} fieldtag;
#define FIELDTAG(f) f,#f

fieldtag fieldlist[] = {
  FIELDTAG(ID3FID_AUDIOCRYPTO),
  FIELDTAG(ID3FID_PICTURE),
  FIELDTAG(ID3FID_COMMENT),
  FIELDTAG(ID3FID_COMMERCIAL),
  FIELDTAG(ID3FID_CRYPTOREG),
  FIELDTAG(ID3FID_EQUALIZATION),
  FIELDTAG(ID3FID_EVENTTIMING),
  FIELDTAG(ID3FID_GENERALOBJECT),
  FIELDTAG(ID3FID_GROUPINGREG),
  FIELDTAG(ID3FID_INVOLVEDPEOPLE),
  FIELDTAG(ID3FID_LINKEDINFO),
  FIELDTAG(ID3FID_CDID),
  FIELDTAG(ID3FID_MPEGLOOKUP),
  FIELDTAG(ID3FID_OWNERSHIP),
  FIELDTAG(ID3FID_PRIVATE),
  FIELDTAG(ID3FID_PLAYCOUNTER),
  FIELDTAG(ID3FID_POPULARIMETER),
  FIELDTAG(ID3FID_POSITIONSYNC),
  FIELDTAG(ID3FID_BUFFERSIZE),
  FIELDTAG(ID3FID_VOLUMEADJ),
  FIELDTAG(ID3FID_REVERB),
  FIELDTAG(ID3FID_SYNCEDLYRICS),
  FIELDTAG(ID3FID_SYNCEDTEMPO),
  FIELDTAG(ID3FID_ALBUM),
  FIELDTAG(ID3FID_BPM),
  FIELDTAG(ID3FID_COMPOSER),
  FIELDTAG(ID3FID_CONTENTTYPE),
  FIELDTAG(ID3FID_COPYRIGHT),
  FIELDTAG(ID3FID_DATE),
  FIELDTAG(ID3FID_PLAYLISTDELAY),
  FIELDTAG(ID3FID_ENCODEDBY),
  FIELDTAG(ID3FID_LYRICIST),
  FIELDTAG(ID3FID_FILETYPE),
  FIELDTAG(ID3FID_TIME),
  FIELDTAG(ID3FID_CONTENTGROUP),
  FIELDTAG(ID3FID_TITLE),
  FIELDTAG(ID3FID_SUBTITLE),
  FIELDTAG(ID3FID_INITIALKEY),
  FIELDTAG(ID3FID_LANGUAGE),
  FIELDTAG(ID3FID_SONGLEN),
  FIELDTAG(ID3FID_MEDIATYPE),
  FIELDTAG(ID3FID_ORIGALBUM),
  FIELDTAG(ID3FID_ORIGFILENAME),
  FIELDTAG(ID3FID_ORIGLYRICIST),
  FIELDTAG(ID3FID_ORIGARTIST),
  FIELDTAG(ID3FID_ORIGYEAR),
  FIELDTAG(ID3FID_FILEOWNER),
  FIELDTAG(ID3FID_LEADARTIST),
  FIELDTAG(ID3FID_BAND),
  FIELDTAG(ID3FID_CONDUCTOR),
  FIELDTAG(ID3FID_MIXARTIST),
  FIELDTAG(ID3FID_PARTINSET),
  FIELDTAG(ID3FID_PUBLISHER),
  FIELDTAG(ID3FID_TRACKNUM),
  FIELDTAG(ID3FID_RECORDINGDATES),
  FIELDTAG(ID3FID_NETRADIOSTATION),
  FIELDTAG(ID3FID_NETRADIOOWNER),
  FIELDTAG(ID3FID_SIZE),
  FIELDTAG(ID3FID_ISRC),
  FIELDTAG(ID3FID_ENCODERSETTINGS),
  FIELDTAG(ID3FID_USERTEXT),
  FIELDTAG(ID3FID_YEAR),
  FIELDTAG(ID3FID_UNIQUEFILEID),
  FIELDTAG(ID3FID_TERMSOFUSE),
  FIELDTAG(ID3FID_UNSYNCEDLYRICS),
  FIELDTAG(ID3FID_WWWCOMMERCIALINFO),
  FIELDTAG(ID3FID_WWWCOPYRIGHT),
  FIELDTAG(ID3FID_WWWAUDIOFILE),
  FIELDTAG(ID3FID_WWWARTIST),
  FIELDTAG(ID3FID_WWWAUDIOSOURCE),
  FIELDTAG(ID3FID_WWWRADIOPAGE),
  FIELDTAG(ID3FID_WWWPAYMENT),
  FIELDTAG(ID3FID_WWWPUBLISHER),
};

int main(unsigned argc, char **argv) {
//  ID3D_INIT_DOUT();
//  ID3D_INIT_WARNING();
//  ID3D_INIT_NOTICE();

  if (argc != 2) {
    printf("Usage: findstr <tagfile>\n");
    exit(-1);
  }

  ID3_Tag tag(argv[1]);

  ID3_Frame         *frame = nullptr;
  const char        *fname = tag.GetFileName();
  ID3_Tag::Iterator *it    = tag.CreateIterator();
  ID3_V2Spec         spec  = tag.GetSpec();

/*
  try {
  for(int i = 0; i < ARRAYSIZE(fieldlist); i++) {
    printf("%s    :<%s>\n",fieldlist[i].tagname,gettag(tag,fieldlist[i].tag));
  }
  } catch(...) {
    printf("caught ... !!\n");
    return -1;
  }
*/
  printf("Title    :<%s>\n",gettag(tag,ID3FID_TITLE     ));
  printf("Artist   :<%s>\n",gettag(tag,ID3FID_LEADARTIST));
  printf("Album    :<%s>\n",gettag(tag,ID3FID_ALBUM     ));
  printf("Encodedby:<%s>\n",gettag(tag,ID3FID_ENCODEDBY ));

  printf("V1Tag:%s\n",tag.HasV1Tag() ? "true":"false");
  printf("V2Tag:%s\n",tag.HasV2Tag() ? "true":"false");
  settag(tag,ID3FID_TITLE     ,"new title"    );
  settag(tag,ID3FID_LEADARTIST,"new artist"   );
  settag(tag,ID3FID_ALBUM     ,"new album"    );
  settag(tag,ID3FID_ENCODEDBY ,"new encodedby");

  flags_t flags = tag.Update();
  printf("flags:%x ID3TT_ALL:%x\n",flags,ID3TT_ALL);
  return 0;
  while((frame = it->GetNext()) != nullptr) {
    const char          *textid = frame->GetTextID();
    ID3_Frame::Iterator *fit    = frame->CreateIterator();

    printf("textid:%s %zd\n",textid,frame->NumFields());
    ID3_Field *field;
    while((field = fit->GetNext()) != nullptr) {
      char value[1000];
      field->Get(value,sizeof(value));
      int n = field->GetNumTextItems();
      printf("  %s numitems:%d\n",value,n);
      for(int i = 0; i < n; i++) {
        field->Get(value,sizeof(value),i);
        printf("    item(%d):%s\n",i,value);
      }
    }
  }
  return 0;
}
