#pragma once

#include "MyString.h"
#include "StreamParameters.h"

String toString(char             ch,                           StreamSize width = 0, FormatFlags flags = 0);
String toString(SHORT            n , StreamSize precision = 0, StreamSize width = 0, FormatFlags flags = 0);
String toString(USHORT           n , StreamSize precision = 0, StreamSize width = 0, FormatFlags flags = 0);
String toString(INT              n , StreamSize precision = 0, StreamSize width = 0, FormatFlags flags = 0);
String toString(UINT             n , StreamSize precision = 0, StreamSize width = 0, FormatFlags flags = 0);
String toString(LONG             n , StreamSize precision = 0, StreamSize width = 0, FormatFlags flags = 0);
String toString(ULONG            n , StreamSize precision = 0, StreamSize width = 0, FormatFlags flags = 0);
String toString(INT64            n , StreamSize precision = 0, StreamSize width = 0, FormatFlags flags = 0);
String toString(UINT64           n , StreamSize precision = 0, StreamSize width = 0, FormatFlags flags = 0);
String toString(float            x , StreamSize precision = 6, StreamSize width = 0, FormatFlags flags = 0);
String toString(double           x , StreamSize precision = 6, StreamSize width = 0, FormatFlags flags = 0);
