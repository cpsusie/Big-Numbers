#include "stdafx.h"

void Database::sysTabUserInsert(const String &username, const String &password) {
  if(username.length() == 0 || username.length() > MAXUSERNAME) {
    throwSqlError(SQL_INVALID_USERNAME,_T("Invalid username <%s>"),username.cstr());
  }

  if(password.length() > MAXPASSWORD) {
    throwSqlError(SQL_INVALID_PASSWORD,_T("Invalid password"));
  }

  SysTableUserData rec;
  _tcsupr(_tcscpy( rec.m_username,username.cstr()) );
  _tcsupr(_tcscpy( rec.m_password,password.cstr()) );

  DataFile datafile(  *this, SYSTEM_USERDATA_FNAME, DBFMODE_READWRITE);
  KeyFile  indexfile( *this, SYSTEM_USERKEY_FNAME , DBFMODE_READWRITE);

  DbAddr addr = datafile.insert( &rec, sizeof(rec));

  KeyFileDefinition keydef(indexfile);
  KeyType key;
  keydef.put(key,0,String(rec.m_username));
  keydef.putRecordAddr(key, addr );
  indexfile.insert(key);

}

void Database::sysTabReadUser(const String &username, SysTableUserData &userrec) const {
  
  DataFile datafile(  *this, SYSTEM_USERDATA_FNAME, DBFMODE_READONLY);
  KeyFile  indexfile( *this, SYSTEM_USERKEY_FNAME , DBFMODE_READONLY);

  KeyFileDefinition keydef(indexfile);
  KeyType userkey;
  keydef.put(userkey, 0, username );
  bool found = indexfile.searchMin( RELOP_EQ, userkey, 1); 
  if(!found) {
    throwSqlError(SQL_INVALID_USERNAME,_T("User <%s> doesn't exist"),username.cstr());
  }
  datafile.readRecord( keydef.getRecordAddr(userkey), &userrec, sizeof(SysTableUserData));
}
