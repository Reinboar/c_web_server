#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "template_parser.h"
#include "string_util.h"

char* extractVarName(char* src) {
    unsigned int srcLen = strlen(src);
    for (int i = 0; i < srcLen; i++) {
        char c = src[i];
        if (c == '}') {
            if (i < srcLen && src[i+1] == '}') {
                char* result = calloc(1, i + 1);
                memcpy(result, src, i);
                return result;
            }
        }
    }
    return NULL;
}

char* renderTemplate(char* src, HashTable* ht) {
    unsigned int srcLen = strlen(src);
    for (int i = 0; i < srcLen; i++) {
        char c = src[i];
        if (c == '{') {
            if (i < srcLen && src[i+1] == '{') {
                char* varName = extractVarName(&(src[i+2]));
                if (!varName)
                    return NULL;
                char* varValue = getElement(ht, varName);
                if (!varValue)
                    return NULL;
                char* resultStr = calloc(1, srcLen - 4 - strlen(varName) + strlen(varValue) + 1);
                strncat(resultStr, src, i);
                strcat(resultStr, varValue);
                strcat(resultStr, &(src[i+4+strlen(varName)]));
                char* actualStr = renderTemplate(resultStr, ht);
                return actualStr;
            }
        }
    }
    return src;
}

bool templateHandlerFunc(HttpRequest* req, HttpResponse* resp, HashTable* ht) {
    char* renderStr = renderTemplate(resp->body, ht);
    free(resp->body);
    resp->body = renderStr;
    return true;
}

ReqHandler* templateHandler() {
    return newReqHandler("template renderer", templateHandlerFunc);
}
