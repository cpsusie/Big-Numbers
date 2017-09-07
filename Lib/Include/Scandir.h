#pragma once

#include "MyUtil.h"
#include <io.h>

typedef _tfinddata64_t DirListEntry;

typedef Comparator<DirListEntry> FindDataComparator;
typedef Selector<  DirListEntry> FindDataSelector;
typedef Array<DirListEntry> DirList;

DirList scandir(const TCHAR  *pattern, FindDataSelector *selector = NULL, FindDataComparator *comparator = NULL);
DirList scandir(const String &pattern, FindDataSelector *selector = NULL, FindDataComparator *comparator = NULL);

enum FindDataField {
  FILEATTRIB
 ,FILENAME
 ,FILEWRITETIME
 ,FILESIZE
};

class FindDataFieldSortDefinition {
public:
  FindDataField m_field;
  bool          m_ascending;
  FindDataFieldSortDefinition() : m_field(FILENAME), m_ascending(true) {
  }

  FindDataFieldSortDefinition(FindDataField field, bool ascending = true) {
    m_field = field, m_ascending = ascending;
  }
};

class CompoundFindDataComparator : public FindDataComparator {
private:
  CompactArray<FindDataFieldSortDefinition> m_sortDefinition;
public:
  int compare(const DirListEntry &e1, const DirListEntry &e2);
  CompoundFindDataComparator(FindDataField field, ...);
  AbstractComparator *clone() const {
    return new CompoundFindDataComparator(*this);
  }
};

extern CompoundFindDataComparator _sortalpha;
extern CompoundFindDataComparator _sortdirfirst;
extern CompoundFindDataComparator _sortdirlast;
extern CompoundFindDataComparator _sortdirfirstalpha;
extern CompoundFindDataComparator _sortdirlastalpha;
extern CompoundFindDataComparator _sorttime;
extern CompoundFindDataComparator _sortdirfirsttime;
extern CompoundFindDataComparator _sortdirlasttime;
extern CompoundFindDataComparator _sortrevtime;
extern CompoundFindDataComparator _sortdirfirstrevtime;
extern CompoundFindDataComparator _sortdirlastrevtime;
extern CompoundFindDataComparator _sortsize;
extern CompoundFindDataComparator _sortdirfirstsize;
extern CompoundFindDataComparator _sortdirlastsize;

#define SORTALPHA           &_sortalpha           // alphabetic
#define SORTDIRFIRST        &_sortdirfirst        // directories first, normal files last
#define SORTLAST            &_sortdirlast         // normal files first, directories last
#define SORTDIRFIRSTALPHA   &_sortdirfirstalpha   // directories first, then alphabetic
#define SORTDIRLASTALPHA    &_sortdirlastalpha    // normal files first, then alphabetic
#define SORTTIME            &_sorttime            // modification time
#define SORTDIRFIRSTTIME    &_sortdirfirsttime    // directories first, then modification time
#define SORTDIRLASTTIME     &_sortdirlasttime     // directories last, then modification time
#define SORTREVTIME         &_sortrevtime         // reverse modification time
#define SORTDIRFIRSTREVTIME &_sortdirfirstrevtime // directories first, then modification time reverse
#define SORTDIRLASTREVTIME  &_sortdirlastrevtime  // directories last, then modification time reverse
#define SORTSIZE            &_sortsize            // filesize
#define SORTDIRFIRSTSIZE    &_sortdirfirstsize    // directories first, then filesize
#define SORTDIRLASTSIZE     &_sortdirlastsize     // directories last, then filesize

extern FindDataSelector *SELECTALLBUTDOT;         // select normal files and subdirectories, except "." and ".."
extern FindDataSelector *SELECTSUBDIR;            // select subdirectories only, except "." and ".."
extern FindDataSelector *SELECTFILE;              // select normal files only
