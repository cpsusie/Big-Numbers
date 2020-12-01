#pragma once

#include <Singleton.h>

typedef enum {
  CPP
 ,JAVA
} Language;

class MatrixOptimizeParameters {
public:
  static constexpr UINT defaultRecurseLevel = 2;
  bool m_enabled       : 1;
  UINT m_maxRecursion  : 4;
  UINT m_minBitSetSize : 8; // [2..255]
  bool m_pruneBitSet   : 1; // always false when compress action-matrix (or it's transposed)
                            // if true, SymbolNode::allocateNode will generate a Don't care node, which skips check on symbol- or stateset
                            // which is only allowed when handling check successor-matrix
  MatrixOptimizeParameters()
    : m_enabled(false)
    , m_maxRecursion(defaultRecurseLevel)
    , m_minBitSetSize(2)
    , m_pruneBitSet(false)
  {
  }
};

typedef enum {
  OPTPARAM_ACTION
 ,OPTPARAM_SHIFT
 ,OPTPARAM_REDUCE
 ,OPTPARAM_SUCC
} OptimizationParameters;

class Options : public Singleton {
private:
  Options();
  MatrixOptimizeParameters m_optParam[4];
public:
  String   m_templateName;
  String   m_implOutputDir;
  String   m_headerOutputDir;
  String   m_wizardName;
  String   m_first1File;
  String   m_nameSpace;
  BYTE     m_tabSize;
  BYTE     m_verboseLevel;
  bool     m_verbose                     : 1;
  Language m_language                    : 2;
  bool     m_lineDirectives              : 1;
  bool     m_generateBreaks              : 1;
  bool     m_generateActions             : 1;
  bool     m_generateLookahead           : 1;
  bool     m_generateNonTerminals        : 1;
  bool     m_skipIfEqual                 : 1;
  bool     m_callWizard                  : 1;
  bool     m_findOptimalTableCompression : 1;
  inline MatrixOptimizeParameters &getOptimizeParameters(OptimizationParameters type) {
    return m_optParam[type];
  }
  DEFINESINGLETON(Options)

  static constexpr UINT maxTabSize          = 16;
  static constexpr UINT defaultTabSize      = 4;
  static constexpr UINT maxVerboseLevel     = 2;
  static constexpr UINT defaultVerboseLevel = 1;
  static constexpr UINT maxRecursiveCalls   = 8;

  void checkTemplateExist(const String &defaultTemplateName);
  static inline const MatrixOptimizeParameters &getOptParam(OptimizationParameters type) {
    return getInstance().getOptimizeParameters(type);
  }
};

void verbose(int level, _In_z_ _Printf_format_string_ TCHAR const *const format, ...);
