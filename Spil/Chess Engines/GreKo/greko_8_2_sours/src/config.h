//  GREKO Chess Engine
//  (c) 2002-2011 Vladimir Medvedev <vrm@bk.ru>
//  http://greko.110mb.com

//  config.cpp: configuration file code
//  modified: 1-Aug-2011

#ifndef CONFIG_H
#define CONFIG_H

#include "types.h"

class Config
{
public:

  Config() {}
  ~Config() {}

  const char* GetStr(const char* key, const char* defVal) const;
  double      GetDouble(const char* key, double defVal) const;
  int         GetInt(const char* key, int defVal) const;
  int*        GetIntArray(const char* key, int* array, int num) const;

  void Read();

private:

  enum { MAX_KEY = 256 };
  enum { MAX_VALUE = 256 };
  enum { MAX_LINES = 256 };

  struct Entry
  {
    char key[MAX_KEY + 1];
    char value[MAX_VALUE + 1];
  };

  Entry m_data[MAX_LINES];
  int   m_size;
};
////////////////////////////////////////////////////////////////////////////////

extern Config g_config;

#define PARAM(name) \
  name = g_config.GetInt(#name, name);

#define PARAM_PAIR(name) \
  g_config.GetIntArray(#name, name, 2); 

#endif

