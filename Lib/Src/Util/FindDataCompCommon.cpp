#include "pch.h"
#include <Scandir.h>

CompoundFindDataComparator _sortalpha(          FILENAME     , true , -1);
CompoundFindDataComparator _sortdirfirst(       FILEATTRIB   , true , -1);
CompoundFindDataComparator _sortdirlast(        FILEATTRIB   , false, -1);
CompoundFindDataComparator _sortdirfirstalpha(  FILEATTRIB   , true , FILENAME     , true , -1);
CompoundFindDataComparator _sortdirlastalpha(   FILEATTRIB   , false, FILENAME     , true , -1);
CompoundFindDataComparator _sorttime(           FILEWRITETIME, true , -1);
CompoundFindDataComparator _sortdirfirsttime(   FILEATTRIB   , true , FILEWRITETIME, true , -1);
CompoundFindDataComparator _sortdirlasttime(    FILEATTRIB   , false, FILEWRITETIME, true , -1);
CompoundFindDataComparator _sortrevtime(        FILEWRITETIME, false, -1);
CompoundFindDataComparator _sortdirfirstrevtime(FILEATTRIB   , true , FILEWRITETIME, false, -1);
CompoundFindDataComparator _sortdirlastrevtime( FILEATTRIB   , false, FILEWRITETIME, false, -1);
CompoundFindDataComparator _sortsize(           FILESIZE     , true , -1);
CompoundFindDataComparator _sortdirfirstsize(   FILEATTRIB   , true , FILESIZE     , true , -1);
CompoundFindDataComparator _sortdirlastsize(    FILEATTRIB   , false, FILESIZE     , true , -1);

class SelectAllButDot : public FindDataSelector {
public:
  bool select(const DirListEntry &e) {
    return _tcscmp(e.name, _T(".")) && _tcscmp(e.name,_T(".."));
  }
  AbstractSelector *clone() const { return new SelectAllButDot; }
};

class SelectSubDir : public FindDataSelector {
public:
  bool select(const DirListEntry &e) {
    return (e.attrib & _A_SUBDIR) && SELECTALLBUTDOT->select(e);
  }
  AbstractSelector *clone() const { return new SelectSubDir; }
};

class SelectFile : public FindDataSelector {
public:
  bool select(const DirListEntry &e) {
    return !(e.attrib & _A_SUBDIR);
  }
  AbstractSelector *clone() const { return new SelectFile; }
};

static SelectAllButDot selectAllButDot;
static SelectSubDir    selectSubDir;
static SelectFile      selectFile;

FindDataSelector *SELECTALLBUTDOT = &selectAllButDot;
FindDataSelector *SELECTSUBDIR    = &selectSubDir;
FindDataSelector *SELECTFILE      = &selectFile;
