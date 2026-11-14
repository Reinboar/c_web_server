#ifndef STRING_UTIL_H
#define STRING_UTIL_H

int sliceStr(char* str, char delim, char** result);
int sliceStrNewline(char* str, char** result);
int skipStrWhile(char* str, char skip);
char* copyString(const char* src);

#endif