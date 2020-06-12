#include "stdafx.h"

#include <stdio.h>
#include <tchar.h>
#include <winsock2.h>
#include <iphlpapi.h>

#pragma comment(lib, "IPHLPAPI.lib")

void listAdaptersInfo(PIP_ADAPTER_INFO pAdapterInfo) {
  PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
  while (pAdapter) {
    _tprintf(_T("\tComboIndex: \t%d\n"  ), pAdapter->ComboIndex);
    _tprintf(_T("\tAdapter Name: \t%s\n"), pAdapter->AdapterName);
    _tprintf(_T("\tAdapter Desc: \t%s\n"), pAdapter->Description);
    _tprintf(_T("\tAdapter Addr: \t"));
    unsigned int i;
    for (i = 0; i < pAdapter->AddressLength; i++) {
      if (i == (pAdapter->AddressLength - 1))
        _tprintf(_T("%.2X\n"), (int) pAdapter->Address[i]);
      else
        _tprintf(_T("%.2X-"), (int) pAdapter->Address[i]);
    }
    _tprintf(_T("\tIndex: \t%d\n"), pAdapter->Index);
    _tprintf(_T("\tType: \t"));
    switch (pAdapter->Type) {
    case MIB_IF_TYPE_OTHER:
        _tprintf(_T("Other\n"));
        break;
    case MIB_IF_TYPE_ETHERNET:
        _tprintf(_T("Ethernet\n"));
        break;
    case MIB_IF_TYPE_TOKENRING:
        _tprintf(_T("Token Ring\n"));
        break;
    case MIB_IF_TYPE_FDDI:
        _tprintf(_T("FDDI\n"));
        break;
    case MIB_IF_TYPE_PPP:
        _tprintf(_T("PPP\n"));
        break;
    case MIB_IF_TYPE_LOOPBACK:
        _tprintf(_T("Lookback\n"));
        break;
    case MIB_IF_TYPE_SLIP:
        _tprintf(_T("Slip\n"));
        break;
    default:
        _tprintf(_T("Unknown type %ld\n"), pAdapter->Type);
        break;
    }

    _tprintf(_T("\tIP Address: \t%s\n"), pAdapter->IpAddressList.IpAddress.String);
    _tprintf(_T("\tIP Mask: \t%s\n"   ), pAdapter->IpAddressList.IpMask.String);
    _tprintf(_T("\tGateway: \t%s\n"   ), pAdapter->GatewayList.IpAddress.String);
    _tprintf(_T("\t***\n"));

    if (pAdapter->DhcpEnabled) {
        _tprintf(_T("\tDHCP Enabled: Yes\n"));
        _tprintf(_T("\t  DHCP Server: \t%s\n"), pAdapter->DhcpServer.IpAddress.String);

#if defined(__GETLOCALTIME__)
        _tprintf("\t  Lease Obtained: ");
        /* Display local time */
        error = _localtime32_s(&newtime, (__time32_t*) &pAdapter->LeaseObtained);
        if (error)
            _tprintf("Invalid Argument to _localtime32_s\n");
        else {
            // Convert to an ASCII representation
            error = asctime_s(buffer, 32, &newtime);
            if (error)
                _tprintf("Invalid Argument to asctime_s\n");
            else
                /* asctime_s returns the string terminated by \n\0 */
                _tprintf("%s", buffer);
        }

        _tprintf("\t  Lease Expires:  ");
        error = _localtime32_s(&newtime, (__time32_t*) &pAdapter->LeaseExpires);
        if (error)
            _tprintf("Invalid Argument to _localtime32_s\n");
        else {
            // Convert to an ASCII representation
            error = asctime_s(buffer, 32, &newtime);
            if (error)
                _tprintf("Invalid Argument to asctime_s\n");
            else
                /* asctime_s returns the string terminated by \n\0 */
                _tprintf("%s", buffer);
        }
#endif
    } else
        _tprintf(_T("\tDHCP Enabled: No\n"));

    if (pAdapter->HaveWins) {
        _tprintf(_T("\tHave Wins: Yes\n"));
        _tprintf(_T("\t  Primary Wins Server:    %s\n"),
               pAdapter->PrimaryWinsServer.IpAddress.String);
        _tprintf(_T("\t  Secondary Wins Server:  %s\n"),
               pAdapter->SecondaryWinsServer.IpAddress.String);
    } else
        _tprintf(_T("\tHave Wins: No\n"));
    pAdapter = pAdapter->Next;
    _tprintf(_T("\n"));
  }
}

void getAdaptersInfo() {
  ULONG entryCount = 100;
  ULONG bufferSize = entryCount * sizeof(IP_ADAPTER_INFO);

  PIP_ADAPTER_INFO pAdapterInfo = new IP_ADAPTER_INFO[entryCount];
  ULONG ret = GetAdaptersInfo(pAdapterInfo, &bufferSize);
  _tprintf(_T("return from GetAdaptersInfo:%lu\n"), ret);
  _tprintf(_T("bufferSize:%d\n"), bufferSize);

  listAdaptersInfo(pAdapterInfo);

  delete[] pAdapterInfo;
}

