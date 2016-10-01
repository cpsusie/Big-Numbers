#include "stdafx.h"
#include <MFCUtil/ProgressWindow.h>
#include "EndGameTablebase.h"

#ifndef TABLEBASE_BUILDER

class InterruptedException {
public:
  bool dummy;
};

class DecompressJob : public InteractiveRunnable, public ByteCounter {
private:
  const EndGameTablebaseList m_list;
  int                        m_current;
  String                     m_title;
  String                     m_progressMsgString;
  unsigned int               m_byteCounter;
  unsigned int               m_currentFileSize;
  void setCurrentFileSize();
public:

  DecompressJob(const EndGameTablebaseList &list);

  unsigned short getMaxProgress() {         // Only called if getSupportedFeatures() contains IR_PROGRESSBAR, IR_SHOWTIMEESTIMATE or IR_SUBPROGRESSBAR
    return (unsigned short)m_list.size();
  }
  unsigned short getProgress() {            // do. Should return a short in the range [0..getMaxProgress()]
    return m_current;
  };
  unsigned short getSubProgressPercent() {  // Only called if getSupportedFeatures() contains IR_SUBPROGRESSBAR
    return (unsigned short)PERCENT(m_byteCounter, m_currentFileSize);
  }
  String getProgressMessage() {
    return m_current < (int)m_list.size() ? format(m_progressMsgString.cstr(), m_list[m_current]->getName().cstr(), m_current+1, (int)m_list.size()) : _T("Done");
  }
  void incrCount(unsigned int n) {
    if(isInterrupted()) {
      throw InterruptedException();
    }
    m_byteCounter += n;
  }
  String getTitle() {
    return m_title;
  }

  int getSupportedFeatures() {
    return IR_PROGRESSBAR | IR_SUBPROGRESSBAR | IR_INTERRUPTABLE | IR_SUSPENDABLE | IR_SHOWPROGRESSMSG;
  }

  UINT run();
};

DecompressJob::DecompressJob(const EndGameTablebaseList &list) : m_list(list) {
  setSelectedLanguageForThread();
  m_title             = format(_T("%s - %s:%s")
                              ,loadString(IDS_DECOMPRESSALLTITLE).cstr()
                              ,loadString(IDS_METRIC).cstr()
                              ,EndGameKeyDefinition::getMetricName()
                              );
  m_progressMsgString = loadString(IDS_DECOMPRESSALLMESSAGE_s_d_d);
  m_current = 0;
}

void DecompressJob::setCurrentFileSize() {
  m_byteCounter     = 0;
  m_currentFileSize = m_list[m_current]->getFileSize(COMPRESSEDTABLEBASE);
}

unsigned int DecompressJob::run() {
  for(m_current = 0; m_current < (int)m_list.size(); m_current++) {
    const EndGameTablebase &tb = *m_list[m_current];
    if(isInterrupted()) {
      break;
    }
    setCurrentFileSize();
    try {
      tb.decompress(this);
    } catch(InterruptedException) {
      break;
    }
    verbose(_T("\n"));
  }
  return 0;
}

static EndGameTablebaseList getNotDecompressedEndGameTablebases() {
  EndGameTablebaseList result = EndGameTablebase::getExistingEndGameTablebases();
  for(size_t i = result.size(); i--;) {
    if(!result[i]->needDecompress()) {
      result.remove(i);
    }
  }
  return result;
}


void EndGameTablebase::decompressAll() { // static
  EndGameTablebaseList jobList = getNotDecompressedEndGameTablebases();
  if(jobList.size() == 0) {
    AfxMessageBox(IDS_ALLTABLEBASESDECOMPRESSED, MB_ICONINFORMATION|MB_OK);
  } else {
    ProgressWindow progressWindow(NULL, DecompressJob(jobList), 0, 800);
  }
}

#endif
