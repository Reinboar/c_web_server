#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "template_parser.h"
#include "string_util.h"
#include "static_handler.h"

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

char* insertTemplateValue(char* str, unsigned int strLen, unsigned int nameLen, int start, char* val) {
    if (!val)
        return NULL;
    char* resultStr = calloc(1, strLen - 4 - nameLen + strlen(val) + 1);
    strncat(resultStr, str, start);
    strcat(resultStr, val);
    strcat(resultStr, &(str[start+4+nameLen]));
    return resultStr;
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
                return renderTemplate(insertTemplateValue(src, srcLen, strlen(varName), i, varValue), ht);
            } else if (i < srcLen && src[i+1] == '#') {
                char* fileName = extractVarName(&(src[i+2]));
                if (!fileName)
                    return NULL;
                int fileSize = getFileSize(fileName);
                if (fileSize < 0)
                    return NULL;
                char* content = calloc(1, fileSize+1);
                readFileContent(fileName, content);
                return renderTemplate(insertTemplateValue(src, srcLen, strlen(fileName), i, content), ht);
            }
        }
    }
    return src;
}

bool templateHandlerFunc(HttpRequest* req, HttpResponse* resp, HashTable* ht) {
    char* renderStr = renderTemplate(resp->body, ht);
    if (!renderStr) {
        resp->statusCode = 500;
        resp->body = copyString("<h1>500 - Internal Server Error</h1>");
        return false;
    }
    resp->body = renderStr;
    return true;
}

ReqHandler* templateHandler() {
    return newReqHandler("template renderer", templateHandlerFunc);
}