void listAdapterAddresses(PIP_ADAPTER_ADDRESSES pAddresses) {
  PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
  PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
  PIP_ADAPTER_ANYCAST_ADDRESS pAnycast = NULL;
  PIP_ADAPTER_MULTICAST_ADDRESS pMulticast = NULL;
  IP_ADAPTER_DNS_SERVER_ADDRESS *pDnServer = NULL;
  IP_ADAPTER_PREFIX *pPrefix = NULL;


  pCurrAddresses = pAddresses;
  while (pCurrAddresses) {
    _tprintf(_T("\tLength of the IP_ADAPTER_ADDRESS struct: %ld\n"),pCurrAddresses->Length);
    _tprintf(_T("\tIfIndex (IPv4 interface): %u\n"), pCurrAddresses->IfIndex);
    _tprintf(_T("\tAdapter name: %s\n"), pCurrAddresses->AdapterName);
    int i;

    pUnicast = pCurrAddresses->FirstUnicastAddress;
    if (pUnicast != NULL) {
      for (i = 0; pUnicast != NULL; i++)
        pUnicast = pUnicast->Next;
      _tprintf(_T("\tNumber of Unicast Addresses: %d\n"), i);
    } else
      _tprintf(_T("\tNo Unicast Addresses\n"));

    pAnycast = pCurrAddresses->FirstAnycastAddress;
    if (pAnycast) {
      for (i = 0; pAnycast != NULL; i++)
        pAnycast = pAnycast->Next;
      _tprintf(_T("\tNumber of Anycast Addresses: %d\n"), i);
    } else
      _tprintf(_T("\tNo Anycast Addresses\n"));

    pMulticast = pCurrAddresses->FirstMulticastAddress;
    if (pMulticast) {
      for (i = 0; pMulticast != NULL; i++)
        pMulticast = pMulticast->Next;
      _tprintf(_T("\tNumber of Multicast Addresses: %d\n"), i);
    } else
      _tprintf(_T("\tNo Multicast Addresses\n"));

    pDnServer = pCurrAddresses->FirstDnsServerAddress;
    if (pDnServer) {
      for (i = 0; pDnServer != NULL; i++)
        pDnServer = pDnServer->Next;
      _tprintf(_T("\tNumber of DNS Server Addresses: %d\n"), i);
    } else
      _tprintf(_T("\tNo DNS Server Addresses\n"));

    _tprintf(_T("\tDNS Suffix: %wS\n"   ), pCurrAddresses->DnsSuffix);
    _tprintf(_T("\tDescription: %wS\n"  ), pCurrAddresses->Description);
    _tprintf(_T("\tFriendly name: %wS\n"), pCurrAddresses->FriendlyName);

    if (pCurrAddresses->PhysicalAddressLength != 0) {
      _tprintf(_T("\tPhysical address: "));
      for (i = 0; i < (int) pCurrAddresses->PhysicalAddressLength; i++) {
         if (i == (int)(pCurrAddresses->PhysicalAddressLength - 1))
           _tprintf(_T("%.2X\n"), (int) pCurrAddresses->PhysicalAddress[i]);
         else
           _tprintf(_T("%.2X-"), (int) pCurrAddresses->PhysicalAddress[i]);
      }
    }
    _tprintf(_T("\tFlags: %ld\n"), pCurrAddresses->Flags);
    _tprintf(_T("\tMtu: %lu\n"), pCurrAddresses->Mtu);
    _tprintf(_T("\tIfType: %ld\n"), pCurrAddresses->IfType);
    _tprintf(_T("\tOperStatus: %ld\n"), pCurrAddresses->OperStatus);
    _tprintf(_T("\tIpv6IfIndex (IPv6 interface): %u\n"),pCurrAddresses->Ipv6IfIndex);
    _tprintf(_T("\tZoneIndices (hex): "));
    for (i = 0; i < 16; i++)
       _tprintf(_T("%lx "), pCurrAddresses->ZoneIndices[i]);
    _tprintf(_T("\n"));

    _tprintf(_T("\tTransmit link speed: %I64u\n"), pCurrAddresses->TransmitLinkSpeed);
    _tprintf(_T("\tReceive link speed: %I64u\n"), pCurrAddresses->ReceiveLinkSpeed);

    pPrefix = pCurrAddresses->FirstPrefix;
    if (pPrefix) {
      for (i = 0; pPrefix != NULL; i++)
        pPrefix = pPrefix->Next;
      _tprintf(_T("\tNumber of IP Adapter Prefix entries: %d\n"), i);
    } else
      _tprintf(_T("\tNumber of IP Adapter Prefix entries: 0\n"));

    _tprintf(_T("\n"));

    pCurrAddresses = pCurrAddresses->Next;
  }
}

void getAdapterAddresses() {
  ULONG entryCount = 100;
  ULONG bufferSize = entryCount * sizeof(IP_ADAPTER_ADDRESSES);
  PIP_ADAPTER_ADDRESSES pAddresses = new IP_ADAPTER_ADDRESSES[entryCount];

  ULONG ret = GetAdaptersAddresses( AF_UNSPEC,0,NULL,pAddresses,&bufferSize);
  _tprintf(_T("return from GetAdaptersAdresses:%lu\n"), ret);
  _tprintf(_T("bufferSize:%d\n"), bufferSize);

  listAdapterAddresses(pAddresses);

  delete[] pAddresses;
}

int _tmain(int argc, TCHAR **argv) {

//  getAdapterAddresses();

  getAdaptersInfo();

  return 0;
}
