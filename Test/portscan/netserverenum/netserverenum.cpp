#include "stdafx.h"
#include <lm.h>

#pragma comment(lib, "netapi32.lib")

class FlagDescription {
public:
  const DWORD m_flag;
  const String m_name;
  const String m_description;
public:
  FlagDescription(DWORD flag, const char *name, const char *description);
};

FlagDescription::FlagDescription(DWORD flag, const char *name, const char *description)
: m_flag(flag)
, m_name(name)
, m_description(description)
{
}

#define flagDescription(flag,str) FlagDescription(SV_TYPE_##flag, #flag, str)

static const FlagDescription flagDescriptions[] = {
  flagDescription(WORKSTATION       ,"A LAN Manager workstation")
 ,flagDescription(SERVER            ,"A LAN Manager server")
 ,flagDescription(SQLSERVER         ,"Any server running with Microsoft SQL Server")
 ,flagDescription(DOMAIN_CTRL       ,"Primary domain controller")
 ,flagDescription(DOMAIN_BAKCTRL    ,"Backup domain controller")
 ,flagDescription(TIME_SOURCE       ,"Server running the Timesource service")
 ,flagDescription(AFP               ,"Apple File Protocol server")
 ,flagDescription(NOVELL            ,"Novell server")
 ,flagDescription(DOMAIN_MEMBER     ,"LAN Manager 2.x domain member")
 ,flagDescription(LOCAL_LIST_ONLY   ,"Servers maintained by the browser")
 ,flagDescription(PRINTQ_SERVER     ,"Server sharing print queue")
 ,flagDescription(DIALIN_SERVER     ,"Server running dial-in service")
 ,flagDescription(XENIX_SERVER      ,"Xenix server")
 ,flagDescription(SERVER_MFPN       ,"Microsoft File and Print for NetWare")
 ,flagDescription(NT                ,"Windows NT/Windows 2000 workstation or server")
 ,flagDescription(WFW               ,"Server running Windows for Workgroups")
 ,flagDescription(SERVER_NT         ,"Windows NT/Windows 2000 server that is not a domain controller")
 ,flagDescription(POTENTIAL_BROWSER ,"Server that can run the browser service")
 ,flagDescription(BACKUP_BROWSER    ,"Server running a browser service as backup")
 ,flagDescription(MASTER_BROWSER    ,"Server running the master browser service")
 ,flagDescription(DOMAIN_MASTER     ,"Server running the domain master browser")
 ,flagDescription(DOMAIN_ENUM       ,"Primary domain ")
 ,flagDescription(WINDOWS           ,"Windows 95 or later")
 ,flagDescription(CLUSTER_NT        ,"Server clusters available in the domain")
};

static StringArray flagsToShortString(DWORD flags) {
  StringArray result;
  for(int i = 0; i < ARRAYSIZE(flagDescriptions); i++) {
    const FlagDescription &fd = flagDescriptions[i];
    if(fd.m_flag & flags) {
      result.add(fd.m_name);
    }
  }
  return result;
}

static StringArray flagsToDescription(DWORD flags) {
  StringArray result;
  for(int i = 0; i < ARRAYSIZE(flagDescriptions); i++) {
    const FlagDescription &fd = flagDescriptions[i];
    if(fd.m_flag & flags) {
      result.add(fd.m_description);
    }
  }
  return result;
}

static TCHAR *platformToString(DWORD platform) {
  switch(platform) {
  case PLATFORM_ID_DOS: return _T("DOS");
  case PLATFORM_ID_OS2: return _T("OS2");
  case PLATFORM_ID_NT : return _T("NT");
  case PLATFORM_ID_OSF: return _T("OSF");
  case PLATFORM_ID_VMS: return _T("VMS");
  default             : return _T("unknown platform");
  }
}

static void usage() {
  _ftprintf(stderr, _T("Usage:getnetservers [-servername]\n"));
  exit(-1);
}

class ServerInfo101 {
public:
  ServerInfo101(const SERVER_INFO_101 &src);
  DWORD  m_platformId;
  String m_name;
  DWORD  m_versionMajor;
  DWORD  m_versionMinor;
  DWORD  m_type;
  String m_comment;

  String getPlatformString() const {
    return platformToString(m_platformId);
  }
  String toString() const;
};

ServerInfo101::ServerInfo101(const SERVER_INFO_101 &src) {
  m_platformId   = src.sv101_platform_id;
  
  m_name         = src.sv101_name;
  m_versionMajor = src.sv101_version_major;
  m_versionMinor = src.sv101_version_minor;
  m_type         = src.sv101_type;
  m_comment      = src.sv101_comment;
};

String ServerInfo101::toString() const {
  String result;
  result += format(_T("Platform:%s\n"   ), platformToString(m_platformId));
  result += format(_T("Name    :%s\n"   ), m_name.cstr());
  result += format(_T("Version :%d.%d\n"), m_versionMajor, m_versionMinor);
  result += format(_T("Type    :%d ("   ), m_type);
  StringArray tmp = flagsToShortString(m_type);
  for(size_t i = 0; i < tmp.size(); i++) {
    if(i > 0) {
      result += _T(",");
    }
    result += tmp[i];
  }
  result += _T(")\n");

  if(m_type & SV_TYPE_DOMAIN_CTRL) {
    result += _T(" (PDC)\n");
  }
  if(m_type & SV_TYPE_DOMAIN_BAKCTRL) {
    result += _T(" (BDC)\n");
  }
  return result;
}

Array<ServerInfo101> getServerInfoLevel101(int serverTypes = SV_TYPE_SERVER) {
  SERVER_INFO_101   *buf = NULL;

  try {
    const DWORD    level        = 101;
    DWORD          entryCount   = 0;
    DWORD          totalEntries = 0;
    DWORD          totalCount   = 0;
    DWORD          resumeHandle = 0;
    NET_API_STATUS status;

    Array<ServerInfo101> result;

    do {

       status = NetServerEnum(NULL
                             ,level
                             ,(LPBYTE*)&buf
                             ,MAX_PREFERRED_LENGTH
                             ,&entryCount
                             ,&totalEntries
                             ,serverTypes
                             ,NULL
                             ,&resumeHandle);
      if(status == NERR_Success || status == ERROR_MORE_DATA) {
        if(buf == NULL) {
          throwException(_T("An access violation has occurred"));
        }
        for(DWORD i = 0; i < entryCount; i++) {
          result.add(ServerInfo101(buf[i]));
        }
      } else {
        throwException(_T("getServerInfoLevel101:NetServerEnum failed:ErrorCode=%#lx"),status);
      }
      if(buf != NULL) {
        NetApiBufferFree(buf);
        buf = NULL;
      }
    } while(status == ERROR_MORE_DATA);
    return result;
  } catch(Exception e) {
    if(buf != NULL) {
      NetApiBufferFree(buf);
    }
    throw;
  }
}

int _tmain(int argc, TCHAR **argv) {
  try {
    Array<ServerInfo101> list = getServerInfoLevel101();
    for(size_t i = 0; i < list.size(); i++) {
      const ServerInfo101 &info = list[i];
      _tprintf(_T("%s"),info.toString().cstr());
    }
  } catch(Exception e) {
    _ftprintf(stderr,_T("Exception:%s\n"), e.what());
    return -1;
  }
  return 0;
}

