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
  size_t                     m_current;
  String                     m_title;
  String                     m_progressMsgString;
  String                     m_currentMsg;
  UINT64                     m_byteCounterStart;
  Semaphore                  m_gate;
  UINT64                     m_currentFileSize;
  void setCurrentFileSize();
  void setCurrentMessage(const String &msg);
public:
  DecompressJob(const EndGameTablebaseList &list);

  USHORT getMaxProgress() {         // Only called if getSupportedFeatures() contains IR_PROGRESSBAR, IR_SHOWTIMEESTIMATE or IR_SUBPROGRESSBAR
    return (USHORT)m_list.size();
  }
  USHORT getProgress() {            // do. Should return a short in the range [0..getMaxProgress()]
    return (USHORT)m_current;
  };
  USHORT getSubProgressPercent() {  // Only called if getSupportedFeatures() contains IR_SUBPROGRESSBAR
    return (USHORT)PERCENT(getCount() - m_byteCounterStart, m_currentFileSize);
  }
  String getProgressMessage() {
    m_gate.wait();
    const String result = m_currentMsg;
    m_gate.signal();
    return result;
  }
  void incrCount(UINT64 n) {
    if(isInterrupted()) {
      throw InterruptedException();
    }
    ByteCounter::incrCount(n);
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
  setCurrentFileSize();
}

void DecompressJob::setCurrentFileSize() {
  m_byteCounterStart = getCount();
  m_currentFileSize  = m_list[m_current]->getFileSize(COMPRESSEDTABLEBASE);
}

void DecompressJob::setCurrentMessage(const String &msg) {
  m_gate.wait();
  m_currentMsg = msg;
  m_gate.signal();
}

UINT DecompressJob::run() {
  for(m_current = 0; m_current < m_list.size(); m_current++) {
    const EndGameTablebase &tb = *m_list[m_current];
    if(isInterrupted()) {
      break;
    }
    setCurrentMessage(format(m_progressMsgString.cstr(), tb.getName().cstr(), (int)m_current+1, (int)m_list.size()));
    setCurrentFileSize();
    try {
      tb.decompress(this);
    } catch(InterruptedException) {
      break;
    }
    verbose(_T("\n"));
  }
  setCurrentMessage(_T("Done"));
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
    ProgressWindow progressWindow(NULL, DecompressJob(jobList), 0, 200);
  }
}

#endif
