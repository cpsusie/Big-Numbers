#include "stdafx.h"
#include <MyUtil.h>
#include <Console.h>
#include "TestCollection.h"

class TestWindow {
private:
  int screenWidth,screenHeight;

public:
  TestWindow() {
    Console::getBufferSize(screenWidth,screenHeight);
    Console::clear();
  }

  int getPromptLine() const { return screenHeight-5; }
  int getInfoLine()   const { return screenHeight-4; }
  void vinfo(int line, const TCHAR *format, va_list argptr);
  void info(const TCHAR *format,...);
  void info(int line, const TCHAR *format,...);
  void prompt(TCHAR *format, ...);
  int getint();
};

void TestWindow::vinfo(int line, const TCHAR *format, va_list argptr) {
  Console::clearRect(1,line,screenWidth,line);
  Console::setCursorPos(1,line);
  _vtprintf(format, argptr);
}

void TestWindow::prompt(TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr,format);
  vinfo(getPromptLine(), format, argptr);
  va_end(argptr);
}

void TestWindow::info(const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  vinfo(getInfoLine(), format, argptr);
  va_end(argptr);
}

void TestWindow::info(int line, const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr, format);
  vinfo(line,format, argptr);
  va_end(argptr);
}

int TestWindow::getint() {
  String line;
  int x;
  for(;;) {
    prompt(_T("Enter number:"));
    readLine(stdin, line);
    if(_stscanf(line.cstr(), _T("%d"), &x) == 1) return x;
  }
}

static void listMap(TestWindow &w, Map<Key,Element> &m) {
  String s;
  for(Iterator<Key> it = m.keySet().getIterator(); it.hasNext();) {
    const Key &k = it.next();
    s += k.toString() + " ";
  }
  w.info(w.getInfoLine()+1,_T("%s"),s.cstr());
}

static void testHashIterator() {
  KeyHashSet s;
  for(int i = 0; i < 100000; i++)
    s.add(i);
  double starttime = getProcessTime();
  for(int i = 0; i < 100; i++) {
    for(Iterator<Key> it = s.getIterator(); it.hasNext();) {
      Key &k = it.next();
    }
  }
  double endtime = getProcessTime();
  _tprintf(_T("time:%.3lf\n"),(endtime - starttime) / 1000000);
}

int main() {
  try {
    testCollection();
//    testHashIterator();

    pause();

//    Key *k = new Key(1);

    KeyElementTreeMap testMap;
    testMap.put(1,2);
    testMap.put(2,3);
    testMap.put(3,3);
    Map<Key,Element> b(testMap);
    Key      key(-1);
    Element *element = NULL;
    TestWindow w;
    bool status = true;

    for(;;) {
      listMap(w,testMap);
      w.info(_T("status:%s count:%3d key:<%s> element:<%s>"),
        boolToStr(status),testMap.size(), key.toString().cstr(),element?element->toString().cstr():_T(""));
      w.prompt(_T("Insert/Delete/Find/Clear/Many/Quit:"));
      switch(Console::getKey()) {
      case '=':
        b = testMap;
        listMap(w,b);
        pause();
        break;
      case 'i':
      case 'I':
        { key = w.getint();
          int y = key.getValue() + 1;
          status = testMap.put(key,y);
          element = testMap.get(key);
          break;
        }
      case 'd':
      case 'D':
        key = w.getint();
        status = testMap.remove(key);
        element = NULL;
        break;

      case 'f':
      case 'F':
        key = w.getint();
        element = testMap.get(key);
        break;
      case 'c':
      case 'C':
        testMap.clear();
        element = NULL;
        break;

      case 'm':
      case 'M':
        { int x = w.getint();
          for( int i = 0; i < x; i++ ) {
            int x10 = 10 * i;
            int y10 = x10 + 1;
            testMap.put(x10,y10);
          }
        }
        break;
      case 'q':
      case 'Q':
        goto afslut;

      default:
        continue;
      }
    }
  } catch(Exception e) {
    _tprintf(_T("Exception:%s\n"),e.what());
    return -1;
  }
afslut:
  return 0;
}
