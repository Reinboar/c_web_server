#include <stdlib.h>
#include <stdio.h>

#include "http_prot.h"

#define RAW_RESPONSE_MAX_SIZE 4*1024*1024 // 4 Megabytes

HttpResponse* newHttpResp() {
    HttpResponse* resp = calloc(1, sizeof(HttpResponse));
    return resp;
}

bool destroyHttpResp(HttpResponse* resp) {
    if (resp == NULL)
        return false;
    if (resp->body != NULL)
        free(resp->body);
    free(resp);
    return true;
}

const char* statusCodeToMsg(int code) {
    switch(code) {
        case 200:
        return "OK";
        case 400:
        return "Bad Request";
        case 401:
        return "Unauthorized";
        case 403:
        return "Forbidden";
        case 404:
        return "Not Found";
        case 405:
        return "Method Not Allowed";
        case 500:
        return "Internal Server Error";
        default:
        return "Unknown Status Code";
    }
}

char* compileHttpResp(HttpResponse* resp) {
    char* rawResp = calloc(1, RAW_RESPONSE_MAX_SIZE+1);
    size_t bodySize = strlen(resp->body);

    // Write HTTP response as a raw string, only include WWW-Authenticate header if it is provided
    if (resp->headers.wwwAuthenticate == NULL) {
        snprintf(rawResp, RAW_RESPONSE_MAX_SIZE, "HTTP/1.1 %d %s\r\nContent-Type: %s\r\nContent-Length: %d\r\nConnection: close\r\n\r\n%s",\
            resp->statusCode, statusCodeToMsg(resp->statusCode), resp->headers.contentType, bodySize, resp->body);
    } else {
        snprintf(rawResp, RAW_RESPONSE_MAX_SIZE, "HTTP/1.1 %d %s\r\nContent-Type: %s\r\nContent-Length: %d\r\nConnection: close\r\nWWW-Authenticate: %s\r\n\r\n%s",\
            resp->statusCode, statusCodeToMsg(resp->statusCode), resp->headers.contentType, bodySize, resp->headers.wwwAuthenticate, resp->body);
    }

    // ensure the raw response is properly terminated
    rawResp[RAW_RESPONSE_MAX_SIZE] = 0;
    return rawResp;
}

HttpRequest* newHttpReq() {
    HttpRequest* req = malloc(sizeof(HttpRequest));
    return req;
}

bool destroyHttpReq(HttpRequest* req) {
    if (req == NULL)
        return false;
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
    if (req->headers.authorization != NULL)
        free(req->headers.authorization);
    free(req);
    return true;
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
        printf("Header: %s\n", result);
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
    HttpRequest* req = newHttpReq();
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
