// netgroupenum.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <assert.h>
#include <lm.h>

#pragma comment(lib, "netapi32.lib")

static void printattributes(ULONG attributes) {
  printf("\tAttr   :%08x (",attributes);
  if(attributes & SE_GROUP_MANDATORY         ) printf("MANDATORY "         );
  if(attributes & SE_GROUP_ENABLED           ) printf("ENABLED "           );
  if(attributes & SE_GROUP_ENABLED_BY_DEFAULT) printf("ENABLED_BY_DEFAULT ");
  if(attributes & SE_GROUP_OWNER             ) printf("OWNER "             );
  printf(")\n");
}

int _tmain(int argc, TCHAR **argv) {
   PGROUP_INFO_2 pBuf = NULL;
   PGROUP_INFO_2 pTmpBuf;
   DWORD dwLevel = 2;
   DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
   DWORD dwEntriesRead = 0;
   DWORD dwTotalEntries = 0;
   DWORD dwTotalCount = 0;
   DWORD_PTR dwResumeHandle = 0;
   NET_API_STATUS nStatus;
   LPTSTR pszServerName = NULL;
   DWORD i;

   if (argc > 2) {
      _ftprintf(stderr, _T("Usage: %s [\\\\ServerName]\n"), argv[0]);
      exit(1);
   }
   // The server is not the default local computer.
   //
   if (argc == 2)
      pszServerName = argv[1];
   //
   // Call the NetGroupEnum function to retrieve information
   //  for the server, specifying information level 101.
   //
   nStatus = NetGroupEnum(pszServerName,
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
      if ((pTmpBuf = pBuf) != NULL) {
         //
         // Loop through the entries and
         //  print the data for all server types.
         //
         for (i = 0; i < dwEntriesRead; i++) {
            assert(pTmpBuf != NULL);

            if (pTmpBuf == NULL) {
               fprintf(stderr, "An access violation has occurred\n");
               break;
            }

            _tprintf(_T("\tName   :%s\n"), pTmpBuf->grpi2_name);
            _tprintf(_T("\tId     :%d\n"), pTmpBuf->grpi2_group_id);

            printattributes(pTmpBuf->grpi2_attributes);

            _tprintf(_T("\tComment:%s\n\n"), pTmpBuf->grpi2_comment);

            pTmpBuf++;
            dwTotalCount++;
         }
         // Display a warning if all available entries were
         //  not enumerated, print the number actually
         //  enumerated, and the total number available.

         if (nStatus == ERROR_MORE_DATA) {
            _ftprintf(stderr, _T("\nMore entries available!!!\n"));
            _ftprintf(stderr, _T("Total entries: %d"), dwTotalEntries);
         }

         _tprintf(_T("\nEntries enumerated: %d\n"), dwTotalCount);
      }
   } else
      _ftprintf(stderr, _T("A system error has occurred: %d\n"), nStatus);
   //
   // Free the allocated buffer.
   //
   if (pBuf != NULL)
      NetApiBufferFree(pBuf);

   return 0;
}
