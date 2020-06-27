// getcpuid.cpp : Defines the entry point for the console application.
//

#include "pch.h"
#include <CPUInfo.h>

static const char *processorTypeStr[] = {
  "Original OEM processor"
 ,"Intel(R) OverDrive(R) processor"
 ,"Dual processor"
 ,"Intel(R) reserved"
};

#define FEATURE(name,description) name##_FLAG,#name,description

const FeatureFlagText FeatureInfo::featureFlagText[] = {
  FEATURE( FPU       ,"Floating Point Unit On-Chip. The processor contains an x87 FPU"),
  FEATURE( VME       ,"Virtual 8086 Mode Enhancements. Virtual 8086 mode enhancements, including CR4.VME for controlling the feature, CR4.PVI for protected mode virtual interrupts, software interrupt indirection, expansion of the TSS with the software indirection bitmap, and EFLAGS.VIF and EFLAGS.VIP flags"),
  FEATURE( DE        ,"Debugging Extensions. Support for I/O breakpoints, including CR4.DE for controlling the feature, and optional trapping of accesses to DR4 and DR5"),
  FEATURE( PSE       ,"Page Size Extension. Large pages of size 4Mbyte are supported, including CR4.PSE for controlling the feature, the defined dirty bit in PDE (Page Directory Entries), optional reserved bit trapping in CR3, PDEs, and PTEs"),
  FEATURE( TSC       ,"Time Stamp Counter. The RDTSC instruction is supported, including CR4.TSD for controlling privilege"),
  FEATURE( MSR       ,"Model Specific Registers RDMSR and WRMSR Instructions. The RDMSR and WRMSR instructions are supported. Some of the MSRs are implementation dependent"),
  FEATURE( PAE       ,"Physical Address Extension. Physical addresses greater than 32 bits are supported: extended page table entry formats, an extra level in the page translation tables is defined, 2 Mbyte pages are supported instead of 4 Mbyte pages if PAE bit is 1. The actual number of address bits beyond 32 is not defined, and is implementation specific"),
  FEATURE( MCE       ,"Machine Check Exception. Exception 18 is defined for Machine Checks, including CR4.MCE for controlling the feature. This feature does not define the model-specific implementations of machine-check error logging, reporting, and processor shutdowns. Machine Check exception handlers may have to depend on processor version to do model specific processing of the exception, or test for the presence of the Machine Check feature"),
  FEATURE( CX8       ,"CMPXCHG8B Instruction. The compare-and-exchange 8 bytes (64 bits) instruction is supported (implicitly locked and atomic)"),
  FEATURE( APIC      ,"APIC On-Chip. The processor contains an Advanced Programmable Interrupt Controller (APIC), responding to memory mapped commands in the physical address range FFFE0000H to FFFE0FFFH (by default - some processors permit the APIC to be relocated)"),
  FEATURE( SEP       ,"SYSENTER and SYSEXIT Instructions. The SYSENTER and SYSEXIT and associated MSRs are supported"),
  FEATURE( MTRR      ,"Memory Type Range Registers. MTRRs are supported. The MTRRcap MSR contains feature bits that describe what memory types are supported, how many variable MTRRs are supported, and whether fixed MTRRs are supported"),
  FEATURE( PGE       ,"PTE Global Bit. The global bit in page directory entries (PDEs) and page table entries (PTEs) is supported, indicating TLB entries that are common to different processes and need not be flushed. The CR4.PGE bit controls this feature"),
  FEATURE( MCA       ,"Machine Check Architecture. The Machine Check Architecture, which provides a compatible mechanism for error reporting in P6 family, Pentium� 4, and Intel� Xeon� processors is supported. The MCG_CAP MSR contains feature bits describing how many banks of error reporting MSRs are supported"),
  FEATURE( CMOV      ,"Conditional Move Instructions. The conditional move instruction CMOV is supported. In addition, if x87 FPU is present as indicated by the CPUID.FPU feature bit, then the FCOMI and FCMOV instructions are supported"),
  FEATURE( PAT       ,"Page Attribute Table. Page Attribute Table is supported. This feature augments the Memory Type Range Registers (MTRRs), allowing an operating system to specify attributes of memory on a 4K granularity through a linear address"),
  FEATURE( PSE_36    ,"32-Bit Page Size Extension. Extended 4-MByte pages that are capable of addressing "
                      "physical memory beyond 4 GBytes are supported. This feature indicates that the upper "
                      "four bits of the physical address of the 4-MByte page is encoded by bits 13-16 "
                      "of the page directory entry"),
  FEATURE( PSN       ,"Processor Serial Number. The processor supports the 96-bit processor identification number feature and the feature is enabled"),
  FEATURE( CLFLSH    ,"CLFLUSH Instruction. CLFLUSH Instruction is supported"),
  FEATURE( DS        ,"Debug Store. The processor supports the ability to write debug information into a "
                      "memory resident buffer. This feature is used by the branch trace store (BTS) "
                      "and precise event-based sampling (PEBS) facilities (see Chapter 15, Debugging and "
                      "Performance Monitoring, in the IA-32 Intel Architecture Software Developer's Manual, Volume 3)"),
  FEATURE( ACPI      ,"Thermal Monitor and Software Controlled Clock Facilities. The processor implements "
                      "internal MSRs that allow processor temperature to be monitored and processor "
                      "performance to be modulated in predefined duty cycles under software control"),
  FEATURE( MMX       ,"Intel MMX Technology. The processor supports the Intel MMX technology"),
  FEATURE( FXSR      ,"FXSAVE and FXRSTOR Instructions. The FXSAVE and FXRSTOR instructions are supported "
                      "for fast save and restore of the floating point context. "
                      "Presence of this bit also indicates that CR4.OSFXSR is available for an "
                      "operating system to indicate that it supports the FXSAVE and FXRSTOR instructions"),
  FEATURE( SSE       ,"SSE. The processor supports the SSE extensions"),
  FEATURE( SSE2      ,"SSE2. The processor supports the SSE2 extensions"),
  FEATURE( SS        ,"Self Snoop. The processor supports the management of conflicting memory types by "
                      "performing a snoop of its own cache structure for transactions issued to the bus"),
  FEATURE( HTT       ,"Hyper-Threading Technology. The processor implements Hyper-Threading technology"),
  FEATURE( TM        ,"Thermal Monitor. The processor implements the thermal monitor automatic thermal control circuitry (TCC)"),
  FEATURE( PBE       ,"Pending Break Enable. The processor supports the use of the FERR#/PBE# pin when "
                      "the processor is in the stop-clock state (STPCLK# is asserted) to signal the processor "
                      "that an interrupt is pending and that the processor should return to normal operation "
                      "to handle the interrupt. Bit 10 (PBE enable) in the IA32_MISC_ENABLE MSR enables this "
                      "capability")
};

