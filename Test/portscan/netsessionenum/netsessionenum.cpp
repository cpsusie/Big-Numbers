#include "stdafx.h"
#include <lm.h>

#pragma comment(lib, "netapi32.lib")

int _tmain(int argc, TCHAR **argv) {
   LPSESSION_INFO_10 pBuf = nullptr;
   LPSESSION_INFO_10 pTmpBuf;
   DWORD dwLevel = 10;
   DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
   DWORD dwEntriesRead = 0;
   DWORD dwTotalEntries = 0;
   DWORD dwResumeHandle = 0;
   DWORD i;
   DWORD dwTotalCount = 0;
   LPTSTR pszServerName = nullptr;
   LPTSTR pszClientName = nullptr;
   LPTSTR pszUserName = nullptr;
   NET_API_STATUS nStatus;
   //
   // Check command line arguments.
   //
   if (argc > 4) {
      _tprintf(_T("Usage: %s [\\\\ServerName] [\\\\ClientName] [UserName]\n"), argv[0]);
      exit(1);
   }

   if (argc >= 2)
      pszServerName = argv[1];

   if (argc >= 3)
      pszClientName = argv[2];

   if (argc == 4)
      pszUserName = argv[3];
   //
   // Call the NetSessionEnum function, specifying level 10.
   //
   do {
      nStatus = NetSessionEnum(pszServerName,
                               pszClientName,
                               pszUserName,
                               dwLevel,
                               (LPBYTE*)&pBuf,
                               dwPrefMaxLen,
                               &dwEntriesRead,
                               &dwTotalEntries,
                               &dwResumeHandle);
      //
      // If the call succeeds,
      //
      if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA)) {
         if ((pTmpBuf = pBuf) != nullptr)
         {
            //
            // Loop through the entries.
            //
            for (i = 0; (i < dwEntriesRead); i++) {
               assert(pTmpBuf != nullptr);

               if (pTmpBuf == nullptr) {
                  _ftprintf(stderr, _T("An access violation has occurred\n"));
                  break;
               }
               //
               // Print the retrieved data.
               //
               _tprintf(_T("\n\tClient: %s\n"), pTmpBuf->sesi10_cname    );
               _tprintf(_T("\tUser:   %s\n"  ), pTmpBuf->sesi10_username );
               _tprintf(_T("\tActive: %d\n"  ), pTmpBuf->sesi10_time     );
               _tprintf(_T("\tIdle:   %d\n"  ), pTmpBuf->sesi10_idle_time);

               pTmpBuf++;
               dwTotalCount++;
            }
         }
      }
      //
      // Otherwise, indicate a system error.
      //
      else
         _ftprintf(stderr, _T("A system error has occurred: %d\n"), nStatus);
      //
      // Free the allocated memory.
      //
      if (pBuf != nullptr) {
         NetApiBufferFree(pBuf);
         pBuf = nullptr;
      }
   }
   //
   // Continue to call NetSessionEnum while
   //  there are more entries.
   //
   while (nStatus == ERROR_MORE_DATA); // end do

   // Check again for an allocated buffer.
   //
   if (pBuf != nullptr)
      NetApiBufferFree(pBuf);
   //
   // Print the final count of sessions enumerated.
   //
   _ftprintf(stderr, _T("\nTotal of %d entries enumerated\n"), dwTotalCount);

   return 0;
}
