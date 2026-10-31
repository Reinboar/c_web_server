#include <stdlib.h>
#include <stdio.h>

#include "http_prot.h"

HttpRequest* createHttpReq() {
    HttpRequest* req = malloc(sizeof(HttpRequest));
    return req;
}

int destroyHttpReq(HttpRequest* req) {
    if (req == NULL)
        return -1;
    if (req->method != NULL)
        free(req->method);
    if (req->url != NULL)
        free(req->url);
    if (req->body != NULL)
        free(req->body);
    if (req->httpVersion != NULL)
        free(req->httpVersion);
    if (req->headers.host != NULL)
        free(req->headers.host);
    free(req);
    return 0;
}

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

int getMethod(HttpRequest* req, char** buffer) {
    char* result;
    int len = sliceStr(*buffer, ' ', &result);
    if (len <= 0)
        return -1;
    req->method = result;
    *buffer += len + 1;
    return len;
}

int getUrl(HttpRequest* req, char** buffer) {
    char* result;
    int len = sliceStr(*buffer, ' ', &result);
    if (len <= 0)
        return -1;
    req->url = result;
    *buffer += len + 1;
    return len;
}

int getVersion(HttpRequest* req, char** buffer) {
    char* result;
    int len = sliceStrNewline(*buffer, &result);
    if (len <= 0)
        return -1;
    req->httpVersion = result;
    *buffer += len + 2;
    return len;
}

int getHeaders(HttpRequest* req, char** buffer) {
    char* headersStart = *buffer;
    char* result;
    while (**buffer != '\0') {
        int len = sliceStr(*buffer, ':', &result) + 1; // Get name of Http header
        *buffer += len; // skip length of header name and the trailing colon
        if (strcmp(result, "Host") == 0) {
            free(result);
            int spaceLen = skipStrWhile(*buffer, ' '); // skip spaces following colon of current header
            *buffer += spaceLen;
            int valLen = sliceStrNewline(*buffer, &result) + 2;
            req->headers.host = result;
            *buffer += valLen;
        } else {
            free(result);
            int headerLen = sliceStrNewline(*buffer, &result) + 2;
            *buffer += headerLen;
        }

        if ((*buffer)[0] == '\r' && (*buffer)[1] == '\n') {
            *buffer += 2;
            break;
        }
    }

    int totalLen = (*buffer) - headersStart;
    return totalLen;
}

int getBody(HttpRequest* req, char** buffer) {
    int bodyLen = strlen(*buffer);
    req->body = malloc(bodyLen);
    strcpy(req->body, *buffer);
    return bodyLen;
}

HttpRequest* readHttpReq(char** buffer) {
    //printf("BUFFER: %s\n", *buffer);
    HttpRequest* req = createHttpReq();
    getMethod(req, buffer);
    printf("Method: %s\n", req->method);
    getUrl(req, buffer);
    printf("URL: %s\n", req->url);
    getVersion(req, buffer);
    printf("Version: %s\n", req->httpVersion);
    getHeaders(req, buffer);
    printf("Host: %s\n", req->headers.host);
    getBody(req, buffer);
    printf("Body: %s\n", req->body);
    printf("\n");
    return req;
}
