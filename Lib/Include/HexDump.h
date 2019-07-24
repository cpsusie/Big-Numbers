#pragma once

String hexdumpString(const void *data, size_t size);
// if f == NULL, use debugLog as output
void   hexdump(const void *data, size_t size, FILE *f = NULL);
