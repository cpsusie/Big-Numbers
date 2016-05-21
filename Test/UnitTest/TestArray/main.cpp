#include "stdafx.h"
#include <Console.h>
#include "TestArray.h"

static void displayArray(int column, TCHAR *name, ArrayType &a) {
  Console::printf(column,0,_T("   <%s>"),name);
  for(int i = 0; i < 20; i++)
    if(i < a.size()) {
      Console::printf(column,i+1,_T("%5d       "),a[i].n);
    } else {
      Console::printf(column,i+1,_T("          "));
    }
}

static int getInteger(TCHAR *prompt) {
  int width,height;
  Console::getBufferSize(width,height);
  for(;;) {
    Console::clearRect(0,height-2,width,height);
    Console::printf(0,height-2,_T("%s"),prompt);
    String line;
    Console::showCursor(true);
    Console::setCursorPos(_tcslen(prompt),height-2);
    readLine(stdin, line);
    int x;
    if(_stscanf(line.cstr(),_T("%d"),&x) == 1) {
      Console::clearRect(0,height-2,width,height);
      return x;
    }
  }
}

static ElemComparator elemComparator;

static int compare1(const ArrayElement &e1, const ArrayElement &e2) {
  return e1.n - e2.n;
}


int _tmain(int argc, TCHAR **argv) {
//  measureSort();
//  exit(0);

  try {
    testArray();
    pause();
  } catch(Exception e) {
    _tprintf(_T("Test failed:%s\n"), e.what());
    return -1;
  }

//  logfile = fopen(_T("fisk.log"),_T("w"));
  ArrayType a;
  a.add(ArrayElement(1));

  ArrayType b(a);
  Array<int> c;

//  measureSort();

  Console::clear();
  int index,e,count;
  int width,height;
  Console::getBufferSize(width,height);
  for(;;) {
    displayArray(0 ,_T("A"),a);
    displayArray(20,_T("B"),b);
    Console::printf(0,height-3,_T("(P)ushback,(i)nsert,b(I)nsert,(R)emove,(r)andom,(A)=B, (B)=A,(C)lear,(S)ort,s(W)ap,sh(u/U)ffle,(Q)uit:"));
    try {
      Console::showCursor(false);
      int key = Console::getKey();
      Console::clearRect(0,height-1,width,height);
      switch(key) {
      case 'p':
        e = getInteger(_T("Enter number:"));
        a.add(e);
        break;
      case 'P':
        a.addAll(b);
        break;
      case 'I':
        e = getInteger(_T("Enter number:"));
        a.binaryInsert(e,elemComparator);
        break;
      case 'i':
        index = getInteger(_T("Enter index:"));
        e = getInteger(_T("Enter number:"));
        a.add(index,e);
        break;
      case 'R':
        index = getInteger(_T("Enter index:"));
        a.removeIndex(index);
        break;
      case 'r':
        count = getInteger(_T("Enter count:"));
        for(e = 0; e < count; e++)
          a.add(rand());
        break;
      case 'A':
      case 'a':
        a = b;
        break;
      case 'B':
      case 'b':
        b = a;
        break;
      case 'C':
      case 'c':
        a.clear();
        break;
      case 's':
        a.sort(elemComparator);
        a.sort(compare1);
        break;
      case 'S':
        index = getInteger(_T("Enter startindex:"));
        count = getInteger(_T("Enter count:"));
        a.sort(index,count,elemComparator);
        break;
      case 'W':
      case 'w':
        index = getInteger(_T("Enter index1:"));
        e     = getInteger(_T("Enter index2:"));
        a.swap(index,e);
        break;
      case 'u':
        a.shuffle();
        break;
      case 'U':
        index = getInteger(_T("Enter startindex:"));
        count = getInteger(_T("Enter count:"));
        a.shuffle(index,count);
        break;
      case 'f':
      case 'F':
        { ArrayElement key(getInteger(_T("Indtast key:")));
          index = a.binarySearch(key,elemComparator);
          Console::printf(0,height-1,_T("index:%d"),index);
          break;
        }
        
      case 'Q':
      case 'q':
        goto done;
      default:
        Console::printf(0,height-1,_T("Invalid key pressed"));
        break;
      }
    } catch(Exception e) {
      Console::printf(0,height-1,_T("Exception:%s "),e.what());
    }
  }
done:
  return 0;
}