bool FeatureInfo::isSet(FeatureFlag flag) const {
#define casebit(bit) case bit##_FLAG: return bit;
  switch(flag) {
  casebit( FPU    );
  casebit( VME    );
  casebit( DE     );
  casebit( PSE    );
  casebit( TSC    );
  casebit( MSR    );
  casebit( PAE    );
  casebit( MCE    );
  casebit( CX8    );
  casebit( APIC   );
  casebit( SEP    );
  casebit( MTRR   );
  casebit( PGE    );
  casebit( MCA    );
  casebit( CMOV   );
  casebit( PAT    );
  casebit( PSE_36 );
  casebit( PSN    );
  casebit( CLFLSH );
  casebit( DS     );
  casebit( ACPI   );
  casebit( MMX    );
  casebit( FXSR   );
  casebit( SSE    );
  casebit( SSE2   );
  casebit( SS     );
  casebit( HTT    );
  casebit( TM     );
  casebit( PBE    );
  default: return false;
  }
}

String FeatureInfo::getFlagText(FeatureFlag flag, bool longDescription) const {
  for(int i = 0; i < ARRAYSIZE(featureFlagText); i++) {
    if(featureFlagText[i].m_flag == flag) {
      if(longDescription)
        return String(featureFlagText[flag].m_name) + _T("\t:") + featureFlagText[flag].m_desc;
      else
        return featureFlagText[flag].m_name;
    }
  }
  return EMPTYSTRING;
}

String FeatureInfo::toString(bool longDescription) const {
  String result;
  for(FeatureFlag flag = FPU_FLAG; flag <= PBE_FLAG; flag = (FeatureFlag)((int)flag + 1)) {
    if(isSet(flag)) {
      if(longDescription) {
        result += _T("\n");
      } else if(result.length() > 0) {
        result += _T(", ");
      }
      result += getFlagText(flag,longDescription);
    }
  }
  return result;
}

#if defined(IS32BIT)

static char *getProcessorBrandString(char *dst) {
  UINT eax_return;
  __asm {
    mov eax,80000000H
    cpuid
    mov eax_return,eax
  }

  if((eax_return & 0x80000000) == 0) {
    strcpy(dst,"");
    return dst;
  }

  char tmp[49];
  memset(tmp,0,sizeof(tmp));
  if(eax_return >= 0x80000004) {
    __asm {
      mov eax,80000002H
      cpuid
      mov dword ptr tmp     ,eax
      mov dword ptr [tmp+4] ,ebx
      mov dword ptr [tmp+8] ,ecx
      mov dword ptr [tmp+12],edx

      mov eax,80000003H
      cpuid
      mov dword ptr [tmp+16],eax
      mov dword ptr [tmp+20],ebx
      mov dword ptr [tmp+24],ecx
      mov dword ptr [tmp+28],edx

      mov eax,80000004H
      cpuid
      mov dword ptr [tmp+32],eax
      mov dword ptr [tmp+36],ebx
      mov dword ptr [tmp+40],ecx
      mov dword ptr [tmp+44],edx
    }
    strcpy(dst,tmp);
  }
  return dst;
}

