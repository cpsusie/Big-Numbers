#pragma once

#include "MyString.h"

typedef struct {
  UINT brandIndex    : 8;
  UINT clflush       : 8;
  UINT numProcessors : 8;
  UINT localApicId   : 8;
} CPUBasics;

typedef enum {
  ORIGINALOEM    = 0
 ,INTELOVERDRIVE = 1
 ,DUAL           = 2
 ,INTELRESERVED  = 3
} ProcessorType;

typedef enum {
  FPU_FLAG
 ,VME_FLAG
 ,DE_FLAG
 ,PSE_FLAG
 ,TSC_FLAG
 ,MSR_FLAG
 ,PAE_FLAG
 ,MCE_FLAG
 ,CX8_FLAG
 ,APIC_FLAG
 ,SEP_FLAG
 ,MTRR_FLAG
 ,PGE_FLAG
 ,MCA_FLAG
 ,CMOV_FLAG
 ,PAT_FLAG
 ,PSE_36_FLAG
 ,PSN_FLAG
 ,CLFLSH_FLAG
 ,DS_FLAG
 ,ACPI_FLAG
 ,MMX_FLAG
 ,FXSR_FLAG
 ,SSE_FLAG
 ,SSE2_FLAG
 ,SS_FLAG
 ,HTT_FLAG
 ,TM_FLAG
 ,PBE_FLAG,
} FeatureFlag;

class FeatureFlagText {
public:
  FeatureFlag m_flag;
  const char *m_name;
  const char *m_desc;
};

typedef struct {
  bool FPU       : 1;
  bool VME       : 1;
  bool DE        : 1;
  bool PSE       : 1;
  bool TSC       : 1;
  bool MSR       : 1;
  bool PAE       : 1;
  bool MCE       : 1;
  bool CX8       : 1;
  bool APIC      : 1;
  bool Reserved1 : 1;
  bool SEP       : 1;
  bool MTRR      : 1;
  bool PGE       : 1;
  bool MCA       : 1;
  bool CMOV      : 1;
  bool PAT       : 1;
  bool PSE_36    : 1;
  bool PSN       : 1;
  bool CLFLSH    : 1;
  bool Reserved2 : 1;
  bool DS        : 1;
  bool ACPI      : 1;
  bool MMX       : 1;
  bool FXSR      : 1;
  bool SSE       : 1;
  bool SSE2      : 1;
  bool SS        : 1;
  bool HTT       : 1;
  bool TM        : 1;
  bool Reserved3 : 1;
  bool PBE       : 1;

  String getFlagText(FeatureFlag flag, bool longDescription) const;
  String toString(bool longDescription = false) const;
  bool isSet(FeatureFlag flag) const;
  static const FeatureFlagText featureFlagText[];
} FeatureInfo; 

typedef struct {
  UINT          steppingId     : 4;
  UINT          model          : 4;
  UINT          family         : 4;
  ProcessorType processorType  : 2;
  UINT          reserved1      : 2;
  UINT          extendedModel  : 4;
  UINT          extendedFamily : 8;
  UINT          reserverd2     : 4;
} VersionInfo;

class CPUInfo {
public:
  String      m_vendor;
  VersionInfo m_versionInfo;
  CPUBasics   m_CPUBasics;
  FeatureInfo m_featureInfo;
  String      m_processorBrandString;
  UINT64      m_processorSerialNumber;
  String      m_processorType;

  static bool CPUSupportsId();
  CPUInfo();
};

int getProcessorCount();
