#include "stdafx.h"
#include <TreeMap.h>
#include <ProcessTools.h>
#include <Thread.h>
#include "TrainerJob.h"

class WordWindowCheck {
public:
  String m_ord;
  bool   m_allowSeparation;
  int    m_count;
  WordWindowCheck(const String &s, bool allowSeparation) {
    m_ord             = s;
    m_allowSeparation = allowSeparation;
    m_count           = 1;
  }
};

typedef StringTreeMap<WordWindowCheck> WordWindowMap;
typedef Array<WordWindow>              TrainingList;

static void readTrainingData(TrainingList &list, WordWindowMap &wwmap) {
  FILE *f = FOPEN(_T("ordliste"),_T("r"));
  TCHAR line[512];
  while(FGETS(line,ARRAYSIZE(line),f)) {
    strRemove(line,'\n');
    String s(strTrim(line));
    WordWindowList ww(line);
    for(UINT i = 0; i < ww.size(); i++) {
      WordWindowCheck *wwc = wwmap.get(ww[i].m_window);
      if(wwc == nullptr) {
        wwmap.put(ww[i].m_window,WordWindowCheck(s,ww[i].m_allowSeparation));
      } else {
        if(wwc->m_allowSeparation == ww[i].m_allowSeparation) {
          wwc->m_count++;
        } else {
          fclose(f);
          throwException(_T("Ordvindue <%s> findes i både %s og %s med forskellig deling")
                        ,ww[i].toString().cstr()
                        ,wwc->m_ord.cstr()
                        ,s.cstr()
                        );
        }
      }
    }
  }
  fclose(f);
  for(auto it wwit = wwmap.getIterator(); wwit.hasNext(); ) {
    Entry<String,WordWindowCheck> &entry = wwit.next();
    list.add(WordWindow(entry.getKey().cstr(),entry.getValue().m_allowSeparation));
  }

  FILE *wwfile = fopen(_T("wwdat.txt"),_T("w"));
  if(wwfile != nullptr) {
    for(auto it wwit = wwmap.getIterator(); wwit.hasNext(); ) {
      Entry<String,WordWindowCheck> &entry = wwit.next();
      _ftprintf(wwfile,_T("%d %s\n"),entry.getValue().m_count,entry.getKey().cstr());
    }
    fclose(wwfile);
  }
}

static void readCycleCount(int &cycleCount, int &errorCount) { // find the last line in error.dat and return cycleCount.
  cycleCount = 0;
  errorCount = 0;

  FILE *f = fopen(_T("error.dat"),_T("r"));
  if(f == nullptr) return;
  TCHAR line[100];
  while(FGETS(line, ARRAYSIZE(line),f)) {
    if(_stscanf(line,_T("%d %d"),&cycleCount,&errorCount) != 2) {
      break;
    }
  }
  fclose(f);
}


TrainerJob::TrainerJob(const WordBpn &n, int priority)
: m_trainingNetwork(n)
, m_priority(priority)
{
}

void TrainerJob::setPriority(int priority) {
  m_priority = priority;
}

UINT TrainerJob::safeRun() {
  setThreadDescription(_T("TrainerJob"));
  try {
    int cycleCount;
    int lastCycleErrorCount;
    readCycleCount(cycleCount,lastCycleErrorCount);
    TrainingList trainingData;
    WordWindowMap wwmap;
    updateMessage(_T("Read trainingData"));
    readTrainingData(trainingData,wwmap);

    for(;;) {
      int cycleErrorCount    = 0;
      trainingData.shuffle();
      CompactIntArray fejlord;
      double starttime = getProcessTime();
      for(UINT i = 0; i < trainingData.size(); i++) {
        if(m_trainingNetwork.learnWordWindow(trainingData[i])) {
          cycleErrorCount++;
          fejlord.add(i);
        }
        if(i % 200 == 0) {
          updateMessage(_T("cycle (%2d %3.0lf%%) %5d (%6.3lf%%) errors in this cycle. last cycle:%5d, time:%2.3lf ")
                       ,cycleCount
                       ,(double)i/trainingData.size()*100
                       ,cycleErrorCount
                       ,(double)cycleErrorCount/(i+1)*100
                       ,lastCycleErrorCount
                       ,(getProcessTime() - starttime)/1000000);
        }
        checkInterruptAndSuspendFlags();
      }
      cycleCount++;
      FILE *logfile = FOPEN(_T("error.dat"),_T("a"));
      _ftprintf(logfile, _T("%d %d\n"),cycleCount,cycleErrorCount);
      fclose(logfile);
      FILE *fejlordfile = FOPEN(_T("fejlord.txt"),_T("w"));
      StringArray tmp;
      size_t i;
      for(i = 0; i < fejlord.size(); i++) {
        int index = fejlord[i];
        WordWindowCheck *wwc = wwmap.get(trainingData[index].m_window);
        tmp.add(format(_T("%-40s %s"),(wwc)?wwc->m_ord.cstr():EMPTYSTRING,trainingData[index].toString().cstr()));
      }
      tmp.sort(stringHashCmp);
      for(i = 0; i < tmp.size(); i++) {
        _ftprintf(fejlordfile,_T("%s\n"),tmp[i].cstr());
      }
      fclose(fejlordfile);
      lastCycleErrorCount = cycleErrorCount;
    }
  } catch(Exception e) {
    updateMessage(_T("%s"), e.what());
  }
  return 0;
}

void TrainerJob::updateMessage(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr,format);
  const String tmp = vformat(format, argptr);
  va_end(argptr);
  m_lock.wait();
  m_msg = tmp;
  m_lock.notify();
}

String TrainerJob::getMessage() {
  m_lock.wait();
  const String res = m_msg;
  m_lock.notify();
  return res;
}