static void getCPUData(char *vendor, VersionInfo &versionInfo, CPUBasics &cpuBasics, FeatureInfo &featureInfo, char *brandString, unsigned __int64 &serialNumber) {
  char             tmp_vendor[13];
  VersionInfo      tmp_versioninfo;
  CPUBasics        tmp_CPUBasics;
  FeatureInfo      tmp_FeatureInfo;
  char             tmp_brandString[49];
  unsigned __int64 tmp_serialNumber;
  UINT             highestEAX;

  memset(tmp_vendor      ,0,sizeof(tmp_vendor));
  memset(&tmp_versioninfo,0,sizeof(tmp_versioninfo));
  memset(&tmp_FeatureInfo,0,sizeof(tmp_FeatureInfo));
  memset(tmp_brandString ,0,sizeof(tmp_brandString));

  __asm {
    mov eax,0
    cpuid
    mov highestEAX,eax;
    mov dword ptr tmp_vendor,ebx
    mov dword ptr [tmp_vendor+4],edx
    mov dword ptr [tmp_vendor+8],ecx
  }

  if(highestEAX >= 1) {
    __asm {
      mov eax,1
      cpuid
      mov tmp_versioninfo, eax
      mov tmp_CPUBasics, ebx
      mov tmp_FeatureInfo, edx
    }
  }

  if(highestEAX >= 3) {
    __asm {
      mov eax,3
      cpuid
      mov dword ptr tmp_serialNumber, ecx
      mov dword ptr [tmp_serialNumber+4], edx
    }
  }

  strcpy(vendor,tmp_vendor);
  versionInfo  = tmp_versioninfo;
  cpuBasics    = tmp_CPUBasics;
  featureInfo  = tmp_FeatureInfo;
  getProcessorBrandString(tmp_brandString);
  strcpy(brandString,tmp_brandString);
  serialNumber = tmp_serialNumber;
}

CPUInfo::CPUInfo() {
  char vendor[13];
  char brandString[49];
  getCPUData(vendor,m_versionInfo, m_CPUBasics, m_featureInfo, brandString, m_processorSerialNumber);
  m_vendor               = vendor;
  m_processorBrandString = brandString;
  m_processorType        = processorTypeStr[m_versionInfo.processorType];
}

bool CPUInfo::CPUSupportsId() { // static
  __asm {
    pushfd
    mov eax, dword ptr [esp]
    and eax, 0FFDFFFFFH
    mov dword ptr [esp], eax
    popfd
    pushfd
    mov eax, dword ptr [esp]
    or eax, 00200000H
    mov dword ptr [esp], eax
    popfd
  }

  return true;
}

#else

// this fucks up in x64 mode !!!!!!

extern "C" {
void   getCPUBrandString(char *dst);
int    getCPUData(char *vendor, UINT &versionInfo, UINT &cpuBasics, UINT &featureInfo);
UINT64 getCPUSerialNumber();
};

CPUInfo::CPUInfo() {
  throwException(_T("CPUInfo not implemented in x64-mode"));

  char vendor[20];
  memset(vendor,0,sizeof(vendor));
/*
  _tprintf(_T("vendor       :%p\n"), vendor);
  _tprintf(_T("this:%p\n"), this);
  _tprintf(_T("m_versionInfo:%p, size:%zd\n"), &m_versionInfo, sizeof(m_versionInfo));
  _tprintf(_T("m_CPUBasic   :%p, size:%zd\n"), &m_CPUBasics  , sizeof(m_CPUBasics  ));
  _tprintf(_T("m_featureInfo:%p, size:%zd\n"), &m_featureInfo, sizeof(m_featureInfo));
*/
  DEBUGTRACE;
  if(getCPUData(vendor, (UINT&)m_versionInfo, (UINT&)m_CPUBasics, (UINT&)m_featureInfo) >= 3) {
  DEBUGTRACE;
    m_processorSerialNumber = 7; // getCPUSerialNumber();
  DEBUGTRACE;
  } else {
    m_processorSerialNumber = 0;
  }
  printf("vendor(ascii):<%s>\n", vendor);
  char brandString[49];
  DEBUGTRACE;
  getCPUBrandString(brandString);
  DEBUGTRACE;
  printf("brand(ascii):<%s>\n", brandString);

  m_vendor               = vendor;
  DEBUGTRACE;
  m_processorBrandString = brandString;
  DEBUGTRACE;
  _tprintf(_T("processorType:%d\n"), m_versionInfo.processorType);
  m_processorType        = processorTypeStr[m_versionInfo.processorType];
  DEBUGTRACE;
}

bool CPUInfo::CPUSupportsId() { // static
  // TODO: missing code
  return true;
}

#endif
