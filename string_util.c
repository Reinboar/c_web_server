#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "string_util.h"

// Creates a new string copied from the given string until the 1-character delimiter is encountered
int sliceStr(char* str, char delim, char** result) {
    int len = 0;
    char* curPos = str;
    while (*curPos != delim) {
        if (*curPos == '\0')
            break;
        
        len++;
        curPos++;
    }

    *result = malloc(len+1);
    memcpy(*result, str, len);
    (*result)[len] = 0; // properly append zero terminator to new string
    return len;
}

// Same as sliceStr except the delimiter is always a newline, which is 2 characters in HTTP requests
int sliceStrNewline(char* str, char** result) {
    int len = 0;
    char* curPos = str;
    while (*curPos != '\r' && *curPos != '\n') {
        if (*curPos == '\0')
            break;
        
        len++;
        curPos++;
    }

    *result = malloc(len+1);
    memcpy(*result, str, len);
    (*result)[len] = 0; // properly append zero terminator to new string
    return len;
}

int skipStrWhile(char* str, char skip) {
    int len = 0;
    char* curPos = str;
    while (*curPos == skip) {
        if (*curPos == '\0')
            break;
        len++;
        curPos++;
    }

    return len;
}

// Allocates a new string and copies the source string into it
char* copyString(const char* srcStr) {
    char* newStr = malloc(strlen(srcStr) + 1);
    newStr = memcpy(newStr, srcStr, strlen(srcStr) + 1);
    return newStr;
}
