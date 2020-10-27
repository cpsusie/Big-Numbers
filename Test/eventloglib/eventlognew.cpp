#include <MyUtil.h>
#include <Registry.h>
#include "eventlog.h"
#include "message.h"

static void addEventSource(const String &logName, const String &srcName, const String &msgDllPath, int categoriCount) {
   // logName       = "Application" or a custom logfile name
   // srcName       = event source name
   // msgDllPath    = path for message DLL
   // categoriCount = number of categories

  RegistryKey key0(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\EventLog");
  RegistryKey key1 = key0.createOrOpenKey(logName);
  RegistryKey key2 = key1.createOrOpenKey(srcName);

  //  String keyName = format("%s\\%s",logName.cstr(),srcName.cstr());

// Create the event source as a subkey of the logfile.
//  RegistryKey key1 = key0.createOrOpenPath(keyName);

//  String keyName = format("SYSTEM\\CurrentControlSet\\Services\\EventLog\\%s\\%s",logName.cstr(),srcName.cstr());
//  HKEY hk;
//  long result = RegCreateKey(HKEY_LOCAL_MACHINE, keyName.cstr(), &hk);
//  if(result != ERROR_SUCCESS) {
//    throwException("RegCreateKey(HKEY_LOCAL_MACHINE,%s) failed:%s", keyName.cstr(),getErrorText(result).cstr());
//  }

//  RegistryKey key1(hk);

   // Set the name of the message file.
  key1.setValue("EventMessageFile", msgDllPath, REG_EXPAND_SZ);

   // Set the supported event types.
  DWORD dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE;
  key1.setValue("TypesSupported",dwData);

   // Set the category message file and number of categories.
  key1.setValue("CategoryMessageFile", msgDllPath, REG_EXPAND_SZ);
  key1.setValue("CategoryCount",categoriCount);
}

static void removeEventSource(const String &logName, const String &srcName) {
  // logName = "Application" or a custom logfile name
  // srcName = event source name

  RegistryKey key0(HKEY_LOCAL_MACHINE,format("SYSTEM\\CurrentControlSet\\Services\\EventLog\\%s",logName.cstr()));
  key0.deleteKey(srcName);
}

class AutoRegister {
private:
  static String moduleFileName;
  static String srcName;
public:
  static void registerEventSource();
  static void deregisterEventSource();
  AutoRegister() {
    moduleFileName = ::getModuleFileName();
    srcName        = FileNameSplitter(moduleFileName).getFileName();
    registerEventSource();
  }

  static const String &getModuleFileName() {
    return moduleFileName;
  }

  static const String &getSrcName() {
    return srcName;
  }
};

String AutoRegister::moduleFileName;
String AutoRegister::srcName;

void AutoRegister::registerEventSource() {
  try {
    addEventSource("Application", getSrcName(), getModuleFileName(), 1);
  } catch(Exception e) {
    String t = e.what();
    // ignore
  }
}

void AutoRegister::deregisterEventSource() {
  removeEventSource("Application", getSrcName());
}

static AutoRegister dummy;

static void vlog(int wtype, const char *format, va_list argptr) {

  HANDLE eventSource = RegisterEventSource(nullptr, AutoRegister::getSrcName().cstr());

  String msg = vformat(format, argptr);

  const char *stringArray[2] = { msg.cstr(), nullptr };

  if(eventSource != nullptr) {
    ReportEvent(eventSource       // Handle of event source
               ,wtype             // Event type
               ,0                 // Event category
               ,MYMSG             // Event ID
               ,nullptr              // Current user's SID
               ,1                 // Number of strings in stringArray
               ,0                 // No bytes of raw data
               ,stringArray       // Array of error strings
               ,nullptr);            // No raw data

    DeregisterEventSource(eventSource);
  }
}

void eventLogError(const char *format,...) {
  va_list argptr;
  va_start(argptr,format);
  vlog(EVENTLOG_ERROR_TYPE,format, argptr);
  va_end(argptr);
}

void eventLogWarning(const char *format,...) {
  va_list argptr;
  va_start(argptr,format);
  vlog(EVENTLOG_WARNING_TYPE,format, argptr);
  va_end(argptr);
}

void eventLogInfo(const char *format,...) {
  va_list argptr;
  va_start(argptr,format);
  vlog(EVENTLOG_INFORMATION_TYPE,format, argptr);
  va_end(argptr);
}

void eventLogSuccess(const char *format,...) {
  va_list argptr;
  va_start(argptr,format);
  vlog(EVENTLOG_SUCCESS,format, argptr);
  va_end(argptr);
}

