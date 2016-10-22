//  GREKO Chess Engine
//  (c) 2002-2011 Vladimir Medvedev <vrm@bk.ru>
//  http://greko.110mb.com

//  config.cpp: configuration file code
//  modified: 1-Aug-2011

#include "config.h"

Config g_config;

double Config::GetDouble(const char* key, double defVal) const
{
  for (int i = 0; i < m_size; ++i)
  {
    if (!strcmp(key, m_data[i].key))
      return atof(m_data[i].value);
  }
  return defVal;
}
////////////////////////////////////////////////////////////////////////////////

int Config::GetInt(const char* key, int defVal) const
{
  for (int i = 0; i < m_size; ++i)
  {
    if (!strcmp(key, m_data[i].key))
      return atoi(m_data[i].value);
  }
  return defVal;
}
////////////////////////////////////////////////////////////////////////////////

int* Config::GetIntArray(const char* key, int* array, int num) const
{
  for (int i = 0; i < m_size; ++i)
  {
    if (!strcmp(key, m_data[i].key))
    {
      char buf[MAX_VALUE + 1];
      strcpy(buf, m_data[i].value);
      char* tk = strtok(buf, " ");
      for (int j = 0; j < num; ++j)
      {
        if (!tk) break;
        array[j] = atoi(tk);
        tk = strtok(NULL, " ");
      }
    }
  }
  return array;
}
////////////////////////////////////////////////////////////////////////////////

const char* Config::GetStr(const char* key, const char* defVal) const
{
  for (int i = 0; i < m_size; ++i)
  {
    if (!strcmp(key, m_data[i].key))
      return m_data[i].value;
  }
  return defVal;
}
////////////////////////////////////////////////////////////////////////////////

void Config::Read()
{
  m_size = 0;

  FILE* src = fopen("GreKo.ini", "rt");
  if (!src) return;

  char buf[MAX_KEY + MAX_VALUE + 1];
  while (fgets(buf, sizeof(buf), src))
  {
    if (buf[strlen(buf) - 1] == '\n')
      buf[strlen(buf) - 1] = 0;

    char* key = strtok(buf, " ");
    if (!key) continue;
    strncpy(m_data[m_size].key, key, MAX_KEY);

    strcpy(m_data[m_size].value, "");
    char* value = strtok(NULL, " ");
    if (!value) continue;
    strncpy(m_data[m_size].value, value, MAX_VALUE);

    ++m_size;
  }
  fclose(src);
}
////////////////////////////////////////////////////////////////////////////////

